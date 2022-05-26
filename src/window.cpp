#include "window.h"

Window::Window(int x, int y, const char* title)
{
	GLFWwindow* window = glfwCreateWindow(x, y, title, NULL, NULL);
	this->window = window;
}

Window::~Window()
{
	glfwDestroyWindow(this->window);
}

int Window::Close()
{
	return glfwWindowShouldClose(this->window);
}
