inline vector2::vector2()
{
	vec_ = DirectX::XMVectorSet(0, 0, 0, 0);
}

inline vector2::vector2(float x, float y)
{
	vec_ = DirectX::XMVectorSet(x, y, 0, 0);
}

inline vector2::operator DirectX::XMVECTOR() const
{
	return vec_;
}

inline scalar vector2::get_x() const
{
	return scalar(DirectX::XMVectorSplatX(vec_));
}

inline scalar vector2::get_y() const
{
	return scalar(DirectX::XMVectorSplatY(vec_));
}

///////////////////////////////////////////////////////////////////////////////

inline vector3::vector3()
{
	vec_ = DirectX::XMVectorSet(0, 0, 0, 0);
}

inline vector3::vector3(float x, float y, float z)
{
	vec_ = DirectX::XMVectorSet(x, y, z, 0);
}

inline vector3::vector3(DirectX::FXMVECTOR vec)
{
	vec_ = vec;
}

inline vector3::operator DirectX::XMVECTOR() const
{
	return vec_; 
}

inline scalar vector3::get_x() const
{
	return scalar(DirectX::XMVectorSplatX(vec_));
}

inline scalar vector3::get_y() const
{
	return scalar(DirectX::XMVectorSplatY(vec_));
}

inline scalar vector3::get_z() const
{
	return scalar(DirectX::XMVectorSplatZ(vec_));
}

inline vector3 vector3::normalize() const
{
	return vector3(DirectX::XMVector3Normalize(vec_));
}

///////////////////////////////////////////////////////////////////////////////

inline vector4::vector4() 
{
	vec_ = DirectX::XMVectorSet(0, 0, 0, 0);
}

inline vector4::vector4(float x, float y, float z, float w)
{
	vec_ = DirectX::XMVectorSet(x, y, z, w);
}

inline vector4::vector4(vector3 const& xyz, float w)
{
	vec_ = DirectX::XMVectorSetW(xyz, w);
}

inline vector4::vector4(DirectX::FXMVECTOR vec)
{
	vec_ = vec;
}

inline vector4::operator DirectX::XMVECTOR() const
{
	return vec_;
}

inline scalar vector4::get_x() const
{
	return scalar(DirectX::XMVectorSplatX(vec_));
}

inline scalar vector4::get_y() const
{
	return scalar(DirectX::XMVectorSplatY(vec_));
}

inline scalar vector4::get_z() const
{
	return scalar(DirectX::XMVectorSplatZ(vec_));
}

inline scalar vector4::get_w() const
{
	return scalar(DirectX::XMVectorSplatW(vec_));
}
