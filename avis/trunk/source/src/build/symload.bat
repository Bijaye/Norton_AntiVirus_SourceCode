set SOFTICE="c:\program files\numega\softicent\nmsym"
cd %OUTD%
rem ----------------------------
rem start loading files
rem ----------------------------
rem AVISCOMMON DLL
%SOFTICE% /load AVIScommon.dll
rem AVISDB DLL
%SOFTICE% /load AVISdb.nms
rem AVISFile DLL
%SOFTICE% /load AVISFile.nms
rem AVISFilter DLL
%SOFTICE% /load AVISFilter.nms
rem AVISPolicy DLL
%SOFTICE% /load AVISPolicy.nms
rem AVISServlets DLL
%SOFTICE% /load AVISServlets.nms


