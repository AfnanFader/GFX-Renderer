#pragma once

#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"
#include "vulkan/vulkan.h"
#include "vulkan/vulkan_beta.h"

namespace Window { class WindowHandler; }

#ifndef VK_RENDERER_HPP // include guard
#define VK_RENDERER_HPP

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
                spdlog::info("VK Instance: Supported -> {}",extension.extensionName);                \
                supportedExtensions.push_back(required);                                             \
                found = true;                                                                        \
                break;                                                                               \
            }                                                                                        \
        }                                                                                            \
        if (!found)                                                                                  \
        {                                                                                            \
            spdlog::warn("VK Instance: Unsupported -> {}", required);                                \
        }                                                                                            \
    }                                                                                                \

namespace Renderer
{

// Struct to store QueueFamilyIndices for each Physical Devices/GPU
struct QueueFamilyIndices
{
    std::optional<uint32_t> graphicFamilyIdx;
    std::optional<uint32_t> presentFamilyIdx;

    bool IsComplete() const
    {
        return (graphicFamilyIdx.has_value() && presentFamilyIdx.has_value());
    }

    bool IsSame() const
    {
        return (graphicFamilyIdx == presentFamilyIdx);
    }
};

struct SwapChainProperties
{
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;

    bool IsValid() const
    {
        return (!formats.empty() && !presentModes.empty());
    }
};

class VkGraphic final
{
    public:
    VkGraphic(Window::WindowHandler* window);
    ~VkGraphic();

    private:

    // Initiallizer functions
    void InitializeVulkan();
    void CreateInstance();
    void SetupDebugMessenger();
    void PickPhysicalDevice();
    void CreateLogicalDeviceAndQueue(); 
    void CreateSurface();
    // void CreateSwapChain(); // Handles the frame queue to be on screen.



    std::vector<VkPhysicalDevice> GetAvailableDevices();
    bool IsPhysicalDeviceCompatible(VkPhysicalDevice device);

    QueueFamilyIndices GetQueueFamilyProperties(VkPhysicalDevice device);
    std::vector<VkQueueFamilyProperties> GetDeviceQueueFamilyProperties(VkPhysicalDevice device);

    std::vector<const char*> GetSupportedInstanceExtensions();

    bool CheckSupportedValidationLayers();

    std::vector<const char*> GetSupportedDeviceExtensions(VkPhysicalDevice device);


    SwapChainProperties GetSwapChainProperties(VkPhysicalDevice device);


    // Object Instances. Note that theses objects needs to be properly destroyed on de-scope.

    VkInstance vkInstance_ = nullptr; // Vulkan Instance
    Window::WindowHandler* windowPtr_; // Pointer to the instantiated GLFW Window

    VkPhysicalDevice physicalDevice_ = nullptr; // GPU Instance
    VkDevice logicalDevice_ = nullptr; // Logical GPU Binding
    VkQueue graphicQueue_ = nullptr; // Graphic Queue
    VkQueue presentQueue_ = nullptr; // KHR Presentation Qeueue

    VkSurfaceKHR surfaceKHR_ = nullptr; 

    // Debugging Instances
    bool debuggingEnabled_ = false; // Vulkan Validation Layer
    VkDebugUtilsMessengerEXT debugMessenger_; // Vulkan debug messenger
};

} // namespace Renderer

#endif // VK_RENDERER_HPP
