# Terminal Ludo Engine 🎲

A complete, single-file C++ implementation of the classic board game Ludo. This project features a text-based state engine that dynamically tracks player positions, manages home stretch requirements, and executes capture mechanics directly within the console environment.

## Features
* **Object-Oriented Architecture:** Clean separation of Tokens, Players, and Game Logic.
* **Dynamic Track Math:** Utilizes modulo arithmetic for seamless 52-space track navigation.
* **Full Game Rules:** Enforces 6-to-start requirements, extra turns on a 6, opponent capturing, and precise home stretch step tracking.
* **Input Buffer Safety:** Includes built-in error handling to prevent infinite loops from invalid user keyboard inputs.

## How to Play

### Compilation
Ensure you have a C++ compiler (like MinGW GCC) installed and added to your system PATH. Open your terminal and compile the file using:
```bash
g++ main.cpp -o ludo_game
