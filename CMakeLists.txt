cmake_minimum_required(VERSION 3.11 FATAL_ERROR)
project(theme_park LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(MSVC)
    add_compile_options(/utf-8)
endif()

set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)

# Load Library #################################################################################

# find Qt Library
set(QT_MAJOR_VERSION "5" CACHE STRING "Major version of QT")
find_package(Qt${QT_MAJOR_VERSION} CONFIG REQUIRED
    COMPONENTS Widgets Gui
)

# fetch GLM
include(FetchContent)
FetchContent_Declare(GLM
  URL "https://github.com/g-truc/glm/releases/download/0.9.9.8/glm-0.9.9.8.zip"
  DOWNLOAD_EXTRACT_TIMESTAMP ON
)

# fetch assimp
OPTION ( ASSIMP_BUILD_TESTS
  "If the test suite for Assimp is built in addition to the library."
  OFF
)
OPTION( ASSIMP_INSTALL
  "Disable this if you want to use assimp as a submodule."
  OFF
)
FetchContent_Declare(ASSIMP
  URL "https://github.com/assimp/assimp/archive/refs/tags/v5.3.1.zip"
  DOWNLOAD_EXTRACT_TIMESTAMP ON
)

message("Fetching GLM Library...")
FetchContent_MakeAvailable(GLM)
message("Fetching ASSIMP Library...")
FetchContent_MakeAvailable(ASSIMP)

# lib directory
add_subdirectory("lib/glad")
add_subdirectory("lib/myUtility")


# Executable ##################################################################################

file(GLOB_RECURSE SHADER_SOURCE RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" CONFIGURE_DEPENDS "src/shader/*.vert" "src/shader/*.frag" "src/shader/*.geom")

add_executable(theme_park
    src/ControlPoint_VAO.h src/ControlPoint_VAO.cpp
    src/Island.h src/Island.cpp
    src/main.cpp
    src/MainWindow.h src/MainWindow.cpp src/MainWindow.ui
    src/ParamEquation.h src/ParamEquation.cpp
    src/Particle.h src/Particle.cpp
    src/PostProcessor.h src/PostProcessor.cpp
    src/Skybox.h src/Skybox.cpp
    src/TrainSystem.h src/TrainSystem.cpp
    src/ViewWidget.h src/ViewWidget.cpp
    src/Water.h src/Water.cpp

    src/img/images.qrc
    ${SHADER_SOURCE}
)

target_link_libraries(theme_park
    Qt${QT_MAJOR_VERSION}::Widgets
    Qt${QT_MAJOR_VERSION}::Gui
    glm::glm
    assimp::assimp

    glad_glad
    my_utility
)



# Other Utility ###############################################################################

# copy shader into binary dir
add_custom_target(copy_shader
  COMMAND ${CMAKE_COMMAND} -E copy_directory "src/shader" ${CMAKE_CURRENT_BINARY_DIR}/shader
  COMMENT "copy shader/ into binary directory"
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
  VERBATIM
)
add_dependencies(theme_park copy_shader)

# copy asset into binary dir
add_custom_target(copy_asset
  COMMAND ${CMAKE_COMMAND} -E copy_directory "asset/" "${CMAKE_CURRENT_BINARY_DIR}/asset"
  COMMENT "copy asset/ into binary directory"
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
  VERBATIM
)
add_dependencies(theme_park copy_asset)



# Install #####################################################################################

install(TARGETS theme_park assimp
        RUNTIME DESTINATION bin COMPONENT Final
)

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/src/shader/
        DESTINATION bin/shader
        COMPONENT Final
)

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/asset/
        DESTINATION bin/asset
        COMPONENT Final
)

if (WIN32)
  add_custom_target(install_final
    COMMAND ${CMAKE_COMMAND} -DCOMPONENT=Final -P cmake_install.cmake
    COMMAND windeployqt ${CMAKE_INSTALL_PREFIX}/bin/theme_park.exe
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Install theme_park executables and associated resources..."
    VERBATIM
  )
else()
  add_custom_target(install_final
    COMMAND ${CMAKE_COMMAND} -DCOMPONENT=Final -P cmake_install.cmake
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Install theme_park executables and associated resources..."
    VERBATIM
  )
endif()
