#include "camera.h"
#include "global.h"

Camera::Camera(float fov, float nearPlane, float farPlane)
    : m_position(0.0f, 0.0f, 3.0f),
      m_target(0.0f, 0.0f, 0.0f),
      m_up(0.0f, 1.0f, 0.0f),
      m_fov(fov),
      m_near(nearPlane),
      m_far(farPlane)
{
}

Camera::Camera(float fov, float nearPlane, float farPlane, const glm::vec3& position, const glm::vec3& target, const glm::vec3& up)
    : m_position(position),
      m_target(target),
      m_up(up),
      m_fov(fov),
      m_near(nearPlane),
      m_far(farPlane)
{
}

void Camera::SetPosition(const glm::vec3& position)
{
    m_position = position;
}

void Camera::SetTarget(const glm::vec3& target)
{
    m_target = target;
}

void Camera::SetUp(const glm::vec3& up)
{
    m_up = up;
}

glm::mat4 Camera::GetViewMatrix() const
{
    return glm::lookAt(m_position, m_target, m_up);
}

glm::mat4 Camera::GetPerspectiveMatrix() const
{
    return glm::perspective(glm::radians(m_fov), Global::ScreenWidth / Global::ScreenHeight * 1.0f,  m_near, m_far);
}

glm::mat4 Camera::GetOrthosMatrix() const
{
    return glm::ortho(-Global::ScreenWidth, Global::ScreenWidth,
                      -Global::ScreenHeight, Global::ScreenHeight,
                      m_near, m_far);
}
