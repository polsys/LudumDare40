#pragma once

#include <iomanip>
#include <sstream>
#include "SFML/System/String.hpp"

namespace lemonade
{
    // Represents money (in cents) as a string.
    inline sf::String moneyToString(int moneyInCents)
    {
        std::stringstream str;
        str << "$" << std::fixed << std::setprecision(2) << (moneyInCents / 100.0f);

        return str.str();
    }
}
