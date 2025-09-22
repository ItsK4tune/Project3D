#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera
{
public:
    Camera(float fov, float nearPlane, float farPlane);
    Camera(float fov, float nearPlane, float farPlane, const glm::vec3 &position, const glm::vec3 &target, const glm::vec3 &up);

    void SetPosition(const glm::vec3 &position);
    void SetTarget(const glm::vec3 &target);
    void SetUp(const glm::vec3 &up);

    glm::vec3 GetPosition() const { return m_position; }
    glm::vec3 GetTarget() const { return m_target; }
    glm::vec3 GetUp() const { return m_up; }
    float &GetYaw() { return yaw; }
    float &GetPitch() { return pitch; }
    
    glm::mat4 GetViewMatrix() const;
    glm::mat4 GetPerspectiveMatrix() const;
    glm::mat4 GetOrthosMatrix() const;

    void Move(const glm::vec3 &delta);

private:
    glm::vec3 m_position;
    glm::vec3 m_target;
    glm::vec3 m_up;

    float m_fov;
    float m_near;
    float m_far;

    float yaw = -90.0f;
    float pitch = 0.0f;
};