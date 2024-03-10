#pragma once

#include "IGame.h"

namespace gamebot {
    // 3x3 matrix will be used
    constexpr size_t MATRIX_SIZE = 3;
    using Matrix = std::array<std::array<char, MATRIX_SIZE>, MATRIX_SIZE>;

    // Tic-Tac-Toe implementation
    class TicTacToe : public IGame {
    public:
        // Simulate the game
        void playGame(const QString &first_user_name, const QString &second_user_name) override;

    private:
        Matrix m_matrix{};

        // Check if the player wins
        [[nodiscard]] bool hasWon(const char &identifier) const;

        // Check if the matrix is full
        [[nodiscard]] bool isFullyLoaded() const;

        // Make a move
        void move(const char &identifier);
    };

} // gamebot
