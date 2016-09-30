@echo off
if "%1" == "" goto Usage

if "%1" != "ENGLISH" if "%1" != "FRENCH" if "%1" != "SPANISH" if "%1" != "GERMAN" if "%1" != "ITALIAN" if "%1" != "PORTUGUESE" goto Usage

for %name in ( ADMTOOLS\TOOLS ADMTOOLS\NSC NAV9X NAVFW NAVIEG NAVNLM NAVNOTES NAVNTSV.351\NEC NAVNTSV.351\INTEL NAVNTSVI NAVNTWS NAVNTWS.351\INTEL NAVNTWS.351\NEC NAVXCHNG NAVDOSWN NAVNETW PCA ) do gosub DoStart_Intel

for %name in ( NAVNTSV.351\ALPHA NAVNTSVA ) do gosub DoStart_Alpha
goto DoneRenameAndCopy

:DoStart_Intel
if exist %name\setup.exe if NOT exist %name\ibmavlic.txt goto DoRenameAndCopy_Intel
return

:DoStart_Alpha
if exist %name\setup.exe if NOT exist %name\ibmavlic.txt goto DoRenameAndCopy_Alpha
return

:DoRenameAndCopy_Intel
attrib -r %name\setup.exe
ren %name\setup.exe %name\setupnav.exe
copy setup_i.exe %name\setup.exe
attrib +r %name\setup.exe
attrib +r %name\setupnav.exe
if "%1" == "ENGLISH" copy iplaea.txt %name\ibmavlic.txt
if "%1" == "FRENCH" copy iplafr.txt %name\ibmavlic.txt
if "%1" == "SPANISH" copy iplasp.txt %name\ibmavlic.txt
if "%1" == "GERMAN" copy iplade.txt %name\ibmavlic.txt
if "%1" == "ITALIAN" copy iplait.txt %name\ibmavlic.txt
if "%1" == "PORTUGUESE" copy iplapb.txt %name\ibmavlic.txt

rem copy to DISK1 folder if one exist
if ISDIR %name\DISK1 copy %name\ibmavlic.txt %name\DISK1

return

:DoRenameAndCopy_Alpha
attrib -r %name\setup.exe
ren %name\setup.exe %name\setupnav.exe
copy setup_a.exe %name\setup.exe
attrib +r %name\setup.exe
attrib +r %name\setupnav.exe
if "%1" == "ENGLISH" copy iplaea.txt %name\ibmavlic.txt
if "%1" == "FRENCH" copy iplafr.txt %name\ibmavlic.txt
if "%1" == "SPANISH" copy iplasp.txt %name\ibmavlic.txt
if "%1" == "GERMAN" copy iplade.txt %name\ibmavlic.txt
if "%1" == "ITALIAN" copy iplait.txt %name\ibmavlic.txt
if "%1" == "PORTUGUESE" copy iplapb.txt %name\ibmavlic.txt

rem copy to DISK1 folder if one exist
if ISDIR %name\DISK1 copy %name\ibmavlic.txt %name\DISK1

return

:Usage
@echo Usage.....
echo setupibm language, where language is one of the following:
echo ENGLISH FRENCH SPANISH GERMAN ITALIAN PORTUGUESE
echo eg., setupibm FRENCH
goto DoneRenameAndCopy


:DoneRenameAndCopy



