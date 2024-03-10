#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "UsersListProxyModel.h"
#include "adduserdialog.h"
#include "DatabaseHelper.h"
#include "DashboardModel.h"
#include "UsersListModel.h"

#include <QSqlQueryModel>
#include <QMessageBox>
#include <QStandardItemModel>
#include <QFileDialog>

MainWindow::MainWindow(std::unique_ptr<DataBaseHelper> dbHelper,
                       std::unique_ptr<QHash<Gamer::GameType, QString>> gamesPaths,
                       QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setupMatchMaker(std::move(dbHelper), std::move(gamesPaths));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setupMatchMaker(std::unique_ptr<DataBaseHelper> dbHelper,
                                 std::unique_ptr<QHash<Gamer::GameType, QString>> gamesPaths)
{
    auto usersListProxyModel = new UsersListProxyModel;
    usersListProxyModel->setSourceModel(new UsersListModel());
    auto dashboardModel = new DashboardModel;

    controller = std::make_unique<MatchMakerEngine>(std::move(dbHelper),
                                                    std::move(gamesPaths),
                                                    usersListProxyModel,
                                                    dashboardModel, this);
    controller->populateData();

    ui->treeView_dashboard->setModel(dashboardModel);
    ui->treeView_dashboard->header()->setSectionResizeMode(QHeaderView::Stretch);
    ui->treeView_dashboard->expandAll();

    ui->tableView_users_list->setModel(usersListProxyModel);

    spanSpecialRow();

    ui->tableView_users_list->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    changeViewDashboardMenuText(true);
    changeViewUserListMenuText(true);

    connect(ui->menuEdit, &QMenu::aboutToShow, this, &MainWindow::editMenuAboutToShow);
    connect(ui->tableView_users_list->horizontalHeader(), &QHeaderView::sortIndicatorChanged,
            this, &MainWindow::tableView_users_list_sortIndicatorChanged);
    connect(ui->tableView_users_list, &QTableView::customContextMenuRequested,
            this, &MainWindow::usersListCustomMenuRequested);
}

void MainWindow::on_action_Add_User_triggered()
{
    AddUserDialog dlg(this);

    if (dlg.exec() != QDialog::Accepted) {
        return;
    }

    if (!controller->addNewUser(dlg.getUserName(), dlg.getFirstName(),
                                dlg.getLastName(), dlg.getPreferredGames())) {
        QMessageBox::critical(this, "Unable to add the user.",
                              "Please ensure that the username does not already exist.\n"
                              "Click Cancel to exit.",
                              QMessageBox::Cancel);
    }
}

void MainWindow::on_action_Remove_User_triggered()
{
    // Check if a row is selected
    const QModelIndex selectedRow = usersListSelectedRow();
    const QAbstractItemModel *model = selectedRow.model();
    if (!selectedRow.isValid() || !model) {
        QMessageBox::warning(this, "Warning", "No user row is selected for deletion.");
        return;
    }

    // Prompt the user for confirmation
    QMessageBox::StandardButton result = QMessageBox::question(
        this, "Confirmation", "Are you sure you want to delete this user?",
        QMessageBox::Yes | QMessageBox::No);

    if (result != QMessageBox::Yes) {
        return;
    }

    const QVariant &data = model->data(model->index(selectedRow.row(), 0), Qt::DisplayRole);
    if (!controller->deleteUser(data.toString())) {
        QMessageBox::critical(this, "Unable to delete the user.",
                              "Something went wrong.\nClick Cancel to exit.",
                              QMessageBox::Cancel);
    }
}

void MainWindow::on_actionShow_Hide_Dashboard_triggered()
{
    const bool bShow = !ui->treeView_dashboard->isVisible();

    ui->treeView_dashboard->setVisible(bShow);

    changeViewDashboardMenuText(bShow);
}

void MainWindow::on_actionShow_Hide_User_List_triggered()
{
    const bool bShow = !ui->tableView_users_list->isVisible();

    ui->tableView_users_list->setVisible(bShow);
    ui->label_username_filter->setVisible(bShow);
    ui->lineEdit_username_filter->setVisible(bShow);
    ui->pushButton_filter->setVisible(bShow);

    changeViewUserListMenuText(bShow);
}

void MainWindow::on_actionSave_the_Dashboard_to_File_triggered()
{
    const QString &filePath = QFileDialog::getSaveFileName(
        nullptr, "Export Data to CSV", "dashboard.csv", "CSV Files (*.csv)");

    if (!filePath.isEmpty()) {
        controller->exportCSV(filePath);
    }
}

void MainWindow::on_lineEdit_username_filter_textChanged(const QString &pattern)
{
    controller->usersListFilterTextChanged(pattern);
    spanSpecialRow();
}

// not needed
void MainWindow::on_pushButton_filter_clicked()
{
    const auto pattern = ui->lineEdit_username_filter->text();
    on_lineEdit_username_filter_textChanged(pattern);
}

void MainWindow::on_tableView_users_list_clicked(const QModelIndex &index)
{
    if (index.isValid() && index.row() == controller->getUsersListRowCount() - 1) {
        on_action_Add_User_triggered();
    }
}

void MainWindow::tableView_users_list_sortIndicatorChanged(int col, Qt::SortOrder)
{
    // Skip sorting for the 'Preferred games' column
    ui->tableView_users_list->setSortingEnabled(col != UsersListModel::HeaderColumns::PreferredGames
    && col != UsersListModel::HeaderColumns::State);
}

void MainWindow::editMenuAboutToShow()
{
    ui->action_Remove_User->setEnabled(usersListSelectedRow().isValid() &&
        usersListSelectedRow().row() != controller->getUsersListRowCount() - 1);
}

void MainWindow::usersListCustomMenuRequested(const QPoint &pos)
{
    auto modelIndex = ui->tableView_users_list->indexAt(pos);

    QMenu contextMenu;

    // Add actions to the context menu
    QAction *actionAddUser = contextMenu.addAction("Add User");
    QAction *actionDeleteUser = contextMenu.addAction("Remove User");
    actionDeleteUser->setEnabled(modelIndex.isValid() && modelIndex.row() != controller->getUsersListRowCount() - 1);

    connect(actionAddUser, &QAction::triggered,
            this, &MainWindow::on_action_Add_User_triggered);
    connect(actionDeleteUser, &QAction::triggered,
            this, &MainWindow::on_action_Remove_User_triggered);

    // Show the context menu at the specified position
    contextMenu.exec(ui->tableView_users_list->viewport()->mapToGlobal(pos));

    // Note: There is a known issue causing the following warning on macOS.
    // It is not impacting the functionality, just ignore:
    // The cached device pixel ratio value was stale on window expose.
    // Please file a QT BUG which explains how to reproduce.
}

void MainWindow::changeViewDashboardMenuText(const bool &bShow)
{
    ui->menuView->actions().at(0)->setText(bShow ? "Hide Dashboard" : "Show Dashboard");
}

void MainWindow::changeViewUserListMenuText(const bool &bShow)
{
    ui->menuView->actions().at(1)->setText(bShow ? "Hide User List" : "Show User List");
}

void MainWindow::spanSpecialRow()
{
    ui->tableView_users_list->setSpan(controller->getUsersListRowCount() - 1, UsersListModel::FirstName,
                                      1, UsersListModel::Count - UsersListModel::FirstName);
}

QModelIndex MainWindow::usersListSelectedRow() const
{
    // Get the selection model
    const QItemSelectionModel *selectionModel = ui->tableView_users_list->selectionModel();
    if (selectionModel) {
        return selectionModel->currentIndex();
    }

    return {};
}
