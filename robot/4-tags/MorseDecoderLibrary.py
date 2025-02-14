import serial
from robot.api import logger
import time

class MorseDecoderLibrary(object):
    ''' Library for interacting with morse sender and decoder
    '''
    ROBOT_LIBRARY_SCOPE = 'SUITE'

    def __init__(self,sender_port,decoder_port):
        self._sender = serial.Serial(sender_port, 115200, timeout = 1)
        self._decoder = serial.Serial(decoder_port, 115200, timeout = 20)


    def set_speed(self, speed):
        self._sender.write(bytes('wpm ' + speed + '\n', 'utf-8'))

    def send_text(self, text):
        self._decoder.reset_input_buffer()
        self._sender.write(bytes("text " + text + '\n', 'utf-8'))

    def speed_should_be(self, expected_speed, err_margin):
        self._decoder.write(bytes('WPM\n', 'utf-8'))
        text = self._decoder.readline().strip().decode('utf-8')
        write_to_console('text is: ' + text)
        speed = int(text.split()[2])
        expected_speed = int(expected_speed)
        err_margin = int(err_margin)
        offset = expected_speed * err_margin / 100
        max_speed = expected_speed + offset
        min_speed = expected_speed - offset
        if speed < min_speed or max_speed < speed:
            raise AssertionError('Expected: ' + str(expected_speed) + ' got: '  + str(speed) + ' line: ' + text)


    def text_should_be(self, expected_text):
        text = self._decoder.readline().strip().decode('utf-8')
        if text != expected_text:
            raise AssertionError('Expected: ' + expected_text + ' got: ' + text)

    def string_to_utf8_characters(self, input):
        out = ""
        for c in list(input):
            for bytes in list(c.encode('utf-8')):
                print(chr(bytes))
                out += chr(bytes)
        return out

    def automatic_printing(self,mode):
        self._decoder.write(bytes('WPM ' + mode + '\n', 'utf-8'))
        self._decoder.readline()


    def immediate_printing(self,mode):
        self._decoder.write(bytes('IMM ' + mode + '\n', 'utf-8'))
        self._decoder.readline()

def write_to_console(s):
        logger.console(s)
