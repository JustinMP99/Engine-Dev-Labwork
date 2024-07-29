#include "math_types.h"

inline float * end::float3::data()
{
	return &x;
}

inline const float * end::float3::data() const
{
	return &x;
}

inline constexpr size_t end::float3::size()
{
	return 3;
}

















