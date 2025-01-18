#ifndef GRAPHICS_VULKAN_VKUTIL_HPP
#define GRAPHICS_VULKAN_VKUTIL_HPP
#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include <string>

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

} // namespace Graphic


#endif