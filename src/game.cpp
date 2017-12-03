#include <iomanip>
#include <random>
#include <sstream>
#include <string>
#include "game.h"

lemonade::Game::Game()
{
    m_font.loadFromFile("ComicNeue_Bold.otf");

    m_fullscreenSprite.setPosition({ 0,0 });
    initializePlanningUi();
    initializeCustomersUi();
    initializeResultsUi();
}

static const std::vector<sf::String> k_dayNames =
    { "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", "Sunday" };

// Represents money (in cents) as a string.
sf::String moneyToString(int moneyInCents)
{
    std::stringstream str;
    str << "$" << std::fixed << std::setprecision(2) << (moneyInCents / 100.0f);

    return str.str();
}

void lemonade::Game::update()
{
    // A state machine controls the various game phases
    switch (m_state)
    {
    case State::BeforePlanning:
    {
        m_amountAvailable = 50;
        m_price = 100;

        // Initialize the planning UI
        m_planDay.setString(k_dayNames[m_day]);
        setWeatherForecast();
        m_planMoney.setString(moneyToString(m_money));
        m_planAmountValue.setString(std::to_string(m_amountAvailable));
        m_planPriceValue.setString(moneyToString(m_price));
        m_fullscreenSprite.setTexture(m_backgroundTexture);

        m_state = State::Planning;
        break;
    }
    case State::Planning:
        updatePlanning();
        break;
    case State::BeforeCustomers:
    {
        calculateSales();
        prepareCustomersAnimation();
        m_state = State::Customers;
        break;
    }
    case State::Customers:
    {
        m_customersFrame++;
        if (m_customersFrame > CustomerPhaseFrames)
        {
            m_state = State::BeforeResults;
        }
        else if (m_customersFrame < CustomerAnimationFrames)
        {
            // Update the animation
            for (auto it = m_customers.begin() + m_customersServed; it != m_customers.end(); ++it)
            {
                auto position = it->getPosition();
                if (m_customersServed * CustomersPerDisplayedCustomer < m_amountAvailable)
                {
                    // The queue only moves when there is lemonade available
                    position.x += CustomerPixelsPerFrame;
                }
                if (position.x >= 700 && position.y < 900)
                {
                    // Hide the customers who have gotten their lemonade
                    m_customersServed++;
                }
                it->setPosition(position);
            }
        }
        else if (m_customersFrame == CustomerAnimationFrames)
        {
            // Make everybody remaining unhappy
            for (auto it = m_customers.begin(); it != m_customers.end(); ++it)
            {
                auto textureRect = it->getTextureRect();
                textureRect.top = 350;
                it->setTextureRect(textureRect);
            }
        }
        break;
    }
    case State::BeforeResults:
    {
        // Update the money
        auto profit = (m_price * m_amountSold) - (CostPerGlass * m_amountAvailable);
        m_money += profit;

        // Initialize the results UI
        m_resultSalesValue.setString(std::to_string(m_amountSold) + "/" + std::to_string(m_amountAvailable));
        m_resultProfitValue.setString(moneyToString(profit));
        m_fullscreenSprite.setTexture(m_backgroundTexture);

        m_state = State::Results;
        break;
    }
    case State::Results:
    {
        if (m_keyWaitFrames > 0)
        {
            m_keyWaitFrames--;
        }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Return))
        {
            m_keyWaitFrames = KeyInterval * 2;
            if (m_day == 6)
            {
                m_state = State::FinalResults;
                initializeFinalResultsUi();
                // TODO: Save highscores
                // TODO: End up here also if out of money
            }
            else
            {
                m_day++;
                calculateWeather();
                m_state = State::BeforePlanning;
            }
        }
        break;
    }
    case State::FinalResults:
    {
        if (m_keyWaitFrames > 0)
        {
            m_keyWaitFrames--;
        }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Return))
        {
            m_gameOver = true;
        }
        break;
    }
    default:
        break;
    }
}

void lemonade::Game::draw(sf::RenderTarget& rt)
{
    rt.clear(sf::Color::Black);

    switch (m_state)
    {
    case State::Planning:
        drawPlanning(rt);
        break;
    case State::Customers:
        rt.draw(m_fullscreenSprite);
        rt.draw(m_priceOnStand);
        for (auto it = m_customers.cbegin() + m_customersServed; it != m_customers.cend(); ++it)
        {
            rt.draw(*it);
        }
        break;
    case State::Results:
        drawResults(rt);
        break;
    case State::FinalResults:
        drawFinalResults(rt);
        break;
    default:
        break;
    }
}

void lemonade::Game::calculateSales()
{
    // Based on the current weather, determine a price the customers are willing to pay
    // This is tuned to be:
    //   - $7.00 (700 cents) at weather level 100
    //   - $0.00 at weather level 20
    auto fairPrice = (-175.0f / 4) + (175.0f / 80) * m_weather;

    // On weekends, this "fair price" is higher
    if (m_day >= 5)
        fairPrice += 1;

    // The number of potential customers is given by the ratio to the actual price
    // Some customers are guaranteed (passer-bys), some are repeat customers
    auto customerFactor = fairPrice / m_price;
    m_potentialCustomers = static_cast<int>(std::round((m_repeatCustomers + 100) * customerFactor));
    if (m_potentialCustomers < 0)
        m_potentialCustomers = 0;
    m_amountSold = std::min(m_potentialCustomers, m_amountAvailable);

    // The price affects the number of repeat customers
    // This is tuned to be +50% at $0.25 and go down 10% per $0.25
    // The total number of repeat customers is allowed to be negative
    auto repeatFactor = (-0.004f * m_price) + 0.6f;
    m_repeatCustomers += static_cast<int>(std::round(repeatFactor * m_amountSold));
}

void lemonade::Game::calculateWeather()
{
    // The weather is determined by the number of people who show up at the lemonade stand
    // (which may be less than sales). This is tuned to be:
    //    - weather level 100 at 0 people
    //    - weather level 0 at 200 people
    m_weather = static_cast<int>(std::round(100 - 0.5f * m_potentialCustomers));
}

void lemonade::Game::setWeatherForecast()
{
    // The forecast:
    if (m_weather < 10)
    {
        // Thunder
        m_planForecastSprite.setTextureRect(sf::IntRect(192, 0, 64, 64));
    }
    else if (m_weather < 40)
    {
        // Rain
        m_planForecastSprite.setTextureRect(sf::IntRect(128, 0, 64, 64));
    }
    else if (m_weather < 60)
    {
        // Partly cloudy
        m_planForecastSprite.setTextureRect(sf::IntRect(64, 0, 64, 64));
    }
    else
    {
        // Sunny
        m_planForecastSprite.setTextureRect(sf::IntRect(0, 0, 64, 64));
    }

    // The temperature is calculated from the weather so that
    //   - weather level 100 -> 30 degrees C
    //   - weather level 0   -> 10 degrees C
    auto celsius = static_cast<int>(std::round(m_weather / 5.0f + 5.0f));
    if (celsius < 5)
        celsius = 5;
    auto fahrenheit = static_cast<int>(std::round((celsius * 9.0f / 5.0f) + 32));
    m_planTemperature.setString(std::to_string(celsius) + " C / " + std::to_string(fahrenheit) + " F");
}

void lemonade::Game::updatePlanning()
{
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Return) && (m_keyWaitFrames == 0))
    {
        m_state = State::BeforeCustomers;
        m_keyWaitFrames = KeyInterval * 2;
        return;
    }

    if (m_planSelection == PlanningInput::Amount)
    {
        if (m_keyWaitFrames > 0)
        {
            m_keyWaitFrames--;
        }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left) && (m_amountAvailable > 10))
        {
            m_amountAvailable -= 10;
            m_planAmountValue.setString(std::to_string(m_amountAvailable));
            m_keyWaitFrames = KeyInterval;
        }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right) && (m_money >= ((m_amountAvailable + 10) * CostPerGlass)))
        {
            m_amountAvailable += 10;
            m_planAmountValue.setString(std::to_string(m_amountAvailable));
            m_keyWaitFrames = KeyInterval;
        }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
        {
            m_planSelection = PlanningInput::Price;
            m_planSelectionRectangle.setPosition(m_planPriceDesc.getPosition());
        }
    }
    else if (m_planSelection == PlanningInput::Price)
    {
        if (m_keyWaitFrames > 0)
        {
            m_keyWaitFrames--;
        }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left) && (m_price > 25))
        {
            m_price -= 25;
            m_planPriceValue.setString(moneyToString(m_price));
            m_keyWaitFrames = KeyInterval;
        }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right) && (m_price < 200))
        {
            m_price += 25;
            m_planPriceValue.setString(moneyToString(m_price));
            m_keyWaitFrames = KeyInterval;
        }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
        {
            m_planSelection = PlanningInput::Amount;
            m_planSelectionRectangle.setPosition(m_planAmountDesc.getPosition());
        }
    }
}

void lemonade::Game::drawPlanning(sf::RenderTarget& rt)
{
    rt.draw(m_fullscreenSprite);
    rt.draw(m_planSelectionRectangle);

    rt.draw(m_planDay);
    rt.draw(m_planForecastSprite);
    rt.draw(m_planTemperature);
    rt.draw(m_planMoney);
    rt.draw(m_planAmountDesc);
    rt.draw(m_planAmountValue);
    rt.draw(m_planPriceDesc);
    rt.draw(m_planPriceValue);
    rt.draw(m_planHelp);
}

void lemonade::Game::prepareCustomersAnimation()
{
    m_priceOnStand.setString(moneyToString(m_price));
    m_customersFrame = 0;

    // Set the background based on weather
    if (m_weather < 40)
        m_fullscreenSprite.setTexture(m_rainyTexture);
    else if (m_weather < 60)
        m_fullscreenSprite.setTexture(m_cloudyTexture);
    else
        m_fullscreenSprite.setTexture(m_sunnyTexture);

    // The customer sprites are randomly chosen and stored in a vector
    auto customerCount = m_potentialCustomers / CustomersPerDisplayedCustomer;
    std::uniform_int_distribution<int> customerColor(0,7);
    std::minstd_rand random(m_weather * 577 + customerCount);
    auto pixelsBetweenCustomers = -CustomerQueueLength / customerCount;

    m_customersServed = 0;
    m_customers.clear();
    m_customers.reserve(customerCount);
    for (auto i = 0; i < customerCount; i++)
    {
        sf::Sprite customer;
        customer.setTexture(m_customerTexture);
        customer.setTextureRect(sf::IntRect(128 * customerColor(random), 0, 128, 350));
        customer.setOrigin({ 0, 350 });
        customer.setPosition({ -128 + pixelsBetweenCustomers * i, 580.0f });
        
        m_customers.push_back(customer);
    }
}

void lemonade::Game::drawResults(sf::RenderTarget& rt)
{
    rt.draw(m_fullscreenSprite);
    rt.draw(m_resultSalesDesc);
    rt.draw(m_resultSalesValue);
    rt.draw(m_resultProfitDesc);
    rt.draw(m_resultProfitValue);
    rt.draw(m_resultHelp);
}

void lemonade::Game::drawFinalResults(sf::RenderTarget& rt)
{
    rt.draw(m_fullscreenSprite);
    rt.draw(m_finalDescription);
    rt.draw(m_finalProfit);
    rt.draw(m_finalHighScore);
    rt.draw(m_resultHelp); // Press ENTER to continue
}

void lemonade::Game::initializePlanningUi()
{
    m_planDay.setFont(m_font);
    m_planDay.setCharacterSize(24);
    m_planDay.setPosition(0.2f * 1280, 0.1f * 720);

    m_planMoney.setFont(m_font);
    m_planMoney.setCharacterSize(24);
    m_planMoney.setPosition(0.2f * 1280, 0.1f * 720 + 30);

    m_planTemperature.setFont(m_font);
    m_planTemperature.setCharacterSize(24);
    m_planTemperature.setPosition(0.7f * 1280, 0.1f * 720 + 15);

    m_planAmountDesc.setFont(m_font);
    m_planAmountDesc.setCharacterSize(24);
    m_planAmountDesc.setPosition(0.3f * 1280, 0.4f * 720);
    m_planAmountDesc.setString("Amount:");

    m_planAmountValue.setFont(m_font);
    m_planAmountValue.setCharacterSize(24);
    m_planAmountValue.setPosition(0.6f * 1280, 0.4f * 720);

    m_planPriceDesc.setFont(m_font);
    m_planPriceDesc.setCharacterSize(24);
    m_planPriceDesc.setPosition(0.3f * 1280, 0.5f * 720);
    m_planPriceDesc.setString("Price:");

    m_planPriceValue.setFont(m_font);
    m_planPriceValue.setCharacterSize(24);
    m_planPriceValue.setPosition(0.6f * 1280, 0.5f * 720);

    m_planHelp.setFont(m_font);
    m_planHelp.setCharacterSize(20);
    m_planHelp.setFillColor(sf::Color(224, 224, 224, 255));
    m_planHelp.setPosition(0.2f * 1280, 0.8f * 720);
    m_planHelp.setString("Using arrow keys, decide how much lemonade you'll have for sale today and\n"
        "the price per glass. A glass of lemonade costs $0.50 to prepare.\n"
        "Press ENTER once you are ready to start selling!");

    m_planSelectionRectangle.setFillColor(sf::Color(32, 32, 32, 255));
    m_planSelectionRectangle.setSize({ 0.4f * 1280, 0.06f * 720 });
    m_planSelectionRectangle.setOrigin({ 6, 6 });
    m_planSelectionRectangle.setPosition(m_planAmountDesc.getPosition());

    m_forecastTexture.loadFromFile("Weather.png");
    m_planForecastSprite.setTexture(m_forecastTexture);
    m_planForecastSprite.setPosition(0.7f * 1280 - 72, 0.1f * 720 - 5);

    m_backgroundTexture.loadFromFile("Planning.png");
}

void lemonade::Game::initializeCustomersUi()
{
    m_sunnyTexture.loadFromFile("BackgroundSunny.png");
    m_cloudyTexture.loadFromFile("BackgroundCloudy.png");
    m_rainyTexture.loadFromFile("BackgroundRainy.png");
    m_customerTexture.loadFromFile("People.png");

    m_priceOnStand.setFont(m_font);
    m_priceOnStand.setCharacterSize(36);
    m_priceOnStand.setPosition({ 1000, 340 });
    m_priceOnStand.setFillColor(sf::Color(54, 54, 54, 255));
}

void lemonade::Game::initializeResultsUi()
{
    m_resultSalesDesc.setFont(m_font);
    m_resultSalesDesc.setCharacterSize(32);
    m_resultSalesDesc.setPosition(0.35f * 1280, 0.4f * 720);
    m_resultSalesDesc.setString("Glasses sold:");

    m_resultSalesValue.setFont(m_font);
    m_resultSalesValue.setCharacterSize(32);
    m_resultSalesValue.setPosition(0.55f * 1280, 0.4f * 720);

    m_resultProfitDesc.setFont(m_font);
    m_resultProfitDesc.setCharacterSize(32);
    m_resultProfitDesc.setPosition(0.35f * 1280, 0.5f * 720);
    m_resultProfitDesc.setString("Profit:");

    m_resultProfitValue.setFont(m_font);
    m_resultProfitValue.setCharacterSize(32);
    m_resultProfitValue.setPosition(0.55f * 1280, 0.5f * 720);

    m_resultHelp.setFont(m_font);
    m_resultHelp.setCharacterSize(20);
    m_resultHelp.setString("Press ENTER to continue");
    m_resultHelp.setFillColor(sf::Color(224, 224, 224, 255));
    m_resultHelp.setPosition((1280 - m_resultHelp.getLocalBounds().width) * 0.5f, 720 * 0.75f);
}

void lemonade::Game::initializeFinalResultsUi()
{
    m_finalDescription.setFont(m_font);
    m_finalDescription.setCharacterSize(32);
    m_finalDescription.setString("Week's up! Total profit:");
    m_finalDescription.setPosition((1280 - m_finalDescription.getLocalBounds().width) * 0.5f, 0.2f * 720);

    m_finalProfit.setFont(m_font);
    m_finalProfit.setCharacterSize(56);
    m_finalProfit.setString(moneyToString(m_money - StartingMoney));
    m_finalProfit.setPosition((1280 - m_finalProfit.getLocalBounds().width) * 0.5f, 0.3f * 720);

    m_finalHighScore.setFont(m_font);
    m_finalHighScore.setCharacterSize(32);
    m_finalHighScore.setString("Is that a high score? I don't know!");
    m_finalHighScore.setPosition((1280 - m_finalHighScore.getLocalBounds().width) * 0.5f, 0.5f * 720);
}
