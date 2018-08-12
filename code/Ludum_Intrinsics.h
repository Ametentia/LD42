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

inline sf::Vector2f Normalise(sf::Vector2f v) {
    sf::Vector2f result;
    f32 len = Length(v);
    if (len != 0) {
        result.x = v.x / len;
        result.y = v.y / len;
    }

    return result;
}

inline bool CircleCheck(Sumo_Circle a, Player b) {
    sf::Vector2f dis(a.position - b.position);
    f32 radius = a.radius + b.radius;
    return Dot(dis, dis) <= Square(radius);
}

inline f32 Absolute(f32 a) {
    if(a < 0) {
        return -a;
    }
    return a;
}

inline bool Offscreen(sf::Vector2f a) {
    return a.x > VIEW_WIDTH || a.x < 0 || a.y > VIEW_HEIGHT || a.y < 0;
}

#endif  // LUDUM_INTRINSICS_H_
