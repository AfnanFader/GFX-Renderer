#pragma once

#include <vulkan/vulkan.h>
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
    static std::vector<const char*> GetGLFWRequiredExtensions();
    static std::vector<VkExtensionProperties> GetSupportedInstanceExtensions();
    static bool CheckInstanceExtensionSupport(std::vector<const char*> requiredExtensions);

    // Object data members.
    VkInstance vkInstance_ = nullptr;
    Window::WindowHandler* windowPtr_;
};

} // namespace Renderer


