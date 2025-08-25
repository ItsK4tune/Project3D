#include "light_source.h"

LightSource::LightSource(std::shared_ptr<Model> m,
                         std::shared_ptr<Shader> s,
                         std::shared_ptr<Texture> t)
    : model(m), shader(s), texture(t),
      position(0.0f), rotation(0.0f), scale(1.0f),
      ambient(1.0f), diffuse(1.0f), specular(1.0f),
      intensity(1.0f),
      constant(1.0f), linear(0.09f), quadratic(0.032f)
{
}

LightSource::LightSource(std::shared_ptr<Model> m,
                         std::shared_ptr<Shader> s,
                         std::shared_ptr<Texture> t,
                         const glm::vec3 &pos,
                         const glm::vec3 &rot,
                         const glm::vec3 &scl)
    : model(m), shader(s), texture(t),
      position(pos), rotation(rot), scale(scl),
      ambient(1.0f), diffuse(1.0f), specular(1.0f),
      intensity(1.0f),
      constant(1.0f), linear(0.09f), quadratic(0.032f)
{
}

void LightSource::Draw(const glm::mat4 &viewMatrix, const glm::mat4 &projectionMatrix)
{
    if (!model || !shader)
        return;

    shader->Use();

    if (texture)
        texture->Bind();

    model->Draw();

    if (texture)
        texture->Unbind();
}