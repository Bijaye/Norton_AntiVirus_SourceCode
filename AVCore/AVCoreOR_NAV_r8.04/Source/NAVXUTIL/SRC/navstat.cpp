// Copyright 1995 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/NAVXUTIL/VCS/NAVSTAT.CPv   1.1   09 Jun 1997 18:29:34   DBUCHES  $
//
// Description:
//      Dynamically load S32STAT.DLL and function pointers.  Provide
//      SYMSTAT covers to other NAV Dll's so we don't statically link
//      to SYMSTAT.
//
// Contains:
//      NavStatInit()
//      NavStatEnd()
//      NavStatAllocInstance()
//      NavStatFreeInstance()
//      NavStatGetInstanceHwnd()
//      NavStatSetInstanceHwnd()
//      NavStatGetLastRunDateTime()
//      NavStatSetLastRunDateTime()
//      NavStatGetDriveState()
//      NavStatSetDriveState()
//      NavStatDisplayAlreadyOwnedError()
//
// See Also:
//************************************************************************
// $Log:   S:/NAVXUTIL/VCS/NAVSTAT.CPv  $
// 
//    Rev 1.1   09 Jun 1997 18:29:34   DBUCHES
// Changed SYMSTAT function names for DEC Alpha platform.  No name mangling
// required on Alpha platform.
// 
//    Rev 1.0   06 Feb 1997 21:05:04   RFULLER
// Initial revision
// 
//    Rev 1.1   24 May 1996 16:46:52   DSACKIN
// Pull the S32STAT.DLL from the place the reg key tells us to.
//
//    Rev 1.0   30 Jan 1996 15:56:14   BARRY
// Initial revision.
//
//    Rev 1.0   30 Jan 1996 14:51:16   BARRY
// Initial revision.
//
//    Rev 1.0   15 Nov 1995 21:44:00   DALLEE
// Initial revision.
//************************************************************************

#include "platform.h"
#include "xapi.h"
#include "symstat.h"
#include "navstat.h"
#include "stbexts.h"

#ifdef SYM_WIN32

// Function pointer typedefs

typedef STATUS (WINAPI *SYMSTATALLOCINSTANCE) (LPCSTR   lpProject);

typedef STATUS (WINAPI *SYMSTATFREEINSTANCE)  (LPCSTR    lpProject);

typedef HWND   (WINAPI *SYMSTATGETINSTANCEHWND) (LPCSTR lpProject);

typedef STATUS (WINAPI *SYMSTATSETINSTANCEHWND) (LPCSTR lpProject,
                                                 HWND   hwndProject);

typedef BOOL   (WINAPI *SYMSTATGETLASTRUNDATETIME) (LPCSTR      lpProduct,
                                                    LPCSTR      lpProject,
                                                    BYTE        bDL,
                                                    SYSTEMTIME  *pst);

typedef BOOL   (WINAPI *SYMSTATSETLASTRUNDATETIME) (LPCSTR  lpProduct,
                                                    LPCSTR  lpProject,
                                                    BYTE    bDL);

typedef DWORD  (WINAPI *SYMSTATGETDRIVESTATE) (BYTE     bDl,
                                               UINT     uEntry,
                                               LPSTR    lpTranslatedName,
                                               UINT     nMaxAppName);

typedef STATUS (WINAPI *SYMSTATSETDRIVESTATE) (BYTE     bDl,
                                               UINT     uEntry,
                                               DWORD    dwState,
                                               LPCSTR   lpTranslatedName);

typedef void   (WINAPI *SYMSTATDISPLAYALREADYOWNEDERROR) (HWND      hWndParent,
                                                          char      cDrive,
                                                          LPCSTR    lpszTitle);


// Static function pointers

STATIC SYMSTATALLOCINSTANCE            lpfnSymStatAllocInstance         = NULL;
STATIC SYMSTATFREEINSTANCE             lpfnSymStatFreeInstance          = NULL;
STATIC SYMSTATGETINSTANCEHWND          lpfnSymStatGetInstanceHwnd       = NULL;
STATIC SYMSTATSETINSTANCEHWND          lpfnSymStatSetInstanceHwnd       = NULL;
STATIC SYMSTATGETLASTRUNDATETIME       lpfnSymStatGetLastRunDateTime    = NULL;
STATIC SYMSTATSETLASTRUNDATETIME       lpfnSymStatSetLastRunDateTime    = NULL;
STATIC SYMSTATGETDRIVESTATE            lpfnSymStatGetDriveState         = NULL;
STATIC SYMSTATSETDRIVESTATE            lpfnSymStatSetDriveState         = NULL;
STATIC SYMSTATDISPLAYALREADYOWNEDERROR lpfnSymStatDisplayAlreadyOwnedError = NULL;


// Statics for loading S32STAT and functions

STATIC int          nLoadedCount = 0;
STATIC HINSTANCE    hSymStat = NULL;
STATIC CHAR         szSymStat [] = "S32STAT.DLL";


#ifdef _M_ALPHA
STATIC CHAR SYMSTATALLOCINSTANCE_NAME			[] = "SymStatAllocInstance";
STATIC CHAR SYMSTATFREEINSTANCE_NAME			[] = "SymStatFreeInstance";
STATIC CHAR SYMSTATGETINSTANCEHWND_NAME			[] = "SymStatGetInstanceHwnd";
STATIC CHAR SYMSTATSETINSTANCEHWND_NAME			[] = "SymStatSetInstanceHwnd";
STATIC CHAR SYMSTATGETLASTRUNDATETIME_NAME		[] = "SymStatGetLastRunDateTime";
STATIC CHAR SYMSTATSETLASTRUNDATETIME_NAME		[] = "SymStatSetLastRunDateTime";
STATIC CHAR SYMSTATGETDRIVESTATE_NAME			[] = "SymStatGetDriveState";
STATIC CHAR SYMSTATSETDRIVESTATE_NAME			[] = "SymStatSetDriveState";
STATIC CHAR SYMSTATDISPLAYALREADYOWNEDERROR_NAME[] = "SymStatDisplayAlreadyOwnedError";
#else
STATIC CHAR SYMSTATALLOCINSTANCE_NAME           [] = "_SymStatAllocInstance@4";
STATIC CHAR SYMSTATFREEINSTANCE_NAME            [] = "_SymStatFreeInstance@4";
STATIC CHAR SYMSTATGETINSTANCEHWND_NAME         [] = "_SymStatGetInstanceHwnd@4";
STATIC CHAR SYMSTATSETINSTANCEHWND_NAME         [] = "_SymStatSetInstanceHwnd@8";
STATIC CHAR SYMSTATGETLASTRUNDATETIME_NAME      [] = "_SymStatGetLastRunDateTime@16";
STATIC CHAR SYMSTATSETLASTRUNDATETIME_NAME      [] = "_SymStatSetLastRunDateTime@12";
STATIC CHAR SYMSTATGETDRIVESTATE_NAME           [] = "_SymStatGetDriveState@16";
STATIC CHAR SYMSTATSETDRIVESTATE_NAME           [] = "_SymStatSetDriveState@16";
STATIC CHAR SYMSTATDISPLAYALREADYOWNEDERROR_NAME[] = "_SymStatDisplayAlreadyOwnedError@12";
#endif

/////////////////////////////////////////////////////////////////////
// GetSymStatDLLName
//
// This function uses the registry key to get the full
// path to symstat.dll
//
// Parameters:
//    LPSTR   szFullPath - [out] full path to SYMSTAT
//
// Return Value:
//    none.
/////////////////////////////////////////////////////////////////////
// 05/24/96 - DSACKIN - Function Created
/////////////////////////////////////////////////////////////////////

VOID LOCAL GetSymStatDLLName (
   LPSTR  szFullPath)
{
auto   HKEY      hAppPathKey = NULL;
auto   int       nRet;
auto   long      lRet;
auto   TCHAR     szAppPathKey [SYM_MAX_PATH];
auto   TCHAR     szPathValue [SYM_MAX_PATH];
auto   BOOL      bSuccess = TRUE;       // Assume success.
auto   DWORD     dwSize = sizeof (szPathValue);

extern HINSTANCE hInstance;

   nRet = LoadString (
             hInstance,
             IDS_HKEY_APPPATH,
             szAppPathKey,
             sizeof (szAppPathKey) );

   if ( nRet > 0 )
   {
      lRet = RegOpenKeyEx(
                HKEY_LOCAL_MACHINE,
                szAppPathKey,
                0,
                KEY_READ | KEY_QUERY_VALUE,
                &hAppPathKey );

      if ( ERROR_SUCCESS == lRet )
      {
         lRet = RegQueryValueEx (
                   hAppPathKey,
                   NULL,
                   NULL,
                   NULL,
                   (LPBYTE)szPathValue,
                   &dwSize);

         if ( ERROR_SUCCESS == lRet )
         {
            NameAppendFile (szPathValue, szSymStat);
            STRCPY (szFullPath, szPathValue);
         }
         else
            bSuccess = FALSE;
      }
      else
         bSuccess = FALSE;
   }
   else
      bSuccess = FALSE;

   if ( !bSuccess )                     // On failure, use default value
      STRCPY (szFullPath, szSymStat);
} //GetSymStatDLLName()


/*@API:**********************************************************************
@Declaration:
STATUS  WINAPI NavStatInit ()

@Description:
This routine dynamically loads S32STAT.DLL and sets up NAVXUTIL's internal
function pointers so the NavStatXXX() cover functions will work.

NavStatEnd() should be called to free the library, but only if this call
succeeds.

@Parameters:
None.

@Returns:
NOERR if successful.
ERR_NAVSTAT_DLL_NOT_FOUND      if S32STAT couldn't be loaded.
ERR_NAVSTAT_FUNCTION_NOT_FOUND if a required function wasn't found.

@See:
@Include:   navstat.h
@Compatibility: W32
****************************************************************************/
// 10/14/95 DALLEE, created.
//**************************************************************************

STATUS SYM_EXPORT WINAPI NavStatInit ()
{
auto  char  szFullPath[SYM_MAX_PATH];

    GetSymStatDLLName(szFullPath);
                                        // Check if first time.
                                        //&? Should create a mutex for
                                        // accessing nLoadedCount.
    if (0 == nLoadedCount)
        {
                                        // Load S32STAT.DLL.
        hSymStat = (HINSTANCE)LoadLibrary(szFullPath);
        if ((HINSTANCE)HINSTANCE_ERROR > hSymStat)
            {
            hSymStat = NULL;
            return (ERR_NAVSTAT_DLL_NOT_FOUND);
            }

                                        // Get function pointers.
        lpfnSymStatAllocInstance            = (SYMSTATALLOCINSTANCE)
                GetProcAddress(hSymStat, SYMSTATALLOCINSTANCE_NAME);

        lpfnSymStatFreeInstance             = (SYMSTATFREEINSTANCE)
                GetProcAddress(hSymStat, SYMSTATFREEINSTANCE_NAME);

        lpfnSymStatGetInstanceHwnd          = (SYMSTATGETINSTANCEHWND)
                GetProcAddress(hSymStat, SYMSTATGETINSTANCEHWND_NAME);

        lpfnSymStatSetInstanceHwnd          = (SYMSTATSETINSTANCEHWND)
                GetProcAddress(hSymStat, SYMSTATSETINSTANCEHWND_NAME);

        lpfnSymStatGetLastRunDateTime       = (SYMSTATGETLASTRUNDATETIME)
                GetProcAddress(hSymStat, SYMSTATGETLASTRUNDATETIME_NAME);

        lpfnSymStatSetLastRunDateTime       = (SYMSTATSETLASTRUNDATETIME)
                GetProcAddress(hSymStat, SYMSTATSETLASTRUNDATETIME_NAME);

        lpfnSymStatGetDriveState            = (SYMSTATGETDRIVESTATE)
                GetProcAddress(hSymStat, SYMSTATGETDRIVESTATE_NAME);

        lpfnSymStatSetDriveState            = (SYMSTATSETDRIVESTATE)
                GetProcAddress(hSymStat, SYMSTATSETDRIVESTATE_NAME);

        lpfnSymStatDisplayAlreadyOwnedError = (SYMSTATDISPLAYALREADYOWNEDERROR)
                GetProcAddress(hSymStat, SYMSTATDISPLAYALREADYOWNEDERROR_NAME);

        if ( (NULL == lpfnSymStatAllocInstance) ||
             (NULL == lpfnSymStatFreeInstance)  ||
             (NULL == lpfnSymStatGetInstanceHwnd) ||
             (NULL == lpfnSymStatSetInstanceHwnd) ||
             (NULL == lpfnSymStatGetLastRunDateTime) ||
             (NULL == lpfnSymStatSetLastRunDateTime) ||
             (NULL == lpfnSymStatGetDriveState) ||
             (NULL == lpfnSymStatSetDriveState) ||
             (NULL == lpfnSymStatDisplayAlreadyOwnedError) )
            {
            //&? Would call FreeProcInstance() first in W16.
            lpfnSymStatAllocInstance            = NULL;
            lpfnSymStatFreeInstance             = NULL;
            lpfnSymStatGetInstanceHwnd          = NULL;
            lpfnSymStatSetInstanceHwnd          = NULL;
            lpfnSymStatGetLastRunDateTime       = NULL;
            lpfnSymStatSetLastRunDateTime       = NULL;
            lpfnSymStatGetDriveState            = NULL;
            lpfnSymStatSetDriveState            = NULL;
            lpfnSymStatDisplayAlreadyOwnedError = NULL;

            FreeLibrary(hSymStat);
            hSymStat = NULL;

            return (ERR_NAVSTAT_FUNCTION_NOT_FOUND);
            }
        }

    nLoadedCount++;

    return (NOERR);
} // NavStatInit()


/*@API:**********************************************************************
@Declaration:
STATUS  WINAPI NavStatEnd ()

@Description:
This routine unloads S32STAT.DLL and zero's NAVXUTIL's internal
function pointers.

@Parameters:
None.

@Returns:
NOERR                       if successful or NavStat never initialized.
ERR_NAVSTAT_DLL_NOT_FREED   if FreeLibrary() failed.

@See:
@Include:   navstat.h
@Compatibility: W32
****************************************************************************/
// 10/14/95 DALLEE, created.
//**************************************************************************

STATUS SYM_EXPORT WINAPI NavStatEnd ()
{
    auto    STATUS  nReturn = NOERR;

                                        //&? Should create a mutex for
                                        // accessing nLoadedCount.
    if (0 < nLoadedCount)
        {
                                        // Decrement count and free DLL on 0.
        if (0 == --nLoadedCount)
            {
            //&? Would call FreeProcInstance() first in W16.
            lpfnSymStatAllocInstance            = NULL;
            lpfnSymStatFreeInstance             = NULL;
            lpfnSymStatGetInstanceHwnd          = NULL;
            lpfnSymStatSetInstanceHwnd          = NULL;
            lpfnSymStatGetLastRunDateTime       = NULL;
            lpfnSymStatSetLastRunDateTime       = NULL;
            lpfnSymStatGetDriveState            = NULL;
            lpfnSymStatSetDriveState            = NULL;
            lpfnSymStatDisplayAlreadyOwnedError = NULL;

            if (FALSE == FreeLibrary(hSymStat))
                {
                //&? DALLEE - I don't think this'll ever fail, but if it does...
                // hSymStat is set = NULL regardless, and a new handle
                // retrieved if NavStatInit() is called again.  This is
                // on the assumption that the FreeLibrary() failed based
                // on a bad handle(?).
                nReturn = ERR_NAVSTAT_DLL_NOT_FREED;
                }
            hSymStat = NULL;
            }
        }

    return (nReturn);
} // NavStatEnd()


// Cover functions with same parameters/returns as SYMSTAT functions.
// These will pass through to the dynamically loaded SYMSTAT routines.

STATUS SYM_EXPORT WINAPI NavStatAllocInstance (LPCSTR   lpProject)
{
    if (NULL != lpfnSymStatAllocInstance)
        {
        return (lpfnSymStatAllocInstance(lpProject));
        }
    else
        {
        return (ERR);
        }
}

STATUS SYM_EXPORT WINAPI NavStatFreeInstance (LPCSTR    lpProject)
{
    if (NULL != lpfnSymStatFreeInstance)
        {
        return (lpfnSymStatFreeInstance(lpProject));
        }
    else
        {
        return (ERR);
        }
}

HWND   SYM_EXPORT WINAPI NavStatGetInstanceHwnd (LPCSTR lpProject)
{
    if (NULL != lpfnSymStatGetInstanceHwnd)
        {
        return (lpfnSymStatGetInstanceHwnd(lpProject));
        }
    else
        {
        return (NULL);
        }
}

STATUS SYM_EXPORT WINAPI NavStatSetInstanceHwnd (LPCSTR lpProject,
                                                 HWND   hwndProject)
{
    if (NULL != lpfnSymStatSetInstanceHwnd)
        {
        return (lpfnSymStatSetInstanceHwnd(lpProject, hwndProject));
        }
    else
        {
        return (ERR);
        }
}

BOOL   SYM_EXPORT WINAPI NavStatGetLastRunDateTime (LPCSTR      lpProduct,
                                                    LPCSTR      lpProject,
                                                    BYTE        bDL,
                                                    SYSTEMTIME  *pst)
{
    if (NULL != lpfnSymStatGetLastRunDateTime)
        {
        return (lpfnSymStatGetLastRunDateTime(lpProduct,
                                              lpProject,
                                              bDL,
                                              pst));
        }
    else
        {
        return (FALSE);
        }
}

BOOL   SYM_EXPORT WINAPI NavStatSetLastRunDateTime (LPCSTR  lpProduct,
                                                    LPCSTR  lpProject,
                                                    BYTE    bDL)
{
    if (NULL != lpfnSymStatSetLastRunDateTime)
        {
        return (lpfnSymStatSetLastRunDateTime(lpProduct,
                                              lpProject,
                                              bDL));
        }
    else
        {
        return (FALSE);
        }
}

DWORD  SYM_EXPORT WINAPI NavStatGetDriveState (BYTE     bDl,
                                               UINT     uEntry,
                                               LPSTR    lpTranslatedName,
                                               UINT     nMaxAppName)
{
    if (NULL != lpfnSymStatGetDriveState)
        {
        return (lpfnSymStatGetDriveState(bDl,
                                         uEntry,
                                         lpTranslatedName,
                                         nMaxAppName));
        }
    else
        {
        return (0);
        }
}

STATUS SYM_EXPORT WINAPI NavStatSetDriveState (BYTE     bDl,
                                               UINT     uEntry,
                                               DWORD    dwState,
                                               LPCSTR   lpTranslatedName)
{
    if (NULL != lpfnSymStatSetDriveState)
        {
        return (lpfnSymStatSetDriveState(bDl,
                                         uEntry,
                                         dwState,
                                         lpTranslatedName));
        }
    else
        {
        return (ERR);
        }
}

void   SYM_EXPORT WINAPI NavStatDisplayAlreadyOwnedError (HWND      hWndParent,
                                                          char      cDrive,
                                                          LPCSTR    lpszTitle)
{
    if (NULL != lpfnSymStatDisplayAlreadyOwnedError)
        {
        lpfnSymStatDisplayAlreadyOwnedError(hWndParent,
                                            cDrive,
                                            lpszTitle);
        }
    else
        {
        }
}

#endif SYM_WIN32
