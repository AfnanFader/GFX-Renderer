#pragma once

#include "vulkan/vulkan.h"
#include <vector>
#include <array>

namespace Window { class WindowHandler; }

namespace Renderer
{

// Required Vulkan Entensions.
const std::array<const char*,1> requiredDeviceExtension = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME
    // Add when needed
};

// Struct to store QueueFamilyIndices for each Physical Devices/GPU
struct QueueFamilyIndices
{
    std::string deviceName;
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

    // Extensions Properties handlers
    static std::vector<const char*> GetGLFWRequiredExtensions();
    static std::vector<VkExtensionProperties> GetSupportedInstanceExtensions();
    bool CheckSupportedExtensionProperties(std::vector<const char*> requiredExtensions);

    // Validation Layer handlers
    static std::vector<VkLayerProperties> GetSupportedValidationLayers();
    bool CheckSupportedValidationLayers(std::vector<const char*> requiredLayers);

    // Physical Device handlers
    std::vector<VkPhysicalDevice> GetAvailableDevices();
    void PopulateFamilyIndices();
    bool IsPhysicalDeviceCompatible(VkPhysicalDevice device);
    bool CheckQueueFamilyProperties(VkPhysicalDevice device);
    std::vector<VkQueueFamilyProperties> GetDeviceQueueFamilyProperties(VkPhysicalDevice device);
    VkPhysicalDeviceProperties GetPhysicalDeviceProperties(VkPhysicalDevice device);
    SwapChainProperties GetSwapChainProperties(VkPhysicalDevice device);

    // Logical Device handlers
    std::vector<VkExtensionProperties> GetSupportedDeviceExtesions(VkPhysicalDevice device);
    bool AreAllDeviceExtensionSupported(VkPhysicalDevice device);

    // Object Instances. Note that theses objects needs to be properly destroyed on de-scope.

    VkInstance vkInstance_ = nullptr; // Vulkan Instance
    Window::WindowHandler* windowPtr_; // Pointer to the instantiated GLFW Window

    VkPhysicalDevice physicalDevice_ = nullptr; // GPU Instance
    QueueFamilyIndices familyIndices_ = {}; // Struct containing indexes for GPU Queues

    VkDevice logicalDevice_ = nullptr; // Logical GPU Binding
    VkQueue graphicQueue_ = nullptr; // Graphic Queue
    VkQueue presentQueue_ = nullptr; // KHR Presentation Qeueue

    VkSurfaceKHR surfaceKHR_ = nullptr; 

    // Debugging Instances
    bool debuggingEnabled_ = false; // Vulkan Validation Layer
    VkDebugUtilsMessengerEXT debugMessenger_; // Vulkan debug messenger
};

} // namespace Renderer


