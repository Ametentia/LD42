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

inline f32 Dot(sf::Vector2f a, sf::Vector2f b) {
    f32 result = a.x * b.x + a.y * b.y;
    return result;
}

inline f32 Length(sf::Vector2f v) {
    f32 result = Sqrt(Dot(v, v));
    return result;
}

inline bool CircleCheck(sf::CircleShape a, sf::CircleShape b) {
    f32 x = a.getPosition().x - b.getPosition().x;
    f32 y = a.getPosition().y - b.getPosition().y;
    f32 r = a.getRadius() + b.getRadius();
    x*=x;
    y*=y;
    r*=r;
    return r > x + y;
}

#endif  // LUDUM_INTRINSICS_H_
