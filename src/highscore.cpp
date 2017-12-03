#include <fstream>
#include "highscore.h"

static lemonade::HighScores s_highScore;

void lemonade::HighScores::initializeHighScores()
{
    std::ifstream file("hiscore.dat", std::ifstream::binary);
    if (file)
    {
        file.read(reinterpret_cast<char*>(&s_highScore.high), sizeof(s_highScore.high));
        file.read(reinterpret_cast<char*>(&s_highScore.low), sizeof(s_highScore.low));
        s_highScore.hasValue = true;
    }
}

lemonade::HighScores lemonade::HighScores::getHighScores()
{
    return s_highScore;
}

bool lemonade::HighScores::addScore(int score)
{
    // Add the score to the list if it exceeds the previous record
    // or these has been no record at all.
    bool isRecord = false;

    if (score > s_highScore.high || !s_highScore.hasValue)
    {
        s_highScore.high = score;
        isRecord = true;
    }

    if (score < s_highScore.low || !s_highScore.hasValue)
    {
        s_highScore.low = score;
        isRecord = true;
    }

    if (isRecord)
    {
        saveHighScores();
    }

    s_highScore.hasValue = true;

    return isRecord;
}

void lemonade::HighScores::saveHighScores()
{
    std::ofstream file("hiscore.dat", std::ofstream::binary);
    if (file)
    {
        file.write(reinterpret_cast<const char*>(&s_highScore.high), sizeof(s_highScore.high));
        file.write(reinterpret_cast<const char*>(&s_highScore.low), sizeof(s_highScore.low));
    }
}
