#pragma once

struct GLFWwindow;
struct GLFWmonitor;

namespace Window
{

class WindowHandler
{
    public:

    WindowHandler();
    ~WindowHandler();

    bool ShouldCloseWindow() const;
    GLFWwindow* GetWindowHandlerPointer() const;
    void MoveMonitorToCenter();

    private:
    GLFWwindow* windowPtr_;
    GLFWmonitor** monitoArrPtr_;
};

} // namespace Window
