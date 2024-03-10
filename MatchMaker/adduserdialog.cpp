// You may need to build the project (run Qt uic code generator) to get "ui_AddUserDialog.h" resolved

#include "adduserdialog.h"
#include "ui_AddUserDialog.h"

#include <QRegularExpression>
#include <QRegularExpressionValidator>

AddUserDialog::AddUserDialog(QWidget *parent)
    : QDialog(parent), ui(new Ui::AddUserDialog)
{
    ui->setupUi(this);

    // a string with no spaces and with length of not greater than 16. All letters should be from English alphabet.
    const QRegularExpression user_name_pattern("^[a-zA-Z]{1,16}$");
    auto *validator = new QRegularExpressionValidator(user_name_pattern, this);
    ui->lineEdit_user_name->setValidator(validator);

    // a string that consists of Latin characters only and starts with a capital letter.
    const QRegularExpression first_name_pattern("^[A-Z][a-zA-Z]*$");
    validator = new QRegularExpressionValidator(first_name_pattern, this);
    ui->lineEdit_first_name->setValidator(validator);

    // a string that consists of Latin characters and also can
    // contain a single quote mark (used for last names like O’Hara or d’Cartes).
    const QRegularExpression last_name_pattern("^[A-Za-z]+('[A-Za-z]+)?$");
    validator = new QRegularExpressionValidator(last_name_pattern, this);
    ui->lineEdit_last_name->setValidator(validator);
}

AddUserDialog::~AddUserDialog()
{
    delete ui;
}

void AddUserDialog::canGoAhead()
{
    bool enable = !ui->lineEdit_user_name->text().isEmpty() &&
        !ui->lineEdit_first_name->text().isEmpty() &&
        !ui->lineEdit_last_name->text().isEmpty() &&
        (ui->checkBox_XO3->isChecked() || ui->checkBox_RPS->isChecked() || ui->checkBox_Dice->isChecked());

    ui->pushButton_ok->setEnabled(enable);
}

QString AddUserDialog::getUserName() const
{
    return std::move(ui->lineEdit_user_name->text());
}
QString AddUserDialog::getFirstName() const
{
    return std::move(ui->lineEdit_first_name->text());
}
QString AddUserDialog::getLastName() const
{
    return std::move(ui->lineEdit_last_name->text());
}
QStringList AddUserDialog::getPreferredGames() const
{
    QStringList preferredGames;
    if (ui->checkBox_XO3->isChecked()) {
        preferredGames << ui->checkBox_XO3->text();
    }

    if (ui->checkBox_RPS->isChecked()) {
        preferredGames << ui->checkBox_RPS->text();
    }

    if (ui->checkBox_Dice->isChecked()) {
        preferredGames << ui->checkBox_Dice->text();
    }

    return preferredGames;
}
