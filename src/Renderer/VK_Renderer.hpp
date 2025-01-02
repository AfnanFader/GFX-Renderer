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

    // Extensions Properties checkers
    static std::vector<const char*> GetGLFWRequiredExtensions();
    static std::vector<VkExtensionProperties> GetSupportedInstanceExtensions();
    bool CheckInstanceExtensionSupport(std::vector<const char*> requiredExtensions);

    // Validation Layer checkers
    static std::vector<VkLayerProperties> GetSupportedValidationLayers();
    bool CheckSupportedValidationLayers(std::vector<const char*> requiredLayers);

    VkInstance vkInstance_ = nullptr; // Vulkan Instance
    Window::WindowHandler* windowPtr_; // Pointer to the instantiated GLFW Window
    bool debuggingEnabled_ = false; // Vulkan Validation Layer
};

} // namespace Renderer


