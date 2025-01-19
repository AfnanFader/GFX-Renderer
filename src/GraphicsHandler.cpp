#include <GraphicsHandler.hpp>
#include <Input/InputHandler.ipp>
#include <Graphics/Vulkan/VkUtil.ipp>

// STD Lib
#include <array>

namespace Graphic
{

GraphicsHandler::GraphicsHandler() 
{
    CreatePipelineLayout();
    CreatePipeline();
    CreateCommandBuffer();
}

GraphicsHandler::~GraphicsHandler()
{
    if (pipelineLayout_ != VK_NULL_HANDLE)
    {
        vkDestroyPipelineLayout(vkInstance_.GetLogicalDevice(), pipelineLayout_, nullptr);
    }
}

void GraphicsHandler::RenderLoop()
{
    glfwSetKeyCallback(window_.GetWindowHandlerPointer(), Input::KeyCallBack);

    while (!window_.ShouldCloseWindow())
    {
        glfwPollEvents();
        DrawFrame();
    }

    vkDeviceWaitIdle(vkInstance_.GetLogicalDevice());

    Cleanup();
}

void GraphicsHandler::CreatePipeline()
{
    auto pipelineConfig = GraphicPipeline::DefaultPipeLineConfigInfo(
        swapChainInst_.GetWidth(),
        swapChainInst_.GetHeight()
    );
    pipelineConfig.renderPass = swapChainInst_.GetRenderPass();
    pipelineConfig.pipelineLayout = pipelineLayout_;

    pipeline_ = std::make_unique<GraphicPipeline>(
        &vkInstance_,
        VERT_SHADER_PATH,
        FRAG_SHADER_PATH,
        pipelineConfig);
}

void GraphicsHandler::CreatePipelineLayout()
{
    VkPipelineLayoutCreateInfo pipeLayoutInfo = {};
    pipeLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipeLayoutInfo.setLayoutCount = 0;
    pipeLayoutInfo.pSetLayouts = nullptr;
    pipeLayoutInfo.pushConstantRangeCount = 0;
    pipeLayoutInfo.pPushConstantRanges = nullptr;

    VK_CHECK(
        vkCreatePipelineLayout(vkInstance_.GetLogicalDevice(), &pipeLayoutInfo, nullptr, &pipelineLayout_),
        "Pipeline Layout: Failed to create Pipeline Layout"
    )

}

void GraphicsHandler::CreateCommandBuffer()
{
    commandBuffer_.resize(swapChainInst_.GetImageCount());

    VkCommandBufferAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = vkInstance_.GetCommandPool();
    allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffer_.size());

    VK_CHECK(
        vkAllocateCommandBuffers(vkInstance_.GetLogicalDevice(), &allocInfo, commandBuffer_.data()),
        "CommandBuffer: Failed to allocate command buffer !!"
    )

    for (int i = 0; i < commandBuffer_.size(); i++)
    {
        // Check if the recording of command is started.
        VkCommandBufferBeginInfo beginInfo = {};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        VK_CHECK(
            vkBeginCommandBuffer(commandBuffer_[i], &beginInfo),
            "CommandBuffer: Failed to start recording command !!"
        )

        VkRenderPassBeginInfo renderPassInfo = {};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = swapChainInst_.GetRenderPass();
        renderPassInfo.framebuffer = swapChainInst_.GetFrameBuffer(i);

        renderPassInfo.renderArea.offset = {0,0};
        renderPassInfo.renderArea.extent = swapChainInst_.GetSwapChainExtent();

        // Both color attachment and depth attachemnt indexs are binded
        // at the Swapchain Instantiation during the CreateRenderPass()
        std::array<VkClearValue, 2> clearValues{};
        clearValues[0].color = {1.0f, 0.1f, 0.1f, 1.0f};
        clearValues[1].depthStencil = {1.0f, 0};
        renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
        renderPassInfo.pClearValues = clearValues.data();

        vkCmdBeginRenderPass(commandBuffer_[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

        pipeline_->BindPipeline(commandBuffer_[i]);
        vkCmdDraw(commandBuffer_[i], 3, 1, 0, 0);

        vkCmdEndRenderPass(commandBuffer_[i]);
        VK_CHECK(
            vkEndCommandBuffer(commandBuffer_[i]),
            "CommandBuffer: Failed to record command in buffers !!"
        )
    }
}

void GraphicsHandler::DrawFrame()
{
    uint32_t imageIndex;
    VkResult result = swapChainInst_.AcquireNextImage(&imageIndex);
    if ((result != VK_SUCCESS) && (result != VK_SUBOPTIMAL_KHR))
    {
        LOG_ERROR("Render: Failed to acquire next SwapChain image");
    }

    VK_CHECK(
        swapChainInst_.SubmitCommandBuffers(&commandBuffer_[imageIndex], &imageIndex),
        "Render: Failed to present SwapChain iamge !!"
    )
}

void GraphicsHandler::Cleanup()
{
}

} // namespace Graphic
