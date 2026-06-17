# C++ Chess Game

A functional Chess game implementation written in C++. This project focuses on the core logic of the game, including board representation, move validation, advanced movements(en Passant, castling, promotion, etc) and game state management.

##  Current Features
* **Core Logic:** Full implementation of standard chess rules.
* **Language:** Written in pure C++ for efficiency.
* **Local Multiplayer:** Currently supports two-player (Human vs. Human) gameplay.
* **Console Output:** Visualizes the board via the command line/terminal.
* **Windows GUI:** Includes a clickable desktop board for local two-player games.
* **Move History:** Shows played moves in the GUI, including captures, castling, promotion, check, and checkmate markers.

## 🔮 Roadmap & Future Plans
This project is currently under active development. The following features are planned for future releases:

- [x] **Graphical User Interface (UI):** Moving away from the console to a windowed graphical experience.
- [ ] **Single Player AI:** Implementing a chess engine to allow playing against the computer.
- [ ] **Personalized Bot:** Adding personalized bots for each player to *analyze patterns and adapt to their playstyle.*

## 🛠️ How to Compile and Run
1.  Clone this repository.
2.  Navigate to the project folder.
3.  Compile the console version using a C++ compiler (like g++):
    ```bash
    g++ main.cpp board.cpp piece.cpp -o chess_game
    ```
4.  Run the executable:
    ```bash
    ./chess_game
    ```

### Windows GUI build
Compile the desktop GUI version with MinGW:
```bash
g++ board.cpp piece.cpp gui.cpp -o chess_gui.exe -mwindows -lgdi32
```

Run `chess_gui.exe`, then click a piece and click its destination. Press `R` to reset the game or `Esc` to quit.

## 🤝 Contributing
Feel free to submit pull requests if you want to help implement the UI or AI!
