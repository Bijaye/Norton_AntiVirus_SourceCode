@echo off
:@echo all
setlocal

:-------------------------------------------------------------------------
: Uncomment these statements if you want to run a test in the DEV area.
:-------------------------------------------------------------------------

:set dest_drv=d:
:set dest_root=\nav\cd_stage\nav50_oem\nav50001
:set src_drv=j:\builds\sec
:set nt_drv=j:

:-------------------------------------------------------------------------
: Comment these statements out if you want to run a test in the DEV area.
:-------------------------------------------------------------------------

set dest_drv=k:
set dest_root=\nav\cd_stage\nav50_oem\nav50001
set src_drv=h:\builds\sec
set nt_drv=h:

:-------------------------------------------------------------------------
: CD generation script for ATOMIC OEM (IBM) CD
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

if not "%_dos%" == "NT" map %nt_drv = ussm-dev3\nav:builds\sec
if "%_dos%" == "NT" net use %nt_drv \\ussm-dev3\nav
if not isdir %src_drv\ goto nomap

: ---------   Delete the previous contents of destination   ---------

gosub do_delete

:-------------------------------------------------------------------------
: Define build numbers
:-------------------------------------------------------------------------

set avb_num=19
set nav95_num=c9x0001.33d
set navnt_num=cnt0001.33d
set navnts_num=cx860001.33d
set navpftw_num=atomic.33d

echo.
echo ***********************************************************
echo Copying files to %dest_drv%%dest_root\
echo ***********************************************************
echo.

:-------------------------------------------------------------------------
: Start copying files to destination
:-------------------------------------------------------------------------

:-------------------------------------------------------------------------
: ATOMIC install (CD only) to \nav9x
:-------------------------------------------------------------------------

echo.
echo Copying Atomic 95 (CD only)
xcopy %src_drv\nav95\atomic\%nav95_num %dest_drv%%dest_root\Wrkstatn\CD_layout\nav9x\ /i
copy %src_drv\avbundle\avbundle.%avb_num\avbundle\source\nav50001\src\instopts.ini %dest_drv%%dest_root\Wrkstatn\CD_layout\nav9x\instopts.ini

:-------------------------------------------------------------------------
: ATOMIC install (Diskette only) \nav9x
:-------------------------------------------------------------------------

echo.
echo Copying Atomic 95 (diskette only)
xcopy %src_drv\nav95\atomic\%nav95_num %dest_drv%%dest_root\Wrkstatn\Floppy_layout\nav9x\ /s /e
del %dest_drv%%dest_root\Wrkstatn\Floppy_layout\nav9x\*.* /y
copy %src_drv\avbundle\avbundle.%avb_num\avbundle\source\nav50001\src\instopts.ini %dest_drv%%dest_root\Wrkstatn\Floppy_layout\nav9x\disk1\instopts.ini

:-------------------------------------------------------------------------
: ATOMIC install (Self extracting file only) \nav9x
:-------------------------------------------------------------------------

echo.
echo Copying Atomic 95 (self extracting file only)
xcopy %src_drv\pftw\%navpftw_num\nav9xus.exe %dest_drv%%dest_root\Wrkstatn\wrapped\nav9x\ /s /e

:-------------------------------------------------------------------------
: ATOMIC NT install (CD only) to \navnt
:-------------------------------------------------------------------------

echo.
echo Copying Atomic NT (CD only)
xcopy %src_drv\navnt\atomic\%navnt_num %dest_drv%%dest_root\Wrkstatn\CD_layout\navnt\ /i
copy %src_drv\avbundle\avbundle.%avb_num\avbundle\source\nav50001\src\instopts-ntw.ini %dest_drv%%dest_root\Wrkstatn\CD_layout\navnt\instopts.ini

:-------------------------------------------------------------------------
: ATOMIC NT install (diskette only) to \navnt
:-------------------------------------------------------------------------

echo.
echo Copying Atomic NT (diskette only)
xcopy %src_drv\navnt\atomic\%navnt_num %dest_drv%%dest_root\Wrkstatn\Floppy_layout\navnt\ /s /e
del %dest_drv%%dest_root\Wrkstatn\Floppy_layout\navnt\*.* /y
copy %src_drv\avbundle\avbundle.%avb_num\avbundle\source\nav50001\src\instopts-ntw.ini %dest_drv%%dest_root\Wrkstatn\Floppy_layout\navnt\disk1\instopts.ini

:-------------------------------------------------------------------------
: ATOMIC NT install (self extracting file only) to \navnt
:-------------------------------------------------------------------------

echo.
echo Copying Atomic NT (self extracting file only)
xcopy %src_drv\pftw\%navpftw_num\navntus.exe %dest_drv%%dest_root\Wrkstatn\wrapped\navnt\ /s /e

:-------------------------------------------------------------------------
: ATOMIC NT Server install (CD only) to \navnts
:-------------------------------------------------------------------------

echo.
echo Copying Atomic NT (CD only)
xcopy %src_drv\navnt\maxwell\%navnts_num %dest_drv%%dest_root\server\CD_layout\navnts\ /i
copy %src_drv\avbundle\avbundle.%avb_num\avbundle\source\nav50001\src\instopts-nts.ini %dest_drv%%dest_root\server\CD_layout\navnts\instopts.ini
xcopy %src_drv\avbundle\avbundle.%avb_num\avdocs\source\avg500.oem\src\navnts50.pdf %dest_drv%%dest_root\server\CD_layout\docs\ /s /e

:-------------------------------------------------------------------------
: Replace partnum.dat files with avbundle partnum.dat
:-------------------------------------------------------------------------

echo.
echo Copying Partnum replacements
copy %src_drv\avbundle\avbundle.%avb_num\avbundle\source\nav50001\src\0001PartNum-Nav9x.dat %dest_drv%%dest_root\Wrkstatn\CD_layout\nav9x\partnum.dat
copy %src_drv\avbundle\avbundle.%avb_num\avbundle\source\nav50001\src\0001PartNum-Nav9x.dat %dest_drv%%dest_root\Wrkstatn\Floppy_layout\nav9x\disk1\partnum.dat
copy %src_drv\avbundle\avbundle.%avb_num\avbundle\source\nav50001\src\0001PartNum-NavNTWks.dat %dest_drv%%dest_root\Wrkstatn\CD_layout\navnt\partnum.dat
copy %src_drv\avbundle\avbundle.%avb_num\avbundle\source\nav50001\src\0001PartNum-NavNTWks.dat %dest_drv%%dest_root\Wrkstatn\Floppy_layout\navnt\disk1\partnum.dat
copy %src_drv\avbundle\avbundle.%avb_num\avbundle\source\nav50001\src\0001PartNum-NavNTSrv.dat %dest_drv%%dest_root\server\CD_layout\navnts\partnum.dat


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
