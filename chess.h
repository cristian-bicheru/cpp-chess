#pragma once

#include <array>
#include <vector>
#include <stack>
#include <memory>

#ifndef CPP_CHESS_LIBRARY_H
#define CPP_CHESS_LIBRARY_H

// Name Declarations
class Board;
// End Name Declarations

class Move {
private:
    std::array<int, 2> from_square;
    std::array<int, 2> to_square;
    // promotion = -1 indicates no promotion, otherwise the type of the piece indicates the piece being promoted to.
    int promotion;
public:
    Move(std::array<int, 2> sq1, std::array<int, 2> sq2, int promo_type);
};

// used as parent for all other piece types, also used as a place holder for empty squares (type = -1 and color = -1)
class Piece {
private:
    // 0-> white, 1 -> black
    int color;
    int type;
    // used for king legal move detection
    bool piece_protected = false;
public:
    explicit Piece(int col);
    void set_color(int col);
    int get_color();
    void set_protected(bool state);
    bool is_protected();
    int get_type();
};

// King piece, inherits from Piece, type is 0.
class King : public Piece {
private:
    int type;
public:
    explicit King(int color);
    std::vector<std::unique_ptr<Move>> get_pseudo_legal_moves(Board *board_ptr, int r, int c);
};

class Queen : public Piece {
private:
    int type;
public:
    explicit Queen(int color);
};

class Rook : public Piece {
private:
    int type;
public:
    explicit Rook(int color);
};

class Bishop : public Piece {
private:
    int type;
public:
    explicit Bishop(int color);
};

class Knight : public Piece {
private:
    int type;
public:
    explicit Knight(int color);
};

class Pawn : public Piece {
private:
    int type;
public:
    explicit Pawn(int color);
};


class Board {
private:
    std::stack<Move> move_stack;
    // array is better than vector here
    std::array<std::array<std::unique_ptr<Piece>, 8>, 8> board;
    std::array<char, 13> name_table = {'.', 'k', 'q', 'r', 'b', 'n', 'p', 'K', 'Q', 'R', 'B', 'N', 'P'};
public:
    Board();
    void push(Move move);
    void pop();
    void reset();
    void mirror();
    std::unique_ptr<std::array<std::array<std::unique_ptr<Piece>, 8>, 8>> get_state();
    void set_state(std::array<std::array<std::unique_ptr<Piece>, 8>, 8> i_board);
    void print_board();
};

#endif //CPP_CHESS_LIBRARY_H