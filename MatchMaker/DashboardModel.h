#pragma once

#include <QStandardItemModel>
#include <QSqlQuery>

class DashboardModel: public QStandardItemModel
{
Q_OBJECT

public:
    class QRatingItem: public QStandardItem
    {
    public:
        QRatingItem() = default;
        explicit QRatingItem(const QString &str);
        explicit QRatingItem(const int &value);

        //bool operator<(const QRatingItem &other) const override
        virtual bool operator<(const QStandardItem &other) const override;
    };

    // Enum representing the Headers
    enum HeaderColumns
    {
        Game,
        Username,
        Rating,
        Count
    };

    explicit DashboardModel(QObject *parent = nullptr);
    ~DashboardModel() override = default;

    void changeUserGameRating(const QString &gameName, const QString &userName, unsigned int newRating);
    void addUserGames(const QString &userName, const QStringList &games);
    void deleteUser(const QString &userName);

    [[nodiscard]] Qt::ItemFlags flags(const QModelIndex &index) const override;

    [[nodiscard]] QStandardItem *findGameItem(const QString &gameName) const;

private:
    [[nodiscard]] QStandardItem *findGameItemByUser(const QString &gameName, const QString &userName) const;
};
