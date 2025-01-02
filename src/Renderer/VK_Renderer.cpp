#include <algorithm>
#include "GLFW/glfw3.h"
#include "Precomp.hpp"
#include "VK_Renderer.hpp"

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
    std::vector<const char*> requiredExtensions = GetGLFWRequiredExtensions();

    if (!CheckInstanceExtensionSupport(requiredExtensions))
    {
        std::exit(EXIT_FAILURE);
    }

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
    instance_creation_info.enabledExtensionCount = static_cast<uint32_t>(requiredExtensions.size());
    instance_creation_info.ppEnabledExtensionNames = requiredExtensions.data();
    instance_creation_info.enabledLayerCount = 0;

    VkResult result = vkCreateInstance(&instance_creation_info, nullptr, &vkInstance_);

    if (result != VK_SUCCESS)
    {
        std::exit(EXIT_FAILURE);
    }
}

std::vector<const char*> VkGraphic::GetGLFWRequiredExtensions() {
    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions;

    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    return std::vector<const char*>(glfwExtensions, glfwExtensions + glfwExtensionCount);
}

std::vector<VkExtensionProperties> VkGraphic::GetSupportedInstanceExtensions()
{
    uint32_t extensionCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

    if (extensionCount == 0) { return {}; }

    std::vector<VkExtensionProperties> properties(extensionCount);
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, properties.data());

    return properties;
}


bool VkGraphic::CheckInstanceExtensionSupport(std::vector<const char*> requiredExtensions)
{
    std::vector<VkExtensionProperties> availableExtensions = GetSupportedInstanceExtensions();

    // Check if all required extensions are available
    for (const char* required : requiredExtensions) {
        bool found = false;
        for (const auto& extension : availableExtensions) {
            if (strcmp(required, extension.extensionName) == 0) {

                std::cout << "[INFO] Supported Extensions : " << extension.extensionName << std::endl;
                found = true;

                break;
            }
        }
        if (!found) {
            std::cerr << "Required extension not found: " << required << std::endl;
            return false;
        }
    }

    return true;
}

} // namespace Renderer
