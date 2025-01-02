#include <algorithm>
#include "GLFW/glfw3.h"
#include "Precomp.hpp"
#include "VK_Renderer.hpp"
#include "VK_Utilities.hpp"

namespace Renderer
{

VkGraphic::VkGraphic(Window::WindowHandler* windowPtr) : windowPtr_(windowPtr)
{
    debuggingEnabled_ = EN_VULKAN_DBG;
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
    VkDebugUtilsMessengerCreateInfoEXT msgCreationInfo = PopulateDebugMessengerCreateInfo();
    std::vector<const char*> requiredExtensions = GetGLFWRequiredExtensions();
    std::vector<const char*> requiredLayers = {"VK_LAYER_KHRONOS_validation"};

    if (!CheckInstanceExtensionSupport(requiredExtensions))
    {
        std::exit(EXIT_FAILURE);
    }

    if (!CheckSupportedValidationLayers(requiredLayers))
    {
        debuggingEnabled_ = false; // Disable it here since it is not supported
    }
    else
    {
        // Only append this if Vulkan validation layer is supported.
        requiredExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }

    VkApplicationInfo appInfo = {};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pNext = nullptr;
    appInfo.pApplicationName = "GfxRenderer";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "VEng";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;

    VkInstanceCreateInfo instanceCreationInfo = {};
    instanceCreationInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instanceCreationInfo.pNext = debuggingEnabled_ ? &msgCreationInfo : nullptr;
    instanceCreationInfo.pApplicationInfo = &appInfo;
    instanceCreationInfo.ppEnabledExtensionNames = requiredExtensions.data();
    instanceCreationInfo.enabledExtensionCount = static_cast<uint32_t>(requiredExtensions.size());
    instanceCreationInfo.ppEnabledLayerNames = debuggingEnabled_ ? requiredLayers.data() : nullptr;
    instanceCreationInfo.enabledLayerCount = debuggingEnabled_ ? static_cast<uint32_t>(requiredLayers.size()) : 0;

    VkResult result = vkCreateInstance(&instanceCreationInfo, nullptr, &vkInstance_);

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
    for (const char* required : requiredExtensions)
    {
        bool found = false;
        for (const auto& extension : availableExtensions)
        {
            if (strcmp(required, extension.extensionName) == 0)
            {

                std::cout << "[INFO] Supported Extensions : " << extension.extensionName << std::endl;
                found = true;

                break;
            }
        }
        if (!found)
        {
            std::cerr << "Required extension not found: " << required << std::endl;
            return false;
        }
    }

    return true;
}

std::vector<VkLayerProperties> VkGraphic::GetSupportedValidationLayers()
{
    uint32_t layerCount = 0;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

    if (layerCount == 0) { return {}; }

    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());
    
    return availableLayers;
}

bool VkGraphic::CheckSupportedValidationLayers(std::vector<const char*> requiredLayers)
{
    std::vector<VkLayerProperties> availableLayers = GetSupportedValidationLayers();

    // Check if all required layers are available
    for (const char* required : requiredLayers)
    {
        bool found = false;
        for (const auto& layers : availableLayers)
        {
            if (strcmp(required, layers.layerName) == 0)
            {
                std::cout << "[INFO] Supported Validation Layers : " << layers.layerName << std::endl;
                found = true;

                break;
            }
        }
        if (!found)
        {
            std::cerr << "Required Validation Layers not found: " << required << std::endl;
            return false;
        }
    }

    return true;
}

} // namespace Renderer
