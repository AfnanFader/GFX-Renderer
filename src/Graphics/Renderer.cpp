#include <Graphics/Renderer.ipp>
#include <Graphics/Vulkan/VkInstanceImpl.ipp>
#include <Graphics/Vulkan/VkUtil.ipp>
#include <Graphics/WindowHandler.ipp>

// STD Lib
#include <array>

namespace Graphic
{

Renderer::Renderer(
    WindowHandler* window,
    VkDeviceInstance* deviceInst)
    : window_(window), deviceInst_(deviceInst)
{
    RecreateSwapChain();
    CreateCommandBuffer();
}

Renderer::~Renderer()
{
    FreeCommandBuffers();
    window_ = nullptr;
    deviceInst_ = nullptr;
}

void Renderer::RecreateSwapChain()
{
    auto extent = window_->GetWindowExtent();
    while ((extent.width == 0) || (extent.height == 0))
    {
        extent = window_->GetWindowExtent();
        glfwWaitEvents();
    }
    vkDeviceWaitIdle(deviceInst_->GetLogicalDevice());

    // Object was not destroyed quickly enough to release 
    // the surface from GLFW. Caused - VK_ERROR_NATIVE_WINDOW_IN_USE_KHR
    // @todo Need to implement Old swapchain handling.
    swapChainInst_.reset();
    swapChainInst_ = std::make_unique<SwapChainInstance>(deviceInst_, extent);
}

void Renderer::CreateCommandBuffer()
{
    commandBuffers_.resize(swapChainInst_->GetImageCount());

    VkCommandBufferAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = deviceInst_->GetCommandPool();
    allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers_.size());

    VK_CHECK(
        vkAllocateCommandBuffers(deviceInst_->GetLogicalDevice(), &allocInfo, commandBuffers_.data()),
        "CommandBuffer: Failed to allocate command buffer !!"
    )
}

VkCommandBuffer Renderer::BeginFrame()
{
    assert(!isFrameStarted_ && "Can't call Begin Frame while in already in progress");

    VkResult result = swapChainInst_->AcquireNextImage(&currImgIdx_);

    if (result == VK_ERROR_OUT_OF_DATE_KHR)
    {
        RecreateSwapChain();
        return nullptr;
    }

    if ((result != VK_SUCCESS) && (result != VK_SUBOPTIMAL_KHR))
    {
        LOG_ERROR("Render: Failed to acquire next SwapChain image");
    }

    // Set this to true to record new commands.
    isFrameStarted_ = true;

    auto commandBuffer = GetCurrentCommandBuffer();

    VkCommandBufferBeginInfo beginInfo = {};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    VK_CHECK(
        vkBeginCommandBuffer(commandBuffer, &beginInfo),
        "CommandBuffer: Failed to start recording command !!"
    )

    return commandBuffer;
}

void Renderer::EndFrame()
{
    assert(isFrameStarted_ && "Can't call EndFrame while frame is not in progress");

    auto commandBuffer = GetCurrentCommandBuffer();

    VK_CHECK(
        vkEndCommandBuffer(commandBuffer),
        "CommandBuffer: Failed to record command in buffers !!"
    )

    VkResult result = swapChainInst_->SubmitCommandBuffers(&commandBuffer, &currImgIdx_);

    // @todo Enhance the swapchain recreation with old swapchain mechanics
    if ((result == VK_ERROR_OUT_OF_DATE_KHR) || 
       (result == VK_SUBOPTIMAL_KHR) || window_->WasWindowResized())
    {
        window_->ResetFrameBufferResized();
        RecreateSwapChain();
    }
    if (result != VK_SUCCESS)
    {
        LOG_WARN( "Render: Failed to present SwapChain iamge !!");
    }

    // End of recording new commands
    isFrameStarted_ = false;
}

void Renderer::BeginSwapChainRenderPass(VkCommandBuffer commandBuffer)
{
    assert(isFrameStarted_ && "Can't call BegunSwapChainRenderPass if frame is not in progress");
    assert(
        commandBuffer == GetCurrentCommandBuffer() &&
        "Can't begin RenderPass due to missmatch on current CommandBuffer");

    VkRenderPassBeginInfo renderPassInfo = {};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = swapChainInst_->GetRenderPass();
    renderPassInfo.framebuffer = swapChainInst_->GetFrameBuffer(currImgIdx_);

    renderPassInfo.renderArea.offset = {0,0};
    renderPassInfo.renderArea.extent = swapChainInst_->GetSwapChainExtent();

    // Both color attachment and depth attachemnt indexs are binded
    // at the Swapchain Instantiation during the CreateRenderPass()
    std::array<VkClearValue, 2> clearValues{};
    clearValues[0].color = {0.01f, 0.01f, 0.01f, 1.0f};
    clearValues[1].depthStencil = {1.0f, 0};
    renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
    renderPassInfo.pClearValues = clearValues.data();

    vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

    VkViewport viewPort{};
    viewPort.x = 0.0f;
    viewPort.y= 0.0f;
    viewPort.width = static_cast<float>(swapChainInst_->GetSwapChainExtent().width);
    viewPort.height = static_cast<float>(swapChainInst_->GetSwapChainExtent().height);
    viewPort.minDepth = 0.0f;
    viewPort.maxDepth = 1.0f;
    VkRect2D scissor{{0,0}, swapChainInst_->GetSwapChainExtent()};
    vkCmdSetViewport(commandBuffer, 0, 1, &viewPort);
    vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
}

void Renderer::EndSwapChainRenderPass(VkCommandBuffer commandBuffer)
{
    assert(isFrameStarted_ && "Can't call EndSwapChainRenderPass if frame is not in progress");
    assert(
        commandBuffer == GetCurrentCommandBuffer() &&
        "Can't end RenderPass due to missmatch on current CommandBuffer");

    vkCmdEndRenderPass(commandBuffer);
}

void Renderer::FreeCommandBuffers()
{
    vkFreeCommandBuffers(
        deviceInst_->GetLogicalDevice(),
        deviceInst_->GetCommandPool(),
        static_cast<uint32_t>(commandBuffers_.size()),
        commandBuffers_.data()
    );
}

} // namespace Graphic
