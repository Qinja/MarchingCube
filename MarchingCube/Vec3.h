#pragma once
#include <math.h>
typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned int uint32;
typedef unsigned long long uint64;

class Vec3
{
public:
	__forceinline Vec3() { x = 0.0f; y = 0.0f; z = 0.0f; }
	__forceinline Vec3(const float& x, const float& y, const float& z) :x(x), y(y), z(z) {}
	__forceinline void Normalize()
	{
		const float m = sqrtf(x * x + y * y + z * z);
		if (m > 0.0f)
		{
			const float d = 1.0f /m;
			x *= d;
			y *= d;
			z *= d;
		}
	}
	__forceinline Vec3 operator*(const float& f) const
	{
		return Vec3(x * f, y * f, z * f);
	}
	__forceinline Vec3 operator+(const Vec3& v) const
	{
		return Vec3(x + v.x, y + v.y, z + v.z);
	}
	float x;
	float y;
	float z;
};

