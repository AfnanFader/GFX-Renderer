#include <Application.hpp>

int32_t main(int argc, const char * argv[])
{
    Graphic::Application gfxHandler;

    gfxHandler.RenderLoop();

    return 0;
}