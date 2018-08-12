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

#define ArrayCount(array) (sizeof(array) / sizeof(array[0]))
#define Swap(type, a, b) { type *tmp = a; a = b; b = tmp; }

struct Game_Button {
    bool is_pressed;
    u32 transitions;
};

struct Game_Controller {
    bool is_connected;
    bool is_controller;

    union {
        struct {
            Game_Button action_top;
            Game_Button action_bottom;
            Game_Button action_left;
            Game_Button action_right;

            Game_Button start;
            Game_Button select;
        };
        Game_Button buttons[6];
    };

    sf::Vector2f left_stick;
    sf::Vector2f right_stick;
};

#define MAX_CONTROLLERS 4
struct Game_Input {
    f32 delta_time;

    Game_Controller controllers[MAX_CONTROLLERS];

    sf::Vector2f mouse_position;
    union {
        struct {
            Game_Button mouse_left;
            Game_Button mouse_right;
            Game_Button mouse_middle;
            Game_Button mouse_extra;
        };
        Game_Button mouse_buttons[4];
    };
};

inline Game_Controller *GetGameController(Game_Input *input, u32 index) {
    Assert(index < MAX_CONTROLLERS);
    Game_Controller *result = input->controllers + index;
    return result;
}

inline bool IsButtonPressed(Game_Button button) {
    bool result = button.is_pressed;
    return result;
}

inline bool WasButtonPressed(Game_Button button) {
    bool result = !button.is_pressed && button.transitions > 0;
    return result;
}

inline void ResetButtons(Game_Controller *controller) {
    for (u32 i = 0; i < ArrayCount(controller->buttons); ++i) {
        Game_Button *button = controller->buttons + i;
        button->is_pressed = false;
        button->transitions = 0;
    }
}

struct Player {
    u32 score;

    // The players radius may change because of special moves
    f32 radius;
    sf::Vector2f position;
    sf::Vector2f move_direction;

    bool is_dashing;
    f32 dash_time;
    sf::Vector2f dash_start;

    sf::CircleShape display;

    Player() {
        score = 0;
        is_dashing = false;
        dash_time = 0;
    }
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

struct State;
struct Game_Context {
    sf::RenderWindow *window;
    Game_Input *input;
    State *current_state;
};

#endif  // LUDUM_PLAFORM_H_
