#include <QRandomGenerator>
#include <QTextStream>

#include "RockPaperScissors.h"

namespace gamebot {
    void RockPaperScissors::playGame(const QString &first_user_name, const QString &second_user_name) {
        // Generate moves for users
        auto first_user_move =
                static_cast<Move_Option>(QRandomGenerator::global()->bounded(static_cast<int>(Move_Option::COUNT)));
        auto second_user_move =
                static_cast<Move_Option>(QRandomGenerator::global()->bounded(static_cast<int>(Move_Option::COUNT)));

        // Determine who has won
        if (first_user_move == second_user_move) {
            // Draw case
            QTextStream(stdout) << first_user_name << " " << second_user_name << "\n";
        } else if ((first_user_move == Move_Option::ROCK && second_user_move == Move_Option::SCISSORS) ||
                   (first_user_move == Move_Option::PAPER && second_user_move == Move_Option::ROCK) ||
                   (first_user_move == Move_Option::SCISSORS && second_user_move == Move_Option::PAPER)) {
            // First player has won
            QTextStream(stdout) << first_user_name << "\n";
        } else {
            // Second player has won
            QTextStream(stdout) << second_user_name << "\n";
        }
    }
} // gamebot
