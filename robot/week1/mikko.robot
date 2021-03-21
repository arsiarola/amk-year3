*** Settings ***


Library    OperatingSystem
Library    Collections
Library    String
Library    Dialogs
Library    FakerLibrary    locale=fi_FI

*** Variables ***
@{list_of_names}
${path} =    contacts
${first_name}
${last_name}
${selection}
${file_content}
${street_address}
${postcode}
${city}
${postal}

*** Keywords ***

Get Random Names
    [Arguments]    ${number_of_names}

    FOR    ${i}    IN RANGE    ${number_of_names}
        ${name} =    FakerLibrary.Name
        Append To List    ${list_of_names}    ${name}
        END
    #Log To Console    ${list_of_names}\n
    FOR    ${i}    IN RANGE    ${number_of_names}
        ${name} =    Get From List    ${list_of_names}    ${i}
        #Log To Console    ${i+1}) ${name}
        END
    #Log To Console    \n
    [Return]    @{list_of_names}

User Input
    ${selection} =    Get Selection From User    Choose name    @{list_of_names}
    #Log To Console    ${selection}\n
    [Return]    ${selection}

Create Contact File
    [Arguments]    ${directory_name}    ${number_of_names}
    Create Directory    ./${directory_name}/

    @{list_of_names} =    Get Random Names  ${number_of_names}
    ${selection} =   User Input

    ${street_address} =    FakerLibrary.Street Address
    ${postcode} =    FakerLibrary.Postcode
    ${city} =    FakerLibrary.City
    ${postal} =    Catenate    ${postcode}    ${city}
    ${file_content} =    Format String    {first}\n{second}\n{third}    first=${selection}    second=${street_address}    third=${postal}

    ${file_count} =    Count Items In Directory    ./${directory_name}/
    #Log To Console    ${file_count}\n

    ${file_name} =    Format String    {index}_{name}    index=${file_count+1}    name=${directory_name}
    Create File    ./${directory_name}/${file_name}.txt    ${file_content}


Remove Existing Address File
    [Arguments]    ${file_path}
    File Should Exist    ${file_path}
    ${file} =    Get File    ${file_path}
    ${name} =    Get Line    ${file}    0
    Log    ${name}
    #Log To Console    ${name}\n
    Remove File    ${file_path}

File Line Count Validation
    [Arguments]    ${file_path}    ${lines_expected}
    ${file} =    Get File    ${file_path}
    ${line_count} =    Get Line Count    ${file}
    #Log To Console    ${line_count}\n
    Should be Equal As Integers    ${line_count}    ${lines_expected}

*** Test Cases ***

Create File first
    Create Contact File    contacts     5

Check Address File
    File Line Count Validation    ./contacts/1_contacts.txt    3

# Remove File
    # Remove Existing Address File    ./contacts/1_contacts.txt


