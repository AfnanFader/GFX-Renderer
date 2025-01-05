#pragma once

#include <vector>

struct VkDebugUtilsMessengerCreateInfoEXT;

namespace Renderer
{

typedef enum VkDebugMessageSeverity {
    VERSBOSE = 0x00000001,
    INFO = 0x00000010,
    WARNING = 0x00000100,
    ERROR = 0x00001000,
    MAX = 0x7FFFFFFF
} VkDebugMessageSeverity;

VkDebugUtilsMessengerCreateInfoEXT GetDebugMessengerCreateInfo();

void DebugPrintVulkanExtension(std::vector<const char*>& requiredExt);

std::vector<const char*> GetGLFWRequiredExtensions();

std::vector<VkLayerProperties> GetAvailableValidationLayers();

std::vector<VkExtensionProperties> GetAvailableInstanceExtensions();

std::vector<VkExtensionProperties> GetAvailableDeviceExtensions(VkPhysicalDevice device);

} // namespace Renderer
