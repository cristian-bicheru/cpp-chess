#pragma once

#include <array>
#include <vector>
#include <stack>
#include <algorithm>
#include <memory>

#ifndef CPP_CHESS_LIBRARY_H
#define CPP_CHESS_LIBRARY_H

typedef enum {EMPTY = -1, KING, QUEEN, ROOK, BISHOP, KNIGHT, PAWN} PIECE_TYPE;
typedef enum {NO_COLOR = -1, WHITE, BLACK} COLOR;
std::array<char, 8> COLUMN_LETTERS {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h'};
std::array<char, 13> NAME_TABLE = {'.', 'k', 'q', 'r', 'b', 'n', 'p', 'K', 'Q', 'R', 'B', 'N', 'P'};

// Name Declarations
class Board;
class Piece;
// End Name Declarations

class Move {
public:
    std::array<int, 2> from_square;
    std::array<int, 2> to_square;
    bool promotion;
    bool castle;
    bool en_passant;
    PIECE_TYPE old_piece;
    PIECE_TYPE new_piece;
    std::unique_ptr<Piece> taken_piece;
public:
    Move(std::array<int, 2> sq1, std::array<int, 2> sq2, bool is_promo, PIECE_TYPE f_piece = EMPTY);
    std::unique_ptr<std::string> get_uci();
};

// used as parent for all other piece types, also used as a place holder for empty squares (type = -1 and color = -1)
class Piece {
private:
    // enums
    COLOR color;
    PIECE_TYPE type;
    // used for king legal move detection
    bool piece_protected = false;
public:
    explicit Piece(COLOR col, PIECE_TYPE Type);
    void set_color(COLOR col);
    COLOR get_color();
    void set_protected(bool state);
    bool is_protected();
    PIECE_TYPE get_type();
};

// King piece, inherits from Piece, type is 0.
class King : public Piece {
private:
    PIECE_TYPE type;
public:
    explicit King(COLOR color);
    std::vector<std::unique_ptr<Move>> get_pseudo_legal_moves(Board *board_ptr, int r, int c);
};

class Queen : public Piece {
private:
    PIECE_TYPE type;
public:
    explicit Queen(COLOR color);
};

class Rook : public Piece {
private:
    PIECE_TYPE type;
public:
    explicit Rook(COLOR color);
};

class Bishop : public Piece {
private:
    PIECE_TYPE type;
public:
    explicit Bishop(COLOR color);
};

class Knight : public Piece {
private:
    PIECE_TYPE type;
public:
    explicit Knight(COLOR color);
};

class Pawn : public Piece {
private:
    PIECE_TYPE type;
public:
    explicit Pawn(COLOR color);
};


class Board {
private:
    std::stack<std::unique_ptr<Move>> move_stack;
    // array is better than vector here
    std::array<std::array<std::unique_ptr<Piece>, 8>, 8> board;
public:
    Board();
    void push(std::unique_ptr<Move> move);
    void pop();
    void reset();
    void mirror();
    void switch_colors();
    std::unique_ptr<std::array<std::array<std::unique_ptr<Piece>, 8>, 8>> get_state();
    void set_state(std::array<std::array<std::unique_ptr<Piece>, 8>, 8> i_board);
    void print_board();
};

std::unique_ptr<Piece> create_piece(PIECE_TYPE type) {
    if (type == PAWN) {
        return std::unique_ptr<Piece>(new Pawn(NO_COLOR));
    } else if (type == KNIGHT) {
        return std::unique_ptr<Piece>(new Knight(NO_COLOR));
    } else if (type == BISHOP) {
        return std::unique_ptr<Piece>(new Bishop(NO_COLOR));
    } else if (type == ROOK) {
        return std::unique_ptr<Piece>(new Rook(NO_COLOR));
    } else if (type == QUEEN) {
        return std::unique_ptr<Piece>(new Queen(NO_COLOR));
    } else if (type == KING) {
        return std::unique_ptr<Piece>(new King(NO_COLOR));
    }
};

inline int index(char item) {
    return std::distance(std::begin(COLUMN_LETTERS), std::find(std::begin(COLUMN_LETTERS), std::end(COLUMN_LETTERS), item));
}

inline PIECE_TYPE piece_from_char(char piece) {
    if (piece == 'p') {
        return PAWN;
    } else if (piece == 'n') {
        return KNIGHT;
    } else if (piece == 'b') {
        return BISHOP;
    } else if (piece == 'r') {
        return ROOK;
    } else if (piece == 'q') {
        return QUEEN;
    } else if (piece == 'k') {
        return KING;
    }
}

std::unique_ptr<Move> move_from_uci(std::unique_ptr<std::string> uci) {
    if (uci->length() == 4) {
        return std::make_unique<Move>(new Move({index((*uci)[0]), (int) (*uci)[1]}, {index((*uci)[2]), (int) (*uci)[3]}, false));
    } else {
        return std::make_unique<Move>(new Move({index((*uci)[0]), (int) (*uci)[1]}, {index((*uci)[2]), (int) (*uci)[3]}, true, piece_from_char((*uci)[4])));
    }

}

#endif //CPP_CHESS_LIBRARY_H