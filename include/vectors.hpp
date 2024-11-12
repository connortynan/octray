#pragma once

#include <cmath>
#include <stdexcept>

struct Vec3f
{
    float x, y, z;

    Vec3f() : x(0.f), y(0.f), z(0.f) {}
    Vec3f(const float _x, const float _y, const float _z) : x(_x), y(_y), z(_z) {}

    // Non-const version for modification
    float &operator[](size_t i)
    {
        switch (i)
        {
        case 0:
            return x;
        case 1:
            return y;
        case 2:
            return z;
        default:
            throw std::runtime_error("Index out of bound for Vec3f");
        }
    }

    // Const version for read-only access
    const float &operator[](size_t i) const
    {
        switch (i)
        {
        case 0:
            return x;
        case 1:
            return y;
        case 2:
            return z;
        default:
            throw std::runtime_error("Index out of bound for Vec3f");
        }
    }

    // Vector addition
    Vec3f operator+(const Vec3f &other) const
    {
        return Vec3f(x + other.x, y + other.y, z + other.z);
    }

    void operator+=(const Vec3f &other)
    {
        x += other.x;
        y += other.y;
        z += other.z;
    }

    // Vector subtraction
    Vec3f operator-(const Vec3f &other) const
    {
        return Vec3f(x - other.x, y - other.y, z - other.z);
    }

    void operator-=(const Vec3f &other)
    {
        x -= other.x;
        y -= other.y;
        z -= other.z;
    }

    // Scalar multiplication
    Vec3f operator*(const float scalar) const
    {
        return Vec3f(x * scalar, y * scalar, z * scalar);
    }

    void operator*=(const float scalar)
    {
        x *= scalar;
        y *= scalar;
        z *= scalar;
    }

    // Component-wise scalar multiplication
    Vec3f operator*(const Vec3f &other) const
    {
        return Vec3f(x * other.x, y * other.y, z * other.z);
    }

    void operator*=(const Vec3f &other)
    {
        x *= other.x;
        y *= other.y;
        z *= other.z;
    }

    // Scalar division
    Vec3f operator/(const float scalar) const
    {
        return Vec3f(x / scalar, y / scalar, z / scalar);
    }

    void operator/=(const float scalar)
    {
        x /= scalar;
        y /= scalar;
        z /= scalar;
    }

    // Component-wise scalar multiplication
    Vec3f operator/(const Vec3f &other) const
    {
        return Vec3f(x / other.x, y / other.y, z / other.z);
    }

    void operator/=(const Vec3f &other)
    {
        x /= other.x;
        y /= other.y;
        z /= other.z;
    }

    // Dot product
    float dot(const Vec3f &other) const
    {
        return x * other.x + y * other.y + z * other.z;
    }

    // Cross product
    Vec3f cross(const Vec3f &other) const
    {
        return Vec3f(
            y * other.z - z * other.y,
            z * other.x - x * other.z,
            x * other.y - y * other.x);
    }

    // Magnitude (length of vector)
    float magnitude() const
    {
        return std::sqrt(x * x + y * y + z * z);
    }

    // Magnitude squared (length of vector)
    float magnitudeSq() const
    {
        return x * x + y * y + z * z;
    }

    // Normalized vector
    Vec3f normalized() const
    {
        float mag = magnitude();
        return (mag > 0) ? *this / mag : Vec3f(0.f, 0.f, 0.f);
    }

    void normalize()
    {
        float mag = magnitude();
        if (mag > 0)
        {
            x /= mag;
            y /= mag;
            z /= mag;
        }
        else
        {
            x = 0.f;
            y = 0.f;
            z = 0.f;
        }
    }

    // Component-wise minimum
    Vec3f min(const Vec3f &other) const
    {
        return Vec3f(std::fmin(x, other.x), std::fmin(y, other.y), std::fmin(z, other.z));
    }

    // Component-wise maximum
    Vec3f max(const Vec3f &other) const
    {
        return Vec3f(std::fmax(x, other.x), std::fmax(y, other.y), std::fmax(z, other.z));
    }

    bool inside(const Vec3f &min, const Vec3f &max) const
    {
        return (x >= min.x && x <= max.x &&
                y >= min.y && y <= max.y &&
                z >= min.z && z <= max.z);
    }
};