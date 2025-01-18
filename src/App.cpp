#include <GraphicsHandler.hpp>

int32_t main(int argc, const char * argv[])
{
    Graphic::GraphicsHandler gfxHandler;

    gfxHandler.RenderLoop();

    return 0;
}