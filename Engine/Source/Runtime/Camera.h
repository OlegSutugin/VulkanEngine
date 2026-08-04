#pragma once

#include "Core/PlatformDefines.h"

#include <glm/glm.hpp>

namespace VulkanEngine
{
class Camera
{
public:
    Camera();

    void MoveForward(float amount);
    void MoveRight(float amount);

    void Rotate(float yawDelta, float pitchDelta);

    glm::mat4 GetView() const;
    glm::mat4 GetProj(float aspectRatio) const;

private:
    void UpdateVectors();

    glm::vec3 m_position{2.0f, 2.0f, 2.0f};
    glm::vec3 m_front{0.0f, 0.0f, -1.0f};
    glm::vec3 m_up{0.0f, 0.0f, 1.0f};

    float m_yaw = -135.0f;
    float m_pitch = -30.0f;

    float m_fov = 45.0f;
    float m_moveSpeed = 3.0f;
};
}  // namespace VulkanEngine
