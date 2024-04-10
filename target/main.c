#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#include "driverlib/sysctl.h"
#include "driverlib/rom.h"
#include "sudokuCom.h"
#include "sudoku.h"

/* The error routine that is called if the driver library encounters an error. */
#ifdef DEBUG
void
__error__(char *pcFilename, uint32_t ui32Line) {
}
#endif

static const uint8_t (*sudokuMatrix)[SUDOKU_SIZE];

static const char DATA_ERROR[] = "Invalid sudoku data!";
static const char NO_SOLUTION[] = "Could not solve the sudoku!";

/* Simple task management. */
static volatile unsigned int task;
enum Task {
    TASK_RX,
    TASK_FIRST = TASK_RX,
    TASK_SOLVE,
    TASK_TX,
    TASK_LAST = TASK_TX
};

static void newSudokuMatrix(const uint8_t* matrix) {
    sudokuMatrix = matrix;
    task = TASK_SOLVE;
}

void init() {
    /* Set the clocking to run directly from the crystal. */
    ROM_SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN |
                       SYSCTL_XTAL_16MHZ);
    sudokuCom_init();
    sudokuCom_registerNewSudokuMatrixSlot(newSudokuMatrix);
}

int main(void) {
    init();
    while(true) {
        switch (task) {
        case TASK_RX:
            sudokuCom_wakeUp();
            /* Advance to the next task in newSudokuMatrix() slot. */
            break;
        case TASK_SOLVE:
            if (sudokuMatrix == NULL) {
                sudokuCom_send(DATA_ERROR, sizeof(DATA_ERROR));
                task = TASK_RX;
                break;
            }
            if (!sudoku_solve(sudokuMatrix)) {
                sudokuCom_send(NO_SOLUTION, sizeof(NO_SOLUTION));
                task = TASK_RX;
            } else {
                task = TASK_TX;
            }
            break;
        case TASK_TX: {
            sudokuCom_sendSudokuMatrix(sudokuMatrix);
            task = TASK_RX;
            break;
        }
        }
    }
}
