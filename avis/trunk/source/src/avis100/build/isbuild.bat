echo make immune system
echo set Perl installation directory:
CALL C:\visualstudio\vc98\bin\vcvars32.bat
set PERL5DIR=D:\perl
set ISDIR=D:\is

echo get all files from source code control

REM ss Get -V "$/Avis100;L %1" -R

rem compile perl
REM del /s/f/q %PERL5DIR%
REM cd D:\%1\src\Avis100\Other\Perl\perl5\win32
REM nmake
REM nmake install
REM ###### cd ..
REM ###### cd ..

set path=%path%;%PERLDIR%\bin

rem compile perl extensions
rem first do libwin32 (really important)
cd D:\%1\src\Avis100\Other\Perl\libwin32
perl Makefile.pl
nmake
nmake install
REM ###### cd ..

rem next do libwww
cd D:\%1\src\Avis100\Other\Perl\libwww
perl Makefile.pl
nmake
nmake install
REM ###### cd ..


rem next do Data Dumper
cd D:\%1\src\Avis100\Other\Perl\Data-Dumper
perl Makefile.pl
nmake
nmake install
REM ####### cd ..

rem next do MD5
cd D:\%1\src\Avis100\Other\Perl\md5
perl Makefile.pl
nmake
nmake install
REM ####### cd ..

rem next do Mime-Base64
cd D:\%1\src\Avis100\Other\Perl\mime-base64
perl Makefile.pl
nmake
nmake install
REM ###### cd ..

rem next do HTML-Parser
cd D:\%1\src\Avis100\Other\Perl\HTML-Parser
perl Makefile.pl
nmake
nmake install
REM ####### cd ..

rem next do libnet
cd D:\%1\src\Avis100\Other\Perl\libnet
perl Makefile.pl
nmake
nmake install
REM ###### cd ..

rem next do MQSeries
REM cd E:\%1\src\Avis100\Other\Perl\MQSeries
REM perl Makefile.pl
REM nmake
REM nmake install
REM ####### cd ..

rem next do Text-CSV_XS
cd D:\%1\src\Avis100\Other\Perl\Text-CSV_XS
perl Makefile.pl
nmake
nmake install
REM ####### cd ..

rem next do MCV
cd D:\%1\src\Avis100\pm\mcv
perl Makefile.pl
nmake
nmake install
REM ####### cd ..

rem next do EntTrac
cd D:\%1\src\Avis100\pm\EntTrac
perl Makefile.pl
nmake
nmake install
REM ####### cd ..

rem next do NAVAPI
cd D:\%1\src\Avis100\pm\NAVAPI
perl Makefile.pl
nmake
nmake install
REM ####### cd ..

rem next do Times
cd D:\%1\src\Avis100\pm\AVIS\Times
perl Makefile.pl
nmake
nmake install
REM ####### cd ..

rem next do crc NOT USED ANYMORE
REM ####### cd crc
REM ####### nmake
REM ####### nmake install
REM ####### cd ..

rem next do autosig/sigcompr
REM ####### cd autosig/sigcompr
REM ####### nmake all
REM ####### cd ..
REM ####### cd ..

cd D:\%1\src\Avis100

rem set perl environment variabales

rem copy build programs from the build directory to the right place

copy D:\%1\src\Avis100\Build\Isbuild.pl
copy D:\%1\src\Avis100\Build\Isbuild.pm
REM copy D:\%1\src\Avis100\Build\mymacros.dsm %MSDEVDIR%\Macros

rem compile other C programs
REM del /s/f/q %ISDIR%
REM mkdir %ISDIR%
perl D:\%1\src\Avis100\Isbuild.pl --compile --install

cd D:\%1\src\Avis100\autosig3
CALL D:\%1\src\Avis100\autosig3\mdbscore.bat
CALL D:\%1\src\Avis100\autosig3\mbldindex.bat
CALL D:\%1\src\Avis100\autosig3\mcombindx.bat
CALL D:\%1\src\Avis100\autosig3\mtrie.bat
cd ..

rem copy local installation of perl to staging directory (with local extensions)

del /s/f/q D:\NTWorkstation\perl
xcopy %PERL5DIR% D:\NTWorkstation\perl\ /s
copy %PERL5DIR%\*. D:\NTWorkstation\perl\bin\
del D:\NTWorkstation\perl\*.

rem copy local immune system installation to staging directory

del /s/f/q D:\NTWorkstation\bin
del /s/f/q D:\NTWorkstation\data

xcopy %ISDIR%\bin  D:\NTWorkstation\bin /s/i
xcopy %ISDIR%\data D:\NTWorkstation\data /s/i

rem ####################################################################
rem NTWorkstation Staging:

set AVISWS=D:\NTWorkstation
REM xcopy D:\%1\src\Avis100\Other\Interim\Dataflow\*.* %AVISWS%\bin /c

xcopy D:\%1\src\Avis100\Other\Fprot           D:\NTWorkstation\Fprot /s/i
xcopy D:\%1\src\Avis100\Other\Nav             D:\NTWorkstation\Nav /s/i
xcopy D:\%1\src\Avis100\Other\ScanNT          D:\NTWorkstation\Scan /s/i
xcopy D:\%1\src\Avis100\Other\data\vimages    D:\NTWorkstation\data\vimages /s/i
xcopy D:\%1\src\Avis100\Other\data\vresources D:\NTWorkstation\data\vresources /s/i
xcopy D:\%1\src\Avis100\pm\AVIS\Macros\Mkdefs\cleans.db D:\NTWorkstation\data /s/i
REM xcopy D:\%1\src\Avis100\Other\DefBuild        D:\NTWorkstation\DefBuild /s/i
copy D:\%1\src\Avis100\tools\mtoolswin32\exec\mtools.exe D:\NTWorkstation\bin\mcopy.exe
copy D:\%1\src\Avis100\tools\mtoolswin32\exec\mtools.exe D:\NTWorkstation\bin\mdir.exe
copy D:\%1\src\Avis100\tools\mtoolswin32\exec\mtools.exe D:\NTWorkstation\bin\mdel.exe
xcopy D:\%1\src\avis100\Vanalysis\codow\codow.exe        D:\NTWorkstation\bin /s/i
xcopy D:\%1\src\avis100\Vanalysis\pamemulate\build.set0  D:\NTWorkstation\bin /s/i
xcopy D:\%1\src\avis100\Vanalysis\pamemulate\master.dat  D:\NTWorkstation\bin /s/i
xcopy D:\%1\src\avis100\Vanalysis\testbinrepair\writeheader.exe  D:\NTWorkstation\bin /s/i
xcopy D:\%1\src\avis100\Vanalysis\testbinrepair\truncate.exe  D:\NTWorkstation\bin /s/i
xcopy D:\%1\src\avis100\Autosig3\dbscore.exe  D:\NTWorkstation\bin /s/i

echo Copying to AVBLDSRV Stage

xcopy D:\NTWorkstation\*.* r:\%1\BlueICE_Install\NTWorkstation /s/i/q

rem AIXWorkstation Staging:

del /s/f/q D:\AIXWorkstation\*.*

xcopy D:\%1\src\Avis100\Scripts\Analyzer\mrep.pl             D:\AIXWorkstation /s/i
xcopy D:\%1\src\Avis100\Scripts\Configuration\ntshuse.pl     D:\AIXWorkstation /s/i
xcopy D:\%1\src\Avis100\pm\DataFlow\DataFlow.pm              D:\AIXWorkstation /s/i
xcopy D:\%1\src\Avis100\pm\Jockey\Jockey.pm                  D:\AIXWorkstation /s/i
xcopy D:\%1\src\Avis100\pm\Jockey\mop.pl                     D:\AIXWorkstation /s/i
xcopy D:\%1\src\Avis100\pm\AVIS\Emulator                     D:\AIXWorkstation\Emulator /s/i
xcopy D:\%1\src\Avis100\pm\AVIS\FSTools\FSTools.pm           D:\AIXWorkstation /s/i
xcopy D:\%1\src\Avis100\pm\AVIS\Local\Local.pm               D:\AIXWorkstation /s/i
xcopy D:\%1\src\Avis100\pm\AVIS\Log\Log.pm                   D:\AIXWorkstation /s/i
xcopy D:\%1\src\Avis100\pm\AVIS\Logger\Logger.pm             D:\AIXWorkstation /s/i
xcopy D:\%1\src\Avis100\pm\AVIS\Macros\Macrodb.pm            D:\AIXWorkstation /s/i
xcopy D:\%1\src\Avis100\pm\AVIS\Macros\Mutils.pm             D:\AIXWorkstation /s/i
xcopy D:\%1\src\Avis100\pm\AVIS\Macros\AIX\*.pm              D:\AIXWorkstation /s/i
xcopy D:\%1\src\Avis100\pm\AVIS\Macros\Macrocrc\Macrocrc.pm  D:\AIXWorkstation /s/i
xcopy D:\%1\src\Avis100\pm\AVIS\Macros\Setup\Setup.pm        D:\AIXWorkstation /s/i
xcopy D:\%1\src\Avis100\pm\AVIS\Macros\Types\Event.pm        D:\AIXWorkstation /s/i
xcopy D:\%1\src\Avis100\pm\AVIS\Macros\Types\Goat.pm         D:\AIXWorkstation /s/i
xcopy D:\%1\src\Avis100\pm\AVIS\Macros\Types\List.pm         D:\AIXWorkstation /s/i
xcopy D:\is\progs\stage\bin\killapp.exe                      D:\AIXWorkstation /s/i
xcopy D:\is\progs\stage\bin\killpopup.exe                    D:\AIXWorkstation /s/i
xcopy D:\is\progs\stage\bin\killproc.exe                     D:\AIXWorkstation /s/i
xcopy D:\is\progs\stage\bin\rc.exe                           D:\AIXWorkstation /s/i
xcopy D:\is\progs\stage\bin\sleep.exe                        D:\AIXWorkstation /s/i
xcopy D:\is\progs\stage\bin\texec.exe                        D:\AIXWorkstation /s/i
xcopy D:\%1\src\Avis100\Macro\Files\Commands                 D:\AIXWorkstation\Commands /s/i
xcopy D:\%1\src\Avis100\Macro\Files\Config\Applications      D:\AIXWorkstation\Applications /s/i
xcopy D:\Images                                              D:\AIXWorkstation\Images /s/i
xcopy D:\%1\src\Avis100\Other\Msdev50\*.*                    D:\AIXWorkstation /s/i
xcopy D:\%1\src\Avis100\build\update*.bat                    D:\AIXWorkstation /s/i
xcopy D:\is\wrkfiles\Buildlogs\*.*                           R:\%1\buildlog /s/i
del D:\is\wrkfiles\Buildlogs\*.* /q
del D:\AIXWorkstation\Blank.hdf /s
del D:\AIXWorkstation\Japanese1.hdf /s

xcopy D:\AIXWorkstation\*.* R:\%1\BlueICE_Install\AIXWorkstation /s/i/q

zip -r -1 -q -o R:\%1\BlueICE_Install\NTWorkstation R:\%1\BlueICE_Install\NTWorkstation
zip -r -1 -q -o R:\%1\BlueICE_Install\AIXWorkstation R:\%1\BlueICE_Install\AIXWorkstation

REM :loop

REM if exist R:\%1\rlog\ea\001\w32_done.txt goto package
REM goto loop

REM :package

copy D:\%1\src\Avis100\autosig3\dbscore.exe   R:\%1\rout\ea\nt\dbscore.exe
copy D:\%1\src\Avis100\autosig3\bldindex.exe  R:\%1\rout\ea\nt\bldindex.exe
copy D:\%1\src\Avis100\autosig3\combindx.exe  R:\%1\rout\ea\nt\combindx.exe
copy D:\%1\src\Avis100\autosig3\trie.exe      R:\%1\rout\ea\nt\trie.exe

REM Gateway Navajo

mkdir R:\%1\BlueICE_Install\Gateway\Avis
mkdir R:\%1\BlueICE_Install\Gateway\SampleFiles
mkdir R:\%1\BlueICE_Install\Gateway\SystemDLLs
mkdir R:\%1\BlueICE_Install\Gateway\tools
mkdir R:\%1\BlueICE_Install\Gateway\SampleFiles\ICEBERG
mkdir R:\%1\BlueICE_Install\Gateway\SampleFiles\NAVAJO
mkdir R:\%1\BlueICE_Install\Gateway\SampleFiles\PILOT
mkdir R:\%1\BlueICE_Install\Gateway\SampleFiles\GATEWAY1
mkdir R:\%1\BlueICE_Install\Gateway\SampleFiles\GATEWAY2
xcopy R:\%1\rout\ea\nt\scanlatest.exe                 R:\%1\BlueICE_Install\Gateway\Avis /s/i
REM xcopy R:\%1\rout\ea\nt\avisdbedit.exe             R:\%1\BlueICE_Install\Gateway\Avis /s/i
xcopy R:\%1\rout\ea\nt\scanblessed.exe                R:\%1\BlueICE_Install\Gateway\Avis /s/i
xcopy R:\%1\rout\ea\nt\navapi32.dll                   R:\%1\BlueICE_Install\Gateway\Avis /s/i
xcopy R:\%1\rout\ea\nt\avisservlets.dll               R:\%1\BlueICE_Install\Gateway\Avis /s/i
xcopy R:\%1\rout\ea\nt\aviscommon.dll                 R:\%1\BlueICE_Install\Gateway\Avis /s/i
xcopy R:\%1\rout\ea\nt\avisdb.dll                     R:\%1\BlueICE_Install\Gateway\Avis /s/i
xcopy R:\%1\rout\ea\nt\avisfile.dll                   R:\%1\BlueICE_Install\Gateway\Avis /s/i
xcopy R:\%1\rout\ea\nt\avisfilter.dll                 R:\%1\BlueICE_Install\Gateway\Avis /s/i
xcopy R:\%1\rout\ea\nt\avispolicy.dll                 R:\%1\BlueICE_Install\Gateway\Avis /s/i
xcopy R:\%1\rout\ea\nt\Navapi.vxd                     R:\%1\BlueICE_Install\Gateway\Avis /s/i
xcopy R:\%1\rout\ea\nt\VDBUnpacker.dll                R:\%1\BlueICE_Install\Gateway\Avis /s/i
xcopy R:\%1\rout\ea\nt\dec2.dll                       R:\%1\BlueICE_Install\Gateway\Avis /s/i
xcopy R:\%1\rout\ea\nt\dec2zip.dll                    R:\%1\BlueICE_Install\Gateway\Avis /s/i
xcopy R:\%1\rout\ea\nt\avissendmail.dll               R:\%1\BlueICE_Install\Gateway\Avis /s/i
xcopy R:\%1\rout\ea\nt\terminateavisprocess.exe       R:\%1\BlueICE_Install\Gateway\Avis /s/i
xcopy R:\%1\rout\ea\nt\avismonitor.exe                R:\%1\BlueICE_Install\Gateway\Avis /s/i
xcopy R:\%1\resdoc\ea\bin\msdev6\PSAPI.dll            R:\%1\BlueICE_Install\Gateway\Avis /s/i
xcopy R:\%1\resdoc\ea\bin\msdev5\msvcp50.dll          R:\%1\BlueICE_Install\Gateway\Avis /s/i
xcopy R:\%1\src\avisdb\scripts\*.*                    R:\%1\BlueICE_Install\Gateway\Avis /s/i
xcopy R:\%1\src\avisservlets\prune.pl                 R:\%1\BlueICE_Install\Gateway\Avis /s/i
xcopy R:\%1\src\avisservlets\PruneLib.pm              R:\%1\BlueICE_Install\Gateway\Avis /s/i
xcopy R:\%1\src\avisrecovery\avisdbexport.pl          R:\%1\BlueICE_Install\Gateway\Avis /s/i
xcopy R:\%1\src\avisrecovery\avisdbimport.pl          R:\%1\BlueICE_Install\Gateway\Avis /s/i
xcopy R:\%1\src\avisrecovery\avisgwsampleexport.pl    R:\%1\BlueICE_Install\Gateway\Avis /s/i
xcopy R:\%1\src\avisrecovery\avisrecovery.pl          R:\%1\BlueICE_Install\Gateway\Avis /s/i
xcopy R:\%1\src\avisrecovery\modifyanalysisresults.pl R:\%1\BlueICE_Install\Gateway\Avis /s/i
xcopy R:\%1\src\avisrecovery\PopulateAVISDB.pl        R:\%1\BlueICE_Install\Gateway\Avis /s/i
xcopy R:\%1\src\tools\zip.exe                         R:\%1\BlueICE_Install\Gateway\tools /s/i
xcopy R:\%1\src\tools\unzip.exe                       R:\%1\BlueICE_Install\Gateway\tools /s/i
xcopy R:\%1\src\samplefiles\*.*               	      R:\%1\BlueICE_Install\Gateway\Samplefiles /s/i
xcopy R:\%1\resdoc\ea\bin\msdev6\*.*                  R:\%1\BlueICE_Install\Gateway\SystemDLLs /s/i
xcopy D:\NTWorkstation\perl                           R:\%1\BlueICE_Install\Gateway\perl /s/i/q
del R:\%1\BlueICE_Install\Gateway\Avis\*.mak
rmdir /s /q R:\%1\BlueICE_Install\Gateway\Samplefiles\Rainier

zip -r -1 -q -o R:\%1\BlueICE_Install\Gateway R:\%1\BlueICE_Install\Gateway

REM GatewayDebugBuild

mkdir R:\%1\BlueICE_Install\GatewayDebugBld\Avis
mkdir R:\%1\BlueICE_Install\GatewayDebugBld\SampleFiles
mkdir R:\%1\BlueICE_Install\GatewayDebugBld\SystemDLLs
mkdir R:\%1\BlueICE_Install\GatewayDebugBld\tools
mkdir R:\%1\BlueICE_Install\GatewayDebugBld\SampleFiles\ICEBERG
mkdir R:\%1\BlueICE_Install\GatewayDebugBld\SampleFiles\NAVAJO
mkdir R:\%1\BlueICE_Install\GatewayDebugBld\SampleFiles\PILOT
mkdir R:\%1\BlueICE_Install\GatewayDebugBld\SampleFiles\GATEWAY1
mkdir R:\%1\BlueICE_Install\GatewayDebugBld\SampleFiles\GATEWAY2
xcopy R:\%1\dout\ea\nt\scanlatest.exe                 R:\%1\BlueICE_Install\GatewayDebugBld\Avis /s/i
xcopy R:\%1\dout\ea\nt\scanblessed.exe                R:\%1\BlueICE_Install\GatewayDebugBld\Avis /s/i
xcopy R:\%1\dout\ea\nt\avisservlets.dll               R:\%1\BlueICE_Install\GatewayDebugBld\Avis /s/i
xcopy R:\%1\dout\ea\nt\aviscommon.dll                 R:\%1\BlueICE_Install\GatewayDebugBld\Avis /s/i
xcopy R:\%1\dout\ea\nt\avisdb.dll                     R:\%1\BlueICE_Install\GatewayDebugBld\Avis /s/i
xcopy R:\%1\dout\ea\nt\avisfile.dll                   R:\%1\BlueICE_Install\GatewayDebugBld\Avis /s/i
xcopy R:\%1\dout\ea\nt\avisfilter.dll                 R:\%1\BlueICE_Install\GatewayDebugBld\Avis /s/i
xcopy R:\%1\dout\ea\nt\avispolicy.dll                 R:\%1\BlueICE_Install\GatewayDebugBld\Avis /s/i
xcopy R:\%1\dout\ea\nt\VDBUnpacker.dll                R:\%1\BlueICE_Install\GatewayDebugBld\Avis /s/i
xcopy R:\%1\rout\ea\nt\dec2.dll                       R:\%1\BlueICE_Install\GatewayDebugBld\Avis /s/i
xcopy R:\%1\rout\ea\nt\dec2zip.dll                    R:\%1\BlueICE_Install\GatewayDebugBld\Avis /s/i
xcopy R:\%1\dout\ea\nt\Navapi.vxd                     R:\%1\BlueICE_Install\GatewayDebugBld\Avis /s/i
xcopy R:\%1\dout\ea\nt\navapi32.dll                   R:\%1\BlueICE_Install\GatewayDebugBld\Avis /s/i
xcopy R:\%1\dout\ea\nt\avissendmail.dll               R:\%1\BlueICE_Install\GatewayDebugBld\Avis /s/i
xcopy R:\%1\dout\ea\nt\terminateavisprocess.exe       R:\%1\BlueICE_Install\GatewayDebugBld\Avis /s/i
xcopy R:\%1\dout\ea\nt\avismonitor.exe                R:\%1\BlueICE_Install\GatewayDebugBld\Avis /s/i
xcopy R:\%1\resdoc\ea\bin\msdev6\PSAPI.dll            R:\%1\BlueICE_Install\GatewayDebugBld\Avis /s/i
xcopy R:\%1\dobj\ea\AVISServlets\avisservlets.pdb     R:\%1\BlueICE_Install\GatewayDebugBld\Avis /s/i
xcopy R:\%1\dout\ea\nt\aviscommon.pdb                 R:\%1\BlueICE_Install\GatewayDebugBld\Avis /s/i
xcopy R:\%1\dout\ea\nt\avisdb.pdb                     R:\%1\BlueICE_Install\GatewayDebugBld\Avis /s/i
xcopy R:\%1\dout\ea\nt\avisfile.pdb                   R:\%1\BlueICE_Install\GatewayDebugBld\Avis /s/i
xcopy R:\%1\dout\ea\nt\avisfilter.pdb                 R:\%1\BlueICE_Install\GatewayDebugBld\Avis /s/i
xcopy R:\%1\dout\ea\nt\scanlatest.pdb                 R:\%1\BlueICE_Install\GatewayDebugBld\Avis /s/i
xcopy R:\%1\dout\ea\nt\scanblessed.pdb                R:\%1\BlueICE_Install\GatewayDebugBld\Avis /s/i
xcopy R:\%1\dout\ea\nt\VDBUnpacker.pdb                R:\%1\BlueICE_Install\GatewayDebugBld\Avis /s/i
xcopy R:\%1\dout\ea\nt\avismonitor.pdb                R:\%1\BlueICE_Install\GatewayDebugBld\Avis /s/i
xcopy R:\%1\dout\ea\nt\terminateavisprocess.pdb       R:\%1\BlueICE_Install\GatewayDebugBld\Avis /s/i
xcopy R:\%1\resdoc\ea\bin\msdev5\msvcp50.dll          R:\%1\BlueICE_Install\GatewayDebugBld\Avis /s/i
xcopy R:\%1\src\avisdb\scripts\*.*                    R:\%1\BlueICE_Install\GatewayDebugBld\Avis /s/i
xcopy R:\%1\src\avisservlets\prune.pl                 R:\%1\BlueICE_Install\GatewayDebugBld\Avis /s/i
xcopy R:\%1\src\avisservlets\PruneLib.pm              R:\%1\BlueICE_Install\GatewayDebugBld\Avis /s/i
xcopy R:\%1\src\avisrecovery\avisdbexport.pl          R:\%1\BlueICE_Install\GatewayDebugBld\Avis /s/i
xcopy R:\%1\src\avisrecovery\avisdbimport.pl          R:\%1\BlueICE_Install\GatewayDebugBld\Avis /s/i
xcopy R:\%1\src\avisrecovery\avisgwsampleexport.pl    R:\%1\BlueICE_Install\GatewayDebugBld\Avis /s/i
xcopy R:\%1\src\avisrecovery\avisrecovery.pl          R:\%1\BlueICE_Install\GatewayDebugBld\Avis /s/i
xcopy R:\%1\src\avisrecovery\modifyanalysisresults.pl R:\%1\BlueICE_Install\GatewayDebugBld\Avis /s/i
xcopy R:\%1\src\avisrecovery\PopulateAVISDB.pl        R:\%1\BlueICE_Install\GatewayDebugBld\Avis /s/i
xcopy R:\%1\src\tools\zip.exe                         R:\%1\BlueICE_Install\GatewayDebugBld\tools /s/i
xcopy R:\%1\src\tools\unzip.exe                       R:\%1\BlueICE_Install\GatewayDebugBld\tools /s/i
xcopy R:\%1\src\samplefiles\*.*               	      R:\%1\BlueICE_Install\GatewayDebugBld\Samplefiles /s/i
xcopy R:\%1\resdoc\ea\bin\msdev6\*.*                  R:\%1\BlueICE_Install\GatewayDebugBld\SystemDLLs /s/i
xcopy D:\NTWorkstation\perl                           R:\%1\BlueICE_Install\GatewayDebugBld\perl /s/i/q
del R:\%1\BlueICE_Install\GatewayDebugBld\Avis\*.mak
rmdir /s /q R:\%1\BlueICE_Install\GatewayDebugBld\Samplefiles\Rainier

zip -r -1 -q -o R:\%1\BlueICE_Install\GatewayDebugBld R:\%1\BlueICE_Install\GatewayDebugBld

REM Rushmore

mkdir R:\%1\BlueICE_Install\Rushmore\Avis
xcopy R:\%1\src\avisdb\scripts\*.*                    R:\%1\BlueICE_Install\Rushmore\Avis /s/i
xcopy D:\NTWorkstation\perl                           R:\%1\BlueICE_Install\Rushmore\perl /s/i/q
del R:\%1\BlueICE_Install\Rushmore\Avis\*.mak

zip -r -1 -q -o R:\%1\BlueICE_Install\Rushmore R:\%1\BlueICE_Install\Rushmore

REM Rainier

mkdir R:\%1\BlueICE_Install\Rainier\Avis
mkdir R:\%1\BlueICE_Install\Rainier\bin
mkdir R:\%1\BlueICE_Install\Rainier\tools
mkdir R:\%1\BlueICE_Install\Rainier\SampleFiles
xcopy R:\%1\src\Avisbackagents\deferrer.pl             R:\%1\BlueICE_Install\Rainier\bin /s/i
xcopy R:\%1\src\Avisbackagents\getdefinitions.pl       R:\%1\BlueICE_Install\Rainier\bin /s/i
xcopy R:\%1\src\Avisbackagents\undeferrer.pl           R:\%1\BlueICE_Install\Rainier\bin /s/i
xcopy R:\%1\src\Avisbackagents\archivesample.pl        R:\%1\BlueICE_Install\Rainier\bin /s/i
xcopy R:\%1\src\Avisbackagents\statusupdater.pl        R:\%1\BlueICE_Install\Rainier\bin /s/i
xcopy R:\%1\src\Avisbackagents\serializebuild.pl       R:\%1\BlueICE_Install\Rainier\bin /s/i
xcopy R:\%1\src\Avisbackagents\builddefinitions.pl     R:\%1\BlueICE_Install\Rainier\bin /s/i
xcopy R:\%1\src\Avisbackagents\unserializebuild.pl     R:\%1\BlueICE_Install\Rainier\bin /s/i
xcopy R:\%1\src\Avisbackagents\getsampleattributes.pl  R:\%1\BlueICE_Install\Rainier\bin /s/i
xcopy R:\%1\src\dataflow\jedislider\jedislider.pl      R:\%1\BlueICE_Install\Rainier\bin /s/i
xcopy R:\%1\src\dataflow\jedislider\jedislider.cfg     R:\%1\BlueICE_Install\Rainier\bin /s/i
xcopy R:\%1\src\avisbackagents\sarcexit.pl             R:\%1\BlueICE_Install\Rainier\bin /s/i
xcopy R:\%1\src\tools\zip.exe                          R:\%1\BlueICE_Install\Rainier\tools /s/i
xcopy R:\%1\src\tools\unzip.exe                        R:\%1\BlueICE_Install\Rainier\tools /s/i
xcopy R:\%1\src\avisrecovery\avisdbexport.pl           R:\%1\BlueICE_Install\Rainier\Avis /s/i
xcopy R:\%1\src\avisrecovery\avisdbimport.pl           R:\%1\BlueICE_Install\Rainier\Avis /s/i
xcopy R:\%1\src\avisrecovery\avisacsampleexport.pl     R:\%1\BlueICE_Install\Rainier\Avis /s/i
xcopy R:\%1\src\avisrecovery\avisrecovery.pl           R:\%1\BlueICE_Install\Rainier\Avis /s/i
xcopy R:\%1\src\avisrecovery\modifyanalysisresults.pl  R:\%1\BlueICE_Install\Rainier\Avis /s/i
xcopy R:\%1\src\avisrecovery\PopulateAVISDB.pl         R:\%1\BlueICE_Install\Rainier\Avis /s/i
xcopy R:\%1\rout\ea\nt\scanexplicit.exe                R:\%1\BlueICE_Install\Rainier\Avis /s/i
xcopy R:\%1\rout\ea\nt\scanlatest.exe                  R:\%1\BlueICE_Install\Rainier\Avis /s/i
xcopy R:\%1\rout\ea\nt\scanblessed.exe                 R:\%1\BlueICE_Install\Rainier\Avis /s/i
xcopy R:\%1\rout\ea\nt\navapi32.dll                    R:\%1\BlueICE_Install\Rainier\Avis /s/i
xcopy R:\%1\rout\ea\nt\avisdf.exe                      R:\%1\BlueICE_Install\Rainier\Avis /s/i
xcopy R:\%1\rout\ea\nt\avisdfrl.dll                    R:\%1\BlueICE_Install\Rainier\Avis /s/i
xcopy R:\%1\rout\ea\nt\avisdfscan.exe                  R:\%1\BlueICE_Install\Rainier\Avis /s/i
xcopy R:\%1\rout\ea\nt\aviscommon.dll                  R:\%1\BlueICE_Install\Rainier\Avis /s/i
xcopy R:\%1\rout\ea\nt\avisdb.dll                      R:\%1\BlueICE_Install\Rainier\Avis /s/i
xcopy R:\%1\rout\ea\nt\avisfile.dll                    R:\%1\BlueICE_Install\Rainier\Avis /s/i
xcopy R:\%1\rout\ea\nt\avisfilter.dll                  R:\%1\BlueICE_Install\Rainier\Avis /s/i
xcopy R:\%1\rout\ea\nt\avisacsignaturecomm.exe         R:\%1\BlueICE_Install\Rainier\Avis /s/i
xcopy R:\%1\rout\ea\nt\avisacsamplecomm.exe            R:\%1\BlueICE_Install\Rainier\Avis /s/i
xcopy R:\%1\rout\ea\nt\avistransactions.dll            R:\%1\BlueICE_Install\Rainier\Avis /s/i
xcopy R:\%1\rout\ea\nt\avissendmail.dll                R:\%1\BlueICE_Install\Rainier\Avis /s/i
xcopy R:\%1\rout\ea\nt\avismonitor.exe                 R:\%1\BlueICE_Install\Rainier\Avis /s/i
REM xcopy R:\%1\rout\ea\nt\avisdbedit.exe              R:\%1\BlueICE_Install\Rainier\Avis /s/i
xcopy R:\%1\rout\ea\nt\signaturedbutil.exe             R:\%1\BlueICE_Install\Rainier\Avis /s/i
xcopy R:\%1\rout\ea\nt\avisdfev.dat                    R:\%1\BlueICE_Install\Rainier\Avis /s/i
REM xcopy R:\%1\rout\ea\nt\Logviewer.exe               R:\%1\BlueICE_Install\Rainier\Avis /s/i
xcopy R:\%1\resdoc\ea\bin\msdev6\PSAPI.dll             R:\%1\BlueICE_Install\Rainier\Avis /s/i
xcopy R:\%1\src\samplefiles\Rainier\*.*		       R:\%1\BlueICE_Install\Rainier\SampleFiles /s/i
xcopy D:\NTWorkstation\perl                            R:\%1\BlueICE_Install\Rainier\perl /s/i/q
xcopy R:\%1\src\avisbackagents\BOSupport.pm            R:\%1\BlueICE_Install\Rainier\perl\lib\site /s/i
xcopy R:\%1\src\avisbackagents\sdgs.pm                 R:\%1\BlueICE_Install\Rainier\perl\lib\site /s/i
xcopy R:\%1\src\avisbackagents\ssss.pm                 R:\%1\BlueICE_Install\Rainier\perl\lib\site /s/i
xcopy R:\%1\src\avisbackagents\ssds.pm                 R:\%1\BlueICE_Install\Rainier\perl\lib\site /s/i


zip -r -1 -q -o R:\%1\BlueICE_Install\Rainier R:\%1\BlueICE_Install\Rainier

REM IcePack

mkdir R:\%1\BlueICE_Install\IcePack
REM mkdir R:\%1\BlueICE_Install\Ddds
REM mkdir R:\%1\BlueICE_Install\IcePack\SampleFiles
REM mkdir R:\%1\BlueICE_Install\Ddds\SampleFiles
xcopy R:\%1\rout\ea\nt\icepack.exe                     R:\%1\BlueICE_Install\IcePack /s/i
xcopy R:\%1\rout\ea\nt\scanexplicit.exe                R:\%1\BlueICE_Install\IcePack /s/i
xcopy R:\%1\rout\ea\nt\aviscommon.dll                  R:\%1\BlueICE_Install\IcePack /s/i
xcopy R:\%1\rout\ea\nt\avisfile.dll                    R:\%1\BlueICE_Install\IcePack /s/i
REM xcopy R:\%1\rout\ea\nt\ddds.dll                        R:\%1\BlueICE_Install\IcePack /s/i
REM xcopy R:\%1\rout\ea\nt\fatalbertunpack.dll             R:\%1\BlueICE_Install\IcePack /s/i
xcopy R:\%1\rout\ea\nt\icepackagentcomm.dll            R:\%1\BlueICE_Install\IcePack /s/i
xcopy R:\%1\rout\ea\nt\navapi32.dll                    R:\%1\BlueICE_Install\IcePack /s/i
xcopy R:\%1\rout\ea\nt\stpdc32i.dll                    R:\%1\BlueICE_Install\IcePack /s/i
xcopy R:\%1\rout\ea\nt\nsctopps.dll                    R:\%1\BlueICE_Install\IcePack /s/i
xcopy R:\%1\rout\ea\nt\IcePackEventMsgs.dll            R:\%1\BlueICE_Install\IcePack /s/i
REM xcopy R:\%1\rout\ea\nt\TserverTest.exe                 R:\%1\BlueICE_Install\IcePack /s/i
xcopy R:\%1\rout\ea\nt\unzip32.dll                     R:\%1\BlueICE_Install\IcePack /s/i
xcopy R:\%1\rout\ea\nt\VDBUnpacker.dll                 R:\%1\BlueICE_Install\IcePack /s/i
xcopy R:\%1\rout\ea\nt\dec2.dll                        R:\%1\BlueICE_Install\IcePack /s/i
xcopy R:\%1\rout\ea\nt\dec2zip.dll                     R:\%1\BlueICE_Install\IcePack /s/i
xcopy R:\%1\resdoc\ea\bin\msdev5\msvcp50.dll           R:\%1\BlueICE_Install\IcePack /s/i
xcopy R:\%1\src\IcePack\IcePack.reg                    R:\%1\BlueICE_Install\IcePack /s/i
REM xcopy R:\%1\src\avisdispdist\pdds.prf                  R:\%1\BlueICE_Install\IcePack\Samplefiles /s/i

zip -r -1 -q -o R:\%1\BlueICE_Install\IcePack R:\%1\BlueICE_Install\IcePack

REM LoadGate

mkdir R:\%1\BlueICE_Install\LoadGate
xcopy R:\%1\rout\ea\nt\loadgate.exe                     R:\%1\BlueICE_Install\LoadGate /s/i
xcopy R:\%1\rout\ea\nt\libeay32.dll                     R:\%1\BlueICE_Install\LoadGate /s/i
xcopy R:\%1\rout\ea\nt\ssleay32.dll                     R:\%1\BlueICE_Install\LoadGate /s/i

zip -r -1 -q -o R:\%1\BlueICE_Install\LoadGate R:\%1\BlueICE_Install\LoadGate

REM Docs

mkdir R:\%1\BlueICE_Install\Docs
xcopy R:\%1\src\docs\*.*                     		R:\%1\BlueICE_Install\Docs /s/i
zip -r -1 -q -o R:\%1\BlueICE_Install\Docs R:\%1\BlueICE_Install\Docs

REM testtools

mkdir R:\%1\BlueICE_Install\Testtools
xcopy R:\%1\src\testtools\*.*                     	R:\%1\BlueICE_Install\Testtools /s/i
zip -r -1 -q -o R:\%1\BlueICE_Install\Testtools R:\%1\BlueICE_Install\Testtools

REM Ddds

REM xcopy R:\%1\rout\ea\nt\ddds.exe                        R:\%1\BlueICE_Install\Ddds /s/i
REM xcopy R:\%1\rout\ea\nt\aviscommon.dll                  R:\%1\BlueICE_Install\Ddds /s/i
REM xcopy R:\%1\rout\ea\nt\navapi32.dll                    R:\%1\BlueICE_Install\Ddds /s/i
REM xcopy R:\%1\rout\ea\nt\navapi.vxd                      R:\%1\BlueICE_Install\Ddds /s/i
REM xcopy R:\%1\src\avisdispdist\pdds.prf                  R:\%1\BlueICE_Install\Ddds\Samplefiles /s/i

REM zip -r -1 -q -o R:\%1\BlueICE_Install\Ddds R:\%1\BlueICE_Install\Ddds

rem the end
