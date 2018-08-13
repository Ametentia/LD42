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

    sf::Texture start;
    sf::RectangleShape start_shape;

    f32 text_time;
    bool show_text;

    Menu_State() {
        Assert(background.loadFromFile("StartScreen.png"));
        Assert(start.loadFromFile("StartGame.png"));

        text_time = 0;
        show_text = true;
        background_shape.setPosition(0, 0);
        background_shape.setSize(sf::Vector2f(VIEW_WIDTH, VIEW_HEIGHT));
        background_shape.setTexture(&background);

        sf::Vector2f size(start.getSize());
        f32 ratio = size.x / 1920;
        size.x = 1920;
        size.y = (size.y / ratio);
        start_shape.setSize(size);
        start_shape.setTexture(&start);
        start_shape.setPosition(VIEW_WIDTH / 2.0 - (size.x / 2.0) + 45,
                VIEW_HEIGHT - (size.y) - 60);
    }
};

struct Character_Select_State {
    sf::Texture characters[4];
    sf::Texture characters_gray[4];
    sf::RectangleShape character_shapes[4];

    sf::Texture ready_texture;
    sf::Texture border;
    sf::RectangleShape border_shape;

    s32 index;
    f32 scale_offset;

    bool ready;

    Character_Select_State() {
        Assert(characters[0].loadFromFile("SumoSelected.png"));
        Assert(characters[1].loadFromFile("LuchadoreSelected.png"));
        Assert(characters[2].loadFromFile("AstroSelected.png"));
        Assert(characters[3].loadFromFile("DevilSelected.png"));

        Assert(characters_gray[0].loadFromFile("SumoUnselected.png"));
        Assert(characters_gray[1].loadFromFile("LuchadoreUnselected.png"));
        Assert(characters_gray[2].loadFromFile("AstroUnselected.png"));
        Assert(characters_gray[3].loadFromFile("DevilUnselected.png"));

        for (u32 i = 0; i < 4; ++i) {
            sf::RectangleShape *shape = character_shapes + i;
            shape->setPosition(i * (VIEW_WIDTH / 4.0), 0);
            shape->setSize(sf::Vector2f(characters[i].getSize().x,  VIEW_HEIGHT));
            shape->setTexture(characters + i);
        }

        Assert(ready_texture.loadFromFile("CSSOverlaySelected.png"));
        Assert(border.loadFromFile("CSSOverlay.png"));
        border_shape.setPosition(0, 0);
        border_shape.setSize(sf::Vector2f(VIEW_WIDTH, VIEW_HEIGHT));
        border_shape.setTexture(&border);

        ready = false;
        index = 0;
        scale_offset = 0;
    }
};

#define MAX_PLAYERS 4
#define MAX_BOTS 8

struct Play_State {
    sf::Texture player_textures[4];

    // @Note: For supers
    sf::Texture ui_elements[8];
    sf::RectangleShape charge_bars[4];

    sf::Font display_font;
    Player players[MAX_PLAYERS];
    s32 player_count;

    Player bots[MAX_BOTS];
    s32 bot_count;

    Sumo_Circle *circle_list;
    s32 circle_count;
    f32 time_since_last_circle;
    f32 count_down;

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
        count_down = 4;

        Assert(player_textures[0].loadFromFile("SumoBall.png"));
        Assert(player_textures[1].loadFromFile("LuchadoreBall.png"));
        Assert(player_textures[2].loadFromFile("AstroBall.png"));
        Assert(player_textures[3].loadFromFile("DevilBall.png"));

        Assert(ui_elements[0].loadFromFile("SumoBarEmpty.png"));
        Assert(ui_elements[1].loadFromFile("LuchadoreBarEmpty.png"));
        Assert(ui_elements[2].loadFromFile("AstroBarEmpty.png"));
        Assert(ui_elements[3].loadFromFile("DevilBarEmpty.png"));

        Assert(ui_elements[4].loadFromFile("SumoBarFull.png"));
        Assert(ui_elements[5].loadFromFile("LuchadoreBarFull.png"));
        Assert(ui_elements[6].loadFromFile("AstroBarFull.png"));
        Assert(ui_elements[7].loadFromFile("DevilBarFull.png"));

        sf::Vector2f size = sf::Vector2f(ui_elements[0].getSize());
        f32 ratio = size.y / size.x;
        for (u32 i = 0; i < 4; ++i) {
            f32 width = (VIEW_WIDTH / 4.0);
            charge_bars[i].setSize(sf::Vector2f(width, width * ratio));
            charge_bars[i].setPosition(i * (VIEW_WIDTH / 4.0), VIEW_HEIGHT - (width * ratio));
            charge_bars[i].setTexture(ui_elements + i);
        }

        Assert(display_font.loadFromFile("Ubuntu.ttf"));
    }
};

struct Game_Over_State {
    bool won;
    Player_Type winner;
    Score scores[4];

    sf::Font display_font;
    sf::Texture char_display[4];

    Game_Over_State() {
        Assert(display_font.loadFromFile("Ubuntu.ttf"));

        Assert(char_display[0].loadFromFile("SumoCat.png"));
        Assert(char_display[1].loadFromFile("LuchadoreCat.png"));
        Assert(char_display[2].loadFromFile("AstroCat.png"));
        Assert(char_display[3].loadFromFile("DevilCat.png"));
    }
};

struct State {
   State_Type type;
   union {
       Logo_State *logo_state;
       Play_State *play_state;
       Menu_State *menu_state;
       Character_Select_State *character_state;
       Game_Over_State *game_over_state;
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
        case StateType_GameOver: {
            result->type = StateType_GameOver;
            result->game_over_state = new Game_Over_State;
        }
        break;
    }

    result->next = 0;
    return result;
}

inline void CleanupState(State *state) {
    switch (state->type) {
        case StateType_Logo: { delete state->logo_state; } break;
        case StateType_Play: { delete state->play_state; } break;
        case StateType_MainMenu: { delete state->menu_state; } break;
        case StateType_CharacterSelect: { delete state->character_state; } break;
        case StateType_GameOver: { delete state->game_over_state; } break;
    }

    delete state;
}

#endif  // LUDUM_STATES_H_
