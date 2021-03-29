touch /tmp/sender
socat -lf /dev/stdout -v PTY,link=/tmp/sender /dev/ttyS4
