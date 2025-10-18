#include <assert.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#include "scenarios.h"
#include "config.h"
#include "core.h"
#include "ai.h"

// declarations
void print_board(Board board);

/* --- Main Program -------------------------------------------------------- */
int main() {
    // set random seed
    srand(time(NULL));
    printf(TITLE);

    // pick scenario from scenarios.h here
    Board board = standard_setup();
    bool should_exit = false;
    Sides turn = BLACK_SIDE;

    while (!should_exit) {
        // computer turn
        if (turn == RED_SIDE) {
            printf("+=====================================+\n");
            printf("|>~~~--     Computer's turn     --~~~<|\n");
            printf("+=====================================+\n");
            MoveResult res = take_ai_turn(&board, RED);
            if (res == SUCCESS) {
                turn = turn == RED_SIDE
                    ? BLACK_SIDE
                    : RED_SIDE;
            }
            if (res == STALEMATE) should_exit = true;
            continue;
        // computer just drew the game
        } else if (is_stalemate(board, turn)) {
            printf("Oops... I hoped to win... not draw");
            should_exit = true;
            continue;
        } else {
            printf("+=====================================+\n");
            printf("|>~~~--        Your turn        --~~~<|\n");
        }

        print_board(board);

        printf("What piece do you want to move?\n");
        char column;
        char row;

        while (true) {
            printf("Enter the column: ");
            scanf(" %c", &column);

            printf("Enter the row: ");
            scanf(" %c", &row);

            int c = column - 'a';
            int r = row - 'a';
            if (c < 0 || c > BOARD_COL_END ||
                r < 0 || r > BOARD_COL_END) {
                printf("\n+-- +-- +-- +--~~~~~~~~~--+ --+ --+ --+\n");
                printf("| Position out of bounds. Pick again. |\n");
                printf("+-- +-- +-- +--~~~~~~~~~--+ --+ --+ --+\n");
                continue;
            }

            // ensure it is the right piece
            Pieces to_move = get_piece(board, coord_to_bit_position(c, r));
            if (to_move == NONE || get_piece_side(to_move) != turn) {
                printf("\n+-- +-- +-- +--~~~~~~~~~--+ --+ --+ --+\n");
                printf("|     Not your piece. Pick again.     |\n");
                printf("+-- +-- +-- +--~~~~~~~~~--+ --+ --+ --+\n");
            } else {
                break;
            }
        }

        printf("Where do you want to move it to?\n");

        char to_column;
        char to_row;
        printf("Enter the column: ");
        scanf(" %c", &to_column);
        printf("Enter the row: ");
        scanf(" %c", &to_row);

        Position from = { column - 'a', row - 'a' };
        Position to = { to_column - 'a', to_row - 'a' };
        bool success = move(&board, from, to, turn);
        printf("\n");

        if (win_condition(board)) should_exit = true;

        // move was valid. Other player's turn now.
        if (success) {
            turn = turn == RED_SIDE ? BLACK_SIDE : RED_SIDE;
        }
    }
}

void print_board(Board board) {
    printf("|-----+-------------------------------+\n");
    for (int i = BOARD_ROW_END; i >= 0; i--) {
        int count = i % 2;
        printf("| %c > ", 'a' + i);
        for (int j = 0; j <= BOARD_COL_END; j++) {
            Pieces piece = get_piece(board, j + i * (BOARD_COL_END + 1));
            char* character;
            switch (piece) {
            case NONE:
                strcpy(character, count % 2 ? EMPTY_SPACE : FILLED_SPACE);
                break;
            case BLACK:
                strcpy(character, BLACK_PIECE_C);
                break;
            case RED:
                strcpy(character, RED_PIECE_C);
                break;
            case RED_KING:
                strcpy(character, RED_KING_C);
                break;
            case BLACK_KING:
                strcpy(character, BLACK_KING_C);
            }
            count++;
            count %= 2;

            printf(LEFT_SEPARATOR);
            printf(character);
            printf(" ");
        }
        printf(RIGHT_SEPARATOR);
        printf("\n|~ ~ ~+-------------------------------+\n");
    }
    printf("|     ");
    for (int i = 0; i <= BOARD_COL_END; i++) {
        printf(" ^%c ", 'a' + i);
    }
    printf("|\n");
    printf("+=====================================+\n");
}
