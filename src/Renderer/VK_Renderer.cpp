#include <algorithm>
#include "GLFW/glfw3.h"
#include "Precomp.hpp"
#include "VK_Renderer.hpp"
#include "VK_Utilities.hpp"
#include "spdlog/spdlog.h"


VKAPI_ATTR VkResult VKAPI_CALL vkCreateDebugUtilsMessengerEXT(
    VkInstance instance,
    const VkDebugUtilsMessengerCreateInfoEXT* pInfo,
    const VkAllocationCallbacks* pAllocator,
    VkDebugUtilsMessengerEXT* debugMessenger)
{
    PFN_vkCreateDebugUtilsMessengerEXT func =
    reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT"));

    if (func != nullptr)
    {
        return func(instance, pInfo, pAllocator, debugMessenger);
    }
    else
    {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

VKAPI_ATTR void VKAPI_CALL vkDestroyDebugUtilsMessengerEXT(
    VkInstance instance,
    VkDebugUtilsMessengerEXT debugMessenger,
    const VkAllocationCallbacks* pAllocator)
{
    PFN_vkDestroyDebugUtilsMessengerEXT func =
    reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT"));

    if (func != nullptr)
    {
        return func(instance, debugMessenger, pAllocator);
    }
}

namespace Renderer
{

VkGraphic::VkGraphic(Window::WindowHandler* windowPtr) : windowPtr_(windowPtr)
{
    debuggingEnabled_ = EN_VULKAN_DBG;
    InitializeVulkan();
}

VkGraphic::~VkGraphic()
{
    if (vkInstance_ != nullptr)
    {
        if (debugMessenger_ != nullptr)
        {
            spdlog::info("VK Instance: Terminate DebugMessenger");
            vkDestroyDebugUtilsMessengerEXT(vkInstance_, debugMessenger_, nullptr);
        }

        spdlog::info("VK Instance: Terminate VkInstance");
        vkDestroyInstance(vkInstance_, nullptr);
    }

}

void VkGraphic::InitializeVulkan()
{
    CreateInstance();
    SetupDebugMessenger();
}

void VkGraphic::CreateInstance()
{
    VkDebugUtilsMessengerCreateInfoEXT msgCreationInfo = GetDebugMessengerCreateInfo();
    std::vector<const char*> requiredExtensions = GetGLFWRequiredExtensions();
    std::vector<const char*> requiredLayers = {"VK_LAYER_KHRONOS_validation"};

    if (!CheckSupportedExtensionProperties(requiredExtensions))
    {
        spdlog::critical("VK Instance: No supported Extension properties found");
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
        spdlog::warn("VK Instance: Vulkan Validation/Debugging mode enabled");
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
        spdlog::critical("VK Instance: Failed to initialize instance");
        std::exit(EXIT_FAILURE);
    }
}

void VkGraphic::SetupDebugMessenger()
{
    if (!debuggingEnabled_) { return; }

    VkDebugUtilsMessengerCreateInfoEXT msgCreationInfo = GetDebugMessengerCreateInfo();
    VkResult result = vkCreateDebugUtilsMessengerEXT(vkInstance_, &msgCreationInfo, nullptr, &debugMessenger_);

    if (result != VK_SUCCESS)
    {
        spdlog::error("VK Instance: Debug Messenger setup failed ..");
        return;
    }
}

std::vector<const char*> VkGraphic::GetGLFWRequiredExtensions()
{
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

bool VkGraphic::CheckSupportedExtensionProperties(std::vector<const char*> requiredExtensions)
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
                spdlog::info("VK Instance: Supported Extensions -> {}",extension.extensionName);
                found = true;

                break;
            }
        }
        if (!found)
        {
            spdlog::error("VK Instance: Required extension not found");
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
                spdlog::info("VK Instance: Supported Validation Layers -> {}",layers.layerName);
                found = true;

                break;
            }
        }
        if (!found)
        {
            spdlog::error("VK Instance: Required Validation Layers not found");
            return false;
        }
    }

    return true;
}

} // namespace Renderer
