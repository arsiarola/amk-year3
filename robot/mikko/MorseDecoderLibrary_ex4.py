import serial
from robot.api import logger

class MorseDecoderLibrary_ex4(object):
    ''' Library for interacting with morse sender and decoder
    '''

    ROBOT_LIBRARY_SCOPE = 'SUITE'
    
    def __init__(self, senderPort, decoderPort):
        self._sender = serial.Serial(senderPort, 115200, timeout = 1)
        self._decoder = serial.Serial(decoderPort, 115200, timeout = 20)


    def set_speed(self, speed):
        self._sender.write(bytes('wpm ' + speed + '\n', 'utf-8'))
        write_to_log_and_console(f"Speed set to {speed}")

    def send_text(self, text):
        self._decoder.reset_input_buffer()
        self._sender.write(bytes("text " + text + '\n', 'utf-8'))
        write_to_log_and_console(f"Phrase '{text}' was sent to decoder")

    def speed_should_be(self, expected_speed):
        self._decoder.write(bytes('WPM\n', 'utf-8'))
        text = self._decoder.readline().strip().decode('utf-8')
        speed = int(text.split()[2])
        write_to_log_and_console(f"speed should {speed}")
        if speed != int(expected_speed):
            raise AssertionError('Expected: ' + str(expected_speed) + ' got: '  + str(speed) + ' line: ' + text)

    def text_should_be(self, expected_text):
        text = self._decoder.readline().strip().decode('utf-8')
        write_to_log_and_console(f"Decoded phrase was '{text}'")
        if text != expected_text:
            raise AssertionError('Expected: ' + expected_text + ' got: ' + text)

    def wpm_automation(self, toggle):
        self._decoder.write(bytes("WPM " + toggle + '\n', 'utf-8'))
        text = self._decoder.readline()
        write_to_log_and_console(text)

    def imm_automation(self, toggle):
        self._decoder.write(bytes("IMM " + toggle + '\n', 'utf-8'))
        text = self._decoder.readline()
        write_to_log_and_console(text)

    def speed_should_be_with_tolerance(self, expected_speed, percentage):
        self._decoder.write(bytes('WPM\n', 'utf-8'))
        text = self._decoder.readline().strip().decode('utf-8')
        speed = int(text.split()[2])
        tolerance = int(expected_speed) / 100 * int(percentage)
        write_to_log_and_console(f"{percentage}% tolerance is ±{tolerance:.1f}")
        max_speed = int(expected_speed) + tolerance
        min_speed = int(expected_speed) - tolerance
        # tolerances = get_speed_range(speed, int(percentage))
        # min_speed = tolerances[0]
        # max_speed = tolerances[1]
        write_to_log_and_console(f"speed should be between {min_speed} and {max_speed},  decoder returned speed {speed}")
        if speed < min_speed or speed > max_speed:
            raise AssertionError('Expected: ' + str(expected_speed) + ' got: '  + str(speed) + ' line: ' + text)

    def get_speed_range(self, speed, percentage):
        tolerance = int(speed) / 100 * int(percentage)
        write_to_log_and_console(f"{percentage}% tolerance from {speed} is ±{tolerance:.1f}")
        arr = []
        arr.append(int(speed) - tolerance)
        arr.append(int(speed) + tolerance)
        return arr

def write_to_console(s):
        logger.console(s)
def write_to_log(s):
        logger.info(s)
def write_to_log_and_console(s):
        logger.info(s, also_console=True)



        
        