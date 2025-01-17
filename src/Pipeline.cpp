#include <Pipeline.hpp>
#include <Logging.hpp>

#include <fstream>

namespace Graphic
{
    
Pipeline::Pipeline(const std::string& vertFilePath, const std::string& fragFilePath)
{
    // Note : PROJECT_DIRECTORY macro is added by CMakeList.txt
    auto const vertPath = (std::string)PROJECT_DIRECTORY + vertFilePath;
    auto const fragPath = (std::string)PROJECT_DIRECTORY + fragFilePath; 
    CreateGraphicPipeline(vertPath, fragPath);
}

Pipeline::~Pipeline() {}

std::vector<char> Pipeline::ReadFile(const std::string& filePath)
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

void Pipeline::CreateGraphicPipeline(const std::string& vertFilePath, const std::string& fragFilePath)
{
    auto vertBin = ReadFile(vertFilePath);
    auto fragBin = ReadFile(fragFilePath);

    LOG_INFO("Size of Vertex Shader Code : {}", vertBin.size());
    LOG_INFO("Size of Fragment Shader Code : {}", fragBin.size());
}

} // namespace Graphic

