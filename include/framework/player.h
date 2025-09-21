// #pragma once
// #include "entity.h"
// #include "camera.h"

// class Player : public Entity {
// public:
//     Player(const std::string &i, std::shared_ptr<Model> m, std::shared_ptr<Shader> s, std::shared_ptr<Texture> t,
//            const glm::vec3 &pos, const glm::vec3 &rot, const glm::vec3 &scl, std::shared_ptr<Camera> c);

//     void AttachCamera(std::shared_ptr<Camera> c) { camera = c; }
//     void DetachCamera() { camera = nullptr; }

// private:
//     std::shared_ptr<Camera> camera;
// };