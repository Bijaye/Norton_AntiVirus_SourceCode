// Copyright 1997 Symantec, Peter Norton Product Group
//***************************************************************************
//
// $Header:   S:/NAVSDK/VCS/navdefs.cpv   1.6   01 Apr 1998 12:20:18   tcashin  $
//
// Description:
//
//      This file contain functions that wrap and export the methods in the
//      Hawking CDefUtil class. 
//
// Contains:
//
//      VirusDefAreNewest()
//      VirusDefClose()
//      VirusDefGetCurrent()
//      VirusDefGetCurrentDate()
//      VirusDefGetNewestDate()
//      VirusDefGetUniqueAppID()
//      VirusDefInit()
//      VirusDefInitInstallApp()
//      VirusDefInitWindowsApp()
//      VirusDefInstall()
//      VirusDefPostUpdate()
//      VirusDefPreUpdate()
//      VirusDefSetCurrent()
//      VirusDefStopUsingDefs()
//      VirusDefUninstall()
//      VirusDefUseNewest()
//
//***************************************************************************
// $Log:   S:/NAVSDK/VCS/navdefs.cpv  $
// 
//    Rev 1.6   01 Apr 1998 12:20:18   tcashin
// In VirusDefInstall, we were not passing the full pathname to 
// GetFileAttributes. This would cause us to not install the defs.
// 
//    Rev 1.5   19 Dec 1997 12:17:30   tcashin
// Handle trailing '\' in virus def dir a little better. We now handle it 
// correctly if it is there or not.
// 
//    Rev 1.4   18 Dec 1997 10:23:12   tcashin
// Get rid of the leading '_' in the AppID we generate.
// 
//    Rev 1.3   08 Dec 1997 17:36:02   tcashin
// Removed an unused parameter in VirusDefUninstall, fixed a bug in the
// file copy part of VirusDefInstall, and added checks in all functions to
// make sure the handle in not NULL.
// 
//    Rev 1.2   05 Dec 1997 18:20:18   DALLEE
// Removed SYMKRNL dependency by chaning FileFindXXX() calls
// to direct calls to Win32 FindXXX() API's.
// 
// 
//    Rev 1.1   01 Dec 1997 13:40:18   tcashin
// Added to $Log record for PVCS.
//
//***************************************************************************



//***************************************************************************
// Include Files
//***************************************************************************
#include "platform.h"
#include <stdlib.h>
#include <time.h>
#include "file.h"

#include "navdefs.h"
#include "defutils.h"



//***************************************************************************
// Defines
//***************************************************************************
#define VIRDEF_TEMP_BUFFER_SIZE     1024
#define VIRDEF_UNIQUEID_PREFIX      _T("NAVSDK")


//***************************************************************************
// Local Function Prototypes
//***************************************************************************
LPCTSTR WINAPI SkipWhiteSpace (LPCTSTR lpString);



//***************************************************************************
// Exported Functions
//***************************************************************************

/*@API:**********************************************************************
@Declaration:

    VOID DllExport WINAPI VirusDefAreNewest (HVIRUSDEF hVirusDef)

@Description:

    Determines if the virus definition files currently used by this 
    application are the newest files available on the machine.

@Parameters:

    HVIRUSDEF   - Virus Definition Library handle from VirusDefInit()
    
@Returns:

    TRUE        - If the current files are the newest
    FALSE       - If newer files are available

@See:

@Include:

    navdefs.h
    defutils.h

@Compatibility: WIN32
****************************************************************************/
BOOL DllExport WINAPI VirusDefAreNewest (
    HVIRUSDEF hVirusDef)  
{
    if (hVirusDef != NULL)
        return (((CDefUtils *)hVirusDef)->DefsAreNewest());
    else
        return FALSE;
}



/*@API:**********************************************************************
@Declaration:

    VOID DllExport WINAPI VirusDefClose (HVIRUSDEF hVirusDef)

@Description:

    Close the Virus Definitions Library by deleting this instance of the
    CDefUtils class.

@Parameters:

    HVIRUSDEF - Virus Definition Library handle from VirusDefInit()
    
@Returns:

    VOID

@See:

@Include:

    navdefs.h
    defutils.h

@Compatibility: WIN32
****************************************************************************/
VOID DllExport WINAPI VirusDefClose (
    HVIRUSDEF hVirusDef)  
{
    if (hVirusDef != NULL)
        delete (CDefUtils *)hVirusDef;
}



/*@API:**********************************************************************
@Declaration:

    BOOL  DllExport WINAPI VirusDefGetCurrent (
	      HVIRUSDEF hVirusDef,
          LPSTR     pszDir,
          int       nMaxCount)

@Description:

    Get the full path name of the directory containing the virus definition
    files currently used by this application.

@Parameters:

    HVIRUSDEF - handle of the current virus definition context
    LPSTR     - buffer to receive the path name
    int       - size of the buffer

@Returns:

    TRUE      - if the path name was successfully returned.
    FALSE     - 

@See:

@Include:

    navdefs.h
    defutils.h

@Compatibility: WIN32
****************************************************************************/
BOOL  DllExport WINAPI VirusDefGetCurrent (
	  HVIRUSDEF hVirusDef,
      LPSTR     pszDir,
      int       nMaxCount)
{
    if (hVirusDef != NULL)
        return (((CDefUtils *)hVirusDef)->GetCurrentDefs(pszDir, nMaxCount));
    else
        return FALSE;
}



/*@API:**********************************************************************
@Declaration:

    BOOL  DllExport WINAPI VirusDefGetCurrentDate (
	      HVIRUSDEF hVirusDef,
          LPWORD    pwYear,
          LPWORD    pwMonth,
          LPWORD    pwDay,
          LPDWORD   pdwRev)

@Description:

    Get the date of the virus definition files currently used by 
    this application.

    The pdwRev will normally be 0. However, if more than one def set
    is released on the same day, this number will be incremented for
    each release.

@Parameters:

    HVIRUSDEF - handle of the current virus definition context
    LPWORD    - word buffer to receive the year. Include the century.
    LPWORD    - word buffer to receive the month. Range is from 1 to 12.
    LPWORD    - word buffer to receive the day. Range is from 1 to 31.
    LPDWORD   - dword buffer to receive the version of the definitions

@Returns:

    TRUE      - if the path name was successfully returned.
    FALSE     - 

@See:

@Include:

    navdefs.h
    defutils.h

@Compatibility: WIN32
****************************************************************************/
BOOL  DllExport WINAPI VirusDefGetCurrentDate (
	  HVIRUSDEF hVirusDef,
      LPWORD    pwYear,
      LPWORD    pwMonth,
      LPWORD    pwDay,
      LPDWORD   pdwRev)
{
    if (hVirusDef != NULL)
        return (((CDefUtils *)hVirusDef)->GetCurrentDefsDate(pwYear, pwMonth, 
                                                         pwDay, pdwRev));
    else
        return FALSE;
}



/*@API:**********************************************************************
@Declaration:

    BOOL  DllExport WINAPI VirusDefGetNewestDate (
	      HVIRUSDEF hVirusDef,
          LPWORD    pwYear,
          LPWORD    pwMonth,
          LPWORD    pwDay,
          LPDWORD   pdwRev)

@Description:

    Get the date of the newest virus definition files in the shared area.

@Parameters:

    HVIRUSDEF - handle of the current virus definition context
    LPWORD    - word buffer to receive the year. Include the century.
    LPWORD    - word buffer to receive the month. Range is from 1 to 12.
    LPWORD    - word buffer to receive the day. Range is from 1 to 31.
    LPDWORD   - dword buffer to receive the version of the definitions

@Returns:

    TRUE      - if the path name was successfully returned.
    FALSE     - 

@See:

@Include:

    navdefs.h
    defutils.h

@Compatibility: WIN32
****************************************************************************/
BOOL  DllExport WINAPI VirusDefGetNewestDate (
	  HVIRUSDEF hVirusDef,
      LPWORD    pwYear,
      LPWORD    pwMonth,
      LPWORD    pwDay,
      LPDWORD   pdwRev)
{
    if (hVirusDef != NULL)
        return (((CDefUtils *)hVirusDef)->GetNewestDefsDate(pwYear, pwMonth, 
                                                         pwDay, pdwRev));
    else
        return FALSE;
}



/*@API:**********************************************************************
@Declaration:

BOOL  DllExport WINAPI VirusDefGetUniqueAppID (
	  HVIRUSDEF hVirusDef,
      LPCSTR    pszAppIDPrefix,
      LPSTR     pszAppID,
      int       nMaxCount)


@Description:

    Get a unique AppID string. If pszAppIDPrefix is not NULL then
    append the generated AppID to the prefix.

    The AppID will be NAVSDK_numbersuffix where number_suffix is a
    random number seeded with the current time().

    If a prefix is specified the AppID is of the form 

    If the length of the generated string is longer than nMaxCount
    the function returns FALSE.

@Parameters:

    HVIRUSDEF - handle of the current virus definition context
    LPCSTR    - buffer containing the prefix for the AppID
    LPSTR     - buffer to receive the generated AppID
    int       - size of the buffer to receive the AppID

@Returns:

    TRUE      - if the path name was successfully returned.
    FALSE     - 

@See:

@Include:

    navdefs.h
    defutils.h

@Compatibility: WIN32
****************************************************************************/
BOOL  DllExport WINAPI VirusDefGetUniqueAppID (
	  HVIRUSDEF hVirusDef,
      LPCSTR    pszAppIDPrefix,
      LPSTR     pszAppID,
      int       nMaxCount)
{

    BOOL    bRetVal = TRUE;
    int     iUnique;
    TCHAR   szTempAppID[VIRDEF_TEMP_BUFFER_SIZE];
    LPSTR   lpszTempAppID = &szTempAppID[0];

    //
    // Seed the random number generator with the time and get a random number.
    //

    srand((unsigned) time(NULL));
    iUnique = rand();

    //
    // Build the unique string in a big temporary buffer
    //

    if (pszAppIDPrefix == NULL || STRLEN(pszAppIDPrefix) == 0 ||
        SkipWhiteSpace(pszAppIDPrefix) == NULL )        
        SPRINTF(lpszTempAppID, "%s_%d", VIRDEF_UNIQUEID_PREFIX, iUnique);
    else
        SPRINTF(lpszTempAppID, "%s_%s_%d", SkipWhiteSpace(pszAppIDPrefix), 
                VIRDEF_UNIQUEID_PREFIX, iUnique);

    //
    // Test the length of the string
    //

    if (STRLEN(lpszTempAppID) > nMaxCount)
        bRetVal = FALSE;
    else
        STRCPY(pszAppID, lpszTempAppID);


    return (bRetVal);
}



/*@API:**********************************************************************
@Declaration:

    BOOL DllExport WINAPI VirusDefInit (LPHANDLE pVirusDefHandle)

@Description:

    Initialize the Virus Defintion Library by creating an instance of the
    CDefUtils class.

@Parameters:

    LPHANDLE - pointer to a handle used in all subsequent calls to Virus
               Definition Library functions.

@Returns:

    TRUE     - if an instance of CDefUtils was successfully created.
    FALSE    - if an instance was not created.

@See:

@Include:

    navdefs.h
    defutils.h

@Compatibility: WIN32
****************************************************************************/
BOOL DllExport WINAPI VirusDefInit (
    LPHANDLE    pVirusDefHandle)
{

    CDefUtils *pDefUtil = new CDefUtils();

    *pVirusDefHandle = (HVIRUSDEF)pDefUtil;

    if (pDefUtil != NULL)
        return (TRUE);
    else
        return (FALSE);
}



/*@API:**********************************************************************
@Declaration:

    BOOL  DllExport WINAPI VirusDefInitInstallApp (
	      HVIRUSDEF hVirusDef,
          LPCSTR    pszAppID,
          LPCSTR    pszAppInstallDir,
          LPCSTR    pszAppDefsDir)

@Description:


@Parameters:

    HVIRUSDEF - handle of the current virus definition context
    LPCSTR    - buffer containing the AppID
    LPCSTR    - used for DOS apps only.
    LPCSTR    - used during install only.

@Returns:

    TRUE      - 
    FALSE     - 

@See:

@Include:

    navdefs.h
    defutils.h

@Compatibility: WIN32
****************************************************************************/
BOOL  DllExport WINAPI VirusDefInitInstallApp (
      HVIRUSDEF hVirusDef,
      LPCSTR    pszAppID,
      LPCSTR    pszAppInstallDir,
      LPCSTR    pszAppDefsDir)
{
    if (hVirusDef != NULL)
        return (((CDefUtils *)hVirusDef)->InitInstallApp(
                        pszAppID, pszAppInstallDir, pszAppDefsDir));
    else
        return FALSE;
}



/*@API:**********************************************************************
@Declaration:

    BOOL  DllExport WINAPI VirusDefInitWindowsApp (
	      HVIRUSDEF hVirusDef,
          LPCSTR    pszAppID)

@Description:

    Initialize the Virus Defintion Library by creating an instance of the
    CDefUtils class.

@Parameters:

    HVIRUSDEF - handle of the current virus definition context
    LPCSTR    - buffer containing the AppID

@Returns:

    TRUE      - 
    FALSE     - 

@See:

@Include:

    navdefs.h
    defutils.h

@Compatibility: WIN32
****************************************************************************/
BOOL  DllExport WINAPI VirusDefInitWindowsApp (
      HVIRUSDEF hVirusDef,
      LPCSTR    pszAppID)
{
    if (hVirusDef != NULL)
        return (((CDefUtils *)hVirusDef)->InitWindowsApp(pszAppID));
    else 
        return FALSE;
}




/*@API:**********************************************************************
@Declaration:

    DWORD DllExport WINAPI VirusDefInstall (
	      HVIRUSDEF hVirusDef,
          LPSTR     pszAppID,
          int       nMaxCount,
          LPCSTR    pszDefSourceDir)

@Description:

    This is the Simplified Install Interface. This will:

       1. Generate a unique AppID 

       2. Register the current application are a virus definitions
          user using the generated AppID

       3. Install the virus definition files if they do not
          exist on the machine or if the installed file are out
          of date.

       4. Return the AppID. The calling application is responsible
          for keeping the AppID as long as it exists on the machine.

@Parameters:

    HVIRUSDEF - handle of the current virus definition context
    LPSTR     - buffer to receive the generated AppID
    int       - size of the buffer to receive the generated AppID
    LPCSTR    - directory to install definitions from

@Returns:

    VIRDEF_OK - this application was successfully registered as a 
                Virus Definition user.

    VIRDEF_?  - Various error codes indicating why install failed.

@See:

@Include:

    navdefs.h
    defutils.h

@Compatibility: WIN32
****************************************************************************/
DWORD DllExport WINAPI VirusDefInstall (
	  HVIRUSDEF hVirusDef,
      LPSTR     pszAppID,
      int       nMaxCount,
      LPCSTR    pszAppIDPrefix,
      LPCSTR    pszDefSourceDir)

{
      auto BOOL     bDefsChanged; 
      auto BOOL     bFoundAFile = TRUE;
      auto BOOL     dwError = VIRDEF_OK;
      auto TCHAR    szTargetDir[VIRDEF_TEMP_BUFFER_SIZE];
      auto TCHAR    szTempFileName[VIRDEF_TEMP_BUFFER_SIZE];
      auto TCHAR    szTempTargetFile[VIRDEF_TEMP_BUFFER_SIZE];
      auto TCHAR    szTempSrcFile[VIRDEF_TEMP_BUFFER_SIZE];

      auto HANDLE   hFind;
      auto WIN32_FIND_DATA  stFindData;

      if ( VirusDefGetUniqueAppID (NULL, pszAppIDPrefix, pszAppID, nMaxCount) == FALSE )      
          return (VIRDEF_NO_APPID_CREATED);

      if ( VirusDefInitInstallApp (hVirusDef, pszAppID, NULL, NULL) == FALSE )
          return (VIRDEF_INIT_INSTALL_ERROR);

      if ( VirusDefPreUpdate(hVirusDef, szTargetDir, nMaxCount, VirusDefInstallScript) == FALSE )
          return (VIRDEF_PREUPDATE_FAILED);

      //
      // Copy files to the directory returned in szTargetDir
      //

      STRCPY(&szTempFileName[0], pszDefSourceDir);

      if ( STRLEN(STRRCHR(&szTempFileName[0],'\\')) > 1 )
                  STRCAT(&szTempFileName[0], _T("\\"));

      STRCAT(&szTempFileName[0], _T("*.*"));

      hFind = FindFirstFile( &szTempFileName[0], &stFindData );
      
      while ( bFoundAFile == TRUE )
      {

          STRCPY(&szTempFileName[0],pszDefSourceDir);
          STRCAT(&szTempFileName[0], _T("\\"));
          STRCAT(&szTempFileName[0],&stFindData.cFileName[0]);

          // If the directory attribute is set, skip it.

          if ( (GetFileAttributes(&szTempFileName[0]) & FILE_ATTRIBUTE_DIRECTORY) != FILE_ATTRIBUTE_DIRECTORY)
          {

              // Set up the source file

              STRCPY(&szTempSrcFile[0],pszDefSourceDir);

              if ( STRLEN(STRRCHR(&szTempSrcFile[0],'\\')) > 1 )
                  STRCAT(&szTempSrcFile[0], _T("\\"));

              STRCAT(&szTempSrcFile[0],&stFindData.cFileName[0]);

              // Set up the target file

              STRCPY(&szTempTargetFile[0],&szTargetDir[0]);
              STRCAT(&szTempTargetFile[0], _T("\\"));
              STRCAT(&szTempTargetFile[0],&stFindData.cFileName[0]);

              if ( CopyFile( &szTempSrcFile[0], &szTempTargetFile[0], FALSE ) == FALSE )
              {
                  dwError = VIRDEF_ERROR_COPYING_FILES;
                  break;
              }

              // Clear out the temp file names.

              MEMSET(&szTempTargetFile[0], 0, VIRDEF_TEMP_BUFFER_SIZE);
              MEMSET(&szTempSrcFile[0], 0, VIRDEF_TEMP_BUFFER_SIZE);
          }

          // Get the next file

          bFoundAFile = FindNextFile( hFind, &stFindData );
      }

      FindClose( hFind );

      if ( dwError != VIRDEF_OK )
          return (dwError);

      // End of file copy


      if ( VirusDefPostUpdate(hVirusDef) == FALSE )
          return (VIRDEF_POSTUPDATE_FAILED);

      if ( ((CDefUtils *)hVirusDef)->UseNewestDefs(&bDefsChanged) == FALSE )
          return (VIRDEF_CANT_USE_NEWEST_DEFS);

      if ( bDefsChanged == FALSE )
          return (VIRDEF_CANT_USE_NEWEST_DEFS);

      return (VIRDEF_OK);
}




/*@API:**********************************************************************
@Declaration:

    BOOL  DllExport WINAPI VirusDefPostUpdate (void)

@Description:

@Parameters:

@Returns:

@See:

@Include:

    navdefs.h
    defutils.h

@Compatibility: WIN32
****************************************************************************/
BOOL  DllExport WINAPI VirusDefPostUpdate (HVIRUSDEF hVirusDef)
{
    if (hVirusDef != NULL)
        return ( ((CDefUtils *)hVirusDef)->PostDefUpdate() );
    else
        return FALSE;
}



/*@API:**********************************************************************
@Declaration:

    BOOL  DllExport WINAPI VirusDefPreUpdate (
	      HVIRUSDEF hVirusDef,
          LPSTR     pszDir,
          int       nMaxCount,
          VirusDefUpdateType type)


@Description:

@Parameters:

@Returns:

@See:

@Include:

    navdefs.h
    defutils.h

@Compatibility: WIN32
****************************************************************************/
BOOL  DllExport WINAPI VirusDefPreUpdate (
	  HVIRUSDEF hVirusDef,
      LPSTR     pszDir,
      int       nMaxCount,
      VirusDefUpdateType type)
{
    if (hVirusDef != NULL)
        return (((CDefUtils *)hVirusDef)->PreDefUpdate(pszDir, nMaxCount, 
                                     (CDefUtils::UpdateType) type));
    else
        return FALSE;
}



/*@API:**********************************************************************
@Declaration:

    BOOL  DllExport WINAPI VirusDefSetCurrent (
	      HVIRUSDEF hVirusDef,
          LPCSTR    pszDir)

@Description:

@Parameters:

@Returns:

@See:

@Include:

    navdefs.h
    defutils.h

@Compatibility: WIN32
****************************************************************************/
BOOL  DllExport WINAPI VirusDefSetCurrent (
	  HVIRUSDEF hVirusDef,
      LPCSTR    pszDir)
{
    if (hVirusDef != NULL)
        return (((CDefUtils *)hVirusDef)->SetCurrentDefs(pszDir));
    else
        return FALSE;
}



/*@API:**********************************************************************
@Declaration:

    BOOL  DllExport WINAPI VirusDefStopUsingDefs (
	      HVIRUSDEF hVirusDef)

@Description:

@Parameters:

@Returns:

@See:

@Include:

    navdefs.h
    defutils.h

@Compatibility: WIN32
****************************************************************************/
BOOL  DllExport WINAPI VirusDefStopUsingDefs (
	  HVIRUSDEF hVirusDef)
{
    if (hVirusDef != NULL)
        return (((CDefUtils *)hVirusDef)->StopUsingDefs());
    else
        return FALSE;
}



/*@API:**********************************************************************
@Declaration:

    BOOL  DllExport WINAPI VirusDefUseNewest (
	      HVIRUSDEF hVirusDef,
          LPSTR     pszDir,
          int       nMaxCount,
          LPBOOL    pbChanged)

@Description:

@Parameters:

@Returns:

@See:

@Include:

    navdefs.h
    defutils.h

@Compatibility: WIN32
****************************************************************************/
BOOL  DllExport WINAPI VirusDefUseNewest (
	  HVIRUSDEF hVirusDef,
      LPSTR     pszDir,
      int       nMaxCount,
      LPBOOL    pbChanged)
{
    if (hVirusDef != NULL)
        return (((CDefUtils *)hVirusDef)->UseNewestDefs(pszDir, nMaxCount, pbChanged));
    else
        return FALSE;
}



/*@API:**********************************************************************
@Declaration:

    BOOL  DllExport WINAPI VirusDefUninstall (
	      HVIRUSDEF hVirusDef,
          LPSTR     pszAppID)

@Description:

@Parameters:

@Returns:

@See:

@Include:

    navdefs.h
    defutils.h

@Compatibility: WIN32
****************************************************************************/
BOOL DllExport WINAPI VirusDefUninstall (
      HVIRUSDEF hVirusDef,
      LPSTR     pszAppID)
{
    if (hVirusDef != NULL)
    {
        ((CDefUtils *)hVirusDef)->InitInstallApp(pszAppID);
        return (((CDefUtils *)hVirusDef)->StopUsingDefs());
    }
    else
        return FALSE;
}




/***************************************************************************
@Declaration:

@Description:

@Parameters:

@Returns:

@See:

@Include:

@Compatibility: WIN32
****************************************************************************/
LPCTSTR WINAPI SkipWhiteSpace (LPCTSTR lpString)
{
    auto LPCTSTR ptr;

    ptr = lpString;

    while (*ptr == ' ' || *ptr == '\t')
        ptr = CharNext(ptr);

    if (*ptr == '\0')
        return (NULL);
    else
        return (ptr);
}
