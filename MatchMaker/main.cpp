#include "mainwindow.h"
#include "DatabaseHelper.h"

#include <QApplication>
#include <QDir>
#include <QCommandLineParser>

void initCommandLineOptions(QCommandLineParser &parser)
{
    parser.setApplicationDescription("Match Maker Tool");
    parser.addVersionOption();
    parser.addHelpOption();

    // Required
    QCommandLineOption xo3Option("xo3", "Path to XO3 executable (mandatory)", "xo3");
    QCommandLineOption rpsOption("rps", "Path to RPS executable (mandatory)", "rps");
    QCommandLineOption diceOption("dice", "Path to Dice executable (mandatory)", "dice");
    parser.addOption(xo3Option);
    parser.addOption(rpsOption);
    parser.addOption(diceOption);

    // Optional
    QCommandLineOption dbOption("db", "Path to SqLite database (optional)", "db");
    dbOption.setDefaultValue(QDir::currentPath() + QDir::separator() + "matchmaker.db");
    parser.addOption(dbOption);
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QCommandLineParser parser;
    // Define command line options
    initCommandLineOptions(parser);

    // Parse command line arguments
    parser.process(a);

    // Check for missing required arguments
    if (!parser.isSet("xo3") || !parser.isSet("rps") || !parser.isSet("dice")) {
        qWarning() << "Missing required arguments. Please provide paths for XO3, Dice, and RPS executables.";
        parser.showHelp();
    }

    // Get executable paths
    QString xo3Path = parser.value("xo3");
    QString rpsPath = parser.value("rps");
    QString dicePath = parser.value("dice");

    // Check if paths are valid executables
    QFileInfo xo3FileInfo(xo3Path);
    QFileInfo rpsFileInfo(rpsPath);
    QFileInfo diceFileInfo(dicePath);

    if (!xo3FileInfo.isExecutable() || !rpsFileInfo.isExecutable() || !diceFileInfo.isExecutable()) {
        qWarning() << "Invalid executable paths provided. Please ensure all paths point to valid executables.";
        return EXIT_FAILURE;
    }

    std::unique_ptr<DataBaseHelper> dbHelper = std::make_unique<DataBaseHelper>();
    if (!dbHelper->setupDataBase(parser.value("db"))) {
        qWarning() << "Error occurred while trying to make a connection to the database.";
        return EXIT_FAILURE;
    }

    std::unique_ptr<QHash<Gamer::GameType, QString>> gamesPaths =
        std::make_unique<QHash<Gamer::GameType, QString>>();
    gamesPaths->insert(
        {
            {Gamer::GameType::XO3, xo3Path},
            {Gamer::GameType::RPS, rpsPath},
            {Gamer::GameType::Dice, dicePath}
        });

    MainWindow w(std::move(dbHelper), std::move(gamesPaths));
    w.show();

    return QApplication::exec();
}
