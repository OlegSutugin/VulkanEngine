#pragma once

#include <cmath>

namespace Math3D
{
struct Vec4
{
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;
    float w = 0.0f;

    Vec4() = default;
    Vec4(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}

    // arifm
    Vec4 operator+(const Vec4& other) const { return Vec4{x + other.x, y + other.y, z + other.z, w + other.w}; }

    Vec4 operator-(const Vec4& other) const { return Vec4{x - other.x, y - other.y, z - other.z, w - other.w}; }

    Vec4 operator*(float scalar) const { return Vec4{x * scalar, y * scalar, z * scalar, w * scalar}; }

    Vec4& operator+=(const Vec4& other)
    {
        x += other.x;
        y += other.y;
        z += other.z;
        w += other.w;
        return *this;
    }

    // geom
    float Dot(const Vec4& other) const { return x * other.x + y * other.y + z * other.z + w * other.w; }

    float Length() const { return std::sqrt(x * x + y * y + z * z + w * w); }

    Vec4 Normalized() const { return (*this) * (1.0f / Length()); }
};
}  // namespace Math3D