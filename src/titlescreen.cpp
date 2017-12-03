#include "highscore.h"
#include "titlescreen.h"
#include "moneytostring.h"

lemonade::TitleScreen::TitleScreen()
{
    m_font.loadFromFile("ComicNeue_Bold.otf");

    m_pressEnterText.setFont(m_font);
    m_pressEnterText.setCharacterSize(32);
    m_pressEnterText.setOutlineThickness(1.0f);
    m_pressEnterText.setFillColor(sf::Color(224, 224, 224, 255));
    m_pressEnterText.setString("Press ENTER to start!");
    m_pressEnterText.setPosition((1280 - m_pressEnterText.getLocalBounds().width) * 0.5f, 720 * 0.75f);

    m_copyrightText.setFont(m_font);
    m_copyrightText.setCharacterSize(24);
    m_copyrightText.setOutlineThickness(1.0f);
    m_copyrightText.setString("A Ludum Dare 40 game by polsys");
    m_copyrightText.setPosition(16, 720 - 32);

    m_highScoreText.setFont(m_font);
    m_highScoreText.setCharacterSize(24);
    m_highScoreText.setPosition(920, 450);

    m_logoTexture.loadFromFile("Logo.png");
    m_logo.setTexture(m_logoTexture);
    m_logo.setPosition({ 256, 0 });

    m_sunnyTexture.loadFromFile("BackgroundSunny.png");
    m_sunnySprite.setTexture(m_sunnyTexture);
    m_sunnySprite.setPosition({ 0, 0 });

    m_rainyTexture.loadFromFile("BackgroundRainy.png");
    m_rainySprite.setTexture(m_rainyTexture);
    m_rainySprite.setPosition({ 0, 0 });

    reset();
}

void lemonade::TitleScreen::update()
{
    m_frame = (m_frame + 1) % 480;
    constexpr float TwoPi = 2 * 3.14159f;
    auto opacity = 4.0f * std::sinf(m_frame / (480.0f) * TwoPi);
    opacity = std::max(std::min(opacity, 1.0f), 0.0f);

    m_rainySprite.setColor(sf::Color{ 255, 255, 255, static_cast<sf::Uint8>(opacity * 255) });

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
    rt.clear();

    rt.draw(m_sunnySprite);
    rt.draw(m_rainySprite);

    rt.draw(m_logo);

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
