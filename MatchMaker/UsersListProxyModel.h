#pragma once

#include <QAbstractTableModel>
#include <QSortFilterProxyModel>
#include <QSqlQueryModel>

class UsersListProxyModel: public QSortFilterProxyModel
{
Q_OBJECT

public:
    explicit UsersListProxyModel(QObject *parent = nullptr);
    ~UsersListProxyModel() override = default;

    [[nodiscard]] QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    [[nodiscard]] bool lessThan(const QModelIndex &left, const QModelIndex &right) const override;
    [[nodiscard]] bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const override;
};
