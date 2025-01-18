#include <Graphics/WindowHandler.hpp>
#include <GLFW/glfw3.h>
#include <Logging.hpp>
#include <Settings.hpp>

namespace Graphic
{

// Callback method for error messages from GLFW
static void GlfwErrorCallback(int32_t errorCode, const char* message)
{
    LOG_ERROR("GLFW [Error : {}]: {}",errorCode ,message);
}

WindowHandler::WindowHandler()
{
    InitWindow();
}

WindowHandler::~WindowHandler()
{
    if (window_ != nullptr)
    {
        glfwDestroyWindow(window_);
    }

    glfwTerminate();
    LOG_INFO("GLFW : Terminate GLFW");
}

void WindowHandler::InitWindow()
{
    glfwSetErrorCallback(GlfwErrorCallback);

    if (glfwInit() != GLFW_TRUE)
    {
        LOG_ERROR_EXIT("GLFW : Initialization Failed !");
    }

    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE); // Temporary setting for non resizable window for now.
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API); // Setto not load OpenGL by default.

    window_ = glfwCreateWindow(DISPLAY_WIDTH, DISPLAY_HEIGHT, "GFX-Renderer", nullptr, nullptr); // Starting with hardcoded 16:9
    monitorArr_ = glfwGetMonitors(&monitorCount_);

    if (window_ == nullptr || monitorArr_ == nullptr)
    {
        LOG_ERROR_EXIT("GLFW Init : Null Pointer detected");
    }
}

bool WindowHandler::ShouldCloseWindow() const
{
    return glfwWindowShouldClose(window_);
}

GLFWwindow* WindowHandler::GetWindowHandlerPointer() const
{
    return window_;
}

glm::ivec2 WindowHandler::GetFrameBufferSize() const
{
    glm::ivec2 windowSize;
    glfwGetWindowSize(window_, &windowSize.x, &windowSize.y);
    return windowSize;
}

// void MoveWindowToMonitor(GLFWwindow* const windowPtr, GLFWmonitor** const monitorArrPtr)
// {
//     ivec2 window_size;
//     ivec2 monitor_coordinates; // X & Y coordinate of the virtual screen.
//     ivec2 monitor_size;

//     glfwGetWindowSize(windowPtr, &window_size.x, &window_size.y);
//     glfwGetMonitorPos(monitorArrPtr[0] , &monitor_coordinates.x, &monitor_coordinates.y);
//     glfwGetMonitorWorkarea(monitorArrPtr[0], nullptr, nullptr, &monitor_size.x, &monitor_size.y);
    
//     // Caculate the center of monitor.
//     ivec2 center_coordinates = monitor_coordinates + (monitor_size/2) - (window_size/2);

//     glfwSetWindowPos(windowPtr, center_coordinates.x, center_coordinates.y);
// }

} // namespace Graphic
