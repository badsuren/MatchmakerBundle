#pragma once

#include "Gamer.h"
#include "DatabaseHelper.h"
#include "DashboardModel.h"
#include "UsersListProxyModel.h"

#include <QObject>
#include <QProcess>

// Controller class
class MatchMakerEngine: public QObject
{
Q_OBJECT

public:
    explicit MatchMakerEngine(std::unique_ptr<DataBaseHelper> db,
                              std::unique_ptr<QHash<Gamer::GameType, QString>> gamesPaths,
                              UsersListProxyModel *usersProxyModel,
                              DashboardModel *dm, QObject *parent = nullptr);
    ~MatchMakerEngine() override = default;

    void populateData();
    [[nodiscard]] int getUsersListRowCount() const; // helper function

    bool addNewUser(const QString &userName, const QString &firstName,
                    const QString &lastName, const QStringList &games);
    bool deleteUser(const QString &userName);

    void exportCSV(const QString &filePath);

    void usersListFilterTextChanged(const QString &pattern);

signals:

private slots:
    void slot_gamerStateChanged(const QString &userName, const Gamer::GamerState &newState);
    void slot_gamerRatingChanged(const QString &userName, const Gamer::GameType &game,
                                 const Gamer::GamerState &newState, unsigned int newRating);
    void slot_gameFinished(int exitCode, QProcess::ExitStatus exitStatus,
                           const QString &player1, const QString &player2);
//    void slot_gameCrashed(QProcess::ProcessError error,
//                          const QString &player1, const QString &player2);

private:
    // database
    std::unique_ptr<DataBaseHelper> databaseHelper;

    // models
    UsersListProxyModel *usersListProxyModel{};
    DashboardModel *dashboardModel{};

    // data - ideally we should have used the data from view models, but
    // in order for the code to be more readable and simple the additional store is used
    QHash<QString, std::shared_ptr<Gamer>> listOfGamers{};

    // TODO: ?
    // Structure: <GameType, <rating, username>>
    // QHash<Gamer::GameType, QMultiMap<int, QString>> waitingGamers;
    // QMultiMap<int, QString>::iterator lower = map.lowerBound(requesterRating - 3);
    // QMultiMap<int, QString>::iterator upper = map.upperBound(requesterRating + 3);
    // then try to find 1 <= std::abs(matchRating - rating) >= 3 from lower iterator to upper
    // But as we will iterate over all games to remove waiting users from all 3 lists - this solution is not optimal
    // <GameType, <username, rating>>
    //QHash<Gamer::GameType, QHash<QString, int>> waitingGamers;

    std::unique_ptr<QHash<Gamer::GameType, QString>> gameToExecPath{};

    void populateUsersList();
    void populateDashboard(); // dashboard and internal listOfGamers
    void addGamer(const QString &userName, const QHash<Gamer::GameType, unsigned int> &map);
    void removeGamer(const QString &userName);
    void startGame(const QString &gamer1, const QString &gamer2, const Gamer::GameType &gameType);
    // recursively process parent and child rows - csv export
    void processCSVRow(const QModelIndex &parentIndex, QTextStream &out);

    // matching
    // try to find opponent and game
    [[nodiscard]] QPair<std::shared_ptr<Gamer>, Gamer::GameType> matching(const QString &userName) const;
};
