#ifndef GRAPHICS_VULKAN_VKSWAPCHAINIMPL_HPP
#define GRAPHICS_VULKAN_VKSWAPCHAINIMPL_HPP
#pragma once

#include <vulkan/vulkan.h>
#include <Settings.hpp>

// std libs
#include <string>
#include <vector>

// Forward declarations
class VkDeviceInstance;

namespace Graphic
{

class SwapChainInstance
{

public:
    SwapChainInstance(VkDeviceInstance* instance, VkExtent2D windowExtent);
    ~SwapChainInstance();

    SwapChainInstance(const SwapChainInstance&) = delete;
    void operator=(const SwapChainInstance&) = delete;

    uint32_t GetWidth() { return swapChainExtent_.width; }
    uint32_t GetHeight() { return swapChainExtent_.height; }
    VkExtent2D GetSwapChainExtent() { return swapChainExtent_; }

    // VkFormat GetSwapChainImageFormat() { return }

private:
    void CreateSwapChain();
    void CreateImageView();
    void CreateDepthResources();
    void CreateRenderPass();
    void CreateFrameBuffers();
    void CreateSyncObjects();

    VkSurfaceFormatKHR chooseSwapSurfaceFormat();
    VkPresentModeKHR chooseSwapPresentMode();
    VkExtent2D choostSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);

//----------------------------------------------------------------------------//

VkDeviceInstance* instance_; // Vulkan instance
VkExtent2D windowExtent_; // X * Y -> window measurements

VkExtent2D swapChainExtent_;
VkFormat swapChainFormat_;

std::vector<VkFramebuffer> frameBuffer_;
VkRenderPass renderPass_ = VK_NULL_HANDLE;

std::vector<VkImage> depthImages;
std::vector<VkDeviceMemory> depthImageMem;
std::vector<VkImageView> depthImageViews;
std::vector<VkImageView> swapChainImgViewss;
std::vector<VkImage> swapChainImgs;

// SwapChain instance
VkSwapchainKHR swapChainInst_ = VK_NULL_HANDLE;

// Thread safety controls
std::vector<VkSemaphore> imgAvailSempahores_;
std::vector<VkSemaphore> renderCompSemaphores_;
std::vector<VkFence> inFlightFences_;
std::vector<VkFence> imgInFlight_;
size_t currentFrame_ = 0;

};

} // namespace Graphic


#endif