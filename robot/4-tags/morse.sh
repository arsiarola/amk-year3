#!/bin/sh
cmd="robot -v sender_port:/dev/ttyS4 -v decoder_port:/dev/ttyS3 --outputdir out "
fname="morse.robot"
no_strict="-e strict"
text_only="-i text_only"

case "$1" in
    all)
        $cmd $fname
        ;;
    no-strict)
        $cmd $no_strict $fname
        ;;
    text-only)
        $cmd $text_only $fname
        ;;
esac
