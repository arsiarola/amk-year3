*** Settings ***
Documentation     Example of morse transmitter test
...
...               Change this example to use data driven style
...               Test with different texts and speeds

Library           MorseDecoderLibrary.py
Library           String

Test template     Morse test

*** Variables ***
${NON_ALPHA} =        [^a-zA-Z0-9\\s]


*** Test Cases ***    Speed    Text
Hello world slow      20       Hello world
Hello world fast      200      Hello world
Specials              50       ®©«¶

*** Keywords ***
Morse test
    [Arguments]        ${speed}    ${text}
 	Set Speed          ${speed}
    Send text          ${text}
    ${out} =           Convert input to output    ${text}
 	Text should be     ${out}

Convert input to output
    [Arguments]       ${input}
    ${out} =          String to utf8 characters    ${input}
    Log to console    ${out}
    ${out} =          Convert To Upper Case    ${out}
    ${out} =          Replace string using regexp    ${out}     ${NON_ALPHA}    X
    Log to console    upper regex ${out}
    [return]          ${out}
