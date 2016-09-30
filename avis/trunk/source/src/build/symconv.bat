set SOFTICE="c:\program files\numega\softicent\nmsym"
cd %OUTD%
del *.nms
rem ----------------------------
rem start translating files
rem ----------------------------
rem AVISCOMMON DLL
%SOFTICE% /translate AVIScommon.dll
rem AVISDB DLL
%SOFTICE% /translate AVISdb.dll
rem AVISFile DLL
%SOFTICE% /translate AVISFile.dll
rem AVISFilter DLL
%SOFTICE% /translate AVISFilter.dll
rem AVISPolicy DLL
%SOFTICE% /translate AVISPolicy.dll
rem AVISServlets DLL
%SOFTICE% /translate AVISServlets.dll


