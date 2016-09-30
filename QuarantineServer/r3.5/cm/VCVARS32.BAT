echo
echo Root of Visual Developer Studio Common files.
set VSCommonDir=C:\depot\tools\MSVC6SP5.X86\COMMON

echo
echo Root of Visual Developer Studio installed files.
echo
set MSDevDir=C:\depot\tools\MSVC6SP5.X86\COMMON\msdev98

echo
echo Root of Visual C++ installed files.
echo
set MSVCDir=C:\depot\tools\MSVC6SP5.X86\VC98

echo
echo VcOsDir is used to help create either a Windows 95 or Windows NT specific path.
echo
set VcOsDir=WIN95
if "%OS%" == "Windows_NT" set VcOsDir=WINNT

echo
echo Setting environment for using Microsoft Visual C++ tools.
echo

if "%OS%" == "Windows_NT" set PATH=%MSDevDir%\BIN;%MSVCDir%\BIN;%VSCommonDir%\TOOLS\%VcOsDir%;%VSCommonDir%\TOOLS;%PATH%
if "%OS%" == "" set PATH="%MSDevDir%\BIN";"%MSVCDir%\BIN";"%VSCommonDir%\TOOLS\%VcOsDir%";"%VSCommonDir%\TOOLS";"%windir%\SYSTEM";"%PATH%"
set INCLUDE=%MSVCDir%\ATL\INCLUDE;%MSVCDir%\INCLUDE;%MSVCDir%\MFC\INCLUDE;%INCLUDE%
set LIB=%MSVCDir%\LIB;%MSVCDir%\MFC\LIB;%LIB%

set VcOsDir=
set VSCommonDir=
