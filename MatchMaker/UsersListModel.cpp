#include "UsersListModel.h"

UsersListModel::UsersListModel(QObject *parent)
    : QStandardItemModel(parent)
{

}

void UsersListModel::changeUserState(const QString &userName, const Gamer::GamerState &state)
{
    auto userItem = findUser(userName);
    if (userItem != nullptr) {
        auto stateColIndex = userItem->index().siblingAtColumn(HeaderColumns::State);
        auto stateColItem = itemFromIndex(stateColIndex);

        if (stateColItem != nullptr) {
            stateColItem->setData(Gamer::gamerStateToString(state), Qt::DisplayRole);
        }
        else {
            qWarning() << "Error: UsersListModel::findUser cannot change state to '" <<
                       Gamer::gamerStateToString(state) << "' for userName: " << userName << " !";
        }
    }
    else {
        qWarning() << "Error: UsersListModel::findUser call for userName: " << userName << " - not found!";
    }
}

void UsersListModel::addUser(const QString &userName, const QString &firstName,
                             const QString &lastName, const QStringList &games)
{
    if (findUser(userName, Qt::MatchFixedString)) {
        Q_ASSERT(false);
    }

    auto *userNameItem = new QStandardItem(userName);
    auto *firstNameItem = new QStandardItem(firstName);
    auto *lastNameItem = new QStandardItem(lastName);
    auto *preferredGamesItem = new QStandardItem(games.join(", "));
    auto *statusItem = new QStandardItem(Gamer::gamerStateToString(Gamer::Free));

    insertRow(rowCount() - 1, QList<QStandardItem *>{userNameItem, firstNameItem,
                                                     lastNameItem, preferredGamesItem, statusItem});
}

void UsersListModel::deleteUser(const QString &userName)
{
    auto userItem = findUser(userName);
    if (userItem == nullptr) {
        Q_ASSERT(false);
    }

    int rowIndex = userItem->row();
    bool result = removeRow(rowIndex);
    if (!result) {
        Q_ASSERT(false);
    }
}

void UsersListModel::addSpecialRow()
{
    // additional row
    auto *addUserItem = new QStandardItem("+Add User");
    appendRow(addUserItem);
}

void UsersListModel::removeLastRow()
{
    removeRow(rowCount() - 1);
}

Qt::ItemFlags UsersListModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags defaultFlags = QStandardItemModel::flags(index);

    if (index.isValid()) {
        return (index.row() == rowCount() - 1) ?
               defaultFlags & ~Qt::ItemIsSelectable : defaultFlags & ~Qt::ItemIsEditable;
    }

    return defaultFlags;
}
QStandardItem *UsersListModel::findUser(const QString &userName, Qt::MatchFlag flag) const
{
    auto items = findItems(userName, flag, HeaderColumns::Username);

    // we should have only one item in the list if user exists
    return items.size() != 1 ? nullptr : items.at(0);
}
