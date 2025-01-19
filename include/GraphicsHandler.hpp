#ifndef GRAPHICs_HANDLER_HPP
#define GRAPHICs_HANDLER_HPP

#include <Graphics/Vulkan/VkInstanceImpl.hpp>
#include <Graphics/Vulkan/VkPipelineImpl.hpp>
#include <Graphics/Vulkan/VkSwapChainImpl.hpp>
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

    void CreatePipelineLayout();

    void CreatePipeline();

    void CreateCommandBuffer();

    void DrawFrame();

    void Cleanup();

    WindowHandler window_{DISPLAY_WIDTH,DISPLAY_HEIGHT};
    VkDeviceInstance vkInstance_{&window_};
    SwapChainInstance swapChainInst_{&vkInstance_, window_.GetWindowExtent()};
    std::unique_ptr<GraphicPipeline> pipeline_;
    VkPipelineLayout pipelineLayout_ = VK_NULL_HANDLE;
    std::vector<VkCommandBuffer> commandBuffer_;
};

} // namespace Graphic


#endif