#include <SFML/Graphics.hpp>

int main(int argc, char **argv) {
	sf::RenderWindow window(sf::VideoMode(1280, 720), "Hello Window", sf::Style::Close);

    sf::CircleShape shape;
    shape.setRadius(5);
    shape.setPosition(100, 100);
    shape.setFillColor(sf::Color::Green);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            switch (event.type) {
                case sf::Event::Closed: {
                    window.close();
                }
                break;
            }
        }

        window.clear(sf::Color(25, 25, 25));
        window.draw(shape);
        window.display();
    }

    return 0;
}
