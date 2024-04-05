#include "sudoku.h"
#include "logging.h"
#include <string.h>

#define SUBMATRIX_SIZE 3 /* == sqrt(SUDOKU_SIZE) */
#define CELLS_COUNT (SUDOKU_SIZE * SUDOKU_SIZE)

static uint16_t binMatrix[SUDOKU_SIZE][SUDOKU_SIZE];

inline static bool isLast(int row, int column) {
    return row >= (SUDOKU_SIZE - 1)
        && column >= (SUDOKU_SIZE - 1);
}

inline static bool isFirst(int row, int column) {
    return row == 0 && column == 0;
}

inline static void next(int* row, int* column) {
    if (*column < (SUDOKU_SIZE - 1)) {
        ++(*column);
    } else {
        if (*row < ((SUDOKU_SIZE - 1))) {
            ++(*row);
            *column = 0;
        }
    }
}

inline static void previous(int* row, int* column) {
    if (*column > 0) {
        --(*column);
    } else {
        if (*row > 0) {
            --(*row);
            *column = SUDOKU_SIZE - 1;
        }
    }
}

inline static bool isEntryValid(int entryRow, int entryColumn,
                                uint8_t entry) {
    if (entry == 0 || entry > SUDOKU_SIZE) {
        return false;
    }
    /* Check row for the duplicate. */
    if (binMatrix[entry-1][entryRow] != 0) {
        return false;
    }
    /* Check column for the duplicate. */
    uint16_t columnMask = 1 << entryColumn;
    for (int row = 0; row < SUDOKU_SIZE; ++row) {
        if ((binMatrix[entry-1][row] & columnMask) != 0) {
            return false;
        }
    }
    /* Check submatrix for the duplicate. */
    int rowOffset = entryRow - entryRow % SUBMATRIX_SIZE, 
        columnOffset = entryColumn - entryColumn % SUBMATRIX_SIZE;
    uint16_t submatrixRowMask = 0x07 << columnOffset;
    for (int i = 0; i < SUBMATRIX_SIZE; ++i) {
        if ((binMatrix[entry-1][i + rowOffset] & submatrixRowMask) != 0) {
            return false;
        }
    }
    return true;
}

inline static uint8_t binMatrixValue(int row, int column) {
    uint16_t mask = 1 << column;
    uint8_t value;
    for (value = 1; value <= SUDOKU_SIZE; ++value) {
        if ((binMatrix[value-1][row] & mask)  != 0) {
            return value;
        }
    }
    return 0;
}

static void matrixDecToBin(uint8_t matrix[][SUDOKU_SIZE]) {
    for (int row = 0; row < SUDOKU_SIZE; ++row) {
        for (int column = 0; column < SUDOKU_SIZE; ++column) {
            if (matrix[row][column] != 0) {
                binMatrix[matrix[row][column]-1][row] |= (1 << column);
            }
        }
    }
}

static void matrixBinToDec(uint8_t matrix[][SUDOKU_SIZE]) {
    for (int row = 0; row < SUDOKU_SIZE; ++row) {
        for (int column = 0; column < SUDOKU_SIZE; ++column) {
            matrix[row][column] = binMatrixValue(row, column);
        }
    }
    
}

/* Go backward and find the first unknown cell (== 0). */
inline static void backward(const uint8_t matrix[][SUDOKU_SIZE],
                            int* row, int* column) {
    do {
        previous(row, column);
    } while (matrix[*row][*column] != 0
             && !isFirst(*row, *column));
}

static void backTrack(uint8_t matrix[][SUDOKU_SIZE]) {
    int row = 0;
    int column = 0;
    while (true) {
        if (matrix[row][column] != 0) {
            if (isLast(row, column)) {
                break;
            } else {
                next(&row, &column);
                continue;
            }
        }
        uint8_t numOld = binMatrixValue(row, column);
        uint8_t num = numOld + 1;
        for (; num <= SUDOKU_SIZE; ++num) {
            if (isEntryValid(row, column, num)) {
                break;
            }
        }
        if (num <= SUDOKU_SIZE) {
            /* Valid entry found. */
            if (numOld != 0) {
                binMatrix[numOld-1][row] &= ~(1 << column);
            }
            binMatrix[num-1][row] |= (1 << column);
            if (isLast(row, column)) {
                /* Finished */
                break;
            } else {
                next(&row, &column);
            }
        } else {
            /* Wrong path, erase and go back. */
            if (numOld != 0) {
                binMatrix[numOld-1][row] &= ~(1 << column);
            }
            backward(matrix, &row, &column);
        }
    }
}

bool sudoku_solve(uint8_t matrix[][SUDOKU_SIZE]) {
    matrixDecToBin(matrix);
    backTrack(matrix);
    matrixBinToDec(matrix);
    return true;
}
