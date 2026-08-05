#include "Camera.h"
#include "Libraries/Common/Math.h"

#include <algorithm>

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
    Math3D::Vec3 right = m_front.Cross(m_up).Normalized();
    m_position += right * amount * m_moveSpeed;
}

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

void Camera::Rotate(float yawDelta, float pitchDelta)
{
    m_yaw += yawDelta;
    m_pitch += pitchDelta;
    m_pitch = std::clamp(m_pitch, -89.0f, 89.0f);
    UpdateVectors();
}

Math3D::Mat4 Camera::GetView() const
{
    return Math3D::Mat4::ViewMatrix(m_position, m_position + m_front, m_up);
}

Math3D::Mat4 Camera::GetProj(float aspectRatio) const
{
    Math3D::Mat4 proj = Math3D::Mat4::PerspectiveProjectionMatrix(Math::DegreesToRadians(m_fov), aspectRatio, 0.1f, 100.0f);
    CLIP_SPACE_Y_FLIP(proj);
    return proj;
}

void Camera::UpdateVectors()
{
    Math3D::Vec3 front;
    front.x = std::cos(Math::DegreesToRadians(m_yaw)) * std::cos(Math::DegreesToRadians(m_pitch));
    front.y = std::sin(Math::DegreesToRadians(m_yaw)) * std::cos(Math::DegreesToRadians(m_pitch));
    front.z = std::sin(Math::DegreesToRadians(m_pitch));
    m_front = front.Normalized();
}
