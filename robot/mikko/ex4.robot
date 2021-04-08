*** Settings ***
Documentation     Morse transmitter with data driven style, separate resources file, suite setup, -tear down and arguments for COM ports
Suite Setup    Automation Toggle    OFF
Suite Teardown    Automation Toggle    ON

Resource    ex4.resource

*** Test Cases ***    Speed    Phrase

Strict tests     [Template]    Morse Strict
                 [Tags]        strict
                 50            hello strict world
                 #25            slower strict world
                 #125           faster strict world
                 125           special-strict!world@
                 80            strict-failing strict world

Relaxed tests    [Template]    Morse Relaxed
                 [Tags]        relaxed
                 50            hello relaxed world
                 #25            slower relaxed world
                 #125           faster relaxed world
                 125           special-relaxed!world@
                 80            strict-failing relaxed world

Text tests       [Template]    Morse Text Only
                 [Tags]        text only
                 50            hello text world
                 #25            slower text world
                 #125           faster text world
                 125           special-textworld@
                 80            strict-failing relaxed world
