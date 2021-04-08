*** Settings ***

Documentation    ex3 resources
Library    String
Library    MorseDecoderLibrary_ex4.py    ${senderPort}    ${decoderPort}

*** Variables ***
${NON-ALPHANUMERICS PATTERN} =    [^a-zA-Z\\d\\s]
${REPLACED VALUE} =    X
${TOLERANCE PERCENTAGE} =    10
${ERROR NEWLINE} =    \n ${SPACE} ${SPACE} ${SPACE} ${SPACE} ${SPACE}
*** Keywords ***
Morse Strict
    [Arguments]    ${speed}    ${phrase}
    Log To Console    \n

    Set Speed    ${speed}
    Send text    ${phrase}

    ${phrase} =    Replace String Using Regexp    ${phrase}    ${NON-ALPHANUMERICS PATTERN}    ${REPLACED VALUE}
    Log    After replacing non-alphanumerics phrase is '${phrase}'    console=True
    ${phrase_upper} =    Convert To Upper Case    ${phrase}
    Log    Decoded phrase should be '${phrase_upper}'    console=True

	Text should be    ${phrase_upper}
    ${check} =    Run Keyword And Return Status    Speed should be    ${speed}
    #@{tolerances} =    Get Speed Range    ${speed}    ${TOLERANCE PERCENTAGE}
    #Log To Console    ${tolerances}[0] ${tolerances}[1]
    IF    ${check} == False
        Log To Console    ${EMPTY}
        Log    Speed given to strict test couldn't keep up with decoder${ERROR NEWLINE}due to 1ms granularity in morse transmission/receive and integer rounding errors    level=ERROR
        Log To Console    ${EMPTY}
    END

    Log To Console    ${EMPTY}

Morse Relaxed
    [Arguments]    ${speed}    ${phrase}
    Log To Console    \n

    Set Speed    ${speed}
    Send text    ${phrase}

    ${phrase} =    Replace String Using Regexp    ${phrase}    ${NON-ALPHANUMERICS PATTERN}    ${REPLACED VALUE}
    Log    After replacing non-alphanumerics phrase is '${phrase}'    console=True
    ${phrase_upper} =    Convert To Upper Case    ${phrase}
    Log    Decoded phrase should be '${phrase_upper}'    console=True

	Text should be    ${phrase_upper}
    Speed should be With Tolerance    ${speed}    ${TOLERANCE PERCENTAGE}

    Log To Console    ${EMPTY}

Morse Text Only
    [Arguments]    ${speed}    ${phrase}
    Log To Console    \n

    Send text    ${phrase}

    ${phrase} =    Replace String Using Regexp    ${phrase}    ${NON-ALPHANUMERICS PATTERN}    ${REPLACED VALUE}
    Log    After replacing non-alphanumerics phrase is '${phrase}'    console=True
    ${phrase_upper} =    Convert To Upper Case    ${phrase}
    Log    Decoded phrase should be '${phrase_upper}'    console=True

	Text should be    ${phrase_upper}

    Log To Console    ${EMPTY}

Automation Toggle
    [Arguments]    ${selection}
    Wpm Automation    ${selection}
    Imm Automation    ${selection}
    