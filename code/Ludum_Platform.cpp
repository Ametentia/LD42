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

    result->inner.setRadius(radius - 3);
    result->inner.setOrigin(radius - 3 , radius - 3);
    result->inner.setPosition(result->position);

    result->inner.setFillColor(sf::Color::Black);
    result->inner.setOutlineColor(sf::Color::Transparent);
    result->inner.setOutlineThickness(-2.0);

    Sumo_Circle *old_head = play_state->circle_list;
    result->next = old_head;
    if(old_head) old_head->prev = result;

    result->prev = 0;
    play_state->circle_list = result;
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

void RemoveSumoCircles(Play_State *play_state) {
    Sumo_Circle *current = play_state->circle_list;
    while (current) {
        if (current->should_delete) {
            if (current->next) current->next->prev = current->prev;
            if (current->prev) current->prev->next = current->next;

            Sumo_Circle *prev = current;
            current = prev->next;

            play_state->circle_count--;
            delete current;
        }
        else {
            current = current->next;
        }
    }
}

void UpdateSumoCircles(Play_State *play_state, f32 delta_time) {
    Sumo_Circle *current = play_state->circle_list;
    while (current) {
        current->radius -= current->shrink_delta * delta_time;
        current->display.setRadius(current->radius);
        current->display.setOrigin(current->radius, current->radius);
        current->inner.setRadius(current->radius - 3);
        current->inner.setOrigin(current->radius - 3, current->radius - 3);

        current->should_delete = current->radius <= 10;
        current = current->next;
    }
}

void AddPlayer(Play_State *play_state, Player_Type type, f32 x, f32 y) {
    if (play_state->player_count >= MAX_PLAYERS) return;
    Player *player = play_state->players + play_state->player_count++;

    player->type = type;
    player->position = { x, y };
    player->radius = 20;
    player->display.setRadius(20);

    switch (player->type) {
        case PlayerType_SumoCat: {
            player->display.setFillColor(sf::Color::White);
            player->push_strength = 100;
            player->move_speed = 250;
            player->dash_length = 0.1;
        }
        break;
        case PlayerType_LuchadorCat: {
            player->display.setFillColor(sf::Color(24, 213, 75));

            player->move_speed = 250;
            player->push_strength = 60;
            player->dash_length = 0.2;
        }
        break;
        case PlayerType_AstroCat: {
            player->display.setFillColor(sf::Color(113, 229, 255));

            player->move_speed = 450;
            player->push_strength = 10;
            player->dash_length = 0.4;
        }
        break;
        case PlayerType_DevilCat: {
            player->display.setFillColor(sf::Color::Red);

            // @Todo
            player->move_speed = 350;
            player->push_strength = 20;
            player->dash_length = 0.2;
        }
        break;
    }
    player->display.setOrigin(20, 20);
}

#define MOVE_SPEED 350
#define DASH_SPEED 1000

void DashPlayer(Player *player, f32 dash_time) {
    player->is_dashing = true;
    player->dash_time = dash_time;
    player->dash_start = player->position;
}

void UpdatePlayer(Player *player, Game_Controller *controller, f32 delta_time) {
    f32 speed = player->move_speed;
    bool has_input = player->is_dashing;
    if (player->is_dashing) {
        player->dash_time -= delta_time;
        player->is_dashing = player->dash_time > 0;
        speed = DASH_SPEED;
    }
    else {
        sf::Vector2f move_direction = player->move_direction;
        if (controller->is_controller) {
            move_direction = Normalise(controller->left_stick);
        }
        else {
            // @Hack: Find a better way of doing this?
            has_input = IsButtonPressed(controller->move_up) || IsButtonPressed(controller->move_down) ||
                IsButtonPressed(controller->move_left) || IsButtonPressed(controller->move_right);

            u32 control_offset = player->reversed_controls ? 1 : 0;
            if (IsButtonPressed(controller->buttons[0 + control_offset]))    { move_direction.y -= 1; }
            if (IsButtonPressed(controller->buttons[1 - control_offset]))  { move_direction.y += 1; }
            if (IsButtonPressed(controller->buttons[2 + control_offset]))  { move_direction.x -= 1; }
            if (IsButtonPressed(controller->buttons[3 - control_offset])) { move_direction.x += 1; }

            if (JustButtonPressed(controller->action_bottom)) {
                speed = DASH_SPEED;
                DashPlayer(player, 0.2);
            }
        }

        move_direction = Normalise(move_direction);
        player->move_direction = move_direction;
    }

    player->position += speed * delta_time * player->move_direction;
    if (!has_input) { player->move_direction = {}; }

    player->score_time += delta_time;
    if (player->score_time >= 1) {
        player->score += player->hit_count;
        player->score_time = 0;
    }
}

void CheckBounds(Sumo_Circle *circle_list, Player *player) {
    u32 in_count = 0;
    if (circle_list && player) {
        Sumo_Circle *circle = circle_list;
        while (circle) {
            if (SumoCircleHitPlayer(circle, player)) in_count++;
            circle = circle->next;
        }

        player->alive = in_count > 0;
        player->hit_count = in_count;
    }
}

void ResolveCollision(Player *a, Player *b) {
    sf::Vector2f impact = a->position - b->position;
    impact = Normalise(impact);

    f32 impact_a = Abs(Dot(a->move_direction, impact));
    f32 impact_b = Abs(Dot(b->move_direction, impact));

    sf::Vector2f deflect_a = impact_a *  impact;
    sf::Vector2f deflect_b = impact_b * -impact;

    a->move_direction += (deflect_a - deflect_b);
    b->move_direction += (deflect_b - deflect_a);


    if (a->is_dashing && b->is_dashing) {
        a->dash_time = 0.05f;
        b->dash_time = 0.05f;
    }
    else if (a->is_dashing) {
        f32 push_factor = a->push_strength;
        a->is_dashing = false;
        DashPlayer(b, push_factor / 100);
    }
    else if (b->is_dashing) {
        f32 push_factor = b->push_strength;
        b->is_dashing = false;
        DashPlayer(a, push_factor / 100);
    }

}

void CheckCollisions(Play_State *play_state) {
    // Every Player
    for (u32 i = 0; i < play_state->player_count; ++i) {
        Player *a = play_state->players + i;
        // Against Every Player
        for (u32 j = i + 1; j < play_state->player_count; ++j) {
            Player *b = play_state->players + j;
            if (PlayerHitPlayer(a, b)) { ResolveCollision(a, b); }
        }

        // Against Every Bot
        for (u32 j = 0; j < play_state->bot_count; ++j) {
            Player *b = play_state->bots + j;
            if (PlayerHitPlayer(a, b)) { ResolveCollision(a, b); }
        }
    }

    // Every Bot
    for (u32 i = 0; i < play_state->bot_count; ++i) {
        Player *a = play_state->bots + i;
        // Against Every Bot
        for (u32 j = i + 1; j < play_state->bot_count; ++j) {
            Player *b = play_state->bots + j;
            if (PlayerHitPlayer(a, b)) { ResolveCollision(a, b); }
        }
    }
}

void RenderSumoCircles(Game_Context *context, Play_State *play_state) {
    Sumo_Circle *current = play_state->circle_list;
    while (current) {
        f32 alpha = 1;
        f32 value = Lerp(255, 0, alpha);
        value = Clamp(value, 0, 255);
        current->display.setOutlineColor(sf::Color(255, value, value));
        context->window->draw(current->display);
        // @Bug: This trips 'Fatal Error' in ucrtbase.dll sometimes will investigate
        current = current->next;
    }

    current = play_state->circle_list;
    while (current) {
        context->window->draw(current->inner);
        current = current->next;
    }

#if 0

 // @Note: Rendering the circles
    head->display.setOutlineColor(sf::Color::Yellow);
    if (head->next) head->next->display.setOutlineColor(sf::Color::Magenta);

    while (head) {
        bool should_delete = UpdateSumoCircle(head, input->delta_time);
        if (!should_delete) {
            sf::CircleShape centre(10);
            centre.setPosition(head->position);
            centre.setOrigin(10, 10);
            centre.setFillColor(sf::Color::Red);
            context->window->draw(centre);
            context->window->draw(head->display);
            head = head->next;
        }
        else {
            Sumo_Circle *old = head;
            Assert(old != play_state->circle_list || play_state->circle_list->next);
            head = head->next;
            RemoveSumoCircle(play_state, old);
        }
    }

    head = play_state->circle_list;
    while(head) {
        context->window->draw(head->inner);
        head = head-> next;
    }
#endif
}

void RenderEntities(Game_Context *context, Play_State *play_state) {
    for (u32 i = 0; i < play_state->player_count; ++i) {
        Player *player = play_state->players + i;
        if (player->alive) {
            player->display.setPosition(player->position);
            context->window->draw(player->display);
        }
    }

    for (u32 i = 0; i < play_state->bot_count; ++i) {
        Player *bot = play_state->players + i;
        if (bot->alive) {
            bot->display.setPosition(bot->position);
            context->window->draw(bot->display);
        }
    }
}

void UpdateRenderPlayState(Game_Context *context, Play_State *play_state) {
    Game_Input *input = context->input;

    // @Debug: Adding Players and quit on escape
    {
        Game_Controller *controller = GetGameController(input, 0);
        if (JustButtonPressed(controller->action_top)) {
            AddPlayer(play_state, cast(Player_Type) RandomInt(0, 4),
                    RandomFloat(VIEW_WIDTH / 2.0 - play_state->min_radius,
                        VIEW_WIDTH / 2.0 + play_state->min_radius), RandomFloat(VIEW_HEIGHT / 2.0
                            - play_state->min_radius, VIEW_HEIGHT / 2.0 + play_state->min_radius));
        }

        if (JustButtonPressed(controller->select)) {
            Player *player = play_state->players;
            player->reversed_controls = !player->reversed_controls;
        }

        if (IsButtonPressed(controller->start)) input->requested_quit = true;
    }


    // Check for any collisions
    CheckCollisions(play_state);

    // Update Players based off input
    for (u32 i = 0; i < play_state->player_count; ++i) {
        Player *player = play_state->players + i;
        Game_Controller *controller = GetGameController(input, i);
        if (player->alive) {
            UpdatePlayer(player, controller, input->delta_time);
            CheckBounds(play_state->circle_list, player);
        }
    }


#if 0
    // Update any AI entities involved
    for (u32 i = 0; i < play_state->bot_count; ++i) {
        Player *bot = play_state->bots + i;
        UpdateBot(bot);
        CheckBounds(play_state->circle_list, bot);
    }
#endif
    // Update Sumo Circles
    UpdateSumoCircles(play_state, input->delta_time);

    // Remove any circles that have become too small
    RemoveSumoCircles(play_state);
#if 0
    // Spawn new circles when necessary
    //SpawnSumoCircles(play_state);

    // Render Dash Trails. Before rendering actual entities so the entities are still on top
    // when dashing close to entities
    RenderDashTrails(play_state);
#else

    // Render Sumo Circles
    RenderSumoCircles(context, play_state);


    for (u32 i = 0; i < play_state->player_count; ++i) {
        Player *player = play_state->players + i;
        char buffer[256];
        snprintf(buffer, 256, "Player [%d] Score: %d\n", i + 1, player->score);
        sf::Text text;
        text.setFont(play_state->display_font);
        text.setString(buffer);
        text.setCharacterSize(24);

        text.setPosition(10, 35 * i);

        context->window->draw(text);
    }

    // @Todo: Spawn Sumo circles
    if (play_state->time_since_last_circle >= 4
        || play_state->circle_list->radius < play_state->min_radius * 0.4)
    {
        if(play_state->circle_list->pattern == Sumo_Circle::Pattern::Random) {
            if (play_state->circle_list && play_state->circle_count <= 3) {
                AddSumoCircle(play_state, play_state->circle_list);
            } else {
                AddSumoCircle(play_state, (2 * play_state->max_radius) + (0.1 * VIEW_WIDTH),
                              (2 * play_state->max_radius) + (0.1 * VIEW_HEIGHT), play_state->max_radius);
            }
            play_state->time_since_last_circle = 0;
        }
        else if(play_state->circle_list->pattern == Sumo_Circle::Pattern::Circle) {
            AddSumoCircle(play_state, play_state->circle_list);
            play_state->time_since_last_circle=3.2f;
        }
    }





    play_state->circle_list->display.setOutlineColor(sf::Color::White);
    if (play_state->circle_list->next)
        play_state->circle_list->next->display.setOutlineColor(sf::Color::White);

    // @Note: Rendering the players
    for (u32 i = 0; i < play_state->player_count; ++i) {
        Player *player = play_state->players + i;
        if (!player->alive) continue;

        // @Todo: Dash Trails seperately
        if (player->is_dashing) {
            player->display.setPosition(player->dash_start);
            player->display.setFillColor(sf::Color::White);
            context->window->draw(player->display);
            u32 dash_iters = 3;
            for (u32 i = 0; i < dash_iters; ++i) {
                sf::CircleShape shape = player->display;
                sf::Vector2f pos = (player->position - player->dash_start);
                pos.x *= (i * (1.0 / 4.0));
                pos.y *= (i * (1.0 / 4.0));

                pos += player->dash_start;
                shape.setPosition(pos);
                shape.setFillColor(sf::Color(0, ((i + 1) * (255 / (dash_iters + 1))), 0));
                context->window->draw(shape);
                player->display.setFillColor(sf::Color::Green);
            }
        }
    }
    // Finally, render entities
    RenderEntities(context, play_state);

    // Post update and render increments
    play_state->time_since_last_circle += input->delta_time;
#endif
}


// @Note: Introduction logo game state
void UpdateRenderLogoState(Game_Context *context, Logo_State *logo) {
    Game_Input *input = context->input;
    context->window->clear();
    logo->rate += logo->delta_rate;
    logo->opacity = Clamp(logo->opacity + input->delta_time * 2.5 * logo->rate, -0.1, 255);
    logo->display.setFillColor(sf::Color(255, 255, 255, logo->opacity));
    context->window->draw(logo->display);

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space) || logo->opacity <= 0) {
        State *old_state = SetState(context, CreateStateFromType(StateType_MainMenu));
        CleanupState(old_state);
    }
    else if (logo->rate > 75) {
        logo->delta_rate = -logo->delta_rate;
    }
}

void UpdateRenderMenuState(Game_Context *context, Menu_State *menu_state) {
    Game_Input *input = context->input;
    Game_Controller *controller = GetGameController(input, 0);
    for (u32 i = 0; i < ArrayCount(controller->buttons); ++i) {
        if (JustButtonPressed(controller->buttons[i])) {
            State *old_state = SetState(context, CreateStateFromType(StateType_Play));
            Play_State *play_state = context->current_state->play_state;

            play_state->min_radius = 320;
            play_state->max_radius = 500;
            AddSumoCircle(play_state, VIEW_WIDTH / 2.0, VIEW_HEIGHT / 2.0, play_state->max_radius);
            CleanupState(old_state);
            return;
        }
    }

    context->window->draw(menu_state->background_shape);
    if (menu_state->text_time > 0.6) {
        menu_state->show_text = !menu_state->show_text;
        menu_state->text_time = 0;
    }

    if (menu_state->show_text) {
        sf::Text text;
        text.setFont(menu_state->display_font);
        text.setCharacterSize(60);
        text.setFillColor(sf::Color(255, 25, 25));
        text.setString("Start Game!");
        text.setStyle(sf::Text::Bold | sf::Text::Underlined);
        text.setPosition(790, 850);

        context->window->draw(text);
    }


    menu_state->text_time += input->delta_time;
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
            UpdateRenderMenuState(context, current->menu_state);
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

