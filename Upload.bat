@echo off
REM Auto Compile and Upload an Aurdino Script
REM Cal.W 2020

Title Arduino File Uploader v1.0

SET INO_FILE=./main.ino
SET PRE_BUILD_COMMAND=python ./genHeaders.py
SET INCULDE_LIB=%INCULDE_LIB%;Adafruit BMP280 Library
SET INCULDE_LIB=%INCULDE_LIB%;SdFat

SET DISABLE_MACRO_EXSPANSION=--pref compiler.cpp.extra_flags="-ftrack-macro-expansion=0 -fno-diagnostics-show-caret"
SET USE_FULL_PRINTF=--pref compiler.c.elf.extra_flags="-Wl,-u,vfprintf -lprintf_flt"
SET EXTRA_ARGS=%DISABLE_MACRO_EXSPANSION% %USE_FULL_PRINTF%

REM Set this to 1 to enable auto pulling if git it installed
SET DO_GIT_PULL=0

REM Set this to 1 to run the prebuild script
SET DO_PRE_BUILD=1

REM These should not have to be changed
SET IDE_PATH="C:\Program Files (x86)\Arduino\"
SET LIB_PATH=%userprofile%\Documents\Arduino\libraries

REM These should not be updated by the user
SET CLI_PATH=%~dp0arduino-cli
SET CLI_URL=https://downloads.arduino.cc/arduino-cli/arduino-cli_latest_Windows_64bit.zip
SET CLI_ZIP=arduino-cli.zip

SET COM_PORT=0 
SET BOARD=0
::COM4
::arduino:avr:mega

REM Update the local %PATH%
SET PATH=%PATH%;%IDE_PATH%;%CLI_PATH%

REM If enabled, attempt to pull the latest version of the repo
IF %DO_GIT_PULL% EQU 1 CALL :gitPull

echo Prepairing to upload %INO_FILE%

REM Download the CLI if its not present
IF NOT EXIST %CLI_PATH% call :getCLI

REM Install libarys if they are needed
setlocal EnableDelayedExpansion
for %%g in (%INCULDE_LIB: =_%) do (
    set "lib=%%g"
    IF NOT EXIST "%LIB_PATH%\!lib!" (
        echo Installing !lib:_= !
        arduino-cli lib install "!lib:_= !"    
    ) else (
        echo !lib:_= ! is present.
    )
)
endlocal

REM Get board info for the uploader
REM [TODO] Make this support mutiple boards - currently it only uses the last one in the list
echo Getting Board Info
SET BOARD_COMMAND=powershell -Command "$boards = %CLI_PATH%\arduino-cli.exe board list --format json | ConvertFrom-Json ; Write-Host $boards[0].address $boards[0].boards.FQBN;"
FOR /F "tokens=1,2" %%g IN ('%BOARD_COMMAND%') do (
    SET COM_PORT=%%g
    SET BOARD=%%h
)

IF NOT %COM_PORT% EQU 0 (
    echo Serial Port: %COM_PORT%
    echo Board Type: %BOARD%
    REM Fix the port/board type for the uploader
    SET COM_PORT=--port %COM_PORT%
    SET BOARD=--board %BOARD%
    
    REM We only want to upload if we have a board and a 
    SET UPLOAD_OR_VERIFY=--upload
) ELSE (
    echo Aurdino not detected, only going to verify the file is compilable.
    SET COM_PORT=
    SET BOARD=
    SET UPLOAD_OR_VERIFY=--verify
)

REM Attempt to run the prebuild command
IF %DO_PRE_BUILD% EQU 1 call :preBuildCommand

IF NOT %COM_PORT% EQU 0 (
    echo Starting compile and upload.
) ELSE (
    echo Starting verification.
)

arduino_debug.exe %EXTRA_ARGS% %COM_PORT% %BOARD% %UPLOAD_OR_VERIFY% %INO_FILE%

echo Done!
pause
GOTO :eof

REM Download and Extract the Arduino-CLI
:getCLI
    echo  Arduino CLI not present. Downloading....
    mkdir "%CLI_PATH%"
    call  :downloadFile '%CLI_URL%' '%CLI_PATH%\%CLI_ZIP%'
    echo  Extracting...
    call  :extractZip '%CLI_PATH%\%CLI_ZIP%' '%CLI_PATH%\'
    echo  Cleaning up files...
    del  /Q "%CLI_PATH%/%CLI_ZIP%"
    echo  Misc Files Removed.
    echo  Arduino-CLI Downloaded and Setup!
    exit /B

REM Attempt to pull the latest repo version
:gitPull
    setlocal EnableDelayedExpansion
    where git 1>NUL 2>NUL
    IF %ERRORLEVEL% EQU 0 (
        echo Git is installed...

        echo Checking if out of date...
        echo Updating remote....
        git remote update >NUL
        echo Compairing Local Repo to Remote...
        git diff origin/master --quiet
        IF !ERRORLEVEL! EQU 1 (
            echo Diffrence in remote and local branch detected!
            echo Here is what differs:
            git --no-pager diff origin/master --color-words
            echo.
            echo You have the option to burn any local changes or ignore any changes made on the remote side.
            echo If you select [C]ancel then %INO_FILE% will not be uploaded, allowing for a manual backup.
            echo WARNING: Selecting [Y]es WILL deleted anything you have done. As git tells you, you should stash any changes you make
            CHOICE /C YNC /M "Would you like to pull the latest version removing any LOCAL changes?"
            IF !ERRORLEVEL! EQU 1 (
                echo Pulling repo.
                git reset --hard
                git pull
                exit /B
            )
            IF !ERRORLEVEL! EQU 2 (
                echo Ignoring remote changes...
                exit /B
            )
            IF !ERRORLEVEL! EQU 3 (
                echo Canceling upload!
                echo You need to upload the code to the Arduino before any changes you made will be present!
                pause
                goto :EOF
            )
            IF !ERRORLEVEL! EQU 0 (
                echo Something went very wrong. :/
                echo Canceling upload!
                echo You need to upload the code to the Arduino before any changes you made will be present!
                pause
                goto :EOF
            )
        )
        echo Runing on the latest verison - I hope
    ) else (
        echo Git is not installed. You have to manually update this folder...
    )
    endlocal
    exit /B

REM Attempt to run the prebuild command
:preBuildCommand
    FOR /F "tokens=1" %%g IN ("%PRE_BUILD_COMMAND%") do (SET FIRST_COMMAND=%%g)
    where %FIRST_COMMAND% 1>NUL 2>NUL
    IF %ERRORLEVEL% EQU 0 (
        echo Prebuild command is present. Attempting to start it...
        %PRE_BUILD_COMMAND%
    ) else (
        echo Could not find prebuild command: "%PRE_BUILD_COMMAND%"
    )
    EXIT /B

REM Download a url to a file
:downloadFile
    powershell -Command "Invoke-WebRequest %1 -OutFile %2 ; echo 'Download Complete.' ; exit"
    exit /B

REM Extract a zip to a folder
:extractZip
    powershell -Command "Expand-Archive -Force %1 %2 ; echo 'Extraction Complete' ; exit"
    exit /B