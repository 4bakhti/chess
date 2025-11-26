#include "board.hpp"
#include <iostream>
#include <cmath>

using namespace std;

Board::Board(){ //constructor
    enPassantTarget={-1,-1};
    for (int i = 0; i < 8; i++){
        for (int j = 0; j < 8; j++){
            grid[i][j]=nullptr;
        }
    }
}
//I will use "The Rule of Three(destructor, copy constructor, copy assignment operator)"
Board::~Board() { //destructor
    for (int i = 0; i < 8; i++)
        for (int j = 0; j < 8; j++)
            delete grid[i][j];
}

Board::Board(const Board& other) { //for using polymorphism
    enPassantTarget = other.enPassantTarget;
    for (int row = 0; row < 8; row++) {
        for (int col = 0; col < 8; col++) {
            if (other.grid[row][col]) grid[row][col] = other.grid[row][col]->clone(); 
            else grid[row][col] = nullptr;
        }
    }
}

Board& Board::operator=(const Board& other) { //Assignment Operator
    if (this != &other) {
        enPassantTarget = other.enPassantTarget;
        for (int row = 0; row < 8; row++)
            for (int col = 0; col < 8; col++) delete grid[row][col]; // Firstly we will delete our current pieces to avoid memory leaks
        
        for (int row = 0; row < 8; row++) {
            for (int col = 0; col < 8; col++) {
                if (other.grid[row][col]) grid[row][col] = other.grid[row][col]->clone();
                else grid[row][col] = nullptr; // Then copy new pieces using Polymorphism
            }
        }
    }
    return *this;
}

void Board::initialize() {
    //pawns
    for(int col=0; col<8; col++){
        grid[6][col]=new Pawn(true); //true represents white
        grid[1][col]=new Pawn(false); //false represents black
    }
    //rooks
    grid[0][0]=new Rook(false);
    grid[0][7]=new Rook(false);
    grid[7][0]=new Rook(true);
    grid[7][7]=new Rook(true);

    //knights
    grid[0][1]=new Knight(false);
    grid[0][6]=new Knight(false);
    grid[7][1]=new Knight(true);
    grid[7][6]=new Knight(true);

    //bishops
    grid[0][2]=new Bishop(false);
    grid[0][5]=new Bishop(false);
    grid[7][2]=new Bishop(true);
    grid[7][5] =new Bishop(true);

    //Queens
    grid[0][3]= new Queen(false);
    grid[7][3]=new Queen(true);

    //Kings
    grid[0][4]=new King(false);
    grid[7][4]=new King(true);
}

void Board::printBoard() const {
    cout << "  a b c d e f g h\n";
    for(int row=0; row<8; row++){
        cout << 8 - row << " ";
        for(int col=0; col<8;col++){
            if(grid[row][col]) cout<<grid[row][col]->getSymbol()<<' ';
            else cout<<". ";
        }
        cout << 8 - row << '\n';
    }
    cout << "  a b c d e f g h\n";
}

Piece* Board::getPiece(int row, int col) const{
    return grid[row][col];
}

void Board::setPiece(int row, int col, Piece* piece) {
    grid[row][col] = piece;
}

bool Board::isPathClear(int fromRow, int fromCol, int toRow, int toCol) const {
    int rowStep = (toRow - fromRow) == 0 ? 0 : (toRow - fromRow) / abs(toRow - fromRow);
    int colStep = (toCol - fromCol) == 0 ? 0 : (toCol - fromCol) / abs(toCol - fromCol);
    int r = fromRow + rowStep;
    int c = fromCol + colStep;

    while (r != toRow || c != toCol) {
        if (grid[r][c] != nullptr) return false; // Blocked
        r += rowStep;
        c += colStep;
    }
    return true;
}

bool Board::isSquareAttacked(int row, int col, bool byWhite) const {
    for (int r = 0; r < 8; ++r) {
        for (int c = 0; c < 8; ++c) {
            Piece* attacker = getPiece(r, c);
            if (!attacker || attacker->isWhitePiece() != byWhite) continue;

            if (attacker->isMoveValid(r, c, row, col)) {
                if (dynamic_cast<Knight*>(attacker) || isPathClear(r, c, row, col)) { // If it's a Knight, it jumps. If not, path must be clear.
                    return true;
                }
            }
        }
    }
    return false;
}

bool Board::isInCheck(bool white) const {
    int kingRow = -1, kingCol = -1;
    for (int row = 0; row < 8; row++) { // to find a king
        for (int col = 0; col < 8; col++) {
            Piece* piece = getPiece(row, col);
            if (piece && dynamic_cast<King*>(piece) && piece->isWhitePiece() == white) {
                kingRow = row; kingCol = col;
                break;
            }
        }
    }
    return isSquareAttacked(kingRow, kingCol, !white); // Is the King under attack?
}

bool Board::isCheckmate(bool white) const {
    for (int fromRow = 0; fromRow < 8; fromRow++) {
        for (int fromCol = 0; fromCol < 8; fromCol++) {
            Piece* piece = getPiece(fromRow, fromCol);
            if (!piece || piece->isWhitePiece() != white) continue; //skipping unnesesary cases

            for (int toRow = 0; toRow < 8; toRow++) {
                for (int toCol = 0; toCol < 8; toCol++) {
                    if (!piece->isMoveValid(fromRow, fromCol, toRow, toCol)) continue;
                    
                    Board tempBoard(*this); 

                    if (tempBoard.movePiece(fromRow, fromCol, toRow, toCol, true) && 
                        !tempBoard.isInCheck(white)) {
                        return false; // found an escape->not checkmate
                    }
                }
            }
        }
    }
    return true; // No escapes found
}

bool Board::isStalemate(bool white) const { //for optimization we can just use two previous functions, if isInCheck() returns false and isCheckmate returns true, then it is Stalemate
    if (isInCheck(white)) return false;
    return isCheckmate(white); 
}

bool Board::canCastleKingSide(bool white) const {
    int row = white ? 7 : 0;
    Piece* king = getPiece(row, 4);
    Piece* rook = getPiece(row, 7);

    if (!king || !rook) return false;
    if (!dynamic_cast<King*>(king) || !dynamic_cast<Rook*>(rook)) return false;

    if (king->hasMovedBefore() || rook->hasMovedBefore()) return false;
    //squares between king and rook should be empty and not attacked
    if (getPiece(row, 5) || getPiece(row, 6)) return false;
    if (isSquareAttacked(row, 4, !white)) return false; // King’s starting square
    if (isSquareAttacked(row, 5, !white)) return false; // Passing square
    if (isSquareAttacked(row, 6, !white)) return false; // Destination square
    return true;
}

bool Board::canCastleQueenSide(bool white) const {
    int row = white ? 7 : 0;
    Piece* king = getPiece(row, 4);
    Piece* rook = getPiece(row, 0);

    if (!king || !rook) return false;
    if (!dynamic_cast<King*>(king) || !dynamic_cast<Rook*>(rook)) return false;

    if (king->hasMovedBefore() || rook->hasMovedBefore()) return false;
    if (getPiece(row, 1) || getPiece(row, 2) || getPiece(row, 3)) return false;
    if (isSquareAttacked(row, 4, !white)) return false;
    if (isSquareAttacked(row, 3, !white)) return false;
    if (isSquareAttacked(row, 2, !white)) return false;
    return true;
}

bool Board::movePiece(int fromRow, int fromCol, int toRow, int toCol,bool autoPromote) {
    Piece* piece = getPiece(fromRow, fromCol);
    if (!piece) {
        cout << "No piece at starting position\n";
        return false;
    }

    if (!piece->isMoveValid(fromRow, fromCol, toRow, toCol)) {
        cout << "Invalid move for that piece.\n";
        return false;
    }

    Piece* target = getPiece(toRow, toCol);
    if (target && target->isWhitePiece() == piece->isWhitePiece()) {
        return false;
    }

    if (Pawn* pawn = dynamic_cast<Pawn*>(piece)) {
        int direction = pawn->isWhitePiece() ? -1 : 1;
        // Forward Move (1 step)
        if (fromCol == toCol && toRow == fromRow + direction) {
            if (target) return false; // Blocked
            enPassantTarget = {-1, -1};
        }
        // Forward Move (2 steps)
        else if (fromCol == toCol && toRow == fromRow + 2 * direction) {
            int startRow = pawn->isWhitePiece() ? 6 : 1;
            if (fromRow != startRow) return false;
            if (getPiece(fromRow + direction, fromCol) || target) return false; // Blocked
            enPassantTarget = {fromRow + direction, fromCol}; // Set Target!
        }
        //Capture
        else if (abs(fromCol - toCol) == 1 && toRow == fromRow + direction) {
            // En passant; logic: If square is empty (no target) BUT it matches enPassantTarget
            if (!target && toRow == enPassantTarget.first && toCol == enPassantTarget.second) {
                int capturedRow = pawn->isWhitePiece() ? toRow + 1 : toRow - 1;
                delete grid[capturedRow][toCol]; // Delete the pawn behind us
                grid[capturedRow][toCol] = nullptr;
                enPassantTarget = {-1, -1};
            }
            // Standard Capture
            else if (target) {
                enPassantTarget = {-1, -1};
            }
            
            else {
                return false; 
            }    
        }
        else { return false; } //for safety
    }
    else {
        enPassantTarget = {-1, -1}; // clearing EP if we move non-pawn.
        if (!dynamic_cast<Knight*>(piece) && !isPathClear(fromRow, fromCol, toRow, toCol)) { //checking for non-knight
            return false;
        }
    }

    //castling
    if (King* king = dynamic_cast<King*>(piece)) {
        int colDiff = toCol - fromCol;
        // King-side castling
        if (colDiff == 2) {
            if (canCastleKingSide(piece->isWhitePiece())) {
                setPiece(toRow, toCol, piece);
                setPiece(fromRow, fromCol, nullptr);

                // Move rook
                Piece* rook = getPiece(fromRow, 7);
                setPiece(fromRow, 5, rook);
                setPiece(fromRow, 7, nullptr);

                king->setMoved();
                rook->setMoved();
                return true;
            }
            return false;
        }
        // Queen-side castling
        else if (colDiff == -2) {
            if (canCastleQueenSide(piece->isWhitePiece())) {
                setPiece(toRow, toCol, piece);
                setPiece(fromRow, fromCol, nullptr);

                Piece* rook = getPiece(fromRow, 0);
                setPiece(fromRow, 3, rook);
                setPiece(fromRow, 0, nullptr);

                king->setMoved();
                rook->setMoved();
                return true;
            }
            return false;
        }
    }

    if (target) delete target; // capture
    grid[toRow][toCol] = piece;
    grid[fromRow][fromCol] = nullptr;
    piece->setMoved();

    if (Pawn* pawn = dynamic_cast<Pawn*>(piece)) {
        int promoRank = pawn->isWhitePiece() ? 0 : 7;
        if (toRow == promoRank) {
            Piece* newPiece = nullptr;
            bool white = pawn->isWhitePiece();

            if (autoPromote) newPiece = new Queen(white); // Default to Queen
            else{
                cout << "Promote to (Q, R, B, N): ";
                char choice;
                cin >> choice;
                switch(toupper(choice)) {
                    case 'R': newPiece = new Rook(white); break;
                    case 'B': newPiece = new Bishop(white); break;
                    case 'N': newPiece = new Knight(white); break;
                    default:  newPiece = new Queen(white); break;
                }
            }
            delete grid[toRow][toCol]; // Remove pawn
            grid[toRow][toCol] = newPiece; // Add Queen (or other)
        }
    }
    return true;
}