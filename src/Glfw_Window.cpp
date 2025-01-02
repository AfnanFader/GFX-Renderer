#include "GLFW/glfw3.h"
#include "Glfw_Monitor.hpp"
#include "Glfw_Window.hpp"
#include "Common/Precomp.hpp"
#include "spdlog/spdlog.h"

namespace Window
{

WindowHandler::WindowHandler()
{
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE); // Temporary setting for non resizable window for now.
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API); // Setto not load OpenGL by default.

    windowPtr_ = glfwCreateWindow(854, 480, "GFX-Renderer", nullptr, nullptr); // Starting with hardcoded 16:9
    monitoArrPtr_ = Window::GetMonitor();

    if (windowPtr_ == nullptr || monitoArrPtr_ == nullptr)
    {
        spdlog::critical("WindowHandler: Initialization Failed");
        std::exit(EXIT_FAILURE);
    }

    // By default let it go to center.
    Window::MoveWindowToMonitor(windowPtr_ , monitoArrPtr_);
}

WindowHandler::~WindowHandler()
{
    spdlog::info("WindowHandler: Terminate WindowHandler");
    glfwDestroyWindow(windowPtr_);
}

bool WindowHandler::ShouldCloseWindow() const
{
    return glfwWindowShouldClose(windowPtr_);
}

GLFWwindow* WindowHandler::GetWindowHandlerPointer() const
{
    return windowPtr_;
}
    
} // namespace Window
