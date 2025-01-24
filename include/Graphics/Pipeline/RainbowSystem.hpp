#ifndef GRAPHICS_RAINBOWSYSTEM_HPP
#define GRAPHICS_RAINBOWSYSTEM_HPP
#pragma once
 
#include <Graphics/GameObject.ipp>
 
// std
#include <memory>
#include <random>
#include <vector>
 
namespace Graphic {

class RainbowSystem {

    public:
    RainbowSystem(float flickerRate) : mFlickerRate{flickerRate} {
        // initialize colors
        mColors = {
            {.8f, .1f, .1f},
            {.1f, .8f, .1f},
            {.1f, .1f, .8f},
            {.8f, .8f, .1f},
            {.8f, .1f, .8f},
            {.1f, .8f, .8f},
        };
        mElapsedTime = mFlickerRate;
    }
    
    // randomly select a color for each game object every mFlickerRate seconds
    void update(float dt, std::vector<GameObject> &gameObjects) {
        mElapsedTime -= dt;
        if (mElapsedTime < 0.f) {
        mElapsedTime += mFlickerRate;
        std::uniform_int_distribution<int> randInt{0, static_cast<int>(mColors.size()) - 1};
        for (auto &obj : gameObjects) {
            int randValue = randInt(mRng);
            obj.color = mColors[randValue];
        }
        }
    }
    
    private:
    std::random_device rd;
    std::mt19937 mRng{rd()};
    
    std::vector<glm::vec3> mColors;
    float mFlickerRate;
    float mElapsedTime;
};

}  // namespace Graphic

#endif