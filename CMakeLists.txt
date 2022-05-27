#cmake version
cmake_minimum_required(VERSION 3.22)
#project name
project(DogeEngine)
#if chiascript is being used, set bigobj
set(CMAKE_CXX_STANDARD 14)

if (MSVC)
    add_definitions(/bigobj)
else ()
    add_definitions(-Wa,-mbig-obj)
endif ()


#set up dependencies folder
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/Dependencies/include/ ${PROJECT_SOURCE_DIR}/Dependencies/include/bullet)
LINK_DIRECTORIES(${PROJECT_SOURCE_DIR}/Dependencies/lib)
#find source
file(GLOB SOURCES
        ${PROJECT_SOURCE_DIR}/*.hpp
        ${PROJECT_SOURCE_DIR}/*.cpp
        ${PROJECT_SOURCE_DIR}/Dependencies/include/imgui/*.cpp #imgui source
        ${PROJECT_SOURCE_DIR}/Dependencies/include/Bullet/*.cpp
        )
#set up source
add_executable(DogeEngine ${SOURCES} GUI.hpp PhysicsManager.hpp Picker.hpp SceneFileManager.hpp)
#add glad subdirectory
add_subdirectory(Dependencies/)

#link libraries
TARGET_LINK_LIBRARIES(DogeEngine glfw3 glad )
#transfer shaders

configure_file(shaders/main.fs main.fs COPYONLY)
configure_file(shaders/main.vs main.vs COPYONLY)
configure_file(shaders/shadow.fs shadow.fs COPYONLY)
configure_file(shaders/shadow.vs shadow.vs COPYONLY)