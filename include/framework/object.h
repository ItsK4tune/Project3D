#pragma once
#include <glm/glm.hpp>
#include <memory>
#include "model.h"
#include "shader.h"
#include "texture.h"

class Object
{
public:
    Object(std::shared_ptr<Model> m, std::shared_ptr<Shader> s, std::shared_ptr<Texture> t);
    Object(std::shared_ptr<Model> m, std::shared_ptr<Shader> s, std::shared_ptr<Texture> t, const glm::vec3 &pos, const glm::vec3 &rot, const glm::vec3 &scl);

    void SetPosition(const glm::vec3 &pos) { position = pos; }
    void SetRotation(const glm::vec3 &rot) { rotation = rot; }
    void SetScale(const glm::vec3 &scl) { scale = scl; }
    void SetModel(std::shared_ptr<Model> m) { model = m; }
    void SetTexture(std::shared_ptr<Texture> t) { texture = t; }
    void SetShader(std::shared_ptr<Shader> s) { shader = s; }

    glm::vec3 GetPosition() const { return position; }
    glm::vec3 GetRotation() const { return rotation; }
    glm::vec3 GetScale() const { return scale; }
    std::shared_ptr<Model> GetModel() const { return model; }
    std::shared_ptr<Texture> GetTexture() const { return texture; }
    std::shared_ptr<Shader> GetShader() const { return shader; }

    void Draw(const glm::mat4 &viewMatrix, const glm::mat4 &projectionMatrix);

private:
    std::shared_ptr<Model> model;
    std::shared_ptr<Texture> texture;
    std::shared_ptr<Shader> shader;

    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 scale;

    glm::mat4 GetWorldMatrix() const;
};
