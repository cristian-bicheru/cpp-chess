#include "chess.h"

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

Move::Move(std::array<int, 2> sq1, std::array<int, 2> sq2, int promo_type) {
    from_square = sq1;
    to_square = sq2;
    promotion = promo_type;
}

// Piece class implementations
Piece::Piece(int col) {
    color = col;
    type = -1;
}

void Piece::set_color(int col) {
    color = col;
}

int Piece::get_color() {
    return color;
}

void Piece::set_protected(bool state) {
    piece_protected = state;
}

bool Piece::is_protected() {
    return piece_protected;
}

int Piece::get_type() {
    return type;
}
// End Piece class implementations

// King class implementations
King::King(int col) : Piece(col) {
    type = 0;
};

Queen::Queen(int col) : Piece(col) {
    type = 1;
};

Rook::Rook(int col) : Piece(col) {
    type = 2;
};

Bishop::Bishop(int col) : Piece(col) {
    type = 3;
};

Knight::Knight(int col) : Piece(col) {
    type = 4;
};

Pawn::Pawn(int col) : Piece(col) {
    type = 5;
};

std::vector<std::unique_ptr<Move>> King::get_pseudo_legal_moves(Board *board_ptr, int r, int c) {
    std::vector<std::unique_ptr<Move>> legal_moves;
    int i, j, tr, tc;
    bool proc;
    std::unique_ptr<std::array<std::array<std::unique_ptr<Piece>, 8>, 8>> board_array_ptr;
    std::array<std::array<std::unique_ptr<Piece>, 8>, 8> board_array;

    board_array_ptr = board_ptr->get_state();
    board_array = static_cast<std::array<std::array<std::unique_ptr<Piece>, 8>, 8> &&>(*board_array_ptr);

    for (i = -1; i<2; i++) {
        for (j = -1; j<2; j++) {
            if (!(i==0 && j==0))
                tr = r+i;
                tc = c+j;
                if (0<=tr<=7 && 0<=tc<=7) {
                    proc = board_array[tr][tc]->is_protected();
                    if (!proc) {
                        legal_moves.push_back(std::unique_ptr<Move>(new Move({r, c}, {tr,tc}, -1)));
                    }
                }
            }
        }
    return legal_moves;
}
// End King class implementations

// Begin Board function implementations
Board::Board() {
    // instantiate board
    reset();
};
void Board::push(Move move) {
    move_stack.push(move);
};
void Board::pop() {
    move_stack.pop();
};

void Board::reset() {
    int i, j;
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

    for (i = 2; i<6; i++) {
        for (j = 0; j<8; j++) {
            board[i][j] = std::unique_ptr<Piece>(new Piece(-1));
        }
    }
};

void Board::mirror() {
    int i, j, color1, color2;
    std::unique_ptr<Piece> ptr1, ptr2;

    for (i = 0; i < 8; i++) {
        for (j = 0; j < 8; j++) {
            // yikes? CLion suggested fixes
            ptr1 = static_cast<std::unique_ptr<Piece> &&>(board[7 - i][7 - j]);
            ptr2 = static_cast<std::unique_ptr<Piece> &&>(board[i][j]);
            color1 = ptr1->get_color();
            color2 = ptr2->get_color();

            // if piece is empty-square type
            if (color1 != -1) {
                color1++;
                color1 %= 2;
            }

            if (color2 != -1) {
                color2++;
                color2 %= 2;
            }
            ptr1->set_color(color1);
            ptr2->set_color(color2);
            board[i][j] = static_cast<std::unique_ptr<Piece> &&>(ptr1);
            board[7-i][7-j] = static_cast<std::unique_ptr<Piece> &&>(ptr2);
        }
    }
};

std::unique_ptr<std::array<std::array<std::unique_ptr<Piece>, 8>, 8>> Board::get_state() {
    // sus
    return std::unique_ptr<std::array<std::array<std::unique_ptr<Piece>, 8>, 8>>(&board);;
}

void Board::set_state(std::array<std::array<std::unique_ptr<Piece>, 8>, 8> i_board) {
    board = static_cast<std::array<std::array<std::unique_ptr<Piece>, 8>, 8> &&>(i_board);
}

void Board::print_board() {
    int i, j;
    int type, color;
    for (i = 0; i < 8; i++) {
        for (j = 0; j < 8; j++) {
            type = board[i][j]->get_type();
            color = board[i][j]->get_color();
            if (type != -1) {
                std::cout << name_table[type*(color+1)+1] << ' ';
            } else {
                std::cout << ". ";
            }
        }
        std::cout << std::endl;
    }
}
// End Board function implementations