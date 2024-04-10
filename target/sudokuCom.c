#include "sudokuCom.h"
#include "serialCom.h"
#include <stdlib.h>

#define RX_DATA_MAX_SIZE 256

static const char SUBMATRIX_HORZ_SEPARATOR[] = "-----------\r\n";
static const char STOP_FRAME[] = "stop";
static unsigned int controlFrameIndex;

static char comData[RX_DATA_MAX_SIZE];
static unsigned int rxDataIndex;
static unsigned int rxDataSize;

static newSudokuMatrixSlot_t newSudokuMatrixSlot;
static uint8_t sudokuMatrix[SUDOKU_SIZE][SUDOKU_SIZE];

static void newDataAvailable(const char* data, unsigned int size) {
    while (size-- > 0) {
        if (*data == STOP_FRAME[controlFrameIndex]) {
            ++controlFrameIndex;
        } else {
            controlFrameIndex = 0;
        }
        if (controlFrameIndex > (sizeof(STOP_FRAME) - 2)) {
            controlFrameIndex = 0;
            /* Make sure the sudoku string is terminated. */
            rxDataIndex -= (sizeof(STOP_FRAME) - 2);
            rxDataSize = rxDataIndex;
            comData[rxDataSize] = '\0';
            rxDataIndex = 0;
            break;
        }
        if (rxDataIndex > (RX_DATA_MAX_SIZE - 1)) {
            controlFrameIndex = 0;
            rxDataIndex = 0;
            break;
        } else {
            comData[rxDataIndex] = *data;
            ++rxDataIndex;
        }
        ++data;
    }
}

void sudokuCom_registerNewSudokuMatrixSlot(newSudokuMatrixSlot_t slot) {
    newSudokuMatrixSlot = slot;
}

void sudokuCom_init() {
    serialCom_init();
    serialCom_registerNewDataSlot(newDataAvailable);
}

void sudokuCom_send(const char* message, unsigned int size) {
    serialCom_send(message, size);
    serialCom_send(STOP_FRAME, sizeof(STOP_FRAME));
}

static bool rxDataToMatrix(uint8_t matrix[][SUDOKU_SIZE],
                           const char* data,
                           unsigned int size) {
    unsigned int row = 0;
    unsigned int column = 0;
    while (size-- > 0) {
        if (*data >= '0' && *data <= '9') {
            uint8_t value = (uint8_t)(*data) - (uint8_t)('0');
            matrix[row][column] = value;
            if (++column > (SUDOKU_SIZE - 1)) {
                column = 0;
                if (++row > (SUDOKU_SIZE - 1)) {
                    return true;
                }
            }
        }
        ++data;
    }
    return false;
}

static unsigned int matrixToTxData(const uint8_t matrix[][SUDOKU_SIZE],
                                   char* data) {
    unsigned int size = 0;
    for (int row = 0; row < SUDOKU_SIZE; ++row) {
        if (row > 0 && row < (SUDOKU_SIZE - 1)
            && ((row % SUBMATRIX_SIZE) == 0)) {
            memcpy(data, SUBMATRIX_HORZ_SEPARATOR, sizeof(SUBMATRIX_HORZ_SEPARATOR));
            data += sizeof(SUBMATRIX_HORZ_SEPARATOR);
            size += sizeof(SUBMATRIX_HORZ_SEPARATOR);
        }
        for (int column = 0; column < SUDOKU_SIZE; ++column) {
            if (column > 0 && column < (SUDOKU_SIZE - 1)
                && ((column % SUBMATRIX_SIZE) == 0)) {
                /* add submatrix separator - not for the last column */
                *data++ = '|';
                ++size;
            }
            *data++ = (char)(matrix[row][column]) + '0';
            ++size;
        }
        *data++ = '\r';
        *data++ = '\n';
        size += 2;
    }
    return size;
}

void sudokuCom_sendSudokuMatrix(const uint8_t matrix[][SUDOKU_SIZE]) {
    unsigned int size = matrixToTxData(sudokuMatrix,
                                       comData);
    sudokuCom_send(comData, size);
}

void sudokuCom_wakeUp() {
    if (rxDataSize > 0) {
        bool result = rxDataToMatrix(sudokuMatrix,
                                     comData,
                                     rxDataSize);
        rxDataSize = 0;
        if (result) {
            newSudokuMatrixSlot(sudokuMatrix);
        } else {
            newSudokuMatrixSlot(NULL);
        }
    }
}
