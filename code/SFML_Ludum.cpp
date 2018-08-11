#include <SFML/Graphics.hpp>

#include "Ludum_Platform.h"

// @Note: Configuration
#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720

#define VIEW_WIDTH 1920
#define VIEW_HEIGHT 1080

#define DELTA (1.0f / 60.0f)

int main() {
    sf::Clock clock;

    sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Ludum Dare 42",
            sf::Style::Close | sf::Style::Titlebar);

    sf::View viewport(sf::FloatRect(0, 0, VIEW_WIDTH, VIEW_HEIGHT));
    window.setView(viewport);

    // @Note: Logo

    sf::RectangleShape logo;
    logo.setPosition(WINDOW_WIDTH - WINDOW_HEIGHT, 0);
    logo.setSize(sf::Vector2f(VIEW_HEIGHT, VIEW_HEIGHT));

    sf::Texture ametentia;
    ametentia.loadFromFile("logo.png");
    logo.setFillColor(sf::Color(0, 0, 0, 0));
    logo.setTexture(&ametentia);

    f32 opacity = 0;
    f32 rate = 1;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed || sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
                window.close();
        }

        if (clock.getElapsedTime().asMilliseconds() > DELTA) {
            clock.restart();
            window.clear();

            rate += 0.02f;
            opacity = Min(opacity + DELTA * 2.5f * rate , 255);
            logo.setFillColor(sf::Color(255, 255, 255, opacity));
            window.draw(logo);

            window.display();
        }
    }
    return 0;
}

