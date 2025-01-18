#ifndef WINDOW_HANDLER_HPP
#define WINDOW_HANDLER_HPP
#pragma once

#include <Global.hpp>

struct GLFWwindow;
struct GLFWmonitor;

namespace Graphic
{

class WindowHandler
{
public:

    WindowHandler();
    ~WindowHandler();

    // Move Constructor
    WindowHandler(const WindowHandler&) = delete;
    WindowHandler &operator=(const WindowHandler&) = delete;


    bool ShouldCloseWindow() const;

    GLFWwindow* GetWindowHandlerPointer() const;

    glm::ivec2 GetFrameBufferSize() const;

private:

    void InitWindow();

//----------------------------------------------------------------------------//

    GLFWwindow* window_ = nullptr;
    GLFWmonitor** monitorArr_ = nullptr;
    int32_t monitorCount_;
};

} // namespace Graphic

#endif