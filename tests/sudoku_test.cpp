#include <gtest/gtest.h>
#include <gtest/gtest-spi.h>
#include <cstring>
#include <cmath>
#include <regex>
#include "sudoku.h"

using namespace std;

class SudokuTest : public testing::Test {
public:
    void SetUp() override {
        m_errorMessage = string();
    }

protected:
    static constexpr uint8_t m_sudoku[SUDOKU_SIZE][SUDOKU_SIZE] = {
        {5, 3, 0, 0, 7, 0, 0, 0, 0},
        {6, 0, 0, 1, 9, 5, 0, 0, 0},
        {0, 9, 8, 0, 0, 0, 0, 6, 0},
        {8, 0, 0, 0, 6, 0, 0, 0, 3},
        {4, 0, 0, 8, 0, 3, 0, 0, 1},
        {7, 0, 0, 0, 2, 0, 0, 0, 6},
        {0, 6, 0, 0, 0, 0, 2, 8, 0},
        {0, 0, 0, 4, 1, 9, 0, 0, 5},
        {0, 0, 0, 0, 8, 0, 0, 7, 9}};
    static constexpr int m_submatrixSize = static_cast<int>(sqrt(SUDOKU_SIZE));
    string m_errorMessage;

    bool isSudokuSolutionOk(const uint8_t original[][SUDOKU_SIZE],
                            const uint8_t solution[][SUDOKU_SIZE]) {
        string dummy;
        return isSudokuSolutionOk(original, solution, dummy);
    }
    bool isSudokuSolutionOk(const uint8_t original[][SUDOKU_SIZE],
                            const uint8_t solution[][SUDOKU_SIZE],
                            string& errorMessage) {
        int row, column;
        stringstream strStream;
        bool result = true;
        if (!isSolutionWithinBounds(solution, row, column)) {
            strStream << "Value " << int(solution[row][column])
                      << " at row " << row << ", column " << column
                      << " is out of bounds";
            result = false;
        } else if (!isOriginalMatch(original,
                             solution,
                             row,
                             column)) {
            strStream << "Mismatch of solution with original value at row " << row
                      << ", column " << column << endl
                      << int(original[row][column]) << " != "
                      << int(solution[row][column]);
            result = false;
        } else if (!isRowSingular(solution,
                           row,
                           column)) {
            strStream << "Row " << row << " contains element "
                      << int(solution[row][column]) << " more than once";
            result = false;
        } else if (!isColumnSingular(solution,
                              row,
                              column)) {
            strStream << "Column " << column << " contains element "
                      << int(solution[row][column]) << " more than once";
            result = false;
        } else if (!isSubmatrixSingular(solution,
                                 row,
                                 column)) {
            strStream << "Submatrix " << getSubmatrixIndex(row, column)
                      << " contains element " << int(solution[row][column])
                      << " more than once";
            result = false;
        } else {
            result = true;
        }

        if (!result) {
            strStream << endl << "Original: " << endl;
            printSudoku(original, strStream);
            strStream << endl << "Solution: " << endl;
            printSudoku(solution, strStream);
        }
        errorMessage = strStream.str();
        return result;
    }

    void printSudoku(const uint8_t sudoku[SUDOKU_SIZE][SUDOKU_SIZE],
                     stringstream& strStream) {
        for (int row = 0; row < SUDOKU_SIZE; ++row) {
            for (int column = 0; column < SUDOKU_SIZE; ++column) {
                strStream << int(sudoku[row][column]) << "\t";
            }
            strStream << endl;
        }
    }

    bool isSolutionWithinBounds(const uint8_t solution[SUDOKU_SIZE][SUDOKU_SIZE],
                                int& invalidRow,
                                int& invalidColumn) {
        for (int column = 0; column < SUDOKU_SIZE; ++column) {
            for (int row = 0; row < SUDOKU_SIZE; ++row) {
                if (!isValueWithinBounds(solution[row][column])) {
                    invalidRow = row;
                    invalidColumn = column;
                    return false;
                }
            }
        }
        return true;
    }
    bool isValueWithinBounds(uint8_t value) {
        return (value >= 1 && value <= SUDOKU_SIZE);
    }
    bool isRowSingular(const uint8_t solution[SUDOKU_SIZE][SUDOKU_SIZE],
                       int& invalidRow,
                       int& invalidColumn) {
        int counter[SUDOKU_SIZE][SUDOKU_SIZE] = {0};
        for (int row = 0; row < SUDOKU_SIZE; ++row) {
            for (int column = 0; column < SUDOKU_SIZE; ++column) {
                if (isValueWithinBounds(solution[row][column])
                    && ++counter[row][solution[row][column] - 1] > 1) {
                    invalidRow = row;
                    invalidColumn = column;
                    return false;
                }
            }
        }
        return true;
    }
    bool isColumnSingular(const uint8_t solution[SUDOKU_SIZE][SUDOKU_SIZE],
                          int& invalidRow,
                          int& invalidColumn) {
        int counter[SUDOKU_SIZE][SUDOKU_SIZE] = {0};
        for (int column = 0; column < SUDOKU_SIZE; ++column) {
            for (int row = 0; row < SUDOKU_SIZE; ++row) {
                if (isValueWithinBounds(solution[row][column])
                    && ++counter[column][solution[row][column] - 1] > 1) {
                    invalidRow = row;
                    invalidColumn = column;
                    return false;
                }
            }
        }
        return true;
    }
    int getSubmatrixIndex(int row, int column) {
        return (row / m_submatrixSize) * m_submatrixSize
            + (column / m_submatrixSize);
    }
    bool isSubmatrixSingular(const uint8_t solution[SUDOKU_SIZE][SUDOKU_SIZE],
                             int& invalidRow,
                             int& invalidColumn) {
        int counter[SUDOKU_SIZE][SUDOKU_SIZE] = {0};
        for (int column = 0; column < SUDOKU_SIZE; ++column) {
            for (int row = 0; row < SUDOKU_SIZE; ++row) {
                int submatrixIndex = getSubmatrixIndex(row, column);
                if (isValueWithinBounds(solution[row][column])
                    && ++counter[submatrixIndex][solution[row][column] - 1] > 1) {
                        invalidRow = row;
                        invalidColumn = column;
                        return false;
                }
            }
        }
        return true;
    }
    bool isOriginalMatch(const uint8_t originalMatrix[SUDOKU_SIZE][SUDOKU_SIZE],
                         const uint8_t newMatrix[SUDOKU_SIZE][SUDOKU_SIZE],
                         int& invalidRow,
                         int& invalidColumn) {
        for (int row = 0; row < SUDOKU_SIZE; ++row) {
            for (int column = 0; column < SUDOKU_SIZE; ++column) {
                if (originalMatrix[row][column] != 0
                    && originalMatrix[row][column] != newMatrix[row][column]) {
                    invalidRow = row;
                    invalidColumn = column;
                    return false;
                }
            }
        }
        return true;
    }
};

class SelfTest : public SudokuTest {};

TEST_F(SelfTest, validSolution) {
    uint8_t solution[SUDOKU_SIZE][SUDOKU_SIZE] = {
        {5, 3, 4, 6, 7, 8, 9, 1, 2},
        {6, 7, 2, 1, 9, 5, 3, 4, 8},
        {1, 9, 8, 3, 4, 2, 5, 6, 7},
        {8, 5, 9, 7, 6, 1, 4, 2, 3},
        {4, 2, 6, 8, 5, 3, 7, 9, 1},
        {7, 1, 3, 9, 2, 4, 8, 5, 6},
        {9, 6, 1, 5, 3, 7, 2, 8, 4},
        {2, 8, 7, 4, 1, 9, 6, 3, 5},
        {3, 4, 5, 2, 8, 6, 1, 7, 9}};
    EXPECT_TRUE(isSudokuSolutionOk(m_sudoku, solution));
}

TEST_F(SelfTest, validSolutionMismatchWithOriginal) {
    static uint8_t solution[SUDOKU_SIZE][SUDOKU_SIZE] = {
        {6, 3, 9, 5, 7, 4, 1, 8, 2},
        {5, 4, 1, 8, 2, 9, 3, 7, 6},
        {7, 8, 2, 6, 1, 3, 9, 5, 4},
        {1, 9, 8, 4, 6, 7, 5, 2, 3},
        {3, 6, 5, 9, 8, 2, 4, 1, 7},
        {4, 2, 7, 1, 3, 5, 8, 6, 9},
        {9, 5, 6, 7, 4, 8, 2, 3, 1},
        {8, 1, 3, 2, 9, 6, 7, 4, 5},
        {2, 7, 4, 3, 5, 1, 6, 9, 8}};
    
    EXPECT_FALSE(isSudokuSolutionOk(m_sudoku, solution, m_errorMessage));
    EXPECT_TRUE(regex_search(m_errorMessage, regex("Mismatch of solution with original value")));
}

TEST_F(SelfTest, invalidSolution) {
    static uint8_t solution[SUDOKU_SIZE][SUDOKU_SIZE] = {
        {5, 3, 4, 6, 7, 8, 9, 1, 9/*2*/},
        {6, 7, 2, 1, 9, 5, 3, 4, 8},
        {1, 9, 8, 3, 4, 2, 5, 6, 7},
        {8, 5, 9, 7, 6, 1, 4, 2, 3},
        {4, 2, 6, 8, 5, 3, 7, 9, 1},
        {7, 1, 3, 9, 2, 4, 8, 5, 6},
        {9, 6, 1, 5, 3, 7, 2, 8, 4},
        {2, 8, 7, 4, 1, 9, 6, 3, 5},
        {3, 4, 5, 2, 8, 6, 1, 7, 9}};
    EXPECT_FALSE(isSudokuSolutionOk(m_sudoku, solution));
}

TEST_F(SelfTest, outofBoundValues) {
    static uint8_t solution[SUDOKU_SIZE][SUDOKU_SIZE] = {
        {5, 3, 4, 6, 7, 8, 9, 1, 2},
        {6, 7, 2, 1, 9, 5, 3, 4, 8},
        {1, 9, 8, 3, 4, 2, 5, 6, 7},
        {8, 5, 9, 7, 6, 1, 4, 2, 3},
        {4, 2, 6, 8, 0/*5*/, 3, 7, 9, 1},
        {7, 1, 3, 9, 2, 4, 8, 5, 6},
        {9, 6, 1, 5, 3, 7, 2, 8, 4},
        {2, 8, 7, 4, 1, 9, 6, 3, 5},
        {3, 4, 5, 2, 8, 6, 1, 7, 9}};
    
    EXPECT_FALSE(isSudokuSolutionOk(m_sudoku, solution, m_errorMessage));
    EXPECT_TRUE(regex_search(m_errorMessage, regex("out of bounds")));
}

TEST_F(SudokuTest, invalidSudoku_repeatedRowElement) {
    uint8_t sudokuOrig[SUDOKU_SIZE][SUDOKU_SIZE] = {
        {0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 3, 0, 8, 5},
        {0, 0, 1, 1/*0*/, 2, 0, 0, 0, 0},
        {0, 0, 0, 5, 0, 7, 0, 0, 0},
        {0, 0, 4, 0, 0, 0, 1, 0, 0},
        {0, 9, 0, 0, 0, 0, 0, 0, 0},
        {5, 0, 0, 0, 0, 0, 0, 7, 3},
        {0, 0, 2, 0, 1, 0, 0, 0, 0},
        {0, 0, 0, 0, 4, 0, 0, 0, 9}};
    uint8_t solution[SUDOKU_SIZE][SUDOKU_SIZE];
    memcpy(solution, sudokuOrig, SUDOKU_SIZE * SUDOKU_SIZE * sizeof(uint8_t));
    
    EXPECT_FALSE(sudoku_solve(solution));
}

TEST_F(SudokuTest, invalidSudoku_repeatedColumnElement) {
    uint8_t sudokuOrig[SUDOKU_SIZE][SUDOKU_SIZE] = {
        {0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 3, 0, 8, 5},
        {0, 0, 1, 5/*0*/, 2, 0, 0, 0, 0},
        {0, 0, 0, 5, 0, 7, 0, 0, 0},
        {0, 0, 4, 0, 0, 0, 1, 0, 0},
        {0, 9, 0, 0, 0, 0, 0, 0, 0},
        {5, 0, 0, 0, 0, 0, 0, 7, 3},
        {0, 0, 2, 0, 1, 0, 0, 0, 0},
        {0, 0, 0, 0, 4, 0, 0, 0, 9}};
    uint8_t solution[SUDOKU_SIZE][SUDOKU_SIZE];
    memcpy(solution, sudokuOrig, SUDOKU_SIZE * SUDOKU_SIZE * sizeof(uint8_t));
    
    EXPECT_FALSE(sudoku_solve(solution));
}

TEST_F(SudokuTest, invalidSudoku_repeatedSubmatrixElement) {
    uint8_t sudokuOrig[SUDOKU_SIZE][SUDOKU_SIZE] = {
        {0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 3, 0, 8, 5},
        {0, 0, 1, 0, 2, 0, 8/*0*/, 0, 0},
        {0, 0, 0, 5, 0, 7, 0, 0, 0},
        {0, 0, 4, 0, 0, 0, 1, 0, 0},
        {0, 9, 0, 0, 0, 0, 0, 0, 0},
        {5, 0, 0, 0, 0, 0, 0, 7, 3},
        {0, 0, 2, 0, 1, 0, 0, 0, 0},
        {0, 0, 0, 0, 4, 0, 0, 0, 9}};
    uint8_t solution[SUDOKU_SIZE][SUDOKU_SIZE];
    memcpy(solution, sudokuOrig, SUDOKU_SIZE * SUDOKU_SIZE * sizeof(uint8_t));
    
    EXPECT_FALSE(sudoku_solve(solution));
}

TEST_F(SudokuTest, solveSudoku) {
    uint8_t solution[SUDOKU_SIZE][SUDOKU_SIZE];
    memcpy(solution, m_sudoku, SUDOKU_SIZE * SUDOKU_SIZE * sizeof(uint8_t));
    
    EXPECT_TRUE(sudoku_solve(solution));
    bool result = isSudokuSolutionOk(m_sudoku, solution, m_errorMessage);
    EXPECT_TRUE(result) << m_errorMessage;
}

TEST_F(SudokuTest, backTrackingImmune) {
    uint8_t sudokuOrig[SUDOKU_SIZE][SUDOKU_SIZE] = {
        {0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 3, 0, 8, 5},
        {0, 0, 1, 0, 2, 0, 0, 0, 0},
        {0, 0, 0, 5, 0, 7, 0, 0, 0},
        {0, 0, 4, 0, 0, 0, 1, 0, 0},
        {0, 9, 0, 0, 0, 0, 0, 0, 0},
        {5, 0, 0, 0, 0, 0, 0, 7, 3},
        {0, 0, 2, 0, 1, 0, 0, 0, 0},
        {0, 0, 0, 0, 4, 0, 0, 0, 9}};
    uint8_t solution[SUDOKU_SIZE][SUDOKU_SIZE];
    memcpy(solution, sudokuOrig, SUDOKU_SIZE * SUDOKU_SIZE * sizeof(uint8_t));
    
    EXPECT_TRUE(sudoku_solve(solution));
    bool result = isSudokuSolutionOk(sudokuOrig, solution, m_errorMessage);
    EXPECT_TRUE(result) << m_errorMessage;
}

TEST_F(SudokuTest, yetAnotherSudoku) {
    uint8_t sudokuOrig[SUDOKU_SIZE][SUDOKU_SIZE] = {
        {1, 0, 0, 0, 0, 0, 0, 8, 9},
        {0, 0, 0, 0, 0, 9, 0, 0, 2},
        {0, 0, 0, 0, 0, 0, 4, 5, 0},
        {0, 0, 0, 7, 6, 0, 0, 0, 0},
        {0, 3, 0, 0, 4, 0, 0, 0, 0},
        {9, 0, 0, 0, 0, 2, 0, 0, 5},
        {0, 0, 4, 0, 7, 0, 0, 0, 0},
        {5, 0, 0, 0, 0, 8, 0, 1, 0},
        {0, 6, 0, 3, 0, 0, 0, 0, 0}};
    uint8_t solution[SUDOKU_SIZE][SUDOKU_SIZE];
    memcpy(solution, sudokuOrig, SUDOKU_SIZE * SUDOKU_SIZE * sizeof(uint8_t));

    EXPECT_TRUE(sudoku_solve(solution));
    bool result = isSudokuSolutionOk(sudokuOrig, solution, m_errorMessage);
    EXPECT_TRUE(result) << m_errorMessage;
}


