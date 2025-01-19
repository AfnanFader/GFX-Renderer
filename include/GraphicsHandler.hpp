#ifndef GRAPHICs_HANDLER_HPP
#define GRAPHICs_HANDLER_HPP

#include <Graphics/Vulkan/VkInstanceImpl.hpp>
#include <Graphics/Vulkan/VkPipelineImpl.hpp>
#include <Graphics/Vulkan/VkSwapChainImpl.hpp>
#include <Graphics/VkModel.hpp>
#include <Settings.hpp>

// STD Lib
#include <memory>
#include <vector>

namespace Graphic
{

class GraphicsHandler
{

public:

    GraphicsHandler();
    ~GraphicsHandler();

    GraphicsHandler(const GraphicsHandler&) = delete;
    GraphicsHandler &operator=(const GraphicsHandler&) = delete;

    void RenderLoop();

private:

    void loadModels();

    void CreatePipelineLayout();

    void CreatePipeline();

    void CreateCommandBuffer();

    void RecordCommandBuffer(int imageIndex);

    void DrawFrame();

    void Cleanup();

    void RecreateSwapChain();

    WindowHandler window_{DISPLAY_WIDTH,DISPLAY_HEIGHT};
    VkDeviceInstance vkInstance_{&window_};
    
    std::unique_ptr<SwapChainInstance> swapChainInst_;

    VkPipelineLayout pipelineLayout_ = VK_NULL_HANDLE;
    std::unique_ptr<GraphicPipeline> pipeline_;
    std::vector<VkCommandBuffer> commandBuffer_;

    std::unique_ptr<VkModel> vkModel_;
};

} // namespace Graphic


#endif