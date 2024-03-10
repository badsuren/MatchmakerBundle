#include "DashboardModel.h"

DashboardModel::QRatingItem::QRatingItem(const QString &str)
    : QStandardItem(str)
{

}

DashboardModel::QRatingItem::QRatingItem(const int &value)
    : QStandardItem(QString::number(value))
{

}

bool DashboardModel::QRatingItem::operator<(const QStandardItem &other) const
{
    // Extract numeric values from the data
    bool ok1, ok2;
    int value1 = data(Qt::DisplayRole).toInt(&ok1);
    int value2 = other.data(Qt::DisplayRole).toInt(&ok2);

    // If conversion to int fails, fall back to string comparison
    if (ok1 && ok2) {
        return value1 < value2;
    }

    return QStandardItem::operator<(other);
}

DashboardModel::DashboardModel(QObject *parent)
    : QStandardItemModel(parent)
{
}

void DashboardModel::changeUserGameRating(const QString &gameName, const QString &userName, unsigned int newRating)
{
    auto ratingItem = findGameItemByUser(gameName, userName);
    if (ratingItem != nullptr) {
        ratingItem->setData(newRating, Qt::DisplayRole);
        ratingItem->parent()->sortChildren(HeaderColumns::Rating, Qt::DescendingOrder);
    }
}

void DashboardModel::addUserGames(const QString &userName, const QStringList &games)
{
    for (const QString &game: games) {
        auto parent = findGameItem(game);
        if (parent != nullptr) {
            bool usernameExists = false;
            for (int row = 0; row < parent->rowCount(); ++row) {
                auto *item = parent->child(row, HeaderColumns::Username);
                if (item && item->text() == userName) {
                    usernameExists = true;
                    break;
                }
            }
            if (!usernameExists) {
                // Create child items for username, rating
                auto *usernameItem = new QStandardItem(userName);
                auto *ratingItem = new QStandardItem(QString::number(0));

                // Add new child items to the parent
                parent->appendRow({{}, usernameItem, ratingItem});
            }
        }
    }
}

void DashboardModel::deleteUser(const QString &userName)
{
    for (int parentRow = 0; parentRow < rowCount(); ++parentRow) {
//        QStandardItem *parent = itemFromIndex(index(parentRow, HeaderColumns::Game));
        QStandardItem *parent = item(parentRow, HeaderColumns::Game);
        if (parent) {
            for (int row = 0; row < parent->rowCount(); ++row) {
                auto *item = parent->child(row, HeaderColumns::Username);
                if (item && item->text() == userName) {
                    parent->removeRow(row);
                    break;
                }
            }
        }
    }
}

Qt::ItemFlags DashboardModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags defaultFlags = QStandardItemModel::flags(index);
    return index.isValid() ? (defaultFlags & ~Qt::ItemIsEditable) : defaultFlags;
}

QStandardItem *DashboardModel::findGameItem(const QString &gameName) const
{
    for (int row = 0; row < rowCount(); ++row) {
        QStandardItem *item = itemFromIndex(index(row, HeaderColumns::Game));
        if (item && item->text() == gameName) {
            return item;
        }
    }

    return nullptr;
}

QStandardItem *DashboardModel::findGameItemByUser(const QString &gameName, const QString &userName) const
{
    auto parent = findGameItem(gameName);
    if (parent != nullptr) {
        for (int row = 0; row < parent->rowCount(); ++row) {
            auto *item = parent->child(row, HeaderColumns::Username);
            if (item && item->text() == userName) {
                return parent->child(row, HeaderColumns::Rating);
            }
        }
    }

    return nullptr;
}

