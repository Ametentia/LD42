#if !defined(LUDUM_STATES_H_)
#define LUDUM_STATES_H_
enum State_Type {
    StateType_Logo,
    StateType_MainMenu,
    StateType_Play,
    StateType_GameOver
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


struct State {
   State_Type type;
   union {
       Logo_State *logo_state;
       Play_State *play_state;
       Menu_State *menu_state;
       // @Todo: Game_Over_State *game_over_state;
       // etc....
   };

   State *next;
};

inline State *PeekCurrentState(Game_Context *context) {
    Assert(context->current_state);
    State *result = context->current_state;
    return result;
}

inline void PushState(Game_Context *context, State *state) {
    if (!context->current_state) {
        context->current_state = state;
    }
    else {
        State *old_top = context->current_state;
        state->next = old_top;
        context->current_state = state;
    }
}

// @Note: Returns the state that was removed from the stack
inline State *PopState(Game_Context *context) {
    State *result = 0;
    if (!context->current_state) return result;

    result = context->current_state;
    context->current_state = result->next;

    return result;
}

// @Note: Returns the state that was removed from the stack
inline State *SetState(Game_Context *context, State *state) {
    State *result = PopState(context);
    PushState(context, state);

    return result;
}

inline State* CreateStateFromType(State_Type type) {
    State *result = new State;
    switch (type) {
        case StateType_Logo: {
            result->type = StateType_Logo;
            result->logo_state = new Logo_State;
        }
        break;
        case StateType_Play: {
            result->type = StateType_Play;
            result->play_state = new Play_State;
        }
        break;
        case StateType_MainMenu: {
            result->type = StateType_MainMenu;
            result->menu_state = new Menu_State;
        }
        break;
        case StateType_GameOver: { Assert(false); } break; // @Todo
    }

    result->next = 0;
    return result;
}

inline void CleanupState(State *state) {
    switch (state->type) {
        case StateType_Logo: { delete state->logo_state; } break;
        case StateType_Play: { delete state->play_state; } break;
        case StateType_MainMenu: { delete state->menu_state; } break;
        case StateType_GameOver: { Assert(false); } break; // @Todo
    }

    delete state;
}

#endif  // LUDUM_STATES_H_
