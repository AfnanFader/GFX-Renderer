#include <Glfw_Initialization.hpp>
#include <GLFW/glfw3.h>
#include <Precomp.hpp>

namespace window
{

GlfwInitialization::GlfwInitialization()
{
    if (glfwInit() != GLFW_TRUE)
    {
        //TODO#1 - Add wrapper for exit and logging.
        std::exit(EXIT_FAILURE);
    }
}

GlfwInitialization::~GlfwInitialization() { glfwTerminate(); }

} // namespace window
