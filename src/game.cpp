#include <iomanip>
#include <sstream>
#include <string>
#include <vector>
#include "game.h"

lemonade::Game::Game()
{
    m_font.loadFromFile("ComicNeue_Bold.otf");

    initializePlanningUi();
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
        m_planTemperature.setString(std::to_string(m_weather) + "=???");
        m_planMoney.setString(moneyToString(m_money));
        m_planAmountValue.setString(std::to_string(m_amountAvailable));
        m_planPriceValue.setString(moneyToString(m_price));

        m_state = State::Planning;
        break;
    }
    case State::Planning:
        updatePlanning();
        break;
    case State::BeforeCustomers:
        calculateSales();
    case State::Customers:
        m_state = State::BeforeResults;
        break;
    case State::BeforeResults:
    {
        // Update the money
        auto profit = (m_price * m_amountSold) - (CostPerGlass * m_amountAvailable);
        m_money += profit;

        // Initialize the results UI
        m_resultSalesValue.setString(std::to_string(m_amountSold) + "/" + std::to_string(m_amountAvailable));
        m_resultProfitValue.setString(moneyToString(profit));

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
    case State::Results:
        drawResults(rt);
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
    rt.draw(m_planSelectionRectangle);

    rt.draw(m_planDay);
    rt.draw(m_planTemperature);
    rt.draw(m_planMoney);
    rt.draw(m_planAmountDesc);
    rt.draw(m_planAmountValue);
    rt.draw(m_planPriceDesc);
    rt.draw(m_planPriceValue);
    rt.draw(m_planHelp);
}

void lemonade::Game::drawResults(sf::RenderTarget& rt)
{
    rt.draw(m_resultSalesDesc);
    rt.draw(m_resultSalesValue);
    rt.draw(m_resultProfitDesc);
    rt.draw(m_resultProfitValue);
    rt.draw(m_resultHelp);
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
    m_planTemperature.setPosition(0.7f * 1280, 0.1f * 720);

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
    m_planHelp.setFillColor(sf::Color(192, 192, 192, 255));
    m_planHelp.setPosition(0.2f * 1280, 0.8f * 720);
    m_planHelp.setString("Using arrow keys, decide how much lemonade you'll have for sale today and\n"
        "its selling price per glass. A glass of lemonade costs $0.50 to prepare.\n"
        "Press ENTER once you are ready to start selling!");

    m_planSelectionRectangle.setFillColor(sf::Color(32, 32, 32, 255));
    m_planSelectionRectangle.setSize({ 0.4f * 1280, 0.06f * 720 });
    m_planSelectionRectangle.setOrigin({ 6, 6 });
    m_planSelectionRectangle.setPosition(m_planAmountDesc.getPosition());
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
    m_resultHelp.setString("Press ENTER to continue.");
    m_resultHelp.setFillColor(sf::Color(192, 192, 192, 255));
    m_resultHelp.setPosition((1280 - m_resultHelp.getLocalBounds().width) * 0.5f, 720 * 0.75f);
}
