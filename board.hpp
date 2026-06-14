#ifndef BOARD_HPP
#define BOARD_HPP

#include <vector>
#include "piece.hpp"
using namespace std;

class Board {
    private:
    Piece* grid[8][8];
    pair<int, int> enPassantTarget;

    static bool isInBounds(int row, int col);
    void clear();
    bool movePieceUnchecked(int fromRow, int fromCol, int toRow, int toCol, bool autoPromote, bool announceErrors);
    bool attacksSquare(Piece* attacker, int fromRow, int fromCol, int toRow, int toCol) const;

    public:
    Board();
    Board(const Board& other);
    Board& operator=(const Board& other);
    ~Board();

    void initialize();                  //for initial position
    void printBoard() const;           // for displaying board
    Piece* getPiece(int row, int col) const; // for choosing a piece
    bool movePiece(int fromRow, int fromCol, int toRow, int toCol, bool autoPromote = false); // for moving the choosen piece to other cell
    bool isPathClear(int fromRow, int fromCol, int toRow, int toCol) const;
    bool isSquareAttacked(int row, int col, bool byWhite) const;
    bool canCastleKingSide(bool white) const;
    bool canCastleQueenSide(bool white) const;
    void setPiece(int row, int col, Piece* piece); // needed in movePiece
    bool isInCheck(bool white) const;
    bool isCheckmate(bool white) const;
    bool isStalemate(bool white) const;
};
#endif
