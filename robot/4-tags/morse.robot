*** Settings ***
Documentation     usage of tags

Resource          ./morse.resource

Suite setup       Setup
Suite teardown    Exit

*** Test Cases ***
Text only case
    [Tags]        Text only
    [Template]    Morse text only
#   Speed         Text
    100           Hello world
    80            ®©«¶

Relaxed case
    [Tags]        Relaxed
    [Template]    Morse relaxed
#   Speed         Text
    100           Hello world
    80            ®©«¶

Strict case
    [Tags]        Strict
    [Template]    Morse strict
#   Speed         Text
    100           Hello world
    80            ®©«¶
