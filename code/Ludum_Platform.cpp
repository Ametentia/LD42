// Will create a new circle at the given (x, y) position and the given radius
void AddSumoCircle(Play_State *play_state, f32 x, f32 y, f32 radius, f32 shrink_delta = RandomFloat(15, 50)) {
    Sumo_Circle *result = new Sumo_Circle;

    result->radius = radius;
    result->position.x = x;
    result->position.y = y;

    result->shrink_delta = shrink_delta;

    result->display.setRadius(radius);
    result->display.setOrigin(radius, radius);
    result->display.setPosition(result->position);

    result->display.setFillColor(sf::Color::Transparent);
    result->display.setOutlineColor(sf::Color::White);
    result->display.setOutlineThickness(-2.0);

    Sumo_Circle *old_head =play_state->circle_list_head;
    result->next = old_head;
    if(old_head) old_head->prev = result;

    result->prev = 0;
    play_state->circle_list_head = result;
    play_state->circle_count++;
}

// Will create a new circle relative to the one given
void AddSumoCircle(Play_State *play_state, Sumo_Circle *base) {
    f32 angle = RandomFloat(0, TAU32);

#if 1
    f32 radius = RandomFloat(play_state->min_radius, play_state->max_radius);
    f32 x = base->position.x + radius * Sin(angle);
    x = Clamp(x, 120, VIEW_WIDTH - (120 / 2.0));
    f32 y = base->position.y + radius * Cos(angle);
    y = Clamp(y, 120, VIEW_HEIGHT - (120 / 2.0));
#else
    f32 x = RandomFloat(0.2 * VIEW_WIDTH, VIEW_WIDTH - (0.2 * VIEW_WIDTH));
    f32 y = RandomFloat(0.2 * VIEW_HEIGHT, VIEW_HEIGHT - (0.2 * VIEW_HEIGHT));

    sf::Vector2f diff = base->position - sf::Vector2f(x, y);
    f32 radius = Max(Length(diff), play_state->min_radius);
#endif

    AddSumoCircle(play_state, x, y, radius, base->shrink_delta + RandomFloat(-0.1, 0.1));
}

void RemoveSumoCircle(Play_State *play_state, Sumo_Circle *circle) {
    if (circle->next) circle->next->prev = circle->prev;
    if (circle->prev) circle->prev->next = circle->next;

    play_state->circle_count--;
    delete circle;
}

bool UpdateSumoCircle(Sumo_Circle *circle) {
    circle->radius -= circle->shrink_delta * DELTA;
    if (circle->radius <= 10) {
        return true;
    }

    circle->display.setRadius(circle->radius);
    circle->display.setOrigin(circle->radius, circle->radius);
    return false;
}

void AddPlayer(Play_State *play_state, f32 x, f32 y) {
    if (play_state->player_count >= MAX_PLAYERS) return;
    Player *player = play_state->players + play_state->player_count++;

    player->position.x = x;
    player->position.y = y;

    player->display.setRadius(20);
    player->display.setFillColor(sf::Color::Green);
}

void UpdatePlayer(Player *player) {
#define MOVE_SPEED 350
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
        Play_State *play_state = context->current_state->play_state;

        play_state->min_radius = 120;
        play_state->max_radius = 500;
        AddSumoCircle(play_state, VIEW_WIDTH / 2.0, VIEW_HEIGHT / 2.0, play_state->max_radius);
        CleanupState(old_state);
    }
    else if (logo->rate > 75) {
        logo->delta_rate = -logo->delta_rate;
    }
}

void UpdateRenderPlayState(Game_Context *context, Play_State *play_state) {
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::F) && !play_state->was_f) {
        AddPlayer(play_state, RandomFloat(VIEW_WIDTH / 2.0 - play_state->min_radius,
                    VIEW_WIDTH / 2.0 + play_state->min_radius), RandomFloat(VIEW_HEIGHT / 2.0
                        - play_state->min_radius, VIEW_HEIGHT / 2.0 + play_state->min_radius));
    }

    if (play_state->player_count > 0) {
        UpdatePlayer(play_state->players);
    }
    if (play_state->time_since_last_circle >= 4
            || play_state->circle_list_head->radius < play_state->min_radius * 0.6)
    {
        if (play_state->circle_list_head && play_state->circle_count <= 3) {
            AddSumoCircle(play_state, play_state->circle_list_head);
        }
        else {
            AddSumoCircle(play_state, (2 * play_state->max_radius) + (0.1 * VIEW_WIDTH),
                    (2 * play_state->max_radius) + (0.1 * VIEW_HEIGHT), play_state->max_radius);
        }
        play_state->time_since_last_circle = 0;
    }

    Sumo_Circle *head = play_state->circle_list_head;
    head->display.setOutlineColor(sf::Color::Yellow);
    play_state->players->display.setFillColor(sf::Color::Red);
    if (head->next) head->next->display.setOutlineColor(sf::Color::Magenta);
    while (head) {
        bool should_delete = UpdateSumoCircle(head);
        if (!should_delete) {
            sf::CircleShape centre(10);
            centre.setPosition(head->position);
            centre.setOrigin(10, 10);
            centre.setFillColor(sf::Color::Red);
            context->window->draw(centre);
            context->window->draw(head->display);
            if (CircleCheck(head->display, play_state->players->display)) {
                play_state->players->display.setFillColor(sf::Color::Green);
            }
            head = head->next;
        }
        else {
            Sumo_Circle *old = head;
            Assert(old != play_state->circle_list_head || play_state->circle_list_head->next);
            head = head->next;
            RemoveSumoCircle(play_state, old);
        }
    }

    play_state->circle_list_head->display.setOutlineColor(sf::Color::White);
    if (play_state->circle_list_head->next)
        play_state->circle_list_head->next->display.setOutlineColor(sf::Color::White);

    for (u32 i = 0; i < play_state->player_count; ++i) {
        Player *player = play_state->players + i;
        player->display.setPosition(player->position.x, player->position.y);
        context->window->draw(player->display);
    }


    // Post update and render increments
    play_state->time_since_last_circle += DELTA;
    play_state->was_f = sf::Keyboard::isKeyPressed(sf::Keyboard::F);
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

