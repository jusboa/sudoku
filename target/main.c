#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_ints.h"
#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/interrupt.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom.h"
#include "serialCom.h"
#include "sudoku.h"

/* The error routine that is called if the driver library encounters an error. */
#ifdef DEBUG
void
__error__(char *pcFilename, uint32_t ui32Line) {
}
#endif

static uint8_t sudokuMatrix[SUDOKU_SIZE][SUDOKU_SIZE];

const char DATA_ERROR[] = "Invalid sudoku data!";
const char NO_SOLUTION[] = "Could not solve the sudoku!";
const char STOP_FRAME[] = "stop";
const char SUBMATRIX_HORZ_SEPARATOR[] = "-----------\r\n";

unsigned int controlFrameIndex;
#define RX_DATA_MAX_SIZE 256
static char comData[RX_DATA_MAX_SIZE];
unsigned int comDataIndex;
unsigned int comDataSize;

volatile unsigned int task;
enum Task {
    TASK_RX,
    TASK_FIRST = TASK_RX,
    TASK_CONVERT,
    TASK_SOLVE,
    TASK_TX,
    TASK_LAST = TASK_TX
};

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
            comDataIndex -= (sizeof(STOP_FRAME) - 2);
            comDataSize = comDataIndex;
            comData[comDataSize] = '\0';
            comDataIndex = 0;
            task = TASK_CONVERT;
            break;
        }
        if (comDataIndex > (RX_DATA_MAX_SIZE - 1)) {
            controlFrameIndex = 0;
            comDataIndex = 0;
            break;
        } else {
            comData[comDataIndex] = *data;
            ++comDataIndex;
        }
        ++data;
    }
}

void init() {
    /* Set the clocking to run directly from the crystal. */
    ROM_SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN |
                       SYSCTL_XTAL_16MHZ);
    serialCom_registerNewDataSlot(newDataAvailable);
    serialCom_init();

    /* Init GPIO */
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    ROM_GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_2);
}

static void sendMessage(const char* message, unsigned int size) {
    serialCom_send(message, size);
    serialCom_send(STOP_FRAME, sizeof(STOP_FRAME));
}

int main(void) {
    init();
    while(true) {
        switch (task) {
        case TASK_RX:
            /* see newDataAvailable */
            break;
        case TASK_CONVERT:
            if (!rxDataToMatrix(sudokuMatrix,
                                comData,
                                comDataSize)) {
                sendMessage(DATA_ERROR, sizeof(DATA_ERROR));
                task = TASK_RX;
            } else {
                task = TASK_SOLVE;
            }
            break;
        case TASK_SOLVE:
            if (!sudoku_solve(sudokuMatrix)) {
                sendMessage(NO_SOLUTION, sizeof(NO_SOLUTION));
                task = TASK_RX;
            } else {
                task = TASK_TX;
            }
            break;
        case TASK_TX: {
            unsigned int size = matrixToTxData(sudokuMatrix,
                                               comData);
            sendMessage(comData, size);
            task = TASK_RX;
            break;
        }
        }
    }
}
