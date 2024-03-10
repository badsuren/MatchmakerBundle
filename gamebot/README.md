# gamebot Console App

## Overview
gamebot is a console application featuring a bot simulator that autonomously plays various games. 
The available games include Tic-Tac-Toe, Rock-Paper-Scissors, and Dice. 
The bot simulator provides an automated gaming experience, 
showcasing the strategies and outcomes of different games.

### Game Result
It accepts two usernames as an input. When the game finishes, it outputs the username of the winner.
If it’s a draw, it outputs usernames of both players.

## Getting Started
### Prerequisites
- C++ compiler
- Qt library (if not already installed)

### Building the Application
1. Clone the repository: `git clone https://github.com/your-username/gamebot.git`
2. Navigate to the project directory: `cd gamebot`
3. Run qmake to generate the Makefile: `qmake`
4. Build the application: `make`

### Running the Simulator
To initiate the bot simulator, use the following command-line options:

```console
~$ ./gameBot <game_type> <user_name1> <user_name2>
```

Replace `<game_type>`, `<user_name1>`, and `<user_name2>` with appropriate values.

### Game Types
- `RPC`: Rock-Paper-Scissors game
- `Dice`: Dice game
- `XO3`: Tic-Tac-Toe game
