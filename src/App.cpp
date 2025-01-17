#include <Graphics/Vulkan/VulkanInitializer.hpp>
#include <Pipeline.hpp>
#include <GLFW/glfw3.h>

#define VERT_SHADER_PATH "/Assets/Compiled_Shaders/simple_vert_shader.spv"
#define FRAG_SHADER_PATH "/Assets/Compiled_Shaders/simple_frag_shader.spv"

//----------------------------------------------------------------------------//

int32_t main(int argc, const char * argv[])
{
    Graphic::WindowHandler window;
    Graphic::VulkanInitializer vkGraphic(&window);
    Graphic::Pipeline(VERT_SHADER_PATH,FRAG_SHADER_PATH);

    while (!window.ShouldCloseWindow())
    {
        glfwPollEvents();
    }

    return 0;
}