struct mouse_input
{
  bool pressed[GLFW_MOUSE_BUTTON_LAST];
  bool down[GLFW_MOUSE_BUTTON_LAST];
  bool released[GLFW_MOUSE_BUTTON_LAST];
  glm::vec2 screenPos, worldPos;
  glm::vec2 scrollOffset;
};

struct kb_input
{
  bool pressed[GLFW_KEY_LAST];
  bool down[GLFW_KEY_LAST];
  bool released[GLFW_KEY_LAST];
};

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

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
  switch (action)
  {
    case GLFW_RELEASE:
      mouse.released[button] = true;
      mouse.down[button] = false;
      break;
    case GLFW_PRESS: // fall-through intentional
      mouse.pressed[button] = true;
    case GLFW_REPEAT:
      mouse.down[button] = true;
      break;
    default:
      ASSERT_MSG(0, "Invalid keycode.");
      break;
  }
  cout << "Mouse clicked: " << (button) << " Action: " << (action) << endl;
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
  camera.zoom(yoffset * 0.1);
}

void init_glfw_callbacks(GLFWwindow* window) {
  glfwSetKeyCallback(window, keyboard_callback);
  glfwSetCursorPosCallback(window.window, mouse_callback);
  glfwSetMouseButtonCallback(window, mouse_button_callback);
  glfwSetScrollCallback(window.window, scroll_callback);
}

void update() {
  glfwPollEvents();
}
