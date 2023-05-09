#pragma once

#include "scalar.h"
#include <DirectXMath.h>

/**
 * @brief 2D向量
 */
class vector2
{
public:
    vector2();
    vector2(float x, float y);

    operator DirectX::XMVECTOR() const;

    scalar get_x() const;
    scalar get_y() const;

private:
    DirectX::XMVECTOR vec_;
};

/**
 * @brief 3D向量
 */
class vector3
{
public:
    vector3();
    vector3(float x, float y, float z);
    explicit vector3(DirectX::FXMVECTOR vec);

    operator DirectX::XMVECTOR() const;

    scalar get_x() const;
    scalar get_y() const;
    scalar get_z() const;

    vector3 normalize() const;

private:
    DirectX::XMVECTOR vec_;
};

/**
 * @brief 4D向量
 */
class vector4
{
public:
    vector4();
    vector4(float x, float y, float z, float w);
    vector4(vector3 const& xyz, float w);
    explicit vector4(DirectX::FXMVECTOR vec);

    operator DirectX::XMVECTOR() const;

    scalar get_x() const;
    scalar get_y() const;
    scalar get_z() const;
    scalar get_w() const;

private:
    DirectX::XMVECTOR vec_;
};

#include "vector.inl"
