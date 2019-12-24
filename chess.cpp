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

Move::Move(std::array<int, 2> sq1, std::array<int, 2> sq2, bool is_promo, PIECE_TYPE f_piece) {
    from_square = sq1;
    to_square = sq2;
    promotion = is_promo;
    new_piece = f_piece;
}

std::unique_ptr<std::string> Move::get_uci() {
    if (!promotion) {
        return std::make_unique<std::string>(COLUMN_LETTERS[8-from_square[0]], (char) from_square[1], COLUMN_LETTERS[8-to_square[0]], (char) to_square[1]);
    } else {
        return std::make_unique<std::string>(COLUMN_LETTERS[8-from_square[0]], (char) from_square[1], COLUMN_LETTERS[8-to_square[0]], (char) to_square[1], NAME_TABLE[1+(int) new_piece]);
    }
}

// Piece class implementations
Piece::Piece(COLOR col, PIECE_TYPE Type = EMPTY) {
    color = col;
    type = Type;
}

void Piece::set_color(COLOR col) {
    color = col;
}

COLOR Piece::get_color() {
    return color;
}

void Piece::set_protected(bool state) {
    piece_protected = state;
}

bool Piece::is_protected() {
    return piece_protected;
}

PIECE_TYPE Piece::get_type() {
    return type;
}
// End Piece class implementations

// King class implementations
King::King(COLOR col) : Piece(col, KING) {};

Queen::Queen(COLOR col) : Piece(col, QUEEN) {};

Rook::Rook(COLOR col) : Piece(col, ROOK) {};

Bishop::Bishop(COLOR col) : Piece(col, BISHOP) {};

Knight::Knight(COLOR col) : Piece(col, KNIGHT) {};

Pawn::Pawn(COLOR col) : Piece(col, PAWN) {};

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

void Board::push(std::unique_ptr<Move> move) {
    int deltax, deltay, i, dist;
    bool clear = true;
    // in special cases such as en-passant or castling, this might be ignored, hence placement might differ in the final release
    move->taken_piece = std::move(board[move->to_square[0]][move->to_square[1]]);
    move->old_piece = board[move->from_square[0]][move->from_square[1]]->get_type();

    // CASTLE CHECKING
    // If the moved piece is a king..
    if (board[move->from_square[0]][move->from_square[1]]->get_type() == KING) {
        // set deltax to be the number of horizontal squares moved
        deltax = move->to_square[1] - move->from_square[1];
        // if deltax is 2 or more
        if (abs(deltax) > 1) {
            // and if there is a rook at the end of the file (which end depends on the direction of deltax)
            if (deltax > 0) {
                if (board[move->from_square[0]][7]->get_type() == ROOK) {
                    // and if the file is clear
                    for (i = move->from_square[1]+1; i<7; i++) {
                        if (board[move->from_square[0]][i]->get_type() != EMPTY) {
                            clear = false;
                            break;
                        }
                    }
                    if (clear) {
                        // then the move is (probably, still need to check distances) a castle and we move the pieces accordingly
                        // note that by default no checks are done to ensure that castling or any moves in particular are legal
                        dist = 7-move->from_square[1];
                        if (dist == 3) {
                            // then the king must be in the fifth square, and this move is a castle
                            move->castle = true;
                            // move king to the seventh square
                            board[move->from_square[0]][6] = std::move(board[move->from_square[0]][4]);
                            // move rook to the sixth square
                            board[move->from_square[0]][5] = std::move(board[move->from_square[0]][7]);
                            // replace the empty squares
                            board[move->from_square[0]][4] = std::unique_ptr<Piece>(new Piece(NO_COLOR, EMPTY));
                            board[move->from_square[0]][7] = std::unique_ptr<Piece>(new Piece(NO_COLOR, EMPTY));
                        } else if (dist == 4) {
                            // then the king must be in the fourth square, and this move is a castle
                            move->castle = true;
                            // move king to the sixth square
                            board[move->from_square[0]][5] = std::move(board[move->from_square[0]][3]);
                            // move rook to the fifth square
                            board[move->from_square[0]][4] = std::move(board[move->from_square[0]][7]);
                            // replace the empty squares
                            board[move->from_square[0]][3] = std::unique_ptr<Piece>(new Piece(NO_COLOR, EMPTY));
                            board[move->from_square[0]][7] = std::unique_ptr<Piece>(new Piece(NO_COLOR, EMPTY));
                        }
                    }
                }
            // otherwise if deltax < 0, we must be castling in the other direction
            } else {
                if (board[move->from_square[0]][0]->get_type() == ROOK) {
                    // and if the file is clear
                    for (i = move->from_square[1]-1; i>=0; i--) {
                        if (board[move->from_square[0]][i]->get_type() != EMPTY) {
                            clear = false;
                            break;
                        }
                    }
                    if (clear) {
                        // then the move is (probably, still need to check distances) a castle and we move the pieces accordingly
                        // note that by default no checks are done to ensure that castling or any moves in particular are legal
                        dist = move->from_square[1];
                        if (dist == 3) {
                            // then the king must be in the fourth square, and this move is a castle
                            move->castle = true;
                            // move king to the second square
                            board[move->from_square[0]][1] = std::move(board[move->from_square[0]][3]);
                            // move rook to the third square
                            board[move->from_square[0]][2] = std::move(board[move->from_square[0]][0]);
                            // replace the empty squares
                            board[move->from_square[0]][3] = std::unique_ptr<Piece>(new Piece(NO_COLOR, EMPTY));
                            board[move->from_square[0]][0] = std::unique_ptr<Piece>(new Piece(NO_COLOR, EMPTY));
                        } else if (dist == 4) {
                            // then the king must be in the fifth square, and this move is a castle
                            move->castle = true;
                            // move king to the third square
                            board[move->from_square[0]][2] = std::move(board[move->from_square[0]][3]);
                            // move rook to the fourth square
                            board[move->from_square[0]][3] = std::move(board[move->from_square[0]][7]);
                            // replace the empty squares
                            board[move->from_square[0]][4] = std::unique_ptr<Piece>(new Piece(NO_COLOR, EMPTY));
                            board[move->from_square[0]][0] = std::unique_ptr<Piece>(new Piece(NO_COLOR, EMPTY));
                        }
                    }
                }
            }
        }
    }
    if (!move->castle) {
        // check for en-passant
        if (board[move->from_square[0]][move->from_square[1]]->get_type() == PAWN) {
            deltax = move->from_square[1]-move->to_square[1];
            deltay = move->from_square[0]-move->to_square[0];

            if (abs(deltax) == 1) {
                if (abs(deltay) == 1) {
                    // if the square immediately to the left or right is an enemy pawn
                    // left case
                    if (board[move->from_square[0]][move->from_square[1]-deltax]->get_color() != board[move->from_square[0]][move->from_square[1]]->get_color()) {
                        if (board[move->from_square[0]][move->from_square[1]-deltax]->get_type() == PAWN) {
                            // then the move is an en-passant move and the pieces are moved accordingly
                            move->en_passant = true;

                            board[move->to_square[0]][move->to_square[1]] = std::move(board[move->from_square[0]][move->from_square[1]]);
                            // replace empty squares
                            board[move->from_square[0]][move->from_square[1]] = std::unique_ptr<Piece>(new Piece(NO_COLOR, EMPTY));
                            board[move->from_square[0]-deltax][move->from_square[1]] = std::unique_ptr<Piece>(new Piece(NO_COLOR, EMPTY));
                        }
                    }

                }
            }
        }
    }
    if (!move->en_passant) {
        if (!move->castle) {
            if (!move->promotion) {
                board[move->to_square[0]][move->to_square[1]] = std::move(board[move->from_square[0]][move->from_square[1]]);
                board[move->from_square[0]][move->from_square[1]] = std::make_unique<Piece>(NO_COLOR);
            } else {
                board[move->to_square[0]][move->to_square[1]] = create_piece(move->new_piece);
                board[move->to_square[0]][move->to_square[1]]->set_color(board[move->from_square[0]][move->from_square[1]]->get_color());
                board[move->from_square[0]][move->from_square[1]] = std::make_unique<Piece>(NO_COLOR);
            }
        }
    }

    move_stack.push(std::move(move));
};
void Board::pop() {
    std::unique_ptr<Move> move;
    int deltax, dist;
    // stack broken?
    move = move_stack.top();
    move_stack.pop();
    if (!move->en_passant) {
        if (!move->castle) {
            if (!move->promotion) {
                board[move->from_square[0]][move->from_square[1]] = std::move(board[move->to_square[0]][move->to_square[1]]);
            } else {
                board[move->from_square[0]][move->from_square[1]] = create_piece(move->old_piece);
            }
        } else {
            //undo castle move
            board[move->from_square[0]][move->from_square[1]] = std::move(board[move->to_square[0]][move->to_square[1]]);
            deltax = move->from_square[1]-move->to_square[1];
            if (deltax > 0) {
                board[move->to_square[0]][7] = std::move(board[move->to_square[0]][move->to_square[1]-1]);
                dist = 7-move->from_square[1];
                // now fill in empty squares
                if (dist == 3) {
                    board[move->to_square[0]][5] = std::make_unique<Piece>(NO_COLOR, EMPTY);
                    board[move->to_square[0]][6] = std::make_unique<Piece>(NO_COLOR, EMPTY);
                } else if (dist == 4) {
                    board[move->to_square[0]][4] = std::make_unique<Piece>(NO_COLOR, EMPTY);
                    board[move->to_square[0]][5] = std::make_unique<Piece>(NO_COLOR, EMPTY);
                    board[move->to_square[0]][6] = std::make_unique<Piece>(NO_COLOR, EMPTY);
                }

            } else {
                board[move->to_square[0]][0] = std::move(board[move->to_square[0]][move->to_square[1]+1]);
                // now fill in empty squares
                if (dist == 3) {
                    board[move->to_square[0]][1] = std::make_unique<Piece>(NO_COLOR, EMPTY);
                    board[move->to_square[0]][2] = std::make_unique<Piece>(NO_COLOR, EMPTY);
                } else if (dist == 4) {
                    board[move->to_square[0]][1] = std::make_unique<Piece>(NO_COLOR, EMPTY);
                    board[move->to_square[0]][2] = std::make_unique<Piece>(NO_COLOR, EMPTY);
                    board[move->to_square[0]][3] = std::make_unique<Piece>(NO_COLOR, EMPTY);
                }
            }
        }
    } else {
        // undo en-passant move
        board[move->from_square[0]][move->from_square[1]] = std::move(board[move->to_square[0]][move->to_square[1]]);
        deltax = move->from_square[1]-move->to_square[1];
        board[move->from_square[0]-deltax][move->from_square[1]] = std::unique_ptr<Piece>(new Pawn((
                board[move->from_square[0]][move->from_square[1]]->get_color()==WHITE ? BLACK : WHITE)));
    }

    board[move->to_square[0]][move->to_square[1]] = std::move(move->taken_piece);
};

void Board::reset() {
    int i, j;
    for (i = 0; i<=1; i++) {
        board[i*7][0] = std::unique_ptr<Piece>(new Rook(static_cast<COLOR>(i)));
        //std::cout << board[0][i*7]->get_type();
        board[i*7][1] = std::unique_ptr<Piece>(new Knight(static_cast<COLOR>(i)));
        board[i*7][2] = std::unique_ptr<Piece>(new Bishop(static_cast<COLOR>(i)));
        board[i*7][3] = std::unique_ptr<Piece>(new Queen(static_cast<COLOR>(i)));
        board[i*7][4] = std::unique_ptr<Piece>(new King(static_cast<COLOR>(i)));
        board[i*7][5] = std::unique_ptr<Piece>(new Bishop(static_cast<COLOR>(i)));
        board[i*7][6] = std::unique_ptr<Piece>(new Knight(static_cast<COLOR>(i)));
        board[i*7][7] = std::unique_ptr<Piece>(new Rook(static_cast<COLOR>(i)));
    }

    for (i = 0; i<=1; i++) {
        board[i*5+1][0] = std::unique_ptr<Piece>(new Pawn(static_cast<COLOR>(i)));
        board[i*5+1][1] = std::unique_ptr<Piece>(new Pawn(static_cast<COLOR>(i)));
        board[i*5+1][2] = std::unique_ptr<Piece>(new Pawn(static_cast<COLOR>(i)));
        board[i*5+1][3] = std::unique_ptr<Piece>(new Pawn(static_cast<COLOR>(i)));
        board[i*5+1][4] = std::unique_ptr<Piece>(new Pawn(static_cast<COLOR>(i)));
        board[i*5+1][5] = std::unique_ptr<Piece>(new Pawn(static_cast<COLOR>(i)));
        board[i*5+1][6] = std::unique_ptr<Piece>(new Pawn(static_cast<COLOR>(i)));
        board[i*5+1][7] = std::unique_ptr<Piece>(new Pawn(static_cast<COLOR>(i)));
    }

    // use nullptr for empty squares?
    for (i = 2; i<6; i++) {
        for (j = 0; j<8; j++) {
            board[i][j] = std::make_unique<Piece>(NO_COLOR);
        }
    }
};

void Board::mirror() {
    int i, j, color1, color2;
    std::unique_ptr<Piece> ptr1, ptr2;

    for (i = 0; i < 4; i++) {
        for (j = 0; j < 8; j++) {
            ptr1 = static_cast<std::unique_ptr<Piece> &&>(board[7 - i][7 - j]);
            ptr2 = static_cast<std::unique_ptr<Piece> &&>(board[i][j]);

            board[i][j] = std::move(ptr1);
            board[7-i][7-j] = std::move(ptr2);
        }
    }
};

void Board::switch_colors() {
    int i, j, color1, color2;
    std::unique_ptr<Piece> ptr1, ptr2;

    for (i = 0; i < 4; i++) {
        for (j = 0; j < 8; j++) {
            ptr1 = static_cast<std::unique_ptr<Piece> &&>(board[7 - i][7 - j]);
            ptr2 = static_cast<std::unique_ptr<Piece> &&>(board[i][j]);
            color1 = ptr1->get_color();
            color2 = ptr2->get_color();

            // if piece is not empty-square type

            if (color1 != -1) {
                color1++;
                color1 %= 2;
            }

            if (color2 != -1) {
                color2++;
                color2 %= 2;
            }

            ptr1->set_color(static_cast<COLOR>(color1));
            ptr2->set_color(static_cast<COLOR>(color2));
            board[i][j] = std::move(ptr2);
            board[7-i][7-j] = std::move(ptr1);
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
            //std::cout << type << " " << color << std::endl;

            if (type != -1) {
                std::cout << NAME_TABLE[color*5+1+color+type] << ' ';
            } else {
                std::cout << ". ";
            }
        }
        std::cout << std::endl;
    }
}
// End Board function implementations