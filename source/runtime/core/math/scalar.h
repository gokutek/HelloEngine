#pragma once

#include <DirectXMath.h>

/**
 * @brief 标量，逻辑上就是float，但是底层仍用XMVECTOR存储，只使用X分量。
 */
class scalar
{
public:
    /**
     * @brief 
     */
    inline scalar() {}

    /**
     * @brief 
     * @param s 
     */
    inline scalar(const scalar& s) { vec_ = s; }

    /**
     * @brief 
     * @param f 
     */
    inline scalar(float f) { vec_ = DirectX::XMVectorReplicate(f); }

    /**
     * @brief 
     * @param vec 
     */
    inline explicit scalar(DirectX::XMVECTOR const& vec) { vec_ = vec; }

    /**
     * @brief 
     */
    inline operator DirectX::XMVECTOR() const { return vec_; }

    /**
     * @brief 
     */
    inline operator float() const { return DirectX::XMVectorGetX(vec_); }

private:
    DirectX::XMVECTOR vec_;
};

inline scalar operator-(scalar s) { return scalar(DirectX::XMVectorNegate(s)); }

inline scalar operator+(scalar s1, scalar s2) { return scalar(DirectX::XMVectorAdd(s1, s2)); }
inline scalar operator-(scalar s1, scalar s2) { return scalar(DirectX::XMVectorSubtract(s1, s2)); }
inline scalar operator*(scalar s1, scalar s2) { return scalar(DirectX::XMVectorMultiply(s1, s2)); }
inline scalar operator/(scalar s1, scalar s2) { return scalar(DirectX::XMVectorDivide(s1, s2)); }

inline scalar operator+(scalar s1, float s2) { return s1 + scalar(s2); }
inline scalar operator-(scalar s1, float s2) { return s1 - scalar(s2); }
inline scalar operator*(scalar s1, float s2) { return s1 * scalar(s2); }
inline scalar operator/(scalar s1, float s2) { return s1 / scalar(s2); }

inline scalar operator+(float s1, scalar s2) { return scalar(s1) + s2; }
inline scalar operator-(float s1, scalar s2) { return scalar(s1) - s2; }
inline scalar operator*(float s1, scalar s2) { return scalar(s1) * s2; }
inline scalar operator/(float s1, scalar s2) { return scalar(s1) / s2; }
