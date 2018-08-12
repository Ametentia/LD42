#include <SFML/Graphics.hpp>

// @Note: Configuration
#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720

#define VIEW_WIDTH 1920
#define VIEW_HEIGHT 1080

#define DELTA (1.0f / 60.0f)

#include "Ludum_Platform.h"
#include "Ludum_Intrinsics.h"
#include "Ludum_Maths.h"

#include "Ludum_States.h"
#include "Ludum_Platform.cpp"

internal void SFMLProcessGameButton(Game_Button *current, Game_Button *prev, bool pressed) {
    current->is_pressed  = pressed;
    current->transitions = (current->is_pressed != prev->is_pressed) ? 1 : 0;
}

int main() {
    srand(time(0));
    sf::Clock clock;

    sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Ludum Dare 42",
            sf::Style::Close | sf::Style::Titlebar);
    window.setFramerateLimit(60);

    sf::View viewport(sf::FloatRect(0, 0, VIEW_WIDTH, VIEW_HEIGHT));
    window.setView(viewport);

    Game_Input input[2] = {};

    Game_Context _context = {};
    Game_Context *context = &_context;
    context->window = &window;

    Game_Input *current = &input[0];
    Game_Input *prev    = &input[1];

    {
        Game_Controller *kbd_current = GetGameController(current, 0);
        Game_Controller *kbd_prev    = GetGameController(prev, 0);

        kbd_current->is_connected  = true;
        kbd_current->is_controller = false;

        kbd_prev->is_connected  = true;
        kbd_prev->is_controller = false;
    }

    PushState(context, CreateStateFromType(StateType_Logo));

    while (window.isOpen()) {
        {
            Game_Controller *kbd_current = GetGameController(current, 0);
            Game_Controller *kbd_prev    = GetGameController(prev, 0);

            ResetButtons(kbd_current);
            for (u32 index = 0; index < ArrayCount(kbd_current->buttons); ++index) {
                kbd_current->buttons[index].is_pressed =
                    kbd_prev->buttons[index].is_pressed;
            }
        }

        sf::Event event;
        while (window.pollEvent(event)) {
            switch (event.type) {
                case sf::Event::Closed: {
                    current->requested_quit = true;
                }
                break;
                case sf::Event::JoystickConnected: {
                    u32 id = event.joystickConnect.joystickId;
                    if (id >= MAX_CONTROLLERS) break;

                    Game_Controller *controller_current = GetGameController(current, id);
                    Game_Controller *controller_prev    = GetGameController(prev, id);

                    controller_current->is_connected = true;
                    controller_prev->is_connected    = true;
                }
                break;
                case sf::Event::JoystickDisconnected: {
                    u32 id = event.joystickConnect.joystickId;
                    if (id >= MAX_CONTROLLERS) break;

                    Game_Controller *controller_current = GetGameController(current, id);
                    Game_Controller *controller_prev    = GetGameController(prev, id);

                    ResetButtons(controller_current);
                    ResetButtons(controller_prev);

                    controller_current->is_connected = false;
                    controller_prev->is_connected    = false;
                }
                break;
                case sf::Event::KeyPressed: {
                    Game_Controller *kbd_current = GetGameController(current, 0);
                    Game_Controller *kbd_prev    = GetGameController(prev, 0);
                    switch (event.key.code) {
                        // Movement
                        case sf::Keyboard::W: {
                            SFMLProcessGameButton(&kbd_current->move_up, &kbd_prev->move_up, true);
                        }
                        break;
                        case sf::Keyboard::S: {
                            SFMLProcessGameButton(&kbd_current->move_down, &kbd_prev->move_down, true);
                        }
                        break;
                        case sf::Keyboard::A: {
                            SFMLProcessGameButton(&kbd_current->move_left, &kbd_prev->move_left, true);
                        }
                        break;
                        case sf::Keyboard::D: {
                            SFMLProcessGameButton(&kbd_current->move_right, &kbd_prev->move_right, true);
                        }
                        break;
                        // Actions
                        case sf::Keyboard::Space: {
                            SFMLProcessGameButton(&kbd_current->action_bottom, &kbd_prev->action_bottom, true);
                        }
                        break;
                        case sf::Keyboard::R: {
                            SFMLProcessGameButton(&kbd_current->action_left, &kbd_prev->action_left, true);
                        }
                        break;
                        case sf::Keyboard::Q: {
                            SFMLProcessGameButton(&kbd_current->action_top, &kbd_prev->action_top, true);
                        }
                        break;
                        case sf::Keyboard::E: {
                            SFMLProcessGameButton(&kbd_current->action_right, &kbd_prev->action_right, true);
                        }
                        break;

                        // Misc
                        case sf::Keyboard::Escape: {
                            SFMLProcessGameButton(&kbd_current->start, &kbd_prev->start, true);
                        }
                        break;
                        case sf::Keyboard::Tab: {
                            SFMLProcessGameButton(&kbd_current->select, &kbd_prev->select, true);
                        }
                        break;
                    }
                }
                break;
                case sf::Event::KeyReleased: {
                    Game_Controller *kbd_current = GetGameController(current, 0);
                    Game_Controller *kbd_prev    = GetGameController(prev, 0);
                    switch (event.key.code) {
                        // Movement
                        case sf::Keyboard::W: {
                            SFMLProcessGameButton(&kbd_current->move_up, &kbd_prev->move_up, false);
                        }
                        break;
                        case sf::Keyboard::S: {
                            SFMLProcessGameButton(&kbd_current->move_down, &kbd_prev->move_down, false);
                        }
                        break;
                        case sf::Keyboard::A: {
                            SFMLProcessGameButton(&kbd_current->move_left, &kbd_prev->move_left, false);
                        }
                        break;
                        case sf::Keyboard::D: {
                            SFMLProcessGameButton(&kbd_current->move_right, &kbd_prev->move_right, false);
                        }
                        break;
                        // Actions
                        case sf::Keyboard::Space: {
                            SFMLProcessGameButton(&kbd_current->action_bottom, &kbd_prev->action_bottom, false);
                        }
                        break;
                        case sf::Keyboard::R: {
                            SFMLProcessGameButton(&kbd_current->action_left, &kbd_prev->action_left, false);
                        }
                        break;
                        case sf::Keyboard::Q: {
                            SFMLProcessGameButton(&kbd_current->action_top, &kbd_prev->action_top, false);
                        }
                        break;
                        case sf::Keyboard::E: {
                            SFMLProcessGameButton(&kbd_current->action_right, &kbd_prev->action_right, false);
                        }
                        break;

                        // Misc
                        case sf::Keyboard::Escape: {
                            SFMLProcessGameButton(&kbd_current->start, &kbd_prev->start, false);
                        }
                        break;
                        case sf::Keyboard::Tab: {
                            SFMLProcessGameButton(&kbd_current->select, &kbd_prev->select, false);
                        }
                        break;
                    }
                }
                break;
            }
        }

        window.clear(sf::Color(231, 191, 120));
        // @Todo: Variable delta time
        current->delta_time = DELTA;
        context->input = current;
        UpdateRenderGame(context);
        window.display();

        if (current->requested_quit) { window.close(); }

        Swap(Game_Input, current, prev);
    }
    return 0;
}

