#include <SFML/Graphics.hpp>

// @Note: Configuration
#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720

#define VIEW_WIDTH 1920
#define VIEW_HEIGHT 1080

#define DELTA (1.0f / 60.0f)

#include "Ludum_Platform.h"
#include "Ludum_Platform.cpp"

int main() {
    sf::Clock clock;

    sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Ludum Dare 42",
            sf::Style::Close | sf::Style::Titlebar);

    sf::View viewport(sf::FloatRect(0, 0, VIEW_WIDTH, VIEW_HEIGHT));
    window.setView(viewport);

    Game_State _state;
    Game_State *game_state = &_state;

    game_state->window = &window;

    game_state->state_count = 0;
    game_state->play_state.player_count = 0;
    game_state->play_state.was_f = false;
    game_state->logo_state.initialised = false;

    PushState(game_state, StateType_Logo);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed || sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
                window.close();
        }

        if (clock.getElapsedTime().asMilliseconds() > DELTA) {
            clock.restart();
            window.clear();
            UpdateRenderGame(game_state);
            window.display();
        }
    }
    return 0;
}

