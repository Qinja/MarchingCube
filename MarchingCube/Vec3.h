#pragma once
#include <math.h>

typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned int uint32;
typedef unsigned long long uint64;


class Vec3
{
public:
	__forceinline Vec3() {}
	__forceinline Vec3(float x, float y, float z) :x(x), y(y), z(z) {}
	float x = 0.0f;
	float y = 0.0f;
	float z = 0.0f;
	__forceinline float magnitude() const
	{
		return sqrtf(x * x + y * y + z * z);
	}
	__forceinline float normalize()
	{
		const float m = 1.0f / magnitude();
		if (m > 0.0f)
		{
			*this = *this * (1.0f / m);
		}
		return m;
	}
	__forceinline Vec3 operator*(float f) const
	{
		return Vec3(x * f, y * f, z * f);
	}
	__forceinline Vec3 operator+(Vec3 v) const
	{
		return Vec3(x + v.x, y + v.y, z + v.z);
	}
};

