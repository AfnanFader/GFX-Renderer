#include <GLFW/glfw3.h>
#include <Precomp.hpp>
#include <VK_Renderer.hpp>

namespace Renderer
{

VkGraphic::VkGraphic(Window::WindowHandler* windowPtr) : windowPtr_(windowPtr)
{
    InitializeVulkan();
}

VkGraphic::~VkGraphic()
{
    std::cout << "[INFO] Destroy VkInstance" << std::endl; 
    vkDestroyInstance(vkInstance_, nullptr);
}

void VkGraphic::InitializeVulkan()
{
    CreateInstance();
}

void VkGraphic::CreateInstance()
{

    std::pair<char const **,uint32_t> suggested_extensions = GetSuggestedExtension();

    VkApplicationInfo app_info = {};
    app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    app_info.pNext = nullptr;
    app_info.pApplicationName = "GfxRenderer";
    app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    app_info.pEngineName = "VEng";
    app_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    app_info.apiVersion = VK_API_VERSION_1_0;

    VkInstanceCreateInfo instance_creation_info = {};
    instance_creation_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instance_creation_info.pNext = nullptr;
    instance_creation_info.pApplicationInfo = &app_info;
    instance_creation_info.enabledExtensionCount = suggested_extensions.second;
    instance_creation_info.ppEnabledExtensionNames = suggested_extensions.first;
    instance_creation_info.enabledLayerCount = 0;

    VkResult result = vkCreateInstance(&instance_creation_info, nullptr, &vkInstance_);

    if (result != VK_SUCCESS)
    {
        std::exit(EXIT_FAILURE);
    }
}

std::pair<char const **,uint32_t> VkGraphic::GetSuggestedExtension()
{
    uint32_t glfwEntensionCount = 0;
    char const ** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwEntensionCount);

    return std::make_pair(glfwExtensions, glfwEntensionCount);
}

} // namespace Renderer
