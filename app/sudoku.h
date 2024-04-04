#ifndef SUDOKU_H
#define SUDOKU_H

#include "sudoku_common.h"
#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define SUDOKU_SIZE 9

bool sudoku_solve(uint8_t matrix[][SUDOKU_SIZE]);

#ifdef __cplusplus
}
#endif

#endif  /* SUDOKU_H */
