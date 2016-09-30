/////////////////////////////////////////////////////////////////////
// File: Migrate.h
// Copyright 1992-1997 Symantec, Peter Norton Product Group
/////////////////////////////////////////////////////////////////////
//
// $Header:   S:/ATMIGRAT/VCS/migrate.h_v   1.1   19 Nov 1998 13:14:28   MBROWN  $
//
//
/////////////////////////////////////////////////////////////////////
//
// $Log:   S:/ATMIGRAT/VCS/migrate.h_v  $
// 
//    Rev 1.1   19 Nov 1998 13:14:28   MBROWN
// Changed method for getting shell folders, deleting NPS links, added entry point for unloading NAV
// 
//    Rev 1.0   26 May 1998 14:36:14   SEDWARD
// Initial revision.
//
//
/////////////////////////////////////////////////////////////////////


#ifndef MIGRATE_HEADER
#define MIGRATE_HEADER

#define _MBCS
// #define SHOW_MESSAGE_BOXES
#ifdef __cplusplus
extern "C" {
#endif

// This defines a ptr to a function that must be created for future versions
// of NAV that may require new methods for unloading AP. The function will
// need to be called NAVMigCfg and reside in a dll called navmig.dll located
// in the windows or system directory.
typedef BOOL __declspec(dllexport) (WINAPI *LPFNNAVMigCfg) (LPCSTR InputString);

#ifdef __cplusplus
}
#endif

// function prototypes
LONG
CALLBACK
QueryVersion (
    OUT     LPCSTR *ProductID,
	OUT     LPUINT DllVersion,
	OUT     LPINT *CodePageArray,	    OPTIONAL
	OUT     LPCSTR *ExeNamesBuf,	    OPTIONAL
	        LPVOID Reserved
    );

LONG
CALLBACK
Initialize9x (
	IN      LPCSTR WorkingDirectory,
	IN      LPCSTR SourceDirectories,
	        LPVOID Reserved
    );

LONG
CALLBACK
MigrateUser9x (
	IN      HWND ParentWnd,
	IN      LPCSTR UnattendFile,
	IN      HKEY UserRegKey,
	IN      LPCSTR UserName,
	        LPVOID Reserved
    );

LONG
CALLBACK
MigrateSystem9x (
	IN      HWND ParentWnd,
	IN      LPCSTR UnattendFile,
	        LPVOID Reserved
    );

LONG
CALLBACK
InitializeNT (
	IN      LPCWSTR WorkingDirectory,
	IN      LPCWSTR SourceDirectories,
	        LPVOID Reserved
    );

LONG
CALLBACK
MigrateUserNT (
	IN      HINF UnattendInfHandle,
	IN      HKEY UserRegKey,
	IN      LPCWSTR UserName,
            LPVOID Reserved
    );

LONG
CALLBACK
MigrateSystemNT (
	IN      HINF UnattendInfHandle,
            LPVOID Reserved
    );

BOOL
CALLBACK
MigrateNAV9x(
    IN      LPCSTR  InputString
    );

#endif  // MIGRATE_HEADER