#include "Glfw_Initialization.hpp"
#include "GLFW/glfw3.h"
#include "Common/Precomp.hpp"

namespace Window
{

GlfwInitialization::GlfwInitialization()
{
    if (glfwInit() != GLFW_TRUE)
    {
        //TODO#1 - Add wrapper for exit and logging.
        std::exit(EXIT_FAILURE);
    }
}

GlfwInitialization::~GlfwInitialization() 
{
    std::cout << "[INFO] Terminate GLFW" << std::endl; 
    glfwTerminate();
}

} // namespace Window
