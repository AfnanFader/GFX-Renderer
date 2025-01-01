#include <GLFW/glfw3.h>
#include <Glfw_Initialization.hpp>
#include <Glfw_Window.hpp>
#include <Precomp.hpp>
#include <VK_Renderer.hpp>

int32_t main(int argc, const char * argv[])
{
    Window::GlfwInitialization const _glfw;
    Window::WindowHandler window;
    Renderer::VkGraphic vkRenderer(&window);

    while (!window.ShouldCloseWindow())
    {
        glfwPollEvents();
    }

    return EXIT_SUCCESS;
}
