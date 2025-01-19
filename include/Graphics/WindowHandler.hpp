#ifndef GRAPHICS_WINDOWHANDLER_HPP
#define GRAPHICS_WINDOWHANDLER_HPP
#pragma once

#include <Global.hpp>

struct GLFWwindow;
struct GLFWmonitor;
struct VkExtent2D;

namespace Graphic
{

class WindowHandler
{
public:

    WindowHandler(int32_t width, int32_t height);
    ~WindowHandler();

    // Move Constructor
    WindowHandler(const WindowHandler&) = delete;
    WindowHandler &operator=(const WindowHandler&) = delete;


    bool ShouldCloseWindow() const;

    GLFWwindow* GetWindowHandlerPointer() const;

    glm::ivec2 GetFrameBufferSize() const;

    VkExtent2D GetWindowExtent();

private:

    void InitWindow();

//----------------------------------------------------------------------------//

    int32_t width_;
    int32_t height_;

    GLFWwindow* window_ = nullptr;
    GLFWmonitor** monitorArr_ = nullptr;
    int32_t monitorCount_;
};

} // namespace Graphic

#endif