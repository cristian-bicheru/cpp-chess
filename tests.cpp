//
// Created by Cristian Bicheru on 12/8/2019.
//

#include "chess.cpp"

/**
 * Required Checks:
 * UCI TO/FROM
 * PUSH/POP MOVES (INCLUDING EN-PASSANT AND CASTLING)
**/

int main() {
    Board board = Board();
    board.print_board();
    board.push(move_from_uci(std::unique_ptr<std::string>(new std::string("e2e4"))));
    board.print_board();
    return 0;
}


