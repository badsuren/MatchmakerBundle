#pragma once

#include "IGame.h"

namespace gamebot {

    // Dice implementation
    class Dice : public IGame {
    public:
        // Simulate the game
        void playGame(const QString &first_user_name, const QString &second_user_name) override;
    };

} // gamebot
