#include <SFML/Graphics.hpp>

// @Note: Configuration
#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720

#define VIEW_WIDTH 1920
#define VIEW_HEIGHT 1080

#define DELTA (1.0f / 60.0f)

#include "Ludum_Platform.h"
#include "Ludum_States.h"
#include "Ludum_Platform.cpp"

int main() {
    srand(time(0));
    sf::Clock clock;

    sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Ludum Dare 42",
            sf::Style::Close | sf::Style::Titlebar);

    sf::View viewport(sf::FloatRect(0, 0, VIEW_WIDTH, VIEW_HEIGHT));
    window.setView(viewport);

    Game_Context _context = {};
    Game_Context *context = &_context;

    context->window = &window;

    PushState(context, CreateStateFromType(StateType_Logo));

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed || sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
                window.close();
        }

        if (clock.getElapsedTime().asMilliseconds() > DELTA) {
            clock.restart();
            window.clear();
            UpdateRenderGame(context);
            window.display();
        }
    }
    return 0;
}

