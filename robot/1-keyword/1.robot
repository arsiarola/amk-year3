*** Settings ***
Library    OperatingSystem
Library    Collections
Library    String
Library    Dialogs
Library    FakerLibrary    locale=fi_FI
Library    BuiltIn

*** Variables ***
${FNAME} =    test

*** Keywords ***
Remove existing address file
    [Arguments]    ${fname}
    ${exists} =    Run Keyword And Return Status    File should exist    ${fname}
    IF    ${exists} == 1
        ${file} =    Get file    ${fname}
        ${line0} =    Get line    ${file}    0
        Log    ${line0}    console=True
        Remove File    ${fname}
        Wait Until Removed    ${fname}
        File should not exist    ${fname}
    ELSE
        Log    Cannot remove file, ${fname} doesn't exist     console=True
    END

Get random names
    [Arguments]    ${count}
    Should be true    ${count} >= 1
    ${names} =    Create list
    FOR    ${i}    IN RANGE    0    ${count}
        ${name} =    FakerLibrary.Name
        Append to List    ${names}    ${name}
    END
    Return from keyword    ${names}

Get address file content
    [Arguments]    ${count}
    @{names} =    Get random names    ${count}
    ${name} =     Get selection from user    Choose a name    @{names}
    Log to console    user selected: ${name}
    ${address} =    FakerLibrary.Address
    ${address_lines} =    Split string    ${address}    \n
    ${file_content} =    Format String    {a}\n{b}\n{c}    a=${name}    b=${address_lines}[0]    c=${address_lines}[1]
    [Return]    ${file_content}


*** Test Cases ***
Remove address file
    Remove existing address file    ${FNAME}

Create address file
    ${file_content} =    Get address file content    5
    Create file    ${FNAME}    ${file_content}
    Wait until created    ${FNAME}
    File should exist    ${FNAME}

Check Address file
    File should exist    ${FNAME}
    ${file} =    Get file    ${FNAME}
    ${lc} =    Get Line Count    ${file}
    Should be true    ${lc} == 3



