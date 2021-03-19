*** Settings ***
Library    OperatingSystem
Library    Collections
Library    String
Library    Dialogs
Library    FakerLibrary

*** Keywords ***
Remove existing address file
    [Arguments]    ${name}
    File should exist    ${name}
    ${file} =    Get file    ${name}
    ${line0} =    Get line    ${file}    0
    Log    ${line0}
    # Remove File    ${file_name}

Get random names
    [Arguments]    ${count}
    Should be true    ${count} >= 1
    ${names} =    FakerLibrary.Name    nb=${count}
    Log    ${names}

*** Test Cases ***
Test
    Remove existing address file    ./test
    Get random names    5
