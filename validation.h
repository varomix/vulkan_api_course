#ifndef VALIDATION_H
#define VALIDATION_H

#include <vector>
#include <vulkan/vulkan.h>
#include <cstdio>

const bool validationEnabled = true;

const std::vector<const char *> validationLayers = {
        "VK_LAYER_LUNARG_standard_validation"
};

// callback function for validation debugging (will be called when validation
// information recorded)
static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
        VkDebugReportFlagsEXT flags,            // type of error
        VkDebugReportObjectTypeEXT objType,     // type of object causing error
        uint64_t obj,                         // ID of object
        size_t location,
        int32_t code,
        const char *layerPrefix,
        const char *message,                    // validation information
        void *userData)
{
    // if validation ERROR, then output error and return failure
    if (flags & VK_DEBUG_REPORT_ERROR_BIT_EXT) {
        printf("VALIDATION ERROR: %s\n", message);
        return VK_TRUE;
    }

    // if validation WARNING, then output warning and return okay
    if (flags & VK_DEBUG_REPORT_WARNING_BIT_EXT) {
        printf("VALIDATION WARNING: %s\n", message);
        return VK_FALSE;
    }

    return VK_FALSE;
}

static VkResult CreateDebugReportCallbackEXT(VkInstance instance, const VkDebugReportCallbackCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugReportCallbackEXT* pCallback)
{
    auto func = (PFN_vkCreateDebugReportCallbackEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugReportCallbackEXT");

    if (func != nullptr) {
        return func(instance, pCreateInfo, pAllocator, pCallback);
    } else {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

static void DestroyDebugReportCallbackEXT(VkInstance instance, VkDebugReportCallbackEXT callback, const VkAllocationCallbacks* pAllocator)
{
    // get function pointer to requested function
    auto func = (PFN_vkDestroyDebugReportCallbackEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugReportCallbackEXT");

    if (func != nullptr) {
        func(instance, callback, pAllocator);
    }
}


#endif // VALIDATION_H
