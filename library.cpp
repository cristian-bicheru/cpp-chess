#include "library.h"

#include <iostream>
#include <stdlib.h>
#include <stack>
#include <vector>
#include <memory>

/**
 * Features:
 * board object
 *  - move stack (push, pop)
 *  - mirror board
 *  - legal moves generator
 *  - detect game end (rep)
 *  - UCI Interfacing
 */

class Move {

};

class Piece {
private:
    // 0-> white, 1 -> black
    int color;
public:
    explicit Piece(int col) {
        color = col;
    }

    void set_color(int col) {
        color = col;
    }

    int get_color() {
        return color;
    }
};

// King piece, inherits from Piece, type is 0.
class King : public Piece {
private:
    int type = 0;
public:
    explicit King(int color) : Piece(color) {}
};

class Queen : public Piece {
private:
    int type = 1;
public:
    explicit Queen(int color) : Piece(color) {}
};

class Rook : public Piece {
private:
    int type = 2;
public:
    explicit Rook(int color) : Piece(color) {}
};

class Bishop : public Piece {
private:
    int type = 3;
public:
    explicit Bishop(int color) : Piece(color) {}
};

class Knight : public Piece {
private:
    int type = 4;
public:
    explicit Knight(int color) : Piece(color) {}
};

class Pawn : public Piece {
private:
    int type = 5;
public:
    explicit Pawn(int color) : Piece(color) {}
};

class Board {
private:
    std::stack<Move> move_stack;
    std::vector<std::vector<std::unique_ptr<Piece>>> board;
public:
    Board() {
        // instantiate board
        reset();
    }

    void push(Move move) {
        move_stack.push(move);
    };
    void pop() {
        move_stack.pop();
    };
    void reset() {
        int i;
        for (i = 0; i<=1; i++) {
            board[0][i*7] = std::unique_ptr<Piece>(new Rook(i));
            board[1][i*7] = std::unique_ptr<Piece>(new Knight(i));
            board[2][i*7] = std::unique_ptr<Piece>(new Bishop(i));
            board[3][i*7] = std::unique_ptr<Piece>(new Queen(i));
            board[4][i*7] = std::unique_ptr<Piece>(new King(i));
            board[5][i*7] = std::unique_ptr<Piece>(new Bishop(i));
            board[6][i*7] = std::unique_ptr<Piece>(new Knight(i));
            board[7][i*7] = std::unique_ptr<Piece>(new Rook(i));
        }

        i = 0;
        for (i = 0; i<=1; i++) {
            board[0][i*6+1] = std::unique_ptr<Piece>(new Pawn(i));
            board[1][i*6+1] = std::unique_ptr<Piece>(new Pawn(i));
            board[2][i*6+1] = std::unique_ptr<Piece>(new Pawn(i));
            board[3][i*6+1] = std::unique_ptr<Piece>(new Pawn(i));
            board[4][i*6+1] = std::unique_ptr<Piece>(new Pawn(i));
            board[5][i*6+1] = std::unique_ptr<Piece>(new Pawn(i));
            board[6][i*6+1] = std::unique_ptr<Piece>(new Pawn(i));
            board[7][i*6+1] = std::unique_ptr<Piece>(new Pawn(i));
        }
    };

    void mirror() {
        std::vector<std::vector<std::unique_ptr<Piece>>> new_board;
        //typedef int (*Piece::get_color)(int col);
        //get_color ptr_get_color = &Piece::get_color;
        int i, j, color;
        std::unique_ptr<Piece> ptr;

        for (i = 0; i < 8; i++) {
            for (j = 0; j < 8; j++) {
                // yikes? CLion suggested fixes
                ptr = static_cast<std::unique_ptr<Piece> &&>(board[7 - i][7 - j]);
                color = ptr->get_color();
                color++;
                color %= 2;
                ptr->set_color(color);
                new_board[i][j] = static_cast<std::unique_ptr<Piece> &&>(ptr);
            }
        }

        board = new_board;
    };
};