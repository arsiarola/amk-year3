*** Settings ***
Documentation     Example of morse transmitter test
...
...               Change this example to use data driven style
...               Test with different texts and speeds

Library           MorseDecoderLibrary.py

*** Test Cases ***
Send text
	Set Speed          50
    Send text          Hello world
	Speed should be    50
	Text should be     HELLO WORLD

