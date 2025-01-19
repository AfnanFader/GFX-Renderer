#ifndef GRAPHICS_VKPIPELINEIMPL_HPP
#define GRAPHICS_VKPIPELINEIMPL_HPP
#pragma once

#include <Graphics/Vulkan/VkInstanceImpl.hpp>

#include <string>
#include <vector>

namespace Graphic
{


struct PipelineConfigInfo
{
    VkViewport viewport;
    VkRect2D scissor;
    VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo;
    VkPipelineRasterizationStateCreateInfo rasterizationInfo;
    VkPipelineMultisampleStateCreateInfo multisampleInfo;
    VkPipelineDepthStencilStateCreateInfo depthStencilInfo;
    VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
    VkRenderPass renderPass = VK_NULL_HANDLE;
    uint32_t subpass = 0;
};

class GraphicPipeline
{

public:
    GraphicPipeline(
        VkDeviceInstance* instance,
        const std::string& vertFilePath,
        const std::string& fragFilePath,
        const PipelineConfigInfo& configInfo);
    ~GraphicPipeline();

    GraphicPipeline(const GraphicPipeline&) = delete;
    GraphicPipeline& operator=(const GraphicPipeline) = delete;

    static PipelineConfigInfo DefaultPipeLineConfigInfo(uint32_t width, uint32_t height);

    void BindPipeline(VkCommandBuffer commandBuffer);

private:
    std::vector<char> ReadFile(const std::string& filePath);

    void CreateGraphicsPipeline(
        const std::string& vertFilePath,
        const std::string& fragFilePath,
        const PipelineConfigInfo& configInfo);

    void CreateShaderModule(const std::vector<char>& code, VkShaderModule* shaderModule);

//----------------------------------------------------------------------------//

    VkDevice device_ = VK_NULL_HANDLE;
    VkPipeline renderPipeline_ = VK_NULL_HANDLE;
    VkShaderModule vertShaderModule_ = VK_NULL_HANDLE;
    VkShaderModule fragShaderModule_ = VK_NULL_HANDLE;
};
    
} // namespace Graphic


#endif