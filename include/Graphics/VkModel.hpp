#ifndef GRAPHICS_VKMODEL_HPP
#define GRAPHICS_VKMODEL_HPP
#pragma once

#include <Graphics/Vulkan/VkInstanceImpl.hpp>

// External Lib
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

// STD Lib
#include <vector>

namespace Graphic
{

struct Vertex
{
    glm::vec2 position;
    glm::vec3 color;

    static std::vector<VkVertexInputBindingDescription> GetBindingDescriptions();
    static std::vector<VkVertexInputAttributeDescription> GetAttributeDescriptions();
};

class VkModel
{
public:
    VkModel(VkDeviceInstance* vkInstance, std::vector<Vertex>& vertices);
    ~VkModel();

    VkModel(const VkModel&) = delete;
    VkModel& operator=(const VkModel&) = delete;

    void Bind(VkCommandBuffer cmdBuffer);
    void Draw(VkCommandBuffer cmdBuffer);

private:

    void CreateVertexBuffers(const std::vector<Vertex>& vertices);

    VkDeviceInstance* vkInstance_ = VK_NULL_HANDLE;
    VkBuffer vertexBuffer_ = VK_NULL_HANDLE;
    VkDeviceMemory vertexBufferMem_ = VK_NULL_HANDLE;
    uint32_t vertexCount_ = 0;
};

} // namespace Graphic


#endif