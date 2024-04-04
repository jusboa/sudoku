#include "sudoku_common.h"
#include <stdio.h>

void printMatrix(const uint8_t* matrix, int size) {
    printf("\n");
    for (int row = 0; row < size; ++row) {
        for (int column = 0; column < size; ++column) {
            printf("%d ", *(matrix + column + row * size));
        }
        printf("\n");
    }
}
