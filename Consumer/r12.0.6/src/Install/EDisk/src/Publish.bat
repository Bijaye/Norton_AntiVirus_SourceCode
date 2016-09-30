@echo off

rem ************************************************************************************
rem save the command prompt
set tprompt=prompt

rem make sure all files exist

set BinFilesToPublish=Emergncy.cmd startup.exe

for %%f in (%BinFilesToPublish%) do if not exist Release\%%f goto FilesMissing

rem ************************************************************************************

if "%BinFilesToPublish%"=="" goto NoBin
cd Release

set FilesToPublish=%BinFilesToPublish%
set PublishProject=$/Publish/EDisk StartUp/Bin
set prompt=:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::$_: Publishing...$_:   %FilesToPublish%$_: from$_:    $p$_: to:$_:    $_%PublishProject%
@echo on

@echo off

ss CP  -I- -O- "%PublishProject%"
ss Add -I- -O- -C- -W %FilesToPublish%

echo : CheckOut without getting file...
ss CheckOut %FilesToPublish% -I- -G-
echo : CheckIn...
ss CheckIn  %FilesToPublish% -I- -G- -C- -W

cd ..
:NoBin

rem ************************************************************************************
goto Done

:FilesMissing

set prompt=:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::$_: Error: Publishing from:$_:     $p$_:$_: The following files are missing:
@echo on

@echo off
echo :
for %%f in (%BinFilesToPublish%) do if not exist Release\%%f echo :     Release\%%f
echo :
echo :::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

:Done

rem restore the command prompt
set prompt=%tprompt%
