# sudoku
Sudoku solver implemented by means of brute force backtracking. The backtracking algorithm is implemented non-recursively. See _app/sudoku.[ch]_ for details.

## Build
```
cmake -S <sudoku_folder> -B <build_folder>
cd <build_folder>
cmake --build .
```
## Test
```
cd <build_folder>
ctest --extra-verbose --test-dir tests
```
## Benchmark on target
The solver was tested on Texas EK-TM4C123GXL devkit utilizing TM4C123G Arm Cortex-M4F processor.
The _target_ folder holds a Code Composer Studio project (tested with CCS 12.6).
Once running the sudoku could be send to the target via UART using _dev/sudokuCom.py_
```
python3 sudokuCom.py --puzzle-file sudoku_1.txt --port <serial_port_number>
```
There are two sample puzzles - _sudoku_1.txt_ and _sudoku_2.txt_ in _dev_ folder.
