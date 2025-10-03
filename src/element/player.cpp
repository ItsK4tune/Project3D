#include "player.h"
#include "player_state_machine.h"
#include "player_state.h"

Player::Player(const std::string &i, const glm::vec3 &pos, const glm::vec3 &rot, const glm::vec3 &scl, std::shared_ptr<Camera> c,
               std::shared_ptr<Model> m, std::shared_ptr<Shader> s, std::shared_ptr<Texture> t)
    : Entity(i, pos, rot, scl, m, s, t), stateMachine(std::make_unique<PlayerStateMachine>()), camera(c)
{
    stateMachine->Push(std::make_unique<PSIdle>());
}

void Player::Update(float deltaTime)
{
    Entity::Update(deltaTime);
    stateMachine->Update(deltaTime, this);
}