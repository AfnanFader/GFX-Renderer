#pragma once

struct GLFWmonitor;
struct GLFWwindow;

namespace Window
{

GLFWmonitor** GetMonitor();
void MoveWindowToMonitor(GLFWwindow* const windowPtr, GLFWmonitor** const monitorArrPtr);

} // namespace Window
