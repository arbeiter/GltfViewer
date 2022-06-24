#include <glad/gl.h>
#include <glm.hpp>
#include <GLFW/glfw3.h>
#include <iostream>
#include <window.h>
#include <ostream>
#include <vector>
#include "arcball.h"
#include "gltfscene.h"

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

    while (!glfwWindowShouldClose(window.window))
    {
        processInput(window.window);
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);
        view = camera.transform();
        scene.setWidthAndHeight(curr_width, curr_height);
        scene.loadAndDrawTriangle(view);
        glfwSwapBuffers(window.window);
        glfwPollEvents();
    }
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
  camera.zoom(yoffset * 0.1);
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
int main()
{
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

    displayLoop(window);
    glfwTerminate();
    return 0;
}
