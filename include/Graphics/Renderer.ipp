#ifndef GRAPHICS_RENDERER_IPP
#define GRAPHICS_RENDERER_IPP
#pragma once

#include <Graphics/Renderer.hpp>

namespace Graphic
{

inline bool Renderer::IsFrameInProgress() const
{
    return isFrameStarted_;
}

inline void Renderer::SetFrameInProgress(bool state)
{
    isFrameStarted_ = state;
}

inline uint32_t Renderer::GetCurrentImageIndex() const
{
    return currImgIdx_;
}

inline VkCommandBuffer Renderer::GetCurrentCommandBuffer() const
{
    assert(isFrameStarted_ && "Unable top get command buffer when frame not in progress");
    return commandBuffers_[currImgIdx_];
}

inline VkRenderPass Renderer::GetRenderPass() const
{
    return swapChainInst_->GetRenderPass();
}

} // namespace Graphic

#endif