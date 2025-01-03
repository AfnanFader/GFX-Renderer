#pragma once

#include "vulkan/vulkan.h"
#include <vector>

namespace Window { class WindowHandler; }

namespace Renderer
{

// Struct to store QueueFamilyIndices for each Physical Devices / GPU
struct QueueFamilyIndices
{
    std::optional<uint32_t> graphicFamily = std::nullopt;
    std::optional<uint32_t> presentationFamily = std::nullopt;

    bool isValid() const
    {
        return (graphicFamily.has_value() /*&& presentationFamily.has_value()*/);
    }
};

class VkGraphic final
{
    public:
    VkGraphic(Window::WindowHandler* window);
    ~VkGraphic();

    private:

    // Initializer functions
    void InitializeVulkan();
    void CreateInstance();
    void SetupDebugMessenger();
    void PickPhysicalDevice();
    void CreateLogicalDeviceAndQueue();

    // Extensions Properties handlers
    static std::vector<const char*> GetGLFWRequiredExtensions();
    static std::vector<VkExtensionProperties> GetSupportedInstanceExtensions();
    bool CheckSupportedExtensionProperties(std::vector<const char*> requiredExtensions);

    // Validation Layer handlers
    static std::vector<VkLayerProperties> GetSupportedValidationLayers();
    bool CheckSupportedValidationLayers(std::vector<const char*> requiredLayers);

    // Physical Device handlers
    std::vector<VkPhysicalDevice> GetAvailableDevices();
    bool IsPhysicalDeviceCompatible(VkPhysicalDevice device);
    std::vector<VkQueueFamilyProperties> GetDeviceQueueFamilyProperties(VkPhysicalDevice device);
    // QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device); <-- To be evalutated

    // Object Instances. Note that theses objects needs to be properly destroyed on de-scope.
    // TODO: May need to store some extra values in the future.
    VkInstance vkInstance_ = nullptr; // Vulkan Instance
    Window::WindowHandler* windowPtr_; // Pointer to the instantiated GLFW Window
    VkPhysicalDevice physicalDevice_ = nullptr; // GPU Instance
    VkDevice logicalDevice_ = nullptr; // Logical GPU Binding
    VkQueue graphicQueue_ = nullptr; // GPU Queue Instance/Access

    // Debugging Instances
    bool debuggingEnabled_ = false; // Vulkan Validation Layer
    VkDebugUtilsMessengerEXT debugMessenger_; // Vulkan debug messenger
};

} // namespace Renderer


