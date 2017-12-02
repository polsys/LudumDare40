#pragma once

#include "SFML/Graphics.hpp"

namespace lemonade
{
    // This class handles the actual gameplay.
    class Game
    {
    public:
        Game();

        // Handles input, etc.
        void update();

        // Clears the rendertarget and draws the title screen there.
        void draw(sf::RenderTarget& rt);

        // Returns true when this screen should be destroyed and replaced with the title screen.
        bool isGameOver() const { return m_gameOver; }

    private:
        // The cost (in cents) of manufacturing a glass of lemonade
        const int CostPerGlass = 50;

        // Number of frames between accepted key presses.
        const int KeyInterval = 10;

        bool m_gameOver = false;
        int m_keyWaitFrames = 0;
        sf::Font m_font;

        // Current (zero-indexed) day
        int m_day = 0;

        // Money, in § cents
        int m_money = 20000;

        // Glasses of lemonade still available
        int m_amountAvailable;

        // Price of a glass of lemonade, in cents
        int m_price;

        enum class State
        {
            BeforePlanning,
            Planning,
            BeforeCustomers,
            Customers,
            BeforeResults,
            Results,
            FinalResults
        };
        State m_state = State::BeforePlanning;

        // Does the job of update() in Planning phase.
        void updatePlanning();
        // Does the work of draw() in Planning phase.
        void drawPlanning(sf::RenderTarget& rt);

        // Planning phase
        sf::Text m_planDay;
        sf::Text m_planMoney;
        sf::Text m_planTemperature;
        sf::Text m_planAmountDesc;
        sf::Text m_planAmountValue;
        sf::Text m_planPriceDesc;
        sf::Text m_planPriceValue;
        sf::Text m_planHelp;
        sf::RectangleShape m_planSelectionRectangle;

        enum class PlanningInput
        {
            Amount,
            Price
        };
        PlanningInput m_planSelection = PlanningInput::Amount;
    };
}
