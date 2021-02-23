from RPi import GPIO
from RPLCD.gpio import CharLCD
rs = 40
en = 38

d4 = 29
d5 = 31
d6 = 33
d7 = 35

lcd = CharLCD(pin_rs=rs, pin_rw=None, pin_e=en, pins_data=[d4,d5,d6,d7],
              numbering_mode=GPIO.BOARD,
              cols=16, rows=2, dotsize=8,
              charmap='A02',
              auto_linebreaks=False)
lcd.clear()
lcd.write_string('Hello world')

