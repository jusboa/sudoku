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

void printBinaryMatrix(const uint16_t array[], int size) {
    printf("\n");
    int bitCount = sizeof(array[0]) * 8;
    char bin[2 * size];
    for (int i = 0; i < size; ++i) {
        for (int j = 0; j < size; ++j) {
            if ((array[i] & (1 << j)) != 0) {
                bin[2 * j] = '1';
            } else {
                bin[2 * j] = '0';
            }
            bin[2 * j + 1] = ' ';
        }
        bin[2 * size] = '\0';
        printf("%s\n", bin);
    }
}
