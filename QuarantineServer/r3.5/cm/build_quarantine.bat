@ECHO on

if "%1"=="" goto USAGE
set BuildNumber=%1

timer on
echo.
echo Automated Build Script for Quarantine
echo.

title Quarantine Build %1

call unset_bld_environment


:COBALTDBINSERT
set CODENUMBER=250
set start_datetime=%_YEAR-%_MONTH-%_DAY %_TIME
set cmtoolsdir=r:\cm
set Cobalt_DB=-h ussm-scspost.corp.symantec.com -u jgrigon -p12345 cobalt_bldhist

:::format: INSERT into build_data VALUES (id,code_number,build_number,start_datetime,finish_datetime,pass_fail,parameters,parsed_errors,comments);

echo INSERT into build_data VALUES (NULL,'%CODENUMBER%','%BUILDNUMBER%','%start_datetime%',NULL,'in_progress',NULL); > startinsert.sql
call %cmtoolsdir%\mysql %Cobalt_DB% < startinsert.sql >& mysql_update_output.txt


:: Setting Clients and Labels
set CLIENTSPEC=cm_qserver_r3.5_client
set LABELNAME=cm_qserver_r3.5_client
set RELCLIENT=release_qserver_r3.5_cm_client

set BehaviorBlockingBuildNumber=5
set ScanAndDeliverNumber=14
set CommonclientBuildNumber=003
set ccEraserBuildNumber=62


:: Setting Directory/Paths
set BuildNumberToUse=%1
SET build_err=build.err
SET BUILDINSTALL=NEW
set rootloc=%_CWD%
set startday=%2
set starthour=%3
set CMTOOLSDIR=c:\depot\tools\cm
set toolsdir=r:\
set BLDDIR=c:\bld_area\qserver3.5
set CLIENTROOT=%BLDDIR%
set BLDCMDIR=%BLDDIR%\norton_antivirus\quarantineserver\cm
set POSTDIR=\\ussm-scspost\unreleased_builds\CentralQuarantine\3.5
set LATESTDIR=\\ussm-scspost\unreleased_builds\CentralQuarantine\3.5\latest
Set ISHIELD=c:\program files\installshield\installshield 5.5 professional edition
set ARCHIVEDIR=\\ussm-cpd\ussm-cpd\build_environments\Archives
set ARCHIVEFILE=qserver_r3.5_%1.zip
set LOCALIZATION=\\ussm-scspost\Build_Environments\CentralQuarantine\3.5
set bld_info_file=%rootloc%\bldhist_file.txt
set sqlserver="-h cm-ftp.symantec.com -u hkim -phkim"
set METRICSDIR=%BLDDIR%\METRICS
set PROJECTNAME="Qserver 3.5 Win32 Platform"
set EMAIL=cm@symantec.com
set BinarySourceBehaviorBlocking=\\ussm-scspost\Unreleased_Builds\BehaviorBlocking\DrPhil\Build %BehaviorBlockingBuildNumber%
set BinarySourceCommonClient=\\ussm-scspost\Unreleased_Builds\CommonClient\CC20051\CommonClient20051.0.8\%CommonclientbuildNumber%
set BinarySourceScanAndDeliver=\\ussm-scspost\Unreleased_Builds\ScanAndDeliver\SwissChamp\%ScanAndDeliverNumber%\LAYOUTS\SND_FOR_RELEASE
set BinarySourceCcEraser=\\ussm-scspost\Unreleased_Builds\EraserEngine\Tectonics\Build %ccEraserBuildNumber%
set binANSIRelTarget=bin.ira
set MailCMD=call python %CMTOOLSDIR%\sendmail.py
set MailCMDOptions=/q /s
set BuildEmailNotifySubjectPrefix=CM Build Notification: %projectname%

:start_build_now

echo ************************************************************************************************
echo This section will send out an email alerting the team that the build has started.

cdd %rootloc%
python r:\cm\sendmail.py maillist.txt "CM Build Notification: Central Quarantine 3.5 Win32 Platform Build %1 has started" startmessage.txt /q /s
echo ************************************************************************************************

echo.
echo ***********************************************
echo The build is starting now...
echo ***********************************************
echo.


:: Setting up for entering data into the build database
cdd %rootloc%
call copy bldhist_default.txt bldhist_file.txt
echo. >> %bld_info_file%
if "%1"=="" goto ERROR
echo [bld_start_time] >> %bld_info_file%
python %toolsdir%\cm\sqltime.py >> %bld_info_file%
echo bld_label_count=1 >> %bld_info_file%
echo bld_label_1="%LABELNAME%.%1" >> %bld_info_file%
echo bld_build_number="%1" >> %bld_info_file%
echo bld_client_1="%CLIENTSPEC%" >> %bld_info_file%

echo **************************************************************************************************
echo removing the build environment

rd /s /q %BLDDIR%


: -----  Sync Quarantine -----

echo.
echo Syncing Quarantine ...
delay 4
call cd %rootloc%
call p4 sync -f ...
set p4client=%CLIENTSPEC%
p4 client -i < %rootloc%\clientspec.txt


echo.
echo Syncing Quarantine...
delay 4
set p4client=%CLIENTSPEC%
p4 sync -f > sync.log
echo.
echo Quarantine has been synced.
echo.
echo Now labeling Quarantine...
delay 4

p4 label -o %LABELNAME%.%1 > temp.lab
p4 label -i < temp.lab
p4 labelsync -l %LABELNAME%.%1 > labelsync.log
p4 label -o %LABELNAME%.%1 > temp.lab

iff exist r:\cm\locklabel.py then
    call python r:\cm\locklabel.py temp.lab
endiff

p4 label -i < temp.lab

: -----  Sync CCSDK -----

echo.
echo Labeling Quarantine complete.

:GetChangesForCobalt
setlocal
set toolsdir=r:\cm
rem ** This section will get and insert condensed changelist info for Cobalt database
echo.
echo ** Gathering changelist info...
set newlabel=%LABELNAME%.%BuildNumber%
set oldlabel=%LABELNAME%.%@EVAL[%BuildNumber%-1]
call %toolsdir%\isitalabel.btm %oldlabel%
iff %errorlevel% != 0 then
  echo.
  echo You will need to run changelist gathering manually!
  echo The previous build label, %oldlabel%, does not exist.
  echo.
else
  call python %toolsdir%\changes_between_labels_v2.py %oldlabel% %newlabel% --condensed --sql --build_number=%BuildNumber% --code_number=250
  set Cobalt_DB=-h ussm-scspost.corp.symantec.com -u jgrigon -p12345 cobalt_bldhist
  call %toolsdir\mysql %Cobalt_DB% < outfile.sql
  echo.
endiff
endlocal

echo
echo Getting CCSDK
call Copyccsdk.btm

echo
echo CCSDK copy complete
if %errorlevel% != 0 goto CCSDKERROR

delay 4

: -----  Build Quarantine

echo.
echo Now building Quarantine...
echo.
cdd %BLDCMDIR%
CALL build.btm
echo.

echo Checking...
echo.
delay 3
python %CMTOOLSDIR%\parseerrors.py %buildr_err% > buildr_errs.txt
if %@filesize[buildr_errs.txt] = 0 goto bld_dbg

echo ********************************
echo There are errors in the build.
echo ********************************


cat buildr_errs.txt
echo.

cdd %rootloc%
goto RELEASEERROR

:bld_dbg
echo.
echo Now building debug
cdd %BLDCMDIR%
CALL build.btm /dbg
echo.
echo Checking...
echo.
delay 3
python %CMTOOLSDIR%\parseerrors.py %buildd_err% > buildd_errs.txt

if %@filesize[buildd_errs.txt] = 0 goto okay 

echo ********************************
echo There are errors in the build.
echo ********************************
echo 
delay 1
echo 
delay 1
echo 

cat buildd_errs.txt
echo.

goto DEBUGERROR

:okay
echo Posting "latest" files to USSM-SCSPOST
mkdir %LATESTDIR% /s
cdd %BLDDIR%\norton_antivirus\quarantineserver\shared
call copy *.* /s %LATESTDIR%

:checkin
echo : -----  Check-In Quarantine files to Release
echo.
echo **********************************************************************
echo Setting the releasing client spec for release...
echo **********************************************************************
echo.
delay 4
cdd %BLDDIR%\norton_antivirus\quarantineserver\cm
echo set p4client=%relclient%
set p4client=%relclient%
p4 client -i < relclientspec.txt
delay 3

echo.
echo **********************************************************************
echo Here is what the client spec looks like:
echo **********************************************************************
echo.
delay 4
echo p4 client -o
p4 client -o
delay 3

echo.
echo **********************************************************************
echo Flushing the client...
echo **********************************************************************
echo.
delay 4

echo p4 flush -f
p4 flush -f

:do_posting

delay 4
echo.
echo **********************************************************************
echo Diffing files and posting in Perforce...
echo **********************************************************************
echo.
delay 4

cdd %BLDDIR%\norton_antivirus\quarantineserver\shared

dir /s /b /a:-d > \add.lst
p4 diff -se ... > \edit.lst

TEXT > \change.txt
# A Perforce Change Specification.
#
#  Change:      The change number. 'new' on a new changelist.  Read-only.
#  Date:        The date this specification was last modified.  Read-only.
#  Client:      The client on which the changelist was created.  Read-only.
#  User:        The user who created the changelist. Read-only.
#  Status:      Either 'pending' or 'submitted'. Read-only.
#  Description: Comments about the changelist.  Required.
#  Jobs:        What opened jobs are to be closed by this changelist.
#               You may delete jobs from this list.  (New changelists only.)
#  Files:       What opened files from the default changelist are to be added
#               to this changelist.  You may delete files from this list.
#               (New changelists only.)

Change: new

Status: new

Description:
	Submit Qserver 3.5 build qserver_build_no into the Release tree.
ENDTEXT

python r:\cm\sr.py \change.txt "qserver_build_no" "%1"

p4 change -i < \change.txt > \change.num

for %i in (@\change.num) do set changenum=%@word[1,%i]

p4 -x \add.lst add -c %changenum%
p4 -x \edit.lst edit -c %changenum%

del \add.lst /qyz
del \edit.lst /qyz
echo del \change.txt /qyz
echo del \change.num /qyz

p4 submit -c %changenum%


: -----  Getting ready for generating layouts

echo.
call subst w: /d
call net use w: /d
call subst w: %BLDDIR%
subst
cdd %BLDDIR%\norton_antivirus\quarantineserver\cm
call attribch.btm -r
echo.

: ----- Sign the SAV files (the cq files get signed in the install copy section)
cdd %BLDDIR%\norton_antivirus\quarantineserver\cm

pushd %BLDDIR%\Norton_AntiVirus\Corporate_Edition\Win32\src\%binansireltarget%
attrib *.* -r
popd
call "signfiles.btm" %BuildNumber% %BLDDIR%\Norton_AntiVirus\Corporate_Edition\Win32\src\%binansireltarget%
if %errorlevel% != 0 goto SIGNINGERROR


:install
cdd %BLDDIR%\norton_antivirus\quarantineserver\install\console
call build.btm
delay 4

cdd %BLDDIR%\norton_antivirus\quarantineserver\install\server
call build.btm
delay 4

: ----- Update lastet ALE
rem - commenting this - no longer used in sav, assuming new process for qserver.exe also
rem cdd %BLDDIR%\norton_antivirus\quarantineserver\cm
rem call latestale.btm

::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
:DeleteLayouts

call %cmtoolsdir%\deletedir.btm "%POSTDIR%"
IFF %errorlevel% != 0 THEN
	set Errorsubject=failed to delete old layouts!
	set ERRORSTRING=There was an error deleting older layouts.
	gosub CMErrorEmail
ENDIFF

: ----- Copying the layouts
echo.
echo Copying the layouts now
echo.
delay 4
call md /s %POSTDIR%\%1\qconsole
call md /s %POSTDIR%\%1\qserver
call copy "%BLDDIR%\Norton_AntiVirus\QuarantineServer\Install\Console\Install Images\Qconsole\Qconsole\DiskImages\DISK1" %POSTDIR%\%1\qconsole
call copy "%BLDDIR%\Norton_AntiVirus\QuarantineServer\Install\Server\Install Images\QServer\QServer\DiskImages\DISK1" %POSTDIR%\%1\qserver


: ----- Generate Metrics
delay 4
echo.
echo Generating Metrics
echo.
mkdir %METRICSDIR% /s

if [ %1 -gt 10 ]
then
	call metrics //depot/... %LABELNAME%.%@EVAL[%1-1] %LABELNAME%.%1 /q
    else
  	call metrics //depot/... %LABELNAME%.0%@EVAL[%1-1] %LABELNAME%.%1 /q
endiff

: ----- Copying the Metrics
delay 4
echo.
echo Copying the Metrics to USSM-SCSPOST
echo.
md /s %POSTDIR%\%1\metrics
cdd %METRICSDIR% 
call copy %METRICSDIR%\cm*.* %POSTDIR%\%1\metrics

: ----- SLOCFORCOBALT
rem This section calculates and inserts SLOC metrics for the Cobalt DB
pushd
cdd "%MetricsDir%"
call r:\cm\SLOC_metrics.btm qserver 3.5 %buildnumber% > %MetricsDir%\SLOC_gen_log.txt
popd
copy "%MetricsDir%\SLOC_gen_log.txt" "%POSTDIR%\%1\metrics\"

::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
:DeleteBEs

call %cmtoolsdir%\deletedir.btm "%LOCALIZATION" BE
IFF %errorlevel% != 0 THEN
	set Errorsubject=failed to delete older build environment files!
	set ERRORSTRING=There was an error deleting older build environment files.  Please delete manually if necessary.
	gosub CMErrorEmail
ENDIFF


: ----- Generating the zip file
delay 4
echo.
echo Generating the zip file for archiving and localization
echo.
call pkzip25 -add -rec -dir -sil c:\build\%ARCHIVEFILE% %BLDDIR%\*
cdd c:\build
echo Copying %ARCHIVEFILE% to the Archive Server.
echo.
call copy %ARCHIVEFILE% %ARCHIVEDIR%

: ---- Posting for localization
delay 4
echo Posting %ARCHIVEFILE% files for localization
echo.
cdd c:\build
md %LOCALIZATION%\%1
call move %ARCHIVEFILE% %LOCALIZATION%\%1


set BLDSTATUS=pass
echo ************************************************************************************************
echo This section will send out an email alerting the team that the build has finished.
cdd %rootloc%
python r:\cm\sendmail.py maillist.txt "CM Build Notification: Central Quarantine 3.5 Win32 Platform Build %1 has finished" finishmessage.txt /q /s
echo ************************************************************************************************

goto SUBMIT

:-------------------------------------------------------------------------
: Error, display usage information
:-------------------------------------------------------------------------

:USAGE
echo.
echo Usage:
echo.
echo build_quarantine <buildnumber>
echo.
echo Example:  build_quarantine 35
echo.
echo.
goto DONE

:HELPERROR
cdd %BLDDIR%\norton_antivirus\quarantineserver\cm
python r:\cm\sendmail.py maillist.txt "CM Build Notification: Central Quarantine 3.5 Win32 Platform Build %1 has Errored" "Error with Help Files" /q /s

set BLDSTATUS=fail
set BLDERROR="Error during generating Help Files" >> %bld_info_file%"


:RELEASEERROR
cdd %BLDDIR%\norton_antivirus\quarantineserver\cm
python r:\cm\sendmail.py maillist.txt "CM Build Notification: Central Quarantine 3.5 Win32 Platform Build %1 has Errored" %BLDCMDIR%\buildr_errs.txt /q /s

set BLDSTATUS=fail
set BLDERROR="echo bld_errors=c:/bld_area/qserver3.5/norton_antivirus/quarantineserver/cm/buildr_errs.txt" >> %bld_info_file%"

goto SUBMIT

:DEBUGERROR
cdd %BLDDIR%\norton_antivirus\quarantineserver\cm
python r:\cm\sendmail.py maillist.txt "CM Build Notification: Central Quarantine 3.5 Win32 Platform Build %1 Debug has Errored" %BLDCMDIR%\buildd_errs.txt /q /s

set BLDSTATUS=fail
set BLDERROR="echo bld_errors=c:/bld_area/qserver3.5/norton_antivirus/quarantineserver/cm/buildd_errs.txt" >> %bld_info_file%"

goto SUBMIT

:INSTALLCOPYERROR
cdd %BLDDIR%\norton_antivirus\quarantineserver\cm
python r:\cm\sendmail.py maillist.txt "CM Build Error Notification: Central Quarantine 3.5 Win32 Platform Build %1 has Errored" buildinstall_errs.txt /q /s

set BLDSTATUS=fail
set BLDERROR="echo bld_errors="Install portion errored" >> %bld_info_file%"

goto SUBMIT

:SIGNINGERROR
cdd %BLDDIR%\norton_antivirus\quarantineserver\cm
python r:\cm\sendmail.py maillist.txt "CM Build Error Notification: Central Quarantine 3.5 Win32 Platform Build %1 has Errored files not signed" "There was an error during the Digital Signing Process." /q /s

set BLDSTATUS=fail
set BLDERROR="echo bld_errors="There was an error during signing." >> %bld_info_file%"

goto SUBMIT

:CCSDKERROR
cdd %BLDDIR%\norton_antivirus\quarantineserver\cm
python r:\cm\sendmail.py maillist.txt "CM Build Error Notification: Central Quarantine 3.5 Win32 Platform Build %1 has Errored, CCSDK (copy) not completed successfully." "There was an error during the CCSDK copy process" /q /s

set BLDSTATUS=fail
set BLDERROR="echo bld_errors="There was an error during the CCSDK copy process." >> %bld_info_file%"

goto SUBMIT

::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
:CMErrorEmail
rem Handle an error, we allow the calling routine to put info to display in the ERRORSTRING variable, or to use
rem a file to hold error messages.
echo.
echo ERROR: %ERRORSTRING%
echo.
echo Sending email notification so that CM is aware of the issue.

%MailCMD% "DL-ENG-SCS-CM" "%BuildEmailNotifySubjectPrefix% build %BuildNumberToUse% %Errorsubject%" "%Errorstring%" %MailCMDOptions%

set errorlevel=
set Errorsubject=
set errorstring=

return


:SUBMIT

:COBALTDBUPDATE
set CODENUMBER=250
set finish_datetime=%_YEAR-%_MONTH-%_DAY %_TIME
set cmtoolsdir=r:\cm
set Cobalt_DB=-h ussm-scspost.corp.symantec.com -u jgrigon -p12345 cobalt_bldhist
iff "%BLDSTATUS%" = "fail" then
  set passing=fail
else 
  set passing=pass
endiff

echo UPDATE build_data SET pass_fail='%passing%',finish_datetime='%finish_datetime%' WHERE code_number=%CODENUMBER% and build_number=%BUILDNUMBER%; > updateinsert.sql

call %cmtoolsdir%\mysql %Cobalt_DB% < updateinsert.sql >>& mysql_update_output.txt


echo **************************************************************
echo Submitting to the Build History Database
echo **************************************************************
echo.
cdd %rootloc%
echo [bld_finish_time] >> %bld_info_file%
python %toolsdir%\cm\sqltime.py >> %bld_info_file%
echo bld_pass_fail="%BLDSTATUS%" >> %bld_info_file%
set > \environment.txt
echo bld_environment=c:/environment.txt >> %bld_info_file%
echo bld_os="NT"	>> %bld_info_file%
%BLDERROR%
echo bld_zipfile="%ARCHIVEFILE%"   >> %bld_info_file%
python %toolsdir%\cm\bldhist_driver.py create_insert bldhist.sql
if errorlevel != 0 exit 2
python %toolsdir%\cm\bldhist_driver.py perform_insert bldhist.sql %sqlserver%
if errorlevel != 0 exit 3

timeroff

if "%BLDSTATUS%" == "fail" goto FAILEXIT
exit

:FAILEXIT
echo BUILD FAILED
exit 1



