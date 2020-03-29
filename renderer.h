#ifndef RENDERER_H
#define RENDERER_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <stdexcept>
#include <vector>

#include "utilities.h"

class Renderer
{
public:
	Renderer();

	int init(GLFWwindow *newWindow);
	void cleanup();

	~Renderer();
private:
	GLFWwindow *window;

	// Vulkan components
	VkInstance instance;
	struct{
		VkPhysicalDevice physicalDevice;
		VkDevice logicalDevice;
	}mainDevice;

	VkQueue graphicsQueue;

	// Vulkan functions
	// - Create Functions
	void createInstance();
	void createLogicalDevice();

	// Get Functions
	void getPhysicalDevice();

	// - Support Functions
	// -- Checker Functions
	bool checkInstanceExtensionSupport(std::vector<const char*> *checkExtensions);
	bool checkDeviceSuitable(VkPhysicalDevice device);

	// -- Getter Functions
	QueueFamiliesIndices getQueueFamilies(VkPhysicalDevice device);
};

#endif // RENDERER_H
