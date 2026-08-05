#pragma once

namespace Math
{
constexpr float PI = 3.14159265358979323846f;

static float DegreesToRadians(float degrees)
{
    return degrees * PI / 180.f;
}

static float RadiansToDegrees(float radians)
{
    return radians * 180.f / PI;
}
}  // namespace Math