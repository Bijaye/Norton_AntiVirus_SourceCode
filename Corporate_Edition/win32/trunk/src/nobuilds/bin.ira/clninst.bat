@echo off
rem *****************************************************
rem ** Batch file to clean up temp files left over from
rem ** Symantec AntiVirus Console's Client Remote.
rem *****************************************************

del /f /s /q c:\temp\clt-inst\*.*
rd /s /q c:\temp\clt-inst
