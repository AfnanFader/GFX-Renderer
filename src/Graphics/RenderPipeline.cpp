#include <Graphics/RenderPipeline.hpp>
#include <Logging.hpp>

#include <fstream>

namespace Graphic
{
    
RenderPipeline::RenderPipeline(
    VkEngine* instance,
    const std::string& vertFilePath,
    const std::string& fragFilePath,
    const PipelineConfigInfo& configInfo) : device_(instance->GetLogicalDevice())
{
    // Note : PROJECT_DIRECTORY macro is added by CMakeList.txt
    auto const vertPath = (std::string)PROJECT_DIRECTORY + vertFilePath;
    auto const fragPath = (std::string)PROJECT_DIRECTORY + fragFilePath; 
    CreateGraphicsPipeline(vertPath, fragPath, configInfo);
}

RenderPipeline::~RenderPipeline()
{
    device_ = VK_NULL_HANDLE;
}

std::vector<char> RenderPipeline::ReadFile(const std::string& filePath)
{
    std::ifstream file(filePath, std::ios::ate | std::ios::binary);

    if (!file.is_open())
    {
        LOG_WARN("Failed to load shader binary : {}",filePath);
    }

    // Initialize buffer for the binary.
    size_t fileSize = static_cast<size_t>(file.tellg());
    std::vector<char> buffer(fileSize);

    file.seekg(0); // Reset reader to first char
    file.read(buffer.data(), fileSize);

    file.close();
    return buffer;
}

void RenderPipeline::CreateGraphicsPipeline(const std::string& vertFilePath, const std::string& fragFilePath, const PipelineConfigInfo& configInfo)
{
    auto vertBin = ReadFile(vertFilePath);
    auto fragBin = ReadFile(fragFilePath);

    LOG_INFO("Size of Vertex Shader Code : {}", vertBin.size());
    LOG_INFO("Size of Fragment Shader Code : {}", fragBin.size());
}

void RenderPipeline::createShaderModule(const std::vector<char>& code, VkShaderModule* shaderModule)
{
    VkShaderModuleCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = code.size();
    createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

    if (vkCreateShaderModule(device_, &createInfo, nullptr, shaderModule) != VK_SUCCESS)
    {
        LOG_ERROR("Pipeline: Failed to create shader module");
    }
}

PipelineConfigInfo RenderPipeline::DefaultPipeLineConfigInfo(uint32_t width, uint32_t height)
{
    PipelineConfigInfo configInfo{};

    configInfo.inputAssemblyInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    configInfo.inputAssemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    configInfo.inputAssemblyInfo.primitiveRestartEnable = VK_FALSE;
    configInfo.inputAssemblyInfo.pNext = nullptr;

    return configInfo;
}

} // namespace Graphic

