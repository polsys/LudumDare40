#include <memory>
#include "SFML/Graphics.hpp"
#include "game.h"
#include "titlescreen.h"

int main()
{
    sf::RenderWindow window(sf::VideoMode(1280, 720), "(Cursed) Lemonade Stand", sf::Style::Titlebar | sf::Style::Close);
    window.setFramerateLimit(60);
    window.setMouseCursorVisible(false); // Mouse is not used in this game

    // The title screen always exists, whereas the gameplay screen is re-created every time the game is started
    lemonade::TitleScreen title;
    std::unique_ptr<lemonade::Game> game;

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        if (!game)
        {
            title.update();
            title.draw(window);
            if (title.shouldStartGame())
                game.reset(new lemonade::Game);
        }
        else
        {
            game->update();
            game->draw(window);
            if (game->isGameOver())
                game.reset(nullptr);
        }
        window.display();
    }

    return 0;
}