#ifndef GRAPHICS_HANDLER_HPP
#define GRAPHICS_HANDLER_HPP
#pragma once

#include <Graphics/GameObject.hpp>
#include <Graphics/Renderer.ipp>
#include <Graphics/Vulkan/VkInstanceImpl.hpp>
#include <Settings.hpp>

// STD Lib
#include <memory>
#include <vector>

namespace Graphic
{

class Application
{

public:

    Application();
    ~Application();

    Application(const Application&) = delete;
    Application &operator=(const Application&) = delete;

    void RenderLoop();

private:

    void LoadGameObjects();

    WindowHandler window_{DISPLAY_WIDTH,DISPLAY_HEIGHT};
    VkDeviceInstance deviceInst_{&window_};
    Renderer renderer_{&window_, &deviceInst_};

    std::vector<GameObject> gameObjects_;
};

} // namespace Graphic


#endif