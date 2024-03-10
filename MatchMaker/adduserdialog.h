#pragma once

#include <QDialog>


QT_BEGIN_NAMESPACE
namespace Ui
{
class AddUserDialog;
}
QT_END_NAMESPACE

class AddUserDialog: public QDialog
{
Q_OBJECT

public:
    explicit AddUserDialog(QWidget *parent = nullptr);
    ~AddUserDialog() override;

    [[nodiscard]] QString getUserName() const;
    [[nodiscard]] QString getFirstName() const;
    [[nodiscard]] QString getLastName() const;
    [[nodiscard]] QStringList getPreferredGames() const;

private slots:
    void canGoAhead();

private:
    Ui::AddUserDialog *ui;
};
