#include <SFML/Graphics.hpp>

float min(float a, float b) {
    if(a > b) {
        return b;
    }
    return a;
}

int main()
{
    sf::Clock clock;

    sf::RenderWindow window(sf::VideoMode(1920/2, 1080/2), "LD42! :D Lets go!", !sf::Style::Resize);
    sf::View viewport(sf::FloatRect(0,0, 1920, 1080));
    window.setView(viewport);
    sf::RectangleShape logo;
    logo.setPosition(1920/2 - 1080/2, 0);
    logo.setSize(sf::Vector2f(1080,1080));
    sf::Texture ametentia;
    ametentia.loadFromFile("logo.png");
    logo.setFillColor(sf::Color(255,255,255, 0));
    logo.setTexture(&ametentia);
    float opacity = 0;
    float rate = 1;

    while (window.isOpen())
    {
        if(clock.getElapsedTime().asMilliseconds() > (float)1/60) {
            clock.restart();
            window.clear();
            window.draw(logo);
            window.display();
            rate += 0.02f;
            opacity = min(opacity + (float)1/60 * (float)2.5 * rate , 255);
            logo.setFillColor(sf::Color(255,255,255,opacity));
            sf::Event event;
            while (window.pollEvent(event))
            {
                if (event.type == sf::Event::Closed || sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
                    window.close();
            }
        }
    }
    return 0;
}

