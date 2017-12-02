#include "titlescreen.h"

lemonade::TitleScreen::TitleScreen():
    m_font(),
    m_descriptionText()
{
    m_font.loadFromFile("ComicNeue_Bold.otf");
    m_descriptionText.setFont(m_font);
    m_descriptionText.setCharacterSize(20);
    m_descriptionText.setOutlineThickness(1.0f);
    m_descriptionText.setString("A Ludum Dare 40 game created by polsys.\nPress ENTER to start!");
    m_descriptionText.setPosition((1280 - m_descriptionText.getLocalBounds().width) * 0.5f, 720 * 0.75f);
}

void lemonade::TitleScreen::update()
{
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Return))
        m_startPressed = true;
}

void lemonade::TitleScreen::draw(sf::RenderTarget& rt)
{
    rt.clear(sf::Color(100, 149, 237, 255));
    rt.draw(m_descriptionText);
}
