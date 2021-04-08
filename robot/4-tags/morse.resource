*** Settings ***
Library           String
Library           MorseDecoderLibrary.py    ${sender_port}    ${decoder_port}

*** Variables ***
${NON_ALPHA} =        [^a-zA-Z0-9\\s]

*** Keywords ***
Morse text only
    [Arguments]        ${speed}    ${text}
 	Set Speed          ${speed}
    Send text          ${text}
    ${out} =           Convert input to output    ${text}
 	Text should be     ${out}

Morse relaxed
    [Arguments]                ${speed}    ${text}
    Morse with error margin    ${speed}    ${text}    10

Morse strict
    [Arguments]                ${speed}    ${text}
    Morse with error margin    ${speed}    ${text}    0

Morse with error margin
    [Arguments]        ${speed}    ${text}    ${error_margin}
 	Set Speed          ${speed}
    Send text          ${text}
    ${out} =           Convert input to output    ${text}
 	Text should be     ${out}
    Speed should be    ${speed}    ${error_margin}

Convert input to output
    [Arguments]        ${input}
    ${out} =           String to utf8 characters    ${input}
    #Log to console    ${out}
    ${out} =           Convert To Upper Case    ${out}
    ${out} =           Replace string using regexp    ${out}     ${NON_ALPHA}    X
    #Log to console    upper regex ${out}
    [return]           ${out}

Setup
    Automatic printing     off
    Immediate printing     off

Exit
    Automatic printing     on
    Immediate printing     on
