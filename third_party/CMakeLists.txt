
add_library(stb_image INTERFACE)
target_include_directories(stb_image INTERFACE stb_image)

add_library(glm INTERFACE)
target_include_directories(glm INTERFACE glm)

add_library(json INTERFACE)
target_include_directories(json INTERFACE json)

add_library(tinygltf STATIC)
target_sources(tinygltf PRIVATE
        tinygltf/tiny_gltf.h
        tinygltf/tiny_gltf.cpp
)
target_include_directories(tinygltf PUBLIC
        tinygltf
        stb_image
        json
)
add_subdirectory(glad/)
