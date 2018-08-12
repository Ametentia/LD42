#if !defined(LUDUM_INTRINSICS_H_)
#define LUDUM_INTRINSICS_H_

#include <math.h>

#ifdef _MSC_VER
    #include <intrin.h>
#elif __llvm__ || __clang__
    #include <x86intrin.h>
#endif

inline f32 Sqrt(f32 value) {
    f32 result = _mm_cvtss_f32(_mm_sqrt_ss(_mm_set_ss(value)));
    return result;
}

inline f32 Sin(f32 rad) {
    f32 result = sinf(rad);
    return result;
}

inline f32 Cos(f32 rad) {
    f32 result = cosf(rad);
    return result;
}

inline f32 Tan(f32 rad) {
    f32 result = tanf(rad);
    return result;
}

#endif  // LUDUM_INTRINSICS_H_
