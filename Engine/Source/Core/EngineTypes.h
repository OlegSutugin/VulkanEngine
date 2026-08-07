#pragma once

#include <cstdint>
#include "Libraries/Math3D/Math3D.h"
#include "Libraries/Common/Math.h"

namespace VulkanEngine
{

struct MeshHandle
{
    uint32_t id = 0;
    bool IsValid() const { return id != 0; }
};

struct Transform
{
    Transform() {};
    Transform(Math3D::Vec3 inLocation, Math3D::Vec3 inRotation, Math3D::Vec3 inScale)  //
        : location(inLocation), rotation(inRotation), scale(inScale) {};

    static Transform ZeroTransform() { return Transform(); }

    Math3D::Vec3 location = {0.f, 0.f, 0.f};
    Math3D::Vec3 rotation = {0.f, 0.f, 0.f};
    Math3D::Vec3 scale = {1.f, 1.f, 1.f};

    Math3D::Mat4 ToMatrix() const
    {
        Math3D::Mat4 rotationX = Math3D::Mat4::Rotate(Math::DegreesToRadians(rotation.x), Math3D::Vec3{1.f, 0.f, 0.f});
        Math3D::Mat4 rotationY = Math3D::Mat4::Rotate(Math::DegreesToRadians(rotation.y), Math3D::Vec3{0.f, 1.f, 0.f});
        Math3D::Mat4 rotationZ = Math3D::Mat4::Rotate(Math::DegreesToRadians(rotation.z), Math3D::Vec3{0.f, 0.f, 1.f});

        Math3D::Mat4 rotationCombined = rotationZ * rotationY * rotationX;

        Math3D::Mat4 scaleMatrix = Math3D::Mat4::Scale(scale);
        Math3D::Mat4 translationMatrix = Math3D::Mat4::Translate(location);

        return translationMatrix * rotationCombined * scaleMatrix;
    }
};

}  // namespace VulkanEngine