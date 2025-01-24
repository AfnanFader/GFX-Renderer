#include <Graphics/Pipeline/SimpleRenderPipeline.hpp>
#include <Graphics/Vulkan/VkInstanceImpl.ipp>
#include <Graphics/Vulkan/VkUtil.ipp>

// External Lib
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

namespace Graphic
{

struct SimplePushConstants
{
    glm::mat2 transform{1.0f};
    glm::vec2 offset;
    alignas(16) glm::vec3 color;
};


SimpleRenderPipeline::SimpleRenderPipeline(
    VkDeviceInstance* deviceInst, VkRenderPass renderPass) :
    deviceInst_(deviceInst)
{
    CreatePipelineLayout();
    CreatePipeline(renderPass);
}

SimpleRenderPipeline::~SimpleRenderPipeline()
{
    if (pipelineLayout_ != VK_NULL_HANDLE)
    {
        vkDestroyPipelineLayout(deviceInst_->GetLogicalDevice(), pipelineLayout_, nullptr);
    }
}

void SimpleRenderPipeline::CreatePipeline(VkRenderPass renderPass)
{
    PipelineConfigInfo pipeConfig = {};
    GraphicPipeline::DefaultPipelineConfigInfo(pipeConfig);
    pipeConfig.renderPass = renderPass;
    pipeConfig.pipelineLayout = pipelineLayout_;

    pipeline_ = std::make_unique<GraphicPipeline>(
        deviceInst_,
        VERT_SHADER_PATH,
        FRAG_SHADER_PATH,
        pipeConfig);
}

void SimpleRenderPipeline::CreatePipelineLayout()
{

    VkPushConstantRange pushConstantRange = {};
    pushConstantRange.stageFlags = (VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT);
    pushConstantRange.offset = 0;
    pushConstantRange.size = sizeof(SimplePushConstants);

    VkPipelineLayoutCreateInfo pipeLayoutInfo = {};
    pipeLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipeLayoutInfo.setLayoutCount = 0;
    pipeLayoutInfo.pSetLayouts = nullptr;
    pipeLayoutInfo.pushConstantRangeCount = 1;
    pipeLayoutInfo.pPushConstantRanges = &pushConstantRange;

    VK_CHECK(
        vkCreatePipelineLayout(deviceInst_->GetLogicalDevice(), &pipeLayoutInfo, nullptr, &pipelineLayout_),
        "Pipeline Layout: Failed to create Pipeline Layout"
    )

}

void SimpleRenderPipeline::RenderGameObjects(
    VkCommandBuffer cmdBuffer, std::vector<GameObject>& gameObjects)
{

    // Apply colour !!
    // rainbow_.update(0.05f ,gameObjects);

    pipeline_->BindPipeline(cmdBuffer);

    for (auto& obj : gameObjects)
    {
        // Triangle vertex
        // obj.transform2d.rotation = glm::mod(obj.transform2d.rotation + 0.01f, glm::two_pi<float>());

        SimplePushConstants push = {};
        push.offset = obj.transform2d.translation;
        push.color = obj.color;
        push.transform = obj.transform2d.mat2();

        vkCmdPushConstants(
            cmdBuffer,
            pipelineLayout_,
            (VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT),
            0,
            sizeof(SimplePushConstants),
            &push
        );

        obj.model->Bind(cmdBuffer);
        obj.model->Draw(cmdBuffer);
    }
}

} // namespace Graphic
