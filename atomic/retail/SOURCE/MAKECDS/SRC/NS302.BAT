@echo off
:@echo all
setlocal
timer

set dest_drv=k:
set dest_root=\nav\cd_stage\ns302
set src_drv=h:

:-------------------------------------------------------------------------
: CD generation script for NAV Solutions CD V3.02
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

: ---------   Map the source drive as H:   ---------

if not "%_dos%" == "NT" map root %src_drv = ussm-dev4\vol1:norton\sec\nav
if "%_dos%" == "NT" net use %src_drv \\ussm-dev4\vol1\norton\sec\nav
if not isdir %src_drv\ goto nomap

: ---------   Delete the previous contents of destination   ---------

gosub do_delete

:-------------------------------------------------------------------------
: Define build numbers
:-------------------------------------------------------------------------

set avb_num=17
set nav95_num=25f
set navnt_num=25f
set maxaxp_num=25f
set maxx86_num=25f
set admtools_num=25f
set rosnt_num=2no
set tesx86_num=2no
set tesaxp_num=2ns
set rosnec_num=05e
set tesnec_num=06a
set pic_num=3nc
set mvp_num=05
set pyro_num=18d
set navieg_num=07k
set luke_num=13v
set bhawk_num=48a
set nhawk_num=35e
set snapin_num=25f

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
copy %src_drv\avbundle\avbundle.%avb_num\avbundle\source\navvipcd\src\contents.txt %dest_drv%%dest_root\

:-------------------------------------------------------------------------
: Acrobat files to \acrobat
:-------------------------------------------------------------------------

echo.
echo Copying acrobat files
xcopy %src_drv\cdsuport\thirdparty\acrobat\win16\ar16e30.exe %dest_drv%%dest_root\acrobat\win16\ /s /e
xcopy %src_drv\cdsuport\thirdparty\acrobat\win32\ar32e30.exe %dest_drv%%dest_root\acrobat\win32\ /s /e

:-------------------------------------------------------------------------------
: Copy Frozen Savannah 3.02 CD environment directory structure to \admtools\nsc
: from the cdsuport area, nsc_302.  (Previously copied nsc_301.)
: The cdsuport area was updated by hand after the most recent Savannah build.
: This is also stored in the nsc302.zip file which is checked into Atomic Nobuilds.
:-------------------------------------------------------------------------------

echo.
echo Copying Savannah admtools\nsc files
xcopy %src_drv\cdsuport\fullproducts\nsc_302 %dest_drv%%dest_root\admtools\nsc\ /s /e

:-------------------------------------------------------------------------------
: Grab the savnav5 readme file and copy it as readme.txt in \admtools\nsc.
:-------------------------------------------------------------------------------

echo.
echo Copying Savannah readme file savnav5.txt and rename as readme.txt
copy %src_drv\avbundle\avbundle.%avb_num\avbundle\source\nobuilds\src\savnav5.txt %dest_drv%%dest_root\admtools\nsc\readme.txt

:-------------------------------------------------------------------------
: Previously, we took the \admtools\nsc\navext contents and copied it
: to a "navext.4" subdirectory, but this is no longer needed, per
: Brian Foster.
: was:  xcopy %src_drv\cdsuport\fullproducts\nsc_301\navext %dest_drv%%dest_root\admtools\nsc\navext.4\ /s /e
:-------------------------------------------------------------------------

:-------------------------------------------------------------------------
: Delete the previous navext directory contents (from Savannah) and get the
: Atomic pkginst "snap-in" instead to \admtools\nsc\navext.
:-------------------------------------------------------------------------

echo.
echo Deleting previous pkginst.
del %dest_drv%%dest_root\admtools\nsc\navext\*.* /y
echo.
echo Copying ATOMIC's pkginst.
xcopy %src_drv\navnt\maxwell\pkginst.%snapin_num\ %dest_drv%%dest_root\admtools\nsc\navext\ /s /e

:-------------------------------------------------------------------------------
: Force the eventmgr (isalert) to be a specific build number.  To do this,
: first delete the previous eventmgr directory contents from \admtools\nsc\eventmgr.
:-------------------------------------------------------------------------------

echo.
echo Deleting previous eventmgr.
del %dest_drv%%dest_root\admtools\nsc\eventmgr\*.* /y

:-------------------------------------------------------------------------
: Copy the Savannah 21j event manager to \admtools\nsc\eventmgr.
: Hard code the 21j number because we don't want anyone to accidentally
: increment this number.
: Per Brian Foster.
: Previously, this was 21i.
:-------------------------------------------------------------------------

echo.
echo Copying eventmgr.
xcopy %src_drv\savannah\savannah.21j\final\isalert\ %dest_drv%%dest_root\admtools\nsc\eventmgr\ /s /e

:-------------------------------------------------------------------------
: Liveupdt.nlu file to \admtools\lv_email
:-------------------------------------------------------------------------

echo.
echo Copying liveupdt.nlu.
xcopy %src_drv\avbundle\avbundle.%avb_num\avdocs\source\admtools\src\liveupdt.nlu %dest_drv%%dest_root\admtools\lv_email\ /s /e

:-------------------------------------------------------------------------
: NSCTOUR to \admtools\nsctour
:-------------------------------------------------------------------------

echo.
echo Copying nsctour demoshield files
xcopy %src_drv\cdsuport\fullproducts\nsctour\ %dest_drv%%dest_root\nsctour\ /s /e

:-------------------------------------------------------------------------
: Admin Tools to \admtools\tools
:-------------------------------------------------------------------------

echo.
echo Copying admtools files
xcopy %src_drv\navnt\maxwell\admtools.%admtools_num %dest_drv%%dest_root\admtools\tools\ /s /e

:-------------------------------------------------------------------------
: Doc files to \docs
:-------------------------------------------------------------------------

: \docs\

echo.
echo Copying doc files
xcopy %src_drv\avbundle\avbundle.%avb_num\avdocs\source\avg500\src\nav4dref.pdf %dest_drv%%dest_root\docs\navwks\ /s /e
xcopy %src_drv\avbundle\avbundle.%avb_num\avdocs\source\avg500\src\nav4dusr.pdf %dest_drv%%dest_root\docs\navwks\ /s /e
xcopy %src_drv\avbundle\avbundle.%avb_num\avdocs\source\avg500\src\nav5nref.pdf %dest_drv%%dest_root\docs\navwks\ /s /e
xcopy %src_drv\avbundle\avbundle.%avb_num\avdocs\source\avg500\src\nav5nusr.pdf %dest_drv%%dest_root\docs\navwks\ /s /e
xcopy %src_drv\avbundle\avbundle.%avb_num\avdocs\source\avg500\src\nav59ref.pdf %dest_drv%%dest_root\docs\navwks\ /s /e
xcopy %src_drv\avbundle\avbundle.%avb_num\avdocs\source\avg500\src\nav59usr.pdf %dest_drv%%dest_root\docs\navwks\ /s /e
xcopy %src_drv\avbundle\avbundle.%avb_num\avdocs\source\avg500\src\navnts50.pdf %dest_drv%%dest_root\docs\admin\ /s /e
xcopy %src_drv\avbundle\avbundle.%avb_num\avdocs\source\avg500\src\navimp.pdf %dest_drv%%dest_root\docs\admin\ /s /e

:\DOCS\NAVFW
xcopy %src_drv\avbundle\avbundle.%avb_num\avdocs\source\navfw\src\navfw.pdf %dest_drv%%dest_root\docs\navfw\ /s /e

:\DOCS\NAVIEG
xcopy %src_drv\avbundle\avbundle.%avb_num\avdocs\source\navieg\src\navieg.pdf %dest_drv%%dest_root\docs\navieg\ /s /e

:\DOCS\NAVNETW
xcopy %src_drv\avbundle\avbundle.%avb_num\avdocs\source\navnetw\src\n32netw.txt %dest_drv%%dest_root\docs\navnetw\ /s /e

:\DOCS\NAVXCHNG

:xcopy doesn't support creating a new directory and renaming a file at the same time,
:so just create the directory first and then do a copy.

md /s %dest_drv%%dest_root\docs\navxchng
copy %src_drv\avbundle\avbundle.%avb_num\avdocs\source\navxchng\src\navme.pdf %dest_drv%%dest_root\docs\navxchng\navxchng.pdf

:\DOCS\NAVNOTES
xcopy %src_drv\avbundle\avbundle.%avb_num\avdocs\source\navnotes\src\ %dest_drv%%dest_root\docs\navnotes\ /s /e

:\DOCS\NAVNLM
xcopy %src_drv\avbundle\avbundle.%avb_num\avdocs\source\navnlm\src\nvn4.pdf %dest_drv%%dest_root\docs\navnlm\ /s /e

:\DOCS\NAVC

:xcopy doesn't support creating a new directory and renaming a file at the same time,
:so just create the directory first and then do a copy.

md /s %dest_drv%%dest_root\docs\navc
copy %src_drv\avbundle\avbundle.%avb_num\avdocs\source\navc\src\nav2n_cs.pdf %dest_drv%%dest_root\docs\navc\navcugd.pdf

:\DOCS\PCA
xcopy %src_drv\avbundle\avbundle.%avb_num\avdocs\source\pca\src\pca32.pdf %dest_drv%%dest_root\docs\pca\ /s /e

:\docs\351
xcopy %src_drv\avbundle\avbundle.%avb_num\avdocs\source\avguide\src\navntref.pdf %dest_drv%%dest_root\docs\351\ /s /e
xcopy %src_drv\avbundle\avbundle.%avb_num\avdocs\source\avguide\src\navntugd.pdf %dest_drv%%dest_root\docs\351\ /s /e

:-------------------------------------------------------------------------
: MVP TOOLS                \macro
:-------------------------------------------------------------------------

echo.
echo Copying \macro files.
xcopy %src_drv\mvp\mvpdef.%mvp_num %dest_drv%%dest_root\macro\ /s /e

:-------------------------------------------------------------------------
: ATOMIC install + DISK sub-dirs to \nav9x
:-------------------------------------------------------------------------

echo.
echo Copying atomic 9x.
xcopy %src_drv\nav95\atomic\atomic.%nav95_num %dest_drv%%dest_root\nav9x\ /s /e
copy %src_drv\avbundle\avbundle.%avb_num\avbundle\source\navvipcd\src\instopts-9x.ini %dest_drv%%dest_root\nav9x\instopts.ini
copy %src_drv\avbundle\avbundle.%avb_num\avbundle\source\navvipcd\src\instopts-9x.ini %dest_drv%%dest_root\nav9x\disk1\instopts.ini

:-------------------------------------------------------------------------
: NAVC install \navc
:-------------------------------------------------------------------------

echo.
echo Copying \navc.
xcopy %src_drv\cdsuport\fullproducts\navc\ %dest_drv%%dest_root\navc\ /s /e

:-------------------------------------------------------------------------
: NAVDOSWN 10-pack (PICASSO) install + DISK sub-dirs to \navdoswn
:-------------------------------------------------------------------------

echo.
echo Copying \navdoswn 10-pack
xcopy %src_drv\navwin16\picasso\picasso.%pic_num\10pak\ %dest_drv%%dest_root\navdoswn\ /s /e

:-------------------------------------------------------------------------
: NAVDOSWN retail (PICASSO) install + DISK sub-dirs to \navdoswn\retail
:-------------------------------------------------------------------------

echo.
echo Copying \navdoswn retail
xcopy %src_drv\navwin16\picasso\picasso.%pic_num\picasso\ %dest_drv%%dest_root\navdoswn\retail\ /s /e

:-------------------------------------------------------------------------
: NAVFW for Firewalls install + DISK sub-dirs to \navfw
:-------------------------------------------------------------------------

echo.
echo Copying \navfw
:xcopy %src_drv\navnt\pyro\pyro.%pyro_num\pyro %dest_drv%%dest_root\navfw\ /s /e
xcopy %src_drv\navnt\pyro\pyro.%pyro_num\navfw %dest_drv%%dest_root\navfw\ /s /e

:-------------------------------------------------------------------------
: NAVIEG for Firewalls install + DISK sub-dirs to \navieg
:-------------------------------------------------------------------------

echo.
echo Copy \navieg
xcopy %src_drv\quikslvr\quikslvr.%navieg_num\navieg\ %dest_drv%%dest_root\navieg\ /s /e

:-------------------------------------------------------------------------
: DOS/Win Configuration Manager    \navnetw
:-------------------------------------------------------------------------

echo.
echo Copying navnetw files
xcopy %src_drv\avbundle\avbundle.%avb_num\avbundle\source\netmgris\src\ %dest_drv%%dest_root\navnetw\ /s /e

:-------------------------------------------------------------------------
: NAVNLM NAV for NETWARE install + DISK sub-dirs to \navnlm
:-------------------------------------------------------------------------

echo.
echo Copying \navnlm
xcopy %src_drv\navnlm\luke\luke.%luke_num %dest_drv%%dest_root\navnlm\ /s /e

:-------------------------------------------------------------------------
: NAV for Lotus Notes install + DISK sub-dirs to \navnotes
:-------------------------------------------------------------------------

echo.
echo Copying \navnotes
xcopy %src_drv\bhawk\bhawk.%bhawk_num\release\navnotes\ %dest_drv%%dest_root\navnotes\ /s /e

:-------------------------------------------------------------------------
: TESLA ALPHA install + DISK sub-dirs to \navntsv.351\alpha
:-------------------------------------------------------------------------

echo.
echo Copying \navntsv.351\alpha
xcopy %src_drv\navnt\tesla\tesaxp.%tesaxp_num %dest_drv%%dest_root\navntsv.351\alpha\ /s /e

:-------------------------------------------------------------------------
: TESLA INTEL install + DISK sub-dirs to \navntsv.351\intel
:-------------------------------------------------------------------------

echo.
echo Copying \navntsv.351\intel
xcopy %src_drv\navnt\tesla\tesx86.%tesx86_num %dest_drv%%dest_root\navntsv.351\intel\ /s /e

:-------------------------------------------------------------------------
: TESLANEC INTEL install + DISK sub-dirs to \navntsv.351\nec
:-------------------------------------------------------------------------

echo.
echo Copying \navntsv.351\nec
xcopy %src_drv\navnt\teslan\tesx86.%tesnec_num %dest_drv%%dest_root\navntsv.351\nec\ /s /e

:-------------------------------------------------------------------------
: MAXWELL ALPHA install + DISK sub-dirs to \navntsva
:-------------------------------------------------------------------------

echo.
echo Copying \navntsva
xcopy %src_drv\navnt\maxwell\maxaxp.%maxaxp_num %dest_drv%%dest_root\navntsva\ /s /e
copy %src_drv\avbundle\avbundle.%avb_num\avbundle\source\navvipcd\src\instopts-nts.ini %dest_drv%%dest_root\navntsva\instopts.ini
copy %src_drv\avbundle\avbundle.%avb_num\avbundle\source\navvipcd\src\instopts-nts.ini %dest_drv%%dest_root\navntsva\disk1\instopts.ini

:-------------------------------------------------------------------------
: MAXWELL INTEL install + DISK sub-dirs to \navntsvi
:-------------------------------------------------------------------------

echo.
echo Copying \navntsvi
xcopy %src_drv\navnt\maxwell\maxx86.%maxx86_num %dest_drv%%dest_root\navntsvi\ /s /e
copy %src_drv\avbundle\avbundle.%avb_num\avbundle\source\navvipcd\src\instopts-nts.ini %dest_drv%%dest_root\navntsvi\instopts.ini
copy %src_drv\avbundle\avbundle.%avb_num\avbundle\source\navvipcd\src\instopts-nts.ini %dest_drv%%dest_root\navntsvi\disk1\instopts.ini

:-------------------------------------------------------------------------
: ATOMIC NT install + DISK sub-dirs to \navntws
:-------------------------------------------------------------------------

echo.
echo Copying \navntws
xcopy %src_drv\navnt\atomic\atomic.%navnt_num %dest_drv%%dest_root\navntws\ /s /e
copy %src_drv\avbundle\avbundle.%avb_num\avbundle\source\navvipcd\src\instopts-ntw.ini %dest_drv%%dest_root\navntws\instopts.ini
copy %src_drv\avbundle\avbundle.%avb_num\avbundle\source\navvipcd\src\instopts-ntw.ini %dest_drv%%dest_root\navntws\disk1\instopts.ini

:-------------------------------------------------------------------------
: ROSWELL NT install + DISK sub-dirs to \navntws.351
:-------------------------------------------------------------------------

echo.
echo Copying \navntws.351
xcopy %src_drv\navnt\roswell\roswell.%rosnt_num %dest_drv%%dest_root\navntws.351\intel\ /s /e
xcopy %src_drv\navnt\rosnec\rosnec.%rosnec_num %dest_drv%%dest_root\navntws.351\nec\ /s /e

:-------------------------------------------------------------------------
: Atomic/Maxwell Demo Shield files to \navtour\alert
:                                 and \navtour\guided
:-------------------------------------------------------------------------

echo.
echo Copying NAVTOUR files
: Add NAVTOUR files here
xcopy %src_drv\cdsuport\fullproducts\atom_demoshield\ %dest_drv%%dest_root\navtour\ /s /e

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
echo Replacing partnum.dat
copy %src_drv\avbundle\avbundle.%avb_num\avbundle\source\navvipcd\src\partnum.dat %dest_drv%%dest_root\nav9x\partnum.dat
copy %src_drv\avbundle\avbundle.%avb_num\avbundle\source\navvipcd\src\partnum.dat %dest_drv%%dest_root\nav9x\disk1\partnum.dat
copy %src_drv\avbundle\avbundle.%avb_num\avbundle\source\navvipcd\src\partnum.dat %dest_drv%%dest_root\navntsva\partnum.dat
copy %src_drv\avbundle\avbundle.%avb_num\avbundle\source\navvipcd\src\partnum.dat %dest_drv%%dest_root\navntsva\disk1\partnum.dat
copy %src_drv\avbundle\avbundle.%avb_num\avbundle\source\navvipcd\src\partnum.dat %dest_drv%%dest_root\navntsvi\partnum.dat
copy %src_drv\avbundle\avbundle.%avb_num\avbundle\source\navvipcd\src\partnum.dat %dest_drv%%dest_root\navntsvi\disk1\partnum.dat
copy %src_drv\avbundle\avbundle.%avb_num\avbundle\source\navvipcd\src\partnum.dat %dest_drv%%dest_root\navntws\partnum.dat
copy %src_drv\avbundle\avbundle.%avb_num\avbundle\source\navvipcd\src\partnum.dat %dest_drv%%dest_root\navntws\disk1\partnum.dat
copy %src_drv\avbundle\avbundle.%avb_num\avbundle\source\navvipcd\src\partnum.dat %dest_drv%%dest_root\navntws.351\intel\partnum.dat
copy %src_drv\avbundle\avbundle.%avb_num\avbundle\source\navvipcd\src\partnum.dat %dest_drv%%dest_root\navntws.351\intel\disk1\partnum.dat
copy %src_drv\avbundle\avbundle.%avb_num\avbundle\source\navvipcd\src\partnum.dat %dest_drv%%dest_root\navntws.351\nec\partnum.dat
copy %src_drv\avbundle\avbundle.%avb_num\avbundle\source\navvipcd\src\partnum.dat %dest_drv%%dest_root\navntws.351\nec\disk1\partnum.dat
copy %src_drv\avbundle\avbundle.%avb_num\avbundle\source\navvipcd\src\partnum.dat %dest_drv%%dest_root\navntsv.351\alpha\partnum.dat
copy %src_drv\avbundle\avbundle.%avb_num\avbundle\source\navvipcd\src\partnum.dat %dest_drv%%dest_root\navntsv.351\alpha\disk1\partnum.dat
copy %src_drv\avbundle\avbundle.%avb_num\avbundle\source\navvipcd\src\partnum.dat %dest_drv%%dest_root\navntsv.351\intel\partnum.dat
copy %src_drv\avbundle\avbundle.%avb_num\avbundle\source\navvipcd\src\partnum.dat %dest_drv%%dest_root\navntsv.351\intel\disk1\partnum.dat
copy %src_drv\avbundle\avbundle.%avb_num\avbundle\source\navvipcd\src\partnum.dat %dest_drv%%dest_root\navntsv.351\nec\partnum.dat
copy %src_drv\avbundle\avbundle.%avb_num\avbundle\source\navvipcd\src\partnum.dat %dest_drv%%dest_root\navntsv.351\nec\disk1\partnum.dat

:-------------------------------------------------------------------------
: NAV for MS Exchange install + DISK sub-dirs to \navxchng
:-------------------------------------------------------------------------

echo.
echo Copying \navxchng
xcopy %src_drv\nhawk\nhawk.%nhawk_num\release\navexch\ %dest_drv%%dest_root\navxchng\ /s /e

:-------------------------------------------------------------------------
: Full Product PC Anywhere 8.0         \pca
:-------------------------------------------------------------------------

echo.
echo Copying PCA
xcopy "%src_drv\cdsuport\fullproducts\pca801lanhost\" %dest_drv%%dest_root\pca\ /s /e

:-------------------------------------------------------------------------
: SARC files to \sarc
:-------------------------------------------------------------------------

echo.
echo Copying SARC
xcopy %src_drv\avbundle\avbundle.%avb_num\avdocs\source\sarc\src\aboutvir.avi %dest_drv%%dest_root\sarc\ /s /e
xcopy %src_drv\avbundle\avbundle.%avb_num\avdocs\source\sarc\src\bloodhnd.pdf %dest_drv%%dest_root\sarc\ /s /e
xcopy %src_drv\avbundle\avbundle.%avb_num\avdocs\source\sarc\src\info32.pdf %dest_drv%%dest_root\sarc\ /s /e
xcopy %src_drv\avbundle\avbundle.%avb_num\avdocs\source\sarc\src\infont.pdf %dest_drv%%dest_root\sarc\ /s /e
xcopy %src_drv\avbundle\avbundle.%avb_num\avdocs\source\sarc\src\striker.pdf %dest_drv%%dest_root\sarc\ /s /e
xcopy %src_drv\avbundle\avbundle.%avb_num\avdocs\source\sarc\src\wpnavieg.pdf %dest_drv%%dest_root\sarc\ /s /e
xcopy %src_drv\cdsuport\sarc\sarc.avi %dest_drv%%dest_root\sarc\ /s /e

:-------------------------------------------------------------------------
: Trialware files to \trial
:-------------------------------------------------------------------------

echo.
echo Copying Trialware
xcopy "%src_drv\cdsuport\trials\act! 3.07\" %dest_drv%%dest_root\trial\act\ /s /e
xcopy "%src_drv\cdsuport\trials\dbanywhere 1.0\" %dest_drv%%dest_root\trial\dbany\ /s /e
xcopy "%src_drv\cdsuport\trials\pcanywhere 801\" %dest_drv%%dest_root\trial\pca\ /s /e
xcopy "%src_drv\cdsuport\trials\speeddisk nt\" %dest_drv%%dest_root\trial\speeddsk\ /s /e
xcopy "%src_drv\cdsuport\trials\winfax pro 803\wfx803tr.exe" %dest_drv%%dest_root\trial\winfax\ /s /e


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
del %dest_drv%%dest_root\*.* /syzq
echo.
echo %dest_drv%%dest_root\*.* deleted.

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

echo.
timer
endlocal
quit
