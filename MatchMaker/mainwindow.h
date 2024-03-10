#pragma once

#include "DatabaseHelper.h"
#include "MatchMakerEngine.h"

#include <QMainWindow>
#include <QMenu>

QT_BEGIN_NAMESPACE
namespace Ui
{
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow: public QMainWindow
{
Q_OBJECT

public:
    explicit MainWindow(std::unique_ptr<DataBaseHelper> dbHelper,
                        std::unique_ptr<QHash<Gamer::GameType, QString>> gamesPaths,
                        QWidget *parent = nullptr);

    ~MainWindow() override;

private slots:
    void on_action_Add_User_triggered();
    void on_action_Remove_User_triggered();
    void on_actionShow_Hide_Dashboard_triggered();
    void on_actionShow_Hide_User_List_triggered();
    void on_actionSave_the_Dashboard_to_File_triggered();
    void on_lineEdit_username_filter_textChanged(const QString &arg1);
    void on_pushButton_filter_clicked();
    void on_tableView_users_list_clicked(const QModelIndex &index);

    void tableView_users_list_sortIndicatorChanged(int col, Qt::SortOrder);
    void editMenuAboutToShow();
    void usersListCustomMenuRequested(const QPoint &pos);

private:
    Ui::MainWindow *ui;
    std::unique_ptr<MatchMakerEngine> controller{};

    void changeViewDashboardMenuText(const bool &bShow);
    void changeViewUserListMenuText(const bool &bShow);
    void spanSpecialRow();
    [[nodiscard]] QModelIndex usersListSelectedRow() const; // get selected row
    void setupMatchMaker(std::unique_ptr<DataBaseHelper> dbHelper,
                         std::unique_ptr<QHash<Gamer::GameType, QString>> gamesPaths);
};
