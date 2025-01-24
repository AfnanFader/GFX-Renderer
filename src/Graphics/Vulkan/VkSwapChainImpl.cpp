#include <Graphics/Vulkan/VkSwapChainImpl.ipp>
#include <Graphics/Vulkan/VkUtil.ipp>
#include <Settings.hpp>

// External Lib
#include <Logging.hpp>

// STD Lib
#include <limits>
#include <set>

namespace Graphic
{
    
SwapChainInstance::SwapChainInstance(
    VkDeviceInstance* instance, VkExtent2D windowExtent)
    : instance_(instance), windowExtent_(windowExtent)
{
    CreateSwapChain();
    CreateImageView();
    CreateRenderPass();
    CreateDepthResources();
    CreateFrameBuffers();
    CreateSyncObjects();
}

SwapChainInstance::~SwapChainInstance()
{
    for (auto imageView : swapChainImgViews_)
    {
        vkDestroyImageView(instance_->GetLogicalDevice(), imageView, nullptr);
    }
    swapChainImgViews_.clear();

    if (swapChainInst_ != VK_NULL_HANDLE)
    {
        vkDestroySwapchainKHR(instance_->GetLogicalDevice(), swapChainInst_, nullptr);
        swapChainInst_ = VK_NULL_HANDLE;
        LOG_INFO("SwapChain: Terminated !!");
    }

    for (int i = 0; i < depthImages_.size(); i++)
    {
        vkDestroyImageView(instance_->GetLogicalDevice(), depthImgViews_[i], nullptr);
        vkDestroyImage(instance_->GetLogicalDevice(), depthImages_[i], nullptr);
        vkFreeMemory(instance_->GetLogicalDevice(), depthImgMem_[i], nullptr);
    }

    for (auto framebuffer : frameBuffer_)
    {
        vkDestroyFramebuffer(instance_->GetLogicalDevice(), framebuffer, nullptr);
    }

    vkDestroyRenderPass(instance_->GetLogicalDevice(), renderPass_, nullptr);

    // cleanup synchronization objects
    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
    {
        vkDestroySemaphore(instance_->GetLogicalDevice(), renderCompSemaphores_[i], nullptr);
        vkDestroySemaphore(instance_->GetLogicalDevice(), imgAvailSemaphores_[i], nullptr);
        vkDestroyFence(instance_->GetLogicalDevice(), inFlightFences_[i], nullptr);
    }
}

void SwapChainInstance::CreateSwapChain()
{
    SwapChainCapabilities swapChainSupport = GetSwapChainSupport(
                                                instance_->GetPhyDevice(),
                                                instance_->GetSurface());

    VkSurfaceFormatKHR surfaceFormat = ChooseSwapSurfaceFormat(swapChainSupport.formats);
    VkPresentModeKHR presentMode = ChooseSwapPresentMode(swapChainSupport.presentModes);
    VkExtent2D extent = ChooseSwapExtent(swapChainSupport.capabilities);

    uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
    if (swapChainSupport.capabilities.maxImageCount > 0 &&
        imageCount > swapChainSupport.capabilities.maxImageCount)
    {
        imageCount = swapChainSupport.capabilities.maxImageCount;
    }

    VkSwapchainCreateInfoKHR createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = instance_->GetSurface();
    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = surfaceFormat.format;
    createInfo.imageColorSpace = surfaceFormat.colorSpace;
    createInfo.imageExtent = extent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    QueueFamilyIndices indices = FindQueueFamilies(instance_->GetPhyDevice(),
                                                  instance_->GetSurface());
    uint32_t queueFamilyIndices[] = {indices.graphicsFamilyIdx, indices.presentFamilyIdx};

    if (indices.graphicsFamilyIdx != indices.presentFamilyIdx)
    {
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices = queueFamilyIndices;
    }
    else
    {
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        createInfo.queueFamilyIndexCount = 0;      // Optional
        createInfo.pQueueFamilyIndices = nullptr;  // Optional
    }

    createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.presentMode = presentMode;
    createInfo.clipped = VK_TRUE;

    createInfo.oldSwapchain = VK_NULL_HANDLE;

    VK_CHECK(
        vkCreateSwapchainKHR(instance_->GetLogicalDevice(), &createInfo, nullptr, &swapChainInst_),
        "SwapChain: Failed to initialize swap chain !!"
    )

    // we only specified a minimum number of images in the swap chain, so the implementation is
    // allowed to create a swap chain with more. That's why we'll first query the final number of
    // images with vkGetSwapchainImagesKHR, then resize the container and finally call it again to
    // retrieve the handles.

    vkGetSwapchainImagesKHR(instance_->GetLogicalDevice(), swapChainInst_, &imageCount, nullptr);
    swapChainImages_.resize(imageCount);
    vkGetSwapchainImagesKHR(instance_->GetLogicalDevice(), swapChainInst_, &imageCount, swapChainImages_.data());

    swapChainImageFormat_ = surfaceFormat.format;
    swapChainExtent_ = extent;
}

void SwapChainInstance::CreateImageView()
{
    swapChainImgViews_.resize(swapChainImages_.size());
    for (size_t i = 0; i < swapChainImages_.size(); i++)
    {
        VkImageViewCreateInfo viewInfo{};
        viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewInfo.image = swapChainImages_[i];
        viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        viewInfo.format = swapChainImageFormat_;
        viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        viewInfo.subresourceRange.baseMipLevel = 0;
        viewInfo.subresourceRange.levelCount = 1;
        viewInfo.subresourceRange.baseArrayLayer = 0;
        viewInfo.subresourceRange.layerCount = 1;

        VK_CHECK(
            vkCreateImageView(instance_->GetLogicalDevice(), &viewInfo, nullptr, &swapChainImgViews_[i]),
            "SwapChain: Failed to create texture image view !!"
        )
    }
}

void SwapChainInstance::CreateDepthResources()
{
    swapChainDepthFormat_ = FindDepthFormat();
    VkExtent2D swapChainExtent = GetSwapChainExtent();

    depthImages_.resize(GetImageCount());
    depthImgMem_.resize(GetImageCount());
    depthImgViews_.resize(GetImageCount());

    for (int i = 0; i < depthImages_.size(); i++)
    {
        VkImageCreateInfo imageInfo{};
        imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imageInfo.imageType = VK_IMAGE_TYPE_2D;
        imageInfo.extent.width = swapChainExtent.width;
        imageInfo.extent.height = swapChainExtent.height;
        imageInfo.extent.depth = 1;
        imageInfo.mipLevels = 1;
        imageInfo.arrayLayers = 1;
        imageInfo.format = swapChainDepthFormat_;
        imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
        imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        imageInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
        imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
        imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        imageInfo.flags = 0;

        instance_->CreateImageWithInfo(
            imageInfo,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
            depthImages_[i],
            depthImgMem_[i]);

        VkImageViewCreateInfo viewInfo{};
        viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewInfo.image = depthImages_[i];
        viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        viewInfo.format = swapChainDepthFormat_;
        viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
        viewInfo.subresourceRange.baseMipLevel = 0;
        viewInfo.subresourceRange.levelCount = 1;
        viewInfo.subresourceRange.baseArrayLayer = 0;
        viewInfo.subresourceRange.layerCount = 1;

        VK_CHECK(
            vkCreateImageView(instance_->GetLogicalDevice(), &viewInfo, nullptr, &depthImgViews_[i]),
            "Depth Resource: Failed to create texture image view !!"
        )
    }
}

void SwapChainInstance::CreateRenderPass()
{
    VkAttachmentDescription depthAttachment{};
    depthAttachment.format = FindDepthFormat();
    depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkAttachmentReference depthAttachmentRef{};
    depthAttachmentRef.attachment = 1;
    depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkAttachmentDescription colorAttachment = {};
    colorAttachment.format = GetSwapChainImageFormat();
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference colorAttachmentRef = {};
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass = {};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentRef;
    subpass.pDepthStencilAttachment = &depthAttachmentRef;

    VkSubpassDependency dependency = {};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.srcAccessMask = 0;
    dependency.srcStageMask =
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    dependency.dstSubpass = 0;
    dependency.dstStageMask =
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    dependency.dstAccessMask =
        VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

    VkAttachmentDescription attachments[2] = {colorAttachment, depthAttachment};
    VkRenderPassCreateInfo renderPassInfo = {};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = static_cast<uint32_t>(2);
    renderPassInfo.pAttachments = attachments;
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;
    renderPassInfo.dependencyCount = 1;
    renderPassInfo.pDependencies = &dependency;

    VK_CHECK(
        vkCreateRenderPass(instance_->GetLogicalDevice(), &renderPassInfo, nullptr, &renderPass_),
        "RenderPass: Failed to create render pass!"
    )
}

void SwapChainInstance::CreateFrameBuffers()
{
    frameBuffer_.resize(GetImageCount());
    for (size_t i = 0; i < GetImageCount(); i++)
    {
        VkImageView attachments[2] = {swapChainImgViews_[i], depthImgViews_[i]};

        VkExtent2D swapChainExtent = swapChainExtent_;
        VkFramebufferCreateInfo framebufferInfo = {};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = renderPass_;
        framebufferInfo.attachmentCount = static_cast<uint32_t>(2);
        framebufferInfo.pAttachments = attachments;
        framebufferInfo.width = swapChainExtent.width;
        framebufferInfo.height = swapChainExtent.height;
        framebufferInfo.layers = 1;

        VK_CHECK(
            vkCreateFramebuffer(instance_->GetLogicalDevice(), &framebufferInfo, nullptr, &frameBuffer_[i]),
            "SwapChain: Failed to create framebuffer !!"
        )
    }
}

void SwapChainInstance::CreateSyncObjects()
{
    imgAvailSemaphores_.resize(MAX_FRAMES_IN_FLIGHT);
    renderCompSemaphores_.resize(MAX_FRAMES_IN_FLIGHT);
    inFlightFences_.resize(MAX_FRAMES_IN_FLIGHT);
    imgInFlight_.resize(GetImageCount(), VK_NULL_HANDLE);

    VkSemaphoreCreateInfo semaphoreInfo = {};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo = {};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
    {
        if ((vkCreateSemaphore(instance_->GetLogicalDevice(), &semaphoreInfo, nullptr, &imgAvailSemaphores_[i]) !=
                VK_SUCCESS) ||
            (vkCreateSemaphore(instance_->GetLogicalDevice(), &semaphoreInfo, nullptr, &renderCompSemaphores_[i]) !=
                VK_SUCCESS) ||
            (vkCreateFence(instance_->GetLogicalDevice(), &fenceInfo, nullptr, &inFlightFences_[i]) != VK_SUCCESS))
        {
            LOG_ERROR("SyncObject: Failed to create synchronization objects for a frame !!");
        }
    }
}

VkResult SwapChainInstance::SubmitCommandBuffers(const VkCommandBuffer* buffers, uint32_t* imageIndex)
{
    if (imgInFlight_[*imageIndex] != VK_NULL_HANDLE)
    {
        vkWaitForFences(
            instance_->GetLogicalDevice(),
            1,
            &imgInFlight_[*imageIndex],
            VK_TRUE,
            UINT64_MAX);
    }
    imgInFlight_[*imageIndex] = inFlightFences_[currentFrame_];

    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore waitSemaphores[] = {imgAvailSemaphores_[currentFrame_]};
    VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;

    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = buffers;

    VkSemaphore signalSemaphores[] = {renderCompSemaphores_[currentFrame_]};
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    vkResetFences(
        instance_->GetLogicalDevice(),
        1,
        &inFlightFences_[currentFrame_]);

    if (vkQueueSubmit(
        instance_->GetGraphicsQ(),
        1, &submitInfo,
        inFlightFences_[currentFrame_]) != VK_SUCCESS)
    {
        LOG_ERROR("SwapChain: Failed to submit draw command buffer !!");
    }

    VkPresentInfoKHR presentInfo = {};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;
    VkSwapchainKHR swapChains[] = {swapChainInst_};
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;
    presentInfo.pImageIndices = imageIndex;

    auto result = vkQueuePresentKHR(instance_->GetPresentQ() ,&presentInfo);

    currentFrame_ = (currentFrame_ + 1) % MAX_FRAMES_IN_FLIGHT;

    return result;
}

VkSurfaceFormatKHR SwapChainInstance::ChooseSwapSurfaceFormat(
    const std::vector<VkSurfaceFormatKHR>& availFormats)
{
    for (const auto& format : availFormats)
    {
        if ((format.format == VK_FORMAT_B8G8R8A8_SRGB) && 
        (format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR))
        {
            return format;
        }
    }

    return availFormats[0];
}

VkPresentModeKHR SwapChainInstance::ChooseSwapPresentMode(
    const std::vector<VkPresentModeKHR>& availPresentMode)
{
    // More selecetive options in the future - Vsync off/on
    // for (auto modes : availPresentMode)
    // {
    //     if (modes == VK_PRESENT_MODE_MAILBOX_KHR)
    //     {
    //         LOG_INFO("Present Mode -> Mailbox");
    //         return modes;
    //     }
    // }
    
    LOG_INFO("Present Mode -> V-Sync");
    return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D SwapChainInstance::ChooseSwapExtent(
    const VkSurfaceCapabilitiesKHR& capabilities)
{
    if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
    {
        return capabilities.currentExtent;
    } 
    else
    {
        VkExtent2D actualExtent = windowExtent_;
        actualExtent.width = std::max(
            capabilities.minImageExtent.width,
            std::min(capabilities.maxImageExtent.width, actualExtent.width));
        actualExtent.height = std::max(
            capabilities.minImageExtent.height,
            std::min(capabilities.maxImageExtent.height, actualExtent.height));

        return actualExtent;
    }
}

VkFormat SwapChainInstance::FindDepthFormat()
{
    return instance_->FindSupportedFormat(
        {VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT},
        VK_IMAGE_TILING_OPTIMAL,
        VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
}

VkResult SwapChainInstance::AcquireNextImage(uint32_t* imageIndex)
{
    vkWaitForFences(
        instance_->GetLogicalDevice(),
        1,
        &inFlightFences_[currentFrame_],
        VK_TRUE,
        std::numeric_limits<uint64_t>::max());

    VkResult result = vkAcquireNextImageKHR(
        instance_->GetLogicalDevice(),
        swapChainInst_,
        std::numeric_limits<uint64_t>::max(),
        imgAvailSemaphores_[currentFrame_],  // must be a not signaled semaphore
        VK_NULL_HANDLE,
        imageIndex);

    return result;
}


} // namespace Graphic