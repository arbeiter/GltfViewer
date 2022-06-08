add_executable(${PROJECT_NAME} 
  shader.h
  main.cpp 
  window.h 
  window.cpp
  gltfscene.cpp 
  gltfscene.h
  utils.hpp)

find_package(glfw3 3.3 REQUIRED)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(LIBS glfw3)

target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")

set(LIBS glm)
set(LIBS stb_image)
set(LIBS json)
set(LIBS tinygltf)

target_link_libraries(${PROJECT_NAME} glad)
target_link_libraries(${PROJECT_NAME} glm stb_image json glfw tinygltf ${LIBS} ${CMAKE_DL_LIBS})
