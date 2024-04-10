#ifndef SUDOKU_COM_H
#define SUDOKU_COM_H

#include "sudoku.h"
#include <stdbool.h>
#include <stdint.h>

/* A callback(slot) to notify client that a new sudoku has been received. */
typedef void (*newSudokuMatrixSlot_t)(const uint8_t matrix[][SUDOKU_SIZE]);

/* Initialize the module. */
void sudokuCom_init();
/* Register a new sudoku callback. */
void sudokuCom_registerNewSudokuMatrixSlot(newSudokuMatrixSlot_t slot);
/* Send a given message. Blocking. */
void sudokuCom_send(const char* message, unsigned int size);
/* Send a given sudoku. Blocking. */
void sudokuCom_sendSudokuMatrix(const uint8_t matrix[][SUDOKU_SIZE]);
/* Do periodic tasks. */
void sudokuCom_wakeUp();

#endif  /* SUDOKU_COM_H */
