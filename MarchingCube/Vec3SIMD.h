#pragma once

#include <intrin.h>
#include "Vec3.h"
class Vec3_Float32_SIMD
{
public:
	__forceinline Vec3_Float32_SIMD() { value = _mm_setzero_ps(); }
	__forceinline Vec3_Float32_SIMD(const __m128& in) { value = in; }
	__forceinline Vec3_Float32_SIMD(const float& x, const float& y, const float& z)
	{
		value = _mm_set_ps(0.0f, z, y, x);
	}
	__forceinline Vec3_Float32_SIMD(const float*& arr)
	{
		value = _mm_load_ps(arr);
	}
	__forceinline Vec3_Float32_SIMD(const Vec3_Int32_SIMD& in)
	{
		value = _mm_cvtepi32_ps(in.value);
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
	__forceinline Vec3_Float32_SIMD Dot(Vec3_Float32_SIMD v) const
	{
		return _mm_dp_ps(value, v.value, 0xff);
		//const __m128 t0 = _mm_mul_ps(value, v.value);							//	aw*bw | az*bz | ay*by | ax*bx
		//const __m128 t1 = _mm_shuffle_ps(t0, t0, _MM_SHUFFLE(1, 0, 3, 2));		//	ay*by | ax*bx | aw*bw | az*bz
		//const __m128 t2 = _mm_add_ps(t0, t1);									//	ay*by + aw*bw | ax*bx + az*bz | aw*bw + ay*by | az*bz + ax*bx
		//const __m128 t3 = _mm_shuffle_ps(t2, t2, _MM_SHUFFLE(2, 3, 0, 1));		//	ax*bx + az*bz | ay*by + aw*bw | az*bz + ax*bx | aw*bw + ay*by
		//const __m128 dot = _mm_add_ps(t3, t2);
	}
	__forceinline Vec3_Float32_SIMD operator*(float f) const
	{
		return Vec3_Float32_SIMD(_mm_mul_ps(value, _mm_load_ps1(&f)));
	}
	__forceinline Vec3_Float32_SIMD operator+(const Vec3_Float32_SIMD& v) const
	{
		return Vec3_Float32_SIMD(_mm_add_ps(value, v.value));
	}
	__forceinline Vec3 ToVec3() const
	{
		float arr[4];
		_mm_store_ps(arr, value);
		return Vec3(arr[0], arr[1], arr[2]);
	}
	__m128 value;
};

class Vec3_Int32_SIMD
{
public:
	__forceinline Vec3_Int32_SIMD() { value = _mm_setzero_si128(); }
	__forceinline Vec3_Int32_SIMD(const __m128i& in) { value = in; }
	__forceinline Vec3_Int32_SIMD(const int & x, const int& y, const int& z)
	{
		value = _mm_set_epi32(0, z, y, x);
	}
	__forceinline Vec3_Int32_SIMD operator+(const Vec3_Int32_SIMD& v) const
	{
		return Vec3_Int32_SIMD(_mm_add_epi32(value, v.value));
	}
	__forceinline Vec3_Int32_SIMD Dot(Vec3_Int32_SIMD v) const
	{
		//return _mm_mullo_epi32(value, v.value);
	}
	__forceinline int DotValue(Vec3_Int32_SIMD v) const
	{
		//return _mm_mullo_epi32(value, v.value);
	}
	__m128i value;
};