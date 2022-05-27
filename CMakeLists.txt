cmake_minimum_required(VERSION 3.7)
set(CMAKE_CXX_STANDARD 17)
project(OpenGLTestApp)
add_subdirectory(third_party)
add_subdirectory(src)
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/bin")

find_package(glfw3 3.3 REQUIRED)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(LIBS glfw3)

add_library(GLAD "include/glad/glad.c")

add_executable(${PROJECT_NAME} src/shader.h src/main.cpp src/window.h src/window.cpp src/gltfscene.cpp src/gltfscene.h include/glad/glad.c)
set(LIBS GLAD)
set(LIBS glm)
set(LIBS stb_image)
set(LIBS tiny_gltf)

include_directories("${CMAKE_SOURCE_DIR}/include")
target_link_libraries(${PROJECT_NAME} glm stb_image tiny_gltf)
target_link_libraries(${PROJECT_NAME} glm stb_image glfw tiny_gltf ${LIBS} ${CMAKE_DL_LIBS})
