#include <Graphics/Vulkan/VkInstanceImpl.ipp>
#include <Graphics/Vulkan/VkUtil.ipp>
#include <Graphics/WindowHandler.hpp>

// External
#include <glm/glm.hpp>
#include <Logging.hpp>

// std
#include <algorithm>
#include <set>
#include <iostream>

namespace Graphic
{

// Logging message callback to be used in Vulkan Instance.
VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                             VkDebugUtilsMessageTypeFlagsEXT messageType,
                                             const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
                                             void* pUserData)
{
    switch(static_cast<VkDebugMessageSeverity>(messageSeverity))
    {
        case VERSBOSE:
        case INFO:
            LOG_INFO("VK Validation: {}",pCallbackData->pMessage);
            break;
        case WARNING:
            LOG_WARN("VK Validation: {}",pCallbackData->pMessage);
            break;
        case ERROR:
            LOG_ERROR("VK Validation: {}",pCallbackData->pMessage);
            break;
        default:
            LOG_ERROR_EXIT("UNKNOWN VK MESSAGE: {}",pCallbackData->pMessage);
    }

    return VK_FALSE;
}

VKAPI_ATTR VkResult VKAPI_CALL CreateDebugUtilsMessengerEXT(
    VkInstance instance,
    const VkDebugUtilsMessengerCreateInfoEXT* pInfo,
    const VkAllocationCallbacks* pAllocator,
    VkDebugUtilsMessengerEXT* debugMessenger)
{
    PFN_vkCreateDebugUtilsMessengerEXT func =
    reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT"));

    if (func != nullptr)
    {
        return func(instance, pInfo, pAllocator, debugMessenger);
    }
    else
    {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

VKAPI_ATTR void VKAPI_CALL DestroyDebugUtilsMessengerEXT(
    VkInstance instance,
    VkDebugUtilsMessengerEXT debugMessenger,
    const VkAllocationCallbacks* pAllocator)
{
    PFN_vkDestroyDebugUtilsMessengerEXT func =
    reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT"));

    if (func != nullptr)
    {
        return func(instance, debugMessenger, pAllocator);
    }
}

//----------------------------------------------------------------------------//

// Types of Validation/Debug messages to be subscribed.
inline VkDebugUtilsMessengerCreateInfoEXT GetDebugMessengerCreateInfo()
{
    VkDebugUtilsMessengerCreateInfoEXT msgCreationInfo = {};
    msgCreationInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    msgCreationInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | 
                                      VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT |
                                      VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT;
    msgCreationInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                                  VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT;
    msgCreationInfo.pfnUserCallback = DebugCallback;
    msgCreationInfo.pUserData = nullptr;

    return msgCreationInfo;
}

inline std::vector<const char*> GetGLFWRequiredExtensions()
{
    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions;

    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    return std::vector<const char*>(glfwExtensions, glfwExtensions + glfwExtensionCount);
}

inline std::vector<VkExtensionProperties> GetAvailableInstanceExtensions()
{
    uint32_t extensionCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

    if (extensionCount == 0) { return {}; }

    std::vector<VkExtensionProperties> availableProperties(extensionCount);
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, availableProperties.data());

    return availableProperties;
}

inline std::vector<VkLayerProperties> GetAvailableValidationLayers()
{
    uint32_t layerCount = 0;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

    if (layerCount == 0) { return {}; }

    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());
    
    return availableLayers;
}

inline std::vector<VkExtensionProperties> GetAvailableDeviceExtensions(VkPhysicalDevice device)
{
    uint32_t allDevExtCount = 0;
    vkEnumerateDeviceExtensionProperties(device, nullptr, &allDevExtCount, nullptr);

    if (allDevExtCount == 0) { return {}; }

    std::vector<VkExtensionProperties> availableProperties(allDevExtCount);
    vkEnumerateDeviceExtensionProperties(device, nullptr, &allDevExtCount, availableProperties.data());

    return availableProperties;
}

inline std::vector<VkPhysicalDevice> GetAvailableDevices(VkInstance instance)
{
    uint32_t phyDevCount = 0;
    vkEnumeratePhysicalDevices(instance, &phyDevCount, nullptr);

    if (phyDevCount == 0) { return {}; }; 

    std::vector<VkPhysicalDevice> availPhysicalDevices(phyDevCount);
    vkEnumeratePhysicalDevices(instance, &phyDevCount, availPhysicalDevices.data());

    return availPhysicalDevices;
}

//----------------------------------------------------------------------------//


VkDeviceInstance::VkDeviceInstance(WindowHandler* window)
{
    window_ = window;
    InitVulkan();
}

VkDeviceInstance::~VkDeviceInstance()
{
    if (logicalDevice_ != VK_NULL_HANDLE)
    {
        if (commandPool_ != VK_NULL_HANDLE)
        {
            vkDestroyCommandPool(logicalDevice_, commandPool_, nullptr);
            LOG_INFO("VK Instance: Terminate CommandPool");
        }

        LOG_INFO("VK Instance: Terminate Logical Device");
        vkDestroyDevice(logicalDevice_, nullptr);
    }

    if (instance_ != VK_NULL_HANDLE)
    {
        if (surfaceKHR_ != VK_NULL_HANDLE)
        {
            vkDestroySurfaceKHR(instance_, surfaceKHR_, nullptr);            
            LOG_INFO("VK Instance: Terminate Surface KHR");
        }

        if ((debugMessenger_ != VK_NULL_HANDLE) || debuggingEnabled_)
        {
            LOG_INFO("VK Instance: Terminate DebugMessenger");
            DestroyDebugUtilsMessengerEXT(instance_, debugMessenger_, nullptr);
        }

        LOG_INFO("VK Instance: Terminate VkInstance");
        vkDestroyInstance(instance_, nullptr);
    }
}

void VkDeviceInstance::InitVulkan()
{
    CreateInstance();
    SetupDebugMessenger(); // Debug - Optional and be disabled
    CreateSurface();
    PickPhysicalDevice();
    CreateLogicalDeviceAndQueue();
    CreateCommandPool();
}

//----------------------------------------------------------------------------//

void VkDeviceInstance::CreateInstance()
{
    VkDebugUtilsMessengerCreateInfoEXT msgCreationInfo = GetDebugMessengerCreateInfo();
    std::vector<const char*> supportedInstanceExt = GetSupportedInstanceExtensions();

    VkApplicationInfo appInfo = {};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pNext = nullptr;
    appInfo.pApplicationName = "GfxRenderer";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "VkRender";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_3;

    VkInstanceCreateInfo instanceCreationInfo = {};
    instanceCreationInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instanceCreationInfo.pNext = debuggingEnabled_ ? &msgCreationInfo : nullptr;
    instanceCreationInfo.pApplicationInfo = &appInfo;
    instanceCreationInfo.ppEnabledExtensionNames = supportedInstanceExt.data();
    instanceCreationInfo.enabledExtensionCount = static_cast<uint32_t>(supportedInstanceExt.size());
    instanceCreationInfo.ppEnabledLayerNames = debuggingEnabled_ ? requiredValidationLayers.data() : nullptr;
    instanceCreationInfo.enabledLayerCount = debuggingEnabled_ ? static_cast<uint32_t>(requiredValidationLayers.size()) : 0;

#ifdef __APPLE__
    // This flag is require for MoltenVK
    instanceCreationInfo.flags = VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
    LOG_WARN("VK Instance: Enumerate Portability Bit for MoltenVK");
#endif

    VK_CHECK(
        vkCreateInstance(&instanceCreationInfo, nullptr, &instance_),
        "VK Instance: Failed to initialize instance"
    )
}

std::vector<const char*> VkDeviceInstance::GetSupportedInstanceExtensions()
{
    std::vector<const char*> reqInstExt = GetGLFWRequiredExtensions();
    std::vector<VkExtensionProperties> availInstExt = GetAvailableInstanceExtensions();
    std::vector<const char*> supInstExt;

#ifdef __APPLE__
    reqInstExt.push_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
    LOG_WARN("VK Instance: Vulkan Portability Enumeration Extension Added");
#endif

    if (!CheckSupportedValidationLayers())
    {
        debuggingEnabled_ = false; // Disable it here since it is not supported
    }
    else
    {
        // Only append this if Vulkan validation layer is supported.
        if (debuggingEnabled_)
        {
            reqInstExt.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
            LOG_WARN("VK Instance: Vulkan Validation/Debugging mode enabled");
        }
    }

    CheckVkSupportedExtProperties(reqInstExt, availInstExt, supInstExt);

    return supInstExt;
}

bool VkDeviceInstance::CheckSupportedValidationLayers()
{
    std::vector<VkLayerProperties> availableLayers = GetAvailableValidationLayers();

    // Check if all required layers are available
    for (const char* required : requiredValidationLayers)
    {
        bool found = false;
        for (const auto& layers : availableLayers)
        {
            if (strcmp(required, layers.layerName) == 0)
            {
                LOG_INFO("VK Instance: Supported Validation Layers -> {}",layers.layerName);
                found = true;

                break;
            }
        }
        if (!found)
        {
            LOG_ERROR("VK Instance: Required Validation Layers not found");
            return false;
        }
    }

    return true;
}

//----------------------------------------------------------------------------//

void VkDeviceInstance::CreateSurface()
{
    VK_CHECK(
        glfwCreateWindowSurface(instance_, window_->GetWindowHandlerPointer(), nullptr, &surfaceKHR_),
        "VK Instance: Failed to create surface."
    )
}

//----------------------------------------------------------------------------//

void VkDeviceInstance::SetupDebugMessenger()
{
    if (!debuggingEnabled_) { return; }

    VkDebugUtilsMessengerCreateInfoEXT msgCreationInfo = GetDebugMessengerCreateInfo();

    VK_CHECK(
        CreateDebugUtilsMessengerEXT(instance_, &msgCreationInfo, nullptr, &debugMessenger_),
        "VK Instance: Debug Messenger setup failed .."
    )
}

//----------------------------------------------------------------------------//

void VkDeviceInstance::PickPhysicalDevice()
{
    std::vector<VkPhysicalDevice> availPhysicalDevices = GetAvailableDevices(instance_);

    // Filter out non compatible devices.
    for (uint32_t index = 0; index < availPhysicalDevices.size(); ++index)
    {
        if (!IsDeviceCompatible(availPhysicalDevices[index]))
        {
            availPhysicalDevices.erase(availPhysicalDevices.begin() + index);
        }
    }

    if (availPhysicalDevices.empty())
    {
        LOG_CRITICAL_EXIT("VK Instance: No available GPU to be binded.");
    }

    // Future logic for choosing with GPU

    physicalDevice_ = availPhysicalDevices[0];
    vkGetPhysicalDeviceProperties(physicalDevice_, &phyDevProperties_);
    LOG_INFO("Vk Instance: Acquired {}",phyDevProperties_.deviceName);
}

bool VkDeviceInstance::IsDeviceCompatible(VkPhysicalDevice device)
{
    return (FindQueueFamilies(device, surfaceKHR_).IsComplete() &&
            !GetSupportedDeviceExtensions(device).empty() &&
            GetSwapChainSupport(device, surfaceKHR_).IsValid());
}

std::vector<const char*> VkDeviceInstance::GetSupportedDeviceExtensions(VkPhysicalDevice device)
{
    std::vector<VkExtensionProperties> availDevExt = GetAvailableDeviceExtensions(device);
    std::vector<const char*> supportedDevExt;

    CheckVkSupportedExtProperties(requiredDevExt, availDevExt, supportedDevExt);
    return supportedDevExt;
}

//----------------------------------------------------------------------------//

void VkDeviceInstance::CreateLogicalDeviceAndQueue()
{
    QueueFamilyIndices familyIndices = FindQueueFamilies(physicalDevice_, surfaceKHR_);
    std::vector<const char*> availableDevExt = GetSupportedDeviceExtensions(physicalDevice_);
    std::set<uint32_t> queueFamilyIndex = {familyIndices.graphicsFamilyIdx, familyIndices.presentFamilyIdx};
    std::vector<VkDeviceQueueCreateInfo> queueCreateList;
    float queuePriority = 1.0f;

    LOG_INFO("Vk Instance: GraphicIdx {} - PresentIdx {}",familyIndices.graphicsFamilyIdx ,familyIndices.presentFamilyIdx);

    for (uint32_t index : queueFamilyIndex)
    {
        VkDeviceQueueCreateInfo queueCreateInfo = {};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = index;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;
        queueCreateInfo.pNext = nullptr;
        queueCreateList.push_back(queueCreateInfo);
    }

    VkPhysicalDeviceFeatures phyDevFeatures = {};

    VkDeviceCreateInfo deviceCreateInfo = {};
    deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateList.size());
    deviceCreateInfo.pQueueCreateInfos = queueCreateList.data();
    deviceCreateInfo.pEnabledFeatures = &phyDevFeatures;
    deviceCreateInfo.enabledExtensionCount = availableDevExt.size();
    deviceCreateInfo.ppEnabledExtensionNames = availableDevExt.data();

    VK_CHECK(
        vkCreateDevice(physicalDevice_, &deviceCreateInfo, nullptr, &logicalDevice_),
        "VK Instance: Logical Device creation failed."
    )

    vkGetDeviceQueue(logicalDevice_, familyIndices.graphicsFamilyIdx, 0, &graphicsQueue_);
    vkGetDeviceQueue(logicalDevice_, familyIndices.presentFamilyIdx, 0, &presentQueue_);
}

//----------------------------------------------------------------------------//

void VkDeviceInstance::CreateCommandPool()
{
    QueueFamilyIndices indices = FindQueueFamilies(physicalDevice_, surfaceKHR_);

    VkCommandPoolCreateInfo poolInfo = {};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.pNext = nullptr;
    poolInfo.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT | VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

    VK_CHECK(
        vkCreateCommandPool(logicalDevice_, &poolInfo, nullptr, &commandPool_),
        "VK Instance: Failed to create comamnd pool"
    )
}

//----------------------------------------------------------------------------//

VkFormat VkDeviceInstance::FindSupportedFormat(
        const std::vector<VkFormat>& candidate,
        VkImageTiling tiling,
        VkFormatFeatureFlags features)
{
    for (auto format : candidate)
    {
        VkFormatProperties properties;
        vkGetPhysicalDeviceFormatProperties(physicalDevice_, format, &properties);

        if ((tiling == VK_IMAGE_TILING_LINEAR) && ((properties.linearTilingFeatures && features) == features))
        {
            return format;
        }
        else if ((tiling == VK_IMAGE_TILING_OPTIMAL) && ((properties.optimalTilingFeatures & features) == features))
        {
            return format;
        }
    }

    LOG_WARN("Vk Instance: Failed to find supported format !!");

    // @todo Need default fallback here
}

uint32_t VkDeviceInstance::FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties)
{
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(physicalDevice_, &memProperties);

    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
    {
        if ((typeFilter & (1 << i)) && ((memProperties.memoryTypes[i].propertyFlags & properties) == properties))
        {
            return i;
        }
    }

    LOG_WARN("Vk Instance: Failed to find suitable memory type !!");

    // @todo Need default fallback here
}

//----------------------------------------------------------------------------//

void VkDeviceInstance::CreateBuffer(
    VkDeviceSize devSize,
    VkBufferUsageFlags usage,
    VkMemoryPropertyFlags properties,
    VkBuffer& buffer,
    VkDeviceMemory& bufferMem)
{
    VkBufferCreateInfo bufferInfo = {};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = devSize;
    bufferInfo.usage = usage;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    VK_CHECK(
        vkCreateBuffer(logicalDevice_, &bufferInfo, nullptr, &buffer),
        "VK Instace: Failed to create vertex buffer !!"
    )

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(logicalDevice_, buffer, &memRequirements);

    VkMemoryAllocateInfo memAllocInfo = {};
    memAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    memAllocInfo.allocationSize = memRequirements.size;
    memAllocInfo.memoryTypeIndex = FindMemoryType(memRequirements.memoryTypeBits, properties);

    VK_CHECK(
        vkAllocateMemory(logicalDevice_, &memAllocInfo, nullptr, &bufferMem),
        "VK Instace: Failed to allocate vertex buffer memory !!"
    )

    

    vkBindBufferMemory(logicalDevice_, buffer, bufferMem, 0);
}
    
VkCommandBuffer VkDeviceInstance::BeginSingleTimeCommands()
{
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = commandPool_;
    allocInfo.commandBufferCount = 1;

    VkCommandBuffer commandBuffer;
    vkAllocateCommandBuffers(logicalDevice_, &allocInfo, &commandBuffer);

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(commandBuffer, &beginInfo);
    return commandBuffer;
}
    
void VkDeviceInstance::EndSingleTimeCommands(VkCommandBuffer cmdBuffer)
{
    vkEndCommandBuffer(cmdBuffer);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &cmdBuffer;

    vkQueueSubmit(graphicsQueue_, 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(graphicsQueue_);

    vkFreeCommandBuffers(logicalDevice_, commandPool_, 1, &cmdBuffer);
}

//----------------------------------------------------------------------------//

void VkDeviceInstance::CopyBuffer(
    VkBuffer srcBuffer,
    VkBuffer dstBuffer,
    VkDeviceSize devSize)
{
    VkCommandBuffer commandBuffer = BeginSingleTimeCommands();

    VkBufferCopy copyRegion{};
    copyRegion.srcOffset = 0;  // Optional
    copyRegion.dstOffset = 0;  // Optional
    copyRegion.size = devSize;
    vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

    EndSingleTimeCommands(commandBuffer);
}

void VkDeviceInstance::CopyBufferToImage(
    VkBuffer buffer,
    VkImage image,
    uint32_t width,
    uint32_t height,
    uint32_t layerCount)
{
    VkCommandBuffer commandBuffer = BeginSingleTimeCommands();

    VkBufferImageCopy region{};
    region.bufferOffset = 0;
    region.bufferRowLength = 0;
    region.bufferImageHeight = 0;

    region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    region.imageSubresource.mipLevel = 0;
    region.imageSubresource.baseArrayLayer = 0;
    region.imageSubresource.layerCount = layerCount;

    region.imageOffset = {0, 0, 0};
    region.imageExtent = {width, height, 1};

    vkCmdCopyBufferToImage(
        commandBuffer,
        buffer,
        image,
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        1,
        &region);
    EndSingleTimeCommands(commandBuffer);
}

void VkDeviceInstance::CreateImageWithInfo(
    const VkImageCreateInfo& imageInfo,
    VkMemoryPropertyFlags properties,
    VkImage& image,
    VkDeviceMemory& imageMem)
{
    VK_CHECK(
        vkCreateImage(logicalDevice_, &imageInfo, nullptr, &image),
        "Failed to create image!"
    )
    
    VkMemoryRequirements memRequirements;
    vkGetImageMemoryRequirements(logicalDevice_, image, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = FindMemoryType(memRequirements.memoryTypeBits, properties);

    VK_CHECK(
        vkAllocateMemory(logicalDevice_, &allocInfo, nullptr, &imageMem),
        "Failed to allocate image memory!"
    )

    VK_CHECK(
        vkBindImageMemory(logicalDevice_, image, imageMem, 0),
        "Failed to bind image memory!"
    )
}

} // namespace Graphic