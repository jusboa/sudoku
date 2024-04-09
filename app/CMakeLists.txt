#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Ofast")
add_library(
  app
  sudoku_common.h
  sudoku_common.c
  sudoku.c
  sudoku.h)

