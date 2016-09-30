/////////////////////////////////////////////////////////////////////////////
//  Copyright (c)1997 - Symantec, Peter Norton Computing Group
/////////////////////////////////////////////////////////////////////////////
//
//  $Header:   S:/NAVINSTNT/VCS/ISUTIL.h_v   1.1   18 Jul 1997 18:59:30   JALLEE  $
//
/////////////////////////////////////////////////////////////////////////////
//  $Log:   S:/NAVINSTNT/VCS/ISUTIL.h_v  $
// 
//    Rev 1.1   18 Jul 1997 18:59:30   JALLEE
// Ported Bruce H's update to the registration and usage count utilities from the
// BORG project.  Many changes, but should facilitate sharing bug fixes / updates.
//
//    Rev 1.0   05 May 1997 19:26:02   sedward
// Initial revision.
//
/////////////////////////////////////////////////////////////////////////////


#ifndef IS_UTIL_HEADER
#define IS_UTIL_HEADER

/////////////////////////////////////////////////////////////////////////////
//  Misc defines
/////////////////////////////////////////////////////////////////////////////

#define IS_FILE_DELETED         0x00000000
#define IS_FILE_NOT_FOUND       0x00000001
#define IS_FILE_IN_USE          0x00000003

#define IS_DIR_REMOVED          IS_FILE_DELETED
#define IS_DIR_NOT_FOUND        IS_FILE_NOT_FOUND
#define IS_DIR_NOT_EMPTY        IS_FILE_IN_USE


/////////////////////////////////////////////////////////////////////////////
//  Function prototypes from ISUtil.CPP
/////////////////////////////////////////////////////////////////////////////

BOOL WINAPI FileExists( LPTSTR lpFile );

BOOL WINAPI DirectoryExists( LPTSTR lpPath );

BOOL WINAPI ProcessUsageCountsFile( LPTSTR lpLogSect, LPTSTR lpLogFile );

BOOL WINAPI GetBaseDir( LPTSTR lpBuffer, UINT uBufSize );

LPTSTR WINAPI LogFileFindSection( FILE *fpFile, LPTSTR lpSection );

LPSTR WINAPI LogFileGetNextLine( FILE *fpFile );

DWORD WINAPI ISDeleteFile( LPTSTR lpFile );

DWORD WINAPI ISRemoveDirectory( LPTSTR lpDir );

BOOL WINAPI LaunchAndWait( LPSTR lpExe, LPSTR lpParams );

BOOL WINAPI FileInUseHandler( LPTSTR lpName );

VOID CDECL LTrace( LPCSTR lpszFmt, ... );

#ifdef __cplusplus
extern "C"
{
#endif


    BOOL WINAPI DecrementUsageCount( LPTSTR lpFile, LPTSTR lpPath,
                                     LPTSTR lpId );

    BOOL WINAPI DeleteDirectoryContents( LPTSTR lpDirectory );


#ifdef __cplusplus
}
#endif








#endif  // IS_UTIL_HEADER
