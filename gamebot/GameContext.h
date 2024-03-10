#pragma once

#include "IGame.h"

namespace gamebot {

    class GameContext {
    public:
        explicit GameContext(std::unique_ptr<IGame> &&game);

        // Start the game
        void execute(const QString &first_user_name, const QString &second_user_name);

    private:
        std::unique_ptr<IGame> m_game_instance; // concrete game

        // Simulate thinking time
        static void wait();
    };

} // gamebot
