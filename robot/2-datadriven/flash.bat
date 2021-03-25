start /wait /b cmd /c %IDE_PATH%\ide\binaries\boot_link2.cmd

set MCUX_WORKSPACE_LOC=S:/mcu
set MCUX_FLASH_DIR0=S:/Programs/MCUXpressoIDE_11.3.0_5222/ide/plugins/com.nxp.mcuxpresso.tools.bin.win32_11.3.0.202101111546/binaries/Flash
set MCUX_IDE_BIN=S:/Programs/MCUXpressoIDE_11.3.0_5222/ide/plugins/com.nxp.mcuxpresso.tools.bin.win32_11.3.0.202101111546/binaries/

%MCUX_IDE_BIN%/crt_emu_cm_redlink --flash-load-exec "%MCUX_WORKSPACE_LOC%/MorseSender/Debug/MorseSender.axf" -p LPC1549 --ConnectScript LPC15RunBootRomConnect.scp --vc --flash-driver LPC15xx_256K.cfx -x %MCUX_WORKSPACE_LOC%/MorseSender/Debug --flash-hashing
