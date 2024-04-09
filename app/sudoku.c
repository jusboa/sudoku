#include "sudoku.h"
#include "logging.h"
#include <string.h>

#define CELLS_COUNT (SUDOKU_SIZE * SUDOKU_SIZE)
#define SUBMATRIX_BASE_MASK (0x0007)

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

static bool isMatrixValid() {
    for (uint8_t num = 1; num <= SUDOKU_SIZE; ++num) {
        uint16_t rowSum = 0;
        uint16_t subMatrix1Sum,
            subMatrix2Sum,
            subMatrix3Sum;
        for (int row = 0; row < SUDOKU_SIZE; ++row) {
            /* Check rows for the duplicates */
            uint16_t rowValue = binMatrix[num-1][row];
            /* Clear the rightmost bit */
            rowValue &= rowValue - 1;
            if (rowValue > 0) {
                /* There is another bit set - repeated row value */
                return false;
            }

            /* Check columns for the duplicates */
            /* rowSum collects bits from columns */
            if ((binMatrix[num-1][row] & rowSum) != 0) {
                /* repeated value within a column */
                return false;
            }
            rowSum |= binMatrix[num-1][row];

            /* Check submatrices for duplicates */
            if ((row % SUBMATRIX_SIZE) == 0) {
                subMatrix1Sum = subMatrix2Sum = subMatrix3Sum = 0;
            }
            subMatrix1Sum |= (binMatrix[num-1][row] & SUBMATRIX_BASE_MASK);
            subMatrix2Sum |= (binMatrix[num-1][row] & (SUBMATRIX_BASE_MASK << SUBMATRIX_SIZE));
            subMatrix3Sum |= (binMatrix[num-1][row] & (SUBMATRIX_BASE_MASK << (2 * SUBMATRIX_SIZE)));
            if ((subMatrix1Sum & (subMatrix1Sum - 1)) > 0
                || (subMatrix2Sum & (subMatrix2Sum - 1)) > 0
                || (subMatrix3Sum & (subMatrix3Sum - 1)) > 0) {
                /* repeated value within a submatrix */
                return false;
            }
        }
    }
    return true;
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
    uint16_t submatrixRowMask = SUBMATRIX_BASE_MASK << columnOffset;
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

static bool decMatrixToBin(uint8_t matrix[][SUDOKU_SIZE]) {
    memset(binMatrix, 0, CELLS_COUNT * sizeof(binMatrix[0][0]));
    for (int row = 0; row < SUDOKU_SIZE; ++row) {
        for (int column = 0; column < SUDOKU_SIZE; ++column) {
            uint8_t value = matrix[row][column];
            if (value != 0) {
                binMatrix[value-1][row] |= (1 << column);
            }
        }
    }
}

static void binMatrixToDec(uint8_t matrix[][SUDOKU_SIZE]) {
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

static bool backTrack(uint8_t matrix[][SUDOKU_SIZE]) {
    int row = 0;
    int column = 0;
    uint8_t* firstEmpty = NULL;
    for (int row = 0; row < SUDOKU_SIZE; ++row) {
        for (int column = 0; column < SUDOKU_SIZE; ++column) {
            if (matrix[row][column] == 0) {
                firstEmpty = &matrix[row][column];
                break;
            }
        }
    }
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
            if (firstEmpty == &matrix[row][column]) {
                /* We've already tried everything */
                return false;
            }
            /* Wrong path, erase and go back. */
            if (numOld != 0) {
                binMatrix[numOld-1][row] &= ~(1 << column);
            }
            backward(matrix, &row, &column);
        }
    }
    return true;
}

bool sudoku_solve(uint8_t matrix[][SUDOKU_SIZE]) {
    decMatrixToBin(matrix);
    if (!isMatrixValid()) {
        return false;
    }
    if (!backTrack(matrix)) {
        return false;
    }
    binMatrixToDec(matrix);
    return true;
}
