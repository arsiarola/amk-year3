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
    File should exist    ${fname}
    ${file} =    Get file    ${fname}
    ${line0} =    Get line    ${file}    0
    Log    ${line0}
    Remove File    ${fname}
    Wait Until Removed    ${fname}
    File should not exist    ${fname}

Get random names
    [Arguments]    ${count}
    Should be true    ${count} >= 1
    ${names} =    Create list
    FOR    ${i}    IN RANGE    0    ${count}
        ${name} =    FakerLibrary.Name
        Log to console    ${name} \n
        Append to List    ${names}    ${name}
    END
    Log to console   ${names}
    Return from keyword    ${names}

Get address file content
    @{names} =    Get random names    5
    ${name} =     Get selections from user    Choose a name    @{names}
    Log to console    user selected: ${name}
    ${name} =     Convert to string    ${name}
    # Strip away characters [']
    ${name} =    Remove string using regexp    ${name}   ['\[\]]*
    # Another more readable option to strip chars:
    # ${name} =    Remove string    ${name}   [
    # ${name} =    Remove string    ${name}   '
    # ${name} =    Remove string    ${name}   ]
    Log to console    stripped name: ${name}\n
    ${address} =    FakerLibrary.Address
    ${address_lines} =    Split string    ${address}    ${\n}
    ${file_content} =    Format String    {a}\n{b}\n{c}    a=${name}    b=${address_lines}[0]    c=${address_lines}[1]
    [Return]    ${file_content}


*** Test Cases ***
Remove address file
    Remove existing address file    ${FNAME}

Create address file
    ${file_content} =    Get address file content
    Log to console    ${file_content}
    Create file    ${FNAME}    ${file_content}
    Wait until created    ${FNAME}
    File should exist    ${FNAME}

Check Address file
    File should exist    ${FNAME}
    ${file} =    Get file    ${FNAME}
    ${lc} =    Get Line Count    ${file}
    Should be true    ${lc} == 3



