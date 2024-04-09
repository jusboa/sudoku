#ifndef SUDOKU_H
#define SUDOKU_H

#include "sudoku_common.h"
#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define SUDOKU_SIZE 9
#define SUBMATRIX_SIZE 3 /* == sqrt(SUDOKU_SIZE) */

bool sudoku_solve(uint8_t matrix[][SUDOKU_SIZE]);

#ifdef __cplusplus
}
#endif

#endif  /* SUDOKU_H */
