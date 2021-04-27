import serial
import time
from fun import *


capture = serial.Serial('/dev/ttyS3', 115200, timeout = 20)
open_limits   = "svut"
closed_limits = "SVUT"
two_bytes     = "PHXY"

def open_limits():
    capture.write(bytes('O', "utf-8"))

def close_limits():
    capture.write(bytes('o', "utf-8"))

def init_signal_capture():
    capture.reset_input_buffer()
    capture.reset_output_buffer()

    capture.write(bytes('q',    "utf-8"))
    capture.write(bytes(chr(1), "utf-8"))
    capture.write(bytes(chr(1), "utf-8"))
    capture.write(bytes(chr(1), "utf-8"))

    close_limits()


def get_1byte_resp():
    c = capture.read()
    while chr(ord(c)) in two_bytes:
        print(c, ": ", byte_to_description(c))
        c = capture.read()
        print(c, ": ", ord(c))
        c = capture.read()
    return c

def set_button_state(button, state):
    if button < 1 or 3 < button:
        print("Button number has to be between 1 and 3")
    capture.write(bytes(get_button_byte(button, state)), "utf-8")

def get_button_byte(button, state):
    if   button == 1: btn = 'j'
    elif button == 2: btn = 'k'
    elif button == 3: btn = 'l'
    return btn.upper() if state == True else btn.lower()

def open_buttons():
    for i in range(1, 4):
        set_button_state(i, False)

def close_buttons():
    for i in range(1, 4):
        set_button_state(i, True)


def test_direction(dir):
    if dir.lower()   == "right": right = True
    elif dir.lower() == "left":  right = False
    else:
        print("Invalid argument in test_direction(), give \"right\" or \"left\" string")
        return

    capture.write(bytes('l', "utf-8"))
    capture.write(bytes('j', "utf-8"))
    capture.write(bytes('L' if right else 'J' , "utf-8"))
    while True:
        c = get_1byte_resp()
        if chr(ord(c)) in closed_limits:
            break

    for i in range(5):
        c = get_1byte_resp()
        if c == b'R' if right else b'L':
            print("moved after stop!! throw error")
            break

    capture.write(bytes('l' if right else 'j', "utf-8"))

try:
    test_direction("right")


except KeyboardInterrupt:
    print("Keyboard Interrupt")

