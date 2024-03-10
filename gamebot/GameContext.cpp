#include <QRandomGenerator>
#include <thread>

#include "GameContext.h"

namespace gamebot {
    // min and max times are given in milliseconds
    constexpr int MIN_WAIT_TIME = 500;
    constexpr int MAX_WAIT_TIME = 2000;

    GameContext::GameContext(std::unique_ptr<IGame> &&game) : m_game_instance{std::move(game)} {

    }

    void GameContext::execute(const QString &first_user_name, const QString &second_user_name) {
        GameContext::wait();
        m_game_instance->playGame(first_user_name, second_user_name);
    }

    void GameContext::wait() {
        const auto thinkingTime = QRandomGenerator::global()->bounded(MIN_WAIT_TIME, MAX_WAIT_TIME);
        std::this_thread::sleep_for(std::chrono::milliseconds(thinkingTime));
    }

} // gamebot
