#include <iostream>

#include "renderer.h"

static GLFWwindow *window;
static Renderer renderer;

void initWindow(const int width = 800, const int height = 600, std::string wName = "Vulkan Course")
{
	glfwInit();

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	window = glfwCreateWindow(width, height, wName.c_str(), nullptr, nullptr);
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
		//glfwPollEvents();
		glfwWaitEventsTimeout(0.01);
    }

	renderer.cleanup();

    glfwDestroyWindow(window);
    glfwTerminate();


    return 0;
}
