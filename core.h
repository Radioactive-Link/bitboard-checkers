#pragma once

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>
#include <stdlib.h>

typedef struct {
    uint64_t red_pieces;
    uint64_t black_pieces;
    uint64_t red_kings;
    uint64_t black_kings;
} Board;

typedef enum game_pieces {
    RED,
    RED_KING,
    BLACK,
    BLACK_KING,
    NONE
} Pieces;

typedef struct {
    uint64_t moves;
    uint64_t captures;
} MoveSummary;

typedef enum sides {
    RED_SIDE,
    BLACK_SIDE,
} Sides;

typedef struct {
    int column;
    int row;
} Position;

typedef enum {
    MOVE,
    CAPTURE,
} MoveKind;

typedef struct {
    int ranking; // higher = better
    MoveKind kind;
    Position from;
    Position to;
    Pieces piece;
} Move;

/**
 * All useful information about the current game state at this exact point in time.
 * Moves are respective to whose turn it is.
 */
typedef struct {
    Pieces turn;
    Board* board;
    Move* possible_moves;
    uint8_t move_size;
} StaticGameState;

typedef enum {
    FAIL,
    SUCCESS,
    STALEMATE // no valid move exists.
} MoveResult;

/* --- declarations -------------------------------------------------------- */
void capture_piece(Board* board, uint8_t at_position);
uint8_t get_captured_position(Position from, Position to);
uint64_t* get_matching_side_from_piece(Board* board, Pieces piece);
void take_move(Board* board, Move move);
uint8_t coord_to_bit_position(int x, int y);
uint8_t position_to_bit_position(Position pos);
Pieces get_piece_kind(Pieces piece);
bool is_king(Pieces piece);
bool can_take(Pieces attacker, Pieces victim);
int check_move_rec(
    Board board,
    Position from,
    Pieces piece,
    int x_offset,
    int y_offset,
    MoveSummary* valid,
    Move* moves,
    uint8_t depth
);
int check_move(
    Board board,
    Position from,
    Pieces piece,
    int x_offset,
    int y_offset,
    MoveSummary* valid,
    Move* moves
);
MoveSummary valid_moves(Board board, Pieces piece, Position from, Move* moves, int* moves_found);
MoveSummary* all_valid_moves(Board board, Sides side, Move* moves, int* moves_found);
uint64_t merge_captures(MoveSummary* moves);
bool move(Board* board, Position from, Position to, Sides turn);
bool is_in_bounds(Position from);
void remove_piece(uint64_t* pieces, uint8_t pos);
void set_piece(uint64_t* pieces, uint8_t pos);
Pieces get_piece(Board board, int position);
uint8_t get_bit(uint64_t pieces, int position);
void print_bit_board(uint64_t pieces);
bool win_condition(Board board);
void handle_kinging(Board* board, Position at_pos, Sides turn);
bool is_stalemate(Board board, Sides side);

/* --- implementations ----------------------------------------------------- */

void capture_piece(Board* board, uint8_t at_position) {
    Pieces to_capture = get_piece(*board, at_position);
    if (to_capture != NONE) {
        uint64_t* pieces = get_matching_side_from_piece(board, to_capture);
        remove_piece(pieces, at_position);
    }
}

uint8_t get_captured_position(Position from, Position to) {
    Position between = { 
        from.column > to.column ? from.column - 1 : from.column + 1,
        from.row > to.row ? from.row -1 : from.row + 1
    };
    return position_to_bit_position(between);
}

uint64_t* get_matching_side_from_piece(Board* board, Pieces piece) {
    assert(piece != NONE);

    switch(piece) {
    case BLACK:
        return &board->black_pieces;
    case BLACK_KING:
        return &board->black_kings;
    case RED:
        return &board->red_pieces;
    case RED_KING:
        return &board->red_kings;
    }
    return NULL;
}

void take_move(Board* board, Move move) {
    switch (move.kind) {
    case CAPTURE:
        uint8_t captured_pos = get_captured_position(move.from, move.to);
        capture_piece(board, captured_pos);
    case MOVE:
        uint64_t* moved_pieces = get_matching_side_from_piece(board, move.piece);
        set_piece(moved_pieces, position_to_bit_position(move.to));
        remove_piece(moved_pieces, position_to_bit_position(move.from));
    }
    // this method is only used for ai moves, so the side is always red.
    handle_kinging(board, move.to, RED_SIDE);
}

uint8_t coord_to_bit_position(int x, int y) {
    // value increases by # of columns as it goes up the rows
    return x + y * (BOARD_COL_END + 1);
}

uint8_t position_to_bit_position(Position pos) {
    return coord_to_bit_position(pos.column, pos.row);
}

Pieces get_piece_kind(Pieces piece) {
    if (piece == NONE) return piece;
    return piece == RED || piece == RED_KING ? RED : BLACK;
}

Sides get_piece_side(Pieces piece) {
    return piece == RED || piece == RED_KING ? RED_SIDE : BLACK_SIDE;
}

bool is_king(Pieces piece) {
    return piece == RED_KING || piece == BLACK_KING;
}

bool can_take(Pieces attacker, Pieces victim) {
    // kings can take other kings
    if (is_king(attacker)) return true;
    // normal pieces can not take kings
    if (is_king(victim)) return false;
    // normal pieces can take other normal pieces
    return true;
}

/**
 * Recursively checks all valid moves for a given piece `piece` at position
 * `from` in a given direction made using the offsets passed in.
 */
int check_move_rec(
    Board board,
    Position from,
    Pieces piece,
    int x_offset,
    int y_offset,
    MoveSummary* valid,
    Move* moves,
    uint8_t depth
) {
    Position new = { from.column + x_offset, from.row + y_offset };
    // printf("[%u] Checking new pos of: %i, %i\n", depth, new.column, new.row);
    Pieces piece_kind = get_piece_kind(piece);
    int moves_idx = 0;
    if (is_in_bounds(new)) {
        uint8_t temp_pos = position_to_bit_position(new);
        Pieces target_piece = get_piece(board, temp_pos);
        // printf(" |> Piece at that spot: %u\n", target_piece);
        Pieces target_piece_kind = get_piece_kind(target_piece);
        if (target_piece_kind != NONE &&
            target_piece_kind != piece_kind &&
            can_take(piece, target_piece)
        ) {
            // check capture in direction
            // limit depth bc a piece can not jump over more than one piece in a row
            if (depth < 1) {
                return check_move_rec(
                    board,
                    new,
                    piece,
                    x_offset,
                    y_offset,
                    valid,
                    moves,
                    depth + 1
                );
            }
            // no valid move
        // empty spot, safe to move there
        } else if (target_piece_kind == NONE) {
            // jumped over a piece to reach this empty spot
            if (depth > 0) {
                set_piece(&valid->captures, temp_pos);
                // printf("  Taking capture move. Capture: %u\n", valid->captures);
                if (moves != NULL) {
                    // here, from is the captured piece, so the original spot is before
                    // that on the opposite trajectory
                    Position previous = { 
                        from.column - x_offset,
                        from.row - y_offset
                    };
                    Move move = {
                        .ranking = 1,
                        .kind = CAPTURE,
                        .from = previous,
                        .to = new,
                        .piece = piece
                    };
                    moves[moves_idx] = move;
                    // printf("    -> from: (%u, %u), to: (%u, %u)\n", move.from.column, move.from.row, move.to.column, move.to.row);
                }
                // increment even if moves is NULL so that just the moves_found param
                // in valid_moves can be used if wanted.
                moves_idx++;
            } else {
                // normal move
                set_piece(&valid->moves, temp_pos);
                // printf("  Taking normal move.\n");
                if (moves != NULL) {
                    Move move = {
                        .ranking = 0,
                        .kind = MOVE,
                        .from = from,
                        .to = new,
                        .piece = piece
                    };
                    moves[moves_idx] = move;
                }
                moves_idx++;
            }
            return moves_idx;
        }
    } else {
        // printf("   Out of bounds. Ignoring\n");
    }
    return 0;
}

int check_move(
    Board board,
    Position from,
    Pieces piece,
    int x_offset,
    int y_offset,
    MoveSummary* valid,
    Move* moves
) {
    return check_move_rec(
        board, from, piece, x_offset, y_offset, valid, moves, 0);
}

/** 
 * Returns what positions this `piece` can move to from `from` as a bitboard.
 * Also populates `moves` with all valid `Move`s along with the amount
 * allocated in `moves_found` for greater specificity.
 * Either or both pointer parameters may be null.
 */
MoveSummary valid_moves(Board board, Pieces piece, Position from, Move* moves, int* moves_found) {
    assert(piece != NONE);

    int x = from.column;
    int y = from.row;
    MoveSummary valid = { 0, 0 };
    // black pieces move forwards, red back
    // orientation stays the same between games
    int y_offset = get_piece_kind(piece) == RED ? -1 : 1;
    
    // standard moves
    int n = check_move(board, from, piece, -1, y_offset, &valid, moves); // left
    n += check_move(board, from, piece, 1, y_offset,
                    &valid, moves != NULL ? moves + n : NULL);  // right
    // king moves
    if (is_king(piece)) {
        n += check_move(board, from, piece, -1, -y_offset,
                        &valid, moves != NULL ? moves + n : NULL);
        n += check_move(board, from, piece, 1, -y_offset,
                        &valid, moves != NULL ? moves + n : NULL);
    }
    if (moves_found != NULL) *moves_found += n;

    return valid;
}

MoveSummary* all_valid_moves(Board board, Sides side, Move* moves, int* moves_found) {
    MoveSummary* valid_move = (MoveSummary*)malloc(MAX_VALID_MOVE_SIZE * sizeof(MoveSummary));
    if (valid_move == NULL) {
        printf("[FATAL]: Failed to allocate memory for valid_moves");
        exit(1);
    }

    uint8_t idx = 0;
    int moves_memory_offset = 0;
    Position from = { 0, 0 };
    for (int pos = 0; pos < BOARD_SIZE; pos++) {
        // printf("moves allocated: %d\n", moves_memory_offset);
        Pieces at_pos = get_piece(board, pos);
        // printf("at_pos: %u =======\n", at_pos);

        if (at_pos != NONE && get_piece_side(at_pos) == side)
            valid_move[idx++] = valid_moves(
                board, at_pos, from,
                moves != NULL ? moves + moves_memory_offset : NULL,
                &moves_memory_offset);

        // keep `from` accurate to `pos`
        from.column++;
        // cleanly wrap around to next row
        if (from.column > BOARD_COL_END) {
            from.column = 0;
            from.row++;
        }
    }
    for (int i = idx; i < MAX_VALID_MOVE_SIZE; i++) {
        MoveSummary empty = { 0, 0 };
        valid_move[i] = empty;
    }
    // printf("idx: %u\n", idx);

    if (moves_found != NULL) *moves_found = moves_memory_offset;
    return valid_move;
}

uint64_t merge_captures(MoveSummary* moves) {
    uint64_t merged = 0;
    for (int i = 0; i < MAX_VALID_MOVE_SIZE; i++) {
        merged |= moves[i].captures;
    }
    return merged;
}

/* Returns true if the move was successful. */
bool move(Board* board, Position from, Position to, Sides turn) {
    int from_bit = position_to_bit_position(from);
    Pieces to_move = get_piece(*board, from_bit);

    MoveSummary valid_move = valid_moves(*board, to_move, from, NULL, NULL);
    MoveSummary* all_valid_move = all_valid_moves(*board, turn, NULL, NULL);

    uint8_t to_bit = position_to_bit_position(to);
    uint64_t new_board = 1ull << to_bit;
    // will be > 0 if a capture exists
    uint64_t merged = merge_captures(all_valid_move);
    free(all_valid_move);

    // has capture but did not take it
    if (merged && !(new_board & valid_move.captures)) {
        printf("\n+-- +-- +-- +--~~~~~~~~~--+ --+ --+ --+\n");
        printf("|    Capture available. Must take.    |\n");
        printf("+-- +-- +-- +--~~~~~~~~~--+ --+ --+ --+\n");
        return false;
    }
    // printf("captures:  %u\n", valid_move.captures);
    // printf("moves:     %u\n", valid_move.moves);
    // printf("new board: %u\n", new_board);
    // moves aligns, take it
    if (new_board & (valid_move.captures | valid_move.moves)) {
        // figure out what board to update
        uint64_t* to_move_pieces = get_matching_side_from_piece(board, to_move);
        // remove old
        remove_piece(to_move_pieces, from_bit);
        // move new
        set_piece(to_move_pieces, to_bit);
        handle_kinging(board, to, turn);

        // did not capture
        // printf("capture mask: %u", !(new_board & valid_move.captures));
        if (!(new_board & valid_move.captures)) return true;

        capture_piece(board, get_captured_position(from, to));
        return true;
    }

    printf("\n+-- +-- +-- +--~~~~~~~~~--+ --+ --+ --+\n");
    printf("|     Invalid Move. Pick another.     |\n");
    printf("+-- +-- +-- +--~~~~~~~~~--+ --+ --+ --+\n");
    return false;
}

bool is_in_bounds(Position from) {
    int r = from.row;
    int c = from.column;
    return !(
        r < BOARD_ROW_START ||
        r > BOARD_ROW_END   ||
        c < BOARD_COL_START ||
        c > BOARD_COL_END
    );
}

bool win_condition(Board board) {
    uint64_t black_side = board.black_pieces | board.black_kings;
    uint64_t red_side = board.red_pieces | board.red_kings;
    uint64_t remaining_pieces = black_side ^ red_side;
    if (remaining_pieces == red_side) {
        printf("Computer won!\n");
        return true;
    } else if (remaining_pieces == black_side) {
        printf("You... won? :(\n");
        return true;
    }
    return false;
}

void handle_kinging(Board* board, Position at_pos, Sides turn) {
    uint8_t at_bit = position_to_bit_position(at_pos);
    if (turn == RED_SIDE && at_pos.row == BOARD_ROW_START) {
        // replace with king
        remove_piece(&board->red_pieces, at_bit);
        set_piece(&board->red_kings, at_bit);
    } else if (turn == BLACK_SIDE && at_pos.row == BOARD_ROW_END) {
        remove_piece(&board->black_pieces, at_bit);
        set_piece(&board->black_kings, at_bit);
    }
}

bool is_stalemate(Board board, Sides side) {
    int moves_found = 0;
    free(all_valid_moves(board, side, NULL, &moves_found));
    return moves_found <= 0;
}

/* --- Bit operations ------------------------------------------------------ */
void remove_piece(uint64_t* pieces, uint8_t pos) {
    assert(pos >= 0 && pos < 64);
    *pieces &= ~(1ull << pos);
}

void set_piece(uint64_t* pieces, uint8_t pos) {
    assert(pos >= 0 && pos < 64);
    *pieces |= 1ull << pos;
}

Pieces get_piece(Board board, int position) {
    assert(position >= 0 && position < 64);
    uint64_t mask = 1ull << position;
    if (board.black_pieces & mask) return BLACK;
    if (board.black_kings & mask) return BLACK_KING;
    if (board.red_pieces & mask) return RED;
    if (board.red_kings & mask) return RED_KING;
    return NONE;
}

/** Returns 1 or 0 based on whether a bit is set at a given `position` */
uint8_t get_bit(uint64_t pieces, int position) {
    assert(position >= 0 && position < 64);
    return (pieces >> position) & 1;
}

void print_bit_board(uint64_t pieces) {
    printf("\n");
    for (int i = BOARD_ROW_END; i >= 0; i--) {
        printf("|");
        for (int j = 0; j <= BOARD_COL_END; j++) {
            printf(" %u |", get_bit(pieces, i * 8 + j));
        }
        printf("\n");
    }
    printf("\n");
}