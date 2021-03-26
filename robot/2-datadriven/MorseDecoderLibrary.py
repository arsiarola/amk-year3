import serial

class MorseDecoderLibrary(object):
    ''' Library for interacting with morse sender and decoder
    '''

    def __init__(self):
        self._sender = serial.Serial('/dev/ttyS4', 115200, timeout = 1)
        self._decoder = serial.Serial('/dev/ttyS3', 115200, timeout = 20)


    def set_speed(self, speed):
        self._sender.write(bytes('wpm ' + speed + '\n', 'utf-8'))


    def send_text(self, text):
        self._decoder.reset_input_buffer()
        self._sender.write(bytes("text " + text + '\n', 'utf-8'))


    def speed_should_be(self, expected_speed):
        text = self._decoder.readline().strip().decode('utf-8')
        speed = int(text.split()[2])
        if speed != int(expected_speed):
            raise AssertionError('Expected: ' + str(expected_speed) + ' got: '  + str(speed) + ' line: ' + text)


    def text_should_be(self, expected_text):
        text = self._decoder.readline().strip().decode('utf-8')
        if text != expected_text:
            raise AssertionError('Expected: ' + expected_text + ' got: ' + text)

    def string_to_utf8_characters(self, input):
        out = ""
        for c in list(input):
            for Bytes in list(c.encode('utf8')):
                print(chr(Bytes))
                out += chr(Bytes)

        return out
