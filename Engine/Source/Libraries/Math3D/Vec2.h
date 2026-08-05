#pragma once

#include <cmath>

namespace Math3D
{
struct Vec2
{
    float x = 0.0f;
    float y = 0.0f;

    Vec2() = default;
    Vec2(float x, float y) : x(x), y(y) {}

    // arifm
    Vec2 operator+(const Vec2& other) const { return Vec2{x + other.x, y + other.y}; }

    Vec2 operator-(const Vec2& other) const { return Vec2{x - other.x, y - other.y}; }

    Vec2 operator*(float scalar) const { return Vec2{x * scalar, y * scalar}; }

    Vec2& operator+=(const Vec2& other)
    {
        x += other.x;
        y += other.y;
        return *this;
    }

    // geom
    float Dot(const Vec2& other) const { return x * other.x + y * other.y; }

    float Length() const { return std::sqrt(x * x + y * y); }

    Vec2 Normalized() const { return (*this) * (1.0f / Length()); }
};
}  // namespace Math3D