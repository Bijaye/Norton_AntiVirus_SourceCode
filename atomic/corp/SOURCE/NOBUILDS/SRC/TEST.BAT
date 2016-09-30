@echo off
cls
echo.
echo.
echo.
IF EXIST c:\nul GOTO SUCCESS
echo  ******************************************************************************
echo  *************************** Norton AntiVirus Rescue **************************
echo  ******************************************************************************
echo.
echo                           Rescue Disk Test Results
echo.
echo.
echo      The C: drive could not be accessed. 
echo.
echo      You may have a multi-boot system or an embedded driver. In this case, 
echo      a change to your Norton Rescue Boot Disk is required. See the README.TXT
echo      file in your Norton AntiVirus folder for instructions on how to modify
echo      your Norton Rescue Boot Disk for proper operation."
echo.
echo.
echo      NOTE: In rare circumstances, the C: drive may not be accessible due to 
echo      a virus infection. To make sure that a virus is not the cause, insert
echo      your Rescue Disk" labeled "Norton AntiVirus Program Disk," type GO and
echo      press Enter.
echo.         
echo.
echo  ******************************************************************************
goto END
:SUCCESS
echo  ******************************************************************************
echo  *************************** Norton AntiVirus Rescue **************************
echo  ******************************************************************************
echo.
echo                           Rescue Disk Test Results
echo.
echo.
echo         SUCCESS! Your Norton AntiVirus Boot Disk passed the drive access
echo         test and will start your computer in an emergency situation.
echo.
echo         Write-protect your Rescue Disks and store them in a safe place.
echo.
echo  ******************************************************************************
:END
echo.
echo.
echo.

