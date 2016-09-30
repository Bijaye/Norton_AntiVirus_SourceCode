@echo off
cls
isgbpart
if errorlevel 1 goto gb_detected

echo  ******************************************************************************
echo  **************** Norton AntiVirus: Emergency Virus Repair ********************
echo  ******************************************************************************
echo.                                                                             
echo        Press any key to begin automatic operation. If you prefer manual      
echo        operation, press Ctrl-C to exit and type NAVDX /? for information.     
echo.                                                                             
echo  ******************************************************************************
echo.
pause
if exist C:\VIRUSDEF\VIRSCAN1.DAT NAVDX C: /m+ /b+ /REPAIR /CFG:C:\VIRUSDEF
if not exist C:\VIRUSDEF\VIRSCAN1.DAT NAVDX C: /m+ /b+ /REPAIR
goto finish

:gb_detected
echo ******************************************************************************
echo **************** Norton AntiVirus: Emergency Virus Repair ********************
echo ******************************************************************************
echo.     
echo Norton AntiVirus has detected that GoBack is running on your computer
echo.     
echo You must disable GoBack before you can boot from this CD
echo To disable GoBack, first remove this CD and then reboot your computer
echo When GoBack prompts you, press the spacebar and follow the onscreen instructions
echo Once GoBack has been disabled, reinsert this CD and then reboot your computer                                                                        
echo.                                                                             
echo ******************************************************************************
echo.
pause



:finish
echo on
