@echo off
if "%1"=="" goto USAGE

set p4port=perforce:1666
set _GENERIC=generic_cm_client
set _PRODNAME=Navapi41 Windows
set _INIT_DIR=%_cwd
set _PRODCLIENT=navapi_r41_p4bldit_windows_cm_client
set _BASEDIR=\bld_area\navapi41

echo.
echo.
echo.
echo **********************************************
echo Automated build script for %_PRODNAME
echo **********************************************
echo.
delay 2

echo.
echo ****************************************************
echo This is a local build.  Since it needs Windows 98,
echo we are using a local copy of envset.inf.
echo This process is automated.
echo ****************************************************
echo.
delay 4

echo.
echo **********************************************
echo Checking for cm-ftp...
echo **********************************************
echo.

if isdir "g:\Unreleased_Builds" echo cm-ftp is mapped to g:.
delay 2

iff not isdir "g:\Unreleased_Builds" then
    echo Can't find cm-ftp connection.  Attempting to map...
    echo.
    delay 2
    net use g: /d /y
    net use g: \\cm-ftp\ftproot
    if %? ne 0 echo ERROR Unable to attach to cm-ftp.
    if not isdir "g:\Unreleased_Builds" echo *** ERROR ***      Unable to attach to cm-ftp.
    if not isdir "g:\Unreleased_Builds" quit
endiff


:next_client

echo.
echo **********************************************
echo Create generic Windows client spec...
echo **********************************************
echo.
delay 2


@echo Client:  %_GENERIC% > temp.cli
@echo Description:  Generic CM Spec for Windows >> temp.cli
@echo Root:  c:\depot >> temp.cli
@echo Options:  allwrite clobber nocompress crlf unlocked modtime >> temp.cli
@echo View: >> temp.cli
@echo        //depot/... //%_GENERIC%/... >> temp.cli
@echo.


@call p4 client -i < temp.cli
iff "%_?" ne "0" then 
  set ERRORMSG=Failed to create generic client.
  echo Setting ERRORMSG...
  echo ERRORMSG has been set to:  %ERRORMSG
  delay 2
  goto /i errmsg
endiff  
del temp.cli
delay 2

echo.
echo ************************************************************
echo Syncing and modifying the inf file for this build ##...
echo ************************************************************
echo.
delay 2

if not isdir \depot mkdir \depot
if not isdir \bld_area mkdir \bld_area
echo p4 sync -f //depot/norton_antivirus/avcore/trunk/source/navapi/cm/navapi.inf
delay 2
p4 sync -f //depot/norton_antivirus/avcore/trunk/source/navapi/cm/navapi.inf
delay 2
echo.
echo attrib -r navapi.inf
delay 2
attrib -r navapi.inf
delay 2

:pyth
echo.
echo **********************************************
echo Using Python to edit build number...
echo **********************************************
echo.
delay 2

python r:\cm\sr.py \depot\norton_antivirus\avcore\trunk\source\navapi\cm\navapi.inf "stamp=navapi41_win32_cm.xx" "stamp=navapi41_win32_cm.%1"

rem python r:\cm\sr.py \depot\norton_antivirus\avcore\trunk\source\navapi\cm\navapi.inf "build-path=c:\\bld_area\\navapi41.xx" "build-path=c:\\bld_area\\navapi41.%1"
rem python r:\cm\sr.py \depot\norton_antivirus\avcore\trunk\source\navapi\cm\navapi.inf "quake-to-use=c:\\bld_area\\navapi41.xx\\core" "quake-to-use=c:\\bld_area\\navapi41.%1\\core"
rem python r:\cm\sr.py \depot\norton_antivirus\avcore\trunk\source\navapi\cm\navapi.inf "sync-path=c:\\bld_area\\navapi41.*" "sync-path=c:\\bld_area\\navapi41.%1"

delay 2

:next_p4inf

echo.
echo **********************************************
echo Running p4bldit against the inf file...
echo **********************************************
echo.
delay 2

echo cd \build
cd \build
del . /y
del hold /y
delay 2
echo if exist \build\navapi.inf del /f \build\navapi.inf
if exist \build\navapi.inf del /f \build\navapi.inf
echo xcopy \depot\norton_antivirus\avcore\trunk\source\navapi\cm\navapi.inf .
xcopy \depot\norton_antivirus\avcore\trunk\source\navapi\cm\navapi.inf .
delay 4
echo p4bldit navapi_r41.inf /b
p4bldit navapi.inf /b
delay 2
echo.
echo move navapi.inf hold
move navapi.inf hold
echo cd hold
cd hold
delay 3


echo.
echo **********************************************
echo Syncing the envset.inf file...
echo **********************************************
echo.
delay 2

echo p4 sync -f //depot/norton_antivirus/avcore/trunk/source/navapi/cm/envset.inf
delay 2
p4 sync -f //depot/norton_antivirus/avcore/trunk/source/navapi/cm/envset.inf
delay 2
echo.
echo attrib -r \depot\norton_antivirus\avcore\trunk\source\navapi\cm\envset.inf
delay 1
attrib -r \depot\norton_antivirus\avcore\trunk\source\navapi\cm\envset.inf

delay 2

echo.
echo **********************************************
echo If previous build exists, delete it...
echo **********************************************
echo.
delay 2
if not isdir %_BASEDIR echo Creating basedir...
if not isdir %_BASEDIR mkdir /s %_BASEDIR

echo cd %_BASEDIR
cd %_BASEDIR
delay 2
echo.
echo del . /sxzy
delay 2
del . /sxzy
delay 2

echo.
echo **********************************************
echo Syncing the client...
echo **********************************************
echo.
delay 2

echo set p4client=%_PRODCLIENT
set p4client=%_PRODCLIENT
echo.
delay 2
echo p4 sync -f
delay 2
rem @call p4 sync -f

echo.
echo **********************************************
echo Preparing to run p4bldit batch files...
echo **********************************************
echo.
echo cd \build\hold
cd \build\hold
delay 2

echo.
echo **********************************************
echo Removing "env CORE" from batch files
echo Removing "env AVCORE" from batch files
echo **********************************************
echo.
delay 2

echo.
echo **********************************************
echo Removing "env" from batch files...
echo **********************************************
echo.
delay 2

echo --> NCORD.BAT
echo.
echo python r:\cm\sr.py NCORD.BAT "env CORE" "env CORE /nomap"
echo python r:\cm\sr.py NCORD.BAT "@CALL ENV CORE" "REM"
python r:\cm\sr.py NCORD.BAT "@CALL ENV CORE" "REM"
delay 1

QUIT

echo --> NAVCD.BAT
echo.
echo python r:\cm\sr.py NAVCD.BAT "@call env AVCORE" "rem call env AVCORE"
python r:\cm\sr.py NAVCD.BAT "@call env AVCORE" "rem call env AVCORE"
delay 1

echo --> NCORB.BAT
echo.
echo python r:\cm\sr.py NCORB.BAT "@call env CORE" "rem call env CORE"
python r:\cm\sr.py NCORB.BAT "@call env CORE" "rem call env CORE"
delay 1

echo --> NAVCB.BAT
echo.
echo python r:\cm\sr.py NAVCB.BAT "@call env AVCORE" "rem call env AVCORE"
python r:\cm\sr.py NAVCB.BAT "@call env AVCORE" "rem call env AVCORE"
delay 1


echo.
echo Before running the core dirs batch file, set the LANG variable
echo to ENGLISH.  If you don't, since we don't "env core" the LANG
echo variable will be null and the "ENGLISH" dirs will not be created.
delay 6

echo set LANG=ENGLISH
set LANG=ENGLISH
delay 2

echo.
echo **********************************************
echo Creating core dirs...
echo **********************************************
echo.
echo call ncorb
delay 2
call ncorb
delay 2

echo.
echo **********************************************
echo Creating avcore dirs...
echo **********************************************
echo.
echo call navcd
delay 2
call navcd
delay 2

quit

echo.
echo **********************************************
echo Adding the "-f" to CNASYNC.BAT...
echo **********************************************
echo.
delay 2
echo python r:\cm\sr.py \build\hold\cnasync.bat "p4 sync" "p4 sync -f"
delay 2
python r:\cm\sr.py \build\hold\cnasync.bat "p4 sync" "p4 sync -f"
delay 2

echo.
echo **********************************************
echo Syncing and labeling %_PRODNAME files...
echo **********************************************
echo.
delay 2
echo call cnasync.bat
call cnasync.bat
delay 2

quit

echo.
echo **********************************************
echo Copying ENVSET.INF...
echo **********************************************
echo.
delay 2
call cmacopy
delay 2

echo.
echo **********************************************
echo Creating core dirs...
echo **********************************************
echo.
cdd c:\build\hold
delay 2
call cmacoredirs
delay 2

echo.
echo **********************************************
echo Creating matrix dirs...
echo **********************************************
echo.
delay 2
call cmacmatrixdirs
delay 2

echo.
echo **********************************************
echo Building Matrix...
echo **********************************************
echo.
delay 2
call cmacmatrixbld
delay 2




:check_build

echo.
echo ****************************************************
echo Checking for errors in s:\errors\trunk\errors...
echo ****************************************************
echo.
delay 2

if %@files[%projpre%\errors\trunk\errors\*.*] eq 3 echo No errors.
if %@files[%projpre%\errors\trunk\errors\*.*] eq 3 goto next_post

echo.
echo ********************************************************
echo          THERE WERE ERRORS IN THE BUILD. 
echo.
echo Check %projpre%\errors\trunk\errors, resolve, and re-submit.
echo ********************************************************
echo.
delay 3
cd %projpre%\errors\trunk\errors
dir
echo.
quit

:next_post



echo.
echo ****************************************************
echo Posting build to cm-ftp... 
echo ****************************************************
echo.

g:
echo g:
echo.
echo Making \Installable_Builds\matrix\%1\win32\api\include
md /s \Installable_Builds\matrix\%1\win32\api\include

echo Making \Installable_Builds\matrix\%1\win32\api\libw32
md /s \Installable_Builds\matrix\%1\win32\api\libw32

echo Making \Installable_Builds\matrix\%1\win32\matrix\binw32
md /s \Installable_Builds\matrix\%1\win32\matrix\binw32

delay 2

echo.
xcopy /f %projpre%\lib.ira\mtrxapi.lib g:\Installable_Builds\matrix\%1\win32\api\libw32
xcopy /f %projpre%\include\src\clientid.h g:\Installable_Builds\matrix\%1\win32\api\include
xcopy /f %projpre%\include\src\ctxid.h g:\Installable_Builds\matrix\%1\win32\api\include
xcopy /f %projpre%\include\src\localeid.h g:\Installable_Builds\matrix\%1\win32\api\include
xcopy /f %projpre%\include\src\mtrxapi.h g:\Installable_Builds\matrix\%1\win32\api\include
xcopy /f %projpre%\include\src\soscb.h g:\Installable_Builds\matrix\%1\win32\api\include
xcopy /f %projpre%\include\src\sosdefs.h g:\Installable_Builds\matrix\%1\win32\api\include
xcopy /f %projpre%\include\src\threatid.h g:\Installable_Builds\matrix\%1\win32\api\include
xcopy /f %projpre%\bin.ira\mtrx32.dll g:\Installable_Builds\matrix\%1\win32\matrix\binw32

echo.
echo ****************************************************
echo Copy complete.
echo ****************************************************
echo.
delay 2




:-------------------------------------------------------------------------
: Zip up the build
:-------------------------------------------------------------------------

:do_zip
echo.
echo *****************************************************
echo Creating a zip of build # %1 on CM-Archive\archives...
echo *****************************************************
echo.
net use j: /d
net use j: \\cm-archive\archives"
j:
cd \
if isdir "temp" goto morezip
echo.
echo ********************************************************************************
echo Unable to map the J: drive to cm-archive/archives.  Aborting...
echo ********************************************************************************
goto done

:morezip

echo.
c:
cd \bld_area\matrix.%1
call pkzip25 -add -rec -path=root "j:\matrix_win32_%1.zip" *.*



echo.
delay 4
echo ********************************************************************************
echo Copying archive from J: to cm-ftp for localization...
echo ********************************************************************************
echo.
delay 4
g:
cd \build_environments
mkdir matrix
cd matrix
mkdir win32
cd win32
mkdir %1
cd %1
xcopy "j:\matrix_win32_%1.zip" .
echo.
delay 4
echo.
c:

echo ***************************************************************
echo Creating metrics for Matrix build %1 ...
echo ***************************************************************

cdd c:
if %1 gt 10 goto gt_10
@call metrics //depot/... matrix_ira_cm.0%@EVAL[%1-1] matrix_ira_cm.%1
goto do_postmet
:gt_10
@call metrics //depot/... matrix_ira_cm.%@EVAL[%1-1] matrix_ira_cm.%1

:do_postmet
echo ***************************************************************
echo Posting metrics on CMFTP ...
echo ***************************************************************

g:
cd \Installable_Builds\matrix\%1
mkdir win32
cd win32
mkdir metrics
cd metrics

copy   "c:\build\matrix*" .



c:
goto END



:USAGE
:-------------------------------------------------------------------------
: Error, display usage information
:-------------------------------------------------------------------------
:USAGE
cls
echo.
echo %0 starts the automated Matrix - Win32 build
echo.
echo Usage:
echo.
echo %0 build_num 
echo.
echo build_num is required.
echo.
echo Example:
echo            %0 05
echo.
echo It's hard-coded to use the c: drive.
echo.
goto DONE




:ERRMSG
echo.
echo **********************************
echo Error in build.
echo **********************************
echo.
echo Error text:   %ERRORMSG%
echo.
goto DONE


:done

cd %_INIT_DIR

echo.
echo Done.

quit

:END

