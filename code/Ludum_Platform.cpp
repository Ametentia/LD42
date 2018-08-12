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
    result->inner.setOrigin(radius - 3 , radius -3);
    result->inner.setPosition(result->position);

    result->inner.setFillColor(sf::Color::Black);
    result->inner.setOutlineColor(sf::Color::Transparent);
    result->inner.setOutlineThickness(-2.0);

    Sumo_Circle *old_head =play_state->circle_list_head;
    result->next = old_head;
    if(old_head) old_head->prev = result;

    result->prev = 0;
    play_state->circle_list_head = result;
    play_state->circle_count++;
}

bool PlayerHitPlayer(Player *a, Player *b) {
#define MOVE_SPEED 350
#define DASH_SPEED 1000
    bool result;
    sf::Vector2f dist_line = b->position - a->position;
    f32 radius_sum = a->radius + b->radius;
    result = Dot(dist_line, dist_line) <= Square(radius_sum);
    return result;
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
    circle->inner.setRadius(circle->radius - 3);
    circle->inner.setOrigin(circle->radius - 3, circle->radius - 3);
    return false;
}

void AddPlayer(Play_State *play_state, f32 x, f32 y) {
    if (play_state->player_count >= MAX_PLAYERS) return;
    Player *player = play_state->players + play_state->player_count++;

    player->position.x = x;
    player->position.y = y;

    player->radius = 20;
    player->display.setRadius(20);
    player->display.setFillColor(sf::Color::Green);
    player->display.setOrigin(20, 20);
}

void UpdatePlayer(Play_State *play_state, Player *player, bool control) {
#define MOVE_SPEED 350
#define DASH_SPEED 1000
    // @Debug: Nowhere near final haha
    f32 speed = MOVE_SPEED;
    if (!player->is_dashing) {
        player->move_direction.x = player->move_direction.y = 0;
        if (control) {
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) { player->move_direction.y -= 1; }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) { player->move_direction.y += 1; }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) { player->move_direction.x -= 1; }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) { player->move_direction.x += 1; }

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space) && !play_state->was_space) {
                player->is_dashing = true;
                player->dash_time = 0.2;
                player->dash_start = player->position;
                speed = DASH_SPEED;
            }
        }
    }
    else {
        player->dash_time -= DELTA;
        player->is_dashing = player->dash_time > 0;
        speed = DASH_SPEED;
    }

    sf::Vector2f velocity = speed * Normalise(player->move_direction);
    player->position += DELTA * velocity;

    play_state->was_space = sf::Keyboard::isKeyPressed(sf::Keyboard::Space);
}

void UpdateRenderLogoState(Game_Context *context, Logo_State *logo) {
    logo->rate += logo->delta_rate;
    logo->opacity = Clamp(logo->opacity + DELTA * 2.5 * logo->rate, -0.1, 255);
    logo->display.setFillColor(sf::Color(255, 255, 255, logo->opacity));
    context->window->draw(logo->display);

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space) || logo->opacity <= 0) {
        State *old_state = SetState(context, CreateStateFromType(StateType_Play));
        Play_State *play_state = context->current_state->play_state;

        play_state->min_radius = 320;
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


    if (play_state->time_since_last_circle >= 4
            || play_state->circle_list_head->radius < play_state->min_radius * 0.4)
    {
        if(play_state->circle_list_head->pattern == Sumo_Circle::Pattern::Random) {
            if (play_state->circle_list_head && play_state->circle_count <= 3) {
                AddSumoCircle(play_state, play_state->circle_list_head);
            } else {
                AddSumoCircle(play_state, (2 * play_state->max_radius) + (0.1 * VIEW_WIDTH),
                              (2 * play_state->max_radius) + (0.1 * VIEW_HEIGHT), play_state->max_radius);
            }
            play_state->time_since_last_circle = 0;
        }
        else if(play_state->circle_list_head->pattern == Sumo_Circle::Pattern::Circle) {
            AddSumoCircle(play_state, play_state->circle_list_head);
            play_state->time_since_last_circle=3.2f;
        }
    }

    // @Note: Rendering the circles
    Sumo_Circle *head = play_state->circle_list_head;
    head->display.setOutlineColor(sf::Color::Yellow);
    if (head->next) head->next->display.setOutlineColor(sf::Color::Magenta);

    // @Todo: Please make this more elegant, I'm tired and can't find a way with less than
    // two variables stored per player...
    for (u32 i = 0; i < play_state->player_count; ++i) {
        if(!play_state->players[i].perma_dead)
            play_state->players[i].frame_alive = false;
    }
    while (head) {
        bool should_delete = UpdateSumoCircle(head);
        if (!should_delete) {
            sf::CircleShape centre(10);
            centre.setPosition(head->position);
            centre.setOrigin(10, 10);
            centre.setFillColor(sf::Color::Red);
            context->window->draw(centre);
            context->window->draw(head->display);
            for (u32 i = 0; i < play_state->player_count; ++i) {
                if (CircleCheck(*head, play_state->players[i]))
                    play_state->players[i].frame_alive = true;
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
    head = play_state->circle_list_head;
    while(head) {
        context->window->draw(head->inner);
        head = head-> next;
    }


    // @Todo: Clear up this check, it finds if the player died, doesn't seem very elegant
    for (u32 i = 0; i < play_state->player_count; ++i) {
        if(!play_state->players[i].frame_alive)
            play_state->players[i].perma_dead = true;
    }

    play_state->circle_list_head->display.setOutlineColor(sf::Color::White);
    if (play_state->circle_list_head->next)
        play_state->circle_list_head->next->display.setOutlineColor(sf::Color::White);

    // @Note: Rendering the players
    for (u32 i = 0; i < play_state->player_count; ++i) {
        Player *player = play_state->players + i;
        for (u32 j = i + 1; j < play_state->player_count; ++j) {
            Player *player_2 = play_state->players + j;
            if (PlayerHitPlayer(player, player_2)) {
                sf::Vector2f impactVector(player_2->position.x - player->position.x, player_2->position.y - player->position.y);
                sf::Vector2f impactVectorNorm = Normalise(impactVector);

                f32 dotImpact1 = Absolute(Dot(player->move_direction, impactVectorNorm));
                f32 dotImpact2 = Absolute(Dot(player_2->move_direction,  impactVectorNorm));

                sf::Vector2f deflect(-impactVectorNorm.x * dotImpact2, -impactVectorNorm.y  * dotImpact2);
                sf::Vector2f deflect2(impactVectorNorm.x * dotImpact1, impactVectorNorm.y  * dotImpact1);

                sf::Vector2f p1Final(player->move_direction.x + deflect.x - deflect2.x, player->move_direction.y + deflect.y - deflect2.y);
                sf::Vector2f p2Final(player_2->move_direction.x + deflect2.x - deflect.x, player_2->move_direction.y + deflect2.y - deflect.y);

                player->move_direction = p1Final;
                player_2->move_direction = p2Final;

                if (player->is_dashing) {
                    // @Fix: Duplicated code
                    player_2->dash_start = player_2->position;
                    player_2->is_dashing = true;
                    player_2->dash_time = player->dash_time;

                    player->is_dashing = false;
                }
                // @Todo: sort out cases, e.g. both players dashing
                else if (player_2->is_dashing) {

                }
            }

        }

        // @Hack: true will control the player
        UpdatePlayer(play_state, play_state->players + i, i == 0);
        if (player->is_dashing && !player->perma_dead) {
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
        if(!player->perma_dead) {
            player->display.setPosition(player->position);
            context->window->draw(player->display);
        }
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

