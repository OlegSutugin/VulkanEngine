#pragma once

#include <cmath>

namespace Math3D
{
struct Vec3
{
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;

    Vec3() = default;
    Vec3(float x, float y, float z) : x(x), y(y), z(z) {}

    // arifm
    Vec3 operator+(const Vec3& other) const { return Vec3{x + other.x, y + other.y, z + other.z}; }

    Vec3 operator-(const Vec3& other) const { return Vec3{x - other.x, y - other.y, z - other.z}; }

    Vec3 operator*(float scalar) const { return Vec3{x * scalar, y * scalar, z * scalar}; }

    Vec3& operator+=(const Vec3& other)
    {
        x += other.x;
        y += other.y;
        z += other.z;
        return *this;
    }

    // geom
    float Dot(const Vec3& other) const { return x * other.x + y * other.y + z * other.z; }

    Vec3 Cross(const Vec3& other) const { return Vec3(y * other.z - z * other.y, z * other.x - x * other.z, x * other.y - y * other.x); }

    float Length() const { return std::sqrt(x * x + y * y + z * z); }

    Vec3 Normalized() const { return (*this) * (1.0f / Length()); }
};
}  // namespace Math3D
