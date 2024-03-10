#include <QRandomGenerator>
#include <QTextStream>

#include "TicTacToe.h"

namespace gamebot {
    void TicTacToe::playGame(const QString &first_user_name, const QString &second_user_name) {
        while (true) {
            // First player turn
            TicTacToe::move('X');

            if (TicTacToe::hasWon('X')) {
                QTextStream(stdout) << first_user_name << "\n";
                break;
            }

            // Draw case
            if (TicTacToe::isFullyLoaded()) {
                QTextStream(stdout) << first_user_name << " " << second_user_name << "\n";
                break;
            }

            // Second player turn
            TicTacToe::move('O');

            if (TicTacToe::hasWon('O')) {
                QTextStream(stdout) << second_user_name << "\n";
                break;
            }

            // Draw case
            if (TicTacToe::isFullyLoaded()) {
                QTextStream(stdout) << first_user_name << " " << second_user_name << "\n";
                break;
            }
        }
    }

    bool TicTacToe::hasWon(const char &identifier) const {
        for (size_t i = 0; i < MATRIX_SIZE; ++i) {
            bool row = true;
            bool col = true;

            for (size_t j = 0; j < MATRIX_SIZE && (row || col); ++j) {
                if (m_matrix[i][j] != identifier) {
                    row = false;
                }

                if (m_matrix[j][i] != identifier) {
                    col = false;
                }
            }

            if (row || col) {
                return true;
            }
        }

        bool primary_diagonal = true;
        bool secondary_diagonal = true;

        for (size_t i = 0; i < MATRIX_SIZE && (primary_diagonal || secondary_diagonal); ++i) {
            if (m_matrix[i][i] != identifier) {
                primary_diagonal = false;
            }

            if (m_matrix[i][MATRIX_SIZE - 1 - i] != identifier) {
                secondary_diagonal = false;
            }
        }

        return primary_diagonal || secondary_diagonal;
    }

    bool TicTacToe::isFullyLoaded() const {
        for (const auto &row: m_matrix) {
            for (const auto &cell: row) {
                if (!cell) {
                    return false;
                }
            }
        }

        return true;
    }

    void TicTacToe::move(const char &identifier) {
        while (true) {
            auto row = QRandomGenerator::global()->bounded(static_cast<int>(MATRIX_SIZE));
            auto col = QRandomGenerator::global()->bounded(static_cast<int>(MATRIX_SIZE));

            if (!m_matrix[row][col]) {
                m_matrix[row][col] = identifier;
                break;
            }
        }
    }

} // gamebot
