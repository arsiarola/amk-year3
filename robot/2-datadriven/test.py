def encoder_to_output(input):
    out = ""
    for c in list(input):
        for Bytes in list(c.encode('utf8')):
            # print(chr(Bytes))
            out += chr(Bytes)

    return out
s = "®testä"
print(s)
print(encoder_to_output(s))
