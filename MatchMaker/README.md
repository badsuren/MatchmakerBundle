# Match Maker

A GUI tool that organizes game matches, shows player ratings, and stores data for retrieval.

## Table of Contents

Description: #description
Installation: #installation
Usage: #usage
Supported Games: #supported-games
Dashboard: #dashboard
Data Storage: #data-storage
## Description

Match Maker is a versatile tool designed to:

Organize game matches for players across various games.
Display real-time player ratings for each game on a user-friendly dashboard.
Persistently store game and player data for retrieval upon subsequent launches.
## Installation

Prerequisites:

Qt framework (version 5 or newer)
Executables for supported games (XO3, RPS, Dice)
Build and run:

Clone or download this repository.
Open the project in a Qt IDE (e.g., Qt Creator).
Build and run the project.
## Usage

Launch the application.
Provide paths to game executables via command-line arguments.
Usage: MatchMaker.exe --xo3 <path/to/xo3> --rps <path/to/rps> --dice <path/to/dice>
Optionally, specify a custom database path:
Usage: MatchMaker.exe --db <path/to/database>
## Supported Games

XO3 (Tic-Tac-Toe)
RPS (Rock-Paper-Scissors)
Dice
## Dashboard

Visualizes player ratings for each supported game in real-time.
## Data Storage

Utilizes an SQLite database to persistently store:
Player information
Player ratings
Data is preserved for future use upon application restarts.