/////////////////////////////////////////////////////////////////////
// File: Global.h
// Copyright 1992-1997 Symantec, Peter Norton Product Group
/////////////////////////////////////////////////////////////////////
//
// $Header:   S:/ATMIGRAT/VCS/global.h_v   1.1   01 Feb 1999 18:27:08   MBROWN  $
//
//
/////////////////////////////////////////////////////////////////////
//
// $Log:   S:/ATMIGRAT/VCS/global.h_v  $
// 
//    Rev 1.1   01 Feb 1999 18:27:08   MBROWN
// Added VENDORINFO stuff
// 
//    Rev 1.0   26 May 1998 14:36:16   SEDWARD
// Initial revision.
//
/////////////////////////////////////////////////////////////////////



#ifndef MIGRATE_GLOBAL_HEADER
#define MIGRATE_GLOBAL_HEADER


#include    "Migrant.h"



// define statements
#define PRODUCT_ID_BUFLEN                   50

// typdefs
typedef struct 
{      
    CHAR   CompanyName[256];      
    CHAR   SupportNumber[256];
    CHAR   SupportUrl[256];      
    CHAR   InstructionsToUser[1024];
} VENDORINFO, *PVENDORINFO;

// constants
extern const CHAR* DEFAULT_USER;
extern const CHAR* S_MIGINF_MASK;


// global variables
extern CMigrant*       g_migrantWorker;
extern VENDORINFO      g_vendorInfo;


#ifdef  __cplusplus
extern "C"  {
#endif

extern BOOL            g_bIsRoswellInstallation;
extern BOOL            g_FoundPassword;
extern CHAR            g_ExeNamesBuf[];
extern CHAR            g_UserNameBuf[];
extern CHAR            g_NavInstallDir[];
extern CHAR            g_SharedComponentsDir[];
extern CHAR            g_SymantecSharedDir[];
extern HANDLE          g_hHeap;
extern HINSTANCE       g_hInst;
extern INT             g_CodePageArray[];
extern LPCSTR          g_User;
extern LPSTR           g_WorkingDirectory;
extern LPSTR           g_SourceDirectories;         // multi-sz
extern LPSTR           g_SettingsFile;
extern LPSTR           g_MigrateDotInf;
extern TCHAR           g_szProductID[];

#ifdef  __cplusplus
    }
#endif

#endif  //  MIGRATE_GLOBAL_HEADER