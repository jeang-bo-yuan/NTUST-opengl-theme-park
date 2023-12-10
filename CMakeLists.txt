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
    src/main.cpp

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


