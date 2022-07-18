### IMGUI

#### TODO
- 1: Text
  - CMake
  - GLFW-IMGui-Opengl
- 2: FilePicker
  - Numbers in a vector
  - Feed number into Loader

#### Cmake
Fetch ImGUI
https://github.com/JulesFouchy/p6/blob/6c4ab661de9a27223626a975e84ba67d55731ecc/CMakeLists.txt
##### Experiments
Attempt 1:

git clone https://github.com/ocornut/imgui
```
# ---Add Dear ImGui---
add_subdirectory(third-party/imgui)
# Build ImGui with glfw
target_link_libraries(ImGui PRIVATE glfw)
target_sources(ImGui PRIVATE third-party/imgui/backends/imgui_impl_glfw.cpp)
# Build ImGui with OpenGL
target_sources(ImGui PRIVATE third-party/imgui/backends/imgui_impl_opengl3.cpp)
#
target_link_libraries(${PROJECT_NAME} PUBLIC ImGui::ImGui)
```

Link ImgUI

### Loading imgui


```
#include "examples/imgui_impl_glfw.h"
#include "examples/imgui_impl_opengl3.h"
#if defined(IMGUI_IMPL_OPENGL_LOADER_GL3W)
#include <GL/gl3w.h> // Initialize with gl3wInit()
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLEW)
#include <GL/glew.h> // Initialize with glewInit()
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLAD)
#include <glad/glad.h> // Initialize with gladLoadGL()
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLBINDING2)
#define GLFW_INCLUDE_NONE // GLFW including OpenGL headers causes ambiguity or
                          // multiple definition errors.
#include <glbinding/Binding.h> // Initialize with glbinding::Binding::initialize()
#include <glbinding/gl/gl.h>
```

#### Code

// Outside render loop
// Initialize ImGUI
IMGUI_CHECKVERSION();
ImGui::CreateContext();
ImGuiIO& io = ImGui::GetIO(); (void)io;
ImGui::StyleColorsDark();
ImGui_ImplGlfw_InitForOpenGL(window, true);
ImGui_ImplOpenGL3_Init("#version 330");

// Variables to be changed in the ImGUI window
bool drawTriangle = true;
float size = 1.0f;
float color[4] = { 0.8f, 0.3f, 0.02f, 1.0f };

/// INSIDE RENDER LOOP
// Tell OpenGL a new frame is about to begin
ImGui_ImplOpenGL3_NewFrame();
ImGui_ImplGlfw_NewFrame();
ImGui::NewFrame();

// ImGUI window creation
ImGui::Begin("My name is window, ImGUI window");
// Text that appears in the window
ImGui::Text("Hello there adventurer!");
// Checkbox that appears in the window
ImGui::Checkbox("Draw Triangle", &drawTriangle);
// Slider that appears in the window
ImGui::SliderFloat("Size", &size, 0.5f, 2.0f);
// Fancy color editor that appears in the window
ImGui::ColorEdit4("Color", color);
// Ends the window
ImGui::End();

// Renders the ImGUI elements
ImGui::Render();
ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

// Cleanup
ImGui_ImplOpenGL3_Shutdown();
ImGui_ImplGlfw_Shutdown();
ImGui::DestroyContext();

### Checklist before git push

- Add Imgui to circle between all models
- Colored vertices test
- First person camera
  - Flyby camera
- Figure out how to input a metallic texture
- Test Non-PBR materials

### Done
- Test Duck.gltf
- Input entry modification
- Set default values if not present to the frag shader
  - Test1
  - Metallic etc.
- Test 4 and 5 one after the other
- Test Sponza first

### Notes
