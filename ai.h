#pragma once

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#include "core.h"

/**
 * @file Ai implementations for the checkers game.
 * Implemented such that they do not make an invalid move, hence the lack of checks.
 */

/** 
 * Shortsighted algorithm that maximizes its returns this move.
 * Basically picks the first available move.
 */
MoveResult greedy_move(StaticGameState state) {
    Move* ms = state.possible_moves;
    // find move with the highest ranking
    Move max = ms[0];
    for (int i = 0; i < state.move_size; i++) {
        // if (ms[i].ranking != -1) {
        //     printf("|> Ranking: %d\n", ms[i].ranking);
        //     printf("|  From   : (%u, %u)\n", ms[i].from.column, ms[i].from.row);
        //     printf("|  To     : (%u, %u)\n", ms[i].to.column, ms[i].to.row);
        // }
        if (ms[i].ranking > max.ranking) {
            max = ms[i];
        }
    }

    take_move(state.board, max);

    return SUCCESS;
}

/** Selects a random valid move. */
MoveResult bogo_move(StaticGameState state) {
    Move* ms = state.possible_moves;
    int highest_ranking = ms[0].ranking;

    // figure out the highest ranking while counting amount in each rank
    for (int i = 0; i < state.move_size; i++) {
        if (ms[i].ranking > highest_ranking) {
            highest_ranking = ms[i].ranking;
        }
        // if (ms[i].ranking != -1) {
        //     printf("|> Ranking: %d\n", ms[i].ranking);
        //     printf("|  From   : (%u, %u)\n", ms[i].from.column, ms[i].from.row);
        //     printf("|  To     : (%u, %u)\n", ms[i].to.column, ms[i].to.row);
        // }
    }

    // pick a random move from the highest ranking set using reservoir sampling
    Move to_take = ms[0];
    int seen = 0;
    for (int i = 0; i < state.move_size; i++) {
        if (ms[i].ranking == highest_ranking) {
            seen++;
            // replace to_take with probability 1 / seen so that the
            // probability to choose a given move (of highest rank) is equal
            if ((rand() % seen) == 0) {
                to_take = ms[i];
            }
        }
    }

    take_move(state.board, to_take);

    return SUCCESS;
}

MoveResult take_ai_turn(Board* board, Pieces turn) {
    // generate static game state
    Move moves[MAX_VALID_MOVE_SIZE];
    Position empty_pos = { 0, 0 };
    Move empty_move = { -1, MOVE, empty_pos, empty_pos, RED };
    int moves_allocated;
    for (int i = 0; i < MAX_VALID_MOVE_SIZE; i++) moves[i] = empty_move;
    free(all_valid_moves(*board, turn, moves, &moves_allocated)); // discard unused result

    if (moves_allocated == 0) {
        printf("You forced us into a draw? Shame...\n");
        return STALEMATE;
    }

    StaticGameState state = {
        .turn = turn,
        .board = board,
        .possible_moves = moves,
        .move_size = moves_allocated
    };

    return bogo_move(state);
}
