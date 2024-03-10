#pragma once

#include <QObject>
#include <QSqlDatabase>

//
// DAO class to work with SQLITE database
//
class DataBaseHelper: public QObject
{
Q_OBJECT

public:
    explicit DataBaseHelper(QObject *parent = nullptr);
    ~DataBaseHelper() override;

    // Prepare a SQLITE database
    bool setupDataBase(const QString &path);

    bool getDashboardData(QSqlQuery &query) const;
    bool getUsersListData(QSqlQuery &query) const;

    // Add new user
    bool addNewUser(const QString &userName, const QString &firstName,
                    const QString &lastName, const QStringList &preferredGames);
    // Delete user
    bool deleteUser(const QString &userName);

    void updateRating(const QString &userName, const QString &game, unsigned int newRating);

private:
    QSqlDatabase sqliteDatabase;

    // Generate tables if they do not exist
    bool setupTables();
};
