#ifndef GRAPHIC_VULKAN_VKINSTANCEIMPL_HPP
#define GRAPHIC_VULKAN_VKINSTANCEIMPL_HPP
#pragma once

#include <Graphics/WindowHandler.hpp>
#include <Global.hpp>
#include <Settings.hpp>

// External Lib
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>
#ifdef __APPLE__
#include <vulkan/vulkan_beta.h>
#endif

// STD Lib
#include <vector>
#include <string>

// Forward Declarations
namespace Graphic { struct QueueFamilyIndices; }
namespace Graphic { struct SwapChainCapabilities; }

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
                supportedExtensions.push_back(required);                                             \
                found = true;                                                                        \
                break;                                                                               \
            }                                                                                        \
        }                                                                                            \
        if (!found)                                                                                  \
        {                                                                                            \
            LOG_WARN("VK Instance: Unsupported -> {}", required);                                    \
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

//----------------------------------------------------------------------------//

// Constants
constexpr uint32_t INVALID_VK_EXTENT = std::numeric_limits<uint32_t>::max();

// Error Callback enums
typedef enum VkDebugMessageSeverity {
    VERSBOSE = 0x00000001,
    INFO = 0x00000010,
    WARNING = 0x00000100,
    ERROR = 0x00001000,
    MAX = 0x7FFFFFFF
} VkDebugMessageSeverity;

//----------------------------------------------------------------------------//

class VkDeviceInstance
{

public:
    VkDeviceInstance(WindowHandler* window);
    ~VkDeviceInstance();

    // Move operators - prevent creating multiple copies
    VkDeviceInstance(const VkDeviceInstance&) = delete;
    VkDeviceInstance(VkDeviceInstance&&) = delete;
    VkDeviceInstance& operator=(VkDeviceInstance&&) = delete;
    VkDeviceInstance& operator=(const VkDeviceInstance&) = delete; 

    // @todo Might need to move these to ipp
    VkPhysicalDevice GetPhyDevice() { return physicalDevice_; }
    VkDevice GetLogicalDevice() { return logicalDevice_; }
    VkCommandPool GetCommandPool() { return commandPool_; }
    VkSurfaceKHR GetSurface() { return surfaceKHR_; }
    VkQueue GetGraphicsQ() { return graphicsQueue_; }
    VkQueue GetPresentQ() { return presentQueue_; }

    uint32_t FindMemoryType(uint32_t typeFiler, VkMemoryPropertyFlags properties);

    VkFormat FindSupportedFormat(
        const std::vector<VkFormat>& candidate,
        VkImageTiling tiling,
        VkFormatFeatureFlags features);

    void CreateBuffer(
        VkDeviceSize devSize,
        VkBufferUsageFlags usage,
        VkMemoryPropertyFlags properties,
        VkBuffer& buffer,
        VkDeviceMemory& bufferMem);
    
    VkCommandBuffer BeginSingleTimeCommands();
    
    void EndSingleTimeCommands(VkCommandBuffer cmdBuffer);

    void CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize devSize);

    void CopyBufferToImage(
        VkBuffer buffer,
        VkImage image,
        uint32_t width,
        uint32_t height,
        uint32_t layerCount);
    
    void CreateImageWithInfo(
        const VkImageCreateInfo& imageInfo,
        VkMemoryPropertyFlags properties,
        VkImage& image,
        VkDeviceMemory& imageMem);

private:
    // Main Initializers
    void InitVulkan();

    // Instance creation functions
    void CreateInstance();
    std::vector<const char*> GetSupportedInstanceExtensions();
    bool CheckSupportedValidationLayers();

    // Debugging/Validation setup functions
    void SetupDebugMessenger();

    // Create Surface functions
    void CreateSurface();

    // Physical Device selections functions.
    void PickPhysicalDevice();
    bool IsDeviceCompatible(VkPhysicalDevice device);
    std::vector<const char*> GetSupportedDeviceExtensions(VkPhysicalDevice device);
    
    // Bind Physical Device into a logical abstract
    void CreateLogicalDeviceAndQueue();

    // Command Pool creation
    void CreateCommandPool();

//----------------------------------------------------------------------------//

    WindowHandler* window_ = nullptr; // Window instance SDL or GLFW

    VkInstance instance_ = VK_NULL_HANDLE; // Vulkan Instance
    VkSurfaceKHR surfaceKHR_ = VK_NULL_HANDLE; // Surface to draw on

    VkPhysicalDevice physicalDevice_ = VK_NULL_HANDLE; // Physical GPU Instance
    VkPhysicalDeviceProperties phyDevProperties_ = {}; // Physical Device Properties

    VkDevice logicalDevice_ = VK_NULL_HANDLE; // Logical GPU instance
    VkQueue graphicsQueue_ = VK_NULL_HANDLE;
    VkQueue presentQueue_ = VK_NULL_HANDLE;

    VkSwapchainKHR swapChainInst_ = VK_NULL_HANDLE;
    
    VkCommandPool commandPool_ = VK_NULL_HANDLE;

    // Debugging control
    bool debuggingEnabled_ = ENABLE_VULKAN_VALIDATION;
    VkDebugUtilsMessengerEXT debugMessenger_;
};

} // namespace Graphic

#endif