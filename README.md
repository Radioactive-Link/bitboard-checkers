# Project 1: Bitboard Checkers

CLI Checkers in C. The game board is represented by 64-bit integers. Play against an AI that makes random moves.

## Features

This game implements the following features from checkers:

- Moving
  - If a capture is available, it must be taken
- Kinging
- Capturing (single only; not double or more)
    - Only kings can capture other kings
- Game winning
- Stalemates

## Prerequisites

Ensure that you have gcc installed. My preferred method is via [chocolatey](https://chocolatey.org/install#individual).

```powershell
choco install mingw
```

> Note: I am using a windows system for building and running, so these instructions make no guarantee for other systems.

## Building

Build the application with:

```powershell
gcc main.c -o checkers
```

Alternatively, the Makefile can be used:

```powershell
choco install make
make game # build
```

## Running

Run the application from the binary produced in the previous step.

```powershell
./checkers
```

## Playing

Most everything plays as a normal checkers game. Anything unique to this implementation is described below.

### Moving

You will be prompted to pick a piece and then pick where you want to move it to. This is done by entering the column and row for each set of coordinates. The starting game board is shown below.

```
+-----+-------------------------------+
| h > |   | S |   | S |   | S |   | S |
|~ ~ ~+-------------------------------+
| g > | S |   | S |   | S |   | S |   |
|~ ~ ~+-------------------------------+
| f > |   | S |   | S |   | S |   | S |
|~ ~ ~+-------------------------------+
| e > | # |   | # |   | # |   | # |   |
|~ ~ ~+-------------------------------+
| d > |   | # |   | # |   | # |   | # |
|~ ~ ~+-------------------------------+
| c > | A |   | A |   | A |   | A |   |
|~ ~ ~+-------------------------------+
| b > |   | A |   | A |   | A |   | A |
|~ ~ ~+-------------------------------+
| a > | A |   | A |   | A |   | A |   |
|~ ~ ~+-------------------------------+
|      ^a  ^b  ^c  ^d  ^e  ^f  ^g  ^h |
+=====================================+
```

As an example, if you (Black pieces - 'A's) wanted to move your top right piece to the right edge, then you would enter in 'gchd' while hitting enter between every character. Note that input is case sensitive.

> __Caution:__ Due to how input is read, hitting enter is not needed between characters, but is recommended for a cleaner user experience. Otherwise, characters entered remain in the input buffer and will be automatically read before you can input new ones.

Once you are familiar with the input system, a move can be made all at once by typing in all four characters first then hitting enter.

### The AI

In this game, you play against a simple AI that makes random valid moves. After your turn is submitted, the AI will make their move and the updated board will be shown. From there, you will be prompted to make your move again and the cycle will continue until the game is either won, drawn, or the program is exited. If any error occurs (like an invalid move was input), then a message box will be displayed over the game board or after the input prompt as you are prompted to make another move.