#include <Graphics/Vulkan/VkUtil.ipp>

// External Lib
#include <Logging.hpp>

namespace Graphic
{

std::vector<VkQueueFamilyProperties> GetDeviceQueueFamilyProperties(VkPhysicalDevice device)
{
    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);
    std::vector<VkQueueFamilyProperties> familiesProperties(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, familiesProperties.data());

    return familiesProperties;
}

QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface)
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
        vkGetPhysicalDeviceSurfaceSupportKHR(device, i , surface, &presentationSupport);

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

SwapChainCapabilities GetSwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR surface)
{
    SwapChainCapabilities properties;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &properties.capabilities);

    uint32_t formatCount = 0;
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);
    properties.formats.resize(formatCount);
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, properties.formats.data());
     
    uint32_t presentModeCount = 0;
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);
    properties.presentModes.resize(presentModeCount);
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, properties.presentModes.data());

    if ((formatCount == 0) && (presentModeCount == 0))
    {
        LOG_WARN("VK Instance: Null SwapChainProperties");
    }

    return properties;
}

} // namespace Graphic
