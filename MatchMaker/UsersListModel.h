#pragma once

#include "Gamer.h"

#include <QStandardItemModel>
#include <QSqlQuery>

// QSqlQueryModel or QSqlRelationalTableModel can be used as a base class as well,
// but with QStandardItemModel - the implementation is clearer/more preferable
class UsersListModel: public QStandardItemModel
{
Q_OBJECT

public:
    // Enum representing the Headers
    enum HeaderColumns
    {
        Username,
        FirstName,
        LastName,
        PreferredGames,
        State,
        Count
    };

    explicit UsersListModel(QObject *parent = nullptr);
    ~UsersListModel() override = default;

    void changeUserState(const QString &userName, const Gamer::GamerState &state);
    void addUser(const QString &userName, const QString &firstName,
                               const QString &lastName, const QStringList &games);
    void deleteUser(const QString &userName);
    void addSpecialRow();
    void removeLastRow();

    [[nodiscard]] Qt::ItemFlags flags(const QModelIndex &index) const override;

private:
    [[nodiscard]] QStandardItem *findUser(const QString &userName, Qt::MatchFlag flag = Qt::MatchExactly) const;
};
