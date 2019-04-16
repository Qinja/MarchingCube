#pragma once
#include <math.h>
class Vec3
{
public:
	inline Vec3() {}
	inline Vec3(float x, float y, float z) :x(x), y(y), z(z) {}
	float x = 0.0f;
	float y = 0.0f;
	float z = 0.0f;
	inline float magnitude() const
	{
		return sqrtf(x * x + y * y + z * z);
	}
	inline float normalize()
	{
		const float m = 1.0f / magnitude();
		if (m > 0.0f)
		{
			*this = *this * (1.0f / m);
		}
		return m;
	}
	inline Vec3 operator*(float f) const
	{
		return Vec3(x * f, y * f, z * f);
	}
	inline Vec3 operator+(Vec3 v) const
	{
		return Vec3(x + v.x, y + v.y, z + v.z);
	}
};

