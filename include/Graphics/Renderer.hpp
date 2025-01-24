#ifndef GRAPHICS_RENDERER_HPP
#define GRAPHICS_RENDERER_HPP
#pragma once

#include <Graphics/Vulkan/VkSwapChainImpl.ipp>

// External Lib
#include <vulkan/vulkan.h>

// STD Lib
#include <cassert>
#include <memory>
#include <vector>

namespace Graphic { class WindowHandler; }
namespace Graphic { class VkDeviceInstance; }

namespace Graphic
{

class Renderer
{

public:

    Renderer(WindowHandler* window, VkDeviceInstance* deviceInst);
    ~Renderer();

    Renderer(const Renderer&) = delete;
    Renderer &operator=(const Renderer&) = delete;

    VkCommandBuffer BeginFrame();
    void EndFrame();

    void BeginSwapChainRenderPass(VkCommandBuffer commandBuffer);
    void EndSwapChainRenderPass(VkCommandBuffer commandBuffer);

    bool IsFrameInProgress() const;
    void SetFrameInProgress(bool state);

    uint32_t GetCurrentImageIndex() const;

    VkCommandBuffer GetCurrentCommandBuffer() const;

    VkRenderPass GetRenderPass() const;

private:

    void CreateCommandBuffer();
    void FreeCommandBuffers();
    void RecreateSwapChain();

    WindowHandler* window_ = nullptr;
    VkDeviceInstance* deviceInst_ = nullptr;

    uint32_t currImgIdx_ = 0;
    bool isFrameStarted_ = false;

    std::unique_ptr<SwapChainInstance> swapChainInst_;
    std::vector<VkCommandBuffer> commandBuffers_;
};

} // namespace Graphic

#endif