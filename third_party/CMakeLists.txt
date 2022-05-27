add_library(stb_image INTERFACE)
add_library(glm INTERFACE)
add_library(tiny_gltf INTERFACE)

target_include_directories(glm INTERFACE glm)
target_include_directories(stb_image INTERFACE stb_image)
target_include_directories(tiny_gltf INTERFACE tiny_gltf)
