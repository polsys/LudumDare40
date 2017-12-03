#pragma once

namespace lemonade
{
    struct HighScores
    {
        // The highest score.
        int high = 0;

        // The lowest score.
        int low = 0;

        // If false, no games have been played so far.
        bool hasValue = false;


        // Loads high score data from a file.
        // This should be called at game startup.
        static void initializeHighScores();

        // Gets the current high scores.
        static HighScores getHighScores();

        // Adds the specified score into high scores if it is a new high/low.
        // If the score is record-breaking, this will return true; false otherwise.
        static bool addScore(int score);

    private:
        static void saveHighScores();
    };
}