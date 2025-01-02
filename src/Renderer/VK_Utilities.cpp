#include "vulkan/vulkan.h"
#include "VK_Utilities.hpp"
#include "Precomp.hpp"

namespace Renderer
{

// Logging message callback to be used in Vulkan Instance.
VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                             VkDebugUtilsMessageTypeFlagsEXT messageType,
                                             const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
                                             void* pUserData)
{
    switch(messageSeverity)
    {
        case VERSBOSE:
            std::cout << "[VERSBOSE-VK] " << pCallbackData->pMessage << std::endl;
            break;
        case INFO:
            std::cout << "[INFO-VK] " << pCallbackData->pMessage << std::endl;
            break;
        case WARNING:
            std::cerr << "[WARNING-VK] " << pCallbackData->pMessage << std::endl;
            break;
        case ERROR:
            std::cerr << "[ERROR-VK] " << pCallbackData->pMessage << std::endl;
            break;
        default:
            std::cerr << "[ERROR] Unknown Message Severity : "<< messageSeverity <<
                         "\n        VulkanK Error Message : "<< pCallbackData->pMessage << std::endl;
    }

    return VK_FALSE;
}

// Types of Validation/Debug messages to be subscribed.
VkDebugUtilsMessengerCreateInfoEXT PopulateDebugMessengerCreateInfo()
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


} // namespace Renderer
