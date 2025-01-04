#include "Precomp.hpp"
#include "VK_Renderer.ipp"
#include "VK_Utilities.hpp"
#include "spdlog/spdlog.h"
#include "Glfw_Window.hpp"
#include <optional>
#include <set>

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

// Required Vulkan Entensions.
static std::vector<const char*> requiredDevExt = {
#ifdef __APPLE__
    VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME,
    VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES2_EXTENSION_NAME,
#endif
    VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

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
        if (surfaceKHR_ != nullptr)
        {
            vkDestroySurfaceKHR(vkInstance_, surfaceKHR_, nullptr);            
            spdlog::info("VK Instance: Terminate Surface KHR");
        }

        if (logicalDevice_ != nullptr)
        {
            spdlog::info("VK Instance: Terminate Logical Device");
            vkDestroyDevice(logicalDevice_, nullptr);
        }

        if ((debugMessenger_ != nullptr) || debuggingEnabled_)
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
    SetupDebugMessenger(); // Note that this is optional for now
    CreateSurface();
    PickPhysicalDevice();
    // CreateLogicalDeviceAndQueue();
    // CreateSwapChain();
}

void VkGraphic::CreateInstance()
{
    VkDebugUtilsMessengerCreateInfoEXT msgCreationInfo = GetDebugMessengerCreateInfo();
    std::vector<const char*> requiredExtensions = GetGLFWRequiredExtensions();
    std::vector<const char*> requiredLayers = {"VK_LAYER_KHRONOS_validation"};

#ifdef __APPLE__
    // MAC specific extensions
    requiredExtensions.push_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
    spdlog::warn("VK Instance: Vulkan Portability Enumeration Extension Added");
#endif

    requiredExtensions = GetAvailableInstanceExtProperties(requiredExtensions);

    if (requiredExtensions.empty())
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

#ifdef __APPLE__
    // This flag is require for MoltenVK
    instanceCreationInfo.flags = VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
    spdlog::warn("VK Instance: Enumerate Portability Bit for MoltenVK");
#endif

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

std::vector<VkExtensionProperties> VkGraphic::GetSupportedInstanceExtensions()
{
    uint32_t extensionCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

    if (extensionCount == 0) { return {}; }

    std::vector<VkExtensionProperties> properties(extensionCount);
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, properties.data());

    return properties;
}

std::vector<const char*> VkGraphic::GetAvailableInstanceExtProperties(std::vector<const char*> requiredExtensions)
{
    std::vector<VkExtensionProperties> availableExtensions = GetSupportedInstanceExtensions();
    std::vector<const char*> supportedExt;

    // Check if all required extensions are available
    for (const char* required : requiredExtensions)
    {
        bool found = false;
        for (const auto& extension : availableExtensions)
        {
            if (strcmp(required, extension.extensionName) == 0)
            {
                spdlog::info("VK Instance: Supported Instance Extensions -> {}",extension.extensionName);
                supportedExt.push_back(required);
                found = true;
                break;
            }
        }

        if (!found)
        {
            spdlog::warn("VK Instance: Unsupported Instance Extension -> {}", required);
        }
    }

    return supportedExt;
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

void VkGraphic::PickPhysicalDevice()
{
    std::vector<VkPhysicalDevice> availPhysicalDevices = GetAvailableDevices();

    // Filter out non compatible devices.
    for (uint32_t index = 0; index < availPhysicalDevices.size(); ++index)
    {
        if (!IsPhysicalDeviceCompatible(availPhysicalDevices[index]))
        {
            availPhysicalDevices.erase(availPhysicalDevices.begin() + index);
        }
    }

    if (availPhysicalDevices.empty())
    {
        spdlog::critical("VK Instance: No available GPU to be binded.");
        std::exit(EXIT_FAILURE);
    }

    // Future logic for choosing with GPU

    physicalDevice_ = availPhysicalDevices[0];
}

bool VkGraphic::IsPhysicalDeviceCompatible(VkPhysicalDevice device)
{
    return (GetQueueFamilyProperties(device).IsComplete() &&
            !GetAvailableDeviceExtension(device).empty() &&
            GetSwapChainProperties(device).IsValid());
}

std::vector<VkExtensionProperties> VkGraphic::GetSupportedDeviceExtesions(VkPhysicalDevice device)
{
    uint32_t allDevExtCount = 0;
    vkEnumerateDeviceExtensionProperties(device, nullptr, &allDevExtCount, nullptr);

    std::vector<VkExtensionProperties> supportedExtProperties(allDevExtCount);
    vkEnumerateDeviceExtensionProperties(device, nullptr, &allDevExtCount, supportedExtProperties.data());

    return supportedExtProperties;
}

std::vector<const char*> VkGraphic::GetAvailableDeviceExtension(VkPhysicalDevice device)
{
    std::vector<VkExtensionProperties> supportedDevExt = GetSupportedDeviceExtesions(device);
    std::vector<const char*> availableDevExt;

    for (auto reqDevExt : requiredDevExt)
    {
        bool found = false;
        for (auto supDevExt : supportedDevExt)
        {
            if (strcmp(reqDevExt, supDevExt.extensionName) == 0)
            {
                availableDevExt.push_back(reqDevExt);
                spdlog::info("VK Instance: Supported Device Extension -> {}",reqDevExt);
                found = true;
            }
        }

        if (!found)
        {
            spdlog::warn("VK Instance: Device Extension Non Supported -> {}",reqDevExt);
        }
    }
    
    return availableDevExt;
}

QueueFamilyIndices VkGraphic::GetQueueFamilyProperties(VkPhysicalDevice device)
{
    bool deviceSupported = false;
    VkBool32 presentationSupport = false;

    std::vector<VkQueueFamilyProperties> familiesProperties = GetDeviceQueueFamilyProperties(device);

    QueueFamilyIndices queueFamilyIndices = {};
    for (uint32_t i = 0; i < familiesProperties.size(); ++i)
    {
        VkBool32 presentationSupport = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(device, i ,surfaceKHR_, &presentationSupport);

        if (familiesProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
        {
            queueFamilyIndices.graphicFamilyIdx = i;
            spdlog::info("VK Instance: graphicFamilyIdx -> {}",i);
        }

        if (presentationSupport)
        {
            queueFamilyIndices.presentFamilyIdx = i;
            spdlog::info("VK Instance: PresentFamilyIdx -> {}",i);
        }

        if (queueFamilyIndices.IsComplete())
        {
            break;
        }
    }

    return queueFamilyIndices;
}

std::vector<VkPhysicalDevice> VkGraphic::GetAvailableDevices()
{
    uint32_t phyDevCount = 0;
    vkEnumeratePhysicalDevices(vkInstance_, &phyDevCount, nullptr);

    if (phyDevCount == 0) { return {}; }; 

    std::vector<VkPhysicalDevice> availPhysicalDevices(phyDevCount);
    vkEnumeratePhysicalDevices(vkInstance_, &phyDevCount, availPhysicalDevices.data());

    return availPhysicalDevices;
}

void VkGraphic::CreateLogicalDeviceAndQueue()
{
    QueueFamilyIndices familyIndices = GetQueueFamilyProperties(physicalDevice_);
    std::vector<const char*> availableDevExt = GetAvailableDeviceExtension(physicalDevice_);
    std::set<uint32_t> uniqueQueueFamilies = {familyIndices.graphicFamilyIdx.value(), familyIndices.presentFamilyIdx.value()};
    std::vector<VkDeviceQueueCreateInfo> queueCreateList;
    float queuePriority = 1.0f;

    for (uint32_t queueFamilyIdx : uniqueQueueFamilies)
    {
        VkDeviceQueueCreateInfo queueCreateInfo = {};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = queueFamilyIdx;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;
        queueCreateInfo.pNext = nullptr;
        queueCreateList.push_back(queueCreateInfo);
    }

    VkPhysicalDeviceFeatures phyDevFeatures = {};

    VkDeviceCreateInfo deviceCreateInfo = {};
    deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateList.size());
    deviceCreateInfo.pQueueCreateInfos = queueCreateList.data();
    deviceCreateInfo.pEnabledFeatures = &phyDevFeatures;
    deviceCreateInfo.enabledExtensionCount = availableDevExt.size();
    deviceCreateInfo.ppEnabledExtensionNames = availableDevExt.data();

    VkResult result = vkCreateDevice(physicalDevice_, &deviceCreateInfo, nullptr, &logicalDevice_);

    if (result != VK_SUCCESS)
    {
        spdlog::error("VK Instance: Logical Device creation failed.");
        return;
    }

    vkGetDeviceQueue(logicalDevice_, familyIndices.graphicFamilyIdx.value(), 0, &graphicQueue_);

    if (familyIndices.IsSame())
    {
        // In some cases both Graphic Queue and Present Queue might be the same.
        presentQueue_ = graphicQueue_;
        spdlog::warn("VK Instance: GraphicQ and PresentQ are the same");
    }
    else
    {
        vkGetDeviceQueue(logicalDevice_, familyIndices.presentFamilyIdx.value(), 0, &presentQueue_);
    }
}

void VkGraphic::CreateSurface()
{
    VkResult result = glfwCreateWindowSurface(vkInstance_, windowPtr_->GetWindowHandlerPointer(), nullptr, &surfaceKHR_);

    if (result != VK_SUCCESS)
    {
        spdlog::error("VK Instance: Failed to create surface.");
        return;
    }
}

SwapChainProperties VkGraphic::GetSwapChainProperties(VkPhysicalDevice device)
{
    SwapChainProperties properties;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surfaceKHR_, &properties.capabilities);

    uint32_t formatCount = 0;
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surfaceKHR_, &formatCount, nullptr);
    properties.formats.resize(formatCount);
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surfaceKHR_, &formatCount, properties.formats.data());
    
    uint32_t presentModeCount = 0;
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, surfaceKHR_, &presentModeCount, nullptr);
    properties.presentModes.resize(presentModeCount);
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, surfaceKHR_, &presentModeCount, properties.presentModes.data());

    if ((formatCount == 0) && (presentModeCount == 0))
    {
        spdlog::warn("VK Instance: Null SwapChainProperties");
    }

    return properties;
}

// VkSurfaceFormatKHR ChooseSwapSurfaceFormat()
// {

// }

// void VkGraphic::CreateSwapChain()
// {
//     SwapChainProperties properties = GetSwapChainProperties(physicalDevice_);

    
// }

} // namespace Renderer
