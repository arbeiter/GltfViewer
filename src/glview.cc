/*

    auto window = glfwCreateWindow(1024, 768, title.str().c_str(), NULL, NULL);
    if (window == NULL)
    {
        std::cerr << "Failed to open GLFW window. " << std::endl;
        glfwTerminate();
        return 1;
    }
*/

/*
    //  II. Input Key Callback
    glfwSetKeyCallback(window, [] (GLFWwindow *window, int key, int scancode, int action, int mods)
    {
        (void)scancode;
        (void)mods;
        if ((action == GLFW_PRESS || action == GLFW_REPEAT) && (key == GLFW_KEY_Q || key == GLFW_KEY_ESCAPE))
        {
            glfwSetWindowShouldClose(window, GL_TRUE);
        }
    });
*/

/*
  III. Setup camera, shaders
  IV. Attempt to setup gltf scene
 */

/*
// V. Render Loop
//
// 
    while (glfwWindowShouldClose(window) == GL_FALSE)
    {
        glfwPollEvents();
        glClearColor(0.3f, 0.4f, 0.6f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 
        glEnable(GL_DEPTH_TEST);
    }
    glfwTerminate();
    return 0;

*/
