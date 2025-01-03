#include "VK_Renderer.hpp"

#ifndef VK_RENDERER_IPP // include guard
#define VK_RENDERER_IPP

namespace Renderer
{

inline std::vector<VkQueueFamilyProperties> VkGraphic::GetDeviceQueueFamilyProperties(VkPhysicalDevice device)
{
    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);
    std::vector<VkQueueFamilyProperties> familiesProperties(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, familiesProperties.data());

    return familiesProperties;
}

} // namespace Renderer

#endif // VK_RENDERER_IPP