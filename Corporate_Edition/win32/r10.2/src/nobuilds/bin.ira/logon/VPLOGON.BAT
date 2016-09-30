@ECHO OFF
REM *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*
REM
REM  VPLOGIN.BAT
REM  Description: Client login script file for Windows NT Servers running 
REM               Symantec AntiVirus/Symantec Client Security Corporate Edition
REM 
REM *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=

REM *=*= OS Specific *=*=
if "%OS%" == "Windows_NT" goto WINNT_NBPSHPOP

REM ----- OS Not Supported -----
goto END

REM *=*= WinNT and beyond *=*=
:WINNT_NBPSHPOP
NBPSHPOP +T: \\%FILE_SERVER%\vplogon
T:\VP_Log32 /p=T: 
NBPSHPOP -T:
GOTO END

REM =*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=
:END


