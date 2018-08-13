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
#define Lerp(begin, end, alpha) (((alpha) * begin) + ((1 - alpha) * end))

struct Game_Button {
    bool is_pressed;
    u32 transitions;
};

struct Game_Controller {
    bool is_connected;
    bool is_controller;

    union {
        struct {
            Game_Button move_up;
            Game_Button move_down;
            Game_Button move_left;
            Game_Button move_right;

            Game_Button action_top;
            Game_Button action_bottom;
            Game_Button action_left;
            Game_Button action_right;

            Game_Button start;
            Game_Button select;
        };
        Game_Button buttons[10];
    };

    sf::Vector2f left_stick;
    sf::Vector2f right_stick;
};

struct Game_Mouse {
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

#define MAX_CONTROLLERS 4
struct Game_Input {
    f32 delta_time;
    bool requested_quit;

    // @Note: Controller 0 is keyboard
    Game_Controller controllers[MAX_CONTROLLERS];
    Game_Mouse mouse;
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

inline bool JustButtonPressed(Game_Button button) {
    bool result = button.is_pressed && button.transitions > 0;
    return result;
}

inline void ResetButtons(Game_Controller *controller) {
    for (u32 i = 0; i < ArrayCount(controller->buttons); ++i) {
        Game_Button *button = controller->buttons + i;
        button->is_pressed = false;
        button->transitions = 0;
    }
}

enum Player_Type {
    PlayerType_SumoCat,
    PlayerType_LuchadorCat,
    PlayerType_AstroCat,
    PlayerType_DevilCat,

    PlayerType_Count
};

struct BotBrain {
    f32 timer;
    sf::Vector2f target;
    f32 reactionTime;
    f32 planExecute;
    f32 wants_dodge;
    u8 action;

    f32 targetNum;

    BotBrain() {
        targetNum = 0;
        action = 1;
        timer = 100000;
        reactionTime = 0.4f;
        planExecute = 0.4;
        wants_dodge = -1;
    };
};

struct Player {
    Player_Type type;

    u32 score;
    f32 score_time;
    u32 hit_count; // How many Sumo_Circles the player is inside of


    bool is_bot;
    bool reversed_controls;

    // @Note: Stats
    f32 move_speed;
    f32 push_strength;
    f32 dash_length;
    //@Note: power variables
    f32 timeLeft;

    // The players radius may change because of special moves
    f32 radius;
    sf::Vector2f position;
    sf::Vector2f move_direction;

    bool alive;

    bool is_dashing;
    f32 dash_time;
    sf::Vector2f dash_start;

    sf::CircleShape display;
    sf::Texture texture;

    BotBrain brain;
    Player() {
        score = 0;
        score_time = 0;
        timeLeft = 0;
        push_strength = 0.2f;
        is_dashing = false;
        dash_time = 0;
        move_direction = sf::Vector2f(0,0);

        radius = 25;
        type = Player_Type::PlayerType_AstroCat;

        alive = true;

        reversed_controls = false;
        is_bot = false;
    }
};

struct Sumo_Circle {
    enum Pattern { Circle, Random };
    sf::Vector2f position;
    f32 radius;
    f32 shrink_delta;
    Pattern pattern;
    bool should_delete;

    sf::CircleShape display;
    sf::CircleShape inner;

    Sumo_Circle *next;
    Sumo_Circle *prev;

    u8 pattern_count;
    u8 pattern_max;

    Sumo_Circle() {
        pattern = Random;
    }
};

struct State;
struct Game_Context {
    sf::RenderWindow *window;
    Game_Input *input;
    State *current_state;
};

#endif  // LUDUM_PLATFORM_H_
