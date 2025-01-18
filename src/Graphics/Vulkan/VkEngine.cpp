#include <Graphics/Vulkan/VkEngine.hpp>
#include <Graphics/WindowHandler.hpp>
#include <glm/glm.hpp>
#include <Logging.hpp>

// std
#include <algorithm>
#include <set>
#include <iostream>

namespace Graphic
{

// Logging message callback to be used in Vulkan Instance.
VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                             VkDebugUtilsMessageTypeFlagsEXT messageType,
                                             const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
                                             void* pUserData)
{
    switch(static_cast<VkDebugMessageSeverity>(messageSeverity))
    {
        case VERSBOSE:
        case INFO:
            LOG_INFO("VK Validation: {}",pCallbackData->pMessage);
            break;
        case WARNING:
            LOG_WARN("VK Validation: {}",pCallbackData->pMessage);
            break;
        case ERROR:
            LOG_ERROR("VK Validation: {}",pCallbackData->pMessage);
            break;
        default:
            LOG_ERROR_EXIT("UNKNOWN VK MESSAGE: {}",pCallbackData->pMessage);
    }

    return VK_FALSE;
}

VKAPI_ATTR VkResult VKAPI_CALL CreateDebugUtilsMessengerEXT(
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

VKAPI_ATTR void VKAPI_CALL DestroyDebugUtilsMessengerEXT(
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

//----------------------------------------------------------------------------//

// Types of Validation/Debug messages to be subscribed.
inline VkDebugUtilsMessengerCreateInfoEXT GetDebugMessengerCreateInfo()
{
    VkDebugUtilsMessengerCreateInfoEXT msgCreationInfo = {};
    msgCreationInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    msgCreationInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | 
                                      VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT |
                                      VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT;
    msgCreationInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                                  VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT;
    msgCreationInfo.pfnUserCallback = DebugCallback;
    msgCreationInfo.pUserData = nullptr;

    return msgCreationInfo;
}

inline std::vector<const char*> GetGLFWRequiredExtensions()
{
    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions;

    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    return std::vector<const char*>(glfwExtensions, glfwExtensions + glfwExtensionCount);
}

inline std::vector<VkExtensionProperties> GetAvailableInstanceExtensions()
{
    uint32_t extensionCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

    if (extensionCount == 0) { return {}; }

    std::vector<VkExtensionProperties> availableProperties(extensionCount);
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, availableProperties.data());

    return availableProperties;
}

inline std::vector<VkLayerProperties> GetAvailableValidationLayers()
{
    uint32_t layerCount = 0;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

    if (layerCount == 0) { return {}; }

    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());
    
    return availableLayers;
}

inline std::vector<VkExtensionProperties> GetAvailableDeviceExtensions(VkPhysicalDevice device)
{
    uint32_t allDevExtCount = 0;
    vkEnumerateDeviceExtensionProperties(device, nullptr, &allDevExtCount, nullptr);

    if (allDevExtCount == 0) { return {}; }

    std::vector<VkExtensionProperties> availableProperties(allDevExtCount);
    vkEnumerateDeviceExtensionProperties(device, nullptr, &allDevExtCount, availableProperties.data());

    return availableProperties;
}

inline std::vector<VkQueueFamilyProperties> GetDeviceQueueFamilyProperties(VkPhysicalDevice device)
{
    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);
    std::vector<VkQueueFamilyProperties> familiesProperties(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, familiesProperties.data());

    return familiesProperties;
}

inline std::vector<VkPhysicalDevice> GetAvailableDevices(VkInstance instance)
{
    uint32_t phyDevCount = 0;
    vkEnumeratePhysicalDevices(instance, &phyDevCount, nullptr);

    if (phyDevCount == 0) { return {}; }; 

    std::vector<VkPhysicalDevice> availPhysicalDevices(phyDevCount);
    vkEnumeratePhysicalDevices(instance, &phyDevCount, availPhysicalDevices.data());

    return availPhysicalDevices;
}

//----------------------------------------------------------------------------//


VkEngine::VkEngine(WindowHandler* window)
{
    window_ = window;
    InitVulkan();
}

VkEngine::~VkEngine()
{
    if (logicalDevice_ != VK_NULL_HANDLE)
    {
        if (swapChainInst_ != VK_NULL_HANDLE)
        {
            vkDestroySwapchainKHR(logicalDevice_, swapChainInst_, nullptr);
            LOG_INFO("VK Instance: Terminate SwapChain");
        }

        LOG_INFO("VK Instance: Terminate Logical Device");
        vkDestroyDevice(logicalDevice_, nullptr);
    }

    if (instance_ != VK_NULL_HANDLE)
    {
        if (surfaceKHR_ != VK_NULL_HANDLE)
        {
            vkDestroySurfaceKHR(instance_, surfaceKHR_, nullptr);            
            LOG_INFO("VK Instance: Terminate Surface KHR");
        }

        if ((debugMessenger_ != VK_NULL_HANDLE) || debuggingEnabled_)
        {
            LOG_INFO("VK Instance: Terminate DebugMessenger");
            DestroyDebugUtilsMessengerEXT(instance_, debugMessenger_, nullptr);
        }

        LOG_INFO("VK Instance: Terminate VkInstance");
        vkDestroyInstance(instance_, nullptr);
    }
}

void VkEngine::InitVulkan()
{
    CreateInstance();
    SetupDebugMessenger(); // Debug - Optional and be disabled
    CreateSurface();
    PickPhysicalDevice();
    CreateLogicalDeviceAndQueue();
    CreateSwapChain();
}

//----------------------------------------------------------------------------//

void VkEngine::CreateInstance()
{
    VkDebugUtilsMessengerCreateInfoEXT msgCreationInfo = GetDebugMessengerCreateInfo();
    std::vector<const char*> supportedInstanceExt = GetSupportedInstanceExtensions();

    VkApplicationInfo appInfo = {};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pNext = nullptr;
    appInfo.pApplicationName = "GfxRenderer";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "VkRender";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_3;

    VkInstanceCreateInfo instanceCreationInfo = {};
    instanceCreationInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instanceCreationInfo.pNext = debuggingEnabled_ ? &msgCreationInfo : nullptr;
    instanceCreationInfo.pApplicationInfo = &appInfo;
    instanceCreationInfo.ppEnabledExtensionNames = supportedInstanceExt.data();
    instanceCreationInfo.enabledExtensionCount = static_cast<uint32_t>(supportedInstanceExt.size());
    instanceCreationInfo.ppEnabledLayerNames = debuggingEnabled_ ? requiredValidationLayers.data() : nullptr;
    instanceCreationInfo.enabledLayerCount = debuggingEnabled_ ? static_cast<uint32_t>(requiredValidationLayers.size()) : 0;

#ifdef __APPLE__
    // This flag is require for MoltenVK
    instanceCreationInfo.flags = VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
    LOG_WARN("VK Instance: Enumerate Portability Bit for MoltenVK");
#endif

    VkResult result = vkCreateInstance(&instanceCreationInfo, nullptr, &instance_);

    if (result != VK_SUCCESS)
    {
        LOG_ERROR("VK Instance: Failed to initialize instance");
        std::exit(EXIT_FAILURE);
    }
}

std::vector<const char*> VkEngine::GetSupportedInstanceExtensions()
{
    std::vector<const char*> reqInstExt = GetGLFWRequiredExtensions();
    std::vector<VkExtensionProperties> availInstExt = GetAvailableInstanceExtensions();
    std::vector<const char*> supInstExt;

#ifdef __APPLE__
    reqInstExt.push_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
    LOG_WARN("VK Instance: Vulkan Portability Enumeration Extension Added");
#endif

    if (!CheckSupportedValidationLayers())
    {
        debuggingEnabled_ = false; // Disable it here since it is not supported
    }
    else
    {
        // Only append this if Vulkan validation layer is supported.
        if (debuggingEnabled_)
        {
            reqInstExt.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
            LOG_WARN("VK Instance: Vulkan Validation/Debugging mode enabled");
        }
    }

    CheckVkSupportedExtProperties(reqInstExt, availInstExt, supInstExt);

    return supInstExt;
}

bool VkEngine::CheckSupportedValidationLayers()
{
    std::vector<VkLayerProperties> availableLayers = GetAvailableValidationLayers();

    // Check if all required layers are available
    for (const char* required : requiredValidationLayers)
    {
        bool found = false;
        for (const auto& layers : availableLayers)
        {
            if (strcmp(required, layers.layerName) == 0)
            {
                LOG_INFO("VK Instance: Supported Validation Layers -> {}",layers.layerName);
                found = true;

                break;
            }
        }
        if (!found)
        {
            LOG_ERROR("VK Instance: Required Validation Layers not found");
            return false;
        }
    }

    return true;
}

VkDevice VkEngine::GetLogicalDevice()
{
    return logicalDevice_;
}

//----------------------------------------------------------------------------//

void VkEngine::CreateSurface()
{
    VkResult result = glfwCreateWindowSurface(instance_, window_->GetWindowHandlerPointer(), nullptr, &surfaceKHR_);

    if (result != VK_SUCCESS)
    {
        LOG_ERROR_EXIT("VK Instance: Failed to create surface.");
        return;
    }
}

//----------------------------------------------------------------------------//

void VkEngine::SetupDebugMessenger()
{
    if (!debuggingEnabled_) { return; }

    VkDebugUtilsMessengerCreateInfoEXT msgCreationInfo = GetDebugMessengerCreateInfo();
    VkResult result = CreateDebugUtilsMessengerEXT(instance_, &msgCreationInfo, nullptr, &debugMessenger_);

    if (result != VK_SUCCESS)
    {
        LOG_ERROR("VK Instance: Debug Messenger setup failed ..");
        return;
    }
}

//----------------------------------------------------------------------------//

void VkEngine::PickPhysicalDevice()
{
    std::vector<VkPhysicalDevice> availPhysicalDevices = GetAvailableDevices(instance_);

    // Filter out non compatible devices.
    for (uint32_t index = 0; index < availPhysicalDevices.size(); ++index)
    {
        if (!IsDeviceCompatible(availPhysicalDevices[index]))
        {
            availPhysicalDevices.erase(availPhysicalDevices.begin() + index);
        }
    }

    if (availPhysicalDevices.empty())
    {
        LOG_CRITICAL_EXIT("VK Instance: No available GPU to be binded.");
    }

    // Future logic for choosing with GPU

    physicalDevice_ = availPhysicalDevices[0];
    vkGetPhysicalDeviceProperties(physicalDevice_, &phyDevProperties_);
    LOG_INFO("Vk Instance: Acquired {}",phyDevProperties_.deviceName);
}

bool VkEngine::IsDeviceCompatible(VkPhysicalDevice device)
{
    return (FindQueueFamilies(device).IsComplete() &&
            !GetSupportedDeviceExtensions(device).empty() &&
            GetSwapChainSupport(device).IsValid());
}

std::vector<const char*> VkEngine::GetSupportedDeviceExtensions(VkPhysicalDevice device)
{
    std::vector<VkExtensionProperties> availDevExt = GetAvailableDeviceExtensions(device);
    std::vector<const char*> supportedDevExt;

    CheckVkSupportedExtProperties(requiredDevExt, availDevExt, supportedDevExt);
    return supportedDevExt;
}

QueueFamilyIndices VkEngine::FindQueueFamilies(VkPhysicalDevice device)
{
    bool deviceSupported = false;
    VkBool32 presentationSupport = false;

    std::vector<VkQueueFamilyProperties> familiesProperties = GetDeviceQueueFamilyProperties(device);

    QueueFamilyIndices queueFamilyIndices = {};
    queueFamilyIndices.graphicsFamilyHaxValue = false;
    queueFamilyIndices.presentFamilyHasValue = false;

    for (uint32_t i = 0; i < familiesProperties.size(); ++i)
    {
        VkBool32 presentationSupport = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(device, i ,surfaceKHR_, &presentationSupport);

        if (familiesProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
        {
            queueFamilyIndices.graphicsFamilyIdx = i;
            queueFamilyIndices.graphicsFamilyHaxValue = true;
        }

        if (presentationSupport)
        {
            queueFamilyIndices.presentFamilyIdx = i;
            queueFamilyIndices.presentFamilyHasValue = true;
        }

        if (queueFamilyIndices.IsComplete())
        {
            break;
        }
    }

    return queueFamilyIndices;
}

SwapChainCapabilities VkEngine::GetSwapChainSupport(VkPhysicalDevice device)
{
    SwapChainCapabilities properties;
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
        LOG_WARN("VK Instance: Null SwapChainProperties");
    }

    return properties;
}

//----------------------------------------------------------------------------//

void VkEngine::CreateLogicalDeviceAndQueue()
{
    QueueFamilyIndices familyIndices = FindQueueFamilies(physicalDevice_);
    std::vector<const char*> availableDevExt = GetSupportedDeviceExtensions(physicalDevice_);
    std::set<uint32_t> queueFamilyIndex = {familyIndices.graphicsFamilyIdx, familyIndices.presentFamilyIdx};
    std::vector<VkDeviceQueueCreateInfo> queueCreateList;
    float queuePriority = 1.0f;

    LOG_INFO("Vk Instance: GraphicIdx {} - PresentIdx {}",familyIndices.graphicsFamilyIdx ,familyIndices.presentFamilyIdx);

    for (uint32_t index : queueFamilyIndex)
    {
        VkDeviceQueueCreateInfo queueCreateInfo = {};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = index;
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
        LOG_ERROR("VK Instance: Logical Device creation failed.");
        return;
    }

    vkGetDeviceQueue(logicalDevice_, familyIndices.graphicsFamilyIdx, 0, &graphicsQueue_);
    vkGetDeviceQueue(logicalDevice_, familyIndices.presentFamilyIdx, 0, &presentQueue_);
}

//----------------------------------------------------------------------------//

void VkEngine::CreateSwapChain()
{
    auto swapCap = GetSwapChainSupport(physicalDevice_);
    VkSurfaceFormatKHR surfaceFormat = PickSwapSurfaceFormat(swapCap.formats);
    VkPresentModeKHR presentMode = PickSwapPresentMode(swapCap.presentModes);
    VkExtent2D extent2D = PickSwapExtent(swapCap.capabilities);
    uint32_t imageCount = PickSwapImageCount(swapCap.capabilities);

    VkSwapchainCreateInfoKHR swapChainInfo  = {};
    swapChainInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapChainInfo.surface = surfaceKHR_;
    swapChainInfo.minImageCount = imageCount;
    swapChainInfo.imageFormat = surfaceFormat.format;
    swapChainInfo.imageColorSpace = surfaceFormat.colorSpace;
    swapChainInfo.imageExtent = extent2D;
    swapChainInfo.imageArrayLayers = 1;
    swapChainInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    swapChainInfo.presentMode = presentMode;
    swapChainInfo.preTransform = swapCap.capabilities.currentTransform;
    swapChainInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    swapChainInfo.clipped = VK_TRUE;
    swapChainInfo.oldSwapchain = VK_NULL_HANDLE;

    QueueFamilyIndices queueFamilies = FindQueueFamilies(physicalDevice_);

    if (queueFamilies.graphicsFamilyIdx != queueFamilies.presentFamilyIdx)
    {
        uint32_t queuefamilyArr[] = {
            queueFamilies.graphicsFamilyIdx,
            queueFamilies.presentFamilyIdx,
        };

        swapChainInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        swapChainInfo.queueFamilyIndexCount = 2;
        swapChainInfo.pQueueFamilyIndices = queuefamilyArr;
    }
    else
    {
        swapChainInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        swapChainInfo.queueFamilyIndexCount = 0;
        swapChainInfo.pQueueFamilyIndices = nullptr;
    }

    VkResult result = vkCreateSwapchainKHR(logicalDevice_, &swapChainInfo, nullptr, &swapChainInst_);

    if (result != VK_SUCCESS)
    {
        spdlog::critical("VK Swap: Unable to create Swap Chain instance");
        std::exit(EXIT_FAILURE);
    }
}

VkSurfaceFormatKHR VkEngine::PickSwapSurfaceFormat(std::vector<VkSurfaceFormatKHR> formats)
{
    // All format is supported
    if ((formats.size() == 1) && formats[0].format == VK_FORMAT_UNDEFINED)
    {
        LOG_INFO("VK Swap: All Format are supported");
        return {VK_FORMAT_R8G8B8A8_SRGB, VK_COLORSPACE_SRGB_NONLINEAR_KHR };
    }

    VkSurfaceFormatKHR choosenFormat = {};
    for (auto i : formats)
    {
        if ((i.colorSpace == VK_COLORSPACE_SRGB_NONLINEAR_KHR) &&
           ((i.format == VK_FORMAT_R8G8B8A8_SRGB) || (i.format == VK_FORMAT_B8G8R8A8_SRGB)))
        {
            std::cout << "Format :" << i.format << "   ColorSpace :" << i.colorSpace << std::endl;
            choosenFormat = i;
        }
    }

    return choosenFormat;
}

VkPresentModeKHR VkEngine::PickSwapPresentMode(std::vector<VkPresentModeKHR> presentModes)
{
    // More selecetive options in the future - Vsync off/on
    VkPresentModeKHR choosenMode;
    for (auto modes : presentModes)
    {
        if (modes == VK_PRESENT_MODE_MAILBOX_KHR)
        {

            LOG_INFO("Vk Instance: Present Mode supported - Mailbox");
            choosenMode = modes;
        }
    }
    
    return choosenMode;
}

VkExtent2D VkEngine::PickSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities)
{
    if (capabilities.currentExtent.width != INVALID_VK_EXTENT)
    {
        return capabilities.currentExtent;
    }
    else
    {
        glm::ivec2 size = window_->GetFrameBufferSize();
        VkExtent2D actualExtent = {
            static_cast<uint_fast32_t>(size.x),
            static_cast<uint_fast32_t>(size.y),
        };

        actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
        actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

        return actualExtent;
    }
}

uint32_t VkEngine::PickSwapImageCount(const VkSurfaceCapabilitiesKHR& capabilities)
{
    uint32_t imageCount = capabilities.minImageCount + 1;
    if ((capabilities.maxImageCount > 0) && capabilities.maxImageCount < imageCount)
    {
        imageCount = capabilities.maxImageCount;
    }

    return imageCount;
}

} // namespace Graphic