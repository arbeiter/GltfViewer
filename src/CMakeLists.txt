add_executable(${PROJECT_NAME} 
  shader.h
  main.cpp 
  window.h 
  window.cpp
  gltfscene.cpp 
  gltfscene.h)

find_package(glfw3 3.3 REQUIRED)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(LIBS glfw3)

target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")

set(LIBS glm)
set(LIBS stb_image)
set(LIBS stb_image_write)
set(LIBS json)
set(LIBS tiny_gltf)

target_link_libraries(${PROJECT_NAME} glad)
target_link_libraries(${PROJECT_NAME} glm stb_image stb_image_write json tiny_gltf )
target_link_libraries(${PROJECT_NAME} glm stb_image stb_image_write json glfw tiny_gltf ${LIBS} ${CMAKE_DL_LIBS})
