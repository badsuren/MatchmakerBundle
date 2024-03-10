#include <QRandomGenerator>
#include <QTextStream>

#include "Dice.h"

namespace gamebot {
    // Dice min and max values
    constexpr int DICE_MIN_VALUE = 1;
    constexpr int DICE_MAX_VALUE = 6;

    void Dice::playGame(const QString &first_user_name, const QString &second_user_name) {
        // Generate moves for users
        auto first_user_move =
                QRandomGenerator::global()->bounded(DICE_MIN_VALUE, DICE_MAX_VALUE + 1);
        auto second_user_move =
                QRandomGenerator::global()->bounded(DICE_MIN_VALUE, DICE_MAX_VALUE + 1);

        // Determine who has won
        if (first_user_move == second_user_move) {
            // Draw case
            QTextStream(stdout) << first_user_name << " " << second_user_name << "\n";
        } else if (first_user_move > second_user_move) {
            // First player has won
            QTextStream(stdout) << first_user_name << "\n";
        } else {
            // Second player has won
            QTextStream(stdout) << second_user_name << "\n";
        }
    }
} // gamebot
