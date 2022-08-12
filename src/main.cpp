#include <glad/gl.h>
#include <glm.hpp>
#include <GLFW/glfw3.h>
#include <iostream>
#include <window.h>
#include <ostream>
#include <vector>
#include "arcball.h"
#include "gltfscene.h"
#include "imgui.h"
// TODO: Backends should be imgui/backends. Fix this
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

int curr_width = 800;
int curr_height = 600;

glm::vec3 eye(0, 0, 5);
glm::vec3 center(0);
glm::vec3 up(0, 1, 0);
glm::vec2 prev_mouse(-2.f);

float zoom_amount = 1;
ArcballCamera camera(eye, center, up);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void processInput(GLFWwindow *window);
int ShowStyleSelector(const char* label, int &selectedLabel);

void initImgUi(Window &window);
int drawGui(int &selectedModel);

void displayLoop(Window &window) {
    Shader ourShader("shader.vert", "shader.frag"); // you can name your shader files however you like
    std::string shader_vert_path = "shader.vert";
    std::string shader_frag_path = "shader.frag";
    glm::mat4 view = camera.transform();
    Scene scene = Scene(ourShader);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
    scene.loadModel(view, 1);
    int selectedModel = 1;

    // Variables to be changed in the ImGUI window
    bool drawTriangle = true;
    float size = 1.0f;
    float color[4] = { 0.8f, 0.3f, 0.02f, 1.0f };

    while (!glfwWindowShouldClose(window.window))
    {
        processInput(window.window);
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);

        // Draw scene
        view = camera.transform();
        scene.setWidthAndHeight(curr_width, curr_height);
        int currentModel = drawGui(selectedModel);
        if(currentModel != selectedModel) {
          selectedModel = currentModel;
          std::cout << " Loading selected Model " << selectedModel << " " << currentModel << std::endl;
          scene.loadModel(view, selectedModel);
        }

        scene.drawScene(view);

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
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
      camera.pan(p_mouse);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
      camera.pan(c_mouse);
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

    const glm::vec2 cur_mouse = transform_mouse(*screen_height, *screen_width, glm::vec2(xpos, ypos));
    if(prev_mouse != glm::vec2(-2.f)) {
      camera.rotate(prev_mouse, cur_mouse);
    }
    prev_mouse = cur_mouse;
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
  return 11;
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

  initImgUi(window);
  displayLoop(window);
  glfwTerminate();
  return 0;
}
