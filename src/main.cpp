#include <glad/gl.h>
#include <glm.hpp>
#include <GLFW/glfw3.h>
#include <iostream>
#include <window.h>
#include <ostream>
#include <vector>
#include "arcball.h"
#include "camera.h"
#include "gltfscene.h"
#include "imgui.h"
#include "framebuffer.hpp"
// TODO: Backends should be imgui/backends. Fix this
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

int curr_width = 800;
int curr_height = 600;
float lastX = 10;
float lastY = 10;
bool isFirstMouse = false;

double previousTime = glfwGetTime();
int frameCount = 0;

glm::vec3 eye(0, 0, 5);
glm::vec3 center(0);
glm::vec3 up(0, 1, 0);
glm::vec2 prev_mouse(-2.f);

float zoom_amount = 1;
ArcballCamera camera(eye, center, up);
Camera quat_camera(center);

void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void processInput(GLFWwindow *window);
Shader setupScreenShader();
void fpsCalc();

int ShowStyleSelector(const char* label, int &selectedLabel);

void initImgUi(Window &window);
int drawGui(int &selectedModel);
void lightingBlob(Shader& ourShader);

static void GLAPIENTRY glDebugCallback(GLenum source, GLenum type, GLuint, GLenum severity, GLsizei,
                                       const GLchar* message, const void*)
{
    // clang-format off
    const char* severityString = "?";
    switch (severity) {
        case GL_DEBUG_SEVERITY_HIGH:            severityString = "high";    break;
        case GL_DEBUG_SEVERITY_MEDIUM:          severityString = "medium";  break;
        case GL_DEBUG_SEVERITY_LOW:             severityString = "low";     break;
        case GL_DEBUG_SEVERITY_NOTIFICATION:                                return;
    }

    const char* sourceString = "?";
    switch (source) {
        case GL_DEBUG_SOURCE_API:               sourceString = "API";               break;
        case GL_DEBUG_SOURCE_WINDOW_SYSTEM:     sourceString = "window system";     break;
        case GL_DEBUG_SOURCE_SHADER_COMPILER:   sourceString = "shader compiler";   break;
        case GL_DEBUG_SOURCE_THIRD_PARTY:       sourceString = "third party";       break;
        case GL_DEBUG_SOURCE_APPLICATION:       sourceString = "app";               break;
        case GL_DEBUG_SOURCE_OTHER:             sourceString = "other";             break;
    }

    const char* typeString = "?";
    switch (type) {
        case GL_DEBUG_TYPE_ERROR:                   typeString = "error";               break;
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:     typeString = "deprecated behavior"; break;
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:      typeString = "undefined behavior";  break;
        case GL_DEBUG_TYPE_PORTABILITY:             typeString = "portability";         break;
        case GL_DEBUG_TYPE_MARKER:                  typeString = "marker";              break;
        case GL_DEBUG_TYPE_PUSH_GROUP:              typeString = "push group";          break;
        case GL_DEBUG_TYPE_POP_GROUP:               typeString = "pop group";           break;
        case GL_DEBUG_TYPE_OTHER:                   typeString = "other";               break;
    }
    // clang-format on

    fprintf(stderr, "OpenGL Message.\n Type: %s\nSeverity: %s\nSource: %s\nMessage: %s\n\n", typeString, severityString,
            sourceString, message);

    if (severity == GL_DEBUG_SEVERITY_HIGH)
        throw(std::runtime_error("GL Error"));
}

void fpsCalc() {
  // Simple FPS timer
  double currentTime = glfwGetTime();
  frameCount++;
  if ( currentTime - previousTime >= 1.0 )
  {
      std::cout << "FPS " << frameCount << std::endl;
      frameCount = 0;
      previousTime = currentTime;
  }
}

static void initGLDebug() {
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(glDebugCallback, NULL);
    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
}

void lightingBlob(Shader& lightingShader) {
  glm::vec3 pointLightPositions[] = {
      glm::vec3( 1.0f,  1.2f,  1.7f),
      glm::vec3( 2.3f, -3.3f, -4.0f),
      glm::vec3(-4.0f,  2.0f, -12.0f),
      glm::vec3( 0.0f,  0.0f, -3.0f)
  };

  lightingShader.setVec3("pointLights[0].position", pointLightPositions[0]);
  lightingShader.setVec3("pointLights[0].ambient", 0.05f, 0.05f, 0.05f);
  lightingShader.setVec3("pointLights[0].diffuse", 0.8f, 0.8f, 0.8f);
  lightingShader.setVec3("pointLights[0].specular", 1.0f, 1.0f, 1.0f);
  lightingShader.setFloat("pointLights[0].constant", 1.0f);
  lightingShader.setFloat("pointLights[0].linear", 0.09f);
  lightingShader.setFloat("pointLights[0].quadratic", 0.032f);

  // point light 2
  lightingShader.setVec3("pointLights[1].position", pointLightPositions[1]);
  lightingShader.setVec3("pointLights[1].ambient", 0.05f, 0.05f, 0.05f);
  lightingShader.setVec3("pointLights[1].diffuse", 0.8f, 0.8f, 0.8f);
  lightingShader.setVec3("pointLights[1].specular", 1.0f, 1.0f, 1.0f);
  lightingShader.setFloat("pointLights[1].constant", 1.0f);
  lightingShader.setFloat("pointLights[1].linear", 0.09f);
  lightingShader.setFloat("pointLights[1].quadratic", 0.032f);

  // point light 3
  lightingShader.setVec3("pointLights[2].position", pointLightPositions[2]);
  lightingShader.setVec3("pointLights[2].ambient", 0.05f, 0.05f, 0.05f);
  lightingShader.setVec3("pointLights[2].diffuse", 0.8f, 0.8f, 0.8f);
  lightingShader.setVec3("pointLights[2].specular", 1.0f, 1.0f, 1.0f);
  lightingShader.setFloat("pointLights[2].constant", 1.0f);
  lightingShader.setFloat("pointLights[2].linear", 0.09f);
  lightingShader.setFloat("pointLights[2].quadratic", 0.032f);

  // point light 4
  lightingShader.setVec3("pointLights[3].position", pointLightPositions[3]);
  lightingShader.setVec3("pointLights[3].ambient", 0.05f, 0.05f, 0.05f);
  lightingShader.setVec3("pointLights[3].diffuse", 0.8f, 0.8f, 0.8f);
  lightingShader.setVec3("pointLights[3].specular", 1.0f, 1.0f, 1.0f);
  lightingShader.setFloat("pointLights[3].constant", 1.0f);
  lightingShader.setFloat("pointLights[3].linear", 0.09f);
  lightingShader.setFloat("pointLights[3].quadratic", 0.032f);
}

Shader setupScreenShader() {
  Shader screenShader("shaders/framebuffer_screen.vs", "shaders/framebuffer_screen.fs");
  screenShader.use();
  screenShader.setInt("screenTexture", 0);
  return screenShader;
}

Shader setupSimpleShader() {
  Shader simpleShader("shaders/simple_shader.vs", "shaders/simple_shader.fs");
  simpleShader.use();
  simpleShader.setInt("texture1", 0);
  return simpleShader;
}

// TODO: Move to plane class
unsigned int planeVAO, planeVBO;
unsigned int floorTexture;

// utility function for loading a 2D texture from file
// ---------------------------------------------------
unsigned int loadTexture(char const * path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}

void loadTestScene() {
    float planeVertices[] = {
         // positions         // texture Coords
         5.0f, -0.5f,  5.0f,  2.0f, 0.0f,
        -5.0f, -0.5f,  5.0f,  0.0f, 0.0f,
        -5.0f, -0.5f, -5.0f,  0.0f, 2.0f,
         5.0f, -0.5f,  5.0f,  2.0f, 0.0f,
        -5.0f, -0.5f, -5.0f,  0.0f, 2.0f,
         5.0f, -0.5f, -5.0f,  2.0f, 2.0f
    };
    // plane VAO
    planeVAO = 0;
    planeVBO = 0;

    glGenVertexArrays(1, &planeVAO);
    glGenBuffers(1, &planeVBO);
    glBindVertexArray(planeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), &planeVertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    floorTexture = loadTexture("resources/textures/metal.png");
}

void drawTestScene(Shader &simpleShader, glm::mat4 &view) {
    glm::mat4 projection = glm::perspective(glm::radians(30.0f), (float)(curr_width / curr_height), 0.1f, 1000.0f);

    glBindVertexArray(planeVAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, floorTexture);
    simpleShader.setMat4("view", view);
    simpleShader.setMat4("model", glm::mat4(1.0f));
    simpleShader.setMat4("projection", projection);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}

void renderModel(Scene &scene, Shader &pbrShader, glm::mat4 &view) {
    view = quat_camera.getViewMatrix();
    scene.setWidthAndHeight(curr_width, curr_height);

    int selectedModel = 1;
    int currentModel = drawGui(selectedModel);
    if(currentModel != selectedModel) {
      selectedModel = currentModel;
      std::cout << " Loading selected Model " << selectedModel << " " << currentModel << std::endl;
      scene.loadModel(view, selectedModel);
    }
    pbrShader.use();
    scene.setShader(pbrShader, quat_camera.Position);
    lightingBlob(pbrShader);
    scene.drawScene(view);
}

void displayLoop(Window &window, std::string filename) {
    Shader ourShader("shaders/shader.vert", "shaders/pbr_shader_simplified.frag"); // you can name your shader files however you like
    Shader geometryShader("shaders/normal.vert", "shaders/normal.frag", "shaders/normal.gs");

    glm::mat4 view = quat_camera.getViewMatrix();
    Scene scene = Scene(ourShader, filename);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);

    scene.loadModel(view, 1);
    bool drawTriangle = true;
    float size = 1.0f;

    Shader screenShader = setupScreenShader();
    Shader simpleShader = setupSimpleShader();

    FrameBuffer fb = FrameBuffer(screenShader, curr_width, curr_height);
    fb.setup();

    while (!glfwWindowShouldClose(window.window))
    {
        processInput(window.window);
        glClearColor(0.1f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);
        fb.renderToFramebuffer();

        ourShader.use();
        renderModel(scene, ourShader, view);
        fb.clearAndRenderQuad();

        glfwSwapBuffers(window.window);
        glfwPollEvents();
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

int drawGui(int &selectedModel) {
    /// INSIDE RENDER LOOP
    // Tell OpenGL a new frame is about to begin
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // ImGUI window creation
    ImGui::Begin("FilePicker");
    int file = ShowStyleSelector("Pick File", selectedModel);
    ImGui::End();

    // Renders the ImGUI elements
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    return file;
}

void processInput(GLFWwindow *window)
{
    glm::vec2 p_mouse(0.005f);
    glm::vec2 c_mouse(-0.005f);
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
      glfwSetWindowShouldClose(window, true);
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
      quat_camera.processKeyboard(Camera_Movement::FORWARD, 0.01);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
      quat_camera.processKeyboard(Camera_Movement::BACKWARD, 0.01);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
      quat_camera.processKeyboard(Camera_Movement::RIGHT, 0.01);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
      quat_camera.processKeyboard(Camera_Movement::LEFT, 0.01);
    }
}

glm::vec2 transform_mouse(int h, int w, glm::vec2 in)
{
    return glm::vec2(in.x * 2.f / w - 1.f, 1.f - 2.f * in.y / h );
}

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    int* screen_width = new int(0);
    int* screen_height = new int(0);
    glfwGetWindowSize( window, screen_width, screen_height );

    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);
    if(isFirstMouse) {
      isFirstMouse = false;
      lastX = (float)xpos;
      lastY = (float)ypos;
    }
    float xoffset = (float)(xpos - lastX);
    float yoffset = (float)(lastY - ypos);
    lastX = (float)xpos;
    lastY = (float)ypos;

    quat_camera.processMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
  camera.zoom(yoffset * 1.0);
}

void glfwSetWindowSizeCallback(GLFWwindow* window, int width, int height)
{
  curr_width = width;
  curr_height = height;
  std::cout << "CURR_WIDTH" << width << std::endl;
};

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

void initImgUi(Window &window) {
  // Initialize ImGUI
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO(); (void)io;
  ImGui::StyleColorsDark();
  ImGui_ImplGlfw_InitForOpenGL(window.window, true);
  ImGui_ImplOpenGL3_Init("#version 330");
}

int ShowStyleSelector(const char* label, int &model_number)
{
    if (ImGui::Combo(label, &model_number, "A\0B\0C\0D\0E\0F\0G\0"))
    {
        switch (model_number)
        {
          case 'A': model_number = 0; break;
          case 'B': model_number = 1; break;
          case 'C': model_number = 2; break;
          case 'D': model_number = 3; break;
          case 'E': model_number = 4; break;
          case 'F': model_number = 5; break;
          case 'G': model_number = 6; break;
          case 'H': model_number = 7; break;
        }
        std::cout << "MODEL NUMBER " << model_number << std::endl;
        return model_number + 1;
    }
    return model_number;
}

int main(int argc, char **argv)
{
  std::string arg = argv[1];
  std::cout << "Take in path " << arg << std::endl;
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef NDEBUG
  printf("Running in release mode\n");
#else
  glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);
  printf("Running in debug mode\n");
#endif

  Window window = Window(800, 600, "TinyGLTF basic example");
  glfwMakeContextCurrent(window.window);
  glfwSetCursorPosCallback(window.window, mouse_callback);
  glfwSetScrollCallback(window.window, scroll_callback);
  glfwSetWindowSizeCallback(window.window, glfwSetWindowSizeCallback);
  glfwSetFramebufferSizeCallback(window.window, framebuffer_size_callback);

  if(!gladLoadGL((GLADloadfunc) glfwGetProcAddress))
  {
      std::cout << "Failed to initialize GLAD" << std::endl;
      return -1;
  }

  int flags; glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
  if(flags & GL_CONTEXT_FLAG_DEBUG_BIT ) {
    std::cout << "Debug flags set" << std::endl;
    initGLDebug();
  }

  initImgUi(window);
  displayLoop(window, arg);
  glfwTerminate();
  return 0;
}
