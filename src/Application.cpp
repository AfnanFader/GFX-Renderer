#include <Application.hpp>
#include <Input/InputHandler.ipp>
#include <Graphics/Vulkan/VkUtil.ipp>

#include <Graphics/Pipeline/SimpleRenderPipeline.ipp>

// External Lib
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

// STD Lib
#include <memory>

namespace Graphic
{

class GravityPhysicsSystem {
public:
GravityPhysicsSystem(float strength) : strengthGravity{strength} {}

const float strengthGravity;

// dt stands for delta time, and specifies the amount of time to advance the simulation
// substeps is how many intervals to divide the forward time step in. More substeps result in a
// more stable simulation, but takes longer to compute
void update(std::vector<GameObject>& objs, float dt, unsigned int substeps = 1) {
    const float stepDelta = dt / substeps;
    for (int i = 0; i < substeps; i++) {
    stepSimulation(objs, stepDelta);
    }
}

glm::vec2 computeForce(GameObject& fromObj, GameObject& toObj) const {
    auto offset = fromObj.transform2d.translation - toObj.transform2d.translation;
    float distanceSquared = glm::dot(offset, offset);

    // clown town - just going to return 0 if objects are too close together...
    if (glm::abs(distanceSquared) < 1e-10f) {
    return {.0f, .0f};
    }

    float force =
        strengthGravity * toObj.rigidBody2d.mass * fromObj.rigidBody2d.mass / distanceSquared;
    return force * offset / glm::sqrt(distanceSquared);
}

private:
void stepSimulation(std::vector<GameObject>& physicsObjs, float dt) {
    // Loops through all pairs of objects and applies attractive force between them
    for (auto iterA = physicsObjs.begin(); iterA != physicsObjs.end(); ++iterA) {
    auto& objA = *iterA;
    for (auto iterB = iterA; iterB != physicsObjs.end(); ++iterB) {
        if (iterA == iterB) continue;
        auto& objB = *iterB;

        auto force = computeForce(objA, objB);
        objA.rigidBody2d.velocity += dt * -force / objA.rigidBody2d.mass;
        objB.rigidBody2d.velocity += dt * force / objB.rigidBody2d.mass;
    }
    }

    // update each objects position based on its final velocity
    for (auto& obj : physicsObjs) {
    obj.transform2d.translation += dt * obj.rigidBody2d.velocity;
    }
}

};
    
class Vec2FieldSystem {
public:
void update(
    const GravityPhysicsSystem& physicsSystem,
    std::vector<GameObject>& physicsObjs,
    std::vector<GameObject>& vectorField) {
    // For each field line we caluclate the net graviation force for that point in space
    for (auto& vf : vectorField) {
    glm::vec2 direction{};
    for (auto& obj : physicsObjs) {
        direction += physicsSystem.computeForce(obj, vf);
    }

    // This scales the length of the field line based on the log of the length
    // values were chosen just through trial and error based on what i liked the look
    // of and then the field line is rotated to point in the direction of the field
    vf.transform2d.scale.x =
        0.005f + 0.045f * glm::clamp(glm::log(glm::length(direction) + 1) / 3.f, 0.f, 1.f);
    vf.transform2d.rotation = atan2(direction.y, direction.x);
    }
}
};

std::vector<Vertex> createSquareModel(glm::vec2 offset) {
    std::vector<Vertex> vertices = {
        {{-0.5f, -0.5f}},
        {{0.5f, 0.5f}},
        {{-0.5f, 0.5f}},
        {{-0.5f, -0.5f}},
        {{0.5f, -0.5f}},
        {{0.5f, 0.5f}},  //
    };
    for (auto& v : vertices) {
    v.position += offset;
    }
    return vertices;
}

std::unique_ptr<VkModel> createCircleModel(VkDeviceInstance* device, uint32_t numSides)
{
    std::vector<Vertex> uniqueVertices{};
    for (int i = 0; i < numSides; i++)
    {
        float angle = i * glm::two_pi<float>() / numSides;
        uniqueVertices.push_back({{glm::cos(angle), glm::sin(angle)}});
    }
    uniqueVertices.push_back({});  // adds center vertex at 0, 0

    std::vector<Vertex> vertices{};
    for (int i = 0; i < numSides; i++)
    {
        vertices.push_back(uniqueVertices[i]);
        vertices.push_back(uniqueVertices[(i + 1) % numSides]);
        vertices.push_back(uniqueVertices[numSides]);
    }
    return std::make_unique<VkModel>(device, vertices);
}


Application::Application() 
{
    LoadGameObjects();
}

Application::~Application()
{
}

void Application::RenderLoop()
{
    // offset model by .5 so rotation occurs at edge rather than center of square
    auto squareVert = createSquareModel({.5f, .0f});
    std::shared_ptr<VkModel> squareModel = std::make_shared<VkModel>(&deviceInst_, squareVert);

    std::shared_ptr<VkModel> circleModel = createCircleModel(&deviceInst_, 64);
    
    // create physics objects
    std::vector<GameObject> physicsObjects{};
    auto red = GameObject::CreateGameObject();
    red.transform2d.scale = glm::vec2{.05f};
    red.transform2d.translation = {.5f, .5f};
    red.color = {1.f, 0.f, 0.f};
    red.rigidBody2d.velocity = {-.1f, 0.0f};
    red.model = circleModel;
    physicsObjects.push_back(std::move(red));
    auto blue = GameObject::CreateGameObject();
    blue.transform2d.scale = glm::vec2{.05f};
    blue.transform2d.translation = {-.45f, -.25f};
    blue.color = {0.f, 0.f, 1.f};
    blue.rigidBody2d.velocity = {.1f, 0.0f};
    blue.model = circleModel;
    physicsObjects.push_back(std::move(blue));
    
    // create vector field
    std::vector<GameObject> vectorField{};
    int gridCount = 40;
    for (int i = 0; i < gridCount; i++) {
        for (int j = 0; j < gridCount; j++) {
        auto vf = GameObject::CreateGameObject();
        vf.transform2d.scale = glm::vec2(0.005f);
        vf.transform2d.translation = {
            -1.0f + (i + 0.5f) * 2.0f / gridCount,
            -1.0f + (j + 0.5f) * 2.0f / gridCount};
        vf.color = glm::vec3(1.0f);
        vf.model = squareModel;
        vectorField.push_back(std::move(vf));
        }
    }
    
    GravityPhysicsSystem gravitySystem{0.81f};
    Vec2FieldSystem vecFieldSystem{};
    SimpleRenderPipeline simpleRender(&deviceInst_, renderer_.GetRenderPass());

    glfwSetKeyCallback(window_.GetWindowHandlerPointer(), Input::KeyCallBack);

    while (!window_.ShouldCloseWindow())
    {
        glfwPollEvents();

        if (auto commandBuffer = renderer_.BeginFrame())
        {
            // More future pipelines to be added shadow pass etc

            // Update physics
            gravitySystem.update(physicsObjects, 1.0f / 60, 5);
            vecFieldSystem.update(gravitySystem, physicsObjects, vectorField);

            renderer_.BeginSwapChainRenderPass(commandBuffer);

            // simpleRender.RenderGameObjects(commandBuffer, gameObjects_);
            simpleRender.RenderGameObjects(commandBuffer, physicsObjects);
            simpleRender.RenderGameObjects(commandBuffer, vectorField);

            renderer_.EndSwapChainRenderPass(commandBuffer);
            renderer_.EndFrame();
        }
    }

    vkDeviceWaitIdle(deviceInst_.GetLogicalDevice());

}

void Application::LoadGameObjects()
{
    std::vector<Vertex> vertices{
        {{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
        {{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
        {{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}
    };
    auto vkModel_ = std::make_shared<VkModel>(&deviceInst_, vertices);

    auto triangle = GameObject::CreateGameObject();
    triangle.model = vkModel_;
    triangle.color = {.1f, .8f, .1f};
    triangle.transform2d.translation.x = .2f;
    triangle.transform2d.scale = {2.f, .5f};
    triangle.transform2d.rotation = .25f * glm::two_pi<float>();

    gameObjects_.push_back(std::move(triangle));
}

} // namespace Graphic
