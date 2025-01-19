#ifndef GRAPHICS_VULKAN_VKUTIL_HPP
#define GRAPHICS_VULKAN_VKUTIL_HPP
#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include <string>
#include <Logging.hpp>


#define VK_CHECK(call, ...)      \
    if (call != VK_SUCCESS)      \
    {                            \
        LOG_ERROR(__VA_ARGS__);  \
    }                            \


namespace Graphic
{

struct SwapChainCapabilities
{
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;

    bool IsValid() const
    {
        return (!formats.empty() && !presentModes.empty());
    }
};

struct QueueFamilyIndices
{
    uint32_t graphicsFamilyIdx;
    uint32_t presentFamilyIdx;
    bool graphicsFamilyHaxValue = false;
    bool presentFamilyHasValue = false;

    bool IsComplete() const
    {
        return (graphicsFamilyHaxValue && presentFamilyHasValue);
    }
};

//----------------------------------------------------------------------------//

std::vector<VkQueueFamilyProperties> GetDeviceQueueFamilyProperties(VkPhysicalDevice device);

QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface);

SwapChainCapabilities GetSwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR surface);

} // namespace Graphic


#endif