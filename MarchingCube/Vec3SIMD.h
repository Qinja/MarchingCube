#pragma once

#include <intrin.h>
#include "Vec3.h"
class Vec3SIMD
{
public:
	__forceinline Vec3SIMD() { value = _mm_setzero_ps(); }
	__forceinline Vec3SIMD(const __m128& in) { value = in; }
	__forceinline Vec3SIMD(const float& x, const float& y, const float& z)
	{
		_mm_set_ps(0.0f, z, y, x);
	}
	__forceinline void Normalize()
	{
		const __m128 t0 = _mm_mul_ps(value, value);								//	aw*bw | az*bz | ay*by | ax*bx
		const __m128 t1 = _mm_shuffle_ps(t0, t0, _MM_SHUFFLE(1, 0, 3, 2));		//	ay*by | ax*bx | aw*bw | az*bz
		const __m128 t2 = _mm_add_ps(t0, t1);									//	ay*by + aw*bw | ax*bx + az*bz | aw*bw + ay*by | az*bz + ax*bx
		const __m128 t3 = _mm_shuffle_ps(t2, t2, _MM_SHUFFLE(2, 3, 0, 1));		//	ax*bx + az*bz | ay*by + aw*bw | az*bz + ax*bx | aw*bw + ay*by
		const __m128 dot = _mm_add_ps(t3, t2);
		value = _mm_div_ps(value, _mm_sqrt_ps(dot));
	}
	__forceinline void NormalizeFast()
	{
		const __m128 t0 = _mm_mul_ps(value, value);								//	aw*bw | az*bz | ay*by | ax*bx
		const __m128 t1 = _mm_shuffle_ps(t0, t0, _MM_SHUFFLE(1, 0, 3, 2));		//	ay*by | ax*bx | aw*bw | az*bz
		const __m128 t2 = _mm_add_ps(t0, t1);									//	ay*by + aw*bw | ax*bx + az*bz | aw*bw + ay*by | az*bz + ax*bx
		const __m128 t3 = _mm_shuffle_ps(t2, t2, _MM_SHUFFLE(2, 3, 0, 1));		//	ax*bx + az*bz | ay*by + aw*bw | az*bz + ax*bx | aw*bw + ay*by
		const __m128 dot = _mm_add_ps(t3, t2);
		value = _mm_mul_ps(value, _mm_rsqrt_ps(dot));
	}
	__forceinline Vec3SIMD operator*(float f) const
	{
		return Vec3SIMD(_mm_mul_ps(value, _mm_load_ps1(&f)));
	}
	__forceinline Vec3SIMD operator+(const Vec3SIMD& v) const
	{
		return Vec3SIMD(_mm_add_ps(value, v.value));
	}
	__forceinline Vec3 ToVec3() const
	{
		float arr[4];
		_mm_store_ps(arr, value);
		return Vec3(arr[0], arr[1], arr[2]);
	}
private:
	__m128 value;
};