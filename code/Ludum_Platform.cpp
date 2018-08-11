void AddPlayer(Play_State *play, f32 x, f32 y) {
    Assert(play->player_count < (MAX_PLAYERS - 1));
    Player *player = play->players + play->player_count++;

    player->position.x = x;
    player->position.y = y;

    player->display.setRadius(20);
    player->display.setFillColor(sf::Color::Green);
}

void UpdatePlayer(Player *player) {
#define MOVE_SPEED 35
    // @Debug: Nowhere near final haha
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) { player->position.y -= (MOVE_SPEED * DELTA); }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) { player->position.y += (MOVE_SPEED * DELTA); }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) { player->position.x -= (MOVE_SPEED * DELTA); }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) { player->position.x += (MOVE_SPEED * DELTA); }
}

// @Fix: Resolution of the logo is broken
void InitialiseLogo(Logo_State *logo) {
    logo->rate = 0;
    logo->delta_rate = 0.02;
    logo->opacity = 0;

    logo->texture.loadFromFile("logo.png");

    logo->display.setPosition(VIEW_WIDTH / 2.0 - VIEW_HEIGHT / 2.0, 0);
    logo->display.setSize(sf::Vector2f(VIEW_WIDTH, VIEW_HEIGHT));
    logo->display.setTexture(&logo->texture);

    logo->initialised = true;
}

void UpdateRenderLogoState(Game_State *game_state, Logo_State *logo) {
    if (!logo->initialised) InitialiseLogo(logo);

    logo->rate += logo->delta_rate;
    logo->opacity = Clamp(logo->opacity + DELTA * 2.5 * logo->rate, -0.1, 255);
    logo->display.setFillColor(sf::Color(255, 255, 255, logo->opacity));
    game_state->window->draw(logo->display);

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space) || logo->opacity <= 0) {
        SetState(game_state, StateType_PlayState);
    }
    else if (logo->rate > 20) {
        logo->delta_rate = -logo->delta_rate;
    }

}

void UpdateRenderPlayState(Game_State *game_state, Play_State *play) {
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::F) && !play->was_f) {
        AddPlayer(play, 30, 30);
    }

    if (play->player_count > 0) {
        UpdatePlayer(play->players);
    }

    for (u32 i = 0; i < play->player_count; ++i) {
        Player *player = play->players + i;
        player->display.setPosition(player->position.x, player->position.y);
        game_state->window->draw(player->display);
    }

    play->was_f = sf::Keyboard::isKeyPressed(sf::Keyboard::F);
}

void UpdateRenderGame(Game_State *state) {
    switch (PeekCurrentState(state)) {
        case StateType_Logo: {
            UpdateRenderLogoState(state, &state->logo_state);
        }
        break;
        case StateType_MainMenu: {
            // @Todo
        }
        break;
        case StateType_PlayState: {
            UpdateRenderPlayState(state, &state->play_state);
        }
        break;
        case StateType_GameOver: {
            // @Todo
        }
        break;
    }
}
