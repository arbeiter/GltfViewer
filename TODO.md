### Code

#version 100
precision mediump float;

float PI = 3.1415;

varying vec3 vPos;
varying vec3 vNormal;
varying vec2 vUv;
varying mat3 vTBN;

uniform vec3 camPos;
uniform vec3 lightPos;

uniform float time;

uniform sampler2D baseColorTexture;
uniform sampler2D metallicRoughnessTexture;
uniform sampler2D normalTexture;

float distribution_ggx(float a, vec3 N, vec3 H) {
    float ndh = max(dot(N, H), 0.0);
    float denom = ndh*ndh*(a*a-1.0)+1.0;
    return a*a / (PI*denom*denom);
}

float geometry_schlick(float a, vec3 N, vec3 V) {
    float r = a + 1.0;
    float k = r*r/8.0;
    float ndv = max(dot(N, V), 0.0);
    return ndv / (ndv * (1.0-k) + k);
}

float geometry_smith(float a, vec3 N, vec3 V, vec3 L) {
    return geometry_schlick(a, N, L) * geometry_schlick(a, N, V);
}

vec3 fresnel_schlick(vec3 f0, vec3 H, vec3 V) {
    return f0 + (1.0 - f0) * pow(1.0 - max(dot(H, V), 0.0), 5.0);
}

void main () {
    float gamma = 2.2;

    // lights
    vec3 lightColor = vec3(1.0, 1.0, 1.0);
    // vec3 lightPos = vec3(cos(time)*2.0, 2.0, sin(time)*2.0);

    // textures
    // convert color from sRGB to linear
    vec4 baseColor = pow(texture2D(baseColorTexture, vUv), vec4(gamma));
    float roughness = texture2D(metallicRoughnessTexture, vUv).g;
    float metallic = texture2D(metallicRoughnessTexture, vUv).b;
    vec3 normal = texture2D(normalTexture, vUv).rgb;
    normal = normalize(2.0*normal - 1.0);
    normal = normal * vec3(1.0, -1.0, 1.0); // idk why, maybe the way painter exports normals?

    // world space normal
    vec3 N = normalize(vTBN * normal);

    // lighting vectors
    vec3 L = normalize(lightPos - vPos);
    vec3 V = normalize(camPos - vPos);
    vec3 H = normalize(L + V);

    // albedo
    vec3 f0 = mix(vec3(0.04), baseColor.rgb, metallic);

    // lighting visibility / occlusion
    float d = distribution_ggx(roughness, N, H);
    float g = geometry_smith(roughness, N, V, L);
    vec3 f = fresnel_schlick(f0, H, V);

    // specular and diffuse
    vec3 specular = d * g * f / (4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001);
    vec3 diffuse = mix(baseColor.rgb - f, vec3(0.0), metallic);

    // result color
    vec3 result = (diffuse + specular) * lightColor * max(dot(N, L), 0.0);

    // ambient lighting
    result += vec3(0.03) * baseColor.rgb;

    // gamma correction
    result = pow(result, vec3(1.0/gamma));

    gl_FragColor = vec4(vec3(result), 1.0);
}

sss Feature List
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
