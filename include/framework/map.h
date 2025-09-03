#pragma once
#include "entity.h"

class Map : public Entity
{
public:
    Map(std::shared_ptr<Model> m, std::shared_ptr<Shader> s, std::shared_ptr<Texture> t,
        const glm::vec3 &pos, const glm::vec3 &rot, const glm::vec3 &scl);

    void Update(float deltaTime, const std::vector<std::shared_ptr<Entity>> &others) override;
};