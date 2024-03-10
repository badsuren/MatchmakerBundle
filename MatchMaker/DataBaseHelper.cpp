#include "DatabaseHelper.h"

#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>

DataBaseHelper::DataBaseHelper(QObject *parent)
    : QObject(parent), sqliteDatabase{QSqlDatabase::addDatabase("QSQLITE")}
{

}

DataBaseHelper::~DataBaseHelper()
{
    sqliteDatabase.close();
}

bool DataBaseHelper::setupDataBase(const QString &path)
{
    sqliteDatabase.setDatabaseName(path);
    if (!sqliteDatabase.open()) {
        return false;
    }

    return setupTables();
}

bool DataBaseHelper::setupTables()
{
    const QStringList tables = sqliteDatabase.tables();
    QSqlQuery query(sqliteDatabase);
    if (!tables.contains("User")) {
        constexpr auto createUserTableQuery =
            "CREATE TABLE IF NOT EXISTS User ( "
            // VARCHAR(16) - SQLite treats TEXT and VARCHAR as interchangeable
            "username TEXT PRIMARY KEY NOT NULL,"
            "firstname TEXT NOT NULL, "
            "lastname TEXT NOT NULL, "
            "UNIQUE(username));";

        // Create User table
        if (!query.exec(createUserTableQuery)) {
            qCritical() << "Error: Unable to create 'User' table. SQL error: " << query.lastError().text();
            return false;
        }
    }

    // TODO
    // I believe using the Game table is a sound design choice.
    if (!tables.contains("Game")) {
        constexpr auto createGameTableQuery =
            "CREATE TABLE IF NOT EXISTS Game ( "
            "gamename TEXT PRIMARY KEY NOT NULL, "
            "UNIQUE(gamename));";

        // Create Game table
        if (!query.exec(createGameTableQuery)) {
            qCritical() << "Error: Unable to create 'Game' table. SQL error: " << query.lastError().text();
            return false;
        }

        // Add predefined games into Game table
        const auto addGamesQuery = "insert into Game VALUES('XO3'), ('RPS'), ('Dice');";

        // Add games
        if (!query.exec(addGamesQuery)) {
            qCritical() << "Error: Unable to add games to 'Game' table. SQL error: " << query.lastError().text();
            return false;
        }
    }

    if (!tables.contains("UserGame")) {
        constexpr auto createUserGameTableQuery =
            "CREATE TABLE IF NOT EXISTS UserGame ( "
            "username TEXT NOT NULL, "
            "gamename TEXT NOT NULL, "
            "rating INTEGER NOT NULL, "
            "PRIMARY KEY (username, gamename), "
            "FOREIGN KEY (username) REFERENCES user(username), "
            "FOREIGN KEY (gamename) REFERENCES game(gamename), "
            "UNIQUE(username, gamename));";

        // Create UserGame table
        if (!query.exec(createUserGameTableQuery)) {
            qCritical() << "Error: Unable to create 'UserGame' table. SQL error: " << query.lastError().text();
            return false;
        }
    }

    return true;
}

bool DataBaseHelper::addNewUser(const QString &userName, const QString &firstName,
                                const QString &lastName, const QStringList &preferredGames)
{
    if (!sqliteDatabase.transaction()) {
        qCritical() << "Error: Unable to begin a transaction on the database.";
        return false;
    }

    QString queryStr = "insert into User VALUES(:userName, :firstName, :lastName)";
    QSqlQuery query(sqliteDatabase);
    query.prepare(queryStr);
    query.bindValue(":userName", userName);
    query.bindValue(":firstName", firstName);
    query.bindValue(":lastName", lastName);
    query.exec();

    for (const QString &preferredGame: preferredGames) {
        queryStr = "insert into UserGame VALUES(:userName, :preferredGame, 0)";
        query.prepare(queryStr);
        query.bindValue(":userName", userName);
        query.bindValue(":preferredGame", preferredGame);
        query.exec();
    }

    if (query.lastError().text().isEmpty()) {
        sqliteDatabase.commit();

        return true;
    }
    else {
        sqliteDatabase.rollback();
        qCritical() << "Error: addNewUser function call is failed. SQL error: " << query.lastError().text();

        return false;
    }
}

bool DataBaseHelper::deleteUser(const QString &userName)
{
    if (!sqliteDatabase.transaction()) {
        qCritical() << "Error: Unable to begin a transaction on the database.";
        return false;
    }

    QSqlQuery query(sqliteDatabase);
    QString queryStr = "delete from UserGame where username=:userName";
    query.prepare(queryStr);
    query.bindValue(":userName", userName);
    query.exec();

    queryStr = "delete from User where username=:userName";
    query.prepare(queryStr);
    query.bindValue(":userName", userName);
    query.exec();

    if (query.lastError().text().isEmpty()) {
        sqliteDatabase.commit();

        return true;
    }
    else {
        sqliteDatabase.rollback();
        qCritical() << "Error: deleteUser function call is failed. Error: " << query.lastError().text();

        return false;
    }
}

void DataBaseHelper::updateRating(const QString &userName, const QString &game, unsigned int newRating)
{
    const QString updateRatingQueryStr =
        "UPDATE UserGame  SET rating=:newRating "
        "WHERE username=:userName "
        "AND gamename=:game";

    QSqlQuery query(sqliteDatabase);
    query.prepare(updateRatingQueryStr);
    query.bindValue(":newRating", newRating);
    query.bindValue(":userName", userName);
    query.bindValue(":game", game);
    query.exec();

    if (!query.lastError().text().isEmpty()) {
        qCritical() << "Error: Cannot update rating field in UserGame table. SqLite error:" << query.lastError().text();
    }
}

bool DataBaseHelper::getDashboardData(QSqlQuery &query) const
{
    constexpr auto queryStr = "select * from UserGame";

    if (!query.exec(queryStr)) {
        qCritical() << "Error: Cannot get UserGame table data. SqLite error:" << query.lastError().text();

        return false;
    }

    return true;
}

bool DataBaseHelper::getUsersListData(QSqlQuery &query) const
{
    constexpr auto userTableQuery =
        "SELECT User.username AS 'Username', "
        "user.firstname AS 'First name', user.lastname AS 'Last name', "
        "GROUP_CONCAT(UserGame.gamename, ', ') AS 'Preferred games' FROM user "
        "LEFT JOIN UserGame ON User.username = UserGame.username GROUP BY User.username;";

    if (!query.exec(userTableQuery)) {
        qCritical() << "Error: Unable to select data from 'User' and 'UserGame' tables. SqLite error:"
                    << query.lastError().text();

        return false;
    }

    return true;
}
