#pragma once

#include "vector.h"
#include "scalar.h"
#include <DirectXMath.h>


/**
 * @brief 四元数
 * 
 *  X、Y和Z是矢量部分，W是标量部分。
 */
class quaternion
{
public:
    /**
     * @brief 默认构造函数，单位四元数
     */
    quaternion() { vector_ = DirectX::XMQuaternionIdentity(); }

    /**
     * @brief 构造函数，根据旋转轴、旋转角度来构造四元数
     * @param axis 旋转轴
     * @param angle 旋转角度
     */
    quaternion(vector3 const& axis, scalar const& angle) { vector_ = DirectX::XMQuaternionRotationAxis(axis, angle); }

    /**
     * @brief 构造函数，根据欧拉角来构造四元数
     * @param pitch 
     * @param yaw 
     * @param roll 
     */
    quaternion(float pitch, float yaw, float roll) { vector_ = DirectX::XMQuaternionRotationRollPitchYaw(pitch, yaw, roll); }

    /**
     * @brief 构造函数，根据旋转矩阵来构造四元数
     * @param matrix 
     */
    explicit quaternion(DirectX::XMMATRIX const& matrix) { vector_ = DirectX::XMQuaternionRotationMatrix(matrix); }

    /**
     * @brief 构造函数，根据底层向量表示来构造四元数
     * @param vec 
     */
    explicit quaternion(DirectX::FXMVECTOR vec) { vector_ = vec; }

    /**
     * @brief 转换函数，即表示四元数的向量形式
     */
    operator DirectX::XMVECTOR() const { return vector_; }

    /**
     * @brief 
     * @param  
     * @return 
     */
    quaternion operator~(void) const { return quaternion(DirectX::XMQuaternionConjugate(vector_)); }

    /**
     * @brief 
     * @param  
     * @return 
     */
    quaternion operator-(void) const { return quaternion(DirectX::XMVectorNegate(vector_)); }

    /**
     * @brief 
     * @param rhs 
     * @return 
     */
    quaternion operator*(quaternion const& rhs) const { return quaternion(DirectX::XMQuaternionMultiply(rhs, vector_)); }

    /**
     * @brief 
     * @param rhs 
     * @return 
     */
    vector3 operator*(vector3 const& rhs) const { return vector3(DirectX::XMVector3Rotate(rhs, vector_)); }

    /**
     * @brief
     * @param rhs
     * @return
     */
    quaternion& operator*=(quaternion const& rhs) { *this = *this * rhs; return *this; }

    /**
     * @brief 
     * @param rhs 
     * @return 
     */
    quaternion& operator=(quaternion const& rhs) { vector_ = rhs; return *this; }

private:
    DirectX::XMVECTOR vector_;
};
