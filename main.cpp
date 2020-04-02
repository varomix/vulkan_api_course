#include <iostream>

#include "renderer.h"

static GLFWwindow *window;
static Renderer renderer;

void initWindow(const int width = 1280, const int height = 720, const std::string& wName = "Vulkan Course")
{
	glfwInit();

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	window = glfwCreateWindow(width, height, wName.c_str(), nullptr, nullptr);
}

void processInput(GLFWwindow *window)
{
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

int main()
{
	initWindow();

	// vulkan renderer instance
	if(renderer.init(window) == EXIT_FAILURE)
	{
		return EXIT_FAILURE;
	}

    while (!glfwWindowShouldClose(window))
    {
        processInput(window);
		//glfwPollEvents();
		glfwWaitEventsTimeout(0.01);
    }

	renderer.cleanup();

    glfwDestroyWindow(window);
    glfwTerminate();


    return 0;
}
