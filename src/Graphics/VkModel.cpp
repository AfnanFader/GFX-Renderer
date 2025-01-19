#include <Graphics/Vulkan/VkUtil.ipp>
#include <Graphics/VkModel.ipp>

#include <Logging.hpp>

namespace Graphic
{

VkModel::VkModel(
    VkDeviceInstance* vkInstance,
    std::vector<Vertex>& vertices) : vkInstance_(vkInstance)
{
    CreateVertexBuffers(vertices);
}

VkModel::~VkModel()
{
    vkDestroyBuffer(vkInstance_->GetLogicalDevice(), vertexBuffer_, nullptr);
    vkFreeMemory(vkInstance_->GetLogicalDevice(), vertexBufferMem_, nullptr);
}

void VkModel::CreateVertexBuffers(const std::vector<Vertex>& vertices)
{
    vertexCount_ = static_cast<uint32_t>(vertices.size());

    if (vertexCount_ < 3)
    {
        LOG_ERROR("Model Binding: Vertex count is less than 3 !");
    }

    VkDeviceSize bufferSize = (sizeof(vertices[0]) * vertexCount_);

    vkInstance_->CreateBuffer(
        bufferSize,
        VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        vertexBuffer_,
        vertexBufferMem_
    );

    void* data;
    VK_CHECK(
        vkMapMemory(vkInstance_->GetLogicalDevice(), vertexBufferMem_, 0, bufferSize, 0, &data),
        "Failed to map memory !"
    );
    memcpy(data, vertices.data(), static_cast<size_t>(bufferSize));
    vkUnmapMemory(vkInstance_->GetLogicalDevice(), vertexBufferMem_);
}

void VkModel::Bind(VkCommandBuffer cmdBuffer)
{
    VkBuffer buffers[] = {vertexBuffer_};
    VkDeviceSize offsets[] = {0};
    vkCmdBindVertexBuffers(cmdBuffer, 0, 1, buffers, offsets);
}

void VkModel::Draw(VkCommandBuffer cmdBuffer)
{
    vkCmdDraw(cmdBuffer, vertexCount_, 1, 0, 0);
}

std::vector<VkVertexInputBindingDescription> Vertex::GetBindingDescriptions()
{
    std::vector<VkVertexInputBindingDescription> bindingDesc(1);
    bindingDesc[0].binding = 0;
    bindingDesc[0].stride = sizeof(Vertex);
    bindingDesc[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
    return bindingDesc;
}

std::vector<VkVertexInputAttributeDescription> Vertex::GetAttributeDescriptions()
{
    std::vector<VkVertexInputAttributeDescription> attributeDesc(2);
    attributeDesc[0].binding = 0;
    attributeDesc[0].location = 0;
    attributeDesc[0].offset = 0;
    attributeDesc[0].format = VK_FORMAT_R32G32_SFLOAT;

    attributeDesc[1].binding = offsetof(Vertex, position);;
    attributeDesc[1].location = 1;
    attributeDesc[1].offset = offsetof(Vertex, color);
    attributeDesc[1].format = VK_FORMAT_R32G32B32_SFLOAT;
    return attributeDesc;
}

} // namespace Graphic
