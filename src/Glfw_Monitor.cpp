#include <Glfw_Monitor.hpp>
#include <GLFW/glfw3.h>
#include <Precomp.hpp>

namespace window
{

GLFWmonitor** GetMonitor()
{
    int32_t monitor_count;
    GLFWmonitor** monitorArrPtr = glfwGetMonitors(&monitor_count);
    return monitor_count == 0 ? nullptr : monitorArrPtr;
}

void MoveWindowToMonitor(GLFWwindow* const windowPtr, GLFWmonitor** const monitorArrPtr)
{
    ivec2 window_size;
    ivec2 monitor_coordinates; // X & Y coordinate of the virtual screen.
    ivec2 monitor_size;

    glfwGetWindowSize(windowPtr, &window_size.x, &window_size.y);
    glfwGetMonitorPos(monitorArrPtr[0] , &monitor_coordinates.x, &monitor_coordinates.y);
    glfwGetMonitorWorkarea(monitorArrPtr[0], nullptr, nullptr, &monitor_size.x, &monitor_size.y);
    
    // Caculate the center of monitor.
    ivec2 center_coordinates = monitor_coordinates + (monitor_size/2) - (window_size/2);

    glfwSetWindowPos(windowPtr, center_coordinates.x, center_coordinates.y);
}

} // namespace window
