# UNO Game

A simple command-line UNO game implemented in C.

## Table of Contents

- [Introduction](#introduction)
- [Features](#features)
- [Installation](#installation)
- [Usage](#usage)
- [Project Structure](#project-structure)
- [Compilation](#compilation)
- [Gameplay Instructions](#gameplay-instructions)
- [Contributing](#contributing)
- [License](#license)

## Introduction

This project is an implementation of the classic card game UNO in C. It supports multiple players and incorporates essential game mechanics such as skip, reverse, draw two, and wild cards. The game is played via the command line, allowing users to interact and enjoy a turn-based experience.

## Features

- Supports 2 to 10 players.
- Implements core UNO game rules.
- Randomized deck shuffling.
- Handles special cards like:
  - Skip
  - Reverse
  - Draw Two
  - Wild
  - Wild Draw Four
- Dynamic game loop with turn-based logic.

## Installation

To install and run the game, follow these steps:

1. Clone the repository:
```
 git clone https://github.com/TheAditya700/uno_game
 cd uno_game
```

2. Ensure you have `gcc` installed:
```
 gcc --version
```

3. Compile the project using `make`.

## Usage

After compiling the project, run the executable:
```
 ./uno_game
```

## Project Structure

The project is organized into the following files:

```
uno_game/
├── main.c       # Entry point of the program
├── uno_game.c   # Core game logic
├── uno_game.h   # Game-related definitions
├── Makefile     # Compilation script
├── README.md    # Documentation
```

## Compilation

To compile the project, run the following command:
```
 make
```

To clean the build files:
```
 make clean
```

## Gameplay Instructions

1. Launch the game by running `./uno_game`.
2. Enter the number of players (between 2 and 10).
3. Each player is prompted to enter their name.
4. The game starts with each player drawing 7 cards.
5. Players take turns playing cards that match the top of the discard pile by color or number.
6. Special cards affect the game:
   - **Skip:** Skips the next player's turn.
   - **Reverse:** Changes the play direction.
   - **Draw Two:** Forces the next player to draw two cards.
   - **Wild:** Allows the player to choose the next color.
   - **Wild Draw Four:** Forces the next player to draw four cards and allows color selection.
7. The game continues until a player has no cards left.
8. Players must call 'UNO' when they have only one card left.

## Contributing

Contributions are welcome! If you'd like to improve the project:

1. Fork the repository.
2. Create a feature branch.
3. Submit a pull request.
