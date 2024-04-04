#include "sudoku.h"
#include "logging.h"
#include <string.h>

#define SUBMATRIX_SIZE 3 /* == sqrt(SUDOKU_SIZE) */
#define CELLS_COUNT (SUDOKU_SIZE * SUDOKU_SIZE)

static unsigned long long iterations;
static uint8_t matrixOrig[SUDOKU_SIZE][SUDOKU_SIZE];

bool isLast(uint8_t row, uint8_t column) {
    return row >= (SUDOKU_SIZE - 1)
        && column >= (SUDOKU_SIZE - 1);
}

bool isFirst(uint8_t row, uint8_t column) {
    return row == 0 && column == 0;
}

static void next(uint8_t* row, uint8_t* column) {
    if (*column < (SUDOKU_SIZE - 1)) {
        ++(*column);
    } else {
        if (*row < ((SUDOKU_SIZE - 1))) {
            ++(*row);
            *column = 0;
        }
    }
}

static void previous(uint8_t* row, uint8_t* column) {
    if (*column > 0) {
        --(*column);
    } else {
        if (*row > 0) {
            --(*row);
            *column = SUDOKU_SIZE - 1;
        }
    }
}

static bool isValidEntry(const uint8_t matrix[][SUDOKU_SIZE],
                         uint8_t entryRow, uint8_t entryColumn,
                         uint8_t entry) {
    if (entry == 0 || entry > SUDOKU_SIZE) {
        return false;
    }

    /* Check row for the duplicate. */
    for (int column = 0; column < SUDOKU_SIZE; ++column) {
        if (matrix[entryRow][column] == entry) {
            return false;
        }
    }
 
    /* Check column for the duplicate. */
    for (int row = 0; row < SUDOKU_SIZE; ++row) {
        if (matrix[row][entryColumn] == entry) {
            return false;
        }
    }
 
    /* Check submatrix for the duplicate. */
    int rowOffset = entryRow - entryRow % SUBMATRIX_SIZE, 
        columnOffset = entryColumn - entryColumn % SUBMATRIX_SIZE;
    for (int i = 0; i < SUBMATRIX_SIZE; ++i) {
        for (int j = 0; j < SUBMATRIX_SIZE; ++j) {
            if (matrix[i + rowOffset][j + columnOffset] == entry) {
                return false;
            }
        }
    }
    return true;
}

/* Go backward and find the first unknown cell (== 0). */
static void backward(uint8_t* row, uint8_t* column) {
    do {
        previous(row, column);
    } while (matrixOrig[*row][*column] != 0
             && !isFirst(*row, *column));
}

static void backTrack(uint8_t matrix[][SUDOKU_SIZE]) {
    uint8_t row = 0;
    uint8_t column = 0;
    while (true) {
        if (matrixOrig[row][column] != 0) {
            if (isLast(row, column)) {
                break;
            } else {
                next(&row, &column);
                continue;
            }
        }
        uint8_t num = matrix[row][column] + 1;
        for (; num <= SUDOKU_SIZE; ++num) {
            if (isValidEntry(matrix, row, column, num)) {
                break;
            }
        }
        if (num <= SUDOKU_SIZE) {
            /* Valid entry found. */
            matrix[row][column] = num;
            if (isLast(row, column)) {
                /* Finished */
                break;
            } else {
                next(&row, &column);
            }
        } else {
            /* Wrong path, erase and go back. */
            matrix[row][column] = 0;
            backward(&row, &column);
        }
        ++iterations;
    }
}

bool sudoku_solve(uint8_t matrix[][SUDOKU_SIZE]) {
    iterations = 0;
    memcpy(matrixOrig, matrix, CELLS_COUNT * sizeof(uint8_t));
    backTrack(matrix);
    LOG("\niterations = %lld", iterations);
    return true;
}
