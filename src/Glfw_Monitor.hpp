#pragma once

struct GLFWmonitor;
struct GLFWwindow;

namespace window
{

GLFWmonitor** GetMonitor();

void MoveWindowToMonitor(GLFWwindow* const windowPtr, GLFWmonitor** const monitorArrPtr);

} // namespace window
