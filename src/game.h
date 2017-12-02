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
        // Calculates how many glasses of lemonade will be sold today.
        void calculateSales();

        // Calculates new weather based on sales figures.
        void calculateWeather();

        // The cost (in cents) of manufacturing a glass of lemonade
        const int CostPerGlass = 50;

        // Number of frames between accepted key presses.
        const int KeyInterval = 10;

        bool m_gameOver = false;
        int m_keyWaitFrames = 3 * KeyInterval;
        sf::Font m_font;

        // Current (zero-indexed) day
        int m_day = 0;

        // Money, in cents
        int m_money = 20000;

        // Glasses of lemonade manufactured for this day
        int m_amountAvailable;

        // Glasses of lemonade sold this day
        int m_amountSold;

        // Price of a glass of lemonade, in cents
        int m_price;

        // The current weather.
        // 0 is abominable, 100 is awesome.
        // Not restricted to between these values!
        int m_weather = 80;

        // Word-of-mouth factor.
        int m_repeatCustomers = 0;

        // How many people show up at the stand today.
        // This is greater than amount sold if there isn't enough lemonade for everybody
        int m_potentialCustomers = 0;

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

        sf::Sprite m_fullscreenSprite;

        // Helper for the Planning UI.
        void setWeatherForecast();
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
        sf::Texture m_forecastTexture;
        sf::Sprite m_planForecastSprite;
        sf::Texture m_backgroundTexture;

        enum class PlanningInput
        {
            Amount,
            Price
        };
        PlanningInput m_planSelection = PlanningInput::Amount;


        // Customers phase
        int m_customersFrame;

        sf::Texture m_sunnyTexture;
        sf::Texture m_cloudyTexture;
        sf::Texture m_rainyTexture;
        sf::Text m_priceOnStand;


        // Results phase
        sf::Text m_resultSalesDesc;
        sf::Text m_resultSalesValue;
        sf::Text m_resultProfitDesc;
        sf::Text m_resultProfitValue;
        sf::Text m_resultHelp;

        // Does the work of draw() in Results phase.
        void drawResults(sf::RenderTarget& rt);

        // Ctor helper methods
        void initializePlanningUi();
        void initializeCustomersUi();
        void initializeResultsUi();
    };
}
