#include "GLFW/glfw3.h"
#include "vulkan/vulkan.h"
#include "VK_Utilities.hpp"
#include "Precomp.hpp"
#include "spdlog/spdlog.h"

namespace Renderer
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
            spdlog::info("VK Validation: {}",pCallbackData->pMessage);
            break;
        case WARNING:
            spdlog::warn("VK Validation: {}",pCallbackData->pMessage);
            break;
        case ERROR:
            spdlog::critical("VK Validation: {}",pCallbackData->pMessage);
            break;
        default:
            spdlog::critical("UNKNOWN VK MESSAGE: {}",pCallbackData->pMessage);
            std::exit(EXIT_FAILURE);
    }

    return VK_FALSE;
}

// Types of Validation/Debug messages to be subscribed.
VkDebugUtilsMessengerCreateInfoEXT GetDebugMessengerCreateInfo()
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

std::vector<const char*> GetGLFWRequiredExtensions()
{
    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions;

    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    return std::vector<const char*>(glfwExtensions, glfwExtensions + glfwExtensionCount);
}


} // namespace Renderer
