#ifndef GRAPHICS_RENDERPIPELINE_HPP
#define GRAPHICS_RENDERPIPELINE_HPP
#pragma once

#include <Graphics/Vulkan/VkEngine.hpp>

#include <string>
#include <vector>

namespace Graphic
{


struct PipelineConfigInfo
{
    VkViewport viewport;
    VkRect2D scissor;
    VkPipelineViewportStateCreateInfo viewportInfo;
    VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo;
    VkPipelineRasterizationStateCreateInfo rasterizationInfo;
    VkPipelineMultisampleStateCreateInfo multisampleInfo;
    VkPipelineColorBlendAttachmentState colorBlendAttachment;
    VkPipelineColorBlendStateCreateInfo colorBlendInfo;
    VkPipelineDepthStencilStateCreateInfo depthStencilInfo;
    VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
    VkRenderPass renderPass = VK_NULL_HANDLE;
    uint32_t subpass = 0;
};

class RenderPipeline
{

public:
    RenderPipeline(
        VkEngine* instance,
        const std::string& vertFilePath,
        const std::string& fragFilePath,
        const PipelineConfigInfo& configInfo);
    ~RenderPipeline();

    RenderPipeline(const RenderPipeline&) = delete;
    void operator=(const RenderPipeline) = delete;

    static PipelineConfigInfo DefaultPipeLineConfigInfo(uint32_t width, uint32_t height);

private:
    std::vector<char> ReadFile(const std::string& filePath);

    void CreateGraphicsPipeline(
        const std::string& vertFilePath,
        const std::string& fragFilePath,
        const PipelineConfigInfo& configInfo);

    void createShaderModule(const std::vector<char>& code, VkShaderModule* shaderModule);

//----------------------------------------------------------------------------//

    VkDevice device_ = nullptr;
    VkPipeline renderPipeline_ = VK_NULL_HANDLE;
    VkShaderModule vertShaderModule_ = VK_NULL_HANDLE;
    VkShaderModule fragShaderMofule_ = VK_NULL_HANDLE;
};
    
} // namespace Graphic


#endif