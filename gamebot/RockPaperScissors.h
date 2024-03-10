#pragma once

#include "IGame.h"

namespace gamebot {

    // Rock-Paper-Scissors implementation
    class RockPaperScissors : public IGame {
        enum class Move_Option {
            ROCK,           // 0
            PAPER,          // 1
            SCISSORS,       // 2
            COUNT           // 3
        };

    public:
        // Simulate the game
        void playGame(const QString &first_user_name, const QString &second_user_name) override;
    };

} // gamebot
