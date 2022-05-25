cmake_minimum_required(VERSION 3.7)
set(CMAKE_CXX_STANDARD 17)
project(OpenGLTestApp)
add_subdirectory(third_party)
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/bin")

find_package(glfw3 3.3 REQUIRED)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(LIBS glfw3)

add_library(GLAD "include/glad/glad.c")

add_executable(${PROJECT_NAME} shader.h main.cpp include/glad/glad.c)
set(LIBS GLAD)
set(LIBS glm)
set(LIBS stb_image)

include_directories("${CMAKE_SOURCE_DIR}/include")
target_link_libraries(${PROJECT_NAME} glm stb_image)
target_link_libraries(${PROJECT_NAME} glm stb_image glfw ${LIBS} ${CMAKE_DL_LIBS})
