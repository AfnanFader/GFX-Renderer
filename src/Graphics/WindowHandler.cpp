#include <Graphics/WindowHandler.ipp>

// External
#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>
#include <Logging.hpp>

namespace Graphic
{

// Callback method for error messages from GLFW
static void GlfwErrorCallback(int32_t errorCode, const char* message)
{
    LOG_ERROR("GLFW [Error : {}]: {}",errorCode ,message);
}

WindowHandler::WindowHandler(int32_t width, int32_t height) : width_(width), height_(height)
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

    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE); // Temporary setting for non resizable window for now.
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API); // Setto not load OpenGL by default.

    window_ = glfwCreateWindow(width_, height_, "GFX-Renderer", nullptr, nullptr); // Starting with hardcoded 16:9
    monitorArr_ = glfwGetMonitors(&monitorCount_);

    glfwSetWindowUserPointer(window_, this);
    glfwSetFramebufferSizeCallback(window_, FrameBufferResizeCallback);

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

VkExtent2D WindowHandler::GetWindowExtent()
{
    return {
        static_cast<uint32_t>(width_),
        static_cast<uint32_t>(height_)
    };
}

void WindowHandler::FrameBufferResizeCallback(GLFWwindow* window, int width, int height)
{
    auto pWin = reinterpret_cast<WindowHandler*>(glfwGetWindowUserPointer(window));
    pWin->frameBufferResized_ = true;
    pWin->width_ = width;
    pWin->height_ = height;
}

} // namespace Graphic
