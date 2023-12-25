# How To Build

1. 使用Qt Creator開啟CMakeLists.txt
2. 選擇適當的toolchain（建議Qt的版本是5.15.2）
3. 點擊左下角的執行

## CACHE Variable

下表是一些可供編輯的CACHE Variable：

|Variable         |Description        |
|---              |---                |
|QT_MAJOR_VERSION |Qt的主版本（預設為5）|
|CMAKE_INSTALL_PREFIX |安裝路徑|
|CMAKE_PREFIX_PATH |如果cmake沒辦法找到Qt package，可嘗試修改該變數，變數指定的目錄下要有`lib/cmake/Qt${QT_MAJOR_VERSION}/Qt${QT_MAJOR_VERSION}Config.cmake`。|

## Custom Target

下表是自定義的實用target，在build資料夾下打`make <Target>`來執行（或者是`cmake --build . -t <Target>`）：

|Target         |Description  |
|---            |---          |
|install_final  |安裝編譯好的可執行檔和必要的資源檔（shader、dll、模型）。如果是Windows平台，會一併執行`windeployqt`，以安裝Qt的dll。|
