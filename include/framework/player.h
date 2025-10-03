#pragma once
#include "entity.h"
#include "camera.h"
#include "player_state_machine.h"

class Player : public Entity
{
public:
    Player(const std::string &i, const glm::vec3 &pos, const glm::vec3 &rot, const glm::vec3 &scl,
           std::shared_ptr<Camera> c, std::shared_ptr<Model> m, std::shared_ptr<Shader> s, std::shared_ptr<Texture> t);

    std::shared_ptr<Camera> GetCamera() { return camera; }
    void AttachCamera(std::shared_ptr<Camera> c) { camera = c; }
    void DetachCamera() { camera = nullptr; }

    void Update(float deltaTime) override;

private:
    std::shared_ptr<Camera> camera;
    std::shared_ptr<PlayerStateMachine> stateMachine;
};