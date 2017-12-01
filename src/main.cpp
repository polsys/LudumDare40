#include "SFML/Graphics.hpp"

int main()
{
    sf::RenderWindow window(sf::VideoMode(1280, 720), "LD40");
    // TODO: Timing etc. options

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        // TODO: Update
        window.clear(sf::Color(100, 149, 237, 255));
        // TODO: Draw
        window.display();
    }

    return 0;
}