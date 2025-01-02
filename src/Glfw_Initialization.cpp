#include "Glfw_Initialization.hpp"
#include "GLFW/glfw3.h"
#include "Common/Precomp.hpp"
#include "spdlog/spdlog.h"

namespace Window
{

GlfwInitialization::GlfwInitialization()
{
    if (glfwInit() != GLFW_TRUE)
    {
        spdlog::critical("GLFW: Initialization Failed !");
        std::exit(EXIT_FAILURE);
    }
}

GlfwInitialization::~GlfwInitialization() 
{
    spdlog::info("GLFW: Terminate GLFW");
    glfwTerminate();
}

} // namespace Window
