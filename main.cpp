#include <iostream>
#include <string>
#include "board.hpp"

#ifdef _WIN32
    #include <windows.h>
#endif

using namespace std;

bool parsePosition(const string& pos, int& row, int& col) {
    if (pos.length() !=2) return false;
    char file = pos[0];
    char rank = pos[1];
    if (file < 'a' || file > 'h' || rank < '1' || rank > '8') return false;
    col = file - 'a';          // 'a' -> 0
    row = 8 - (rank - '0');    // '8' -> 0, '1' -> 7
    return true;
}

int main() {
    #ifdef _WIN32
        SetConsoleOutputCP(65001);
    #endif

    Board board;
    board.initialize();
    board.printBoard();

    string fromSqr, toSqr;
    bool whiteTurn = true;
    while (true) {
        if (board.isInCheck(whiteTurn)) {
        cout << (whiteTurn ? "White" : "Black") << " is in check!\n";
        }
        if (board.isCheckmate(whiteTurn)) {
            cout << (whiteTurn ? "Black" : "White") << " wins by checkmate!\n";
            break;
        }
        if (board.isStalemate(whiteTurn)) {
            cout << "Stalemate! Game is a draw.\n";
            break;
        }
        cout << "Enter move (e.g., e2 e4) or 'X' to quit: ";
        cout << (whiteTurn ? "White" : "Black") << "'s move: ";
        
        cin >> fromSqr;
        if (fromSqr == "X") break;
        cin >> toSqr;

        int fromRow, fromCol, toRow, toCol;
        if (!parsePosition(fromSqr, fromRow, fromCol) || 
            !parsePosition(toSqr, toRow, toCol)) {
            cout << "Error. Use format like 'e2' or 'a1'.\n";
            continue;
        }

        Piece* piece = board.getPiece(fromRow, fromCol);
        if (!piece || piece->isWhitePiece() != whiteTurn) {
            cout << "Error: not your piece.\n";
            continue;
        }
        Board tempBoard(board);  // copy current board

        if (!tempBoard.movePiece(fromRow, fromCol, toRow, toCol, true)) {
            cout << "Invalid move. Try again.\n";
            continue;
        }

        // Check if move leaves the king in check
        if (tempBoard.isInCheck(whiteTurn)) {
            cout << "Illegal move: your king would be in check!\n";
            continue; // don't allow this move
        }

        if (board.movePiece(fromRow, fromCol, toRow, toCol)) {
            whiteTurn = !whiteTurn; // Switch turns only if move is valid
                board.printBoard();
            if (board.isCheckmate(!whiteTurn)) {
                    cout << (whiteTurn ? "White" : "Black") << " wins by checkmate!\n";
                    board.printBoard();
                    break;
                }
                
            
            else if (board.isInCheck(!whiteTurn)){
                cout << (!whiteTurn ? "White" : "Black") << " is in check!\n";}
            
            else if (board.isStalemate(!whiteTurn)) {
                cout << "Stalemate! Game is a draw.\n";
                break;
                }
            }
        else cout << "Invalid move. Try again.\n";
    }
    cout << "Thanks for playing!\n";
    return 0;
}