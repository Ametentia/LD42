#if !defined(LUDUM_PLATFORM_H_)
#define LUDUM_PLATFORM_H_

#include <stdint.h>

typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t  s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;

typedef float  f32;
typedef double f64;

typedef uintptr_t umm;
typedef intptr_t  smm;

#define cast

#define Assert(exp) \
do { \
    if (!(exp)) { \
        printf("Assertion Failed: %s (%s -> %d)\n", #exp, __FILE__, __LINE__); \
        asm("int3"); \
    } \
} while (0)

#define global static
#define internal static
#define local static

#define PI32  3.14159265359f
#define TAU32 6.28318530717958647692f

#define Abs(x) ((x) < 0 ? -(x) : (x))
#define Min(a, b) (a) < (b) ? (a) : (b)
#define Max(a, b) (a) > (b) ? (a) : (b)

#define Clamp(x, min, max) Max(min, Min(x, max))

#define Radians(deg) ((deg) * (PI32 / 180.0f))
#define Degrees(rad) ((rad) * (180.0f / PI32))

struct State;

struct Player {
    u32 score;
    sf::CircleShape display;
    sf::Vector2f position;
};

struct Sumo_Circle {
    sf::Vector2f position;
    f32 radius;
    f32 shrink_delta;

    sf::CircleShape display;

    Sumo_Circle *next;
    Sumo_Circle *prev;

    Sumo_Circle() {}
};

struct Game_Context {
    sf::RenderWindow *window;
    State *current_state;
};

inline f32 RandomFloat(f32 min, f32 max) {
    f32 result = 0;
    f32 rnd = (rand() / cast(f32) RAND_MAX);

    result = (min + (rnd * (max - min)));
    return result;
}

#endif  // LUDUM_PLATFORM_H_
