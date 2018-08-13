// Will create a new circle at the given (x, y) position and the given radius
void AddSumoCircle(Play_State *play_state, f32 x, f32 y, f32 radius, f32 shrink_delta = RandomFloat(-90, -15)) {
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

    if(RandomInt(0, 5) == 1) {
        result->pattern = Sumo_Circle::Pattern::Circle;
    }

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
    f32 radius = RandomFloat(play_state->min_radius, play_state->max_radius*0.5f);
    f32 x = base->position.x + radius * RandomFloat(0.8, 1.4) * Sin(angle);
    x = Clamp(x, radius, VIEW_WIDTH - (radius / 2.0));
    f32 y = base->position.y + radius * Cos(angle);
    y = Clamp(y, radius, VIEW_HEIGHT - (radius / 2.0));

#else
    f32 x = RandomFloat(0.2 * VIEW_WIDTH, VIEW_WIDTH - (0.2 * VIEW_WIDTH));
    f32 y = RandomFloat(0.2 * VIEW_HEIGHT, VIEW_HEIGHT - (0.2 * VIEW_HEIGHT));

    sf::Vector2f diff = base->position - sf::Vector2f(x, y);
    f32 radius = Max(Length(diff), play_state->min_radius);
#endif

    AddSumoCircle(play_state, x, y, radius, RandomFloat(-50, -15));
}
void AddBot(Play_State *play_state, Player_Type type, f32 x, f32 y) {
    if (play_state->bot_count >= MAX_BOTS) return;
    Player *player = play_state->bots + play_state->bot_count++;

    player->alive = true;
    player->type = type;
    player->position = { x, y };
    player->radius = 20;
    player->display.setRadius(20);
    player->brain.action = RandomInt(0,1);

    player->display.setTexture(play_state->player_textures + type);

    switch (player->type) {
        case PlayerType_SumoCat: {
            player->push_strength = 100;
            player->move_speed = 250;
            player->dash_length = 0.1;
            player->charge_needed = 8;
        }
            break;
        case PlayerType_LuchadorCat: {
            player->move_speed = 250;
            player->push_strength = 60;
            player->dash_length = 0.2;
            player->charge_needed = 8;
        }
            break;
        case PlayerType_AstroCat: {
            player->move_speed = 450;
            player->push_strength = 10;
            player->dash_length = 0.4;
            player->charge_needed = 6;
        }
            break;
        case PlayerType_DevilCat: {
            player->move_speed = 350;
            player->push_strength = 20;
            player->dash_length = 0.2;
            player->charge_needed = 15;
        }
            break;
    }
    player->display.setOrigin(20, 20);
    player->is_bot = true;
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
            // @TODO: @James this delete seemed to cause me a crash, though i fear memory leaks without it
            //delete current;
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
        current->shrink_delta+=Clamp(20*delta_time, -90, 70);

        current->should_delete = current->radius <= 10;
        current = current->next;
    }
}

void AddPlayer(Play_State *play_state, Player_Type type, f32 x, f32 y) {
    if (play_state->player_count >= MAX_PLAYERS) return;
    Player *player = play_state->players + play_state->player_count++;

    player->alive = true;
    player->type = type;
    player->position = { x, y };
    player->radius = 20;
    player->display.setRadius(20);

    player->display.setTexture(play_state->player_textures + type);
    switch (player->type) {
        case PlayerType_SumoCat: {
            player->push_strength = 100;
            player->move_speed = 250;
            player->dash_length = 0.1;
            player->charge_needed = 8;
        }
        break;
        case PlayerType_LuchadorCat: {
            player->move_speed = 250;
            player->push_strength = 60;
            player->dash_length = 0.2;
            player->charge_needed = 8;
        }
        break;
        case PlayerType_AstroCat: {
            player->move_speed = 450;
            player->push_strength = 10;
            player->dash_length = 0.4;
            player->charge_needed = 6;
        }
        break;
        case PlayerType_DevilCat: {
            player->move_speed = 350;
            player->push_strength = 20;
            player->dash_length = 0.2;
            player->charge_needed = 15;
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

void UpdatePlayer(Player *player, Game_Controller *controller, f32 delta_time, Play_State *play_state) {
    f32 speed = player->move_speed;
    bool has_input = player->is_dashing;
    if(player->timeLeft < 0)
        player->chargeTimeup += delta_time;
    player->timeLeft -= delta_time;
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

        }
        if(JustButtonPressed(controller->action_top) && player->chargeTimeup > player->charge_needed) {
            switch(player->type) {
                case PlayerType_AstroCat:
                    {
                        f32 min = 1000000;
                        sf::Vector2f pos;
                        s32 index = -1;
                        for (int i = 0; i < play_state->bot_count; i++) {
                            f32 len = Length(player->position - (play_state->bots + i)->position);
                            if ((play_state->bots + i)->alive && len < min) {
                                pos = (play_state->bots + i)->position;
                                min = len;
                                index = i;
                            }
                        }
                        (play_state->bots +index)->position = player->position;
                        player->position = pos;
                        player->chargeTimeup = 0;
                    }
                    break;
                case PlayerType_DevilCat:
                    {
                        player->timeLeft = 2.5f;
                        f32 min = 1000000;
                        sf::Vector2f pos;
                        s32 index = -1;
                        for (int i = 0; i < play_state->bot_count; i++) {
                            f32 len = Length(player->position - (play_state->bots + i)->position);
                            if ((play_state->bots + i)->alive && len < min) {
                                min = len;
                                index = i;
                            }
                        }
                        (play_state->bots +index)->reversed_controls = true;
                        player->chargeTimeup = 0;
                    }
                break;
                case PlayerType_LuchadorCat:
                    {
                        player->radius = 40;
                        player->display.setRadius(40);
                        player->timeLeft = 5.0f;
                        player->chargeTimeup = 0;
                    }
                break;
                case PlayerType_SumoCat:
                    {
                        player->timeLeft = 5.0;
                        player->chargeTimeup = 0;
                    }
                break;
            }
        }
        if (JustButtonPressed(controller->action_bottom)) { speed = DASH_SPEED;
            DashPlayer(player, player->dash_length);
        }
        move_direction = Normalise(move_direction);
        player->move_direction = move_direction;
    }
    if(player->timeLeft < 0) {
        switch(player->type) {
            case PlayerType_LuchadorCat:
            {
                player->radius = 20;
                player->display.setRadius(20);
            }
            case PlayerType_DevilCat:
                for(int i = 0; i < play_state->player_count; i++) {
                    (play_state->players+i)->reversed_controls = false;
                }
                for(int i = 0; i < play_state->bot_count; i++) {
                    (play_state->bots+i)->reversed_controls = false;
                }
            break;
        }
    }

    player->timeLeft -= delta_time;

    player->position += speed * delta_time * player->move_direction;
    if (!has_input) { player->move_direction = {}; }

    player->score_time += delta_time;
    if (player->score_time >= 1) {
        player->score += player->hit_count;
        player->score_time = 0;
    }
}

bool CheckBounds(Sumo_Circle *circle_list, sf::Vector2f pos) {
    if( circle_list) {
        Sumo_Circle *circle = circle_list;
        while(circle) {
            if (SumoCircleHitPos(circle, pos)) return true;
            circle = circle -> next;
        }
    }
    return false;
}
void UpdateBot(Player *bot, f32 delta_time, Play_State *play_state) {
    bot->brain.timer += delta_time;

    if(bot->brain.timer > bot->brain.planExecute) {
        switch(bot->brain.action) {
            case 0:
            {
                f32 min = 1000000;
                for (int i = 0; i < play_state->player_count; i++) {
                    f32 len = Length(bot->position - (play_state->players + i)->position);
                    if ((play_state->players + i)->alive && len < min &&
                        CheckBounds(play_state->circle_list, (play_state->players + i)->position)) {
                        bot->brain.target = (play_state->players + i)->position;
                        min = len;
                        bot->brain.targetNum = i;
                    }
                }
                for (int i = 0; i < play_state->bot_count; i++) {
                    if ((play_state->bots + i)->position != bot->position) {
                        f32 len = Length(bot->position - (play_state->bots + i)->position);
                        if ((play_state->bots + i)->alive && len < min &&
                            CheckBounds(play_state->circle_list, (play_state->bots + i)->position)) {
                            bot->brain.target = (play_state->bots + i)->position;
                            min = len;
                            bot->brain.targetNum = i + MAX_PLAYERS;
                        }
                    }
                }
                if(bot->brain.targetNum >= 0)
                    bot->brain.action = 2;
                else
                    bot->brain.action = 1;
                if(bot->reversed_controls) {
                    bot->brain.action = 1;
                }
                break;
            }
            case 1:
            {
                bot->brain.target = play_state->circle_list->position;
                f32 range = play_state->circle_list->radius * 0.7;
                bot->brain.target.x += RandomFloat(-range, range);
                bot->brain.target.y += RandomFloat(-range, range);
                bot->brain.action = 0;
                if(bot->reversed_controls) {
                    bot->brain.action = 1;
                }
                break;
            }
            case 2:
            {
                u8 targetI = bot->brain.targetNum;
                if(bot->brain.targetNum > MAX_PLAYERS) {
                    targetI = bot->brain.targetNum-MAX_PLAYERS;
                    bot->brain.target = (play_state->bots+targetI)->position;
                    sf::Vector2f diff = bot->position - (play_state->bots+targetI)->position;
                    f32 len_sq = Dot(diff, diff);
                    if(len_sq < 400.0*400.0 &&(play_state->bots+targetI)-> alive) {
                        bot->brain.wants_dodge = true;
                    }
                }
                else {
                    bot->brain.target = (play_state->players+targetI)->position;
                    sf::Vector2f diff = bot->position - (play_state->players+targetI)->position;
                    f32 len_sq = Dot(diff, diff);
                    if(len_sq < 400.0*400.0 && (play_state->players+targetI)-> alive) {
                        bot->brain.wants_dodge = true;
                    }
                }
                bot->brain.action = 1;
                if(bot->reversed_controls) {
                    bot->brain.action = 0;
                }
            }
            break;
        }
        bot->brain.timer = RandomFloat(-0.6, 0);
    }


    f32 speed = bot->move_speed;
    if(bot->timeLeft < 0) {
        bot->chargeTimeup+=delta_time;
    }
    bot->timeLeft-=delta_time;
    bool has_input = bot->is_dashing;
    if (bot->is_dashing) {
        bot->dash_time -= delta_time;
        bot->is_dashing = bot->dash_time > 0;
        speed = DASH_SPEED;
    } else {
        if (bot->chargeTimeup > bot->charge_needed && RandomFloat(0, 100) > 90) {
            switch (bot->type) {
                case PlayerType_AstroCat: {
                    f32 min = 1000000;
                    sf::Vector2f pos;
                    s32 index = -1;
                    for (int i = 0; i < play_state->bot_count; i++) {
                        f32 len = Length(bot->position - (play_state->bots + i)->position);
                        if ((play_state->bots + i)->alive && len < min) {
                            pos = (play_state->bots + i)->position;
                            min = len;
                            index = i;
                        }
                    }
                    f32 len = Length(bot->position - (play_state->players)->position);
                    if ((play_state->players)->alive && len < min) {
                        pos = (play_state->players)->position;
                        min = len;
                        (play_state->bots + index)->position = bot->position;
                        bot->position = pos;
                        bot->chargeTimeup = 0;
                    } else {
                        (play_state->bots + index)->position = bot->position;
                        bot->position = pos;
                        bot->chargeTimeup = 0;
                    }
                }
                    break;
                case PlayerType_DevilCat: {
                    bot->timeLeft = 2.5f;
                    f32 min = 1000000;
                    sf::Vector2f pos;
                    s32 index = -1;
                    for (int i = 0; i < play_state->bot_count; i++) {
                        f32 len = Length(bot->position - (play_state->bots + i)->position);
                        if ((play_state->bots + i)->alive && len < min) {
                            min = len;
                            index = i;
                        }
                    }
                    f32 len = Length(bot->position - (play_state->players)->position);
                    if ((play_state->players)->alive && len < min) {
                        play_state->players->reversed_controls = true;
                        bot->chargeTimeup = 0;
                    } else {
                        (play_state->bots + index)->reversed_controls = true;
                        bot->chargeTimeup = 0;
                    }
                }
                    break;
                case PlayerType_LuchadorCat: {
                    bot->radius = 40;
                    bot->display.setRadius(40);
                    bot->timeLeft = 5.0f;
                    bot->chargeTimeup = 0;
                }
                    break;
                case PlayerType_SumoCat: {
                    bot->timeLeft = 5.0;
                    bot->chargeTimeup = 0;
                }
            }
        }
        sf::Vector2f moveDirection(0, 0);
        if(bot->timeLeft < 0) {
            switch(bot->type) {
                case PlayerType_LuchadorCat:
                {
                    bot->radius = 20;
                    bot->display.setRadius(20);
                }
                case PlayerType_DevilCat:
                    for(int i = 0; i < play_state->player_count; i++) {
                        (play_state->players+i)->reversed_controls = false;
                    }
                    for(int i = 0; i < play_state->bot_count; i++) {
                        (play_state->bots+i)->reversed_controls = false;
                    }
                    break;
            }
        }
        sf::Vector2f dist_line = bot->brain.target - bot->position;
        if (Dot(dist_line, dist_line) > 10) {
            moveDirection = -Normalise(bot->position - bot->brain.target);
        }
        if (bot->reversed_controls) {
            moveDirection.x = -moveDirection.x;
            moveDirection.y = -moveDirection.y;
            bot->brain.planExecute = 0.15f;
        } else
            bot->brain.planExecute = 0.4f;
        bot->move_direction = moveDirection;
        if (bot->brain.wants_dodge) {
            DashPlayer(bot, bot->dash_length);
            bot->brain.wants_dodge = false;
        }
    }

    bot->position += speed * delta_time * bot->move_direction;
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

    if(a->type == PlayerType_SumoCat && a->timeLeft > 0) {
        b->move_direction += ((deflect_b - deflect_a) * 2.0f);
    }
    else if(b->type == PlayerType_SumoCat && b->timeLeft > 0) {
        a->move_direction += ((deflect_a - deflect_b) * 2.0f);
    }
    else {
        a->move_direction += (deflect_a - deflect_b) * 1.3f;
        b->move_direction += (deflect_b - deflect_a) * 1.3f;
    }


    if (a->is_dashing && b->is_dashing) {
        a->dash_time = 0.05f;
        b->dash_time = 0.05f;
    }
    else if (a->is_dashing) {
        f32 push_factor = a->dash_time;
        a->is_dashing = b->type == PlayerType_SumoCat && b->timeLeft > 0;
        DashPlayer(b, push_factor);
    }
    else if (b->is_dashing) {
        f32 push_factor = b->dash_time;
        b->is_dashing = a->type == PlayerType_SumoCat && a->timeLeft > 0;
        DashPlayer(a, push_factor);
    }

}

void CheckCollisions(Play_State *play_state) {
    // Every Player
    for (u32 i = 0; i < play_state->player_count; ++i) {
        Player *a = play_state->players + i;
        // Against Every Player
        for (u32 j = i + 1; j < play_state->player_count; ++j) {
            Player *b = play_state->players + j;
            if (PlayerHitPlayer(a, b) && a->alive && b->alive) { ResolveCollision(a, b); }
        }

        // Against Every Bot
        for (u32 j = 0; j < play_state->bot_count; ++j) {
            Player *b = play_state->bots + j;
            if (PlayerHitPlayer(a, b) && a->alive && b->alive) { ResolveCollision(a, b); }
        }
    }

    // Every Bot
    for (u32 i = 0; i < play_state->bot_count; ++i) {
        Player *a = play_state->bots + i;
        // Against Every Bot
        for (u32 j = i + 1; j < play_state->bot_count; ++j) {
            Player *b = play_state->bots + j;
            if (PlayerHitPlayer(a, b) && a->alive && b->alive) { ResolveCollision(a, b); }
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
        Player *bot = play_state->bots+ i;
        if (bot->alive) {
            bot->display.setPosition(bot->position);
            context->window->draw(bot->display);
        }
    }
}

void UpdateRenderPlayState(Game_Context *context, Play_State *play_state) {
    Game_Input *input = context->input;

    // @Debug: Adding Players and quit on escape
#if 0
    {
        Game_Controller *controller = GetGameController(input, 0);
        if (JustButtonPressed(controller->action_right)) {
            AddPlayer(play_state, PlayerType_LuchadorCat,
                    RandomFloat(VIEW_WIDTH / 2.0 - play_state->min_radius,
                        VIEW_WIDTH / 2.0 + play_state->min_radius), RandomFloat(VIEW_HEIGHT / 2.0
                            - play_state->min_radius, VIEW_HEIGHT / 2.0 + play_state->min_radius));
        }
        if (JustButtonPressed(controller->action_right)) {
            AddBot(play_state, PlayerType_SumoCat,
                   RandomFloat(VIEW_WIDTH / 2.0 - play_state->min_radius,
                               VIEW_WIDTH / 2.0 + play_state->min_radius), RandomFloat(VIEW_HEIGHT / 2.0
                                                                                       - play_state->min_radius,
                                                                                       VIEW_HEIGHT / 2.0 +
                                                                                       play_state->min_radius));
            AddBot(play_state, PlayerType_SumoCat,
                   RandomFloat(VIEW_WIDTH / 2.0 - play_state->min_radius,
                               VIEW_WIDTH / 2.0 + play_state->min_radius), RandomFloat(VIEW_HEIGHT / 2.0
                                                                                       - play_state->min_radius,
                                                                                       VIEW_HEIGHT / 2.0 +
                                                                                       play_state->min_radius));
            AddBot(play_state, PlayerType_SumoCat,
                   RandomFloat(VIEW_WIDTH / 2.0 - play_state->min_radius,
                               VIEW_WIDTH / 2.0 + play_state->min_radius), RandomFloat(VIEW_HEIGHT / 2.0
                                                                                       - play_state->min_radius,
                                                                                       VIEW_HEIGHT / 2.0 +
                                                                                       play_state->min_radius));
            AddBot(play_state, PlayerType_SumoCat,
                   RandomFloat(VIEW_WIDTH / 2.0 - play_state->min_radius,
                               VIEW_WIDTH / 2.0 + play_state->min_radius), RandomFloat(VIEW_HEIGHT / 2.0
                                                                                       - play_state->min_radius,
                                                                                       VIEW_HEIGHT / 2.0 +
                                                                                       play_state->min_radius));
        }

        if (JustButtonPressed(controller->select)) {
            Player *player = play_state->players;
            player->reversed_controls = !player->reversed_controls;
        }

        if (IsButtonPressed(controller->start)) input->requested_quit = true;
    }
#endif


    // Check for any collisions
    CheckCollisions(play_state);

    // Update Players based off input
    for (u32 i = 0; i < play_state->player_count; ++i) {
        Player *player = play_state->players + i;
        Game_Controller *controller = GetGameController(input, i);
        if (player->alive) {
            UpdatePlayer(player, controller, input->delta_time, play_state);
            CheckBounds(play_state->circle_list, player);
        }
    }

    // Update any AI entities involved
    for (u32 i = 0; i < play_state->bot_count; ++i) {
        Player *bot = play_state->bots + i;
        if (bot->alive) {
            UpdateBot(bot, input->delta_time, play_state);
            CheckBounds(play_state->circle_list, bot);
        }
    }

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
        snprintf(buffer, 256, "Player [%d] Score: %d\n Bot Count %d\n", i + 1, player->score, play_state->bot_count);
        sf::Text text;
        text.setFont(play_state->display_font);
        text.setString(buffer);
        text.setCharacterSize(24);

        text.setPosition(10, 35 * i);

        context->window->draw(text);
    }


    bool game_over = !play_state->bots[0].alive;
    for (u32 i = 1; i < play_state->bot_count; i++) {
        game_over &= !play_state->bots[i].alive;
    }

    game_over = game_over | !play_state->players[0].alive;

    if (game_over) {
        State *old_state = SetState(context, CreateStateFromType(StateType_GameOver));
        Game_Over_State *state = context->current_state->game_over_state;

        state->won = play_state->players[0].alive;
        if (state->won) {
            state->winner = play_state->players[0].type;
        }
        else {
            state->winner = play_state->bots[RandomInt(0, play_state->bot_count)].type;
        }
        state->scores[0] = { play_state->players[0].score, play_state->players[0].type };
        for (u32 i = 0; i < play_state->bot_count; ++i) {
            state->scores[i + 1] = { play_state->bots[i].score, play_state->bots[i].type };
        }

        CleanupState(old_state);
        return;
    }

    // @Todo: Spawn Sumo circles
    if (play_state->time_since_last_circle >= 4
        || play_state->circle_list->shrink_delta > 50)
    {
        if(play_state->circle_list->pattern == Sumo_Circle::Pattern::Random) {
            AddSumoCircle(play_state, play_state->circle_list);
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

        if (player->is_dashing) {
            sf::Color norm = sf::Color::White;
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
                shape.setFillColor(sf::Color(((i+1)*(norm.r/(dash_iters+1))),(i+1)*(norm.g/(dash_iters+1)), (i+1)*(norm.b/(dash_iters+1))));
                context->window->draw(shape);
                player->display.setFillColor(norm);
            }
        }
    }
    for(u32 i = 0; i < play_state->bot_count; ++i) {
        Player *player = play_state->bots + i;
        if (!player->alive) continue;
        if (player->is_dashing) {
            sf::Color norm = player->display.getFillColor();
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
                shape.setFillColor(sf::Color(((i+1)*(norm.r/(dash_iters+1))),(i+1)*(norm.g/(dash_iters+1)), (i+1)*(norm.b/(dash_iters+1))));
                context->window->draw(shape);
                player->display.setFillColor(norm);
            }
        }
    }
    // Finally, render entities
    RenderEntities(context, play_state);

    for (u32 i = 0; i < play_state->player_count; ++i) {
        Player *player = play_state->players + i;
        if (player->chargeTimeup > player->charge_needed) {
            // @Todo
            play_state->charge_bars[i].setTexture(play_state->ui_elements + player->type + 4);
        }
        else {
            play_state->charge_bars[i].setTexture(play_state->ui_elements + player->type);
            f32 width = ((player->chargeTimeup / player->charge_needed)) * (VIEW_WIDTH / 4.0);
            sf::RectangleShape shape;
            shape.setSize(sf::Vector2f(width, 100));
            shape.setPosition(20 + i * (VIEW_WIDTH / 4.0), VIEW_HEIGHT - 100);
            shape.setFillColor(sf::Color(255, 102, 0));

            context->window->draw(shape);

        }

        context->window->draw(play_state->charge_bars[i]);
    }

    f32 offset = (play_state->player_count * (VIEW_WIDTH / 4.0));
    for (u32 i = 0; i < play_state->bot_count; ++i) {
        Player *bot = play_state->bots + i;
        if (bot->chargeTimeup >= bot->charge_needed) {
            // @Todo
            play_state->charge_bars[i + play_state->player_count].setTexture(play_state->ui_elements + bot->type + 4);
        }
        else {
            play_state->charge_bars[i + play_state->player_count].setTexture(play_state->ui_elements + bot->type);
            f32 width = ((bot->chargeTimeup / bot->charge_needed)) * (VIEW_WIDTH / 4.0);
            sf::RectangleShape shape;
            shape.setSize(sf::Vector2f(width, 100));
            shape.setPosition(20 + offset + i * (VIEW_WIDTH / 4.0), VIEW_HEIGHT - 100);
            shape.setFillColor(sf::Color(255, 102, 0));

            context->window->draw(shape);

        }

        context->window->draw(play_state->charge_bars[i + play_state->player_count]);
    }

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

void UpdateRenderCharacterSelect(Game_Context *context, Character_Select_State *character_select) {
    Game_Controller *controller = GetGameController(context->input, 0);
    if (!character_select->ready && JustButtonPressed(controller->move_left)) {
        character_select->index = character_select->index - 1 < 0 ? 3 : character_select->index - 1;
    }
    else if (!character_select->ready && JustButtonPressed(controller->move_right)) {
        character_select->index = character_select->index + 1 >= 4 ? 0 : character_select->index + 1;
    }

    if (JustButtonPressed(controller->action_bottom) && character_select->ready) {
        Player_Type player_type = cast(Player_Type) character_select->index;

        State *old_state = SetState(context, CreateStateFromType(StateType_Play));
        Play_State *play_state = context->current_state->play_state;

        play_state->min_radius = 320;
        play_state->max_radius = 500;
        AddSumoCircle(play_state, VIEW_WIDTH / 2.0, VIEW_HEIGHT / 2.0, play_state->max_radius);

        sf::Vector2f half_sceen = { VIEW_WIDTH / 2.0, VIEW_HEIGHT / 2.0 };
        sf::Vector2f positions[4] = {
            { half_sceen.x - play_state->min_radius, half_sceen.y - play_state->min_radius },
            { half_sceen.x + play_state->min_radius, half_sceen.y - play_state->min_radius },
            { half_sceen.x + play_state->min_radius, half_sceen.y + play_state->min_radius },
            { half_sceen.x - play_state->min_radius, half_sceen.y + play_state->min_radius }
        };

        sf::Vector2f position = positions[player_type];
        AddPlayer(play_state, player_type, position.x, position.y);
        for (u32 i = 0; i < 4; ++i) {
            Player_Type type = cast(Player_Type) i;
            if (type == player_type) continue;

            position = positions[i];
            AddBot(play_state, type, position.x, position.y);
        }

        character_select->music.stop();
        play_state->music.setVolume(20);
        play_state->music.play();

        CleanupState(old_state);
        return;
    }
    else if (JustButtonPressed(controller->action_bottom)) {
        character_select->ready = true;
    }
    else if (JustButtonPressed(controller->start)) {
        character_select->ready = false;
    }

    f32 sin = Sin(character_select->scale_offset);
    f32 cos = Cos(character_select->scale_offset);

    s32 index = character_select->index;
    sf::RectangleShape *shape = character_select->character_shapes + index;

    shape->setTexture(character_select->characters + index);

    sf::Vector2f position = shape->getPosition();
    sf::Vector2f size = shape->getSize();

    //shape->setPosition(position.x + (sin / 2.0), position.y + (sin / 2.0));
    //shape->setSize(sf::Vector2f(size.x - sin, size.y - sin));
    context->window->draw(character_select->character_shapes[index]);

    for (u32 i = 0; i < 4; ++i) {
        if (i == character_select->index) continue;
        //character_select->character_shapes[i].setSize(sf::Vector2f(VIEW_WIDTH / 4.0, 0.6 * VIEW_HEIGHT));
        //character_select->character_shapes[i].setPosition(i * (VIEW_WIDTH / 4.0), 40);
        character_select->character_shapes[i].setTexture(character_select->characters_gray + i);
        context->window->draw(character_select->character_shapes[i]);
    }

    if (character_select->ready) {
        character_select->border_shape.setTexture(&character_select->ready_texture);
        context->window->draw(character_select->border_shape);
    }
    else {
        character_select->border_shape.setTexture(&character_select->border);
        context->window->draw(character_select->border_shape);
    }

    character_select->scale_offset += 1.8 * context->input->delta_time;
}

void UpdateRenderMenuState(Game_Context *context, Menu_State *menu_state) {
    Game_Input *input = context->input;
    Game_Controller *controller = GetGameController(input, 0);
    for (u32 i = 0; i < ArrayCount(controller->buttons); ++i) {
        if (JustButtonPressed(controller->buttons[i])) {
            State *old_state = SetState(context, CreateStateFromType(StateType_CharacterSelect));
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
        context->window->draw(menu_state->start_shape);
    }


    menu_state->text_time += input->delta_time;
}

void UpdateRenderGameOver(Game_Context *context, Game_Over_State *game_over) {

    Game_Controller *controller = GetGameController(context->input, 0);
    if (JustButtonPressed(controller->action_bottom)) {
        Player_Type player_type = game_over->scores[0].type;

        State *old_state = SetState(context, CreateStateFromType(StateType_Play));
        Play_State *play_state = context->current_state->play_state;
        play_state->music.setVolume(20);
        play_state->music.play();

        play_state->min_radius = 320;
        play_state->max_radius = 500;
        AddSumoCircle(play_state, VIEW_WIDTH / 2.0, VIEW_HEIGHT / 2.0, play_state->max_radius);

        sf::Vector2f half_sceen = { VIEW_WIDTH / 2.0, VIEW_HEIGHT / 2.0 };
        sf::Vector2f positions[4] = {
            { half_sceen.x - play_state->min_radius, half_sceen.y - play_state->min_radius },
            { half_sceen.x + play_state->min_radius, half_sceen.y - play_state->min_radius },
            { half_sceen.x + play_state->min_radius, half_sceen.y + play_state->min_radius },
            { half_sceen.x - play_state->min_radius, half_sceen.y + play_state->min_radius }
        };

        sf::Vector2f position = positions[player_type];
        AddPlayer(play_state, player_type, position.x, position.y);
        for (u32 i = 0; i < 4; ++i) {
            Player_Type type = cast(Player_Type) i;
            if (type == player_type) continue;

            position = positions[i];
            AddBot(play_state, type, position.x, position.y);
        }

        CleanupState(old_state);
        return;
    }
    else if (JustButtonPressed(controller->action_left)) {
        State *old_state = SetState(context, CreateStateFromType(StateType_CharacterSelect));
        CleanupState(old_state);
        return;
    }

    if (game_over->won) {
        Score *score = game_over->scores;
        sf::Text text;
        text.setFont(game_over->display_font);
        char buf[256];
        snprintf(buf, 256, "You Won!\nScore: %d", score->score);
        text.setCharacterSize(140);
        text.setPosition(80, VIEW_HEIGHT / 4);
        text.setString(buf);
        text.setFillColor(sf::Color::Green);

        context->window->draw(text);
    }
    else {
        Score *score = game_over->scores;
        sf::Text text;
        text.setFont(game_over->display_font);
        char buf[256];
        snprintf(buf, 256, "You Lost!\nScore: %d", score->score);
        text.setString(buf);
        text.setFillColor(sf::Color::Red);
        text.setCharacterSize(140);
        text.setPosition(80, VIEW_HEIGHT / 4);

        context->window->draw(text);
    }

    sf::Texture *texture = game_over->char_display + game_over->winner;

    sf::RectangleShape shape;
    shape.setPosition(VIEW_WIDTH - texture->getSize().x / 2, VIEW_HEIGHT - texture->getSize().y / 2);
    shape.setSize(sf::Vector2f(texture->getSize().x / 2, texture->getSize().y / 2));
    shape.setTexture(texture);

    context->window->draw(shape);

    sf::Text text;
    text.setFont(game_over->display_font);
    text.setString("Press 'Space' to restart");
    text.setPosition(80, VIEW_HEIGHT - 300);
    text.setCharacterSize(60);
    text.setFillColor(sf::Color::Black);

    context->window->draw(text);

    text.setString("Press 'R' to go back to the\ncharacter select screen");
    text.setPosition(80, VIEW_HEIGHT - 160);
    context->window->draw(text);
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
        case StateType_CharacterSelect: {
            UpdateRenderCharacterSelect(context, current->character_state);
        }
        break;
        case StateType_GameOver: {
            UpdateRenderGameOver(context, current->game_over_state);
        }
        break;
    }
}

