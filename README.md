# Blackjack
## The Game

In this game, the player competes against the dealer. The player begins by setting their bet using the number keys and confirming with Enter. Once the bet is placed, the game moves into the main phase where the player chooses to hit or stay:

Press 1 to Hit (draw another card)

Press 2 to Stay (end your turn)

After the player finishes their turn, the dealer plays automatically, drawing cards until reaching 17 or higher.

## How It Works

This game is built using the Win32 API in C, allowing for a fast and responsive experience with no lag and no external dependencies other than Windows.

The player’s chip count is stored in a small .bin file, allowing progress to carry over between sessions so the player can watch their winnings grow (or shrink) over time.

## Features

Classic Blackjack gameplay

Keyboard-only controls

Persistent chip storage

Lightweight native Windows executable

No external libraries required

## Requirements

Windows OS

A keyboard

## Building From Source

Compile using any C compiler that supports Win32 development (e.g., MSVC, MinGW).

Link against the Win32 API as required by your build environment using -lgdi32.

Run the produced executable.

Notes

This project is mainly for learning and fun. Feel free to modify, expand, or reuse the code to add features such as split hands, doubling down, animations, or a GUI.
