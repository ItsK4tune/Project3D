#pragma once
#include <glm/glm.hpp>
#include <memory>
#include "model.h"
#include "shader.h"
#include "texture.h"

class LightSource
{
public:
    LightSource(std::shared_ptr<Model> m,
                std::shared_ptr<Shader> s,
                std::shared_ptr<Texture> t);

    LightSource(std::shared_ptr<Model> m,
                std::shared_ptr<Shader> s,
                std::shared_ptr<Texture> t,
                const glm::vec3 &pos,
                const glm::vec3 &rot,
                const glm::vec3 &scl);

    void SetPosition(const glm::vec3 &pos) { position = pos; }
    void SetRotation(const glm::vec3 &rot) { rotation = rot; }
    void SetScale(const glm::vec3 &scl) { scale = scl; }
    void SetModel(std::shared_ptr<Model> m) { model = m; }
    void SetTexture(std::shared_ptr<Texture> t) { texture = t; }
    void SetShader(std::shared_ptr<Shader> s) { shader = s; }

    void SetAmbient(const glm::vec3 &a) { ambient = a; }
    void SetDiffuse(const glm::vec3 &d) { diffuse = d; }
    void SetSpecular(const glm::vec3 &s) { specular = s; }
    void SetIntensity(float i) { intensity = i; }

    void SetAttenuation(float c, float l, float q)
    {
        constant = c;
        linear = l;
        quadratic = q;
    }

    glm::vec3 GetPosition() const { return position; }
    glm::vec3 GetRotation() const { return rotation; }
    glm::vec3 GetScale() const { return scale; }
    std::shared_ptr<Model> GetModel() const { return model; }
    std::shared_ptr<Texture> GetTexture() const { return texture; }
    std::shared_ptr<Shader> GetShader() const { return shader; }

    glm::vec3 GetAmbient() const { return ambient; }
    glm::vec3 GetDiffuse() const { return diffuse; }
    glm::vec3 GetSpecular() const { return specular; }
    float GetIntensity() const { return intensity; }

    float GetConstant() const { return constant; }
    float GetLinear() const { return linear; }
    float GetQuadratic() const { return quadratic; }

    void Draw(const glm::mat4 &viewMatrix, const glm::mat4 &projectionMatrix);

private:
    std::shared_ptr<Model> model;
    std::shared_ptr<Texture> texture;
    std::shared_ptr<Shader> shader;

    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 scale;

    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    float intensity;

    float constant;
    float linear;
    float quadratic;

    glm::mat4 GetWorldMatrix() const;
};