#ifndef PIPELINE_HPP
#define PIPELINE_HPP
#pragma once

#include <string>
#include <vector>

namespace Graphic
{

class Pipeline
{

public:
    Pipeline(const std::string& vertFilePath, const std::string& fragFilePath);
    ~Pipeline();

private:
    std::vector<char> ReadFile(const std::string& filePath);

    void CreateGraphicPipeline(const std::string& vertFilePath, const std::string& fragFilePath);
};
    
} // namespace Graphic


#endif