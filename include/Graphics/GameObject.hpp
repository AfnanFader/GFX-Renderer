#ifndef GRAPHICS_GAMEOBJECT_HPP
#define GRAPHICS_GAMEOBJECT_HPP
#pragma once

#include <Graphics/VkModel.hpp>

// std
#include <memory>

namespace Graphic {

struct RigidBody2dCompoenent
{
    glm::vec2 velocity;
    float mass{.1f};
};

struct Transform2dComponent {
    glm::vec2 translation{};  // (position offset)
    glm::vec2 scale{1.f, 1.f};
    float rotation;

    glm::mat2 mat2() {
        const float s = glm::sin(rotation);
        const float c = glm::cos(rotation);
        glm::mat2 rotMatrix{{c, s}, {-s, c}};

        glm::mat2 scaleMat{{scale.x, .0f}, {.0f, scale.y}};
        // return rotMatrix * scaleMat;
        return scaleMat * rotMatrix;
    }
};

class GameObject {
    public:
    using id_t = unsigned int;

    static GameObject CreateGameObject() {
        static id_t currentId = 0;
        return GameObject{currentId++};
    }

    GameObject(const GameObject &) = delete;
    GameObject &operator=(const GameObject &) = delete;
    GameObject(GameObject &&) = default;
    GameObject &operator=(GameObject &&) = default;

    id_t GetId() { return id; }

    std::shared_ptr<VkModel> model{};
    glm::vec3 color{};
    Transform2dComponent transform2d{};
    RigidBody2dCompoenent rigidBody2d{};

    private:
    GameObject(id_t objId) : id{objId} {}

    id_t id;
};

}  // namespace Graphic

#endif
