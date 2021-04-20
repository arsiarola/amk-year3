import serial
import time


capture = serial.Serial('/dev/ttyS3', 115200, timeout = 1)
#init
# while True:
#     c = capture.read(1).decode('utf-8')
#     print("c = ", c)
#     if c == 'Q':
#         break
limits = "svut"

capture.write(bytes('q', "utf-8"))
capture.write(bytes(chr(100), "utf-8"))
capture.write(bytes(chr(150), "utf-8"))
capture.write(bytes(chr(1), "utf-8"))
capture.write(bytes('O', "utf-8"))
ok = capture.read(4)
capture.write(bytes('J', "utf-8"))

# print(ok)
# print(all(c in str(ok) for c in limits))

try:
    while True:
        print(capture.read())
except KeyboardInterrupt:
    print("Keyboard Interrupt")

# class MorseDecoderLibrary(object):
#     def __init__(self, port):
#         self.port = serial.Serial(port, 115200, timeout = 1)
