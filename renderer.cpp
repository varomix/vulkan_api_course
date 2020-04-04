#include <string.h>

#include "validation.h"
#include "renderer.h"


Renderer::Renderer() {

}

int Renderer::init(GLFWwindow *newWindow) {
    window = newWindow;

    try {
        createInstance();
        createDebugCallback();
        createSurface();
        getPhysicalDevice();
        createLogicalDevice();

    } catch (std::runtime_error &e) {
        printf("ERROR: %s\n", e.what());
        return EXIT_FAILURE;
    }
    return 0;
}

void Renderer::cleanup() {
    vkDestroySurfaceKHR(instance, surface, nullptr);
    vkDestroyDevice(mainDevice.logicalDevice, nullptr);

    if(validationEnabled){
        DestroyDebugReportCallbackEXT(instance, callback, NULL);
    }

    vkDestroyInstance(instance, nullptr);
}

Renderer::~Renderer() {

}

void Renderer::createInstance() {
    if (validationEnabled && !checkValidationLayerSupport()) {
        throw std::runtime_error("Required Validation Layers not supported");
    }

    // for developer convenience
    VkApplicationInfo appInfo = {};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Vulkan App";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "No Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_1;

    VkInstanceCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;

    // create list to hold extensions
    std::vector<const char *> instanceExtensions = std::vector<const char *>();

    uint32_t glfwExtensionCount = 0;
    const char **glfwExtensions;    //Extensions passed as array of cstrings.

    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    // Add GLFW extensions to list of extensions
    for (size_t i = 0; i < glfwExtensionCount; i++) {
        instanceExtensions.push_back(glfwExtensions[i]);
    }

    // if validation enabled, add extension to report validation debug info
    if (validationEnabled) {
        instanceExtensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
    }

    if (!checkInstanceExtensionSupport(&instanceExtensions)) {
        throw std::runtime_error("vkInstance does not support required extension");
    }

    createInfo.enabledExtensionCount = static_cast<uint32_t>(instanceExtensions.size());
    createInfo.ppEnabledExtensionNames = instanceExtensions.data();

    if (validationEnabled) {
        createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
        createInfo.ppEnabledLayerNames = validationLayers.data();
    } else {
        createInfo.enabledLayerCount = 0;
        createInfo.ppEnabledLayerNames = nullptr;
    }

    // create instance
    VkResult result = vkCreateInstance(&createInfo, nullptr, &instance);

    if (result != VK_SUCCESS) {
        throw std::runtime_error("Failed to create a Vulkan Instance");
    }

}

void Renderer::createLogicalDevice() {
    // get the queue family indices for the chosen physical device
    QueueFamiliesIndices indices = getQueueFamilies(mainDevice.physicalDevice);

    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    std::set<int> queueFamilyIndices = {indices.graphicsFamily, indices.presentationFamily};


    // Queue the logical device needs to create and info to do so
    VkDeviceQueueCreateInfo queueCreateInfo = {};
    queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfo.queueFamilyIndex = indices.graphicsFamily;
    queueCreateInfo.queueCount = 1;
    float priority = 1.0f;
    queueCreateInfo.pQueuePriorities = &priority;

    // we don't need any layers here in vulkan 1.1
    // information to create logical device (sometimes called "device")
    VkDeviceCreateInfo deviceCreateInfo = {};
    deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceCreateInfo.queueCreateInfoCount = 1;
    deviceCreateInfo.pQueueCreateInfos = &queueCreateInfo;
    deviceCreateInfo.enabledExtensionCount = 0;
    deviceCreateInfo.ppEnabledExtensionNames = nullptr;

    VkPhysicalDeviceFeatures deviceFeatures = {};
    deviceCreateInfo.pEnabledFeatures = &deviceFeatures;

    VkResult result = vkCreateDevice(mainDevice.physicalDevice, &deviceCreateInfo, nullptr, &mainDevice.logicalDevice);
    if (result != VK_SUCCESS) {
        throw std::runtime_error("Failed to create a logical device");
    }

    // Queues are created at the same time as the device
    // so we want handle to queues
    vkGetDeviceQueue(mainDevice.logicalDevice, indices.graphicsFamily, 0, &graphicsQueue);

}

void Renderer::createSurface() {
    // Create Surface (create a surface create info struct, runs the create surface function
    VkResult result = glfwCreateWindowSurface(instance, window, nullptr, &surface);

    if (result != VK_SUCCESS) {
        throw std::runtime_error("Failed to create a surface");
    }
}

void Renderer::getPhysicalDevice() {
    // Enumerate physicla devices
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

    // if no devices available
    if (deviceCount == 0) {
        throw std::runtime_error("Can find GPU that supports Vulkan instance!");
    }

    std::vector<VkPhysicalDevice> deviceList(deviceCount);
    vkEnumeratePhysicalDevices(instance, &deviceCount, deviceList.data());

    for (const auto &device : deviceList) {
        if (checkDeviceSuitable(device)) {
            mainDevice.physicalDevice = device;
            break;
        }
    }
}

bool Renderer::checkInstanceExtensionSupport(std::vector<const char *> *checkExtensions) {
    // Need to get number of extensions to create array to correct size
    uint32_t extensionsCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionsCount, nullptr);

    // Create a list of extension properties
    std::vector<VkExtensionProperties> extensions(extensionsCount);
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionsCount, extensions.data());

    // check if given extensions are in a list of available extensions
    for (const auto &checkExtension: *checkExtensions) {
        bool hasExtension = false;
        for (const auto &extension: extensions) {
            if (strcmp(checkExtension, extension.extensionName) == 0) {
                hasExtension = true;
                break;
            }
        }

        if (!hasExtension) return false;
    }
    return true;
}

bool Renderer::checkValidationLayerSupport() {
    uint32_t validationLayerCount;
    vkEnumerateInstanceLayerProperties(&validationLayerCount, nullptr);

    // check if no validation layers found AND we want at least 1 layer
    if (validationLayerCount == 0 && validationLayers.size() > 0) {
        return false;
    }


    std::vector<VkLayerProperties> availableLayers(validationLayerCount);
    vkEnumerateInstanceLayerProperties(&validationLayerCount, availableLayers.data());

    // check if given validation layer is in list of given validation Layers
    for (const auto &validationLayer : validationLayers) {
        bool hasLayer = false;
        for (const auto &availableLayer : availableLayers) {
            if (strcmp(validationLayer, availableLayer.layerName) == 0) {
                hasLayer = true;
                break;
            }
        }

        if (!hasLayer) {
            return false;
        }
    }
    return true;
}

bool Renderer::checkDeviceSuitable(VkPhysicalDevice device) {

    // Information about the device(ID, name, type, vendor, etc.)
    VkPhysicalDeviceProperties deviceProperties;
    vkGetPhysicalDeviceProperties(device, &deviceProperties);
    printf("Device Properties %s \n Device type %u \n", deviceProperties.deviceName, deviceProperties.deviceType);

    // Information about what the device can do(geo shader, tess shader, wide lines, etc.
    VkPhysicalDeviceFeatures deviceFeatures;
    vkGetPhysicalDeviceFeatures(device, &deviceFeatures);


    QueueFamiliesIndices indices = getQueueFamilies(device);

    // getting the discreet GPU
    if(deviceProperties.deviceType == VkPhysicalDeviceType::VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
    {
        return indices.isValid();
    }
    return false;
}

QueueFamiliesIndices Renderer::getQueueFamilies(VkPhysicalDevice device) {
    QueueFamiliesIndices indices;

    // Get all queue family property info for the given device
    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilyList(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilyList.data());

    // Check if queue family has at least 1 queue
    int i = 0;
    for (const auto &queueFamily : queueFamilyList) {
        // first check if queue family has at least 1 queue in the family (could have no queues)
        // Queue can be multiple types defined through bitfield, Need to bitwse AND with VK_QUEUE_*_BIT to check if has required type
        if (queueFamily.queueCount > 0 && queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            indices.graphicsFamily = i;  // if queue family is valid, then get index
        }

        // check if queue family supports presentation
        VkBool32 presentationSupport = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentationSupport);

        if (queueFamily.queueCount > 0 && presentationSupport) {
            indices.presentationFamily = i;
        }

        // Check if queue family indices are in a valid state, stop searching if so.
        if (indices.isValid()) break;

        i++;
    }
    return indices;
}

void Renderer::createDebugCallback() {
    // only create callback if validation enabled
    if (!validationEnabled) return;

    VkDebugReportCallbackCreateInfoEXT callbackCreateInfo = {};
    callbackCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
    callbackCreateInfo.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT;
    callbackCreateInfo.pfnCallback = debugCallback;         // pointer to callback function

    // create debug callback with custom create function
    VkResult result = CreateDebugReportCallbackEXT(instance, &callbackCreateInfo, nullptr, &callback);

    if(result != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create debug callback");
    }
}


