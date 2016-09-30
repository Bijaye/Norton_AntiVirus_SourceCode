/////////////////////////////////////////////////////////////////////
// File: Global.cpp
//
// Copyright 1992-1997 Symantec, Peter Norton Product Group
/////////////////////////////////////////////////////////////////////
// $Header:   S:/ATMIGRAT/VCS/global.cpv   1.1   01 Feb 1999 18:27:08   MBROWN  $
//
//
/////////////////////////////////////////////////////////////////////
//
// $Log:   S:/ATMIGRAT/VCS/global.cpv  $
// 
//    Rev 1.1   01 Feb 1999 18:27:08   MBROWN
// Added VENDORINFO stuff
// 
//    Rev 1.0   26 May 1998 14:31:20   SEDWARD
// Initial revision.
//
/////////////////////////////////////////////////////////////////////



#include <windows.h>
#include <windowsx.h>

#include <tchar.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "global.h"



/////////////////////////////////////////////////////////////////////////////
//  Global Variables
/////////////////////////////////////////////////////////////////////////////

#include    "Migrant.h"


// global constants
const CHAR* DEFAULT_USER = "Default User";
const CHAR* S_MIGINF_MASK = "%s\\migrate.inf";



// global variables
CMigrant*       g_migrantWorker;
VENDORINFO      g_vendorInfo;


#ifdef  __cplusplus
extern "C"  {
#endif

HANDLE          g_hHeap;
HINSTANCE       g_hInst;

#if 0   // IFDEF_DISABLE
LPSTR           g_WorkingDirectory = NULL;
LPSTR           g_SourceDirectories = NULL;         // multi-sz
LPSTR           g_SettingsFile = NULL;
LPSTR           g_MigrateDotInf = NULL;
TCHAR           g_ExeNamesBuf[] = {NULL};
TCHAR           g_NavInstallDir[MAX_PATH*2];
TCHAR           g_SharedComponentsDir[MAX_PATH*2];
TCHAR           g_SymantecSharedDir[MAX_PATH*2];
TCHAR           g_szProductID[PRODUCT_ID_BUFLEN];
#endif  // IFDEF_DISABLE

#ifdef  __cplusplus
    }
#endif