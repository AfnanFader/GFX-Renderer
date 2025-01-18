#ifndef GRAPHICs_HANDLER_HPP
#define GRAPHICs_HANDLER_HPP

#include <Graphics/Vulkan/VkEngine.hpp>
#include <Graphics/RenderPipeline.hpp>

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
    VkEngine vkInstance_{&window_};
    RenderPipeline pipe_{
        &vkInstance_,
        VERT_SHADER_PATH,
        FRAG_SHADER_PATH,
        RenderPipeline::DefaultPipeLineConfigInfo(DISPLAY_WIDTH, DISPLAY_HEIGHT)};
};

} // namespace Graphic


#endif