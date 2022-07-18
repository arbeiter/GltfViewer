cmake_minimum_required(VERSION 3.7)
set(CMAKE_CXX_STANDARD 17)
project(MinimalOpenGL)
add_subdirectory(third_party)
add_subdirectory(src)

set (CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/bin")
