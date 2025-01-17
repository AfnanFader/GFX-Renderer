#ifndef GRAPHIC_VULKAN_VULKANINITIALIZER_HPP
#define GRAPHIC_VULKAN_VULKANINITIALIZER_HPP
#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>
#include <WindowHandler.hpp>
#include <Global.hpp>
#include <Settings.hpp>
#include <vector>
#include <string>

// Mac workaroud
#ifndef VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES2_EXTENSION_NAME 
#define VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES2_EXTENSION_NAME "VK_KHR_get_physical_device_properties2" 
#endif

// Macro for Check Extensions properties
#define CheckVkSupportedExtProperties(requiredExtensions, availableExtensions, supportedExtensions)  \
    for (const char* required : requiredExtensions)                                                  \
    {                                                                                                \
        bool found = false;                                                                          \
        for (const auto& extension : availableExtensions)                                            \
        {                                                                                            \
            if (strcmp(required, extension.extensionName) == 0)                                      \
            {                                                                                        \
                LOG_INFO("VK Instance: Supported -> {}",extension.extensionName);                \
                supportedExtensions.push_back(required);                                             \
                found = true;                                                                        \
                break;                                                                               \
            }                                                                                        \
        }                                                                                            \
        if (!found)                                                                                  \
        {                                                                                            \
            LOG_WARN("VK Instance: Unsupported -> {}", required);                                \
        }                                                                                            \
    }                                                                                                \


namespace Graphic
{


// Required Vulkan Entensions.
static std::vector<const char*> requiredDevExt = {
#ifdef __APPLE__
    VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME,
    VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES2_EXTENSION_NAME,
#endif
    VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

// Required Validation Layers for debugging.
static std::vector<const char*> requiredValidationLayers = {
    "VK_LAYER_KHRONOS_validation",
};

// Error Callback enums
typedef enum VkDebugMessageSeverity {
    VERSBOSE = 0x00000001,
    INFO = 0x00000010,
    WARNING = 0x00000100,
    ERROR = 0x00001000,
    MAX = 0x7FFFFFFF
} VkDebugMessageSeverity;

//----------------------------------------------------------------------------//

struct SwapChainCapabilities
{
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};

struct QueueFamilyIndices
{
    uint32_t graphicsFamilyIdx;
    uint32_t presentFamilyIdex;
    bool graphicsFamilyHaxValue = false;
    bool presentFamilyHasValue = false;

    bool IsComplete()
    {
        return (graphicsFamilyHaxValue && presentFamilyHasValue);
    }
};

//----------------------------------------------------------------------------//

class VulkanInitializer
{

public:
    VulkanInitializer(WindowHandler* window);
    ~VulkanInitializer();

    // Move operators - prevent creating multiple copies
    VulkanInitializer(const VulkanInitializer&) = delete;
    VulkanInitializer(VulkanInitializer&&) = delete;
    VulkanInitializer &operator=(VulkanInitializer&&) = delete;
    void operator=(const VulkanInitializer&) = delete; 

private:
    void InitVulkan();
    void CreateSurface();
    void SetupDebugMessenger();
    void PickPhysicalDevice();

    // Instance creation functions
    void CreateInstance();
    std::vector<const char*> GetSupportedInstanceExtensions();
    bool CheckSupportedValidationLayers();
    
//----------------------------------------------------------------------------//

    WindowHandler* window_ = nullptr; // Window instance SDL or GLFW

    VkInstance instance_ = VK_NULL_HANDLE; // Vulkan Instance
    VkSurfaceKHR surfaceKHR_ = VK_NULL_HANDLE; // Surface to draw on

    VkPhysicalDevice physicalDevice_ = VK_NULL_HANDLE; // Physical GPU Instance
    VkDevice logicalDevice_ = VK_NULL_HANDLE; // Logical GPU instance

    // Debugging control
    bool debuggingEnabled_ = ENABLE_VULKAN_VALIDATION;
    VkDebugUtilsMessengerEXT debugMessenger_;
};

} // namespace Graphic

#endif