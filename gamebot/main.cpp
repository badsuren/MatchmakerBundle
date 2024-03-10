#include <QCoreApplication>

#include "IGame.h"
#include "TicTacToe.h"
#include "RockPaperScissors.h"
#include "Dice.h"
#include "GameContext.h"

int main(int argc, char *argv[])
{
    // Validate the args
    if (argc < 2 || (argc == 2 && QString(argv[1]).toLower() == "--help")) {
        // If no args or --help
        QTextStream(stdout) << "Usage: " << argv[0] << " <game_type> <user_name1> <user_name2>\n";
        QTextStream(stdout) << "Available game types:\n";
        QTextStream(stdout) << "    XO3 - TicTacToe\n";
        QTextStream(stdout) << "    RPS - RockPaperScissors\n";
        QTextStream(stdout) << "    Dice - Dice\n";

        return 0;
    }

    if (argc != 4) {
        QTextStream(stderr) << "Error: Wrong number of arguments. Use --help for usage information.\n";
        return 1;
    }

    // Instance should be created based on the game type
    std::unique_ptr<gamebot::IGame> instance;

    const QString game_type = QString(argv[1]).toLower();

    if (game_type == "xo3") {
        instance = std::make_unique<gamebot::TicTacToe>();
    } else if (game_type == "rps") {
        instance = std::make_unique<gamebot::RockPaperScissors>();
    } else if (game_type == "dice") {
        instance = std::make_unique<gamebot::Dice>();
    } else {
        QTextStream(stderr) << "Error: Invalid game type. Supported types: XO3, RPS, Dice.\n";
        return 1;
    }

    gamebot::GameContext game(std::move(instance));
    // Play the selected game
    game.execute(argv[2], argv[3]);
}
