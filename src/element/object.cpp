#include "object.h"
#include <glm/gtc/matrix_transform.hpp>

Object::Object(std::shared_ptr<Model> m, std::shared_ptr<Shader> s, std::shared_ptr<Texture> t)
    : model(m), shader(s), texture(t), position(0.0f), rotation(0.0f), scale(1.0f)
{
}

Object::Object(std::shared_ptr<Model> m, std::shared_ptr<Shader> s, std::shared_ptr<Texture> t, const glm::vec3 &pos, const glm::vec3 &rot, const glm::vec3 &scl)
    : model(m), shader(s), texture(t), position(pos), rotation(rot), scale(scl)
{
}    

glm::mat4 Object::GetWorldMatrix() const
{
    glm::mat4 translation = glm::translate(glm::mat4(1.0f), position);
    glm::mat4 rotX = glm::rotate(glm::mat4(1.0f), glm::radians(rotation.x), glm::vec3(1, 0, 0));
    glm::mat4 rotY = glm::rotate(glm::mat4(1.0f), glm::radians(rotation.y), glm::vec3(0, 1, 0));
    glm::mat4 rotZ = glm::rotate(glm::mat4(1.0f), glm::radians(rotation.z), glm::vec3(0, 0, 1));
    glm::mat4 rotationMat = rotZ * rotY * rotX;
    glm::mat4 scaling = glm::scale(glm::mat4(1.0f), scale);
    return translation * rotationMat * scaling;
}

void Object::Draw(const glm::mat4 &viewMatrix, const glm::mat4 &projectionMatrix)
{
    if (!model || !shader)
        return;

    shader->Use();
    glm::mat4 worldMatrix = GetWorldMatrix();
    shader->SetMat4("model", worldMatrix);
    shader->SetMat4("view", viewMatrix);
    shader->SetMat4("projection", projectionMatrix);

    if (texture)
    {
        texture->Bind(0);
        shader->SetInt("texture1", 0);
    }

    model->Draw();

    if (texture)
        texture->Unbind();
}