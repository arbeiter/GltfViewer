
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

add_library(imgui INTERFACE)
set(IMGUI_DIR ${CMAKE_CURRENT_SOURCE_DIR}/imgui)
set(IMGUI_SRC
${IMGUI_DIR}/imgui.cpp
${IMGUI_DIR}/imgui_demo.cpp
${IMGUI_DIR}/imgui_draw.cpp
${IMGUI_DIR}/imgui_tables.cpp
${IMGUI_DIR}/imgui_widgets.cpp
${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
${IMGUI_DIR}/backends/imgui_impl_opengl3.h
${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
${IMGUI_DIR}/backends/imgui_impl_glfw.h
)
target_sources(imgui INTERFACE ${IMGUI_SRC})
target_include_directories(imgui INTERFACE ${IMGUI_DIR})
