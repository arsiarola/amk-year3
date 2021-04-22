import serial
import time
from fun import *


capture = serial.Serial('/dev/ttyS3', 115200, timeout = 20)
open_limits =   "svut"
closed_limits = "SVUT"

capture.reset_input_buffer()
capture.reset_output_buffer()

capture.write(bytes('q',    "utf-8"))
capture.write(bytes(chr(1), "utf-8"))
capture.write(bytes(chr(1), "utf-8"))
capture.write(bytes(chr(1), "utf-8"))

capture.write(bytes('O', "utf-8"))

# ok = capture.read(4)
# print("ok = ", ok)
# print(all(c in str(ok) for c in limits))

try:
    capture.write(bytes('J', "utf-8"))
    while True:
        c = capture.read()
        print(c, ": ", byte_to_description(c))
        if str(c) in closed_limits:
            print("Limit hit", c)
            break

    # for i in range(5):
    #     c = capture.read()
    #     print("c = ", c)
    #     if c == b'L' or c == b'R':
    #         print("moved after stop!! throw error")


except KeyboardInterrupt:
    print("Keyboard Interrupt")

