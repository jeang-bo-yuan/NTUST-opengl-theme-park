# 自己寫的工具

add_library(my_utility STATIC
    ArcBall.cpp                 "include/ArcBall.h"
    Box_VAO.cpp                 "include/Box_VAO.h"
    DynamicHeightMap.cpp        "include/DynamicHeightMap.h"
    Mesh.cpp                    "include/Mesh.h"
                                "include/Plane_VAO.h"
    qtTextureCubeMap.cpp        "include/qtTextureCubeMap.h"
    qtTextureImage2D.cpp        "include/qtTextureImage2D.h"
    Shader.cpp                  "include/Shader.h"
    UBO.cpp                     "include/UBO.h"
                                "include/VAO_Interface.h"
    Wave_VAO.cpp                "include/Wave_VAO.h"
)

target_include_directories(my_utility PUBLIC include)
target_link_libraries(my_utility
  Qt${QT_MAJOR_VERSION}::Gui
  glm::glm
  glad_glad
)

