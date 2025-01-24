#ifndef GRAPHICS_VULKAN_VKSWAPCHAINIMPL_IPP
#define GRAPHICS_VULKAN_VKSWAPCHAINIMPL_IPP
#pragma once

#include <Graphics/Vulkan/VkSwapChainImpl.hpp>

namespace Graphic
{

inline bool SwapChainInstance::CompareSwapFormats(VkFormat imageFormat, VkFormat depthFormat) const
{
    return ((imageFormat == swapChainImageFormat_) &&
           (depthFormat == swapChainDepthFormat_));
}

} // namespace Graphic

#endif