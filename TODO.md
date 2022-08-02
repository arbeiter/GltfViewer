### Feature List
- Import camera
- Integrate normal and tangent geometry
- Normal viewer with geometry shader

### Nice to have
- Bounding box and move camera to a position above the centroid

- Figure out how to make multiple textures work
- Node hierarchy broken?

### Bugs
- Clear texture when switching between filetypes
- Colored vertices: Figure out how to reset stuff when its not set

### Checklist before git push
- Add Imgui to circle between all models
- First person camera
  - Flyby camera
- Figure out how to input a metallic texture
- Test Non-PBR materials

### Done
- Integrate Imgui
- Test Duck.gltf
- Input entry modification
- Set default values if not present to the frag shader
  - Test1
  - Metallic etc.
- Test 4 and 5 one after the other
- Test Sponza first

### Notes

#### IMGUI Static linking notes
###### Move glad to third_party and compile

###### Option 0: Link as interface

```
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
```

###### Option 1: Statically link IMGUI

In third_party
```
set(IMGUI_DIR imgui)
add_library(imgui STATIC)
add_library(imgui::imgui ALIAS imgui)
target_sources(imgui
  PRIVATE
    ##### Main Files #####
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_demo.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
    ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp)
  target_include_directories(imgui
    PUBLIC
      ${IMGUI_DIR}
  )
  target_link_libraries(imgui
    PUBLIC
      Glad
      glfw::glfw
  )
```

```
target_link_libraries(${PROJECT_NAME}
  PRIVATE
    renderer::backend
    imgui::imgui
    glfw::glfw
)
```

```
    set(IMGUI_INCLUDE_DIR ${imgui_SOURCE_DIR}/)
    file(GLOB IMGUI_SOURCES ${imgui_SOURCE_DIR}/*.cpp)
    file(GLOB IMGUI_HEADERS ${imgui_SOURCE_DIR}/*.h)
    add_library(imgui STATIC ${IMGUI_SOURCES}
                             ${IMGUI_SOURCES}
                             ${imgui_SOURCE_DIR}/examples/imgui_impl_glfw.cpp
                             ${imgui_SOURCE_DIR}/examples/imgui_impl_opengl3.cpp)
    add_definitions(-DIMGUI_IMPL_OPENGL_LOADER_GLAD)
    target_include_directories(imgui PUBLIC ${IMGUI_INCLUDE_DIR}
                                            ${OPENGL_INCLUDE_DIR}
                                            ${GLFW_INCLUDE_DIR}
                                            ${GLAD_INCLUDE_DIR})
    target_link_libraries(imgui ${OPENGL_LIBRARIES} glfw glad)
endif ()

```

###### Failed Attempt 1:

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

  Add a CMake to DearImgUi
  ```

  cmake_minimum_required(VERSION 3.8)
  add_library(ImGui)
  add_library(ImGui::ImGui ALIAS ImGui)
  target_include_directories(ImGui PUBLIC ./)
  target_sources(ImGui PRIVATE
      imgui_demo.cpp
      imgui_draw.cpp
      imgui_tables.cpp
      imgui_widgets.cpp
      imgui.cpp
      misc/cpp/imgui_stdlib.cpp
  )
  ```

  Other:
  ```
    add_library(
            imgui
            imgui/imgui.cpp
            imgui/imgui.h
            imgui/imgui_demo.cpp
            imgui/imgui_draw.cpp
            imgui/imgui_widgets.cpp
            imgui/imgui_tables.cpp
            imgui/imgui_internal.h
            imgui/imstb_rectpack.h
            imgui/imstb_textedit.h
            imgui/imstb_truetype.h
            imgui/backends/imgui_impl_opengl3.cpp
            imgui/backends/imgui_impl_opengl3.h
            imgui/backends/imgui_impl_glfw.cpp
            imgui/backends/imgui_impl_glfw.h
    )

    target_link_libraries(imgui PUBLIC glfw glad)
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DIMGUI_IMPL_OPENGL_LOADER_GLAD2")
    target_include_directories(
            imgui
            PUBLIC
            imgui
            imgui/backends
    )
  ```

  Static Link:
  ```
  set( IMGUI_SRC_DIR ${THIRDPARTY_DIR}/imgui )
  set( IMGUI_PUBLIC_HEADERS ${IMGUI_SRC_DIR}/imgui.h )
  set( IMGUI_SOURCES
      ${IMGUI_SRC_DIR}/imgui.cpp
      ${IMGUI_SRC_DIR}/imgui_draw.cpp
      ${IMGUI_SRC_DIR}/imgui_demo.cpp
      ${IMGUI_SRC_DIR}/imgui_widgets.cpp
      ${THIRDPARTY_DIR}/glad/glad.c )
  add_library( imgui STATIC ${IMGUI_SOURCES} )
  target_include_directories( imgui PRIVATE ${THIRDPARTY_DIR} ${IMGUI_SRC_DIR} )
  set( MAIN_APP_SOURCES
      ${CMAKE_CURRENT_LIST_DIR}/src/main.cpp
      ${CMAKE_CURRENT_LIST_DIR}/src/imgui_impl/opengl3.cpp
      ${CMAKE_CURRENT_LIST_DIR}/src/imgui_impl/glfw.cpp
      ${CMAKE_CURRENT_LIST_DIR}/src/imgui_impl/fonts.cpp )
  ```

  https://alandefreitas.github.io/moderncpp/guis/imgui/
  ```
  FetchContent_Declare(
          imgui
          GIT_REPOSITORY https://github.com/ocornut/imgui.git
          GIT_TAG v1.77
  )
  FetchContent_GetProperties(imgui)
  if (NOT imgui_POPULATED)
      FetchContent_Populate(imgui)
      set(IMGUI_INCLUDE_DIR ${imgui_SOURCE_DIR}/)
      file(GLOB IMGUI_SOURCES ${imgui_SOURCE_DIR}/*.cpp)
      file(GLOB IMGUI_HEADERS ${imgui_SOURCE_DIR}/*.h)
      add_library(imgui STATIC ${IMGUI_SOURCES}
                               ${IMGUI_SOURCES}
                               ${imgui_SOURCE_DIR}/examples/imgui_impl_glfw.cpp
                               ${imgui_SOURCE_DIR}/examples/imgui_impl_opengl3.cpp)
      add_definitions(-DIMGUI_IMPL_OPENGL_LOADER_GLAD)
      target_include_directories(imgui PUBLIC ${IMGUI_INCLUDE_DIR} ${OPENGL_INCLUDE_DIR} ${GLFW_INCLUDE_DIR} ${GLAD_INCLUDE_DIR})
      target_link_libraries(imgui ${OPENGL_LIBRARIES} glfw glad)
  endif ()
  ```
