#include <Graphics/WindowHandler.hpp>
#include <Input/InputHandler.ipp>

// External Lib
#include <GLFW/glfw3.h>
#include <Logging.hpp>

namespace Input
{
    
void KeyCallBack(GLFWwindow* window, int32_t key, int32_t scancode, int32_t action, int32_t mods)
{
    if (key == GLFW_KEY_ESCAPE)
    {
        glfwSetWindowShouldClose(window, true);
    }

    if ((key == GLFW_KEY_H) && (action == GLFW_PRESS) && (mods & GLFW_MOD_CONTROL))
    {
        LOG_ERROR_EXIT("TEST ERROR KEY");
    }
}

} // namespace Input

