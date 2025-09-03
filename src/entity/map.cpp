#include "map.h"

Map::Map(std::shared_ptr<Model> m, std::shared_ptr<Shader> s, std::shared_ptr<Texture> t,
         const glm::vec3 &pos, const glm::vec3 &rot, const glm::vec3 &scl)
    : Entity(m, s, t, pos, rot, scl)
{
}

void Map::Update(float deltaTime, const std::vector<std::shared_ptr<Entity>> &others)
{
    // Bản đồ tĩnh, không cần cập nhật gì
}