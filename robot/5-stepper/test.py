import serial
import time
from fun import *


capture = serial.Serial('/dev/ttyS3', 115200, timeout = 20)
open_limits =   "svut"
closed_limits = "SVUT"
two_bytes = "PHXY"

capture.reset_input_buffer()
capture.reset_output_buffer()

capture.write(bytes('q',    "utf-8"))
capture.write(bytes(chr(1), "utf-8"))
capture.write(bytes(chr(1), "utf-8"))
capture.write(bytes(chr(1), "utf-8"))

capture.write(bytes('O', "utf-8"))

def get_1byte_resp():
    c = capture.read()
    while chr(ord(c)) in two_bytes:
        print(c, ": ", byte_to_description(c))
        c = capture.read()
        print(c, ": ", ord(c))
        c = capture.read()
    return c

def test_direction(dir):
    if dir == "right":  dir = False
    elif dir == "left": dir = True
    else:
        print("Invalid argument in test_direction(), give \"right\" or \"left\" string")
        return
    capture.write(bytes('l', "utf-8"))
    capture.write(bytes('j', "utf-8"))
    capture.write(bytes('J' if dir else 'L', "utf-8"))
    while True:

        elif chr(ord(c)) in closed_limits:
            break

    for i in range(5):
        c = capture.read()
        if c == b'L' or c == b'R':
            print("moved after stop!! throw error")
            break

    capture.write(bytes('j' if dir else 'l', "utf-8"))

try:
    test_direction("right")


except KeyboardInterrupt:
    print("Keyboard Interrupt")

