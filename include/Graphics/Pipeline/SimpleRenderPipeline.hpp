#ifndef GRAPHICS_PIPELINE_SIMPLERENDERPIPELINE_HPP
#define GRAPHICS_PIPELINE_SIMPLERENDERPIPELINE_HPP
#pragma once

#include <Graphics/GameObject.hpp>
#include <Graphics/Vulkan/VkPipelineImpl.hpp>

// Effects
#include <Graphics/Pipeline/RainbowSystem.hpp>

// STD Lib
#include <memory>
#include <vector>

// Forward declaraions
namespace Graphic { class VkDeviceInstance; }

namespace Graphic
{

class SimpleRenderPipeline
{

public:

    SimpleRenderPipeline(VkDeviceInstance* deviceInst, VkRenderPass renderPass);
    ~SimpleRenderPipeline();

    SimpleRenderPipeline(const SimpleRenderPipeline&) = delete;
    SimpleRenderPipeline &operator=(const SimpleRenderPipeline&) = delete;

    void RenderGameObjects(VkCommandBuffer commandBuffer, std::vector<GameObject>& gameObjects);

private:

    void CreatePipelineLayout();

    void CreatePipeline(VkRenderPass renderPass);

    VkDeviceInstance* deviceInst_;

    RainbowSystem rainbow_{1.0f};
    
    VkPipelineLayout pipelineLayout_ = VK_NULL_HANDLE;
    std::unique_ptr<GraphicPipeline> pipeline_;
};

} // namespace Graphic


#endif