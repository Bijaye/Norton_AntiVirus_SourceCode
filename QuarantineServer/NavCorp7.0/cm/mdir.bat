@echo off
if exist %1 goto Done
echo Creating directory %1
md %1
:Done
