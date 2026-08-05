#pragma once

#include "Vec3.h"
#include "Vec4.h"
#include <cmath>

/* Column-major (Vulkan/GLSL)
 *   | m00 m01 m02 m03 |
 *   | m10 m11 m12 m13 |
 *   | m20 m21 m22 m23 |
 *   | m30 m31 m32 m33 |
 *           into
 *  [m00, m10, m20, m30,  m01, m11, m21, m31,  m02, m12, m22, m32,  m03, m13, m23, m33]
 */

namespace Math3D
{
struct Mat4
{
    // m[col][row] — колонко-мажорное хранение
    float m[4][4] = {};

    Mat4() = default;

    static Mat4 Identity()
    {
        Mat4 result;
        result.m[0][0] = 1.0f;
        result.m[1][1] = 1.0f;
        result.m[2][2] = 1.0f;
        result.m[3][3] = 1.0f;
        return result;
    }

    Mat4 operator*(const Mat4& other) const
    {
        Mat4 result;
        for (int col = 0; col < 4; ++col)
        {
            for (int row = 0; row < 4; ++row)
            {
                float sum = 0.0f;
                for (int k = 0; k < 4; ++k)
                {
                    sum += m[k][row] * other.m[col][k];
                }
                result.m[col][row] = sum;
            }
        }
        return result;
    }

    Vec4 operator*(const Vec4& v) const
    {
        Vec4 result;
        result.x = m[0][0] * v.x + m[1][0] * v.y + m[2][0] * v.z + m[3][0] * v.w;
        result.y = m[0][1] * v.x + m[1][1] * v.y + m[2][1] * v.z + m[3][1] * v.w;
        result.z = m[0][2] * v.x + m[1][2] * v.y + m[2][2] * v.z + m[3][2] * v.w;
        result.w = m[0][3] * v.x + m[1][3] * v.y + m[2][3] * v.z + m[3][3] * v.w;
        return result;
    }

    static Mat4 Diagonal(float scalar)
    {
        Mat4 result;
        result.m[0][0] = scalar;
        result.m[1][1] = scalar;
        result.m[2][2] = scalar;
        result.m[3][3] = scalar;
        return result;
    }

    static Mat4 Translate(const Vec3& t)
    {
        Mat4 result = Identity();
        result.m[3][0] = t.x;
        result.m[3][1] = t.y;
        result.m[3][2] = t.z;
        return result;
    }

    static Mat4 Rotate(const Vec3& s)
    {
        Mat4 result = Identity();
        return result;
    }

    static Mat4 Scale(const Vec3& s)
    {
        Mat4 result = Identity();
        result.m[0][0] = s.x;
        result.m[1][1] = s.y;
        result.m[2][2] = s.z;
        return result;
    }

    static Mat4 ViewMatrix(const Vec3& eye, const Vec3& target, const Vec3& worldUp)
    {
        Mat4 result = Identity();

        Vec3 forwardNorm = (target - eye).Normalized();
        Vec3 rightNorm = forwardNorm.Cross(worldUp).Normalized();
        Vec3 upNorm = rightNorm.Cross(forwardNorm);

        // right
        result.m[0][0] = rightNorm.x;
        result.m[1][0] = rightNorm.y;
        result.m[2][0] = rightNorm.z;

        // up
        result.m[0][1] = upNorm.x;
        result.m[1][1] = upNorm.y;
        result.m[2][1] = upNorm.z;

        // -forward
        result.m[0][2] = -forwardNorm.x;
        result.m[1][2] = -forwardNorm.y;
        result.m[2][2] = -forwardNorm.z;

        // translation
        result.m[3][0] = -rightNorm.Dot(eye);
        result.m[3][1] = -upNorm.Dot(eye);
        result.m[3][2] = forwardNorm.Dot(eye);
        result.m[3][3] = 1.0f;

        return result;
    }

    static Mat4 PerspectiveProjectionMatrix(float fovYRadians, float aspectRatio, float nearPlane, float farPlane)
    {
        Mat4 result;

        float tanHalfFov = std::tan(fovYRadians / 2.0f);
        result.m[0][0] = 1.f / (aspectRatio * tanHalfFov);
        result.m[1][1] = 1.f / tanHalfFov;
        result.m[2][2] = farPlane / (nearPlane - farPlane);
        result.m[2][3] = -1.f;
        result.m[3][2] = (nearPlane * farPlane) / (nearPlane - farPlane);

        return result;
    }
};
}  // namespace Math3D
