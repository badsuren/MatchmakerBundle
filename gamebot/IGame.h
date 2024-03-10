#pragma once

#include <QString>

namespace gamebot {

    // Interface for games
    class IGame {
    public:
        virtual ~IGame() = default;

        virtual void playGame(const QString &, const QString &) = 0;
    };
} // gamebot
