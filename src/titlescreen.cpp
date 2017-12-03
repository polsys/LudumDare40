#include "highscore.h"
#include "titlescreen.h"
#include "moneytostring.h"

lemonade::TitleScreen::TitleScreen():
    m_font(),
    m_pressEnterText()
{
    m_font.loadFromFile("ComicNeue_Bold.otf");

    m_pressEnterText.setFont(m_font);
    m_pressEnterText.setCharacterSize(24);
    m_pressEnterText.setFillColor(sf::Color(224, 224, 224, 255));
    m_pressEnterText.setString("Press ENTER to start!");
    m_pressEnterText.setPosition((1280 - m_pressEnterText.getLocalBounds().width) * 0.5f, 720 * 0.75f);

    m_copyrightText.setFont(m_font);
    m_copyrightText.setCharacterSize(24);
    m_copyrightText.setString("A Ludum Dare 40 game by polsys");
    m_copyrightText.setPosition(16, 720 - 32);

    m_highScoreText.setFont(m_font);
    m_highScoreText.setCharacterSize(24);
    m_highScoreText.setPosition(16, 50);

    reset();
}

void lemonade::TitleScreen::update()
{
    if (m_framesUntilKeyInputAllowed > 0)
    {
        m_framesUntilKeyInputAllowed--;
    }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Return))
    {
        m_startPressed = true;
    }
}

void lemonade::TitleScreen::draw(sf::RenderTarget& rt)
{
    rt.clear(sf::Color(100, 149, 237, 255));
    rt.draw(m_pressEnterText);
    rt.draw(m_copyrightText);
    rt.draw(m_highScoreText);
}

void lemonade::TitleScreen::reset()
{
    m_startPressed = false;
    m_framesUntilKeyInputAllowed = 20;

    m_highScoreText.setString("High score: " + moneyToString(HighScores::getHighScores().high) +
        "\nLow score: " + moneyToString(HighScores::getHighScores().low));
}
