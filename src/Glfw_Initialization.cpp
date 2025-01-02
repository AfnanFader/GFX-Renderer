#include "Glfw_Initialization.hpp"
#include "GLFW/glfw3.h"
#include "Common/Precomp.hpp"
#include "spdlog/spdlog.h"

namespace Window
{

// Callback method for error messages from GLFW
static void GlfwErrorCallback(int32_t errorCode, const char* message)
{
    spdlog::error("GLFW: {}", message);
}

GlfwInitialization::GlfwInitialization()
{
    glfwSetErrorCallback(GlfwErrorCallback);

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
