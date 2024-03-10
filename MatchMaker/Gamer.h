#pragma once

#include <QObject>
#include <QTimer>
#include <QHash>

class Gamer: public QObject
{
Q_OBJECT

public:
    enum GamerState
    {
        Free,
        Waiting,
        Busy
    };

    enum GameType
    {
        XO3,
        RPS,
        Dice,
        Undefined
    };

    inline uint qHash(GameType key, uint seed)
    {
        return ::qHash(static_cast<uint>(key), seed);
    }

    explicit Gamer(QString userName, const QHash<GameType, unsigned int> &games, QObject *parent = nullptr);
    ~Gamer() override = default;

    void gameStart(const GameType &game);
    void gameOver(bool gamerWon);

    [[nodiscard]] inline const QString &getGamerName() const
    {
        return gamer;
    }

    [[nodiscard]] inline const GamerState &getCurrentState() const
    {
        return currentState;
    }

    [[nodiscard]] inline const QHash<GameType, unsigned int> &getGameToRatingMap() const
    {
        return gameToRatingMap;
    }

    // converters
    static QString gamerStateToString(const GamerState &state);
    static QString gameTypeToString(const GameType &type);
    static Gamer::GameType stringToGameType(const QString &str);

signals:
    void signal_gamerStateChanged(const QString &userName, const GamerState &newState); // only state
    void signal_gamerRatingChanged(const QString &userName, const GameType &game,
                                   const GamerState &newState, unsigned int newRating); // state and rating

public slots:
    void slot_reviewState();

private:
    QString gamer; // userName
    GamerState currentState{Free};
    GameType currentGame{Undefined};
    QHash<GameType, unsigned int> gameToRatingMap;
    QTimer timer;

    static constexpr int FREE_WAITING_TIME = 2000; // in milliseconds
    static constexpr int BUSY_MIN_WAITING_TIME = 2000; // in milliseconds
    static constexpr int BUSY_MAX_WAITING_TIME = 3000; // in milliseconds
};
