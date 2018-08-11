void AddPlayer(Play_State *play, f32 x, f32 y) {
    Assert(play->player_count < MAX_PLAYERS);
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
void UpdateRenderLogoState(Game_Context *context, Logo_State *logo) {
    logo->rate += logo->delta_rate;
    logo->opacity = Clamp(logo->opacity + DELTA * 2.5 * logo->rate, -0.1, 255);
    logo->display.setFillColor(sf::Color(255, 255, 255, logo->opacity));
    context->window->draw(logo->display);

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space) || logo->opacity <= 0) {
        State *old_state = SetState(context, CreateStateFromType(StateType_Play));
        CleanupState(old_state);
    }
    else if (logo->rate > 75) {
        logo->delta_rate = -logo->delta_rate;
    }
}

void UpdateRenderPlayState(Game_Context *context, Play_State *play) {
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::F) && !play->was_f) {
        AddPlayer(play, RandomFloat(0, VIEW_WIDTH), RandomFloat(0, VIEW_HEIGHT));
    }

    if (play->player_count > 0) {
        UpdatePlayer(play->players);
    }

    for (u32 i = 0; i < play->player_count; ++i) {
        Player *player = play->players + i;
        player->display.setPosition(player->position.x, player->position.y);
        context->window->draw(player->display);
    }

    play->was_f = sf::Keyboard::isKeyPressed(sf::Keyboard::F);
}

void UpdateRenderGame(Game_Context *context) {
    State *current = PeekCurrentState(context);
    Assert(current);
    switch (current->type) {
        case StateType_Logo: {
            UpdateRenderLogoState(context, current->logo_state);
        }
        break;
        case StateType_MainMenu: {
            // @Todo
        }
        break;
        case StateType_Play: {
            UpdateRenderPlayState(context, current->play_state);
        }
        break;
        case StateType_GameOver: {
            // @Todo
        }
        break;
    }
}

