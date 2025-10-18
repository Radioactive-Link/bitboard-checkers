#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#include "config.h"
#include "core.h"

/**
 * @file Setups for the game for use in the main function 
 */

Board standard_setup() {
    Board board = { 0, 0 };

    // populate black side
    for (int i = 0; i < 3; i++) {
        bool flip = !(i % 2);
        for (int j = 0; j < 8; j++) {
            if (flip) {
                board.black_pieces |= (1ull << (i*8) + j);
            }
            // flop
            flip = !flip;
        }
    }

    // populate red side
    for (int i = BOARD_ROW_END; i > BOARD_ROW_END - 3; i--) {
        bool flip = !(i % 2);
        for (int j = 0; j < 8; j++) {
            if (flip) {
                board.red_pieces |= (1ull << (i*8) + j);
            }
            flip = !flip;
        }
    }

    return board;
}

Board test_capture_ai() {
    Board board = { 0, 0 };
    
    board.black_pieces |= 1;
    board.black_pieces |= 1ull << 22;
    board.black_pieces |= 1ull << 54;

    board.red_pieces |= 1ull << 31;
    board.red_pieces |= 1ull << 63;

    return board;
}

Board test_win_condition_black() {
    Board board = { 0, 0 };

    board.black_pieces |= 1ull << 27;
    board.black_kings |= 1ull << 61;

    board.red_pieces |= 1ull << 36;

    return board;
}

Board test_win_condition_red() {
    Board board = { 0, 0 };

    board.black_pieces |= 1ull << 16;

    board.red_pieces |= 1ull << 34;
    board.red_kings |= 1ull << 4;

    return board;
}

Board test_draw() {
    Board board = { 0, 0 };

    board.black_pieces |= 1ull << 59;
    board.black_pieces |= 1ull << 61;
    board.black_pieces |= 1ull << 43;

    board.red_pieces |= 1ull << 2;

    return board;
}

Board test_draw_ai() {
    Board board = { 0, 0 };

    board.black_pieces |= 1ull << 59;
    board.black_pieces |= 1ull << 61;
    board.black_pieces |= 1ull << 43;

    board.red_pieces |= 1ull << 0;
    board.red_pieces |= 1ull << 2;
    board.red_pieces |= 1ull << 16;

    return board;
}

Board test_ai_kinging() {
    Board board = { 0, 0 };

    board.black_pieces |= 1ull << 45;

    board.red_pieces |= 1ull << 9;

    return board;
}
