add_executable(${PROJECT_NAME}
  arcball.h
  arcball.cpp
  shader.h
  main.cpp
  camera.h
  camera.cpp
  window.h
  window.cpp
  gltfscene.h
  gltfscene.cpp
  framebuffer.hpp
  custom_geometry.hpp
  mesh.hpp
  mesh.cpp
  texture.hpp
  texture.cpp
  utils.hpp)

find_package(glfw3 3.3 REQUIRED)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(LIBS glfw3)

target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")

set(LIBS glm)
set(LIBS stb_image)
set(LIBS json)
set(LIBS tinygltf)
set(LIBS imgui)

target_link_libraries(${PROJECT_NAME} glad)
target_link_libraries(${PROJECT_NAME} glm stb_image json glfw tinygltf imgui ${LIBS} ${CMAKE_DL_LIBS})
