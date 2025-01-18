#include <GraphicsHandler.hpp>
#include <Input/InputHandler.ipp>

namespace Graphic
{

GraphicsHandler::GraphicsHandler() 
{
}

GraphicsHandler::~GraphicsHandler()
{
}

void GraphicsHandler::RenderLoop()
{
    glfwSetKeyCallback(window_.GetWindowHandlerPointer(), Input::KeyCallBack);

    while (!window_.ShouldCloseWindow())
    {
        glfwPollEvents();
    }

    Cleanup();
}

void GraphicsHandler::Cleanup()
{
}

} // namespace Graphic
