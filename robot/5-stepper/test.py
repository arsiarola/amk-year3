import serial
import time


port = serial.Serial('/dev/ttyS3', 115200, timeout = 1)
try:
    while True:
        print(port.read())
except KeyboardInterrupt:
    print("Keyboard Interrupt")

# class MorseDecoderLibrary(object):
#     def __init__(self, port):
#         self.port = serial.Serial(port, 115200, timeout = 1)

