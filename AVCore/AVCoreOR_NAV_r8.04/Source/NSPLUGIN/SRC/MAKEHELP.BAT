::************************************************************************
:: Copyright 1997 Symantec Corporation
:: _______________________________________________________________________
::
:: $Header:   S:/NSPLUGIN/VCS/MAKEHELP.BAv   1.0   21 May 1997 16:31:14   MDUNN  $
::
:: Description: Batch file that builds the NPLUGIN help file.
::
::************************************************************************
::  $Log:   S:/NSPLUGIN/VCS/MAKEHELP.BAv  $
:  
:     Rev 1.0   21 May 1997 16:31:14   MDUNN
:  Initial revision.
::************************************************************************

@echo off
REM -- First make map file from Microsoft Visual C++ generated resource.h
echo // MAKEHELP.BAT generated Help Map file.  Used by NPLUGIN.HPJ. >"NPlugIn.hm"
echo. >>"NPlugIn.hm"
echo // Commands (ID_* and IDM_*) >>"NPlugIn.hm"
makehm ID_,HID_,0x10000 IDM_,HIDM_,0x10000 resource.h >>"NPlugIn.hm"
echo. >>"NPlugIn.hm"
echo // Prompts (IDP_*) >>"NPlugIn.hm"
makehm IDP_,HIDP_,0x30000 resource.h >>"NPlugIn.hm"
echo. >>"NPlugIn.hm"
echo // Resources (IDR_*) >>"NPlugIn.hm"
makehm IDR_,HIDR_,0x20000 resource.h >>"NPlugIn.hm"
echo. >>"NPlugIn.hm"
echo // Dialogs (IDD_*) >>"NPlugIn.hm"
makehm IDD_,HIDD_,0x20000 resource.h >>"NPlugIn.hm"

rem echo. >>"NPlugIn.hm"
rem echo // Frame Controls (IDW_*) >>"NPlugIn.hm"
rem makehm IDW_,HIDW_,0x50000 resource.h >>"NPlugIn.hm"

echo. >>"NPlugIn.hm"
echo // Dialog Controls (IDC_*) >>"NPlugIn.hm"
makehm IDC_,HIDC_,0x50000 resource.h >>"NPlugIn.hm"

REM -- Make help for Project NPLUGIN


echo Building Win32 Help files
start /wait hcrtf -x NPlugIn.hpj
echo.
if exist Debug\nul copy "NPlugIn.hlp" Debug
if exist Debug\nul copy "NPlugIn.cnt" Debug
if exist Release\nul copy "NPlugIn.hlp" Release
if exist Release\nul copy "NPlugIn.cnt" Release
if exist DebugU\nul copy "NPlugIn.hlp" DebugU
if exist DebugU\nul copy "NPlugIn.cnt" DebugU
if exist ReleaseU\nul copy "NPlugIn.hlp" ReleaseU
if exist ReleaseU\nul copy "NPlugIn.cnt" ReleaseU
echo.


