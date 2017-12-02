#pragma once

#include "SFML/Graphics.hpp"

namespace lemonade
{
    // This class shows the title screen and waits for the user to start playing.
    class TitleScreen : sf::NonCopyable
    {
    public:
        // This loads all graphical resources needed.
        TitleScreen();

        // Handles input, etc.
        void update();

        // Clears the rendertarget and draws the title screen there.
        void draw(sf::RenderTarget& rt);

        // Returns true if the player has pressed the start key, until reset() is called.
        bool shouldStartGame() const { return m_startPressed; }

        // Call this when re-showing this screen.
        void reset() { m_startPressed = false; m_framesUntilKeyInputAllowed = 20; }

    private:
        bool m_startPressed = false;
        int m_framesUntilKeyInputAllowed = 20;
        sf::Font m_font;
        sf::Text m_descriptionText;
    };
}
