#pragma once

#include "vulkan/vulkan.h"
#include <vector>

namespace Window { class WindowHandler; }

namespace Renderer
{

class VkGraphic final
{
    public:
    VkGraphic(Window::WindowHandler* window);
    ~VkGraphic();

    private:

    void InitializeVulkan();
    void CreateInstance();
    void SetupDebugMessenger();

    // Extensions Properties checkers
    static std::vector<const char*> GetGLFWRequiredExtensions();
    static std::vector<VkExtensionProperties> GetSupportedInstanceExtensions();
    bool CheckSupportedExtensionProperties(std::vector<const char*> requiredExtensions);

    // Validation Layer checkers
    static std::vector<VkLayerProperties> GetSupportedValidationLayers();
    bool CheckSupportedValidationLayers(std::vector<const char*> requiredLayers);

    // Object Instances
    VkInstance vkInstance_ = nullptr; // Vulkan Instance
    Window::WindowHandler* windowPtr_; // Pointer to the instantiated GLFW Window

    // Debugging Instances
    bool debuggingEnabled_ = false; // Vulkan Validation Layer
    VkDebugUtilsMessengerEXT debugMessenger_; // Vulkan debug messenger
};

} // namespace Renderer


