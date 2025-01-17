#include <Graphics/Vulkan/VulkanInitializer.hpp>
#include <Logging.hpp>

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
            LOG_ERROR_EXIT("VK Validation: {}",pCallbackData->pMessage);
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

//----------------------------------------------------------------------------//


VulkanInitializer::VulkanInitializer(WindowHandler* window)
{
    window_ = window;
    InitVulkan();
}

VulkanInitializer::~VulkanInitializer()
{
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

void VulkanInitializer::InitVulkan()
{
    CreateInstance();
    SetupDebugMessenger(); // Note that this is optional for now
    CreateSurface();
    PickPhysicalDevice();
}

//----------------------------------------------------------------------------//

void VulkanInitializer::CreateInstance()
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
    appInfo.apiVersion = VK_API_VERSION_1_0;

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

std::vector<const char*> VulkanInitializer::GetSupportedInstanceExtensions()
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

    LOG_INFO("VK Instance: Checking Supported Instance Extensions");
    CheckVkSupportedExtProperties(reqInstExt, availInstExt, supInstExt);

    return supInstExt;
}

bool VulkanInitializer::CheckSupportedValidationLayers()
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

//----------------------------------------------------------------------------//

void VulkanInitializer::CreateSurface()
{
    VkResult result = glfwCreateWindowSurface(instance_, window_->GetWindowHandlerPointer(), nullptr, &surfaceKHR_);

    if (result != VK_SUCCESS)
    {
        LOG_ERROR_EXIT("VK Instance: Failed to create surface.");
        return;
    }
}

//----------------------------------------------------------------------------//

void VulkanInitializer::SetupDebugMessenger()
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

void VulkanInitializer::PickPhysicalDevice()
{

}

} // namespace Graphic