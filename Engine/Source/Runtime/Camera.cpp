#include "Camera.h"
#include <glm/gtc/matrix_transform.hpp>

using namespace VulkanEngine;

Camera::Camera()
{
    UpdateVectors();  // for initial
}

void Camera::MoveForward(float amount)
{
    m_position += m_front * amount * m_moveSpeed;
}

void Camera::MoveRight(float amount)
{
    glm::vec3 right = glm::normalize(glm::cross(m_front, m_up));
    m_position += right * amount * m_moveSpeed;
}

void Camera::Rotate(float yawDelta, float pitchDelta)
{
    m_yaw += yawDelta;
    m_pitch += pitchDelta;
    m_pitch = std::clamp(m_pitch, -89.0f, 89.0f);
    UpdateVectors();
}

glm::mat4 Camera::GetView() const
{
    return glm::lookAt(m_position, m_position + m_front, m_up);
}

glm::mat4 Camera::GetProj(float aspectRatio) const
{
    glm::mat4 proj = glm::perspective(glm::radians(m_fov), aspectRatio, 0.1f, 100.0f);
    CLIP_SPACE_Y_FLIP(proj);
    return proj;
}

void Camera::UpdateVectors()
{
    glm::vec3 front;
    front.x = cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
    front.y = sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
    front.z = sin(glm::radians(m_pitch));
    m_front = glm::normalize(front);
}
