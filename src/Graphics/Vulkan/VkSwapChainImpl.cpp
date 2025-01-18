#include <Graphics/Vulkan/VkSwapChainImpl.ipp>

namespace Graphic
{
    
} // namespace Graphic




// void VkEngine::CreateSwapChain()
// {
//     auto swapCap = GetSwapChainSupport(physicalDevice_);
//     VkSurfaceFormatKHR surfaceFormat = PickSwapSurfaceFormat(swapCap.formats);
//     VkPresentModeKHR presentMode = PickSwapPresentMode(swapCap.presentModes);
//     VkExtent2D extent2D = PickSwapExtent(swapCap.capabilities);
//     uint32_t imageCount = PickSwapImageCount(swapCap.capabilities);

//     VkSwapchainCreateInfoKHR swapChainInfo  = {};
//     swapChainInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
//     swapChainInfo.surface = surfaceKHR_;
//     swapChainInfo.minImageCount = imageCount;
//     swapChainInfo.imageFormat = surfaceFormat.format;
//     swapChainInfo.imageColorSpace = surfaceFormat.colorSpace;
//     swapChainInfo.imageExtent = extent2D;
//     swapChainInfo.imageArrayLayers = 1;
//     swapChainInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
//     swapChainInfo.presentMode = presentMode;
//     swapChainInfo.preTransform = swapCap.capabilities.currentTransform;
//     swapChainInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
//     swapChainInfo.clipped = VK_TRUE;
//     swapChainInfo.oldSwapchain = VK_NULL_HANDLE;

//     QueueFamilyIndices queueFamilies = FindQueueFamilies(physicalDevice_);

//     if (queueFamilies.graphicsFamilyIdx != queueFamilies.presentFamilyIdx)
//     {
//         uint32_t queuefamilyArr[] = {
//             queueFamilies.graphicsFamilyIdx,
//             queueFamilies.presentFamilyIdx,
//         };

//         swapChainInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
//         swapChainInfo.queueFamilyIndexCount = 2;
//         swapChainInfo.pQueueFamilyIndices = queuefamilyArr;
//     }
//     else
//     {
//         swapChainInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
//         swapChainInfo.queueFamilyIndexCount = 0;
//         swapChainInfo.pQueueFamilyIndices = nullptr;
//     }

//     VkResult result = vkCreateSwapchainKHR(logicalDevice_, &swapChainInfo, nullptr, &swapChainInst_);

//     if (result != VK_SUCCESS)
//     {
//         spdlog::critical("VK Swap: Unable to create Swap Chain instance");
//         std::exit(EXIT_FAILURE);
//     }
// }

// VkSurfaceFormatKHR VkEngine::PickSwapSurfaceFormat(std::vector<VkSurfaceFormatKHR> formats)
// {
//     // All format is supported
//     if ((formats.size() == 1) && formats[0].format == VK_FORMAT_UNDEFINED)
//     {
//         LOG_INFO("VK Swap: All Format are supported");
//         return {VK_FORMAT_R8G8B8A8_SRGB, VK_COLORSPACE_SRGB_NONLINEAR_KHR };
//     }

//     VkSurfaceFormatKHR choosenFormat = {};
//     for (auto i : formats)
//     {
//         if ((i.colorSpace == VK_COLORSPACE_SRGB_NONLINEAR_KHR) &&
//            ((i.format == VK_FORMAT_R8G8B8A8_SRGB) || (i.format == VK_FORMAT_B8G8R8A8_SRGB)))
//         {
//             std::cout << "Format :" << i.format << "   ColorSpace :" << i.colorSpace << std::endl;
//             choosenFormat = i;
//         }
//     }

//     return choosenFormat;
// }

// VkPresentModeKHR VkEngine::PickSwapPresentMode(std::vector<VkPresentModeKHR> presentModes)
// {
//     // More selecetive options in the future - Vsync off/on
//     VkPresentModeKHR choosenMode;
//     for (auto modes : presentModes)
//     {
//         if (modes == VK_PRESENT_MODE_MAILBOX_KHR)
//         {

//             LOG_INFO("Vk Instance: Present Mode supported - Mailbox");
//             choosenMode = modes;
//         }
//     }
    
//     return choosenMode;
// }

// VkExtent2D VkEngine::PickSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities)
// {
//     if (capabilities.currentExtent.width != INVALID_VK_EXTENT)
//     {
//         return capabilities.currentExtent;
//     }
//     else
//     {
//         glm::ivec2 size = window_->GetFrameBufferSize();
//         VkExtent2D actualExtent = {
//             static_cast<uint_fast32_t>(size.x),
//             static_cast<uint_fast32_t>(size.y),
//         };

//         actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
//         actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

//         return actualExtent;
//     }
// }

// uint32_t VkEngine::PickSwapImageCount(const VkSurfaceCapabilitiesKHR& capabilities)
// {
//     uint32_t imageCount = capabilities.minImageCount + 1;
//     if ((capabilities.maxImageCount > 0) && capabilities.maxImageCount < imageCount)
//     {
//         imageCount = capabilities.maxImageCount;
//     }

//     return imageCount;
// }