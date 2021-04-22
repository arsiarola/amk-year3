def byte_to_description(b):
    switch = {
        b'R': "step on the X-axis",
        b'L': "step on the X-axis (opposite direction)",
        b'B': "step on the Y-axis",
        b'F': "step on the Y-axis (opposite direction)",
        b'!': "stepper pulse timing violation (too short pulse) on any axis",

        b'S': "(capital-S) – Limit 1 closed",
        b's': "(lowercase-s) – Limit 1 open",
        b'T': "(capital-T) – Limit 2 closed",
        b't': "(lowercase-t) – Limit 2 open",
        b'U': "(capital-U) – Limit 3 closed",
        b'u': "(lowercase-u) – Limit 3 open",
        b'V': "(capital-V) – Limit 4 closed",
        b'v': "(lowercase-v) – Limit 4 open",

        b'P': "Servo value indication (2 bytes)",
        b'?': "Servo pulse widths violation ",
        b'H': "Laser PWM value indication (2 bytes)",
        b'X': "X pulse length",
        b'Y': "Y pulse length"

    }
    return switch.get(b, "Invalid byte")

