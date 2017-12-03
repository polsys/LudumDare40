#pragma once
#include <vector>
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

        const int StartingMoney = 20000;

        bool m_gameOver = false;
        int m_keyWaitFrames = 3 * KeyInterval;
        sf::Font m_font;

        // Current (zero-indexed) day
        int m_day = 0;

        // Money, in cents
        int m_money = StartingMoney;

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

        // If true, this day a tornado will trash the place.
        bool m_tornado = false;

        // Word-of-mouth factor.
        int m_repeatCustomers = 0;

        // The number of repeat customers gained or lost this time.
        int m_repeatCustomersThisDay = 0;

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
        static constexpr int CustomerPhaseFrames = 11 * 60;
        static constexpr int CustomerAnimationFrames = 8 * 60;
        static constexpr float CustomerQueueLength = 4000.0f;
        static constexpr float CustomerPixelsPerFrame = (800.0f + CustomerQueueLength) / CustomerAnimationFrames;
        static constexpr int CustomersPerDisplayedCustomer = 3;
        static constexpr float TornadoStartX = -800;
        static constexpr float TornadoPixelsPerFrame = (550.0f - TornadoStartX) / CustomerPhaseFrames;

        sf::Texture m_sunnyTexture;
        sf::Texture m_cloudyTexture;
        sf::Texture m_rainyTexture;
        sf::Texture m_customerTexture;
        sf::Texture m_particleTexture;
        sf::Texture m_tornadoTexture;
        sf::Sprite m_tornadoSprite;
        std::vector<sf::Sprite> m_customers;
        std::vector<sf::Sprite> m_particles;
        int m_customersServed;
        sf::Text m_priceOnStand;

        void prepareCustomersAnimation();
        void addParticle();
        void updateCustomers();
        void drawCustomers(sf::RenderTarget& rt);


        // Results phase
        sf::Text m_resultSalesDesc;
        sf::Text m_resultSalesValue;
        sf::Text m_resultProfitDesc;
        sf::Text m_resultProfitValue;
        sf::Text m_resultCustomersDesc;
        sf::Text m_resultCustomersValue;
        sf::Text m_resultHelp;

        // Does the work of draw() in Results phase.
        void drawResults(sf::RenderTarget& rt);


        // Final Results phase
        sf::Text m_finalDescription;
        sf::Text m_finalProfit;
        sf::Text m_finalHighScore;

        // Does the work of draw() in FinalResults phase.
        void drawFinalResults(sf::RenderTarget& rt);


        // Ctor helper methods
        void initializePlanningUi();
        void initializeCustomersUi();
        void initializeResultsUi();

        // This should be called only once the UI should be shown.
        void initializeFinalResultsUi();
    };
}
