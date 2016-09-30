@echo off
:@echo all
setlocal

:-------------------------------------------------------------------------
: Uncomment these statements if you want to run a test in the DEV area.
:-------------------------------------------------------------------------

:set dest_drv=d:
:set dest_root=\nav\cd_stage\nav50_oem\nav50002
:set src_drv=j:\builds\sec
:set nt_drv=j:

:-------------------------------------------------------------------------
: Comment these statements out if you want to run a test in the DEV area.
:-------------------------------------------------------------------------

set dest_drv=k:
set dest_root=\nav\cd_stage\nav50_oem\nav50002
set src_drv=h:\builds\sec
set nt_drv=h:

:-------------------------------------------------------------------------
: CD generation script for ATOMIC OEM (COMPAQ) CD
:-------------------------------------------------------------------------

:-------------------------------------------------------------------------
: Developer:

: Source files:  All files in SOURCE directories must be referenced
:                by the drive letter %src_drv.  This batch file maps the
:                drive letter %src_drv to the setting of the "source_root"
:                variable.  This variable must always point to
:                DEV4\VOL1.

: Destination:   To allow for testing flexibility, the destination
:                drive letter may be a local drive or it may be
:                be mapped to a network drive prior to execution
:                of this batch file.
:-------------------------------------------------------------------------

:Verify a few things first.

gosub do_verify

: ---------   Map the source drive as %src_drv   ---------

if not "%_dos%" == "NT" map %nt_drv = \\ussm-dev3\nav:builds\sec
if "%_dos%" == "NT" net use %nt_drv \\ussm-dev3\nav
if not isdir %src_drv\ goto nomap

: ---------   Delete the previous contents of destination   ---------

gosub do_delete

:-------------------------------------------------------------------------
: Define build numbers
:-------------------------------------------------------------------------

set avb_num=19a
set nav95_num=c9x0002.33d
set navnt_num=cnt0002.33d
set atcdhlp_num=33b

echo.
echo ***********************************************************
echo Copying files to %dest_drv%%dest_root\
echo ***********************************************************
echo.

:-------------------------------------------------------------------------
: Start copying files to destination
:-------------------------------------------------------------------------

:-------------------------------------------------------------------------
: Root level CD files \
:-------------------------------------------------------------------------

echo.
echo Copying root level files
xcopy %src_drv\avbundle\avbundle.%avb_num\avbundle\source\nav50002\src\autorun.inf %dest_drv%%dest_root\

: The files below are built by CDSTRHLP/CDSTART in Atomic and are copied by CM
: to the atcdhlp.xx directory on dev4\vol1 after every Atomic build.

copy %src_drv\nav95\atomic\atcdhlp.%atcdhlp_num\cdstart.exe %dest_drv%%dest_root\
copy %src_drv\nav95\atomic\atcdhlp.%atcdhlp_num\autooem.hlp %dest_drv%%dest_root\cdstart.hlp

copy %src_drv\avbundle\avbundle.%avb_num\avbundle\source\nobuilds\src\cdstart.ico %dest_drv%%dest_root\
copy %src_drv\avbundle\avbundle.%avb_num\avbundle\source\nav50002\src\0002contents.txt %dest_drv%%dest_root\contents.txt

:-------------------------------------------------------------------------
: Acrobat files to \acrobat
:-------------------------------------------------------------------------

echo.
echo Copying acrobat files
xcopy %src_drv\cdsuport\thirdparty\acrobat\win32\ar32e30.exe %dest_drv%%dest_root\acrobat\win32\ /s /e

:-------------------------------------------------------------------------
: Doc files to \docs
:-------------------------------------------------------------------------

echo.
echo Copying doc files
xcopy %src_drv\avbundle\avbundle.%avb_num\avdocs\source\avg500.oem\src\av4w2.pdf %dest_drv%%dest_root\docs\ /s /e
ren %dest_drv%%dest_root\docs\av4w2.pdf nav59ref.pdf
copy %src_drv\avbundle\avbundle.%avb_num\avdocs\source\avg500.oem\src\av498.pdf %dest_drv%%dest_root\docs\nav59usr.pdf
copy %src_drv\avbundle\avbundle.%avb_num\avdocs\source\avg500.oem\src\av4n2.pdf %dest_drv%%dest_root\docs\nav5nref.pdf
copy %src_drv\avbundle\avbundle.%avb_num\avdocs\source\avg500.oem\src\av5nt.pdf %dest_drv%%dest_root\docs\nav5nusr.pdf
copy %src_drv\avbundle\avbundle.%avb_num\avdocs\source\navc\src\nav2n_cs.pdf %dest_drv%%dest_root\docs\navcugd.pdf

:-------------------------------------------------------------------------
: ATOMIC install + DISK sub-dirs to \nav9x
:-------------------------------------------------------------------------

echo.
echo Copying Atomic 95
xcopy %src_drv\nav95\atomic\%nav95_num %dest_drv%%dest_root\nav9x\ /s /e
copy %src_drv\avbundle\avbundle.%avb_num\avbundle\source\nav50002\src\instopts.ini %dest_drv%%dest_root\nav9x\instopts.ini
copy %src_drv\avbundle\avbundle.%avb_num\avbundle\source\nav50002\src\instopts.ini %dest_drv%%dest_root\nav9x\disk1\instopts.ini

:-------------------------------------------------------------------------
: NAVC install \navc
:-------------------------------------------------------------------------

echo.
echo Copying Navc
xcopy %src_drv\cdsuport\fullproducts\navc %dest_drv%%dest_root\navc\ /s /e

:-------------------------------------------------------------------------
: ATOMIC NT install + DISK sub-dirs to \navntws
:-------------------------------------------------------------------------

echo.
echo Copying Atomic NT
xcopy %src_drv\navnt\atomic\%navnt_num %dest_drv%%dest_root\navntws\ /s /e
copy %src_drv\avbundle\avbundle.%avb_num\avbundle\source\nav50002\src\instopts-ntw.ini %dest_drv%%dest_root\navntws\instopts.ini
copy %src_drv\avbundle\avbundle.%avb_num\avbundle\source\nav50002\src\instopts-ntw.ini %dest_drv%%dest_root\navntws\disk1\instopts.ini

:-------------------------------------------------------------------------
: Atomic/Maxwell Demo Shield files to \navtour
:-------------------------------------------------------------------------

echo.
echo Copying NAVTOUR files
: Add NAVTOUR files here
xcopy %src_drv\cdsuport\fullproducts\atom_demoshield %dest_drv%%dest_root\navtour\ /s /e
del %dest_drv%%dest_root\navtour\alert\16bit /s /x /y
del %dest_drv%%dest_root\navtour\guided\16bit /s /x /y

: Delete the README.TXT files from alert, guided.  They will be replaced on the
: \NAVTOUR directory by AVBUNDLE versions.

del %dest_drv%%dest_root\navtour\alert\readme.txt
del %dest_drv%%dest_root\navtour\guided\readme.txt

: Add in the alert.txt and guided.txt into the \NAVTOUR directory.

copy %src_drv\avbundle\avbundle.%avb_num\avbundle\source\nobuilds\src\alert.txt %dest_drv%%dest_root\navtour
copy %src_drv\avbundle\avbundle.%avb_num\avbundle\source\nobuilds\src\guided.txt %dest_drv%%dest_root\navtour

:-------------------------------------------------------------------------
: Replace partnum.dat files with avbundle partnum.dat
:-------------------------------------------------------------------------

echo.
echo Copying Partnum replacements
copy %src_drv\avbundle\avbundle.%avb_num\avbundle\source\nav50002\src\0002partnum-Nav9x.dat %dest_drv%%dest_root\nav9x\partnum.dat
copy %src_drv\avbundle\avbundle.%avb_num\avbundle\source\nav50002\src\0002partnum-Nav9x.dat %dest_drv%%dest_root\nav9x\disk1\partnum.dat
copy %src_drv\avbundle\avbundle.%avb_num\avbundle\source\nav50002\src\0002partnum-NavNTWks.dat %dest_drv%%dest_root\navntws\partnum.dat
copy %src_drv\avbundle\avbundle.%avb_num\avbundle\source\nav50002\src\0002partnum-NavNTWks.dat %dest_drv%%dest_root\navntws\disk1\partnum.dat

:-------------------------------------------------------------------------
: SARC files to \sarc
:-------------------------------------------------------------------------

echo.
echo Copying SARC files
xcopy %src_drv\avbundle\avbundle.%avb_num\avdocs\source\sarc\src\aboutvir.avi %dest_drv%%dest_root\sarc\ /s /e
xcopy %src_drv\avbundle\avbundle.%avb_num\avdocs\source\sarc\src\bloodhnd.pdf %dest_drv%%dest_root\sarc\ /s /e
xcopy %src_drv\avbundle\avbundle.%avb_num\avdocs\source\sarc\src\info32.pdf %dest_drv%%dest_root\sarc\ /s /e
xcopy %src_drv\avbundle\avbundle.%avb_num\avdocs\source\sarc\src\infont.pdf %dest_drv%%dest_root\sarc\ /s /e
xcopy %src_drv\avbundle\avbundle.%avb_num\avdocs\source\sarc\src\striker.pdf %dest_drv%%dest_root\sarc\ /s /e
xcopy %src_drv\cdsuport\sarc\sarc.avi %dest_drv%%dest_root\sarc\ /s /e

:-------------------------------------------------------------------------
: PCAnywhere 802 Serialized Host Only files.      \support\awhost
:-------------------------------------------------------------------------

echo.
echo Copying \support\awhost
xcopy %src_drv\cdsuport\fullproducts\pca802ser_host_only %dest_drv%%dest_root\support\awhost\ /s /e

:-------------------------------------------------------------------------
: Support directory files.                        \support\getmbr
:-------------------------------------------------------------------------

echo.
echo Copying \support\getmbr
xcopy %src_drv\avbundle\avbundle.%avb_num\avbundle\source\nav__500\src\getmbr.zip %dest_drv%%dest_root\support\getmbr\

:-------------------------------------------------------------------------
: Trialware files to \trial
:-------------------------------------------------------------------------

echo.
echo Copying Trialware
xcopy "%src_drv\cdsuport\trials\act! 3.07\*.*" %dest_drv%%dest_root\trial\act\ /s /e
xcopy "%src_drv\cdsuport\trials\dbanywhere 1.0\*.*" %dest_drv%%dest_root\trial\dbany\ /s /e
xcopy "%src_drv\cdsuport\trials\pcanywhere 801\*.*" %dest_drv%%dest_root\trial\pca\ /s /e
xcopy "%src_drv\cdsuport\trials\speeddisk nt\*.*" %dest_drv%%dest_root\trial\speeddsk\ /s /e
xcopy "%src_drv\cdsuport\trials\winfax pro 803\*.*" %dest_drv%%dest_root\trial\winfax\ /s /e
xcopy "%src_drv\cdsuport\trials\nu3trial\*.*" %dest_drv%%dest_root\trial\nu3trial\ /s /e


:-------------------------------------------------------------------------
: Done!
:-------------------------------------------------------------------------

echo.
echo.
echo Finished copying CD image to %dest_drv%%dest_root!
goto end

:-------------------------------------------------------------------------
: Subroutines
:-------------------------------------------------------------------------

:-------------------------------------------------------------------------
: Validate a few things
:-------------------------------------------------------------------------

:DO_VERIFY

if "%1"=="/help" .or. "%1"=="?" .or. "%1"=="/?" goto USAGE
:if "%_dos%" == "NT" goto nont
if not isdir %dest_drv%\ goto nodest

return

:DO_DELETE

: ---------   Delete the previous contents of dest_root   ---------

if not isdir %dest_drv%%dest_root goto alrdltd
echo.
echo Deleting the contents of %dest_drv%%dest_root\
echo.
inkey Press enter to continue or q to quit: %%answer
echo.
if "%answer"=="q" goto end
del %dest_drv%%dest_root /qsxyz
echo.
echo %dest_drv%%dest_root\ deleted.

return

:ALRDLTD
md /s %dest_drv%%dest_root%

return

:-------------------------------------------------------------------------
: Error, display usage information
:-------------------------------------------------------------------------
:USAGE
cls
echo.
echo.
echo %0 copies files from a source directory to a destination
echo directory for the purpose of creating a CD image.  The image
echo can then be burned to a CD by Configuration Management.
echo.
echo Usage:  %0     (No arguments)
echo.
echo.
echo.
goto END

:-------------------------------------------------------------------------
: Error MSGS
:-------------------------------------------------------------------------
:NODEST
echo.
echo.
echo ERROR! Destination %dest_drv not found.
echo.
echo Please make sure %dest_drv is mapped to the drive/server
echo that you are copying the CD image to.
echo.
echo.
goto END

:NOMAP
echo.
echo.
echo ERROR! Could not complete network map.
echo.
echo Check %0 batch file.
echo.
goto END

:NONT
echo.
echo.
echo ERROR! NT is not currently supported.
echo Please use Windows 95/98.
echo.
goto END


:-------------------------------------------------------------------------
: The End
:-------------------------------------------------------------------------
:END

:Author - Dwight Wilson
:         Configuration Management

endlocal
quit
