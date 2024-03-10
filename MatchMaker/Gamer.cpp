#include <QRandomGenerator>
#include <QDebug>
#include <utility>
#include "Gamer.h"

Gamer::Gamer(QString userName, const QHash<GameType, unsigned int> &map, QObject *parent)
    : QObject(parent), gamer(std::move(userName)), gameToRatingMap(map)
{
    QObject::connect(&timer, &QTimer::timeout, this, &Gamer::slot_reviewState);

    // start process for current user
    timer.setInterval(FREE_WAITING_TIME);
    timer.start();
}

QString Gamer::gamerStateToString(const GamerState &state)
{
    switch (state) {
    case Free:
        return "Free";
    case Waiting:
        return "Waiting";
    case Busy:
        return "Busy";
    default:
        Q_ASSERT(false);
    }
}

QString Gamer::gameTypeToString(const GameType &type)
{
    switch (type) {
    case XO3:
        return "XO3";
    case RPS:
        return "RPS";
    case Dice:
        return "Dice";
    default:
        Q_ASSERT(false);
    }
}

Gamer::GameType Gamer::stringToGameType(const QString &str)
{
    if (!str.compare("XO3")) {
        return XO3;
    }
    else if (!str.compare("RPS")) {
        return RPS;
    }
    else if (!str.compare("Dice")) {
        return Dice;
    }

    Q_ASSERT(false);
}

void Gamer::gameStart(const GameType &game)
{
    Q_ASSERT(currentState == Waiting);
    Q_ASSERT(currentGame == Undefined);

    timer.stop();

    currentState = Busy;
    currentGame = game;
}

void Gamer::gameOver(bool gamerWon)
{
    Q_ASSERT(currentState == Busy);
    Q_ASSERT(currentGame != Undefined);

//    qDebug() << "Game over. Game: " << gameTypeToString(currentGame) << " State: "
//    << gamerStateToString(currentState) << " User: " << gamer << " Rating: " << gameToRatingMap[currentGame];

    currentState = Free;

    if (gamerWon) {
        ++gameToRatingMap[currentGame];

        emit signal_gamerRatingChanged(gamer, currentGame,
                                       currentState, gameToRatingMap[currentGame]);
    }
    else {
        signal_gamerStateChanged(gamer, currentState);
    }

    currentGame = Undefined;

    timer.start();
}

void Gamer::slot_reviewState()
{
    timer.stop();

    GamerState previousState = currentState;
    switch (currentState) {
    case Free:
        currentState = Waiting;
        timer.setInterval(QRandomGenerator::global()->
            bounded(BUSY_MIN_WAITING_TIME, BUSY_MAX_WAITING_TIME));
        timer.start();
        break;
    case Waiting:
        currentState = Free;
        timer.setInterval(FREE_WAITING_TIME);
        timer.start();
        break;
    case Busy:
        break;
    }

    if (previousState != currentState) {
        emit signal_gamerStateChanged(gamer, currentState);
    }
}
