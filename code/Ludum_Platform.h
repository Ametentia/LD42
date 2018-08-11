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

enum State_Type {
    StateType_Logo,
    StateType_MainMenu,
    StateType_PlayState,
    StateType_GameOver
};

/*
   @Todo: Maybe store states like this
   struct State {
       State_Type type;
       union {
           Logo_State logo_state;
           Play_State play_state;
           Menu_State menu_state;
           Game_Over_State game_over_state;
           // etc....
       };

       State *next;
   };

   Then PushState, PopState and SetState can just manipulate the linked list
*/

struct Player {
    sf::CircleShape display;
    sf::Vector2f position;
};

struct Logo_State {
    bool initialised;
    f32 delta_rate;
    f32 rate;
    f32 opacity;

    sf::RectangleShape display;
    sf::Texture texture;
};

struct Menu_State {
    // @Note: Not real
    // @Todo: Implement actual menu
    u32 x, y;
};

#define MAX_PLAYERS 4
struct Play_State {
    Player players[MAX_PLAYERS];
    u32 player_count;

    // @Todo: At this point I might as well to proper input
    bool was_f;
};

#define MAX_STATES 10
struct Game_State {
    sf::RenderWindow *window;

    s32 state_count;
    State_Type current_states[MAX_STATES];

    Logo_State logo_state;
    Play_State play_state;
    Menu_State menu_state;
};

inline State_Type PeekCurrentState(Game_State *state) {
    State_Type result = state->current_states[state->state_count - 1];
    return result;
}

inline void PushState(Game_State *state, State_Type type) {
    Assert(state->state_count < MAX_STATES);
    state->current_states[state->state_count++] = type;
}

inline void PopState(Game_State *state) {
    state->state_count--;
}

inline void SetState(Game_State *state, State_Type type) {
    PopState(state);
    PushState(state, type);
}

#endif  // LUDUM_PLATFORM_H_
