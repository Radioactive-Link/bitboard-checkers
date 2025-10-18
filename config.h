#pragma once

/* --- Config -------------------------------------------------------------- */
// --- Display
const char* LEFT_SEPARATOR  = "| ";
const char* RIGHT_SEPARATOR = "|";
const char* EMPTY_SPACE     = " ";
const char* FILLED_SPACE    = "#";
const char* BLACK_PIECE_C   = "A";
const char* BLACK_KING_C    = "@";
const char* RED_PIECE_C     = "S";
const char* RED_KING_C      = "$";
const char* TITLE =
    " _____ _           _               \n"
    "|     | |_ ___ ___| |_ ___ ___ ___ \n"
    "|   --|   | -_|  _| '_| -_|  _|_ -|\n"
    "|_____|_|_|___|___|_,_|___|_| |___|\n";

// --- Board
// 8x8 grid considering 0-based indexing. Positive is right and up.
const int BOARD_ROW_START   = 0;
const int BOARD_ROW_END     = 7;
const int BOARD_COL_START   = 0;
const int BOARD_COL_END     = 7;
const int BOARD_SIZE        = 64;

// --- Misc
const int MAX_VALID_MOVE_SIZE = 40;