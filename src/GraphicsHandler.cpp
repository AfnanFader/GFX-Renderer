#include <GraphicsHandler.hpp>
#include <Input/InputHandler.ipp>
#include <Graphics/Vulkan/VkUtil.ipp>

// STD Lib
#include <array>

namespace Graphic
{

static void sierpinski(
        std::vector<Vertex> &vertices,
        int depth,
        glm::vec2 left,
        glm::vec2 right,
        glm::vec2 top) {
    if (depth <= 0) {
        vertices.push_back({top});
        vertices.push_back({right});
        vertices.push_back({left});
    } else {
        auto leftTop = 0.5f * (left + top);
        auto rightTop = 0.5f * (right + top);
        auto leftRight = 0.5f * (left + right);
        sierpinski(vertices, depth - 1, left, leftRight, leftTop);
        sierpinski(vertices, depth - 1, leftRight, right, rightTop);
        sierpinski(vertices, depth - 1, leftTop, rightTop, top);
    }
}

GraphicsHandler::GraphicsHandler() 
{
    loadModels();
    CreatePipelineLayout();
    RecreateSwapChain();
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
        swapChainInst_->GetWidth(),
        swapChainInst_->GetHeight()
    );
    pipelineConfig.renderPass = swapChainInst_->GetRenderPass();
    pipelineConfig.pipelineLayout = pipelineLayout_;

    pipeline_ = std::make_unique<GraphicPipeline>(
        &vkInstance_,
        VERT_SHADER_PATH,
        FRAG_SHADER_PATH,
        pipelineConfig);
}

void GraphicsHandler::RecreateSwapChain()
{
    auto extent = window_.GetWindowExtent();
    while ((extent.width == 0) || (extent.height == 0))
    {
        extent = window_.GetWindowExtent();
        glfwWaitEvents();
    }
    vkDeviceWaitIdle(vkInstance_.GetLogicalDevice());

     // HELL YEAH, the object was not destroyed quickly enough to release 
     // the surface from GLFW. Caused - VK_ERROR_NATIVE_WINDOW_IN_USE_KHR
    swapChainInst_.reset();

    swapChainInst_ = std::make_unique<SwapChainInstance>(&vkInstance_, extent);
    CreatePipeline();
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
    commandBuffer_.resize(swapChainInst_->GetImageCount());

    VkCommandBufferAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = vkInstance_.GetCommandPool();
    allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffer_.size());

    VK_CHECK(
        vkAllocateCommandBuffers(vkInstance_.GetLogicalDevice(), &allocInfo, commandBuffer_.data()),
        "CommandBuffer: Failed to allocate command buffer !!"
    )
}

void GraphicsHandler::RecordCommandBuffer(int imageIndex)
{
    // Check if the recording of command is started.
    VkCommandBufferBeginInfo beginInfo = {};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    VK_CHECK(
        vkBeginCommandBuffer(commandBuffer_[imageIndex], &beginInfo),
        "CommandBuffer: Failed to start recording command !!"
    )

    VkRenderPassBeginInfo renderPassInfo = {};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = swapChainInst_->GetRenderPass();
    renderPassInfo.framebuffer = swapChainInst_->GetFrameBuffer(imageIndex);

    renderPassInfo.renderArea.offset = {0,0};
    renderPassInfo.renderArea.extent = swapChainInst_->GetSwapChainExtent();

    // Both color attachment and depth attachemnt indexs are binded
    // at the Swapchain Instantiation during the CreateRenderPass()
    std::array<VkClearValue, 2> clearValues{};
    clearValues[0].color = {0.1f, 0.1f, 0.1f, 1.0f};
    clearValues[1].depthStencil = {1.0f, 0};
    renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
    renderPassInfo.pClearValues = clearValues.data();

    vkCmdBeginRenderPass(commandBuffer_[imageIndex], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

    pipeline_->BindPipeline(commandBuffer_[imageIndex]);
    vkModel_->Bind(commandBuffer_[imageIndex]);
    vkModel_->Draw(commandBuffer_[imageIndex]);

    vkCmdEndRenderPass(commandBuffer_[imageIndex]);
    VK_CHECK(
        vkEndCommandBuffer(commandBuffer_[imageIndex]),
        "CommandBuffer: Failed to record command in buffers !!"
    )
}

void GraphicsHandler::DrawFrame()
{
    uint32_t imageIndex;
    VkResult result = swapChainInst_->AcquireNextImage(&imageIndex);

    if (result == VK_ERROR_OUT_OF_DATE_KHR)
    {
        RecreateSwapChain();
        return;
    }

    if ((result != VK_SUCCESS) && (result != VK_SUBOPTIMAL_KHR))
    {
        LOG_ERROR("Render: Failed to acquire next SwapChain image");
    }

    RecordCommandBuffer(imageIndex);
    result = swapChainInst_->SubmitCommandBuffers(&commandBuffer_[imageIndex], &imageIndex);

    if ((result == VK_ERROR_OUT_OF_DATE_KHR) || (result == VK_SUBOPTIMAL_KHR) || window_.WasWindowResized())
    {
        window_.ResetFrameBufferResized();
        RecreateSwapChain();
        return;
    }
    if (result != VK_SUCCESS)
    {
        LOG_WARN( "Render: Failed to present SwapChain iamge !!");
    }
}

void GraphicsHandler::loadModels()
{
    std::vector<Vertex> vertices{
        {{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
        {{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
        {{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}
    };
    // sierpinski(vertices, 3, {-0.5f, 0.5f}, {0.5f, 0.5f}, {0.0f, -0.5f});
    vkModel_ = std::make_unique<VkModel>(&vkInstance_, vertices);
}

void GraphicsHandler::Cleanup()
{
}

} // namespace Graphic
