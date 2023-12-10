# Design Principles of Shader and VAO

## Shader

- 使用版本： `#version 430 core`

- Vertex Shader的attribute遵守下列順序

|Index|Type|Content|
|-----|----|-------|
|0    |vec3|頂點的座標|
|1    |vec2|Texture Coordinate|
|2    |vec3|Normal|

- Shader的Uniform Block必須遵守下列規範

|Binding|Type|Content|Description|
|---    |--- |---    |---        |
|0      |mat4|view_matrix|將世界座標轉成視空間座標的矩陣|
|       |mat4|proj_matrix|將視空間轉成clip coordinate的矩陣|

|Binding|Type|Content|Description|
|---    |--- |---    |---        |
|1      |vec4|eye_position|眼睛的位置，w忽略不看|
|       |vec4|light_position|光源的位置。w為0是方向光源，w為1是點光源。|


### Texture from obj files

從obj載入的texture可能不只一張， `Mesh::draw` 會將這些texture以下列順序綁定：

|Name                |Sampler      |
|--------------------|-------------|
|texture_diffuse1    |0            |
|texture_specular1   |1            |
|texture_diffuse2    |2            |
|texture_specular2   |3            |
|texture_diffuse3    |4            |
|texture_specular3   |5            |
|...                 |             |
|texture_diffuseN    |2 * (N-1)    |
|texture_specularN   |2 * (N-1) + 1|

## VAO

- 子類別在覆寫`VAO_Interface::draw`時，不該呼叫`glUseProgram`，使用的Shader要由呼叫者自行決定