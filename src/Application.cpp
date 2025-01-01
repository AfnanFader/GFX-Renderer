#include <GLFW/glfw3.h>
#include <Glfw_Monitor.hpp>
#include <Glfw_Initialization.hpp>
#include <Precomp.hpp>

int32_t main(int argc, const char * argv[])
{
    window::GlfwInitialization const _glfw;

    // TODO#2 Add null checking method
    GLFWwindow* windowPtr = glfwCreateWindow(854, 480, "GFX-Renderer", nullptr, nullptr); // Starting with hardcoded 16:9
    GLFWmonitor** monitoArrPtr = window::GetMonitor();

    if (monitoArrPtr != nullptr)
    {
        window::MoveWindowToMonitor(windowPtr, monitoArrPtr);
    }

    while (!glfwWindowShouldClose(windowPtr))
    {
        glfwPollEvents();
    }

    return EXIT_SUCCESS;
}
