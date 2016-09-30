echo off
echo Please insert the floppy disk labelled "Rescue Disk"
pause
echo Creating a temporary directory...
set temp=c:\temp > x
md c:\temp > x
echo Running the Symantec AntiVirus DOS client
echo to restore the critical disk data
vscand /rp
erase x
echo Please reboot your machine now.
pause
echo on
