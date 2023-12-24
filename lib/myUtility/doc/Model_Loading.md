# 模型載入踩雷心得

## Maya's paint vertex color tool

Maya有一個工具叫paint vertex color tool，它可以用來替頂點著色。

1. 在繪製的時候，繪製的方式要選擇「Vertex」，而不是「Vertex Face」或「Face」，不然匯出成模型檔時vertex color不會匯出。

2. 建議匯出成fbx檔，因為一般的obj檔不會儲存頂點的顏色。

## 匯入fbx檔

我原本是用 [learnopengl.com](https://learnopengl.com/Model-Loading/Model) 提供的方式匯入fbx檔。
但遇到了件奇怪的事，就是不管我在Maya怎麼調模型的大小、位置，匯入並顯示後永遠都長一樣；
可是當我用同樣的方式匯入obj檔時就正常了。

後來我看了 [assimp的文件](https://assimp-docs.readthedocs.io/en/v5.3.0/usage/use_the_lib.html#the-node-hierarchy)。
發現在它提供的偽代碼中都有處理每個node的transformation——也就是`aiNode::mTransformation`，它記錄了目前的node相對於parent node的矩陣轉換關係。

看起來，我需要知道從「根節點」到「目前節點」間所有節點的`aiNode::mTransformation`，才能知道怎麼將目前節點的點轉成世界座標。

因此我做了修改，在`Model::processNode`加入了transform的參數，來在遞迴的過程中記錄目前這個node相對於世界座標的矩陣轉換關係（也就是Model Matrix）。
然後就能正常匯入fbx檔了。

我猜測，obj檔是直接將每個點的世界座標存起來才不會遇到這問題；
而fbx檔則是分了很多層來儲存每個node和parent node的關係。

## color set

Maya有一個工具叫color set editor，它可以替每個mesh創建許多組的顏色，每一組顏色稱作一個color set。如果匯出為fbx時這些color set也會一併匯出。

在用assimp匯入頂點顏色時有幾個要注意的：
- `AI_MAX_NUMBER_OF_COLOR_SETS` - 這個巨集記錄了assimp最多能匯入幾個color set（目前它的值為8）
- [`aiMesh::HasVertexColors`](https://assimp-docs.readthedocs.io/en/v5.3.0/API/API-Documentation.html#_CPPv4NK6aiMesh15HasVertexColorsEj) - 用來檢查這個mesh是否包含特定的color set
- `aiMesh::mColors` - 它是一個2維陣列，第一個index代表color set的index，第二個index則是vertex的index。例如`mColors[0][1]`代表「index=1的vertex」在「index=0的color set」中的顏色。
