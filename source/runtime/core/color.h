#pragma once

#include <DirectXMath.h>
#include <stdint.h>

struct color
{
	color(float r, float g, float b, float a);

	float get_r() const;
	float get_g() const;
	float get_b() const;
	float get_a() const;

	void set_r(float r);
	void set_g(float g);
	void set_b(float b);
	void set_a(float a);

	//TODO: 不同颜色格式的转换，如sRGB

private:
	DirectX::XMVECTORF32 value_; ///< 归一化后的RGBA
};

inline color::color(float r, float g, float b, float a)
{
	value_.v = DirectX::XMVectorSet(r, g, b, a);
	//TODO: 与直接设置value_.f的对比
}

inline float color::get_r() const
{
	return DirectX::XMVectorGetX(value_);
}

inline float color::get_g() const
{
	return DirectX::XMVectorGetY(value_);
}

inline float color::get_b() const
{
	return DirectX::XMVectorGetZ(value_);
}

inline float color::get_a() const
{
	return DirectX::XMVectorGetW(value_);
}

inline void color::set_r(float r)
{
	value_.f[0] = r;
}

inline void color::set_g(float g)
{
	value_.f[1] = g;
}

inline void color::set_b(float b)
{
	value_.f[2] = b;
}

inline void color::set_a(float a)
{
	value_.f[3] = a;
}
