#include "UsersListProxyModel.h"

UsersListProxyModel::UsersListProxyModel(QObject *parent)
    : QSortFilterProxyModel{parent}
{
}

QVariant UsersListProxyModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Vertical && role == Qt::DisplayRole) {
        if (section == QSortFilterProxyModel::rowCount() - 1) {
            return "";
        }

        return section + 1;
    }

    return QSortFilterProxyModel::headerData(section, orientation, role);
}

bool UsersListProxyModel::lessThan(const QModelIndex &left, const QModelIndex &right) const
{
    // skip +Add User row
    int lastRowNum = sourceModel()->rowCount() - 1;
    if ((left.isValid() && left.row() == lastRowNum) ||
        (right.isValid() && right.row() == lastRowNum)) {
        return false;
    }

    return QSortFilterProxyModel::lessThan(left, right);
}

bool UsersListProxyModel::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
    Q_UNUSED(source_parent)

    // "+Add User" - always should be the last row
    if (source_row == sourceModel()->rowCount() - 1) {
        // Do not filter
        return true;
    }

    return QSortFilterProxyModel::filterAcceptsRow(source_row, source_parent);
}
