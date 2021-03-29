*** Settings ***
Documentation     Example of morse transmitter test
...
...               Change this example to use data driven style
...               Test with different texts and speeds

Resource          ./morse.resource

Test template     Morse test
Suite setup       Setup
Suite teardown    exit

*** Test Cases ***    Speed    Text
Hello world slow      20      Hello
#Hello world fast      200      Hello world
#Specials              100       ®©«¶
