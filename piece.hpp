#ifndef PIECE_HPP
#define PIECE_HPP

#include <cmath>
#include <string> 
using namespace std;

class Piece{
protected:
    bool isWhite;
    bool hasMoved=false;
public:
    Piece(bool isWhite); //constructor 
    virtual ~Piece(); //destructor for polymorphism
    bool isWhitePiece() const;
    void setMoved() {hasMoved=true;};
    bool hasMovedBefore (){return hasMoved;} //for castling
    virtual string getSymbol() const=0; //derived classes
    virtual bool isMoveValid(int fromRow, int fromCol, int toRow, int toCol) const = 0; //for checking only valid moves
    virtual Piece* clone() const = 0;
};

    //Pawn
    class Pawn: public Piece{
        public: 
        Pawn(bool isWhite);
        string getSymbol() const {return isWhite ? "♙": "♟";}
        bool isMoveValid(int fromRow, int fromCol, int toRow, int toCol) const {
            int direction=isWhite? -1: 1;
            if(toCol==fromCol && toRow==fromRow+direction) return true;
            if((!isWhite && fromRow==1) || (isWhite && fromRow==6)){
                if(toCol==fromCol && toRow==fromRow+2*direction) return true;
            }
            if (abs(toCol - fromCol) == 1 && toRow == fromRow + direction) return true;
            
            return false;
        }
        Piece* clone() const override {return new Pawn(*this);}
    };

    //Rook
    class Rook: public Piece{
        public:
        Rook(bool isWhite);
        string getSymbol() const {return isWhite? "♖" : "♜";}
        bool isMoveValid(int fromRow, int fromCol, int toRow, int toCol) const {
            if(fromRow==toRow || fromCol==toCol) return true;
        return false;
        }
        Piece* clone() const override { return new Rook(*this);}
    };

    //Knight
    class Knight: public Piece{
        public:
        Knight(bool isWhite);
        string getSymbol() const {return isWhite ? "♘" : "♞";}
        bool isMoveValid(int fromRow, int fromCol, int toRow, int toCol) const{
            int rowDiff=abs(fromRow-toRow);
            int colDiff=abs(fromCol-toCol);
            if((rowDiff==1 && colDiff==2) || (rowDiff==2 &&colDiff==1)) return true;
            return false;
        }
        Piece* clone() const override { return new Knight(*this);}    
    };

    //Bishop
    class Bishop: public Piece{
        public:
        Bishop(bool isWhite);
        string getSymbol() const {return isWhite ? "♗":"♝";}
        bool isMoveValid(int fromRow, int fromCol, int toRow, int toCol) const {
            if(abs(fromRow-toRow)==abs(fromCol-toCol)) return true;
            return false;
        }
        Piece* clone() const override { return new Bishop(*this);}
    };
    //Queen
    class Queen: public Piece{
        public:
        Queen(bool isWhite);
        string getSymbol() const {return isWhite ? "♕" :"♛";}
        bool isMoveValid(int fromRow, int fromCol, int toRow, int toCol) const {
            if(abs(fromRow-toRow)==abs(fromCol-toCol) || fromRow==toRow || fromCol==toCol) return true;
            return false;
        }
        Piece* clone() const override { return new Queen(*this);}
    };

    //King
    class King: public Piece{
        public:
        King(bool isWhite);
        string getSymbol() const{ return isWhite ? "♔":"♚";}
        bool isMoveValid(int fromRow, int fromCol, int toRow, int toCol) const {
            if (abs(fromRow - toRow) <= 1 && abs(fromCol - toCol) <= 1 &&
                        !(fromRow == toRow && fromCol == toCol)) return true;
            if (abs(fromRow-toRow) == 0 && abs(fromCol-toCol) == 2) return true;
            return false;
        }
        Piece* clone() const override { return new King(*this);}
    };

#endif