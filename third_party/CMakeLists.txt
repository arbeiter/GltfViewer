add_library(stb_image INTERFACE)
add_library(glm INTERFACE)

target_include_directories(glm INTERFACE glm)
target_include_directories(stb_image INTERFACE stb_image)
