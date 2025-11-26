    #include "piece.hpp"

    Piece::Piece(bool isWhite) : isWhite(isWhite), hasMoved(false) {} //constructor
    Piece::~Piece() {} //destructor

    bool Piece::isWhitePiece() const { //returns true for white, false for clack pieces
        return isWhite; 
    }

    // Derived class constructors
    Pawn::Pawn(bool isWhite) : Piece(isWhite) {}
    Rook::Rook(bool isWhite) : Piece(isWhite) {}
    Knight::Knight(bool isWhite) : Piece(isWhite) {}
    Bishop::Bishop(bool isWhite) : Piece(isWhite) {}
    Queen::Queen(bool isWhite) : Piece(isWhite) {}
    King::King(bool isWhite) : Piece(isWhite) {}