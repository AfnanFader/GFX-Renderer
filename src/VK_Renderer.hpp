#pragma once

#include <vulkan/vulkan.h>

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
    static std::pair<char const **, uint32_t> GetSuggestedExtension();
    static std::vector<VkExtensionProperties> GetSupportedInstanceExtensions();

    // Object data members.
    VkInstance vkInstance_ = nullptr;
    Window::WindowHandler* windowPtr_;
};

} // namespace Renderer
