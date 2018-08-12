#if !defined(LUDUM_MATHS_H_)
#define LUDUM_MATHS_H_

#define PI32  3.14159265359f
#define TAU32 6.28318530717958647692f

// @Note: Some simple maths macros
#define Square(x) ((x) * (x))
#define Abs(x) ((x) < 0 ? -(x) : (x))
#define Min(a, b) (a) < (b) ? (a) : (b)
#define Max(a, b) (a) > (b) ? (a) : (b)

#define Clamp(x, min, max) Max(min, Min(x, max))

#define Radians(deg) ((deg) * (PI32 / 180.0f))
#define Degrees(rad) ((rad) * (180.0f / PI32))

inline f32 RandomFloat(f32 min, f32 max) {
    f32 result = 0;
    f32 rnd = (rand() / cast(f32) RAND_MAX);

    result = (min + (rnd * (max - min)));
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

inline bool PlayerHitPlayer(Player *a, Player *b) {
    bool result;
    sf::Vector2f dist_line = b->position - a->position;
    f32 radius_sum = a->radius + b->radius;
    result = Dot(dist_line, dist_line) <= Square(radius_sum);
    return result;
}

inline bool SumoCircleHitPlayer(Sumo_Circle *circle, Player *player) {
    bool result = false;
    sf::Vector2f diff = circle->position - player->position;
    f32 len_sq = Dot(diff, diff);
    f32 radius_sum = circle->radius + player->radius;
    result = len_sq <= Square(radius_sum);

    return result;
}

#endif  // LUDUM_MATHS_H_
