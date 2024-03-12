#include "MatchMakerEngine.h"
#include "UsersListModel.h"

#include <QProcess>
#include <QFile>
#include <QRandomGenerator>
//#include <QtMath>

MatchMakerEngine::MatchMakerEngine(std::unique_ptr<DataBaseHelper> db,
                                   std::unique_ptr<QHash<Gamer::GameType, QString>> gamesPaths,
                                   UsersListProxyModel *usersProxyModel,
                                   DashboardModel *dm, QObject *parent)
    : QObject(parent),
      databaseHelper(std::move(db)),
      gameToExecPath(std::move(gamesPaths)),
      usersListProxyModel(usersProxyModel),
      dashboardModel(dm)
{

}

void MatchMakerEngine::populateData()
{
    populateUsersList();
    populateDashboard();
}

int MatchMakerEngine::getUsersListRowCount() const
{
    return usersListProxyModel ? usersListProxyModel->rowCount() : 0;
}

bool MatchMakerEngine::addNewUser(const QString &userName, const QString &firstName,
                                  const QString &lastName, const QStringList &games)
{
    // database changes
    if (!databaseHelper->addNewUser(userName, firstName, lastName, games)) {
        return false;
    }

    auto userModel = dynamic_cast<UsersListModel *>(usersListProxyModel->sourceModel());

    // models changes
    userModel->addUser(userName, firstName, lastName, games);
    dashboardModel->addUserGames(userName, games);

    // internal data changes
    QHash<Gamer::GameType, unsigned int> map;
    for (const auto &game: games) {
        map[Gamer::stringToGameType(game)] = 0;
    }
    addGamer(userName, map);

    return true;
}

bool MatchMakerEngine::deleteUser(const QString &userName)
{
    // database changes
    if (!databaseHelper->deleteUser(userName)) {
        return false;
    }

    auto userModel = dynamic_cast<UsersListModel *>(usersListProxyModel->sourceModel());

    // models changes
    userModel->deleteUser(userName);
    dashboardModel->deleteUser(userName);

    // internal data changes
    removeGamer(userName);

    return true;
}

void MatchMakerEngine::processCSVRow(const QModelIndex &parentIndex, QTextStream &out)
{
    for (int row = 0; row < dashboardModel->rowCount(parentIndex); ++row) {
        QModelIndex index = dashboardModel->index(row, DashboardModel::Game, parentIndex);
        for (int column = 0; column < dashboardModel->columnCount(); ++column) {
            out << dashboardModel->data(index.sibling(row, column)).toString() << ",";
        }
        out << "\n";

        // Recursively process child rows
        processCSVRow(index, out);
    }
}

void MatchMakerEngine::exportCSV(const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qCritical() << "Failed to open file for writing:" << filePath;
        return;
    }

    // Create text stream for writing CSV
    QTextStream out(&file);

    // headers
    int columnCount = dashboardModel->columnCount();
    for (int col = 0; col < columnCount; ++col) {
        out << dashboardModel->headerData(col, Qt::Horizontal).toString() << ",";
    }

    out << "\n";

    processCSVRow(QModelIndex(), out);

    file.close();
}

void MatchMakerEngine::populateUsersList()
{
    QSqlQuery query;
    if (!databaseHelper->getUsersListData(query)) {
        return;
    }

    auto userModel = dynamic_cast<UsersListModel *>(usersListProxyModel->sourceModel());

    // Populate the model with data
    while (query.next()) {
        QString userName = query.value(UsersListModel::Username).toString();
        QString firstName = query.value(UsersListModel::FirstName).toString();
        QString lastName = query.value(UsersListModel::LastName).toString();
        QString preferredGames = query.value(UsersListModel::PreferredGames).toString();
        Gamer::GamerState state = Gamer::Free;

        auto *userNameItem = new QStandardItem(userName);
        auto *firstNameItem = new QStandardItem(firstName);
        auto *lastNameItem = new QStandardItem(lastName);
        auto *preferredGamesItem = new QStandardItem(preferredGames);
        auto *statusItem = new QStandardItem(Gamer::gamerStateToString(state));

        userModel->appendRow({userNameItem, firstNameItem, lastNameItem, preferredGamesItem, statusItem});
    }

    userModel->addSpecialRow();

    userModel->setColumnCount(UsersListModel::Count);
    userModel->setHeaderData(UsersListModel::Username, Qt::Horizontal, "Username");
    userModel->setHeaderData(UsersListModel::FirstName, Qt::Horizontal, "First name");
    userModel->setHeaderData(UsersListModel::LastName, Qt::Horizontal, "Last name");
    userModel->setHeaderData(UsersListModel::PreferredGames, Qt::Horizontal, "Preferred games");
    userModel->setHeaderData(UsersListModel::State, Qt::Horizontal, "State");
}

void MatchMakerEngine::populateDashboard()
{
    QSqlQuery query;
    if (!databaseHelper->getDashboardData(query)) {
        return;
    }

    QHash<QString, QHash<Gamer::GameType, unsigned int>> data;

    // Populate the model and internal map with data
    while (query.next()) {
        QString gameName = query.value("gamename").toString();
        QString userName = query.value("username").toString();
        int rating = query.value("rating").toInt();

        // Find or create the parent item for the game
        QStandardItem *parentItem = dashboardModel->findGameItem(gameName);
        if (!parentItem) {
            parentItem = new QStandardItem(gameName);
            dashboardModel->appendRow(parentItem);
        }

        // Create child items for username, rating
        auto *usernameItem = new QStandardItem(userName);
        auto *ratingItem = new DashboardModel::QRatingItem(rating);

        // Add child items to the parent
        parentItem->appendRow({{}, usernameItem, ratingItem});

        Gamer::GameType gameType = Gamer::stringToGameType(gameName);
        QHash<Gamer::GameType, unsigned int> &games = data[userName];
        games[gameType] = rating;
    }

    QStandardItem *parentItem;
    for (int type = 0; type < Gamer::Undefined; ++type) {
        const QString &gameName = Gamer::gameTypeToString(static_cast<Gamer::GameType>(type));
        if (!dashboardModel->findGameItem(gameName)) {
            parentItem = new QStandardItem(gameName);
            dashboardModel->appendRow(parentItem);
        }
    }


    dashboardModel->setColumnCount(DashboardModel::Count);
    dashboardModel->setHeaderData(DashboardModel::Game, Qt::Horizontal, "Game");
    dashboardModel->setHeaderData(DashboardModel::Username, Qt::Horizontal, "Username");
    dashboardModel->setHeaderData(DashboardModel::Rating, Qt::Horizontal, "Rating");

    // Set sorting criteria
    dashboardModel->sort(DashboardModel::Game);
    dashboardModel->sort(DashboardModel::Rating, Qt::DescendingOrder);

    const auto &userNames = data.keys();
    for (const auto &userName: userNames) {
        addGamer(userName, data[userName]);
    }
}

void MatchMakerEngine::addGamer(const QString &userName, const QHash<Gamer::GameType, unsigned int> &map)
{
    Q_ASSERT(!listOfGamers.contains(userName));

    listOfGamers[userName] = std::make_shared<Gamer>(userName, map);
    QObject::connect(listOfGamers[userName].get(), &Gamer::signal_gamerStateChanged,
                     this, &MatchMakerEngine::slot_gamerStateChanged);
    QObject::connect(listOfGamers[userName].get(), &Gamer::signal_gamerRatingChanged,
                     this, &MatchMakerEngine::slot_gamerRatingChanged);

}

void MatchMakerEngine::removeGamer(const QString &userName)
{
    Q_ASSERT(listOfGamers.contains(userName));

    listOfGamers.remove(userName);
}

void MatchMakerEngine::startGame(const QString &gamer1, const QString &gamer2, const Gamer::GameType &gameType)
{
    const QStringList arguments{Gamer::gameTypeToString(gameType), gamer1, gamer2};
    auto process = new QProcess;

    // Connect finished signal
    QObject::connect(process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
                     this, [=](int exitCode, QProcess::ExitStatus exitStatus)
                     {
                         slot_gameFinished(exitCode, exitStatus, gamer1, gamer2);
                     });

    // not needed, anyway we handle issues on game finished signal/slot
//    // Connect errorOccurred signal for crash handling
//    QObject::connect(process, &QProcess::errorOccurred, this,
//                     [=](QProcess::ProcessError error)
//                     {
//                         slot_gameCrashed(error, gamer1, gamer2);
//                     });

    process->start(gameToExecPath->value(gameType), arguments);
}

void MatchMakerEngine::usersListFilterTextChanged(const QString &pattern)
{
    Q_ASSERT(usersListProxyModel);

    auto userModel = dynamic_cast<UsersListModel *>(usersListProxyModel->sourceModel());
    userModel->removeLastRow();

    QRegularExpression regexp(pattern, QRegularExpression::CaseInsensitiveOption);
    if (regexp.isValid()) {
        usersListProxyModel->setFilterRegularExpression(regexp);
    }
    else {
        usersListProxyModel->setFilterRegularExpression(QRegularExpression());
    }

    userModel->addSpecialRow();
    usersListProxyModel->invalidate();
}

void MatchMakerEngine::slot_gamerStateChanged(const QString &userName, const Gamer::GamerState &newState)
{
    Q_ASSERT(usersListProxyModel);

    auto userModel = dynamic_cast<UsersListModel *>(usersListProxyModel->sourceModel());
    auto updatedState = newState;

    if (newState == Gamer::Waiting) {
        const auto &opponent = matching(userName);
        if (opponent.first) { // matched
            const QString &secondUserName = opponent.first->getGamerName();
            const Gamer::GameType &gameType = opponent.second;

            listOfGamers[userName]->gameStart(gameType); // first player
            listOfGamers[secondUserName]->gameStart(gameType); // second player

            updatedState = Gamer::Busy;
            userModel->changeUserState(secondUserName, Gamer::Busy);

            startGame(userName, secondUserName, gameType);

//            qDebug() << "Type: " << Gamer::gameTypeToString(gameType) << " Start game: " << userName << " rating: "
//                     << listOfGamers[userName]->getGameToRatingMap()[gameType] << " vs "
//                     << secondUserName << " rating: " << listOfGamers[secondUserName]->getGameToRatingMap()[gameType];
        }
    }

    userModel->changeUserState(userName, updatedState);
}

void MatchMakerEngine::slot_gamerRatingChanged(const QString &userName, const Gamer::GameType &game,
                                               const Gamer::GamerState &newState, unsigned int newRating)
{
    Q_ASSERT(usersListProxyModel);
    Q_ASSERT(dashboardModel);

    databaseHelper->updateRating(userName, Gamer::gameTypeToString(game), newRating);

    auto userModel = dynamic_cast<UsersListModel *>(usersListProxyModel->sourceModel());
    userModel->changeUserState(userName, newState);

    dashboardModel->changeUserGameRating(Gamer::gameTypeToString(game), userName, newRating);
}

void MatchMakerEngine::slot_gameFinished(int exitCode, QProcess::ExitStatus exitStatus,
                                         const QString &player1, const QString &player2)
{
//    qDebug() << "slot_gameFinished. exitCode: " << exitCode << " exitStatus: " << exitStatus <<
//             " user1: " << player1 << " vs " << " user2: " << player2;

    auto process = qobject_cast<QProcess *>(sender());
    bool processHasError = !process;

    // Validate exit code and exit status
    if (!processHasError && (exitStatus != QProcess::NormalExit || exitCode != 0)) {
        qWarning() << "Game process exited abnormally. Exit status:" << exitStatus << " Exit code:" << exitCode;
        processHasError = true;
    }

    if (!processHasError) {
        const auto &processOutput = process->readAllStandardOutput();
        const auto &outputLines = QString(processOutput).split('\n', Qt::SkipEmptyParts);

        // Ensure output has expected number of lines
        if (outputLines.size() != 1 && outputLines.size() != 2) {
            qWarning() << "Unexpected game output format: " << processOutput;
            processHasError = true;
        }
        else {
            // Ideally we should have compared player1 and player2 with outputLines[0] and/or outputLines[1] too,
            // and if they didn't match, show a warning message as well

            // Handle game outcome based on output lines
            if (outputLines.size() == 1) // Winner
            {
                const QString &winner = outputLines[0].trimmed();
                safeGameOver(player1, !player1.compare(winner),
                             player2, !player2.compare(winner));
            }
            else // Draw
            {
                safeGameOver(player1, false, player2, false);
            }
        }
    }

    // something went wrong
    if (processHasError) {
        safeGameOver(player1, false, player2, false);
    }

    if (process) {
        process->deleteLater();
    }
}

void MatchMakerEngine::safeGameOver(const QString &player1, const bool &win1,
                                    const QString &player2, const bool &win2)
{
    auto itEnd = listOfGamers.end();

    auto it1 = listOfGamers.find(player1);
    if (it1 != itEnd) {
        it1.value()->gameOver(win1);
    }

    auto it2 = listOfGamers.find(player2);
    if (it2 != itEnd) {
        it2.value()->gameOver(win2);
    }
}

//void MatchMakerEngine::slot_gameCrashed(QProcess::ProcessError error,
//                                        const QString &player1, const QString &player2)
//{
//    qWarning() << "Game process crashed with error:" << error;
//
//    listOfGamers[player1]->gameOver(false);
//    listOfGamers[player2]->gameOver(false);
//
//    auto process = qobject_cast<QProcess *>(sender());
//    if (process) {
//        process->deleteLater();
//    }
//}

// TODO: ?
// Considering running the matching algorithm on a separate thread to prevent blocking the main thread.
// However, using mutexes for data protection might affect performance.
// It's important to balance the benefits of parallelism with synchronization overhead.
// Profiling and benchmarking can guide the decision for the most suitable approach.
QPair<std::shared_ptr<Gamer>, Gamer::GameType> MatchMakerEngine::matching(const QString &userName) const
{
    // Find the gamer for the provided username
    auto gamerIt = listOfGamers.find(userName);
    if (gamerIt == listOfGamers.end()) {
        Q_ASSERT(false); // Handle invalid username
        return {};
    }

    // random matching
    QVector<QPair<std::shared_ptr<Gamer>, Gamer::GameType>> matchedGames;

    // Find the first compatible opponent based on availability and game compatibility
    for (auto opponentIt = listOfGamers.constBegin();
         opponentIt != listOfGamers.constEnd(); ++opponentIt) {
        const auto &opponent = opponentIt.value();
        if (opponentIt == gamerIt || opponent->getCurrentState() != Gamer::Waiting) {
            continue;
        }

        // Find a game where both gamers have compatible ratings
        QHash<Gamer::GameType, unsigned int>::const_iterator it;
        for (it = gamerIt.value()->getGameToRatingMap().constBegin();
             it != gamerIt.value()->getGameToRatingMap().constEnd(); ++it) {
            const Gamer::GameType &gameType = it.key();
            const long gamerRating = it.value();
//            qDebug() << "Gamer_1: " << userName << " game: " << Gamer::gameTypeToString(gameType) << " rating: " << gamerRating;
            auto opponentRatingIt = opponent->getGameToRatingMap().find(gameType);
            if (opponentRatingIt != opponent->getGameToRatingMap().end()) { // Both have the game
                const long opponentRating = opponentRatingIt.value();
//                qDebug() << "Gamer_2: " << opponent->getGamerName() << " game: " << Gamer::gameTypeToString(opponentRatingIt.key()) << " rating: " << opponentRatingIt.value();
                //auto ratingsDiff = qFabs(opponentRating - gamerRating);
                const long ratingsDiff = opponentRating - gamerRating;
                if (ratingsDiff >= -1 && ratingsDiff <= 3) {
                    // Return the first compatible opponent and game type
                    //return std::make_pair(opponent, gameType);
                    matchedGames.append(std::make_pair(opponent, gameType));

//                    qDebug() << Gamer::gameTypeToString(gameType) << " User1: " << userName << " rating: "
//                             << gamerRating << " User2: " << opponent->getGamerName() << " rating: "
//                             << opponent->getGameToRatingMap()[gameType];
                }
            }
        }
    }

    if (!matchedGames.isEmpty()) {
        return matchedGames.at(QRandomGenerator::global()->bounded(matchedGames.size()));
    }

    // No suitable opponent found
    return {};
}
