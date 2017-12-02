#include <string>
#include <vector>
#include "game.h"

lemonade::Game::Game()
{
    m_font.loadFromFile("ComicNeue_Bold.otf");

    // Planning phase UI
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
        "its selling price per glass. A glass of lemonade costs §0.50 to prepare.");

    m_planSelectionRectangle.setFillColor(sf::Color(32, 32, 32, 255));
    m_planSelectionRectangle.setSize({ 0.4f * 1280, 0.06f * 720 });
    m_planSelectionRectangle.setOrigin({ 6, 6 });
    m_planSelectionRectangle.setPosition(m_planAmountDesc.getPosition());
}

static const std::vector<sf::String> k_dayNames =
    { "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", "Sunday" };

// Represents money (in cents) as a string.
sf::String moneyToString(int moneyInCents)
{
    // This is a bit complicated but probably simpler than anything provided by the standard library
    auto cents = std::to_string(moneyInCents % 100);
    if (cents.length() == 1)
        cents = "0" + cents;

    return "$" + std::to_string(moneyInCents / 100) + "." + cents;
}

void lemonade::Game::update()
{
    // A state machine controls the various game phases
    switch (m_state)
    {
    case State::BeforePlanning:
    {
        m_amountAvailable = 10;
        m_price = 100;

        // Initialize the planning UI
        m_planDay.setString(k_dayNames[m_day]);
        m_planTemperature.setString("NO WEATHER");
        m_planMoney.setString(moneyToString(m_money));
        m_planAmountValue.setString(std::to_string(m_amountAvailable));
        m_planPriceValue.setString(moneyToString(m_price));

        m_state = State::Planning;
        break;
    }
    case State::Planning:
        updatePlanning();
        break;
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
    default:
        break;
    }
}

void lemonade::Game::updatePlanning()
{
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
