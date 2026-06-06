cmake_minimum_required(VERSION 3.14)
project(PlantsVsZombies)

set(CMAKE_CXX_STANDARD 17)

include(FetchContent)

FetchContent_Declare(
    raylib
    GIT_REPOSITORY https://github.com/raysan5/raylib.git
    GIT_TAG 5.5
)

FetchContent_MakeAvailable(raylib)

file(GLOB_RECURSE SOURCES
    "${CMAKE_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_SOURCE_DIR}/src/*.c"
)

add_executable(CS163_DataStructureVisualize ${SOURCES})

target_include_directories(CS163_DataStructureVisualize
    PRIVATE ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(CS163_DataStructureVisualize
    raylib
    winmm
    gdi32
    opengl32
    comdlg32
    ole32
)