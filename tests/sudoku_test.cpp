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
        memcpy(m_testSudoku, SOLUTION, SUDOKU_SIZE_B);
    }

protected:
    static constexpr int SUDOKU_SIZE_B = SUDOKU_SIZE * SUDOKU_SIZE * sizeof(uint8_t);
    static constexpr uint8_t SUDOKU_1[SUDOKU_SIZE][SUDOKU_SIZE] = {
        {5, 3, 0, 0, 7, 0, 0, 0, 0},
        {6, 0, 0, 1, 9, 5, 0, 0, 0},
        {0, 9, 8, 0, 0, 0, 0, 6, 0},
        {8, 0, 0, 0, 6, 0, 0, 0, 3},
        {4, 0, 0, 8, 0, 3, 0, 0, 1},
        {7, 0, 0, 0, 2, 0, 0, 0, 6},
        {0, 6, 0, 0, 0, 0, 2, 8, 0},
        {0, 0, 0, 4, 1, 9, 0, 0, 5},
        {0, 0, 0, 0, 8, 0, 0, 7, 9}};
    static constexpr uint8_t SUDOKU_2[SUDOKU_SIZE][SUDOKU_SIZE] = {
        {0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 3, 0, 8, 5},
        {0, 0, 1, 0, 2, 0, 0, 0, 0},
        {0, 0, 0, 5, 0, 7, 0, 0, 0},
        {0, 0, 4, 0, 0, 0, 1, 0, 0},
        {0, 9, 0, 0, 0, 0, 0, 0, 0},
        {5, 0, 0, 0, 0, 0, 0, 7, 3},
        {0, 0, 2, 0, 1, 0, 0, 0, 0},
        {0, 0, 0, 0, 4, 0, 0, 0, 9}};
    static constexpr uint8_t SOLUTION[SUDOKU_SIZE][SUDOKU_SIZE] = {
        {5, 3, 4, 6, 7, 8, 9, 1, 2},
        {6, 7, 2, 1, 9, 5, 3, 4, 8},
        {1, 9, 8, 3, 4, 2, 5, 6, 7},
        {8, 5, 9, 7, 6, 1, 4, 2, 3},
        {4, 2, 6, 8, 5, 3, 7, 9, 1},
        {7, 1, 3, 9, 2, 4, 8, 5, 6},
        {9, 6, 1, 5, 3, 7, 2, 8, 4},
        {2, 8, 7, 4, 1, 9, 6, 3, 5},
        {3, 4, 5, 2, 8, 6, 1, 7, 9}};
    uint8_t m_testSudoku[SUDOKU_SIZE][SUDOKU_SIZE];
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
    EXPECT_TRUE(isSudokuSolutionOk(SUDOKU_1, m_testSudoku));
}

TEST_F(SelfTest, validSolutionMismatchWithOriginal) {
    m_testSudoku[0][0] = 6;       // original = 5
    EXPECT_FALSE(isSudokuSolutionOk(SUDOKU_1, m_testSudoku, m_errorMessage));
    EXPECT_TRUE(regex_search(m_errorMessage, regex("Mismatch of solution with original value")));
}

TEST_F(SelfTest, invalidSolution) {
    m_testSudoku[0][8] = 9;       // correct value = 2
    EXPECT_FALSE(isSudokuSolutionOk(SUDOKU_1, m_testSudoku));
}

TEST_F(SelfTest, outofBoundValues) {
    m_testSudoku[4][4] = 0;       // correct value = 5
    EXPECT_FALSE(isSudokuSolutionOk(SUDOKU_1, m_testSudoku, m_errorMessage));
    EXPECT_TRUE(regex_search(m_errorMessage, regex("out of bounds")));
}

TEST_F(SudokuTest, invalidSudoku_repeatedRowElement) {
    memcpy(m_testSudoku, SUDOKU_2, SUDOKU_SIZE_B);
    m_testSudoku[2][3] = 1;       // original = 0
    EXPECT_FALSE(sudoku_solve(m_testSudoku));
}

TEST_F(SudokuTest, invalidSudoku_repeatedColumnElement) {
    memcpy(m_testSudoku, SUDOKU_2, SUDOKU_SIZE_B);
    m_testSudoku[2][3] = 5;       // original = 0
    EXPECT_FALSE(sudoku_solve(m_testSudoku));
}

TEST_F(SudokuTest, invalidSudoku_repeatedSubmatrixElement) {
    memcpy(m_testSudoku, SUDOKU_2, SUDOKU_SIZE_B);
    m_testSudoku[2][6] = 8;       // original = 0
    EXPECT_FALSE(sudoku_solve(m_testSudoku));
}

TEST_F(SudokuTest, solveSudoku) {
    EXPECT_TRUE(sudoku_solve(m_testSudoku));
    bool result = isSudokuSolutionOk(SUDOKU_1, m_testSudoku, m_errorMessage);
    EXPECT_TRUE(result) << m_errorMessage;
}

TEST_F(SudokuTest, backTrackingImmune) {
    memcpy(m_testSudoku, SUDOKU_2, SUDOKU_SIZE_B);
    EXPECT_TRUE(sudoku_solve(m_testSudoku));
    bool result = isSudokuSolutionOk(SUDOKU_2, m_testSudoku, m_errorMessage);
    EXPECT_TRUE(result) << m_errorMessage;
}

TEST_F(SudokuTest, yetAnotherSudoku) {
    uint8_t sudoku[SUDOKU_SIZE][SUDOKU_SIZE] = {
        {1, 0, 0, 0, 0, 0, 0, 8, 9},
        {0, 0, 0, 0, 0, 9, 0, 0, 2},
        {0, 0, 0, 0, 0, 0, 4, 5, 0},
        {0, 0, 0, 7, 6, 0, 0, 0, 0},
        {0, 3, 0, 0, 4, 0, 0, 0, 0},
        {9, 0, 0, 0, 0, 2, 0, 0, 5},
        {0, 0, 4, 0, 7, 0, 0, 0, 0},
        {5, 0, 0, 0, 0, 8, 0, 1, 0},
        {0, 6, 0, 3, 0, 0, 0, 0, 0}};
    memcpy(m_testSudoku, sudoku, SUDOKU_SIZE_B);
    EXPECT_TRUE(sudoku_solve(m_testSudoku));
    bool result = isSudokuSolutionOk(sudoku, m_testSudoku, m_errorMessage);
    EXPECT_TRUE(result) << m_errorMessage;
}


