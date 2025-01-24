#ifndef GRAPHICS_VULKAN_VKSWAPCHAINIMPL_HPP
#define GRAPHICS_VULKAN_VKSWAPCHAINIMPL_HPP
#pragma once

#include <Graphics/Vulkan/VkInstanceImpl.hpp>

// External Libs
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
    SwapChainInstance operator=(const SwapChainInstance&) = delete;

    uint32_t GetWidth() { return swapChainExtent_.width; }
    uint32_t GetHeight() { return swapChainExtent_.height; }
    VkExtent2D GetSwapChainExtent() { return swapChainExtent_; }

    VkFormat GetSwapChainImageFormat() { return swapChainImageFormat_; }
    VkFormat GetSwapChainDepthFormat() { return swapChainDepthFormat_; }
    size_t GetImageCount() { return swapChainImages_.size(); }

    VkImageView GetImageView(int32_t index) {return swapChainImgViews_[index]; }
    VkFramebuffer GetFrameBuffer(int32_t index) { return frameBuffer_[index]; }
    VkRenderPass GetRenderPass() const { return renderPass_; }

    float ExtendAspectRatio() { 
        return (static_cast<float>(swapChainExtent_.width)/static_cast<float>(swapChainExtent_.height));
    }

    // To check compatibility !!!
    bool CompareSwapFormats(VkFormat imageFormat, VkFormat depthFormat) const;

    VkFormat FindDepthFormat();
    VkResult AcquireNextImage(uint32_t* imageIndex);
    VkResult SubmitCommandBuffers(const VkCommandBuffer* buffers, uint32_t* imageIndex);

private:
    void CreateSwapChain();
    void CreateImageView();
    void CreateDepthResources();
    void CreateRenderPass();
    void CreateFrameBuffers();
    void CreateSyncObjects();

    VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availFormats);
    VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availPresentMode);
    VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);

//----------------------------------------------------------------------------//

VkDeviceInstance* instance_; // Vulkan instance
VkExtent2D windowExtent_; // X * Y -> window measurements

VkExtent2D swapChainExtent_;
VkFormat swapChainImageFormat_;
VkFormat swapChainDepthFormat_;

std::vector<VkFramebuffer> frameBuffer_;
VkRenderPass renderPass_ = VK_NULL_HANDLE;

std::vector<VkImage> depthImages_;
std::vector<VkDeviceMemory> depthImgMem_;
std::vector<VkImageView> depthImgViews_;
std::vector<VkImageView> swapChainImgViews_;
std::vector<VkImage> swapChainImages_;

// SwapChain instance
VkSwapchainKHR swapChainInst_ = VK_NULL_HANDLE;

// Thread safety controls
std::vector<VkSemaphore> imgAvailSemaphores_;
std::vector<VkSemaphore> renderCompSemaphores_;
std::vector<VkFence> inFlightFences_;
std::vector<VkFence> imgInFlight_;
size_t currentFrame_ = 0;

};

} // namespace Graphic


#endif