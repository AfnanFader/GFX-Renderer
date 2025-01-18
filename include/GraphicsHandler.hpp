#ifndef GRAPHICs_HANDLER_HPP
#define GRAPHICs_HANDLER_HPP

#include <Graphics/Vulkan/VkInstanceImpl.hpp>
#include <Graphics/Vulkan/VkPipelineImpl.hpp>

namespace Graphic
{

class GraphicsHandler
{

public:

    GraphicsHandler();
    ~GraphicsHandler();

    void RenderLoop();

private:

    void Cleanup();

    WindowHandler window_{};
    VkDeviceInstance vkInstance_{&window_};
    GraphicPipeline pipe_{
        &vkInstance_,
        VERT_SHADER_PATH,
        FRAG_SHADER_PATH,
        GraphicPipeline::DefaultPipeLineConfigInfo(DISPLAY_WIDTH, DISPLAY_HEIGHT)};
};

} // namespace Graphic


#endif