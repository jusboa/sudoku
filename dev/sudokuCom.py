import serial
import argparse
import time

STOP_FRAME = 'stop'

def main():
    parser = argparse.ArgumentParser(description='Send sudoku puzzle to a defined serial port.')
    parser.add_argument('--puzzle-file',
                        help='A file containing unsolved sudoku.',
                        required=True)
    parser.add_argument('--port',
                        help='Serial port to send the data to.',
                        default='/dev/ttyACM0')
    parser.add_argument('--baudrate',
                        help='Serial port baud rate.',
                        default=115200)
    args = parser.parse_args()
    com = serial.Serial(port=args.port, baudrate=args.baudrate, timeout=0)

    with open(args.puzzle_file, 'r') as f:
        puzzle = f.read()
        com.write(puzzle.encode('utf-8'))
        com.write(STOP_FRAME.encode('utf-8'))

        answer = ''
        while (True):
            data = com.read(1)
            if len(data) > 0:
                answer += data.decode()
                if (STOP_FRAME in answer):
                    break;
        print (f'\n{answer[:-4]}')

if __name__ == '__main__':
    main()
