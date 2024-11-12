cmake_minimum_required(VERSION 3.10)
project(octray VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_FLAGS_DEBUG "-g -DDEBUG")

include_directories(libs/glm)
include_directories(include)
file(GLOB SOURCES "src/*.cpp")

set(IMGUI_DIR libs/imgui)
file(GLOB IMGUI_SOURCES 
    ${IMGUI_DIR}/*.cpp 
    ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp 
    ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
)
add_subdirectory(libs/glfw)

add_library(glad "libs/glad/src/glad.c")
target_include_directories(glad PUBLIC libs/glad/include)

add_executable(${PROJECT_NAME} ${SOURCES} ${IMGUI_SOURCES})

target_include_directories(${PROJECT_NAME} 
    PRIVATE 
        ${IMGUI_DIR} 
        ${IMGUI_DIR}/backends 
        libs/glfw/include
        libs/glad/include
)
target_link_libraries(${PROJECT_NAME} 
    PRIVATE 
        glfw
        glad
)

set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
)

target_compile_definitions(${PROJECT_NAME} PRIVATE "IMGUI_IMPL_OPENGL_LOADER_GLAD")

if(WIN32)
    target_link_libraries(${PROJECT_NAME} PRIVATE opengl32)
elseif(APPLE)
    find_library(OpenGL_LIB OpenGL)
    target_link_libraries(${PROJECT_NAME} PRIVATE ${OpenGL_LIB})
else()
    target_link_libraries(${PROJECT_NAME} PRIVATE GL)
endif()
