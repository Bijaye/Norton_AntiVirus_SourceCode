@echo off
rem This batch file re-compiles the ACTA string definition files for
rem use by other projects in the NavCorp Core_Technology build
rem By David Luxford, 2002 May 6
rem
rem Arguments:
rem    %1 - full pathname to the NavCorp perforce client root (on the current drive)

if "%1" == "" goto ShowCommandHelp

if exist "%1\Norton_AntiVirus\Corporate_Edition\Common\src\PScan\acta.h" del /f "%1\Norton_AntiVirus\Corporate_Edition\Common\src\PScan\acta.h"
if exist "%1\Norton_AntiVirus\Corporate_Edition\Common\src\PScan\acta.c" del /f "%1\Norton_AntiVirus\Corporate_Edition\Common\src\PScan\acta.c"
cd "%1"
cd Norton_AntiVirus\Corporate_Edition\Common\src\MakeMsgs
Makemsgs h
cd ..\Pscan
rem Just in case output files have been accidentally marked readonly (yes, this happened...):
attrib -r msgs.h
attrib -r msgs.rc
attrib -r msg*.bin
mc msgs.mc
goto End

:ShowCommandHelp
echo * SavCorp perforce client root not specified - please specify.

:End
