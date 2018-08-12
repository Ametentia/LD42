#if !defined(LUDUM_STATES_H_)
#define LUDUM_STATES_H_
enum State_Type {
    StateType_Logo,
    StateType_MainMenu,
    StateType_Play,
    StateType_CharacterSelect,
    StateType_GameOver
};

struct Logo_State {
    f32 delta_rate;
    f32 rate;
    f32 opacity;

    sf::RectangleShape display;
    sf::Texture texture;

    Logo_State() {
        delta_rate = 1.0;
        rate = 0;
        opacity = 0;

        texture.loadFromFile("logo.png");

        display.setPosition(VIEW_WIDTH / 2.0 - VIEW_HEIGHT / 2.0, 0);
        display.setSize(sf::Vector2f(VIEW_HEIGHT, VIEW_HEIGHT));
        display.setTexture(&texture);
    }
};

struct Menu_State {
    sf::Texture background;
    sf::RectangleShape background_shape;

    sf::Font display_font;

    f32 text_time;
    bool show_text;

    Menu_State() {
        Assert(background.loadFromFile("StartScreen.png"));
        Assert(display_font.loadFromFile("Ubuntu.ttf"));

        text_time = 0;
        show_text = true;
        background_shape.setPosition(0, 0);
        background_shape.setSize(sf::Vector2f(1920, 1080));
        background_shape.setTexture(&background);
    }
};

struct Character_Select_State {
    // @Todo
};

#define MAX_PLAYERS 4
#define MAX_BOTS 8

struct Play_State {
    sf::Font display_font;
    Player players[MAX_PLAYERS];
    s32 player_count;

    Player bots[MAX_BOTS];
    s32 bot_count;

    Sumo_Circle *circle_list;
    s32 circle_count;
    f32 time_since_last_circle;

    f32 min_radius;
    f32 max_radius;

    bool was_f;
    bool was_space;

    Play_State() {
        circle_list = 0;
        time_since_last_circle = 0;
        circle_count = 0;
        player_count = 0;
        bot_count = 0;

        Assert(display_font.loadFromFile("Ubuntu.ttf"));
    }
};


struct State {
   State_Type type;
   union {
       Logo_State *logo_state;
       Play_State *play_state;
       Menu_State *menu_state;
       Character_Select_State *character_state;
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
        case StateType_CharacterSelect: {
            result->type = StateType_CharacterSelect;
            result->character_state = new Character_Select_State;
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
