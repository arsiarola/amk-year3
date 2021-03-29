touch /tmp/decoder
socat -lf /dev/stdout -v PTY,link=/tmp/decoder /dev/ttyS3
