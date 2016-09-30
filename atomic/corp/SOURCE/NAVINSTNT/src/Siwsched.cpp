//************************************************************************
// Copyright 1997 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/NAVINSTNT/VCS/Siwsched.cpv   1.0   11 May 1997 21:36:50   SEDWARD  $
//
// Description:
//      Dynamically load NAVNTSCH.DLL and function pointers.
//      Then add/remove scheduler jobs.
//
// Contains:
//      SiwSchedInit()
//      SiwSchedEnd()
//
//
// See Also:
//************************************************************************
// $Log:   S:/NAVINSTNT/VCS/Siwsched.cpv  $
// 
//    Rev 1.0   11 May 1997 21:36:50   SEDWARD
// Initial revision.
//
//    Rev 1.0   09 May 1997 12:17:58   SEDWARD
// Initial revision
//
//************************************************************************

#include "platform.h"
#include "xapi.h"
#include "navntsch.h"
#include "NavInst.h"
#include "resource.h"

//#define _NO_WIN_TYPEDEFS_
//#include "siuniv40.h"
//#include "siwnav16.h"


#ifdef SYM_WIN32



// From siwnav16.cpp


// Error codes for SiwSchedInit() and SiwSchedEnd()
// NOERR or...
#define ERR_SIWSCHED_DLL_NOT_FOUND       (STATUS) 1
#define ERR_SIWSCHED_FUNCTION_NOT_FOUND  (STATUS) 2
#define ERR_SIWSCHED_DLL_NOT_FREED       (STATUS) 3

STATUS WINAPI SiwSchedInit ();
STATUS WINAPI SiwSchedEnd ();
STATIC WORD SiwSchedScheduleScanFromInstall (BOOL       SchedulScan,
                                             WORD       DayOfWeek,
                                             WORD       HourOfDay,
                                             WORD       MinuteOfHour,
                                             LPCTSTR    CommandText,
                                             DWORD      Flags);
STATIC WORD SiwSchedPurgeScheduledScans (LPCTSTR    CommandText,
                                         DWORD      Flags);
STATIC BOOL SiwSchedSchedulerIsRunning ();



// Function pointer typedefs
typedef WORD  (WINAPI *SCHEDULESCANFROMINSTALL) (BOOL       SchedulScan,
                                                 WORD       DayOfWeek,
                                                 WORD       HourOfDay,
                                                 WORD       MinuteOfHour,
                                                 LPCTSTR    CommandText,
                                                 DWORD      Flags);

typedef WORD  (WINAPI *PURGESCHEDULEDSCANS) (LPCTSTR    CommandText,
                                             DWORD      Flags);

typedef WORD  (WINAPI *SCHEDULERISRUNNING) ();



// Static function pointers
STATIC SCHEDULESCANFROMINSTALL  lpfnScheduleScanFromInstall = NULL;
STATIC PURGESCHEDULEDSCANS      lpfnPurgeScheduledScans = NULL;
STATIC SCHEDULERISRUNNING       lpfnSchedulerIsRunning = NULL;



// Statics for loading S32STAT and functions
STATIC int          nLoadedCount = 0;
STATIC HINSTANCE    hNavSched = NULL;
STATIC CHAR         szNavSched [] = "NAVNTSCH.DLL";

STATIC CHAR SCHEDULESCANFROMINSTALL_NAME    [] = "ScheduleScanFromInstall";
STATIC CHAR PURGESCHEDULEDSCANS_NAME        [] = "PurgeScheduledScans";
STATIC CHAR SCHEDULERISRUNNING_NAME         [] = "SchedulerIsRunning";


/*@API:**********************************************************************
@Declaration:
STATUS  WINAPI SiwSchedInit ()

@Description:
This routine dynamically loads NAVNTSCH.DLL and sets up the internal
function pointers so the SiwSchedXXX() cover functions will work.

SiwSchedEnd() should be called to free the library, but only if this call
succeeds.

@Parameters:
None.

@Returns:
NOERR if successful.
ERR_SIWSCHED_DLL_NOT_FOUND      if NAVNTSCH couldn't be loaded.
ERR_SIWSCHED_FUNCTION_NOT_FOUND if a required function wasn't found.

@See:
@Include:   navsched.h
@Compatibility: W32
****************************************************************************/
// 12/01/95 DALLEE, created.
//**************************************************************************

STATUS WINAPI SiwSchedInit(void)
{
    auto    TCHAR       szTempBuf[SYM_MAX_PATH];

                                        // Check if first time.
                                        //&? Should create a mutex for
                                        // accessing nLoadedCount.
    if (0 == nLoadedCount)
        {
                                        // Load NAVNTSCH.DLL.
        lstrcpy(szTempBuf, g_szTarget);
        lstrcat(szTempBuf, "\\");
        lstrcat(szTempBuf, szNavSched);
        hNavSched = (HINSTANCE)LoadLibrary(szTempBuf);
        if ((HINSTANCE)HINSTANCE_ERROR > hNavSched)
            {
            hNavSched = NULL;
            return (ERR_SIWSCHED_DLL_NOT_FOUND);
            }

                                        // Get function pointers.
        lpfnScheduleScanFromInstall = (SCHEDULESCANFROMINSTALL)
                GetProcAddress(hNavSched, SCHEDULESCANFROMINSTALL_NAME);

        lpfnPurgeScheduledScans     = (PURGESCHEDULEDSCANS)
                GetProcAddress(hNavSched, PURGESCHEDULEDSCANS_NAME);

        lpfnSchedulerIsRunning      = (SCHEDULERISRUNNING)
                GetProcAddress(hNavSched, SCHEDULERISRUNNING_NAME);

        if ( (NULL == lpfnScheduleScanFromInstall) ||
             (NULL == lpfnPurgeScheduledScans) ||
             (NULL == lpfnSchedulerIsRunning) )
            {
            //&? Would call FreeProcInstance() first in W16.
            lpfnScheduleScanFromInstall = NULL;
            lpfnPurgeScheduledScans     = NULL;
            lpfnSchedulerIsRunning      = NULL;

            FreeLibrary(hNavSched);
            hNavSched = NULL;

            return (ERR_SIWSCHED_FUNCTION_NOT_FOUND);
            }
        }

    nLoadedCount++;

    return (NOERR);

} // SiwSchedInit()



/*@API:**********************************************************************
@Declaration:
STATUS  WINAPI SiwSchedEnd ()

@Description:
This routine unloads NAVNTSCH.DLL and zero's the internal function pointers.

@Parameters:
None.

@Returns:
NOERR                        if successful or SiwSched never initialized.
ERR_SIWSCHED_DLL_NOT_FREED   if FreeLibrary() failed.

@See:
@Include:   navsched.h
@Compatibility: W32
****************************************************************************/
// 12/01/95 DALLEE, created.
//**************************************************************************

STATUS WINAPI SiwSchedEnd ()
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
            lpfnScheduleScanFromInstall = NULL;
            lpfnPurgeScheduledScans     = NULL;
            lpfnSchedulerIsRunning      = NULL;

            if (FALSE == FreeLibrary(hNavSched))
                {
                //&? DALLEE - I don't think this'll ever fail, but if it does...
                // hNavSched is set = NULL regardless, and a new handle
                // retrieved if SiwSchedInit() is called again.  This is
                // on the assumption that the FreeLibrary() failed based
                // on a bad handle(?).
                nReturn = ERR_SIWSCHED_DLL_NOT_FREED;
                }
            hNavSched = NULL;
            }
        }

    return (nReturn);

} // SiwSchedEnd()






// Cover functions with same parameters/returns as SYMSTAT functions.
// These will pass through to the dynamically loaded SYMSTAT routines.

// === SiwSchedScheduleScanFromInstall ====================================
//
//
// ========================================================================
// ========================================================================

STATIC WORD SiwSchedScheduleScanFromInstall(BOOL       SchedulScan,
                                             WORD       DayOfWeek,
                                             WORD       HourOfDay,
                                             WORD       MinuteOfHour,
                                             LPCTSTR    CommandText,
                                             DWORD      Flags)
{
    if (NULL != lpfnScheduleScanFromInstall)
        {
        return (lpfnScheduleScanFromInstall(SchedulScan,
                                            DayOfWeek,
                                            HourOfDay,
                                            MinuteOfHour,
                                            CommandText,
                                            Flags));
        }
    else
        {
        return (ERR);
        }

}  //  SiwSchedScheduleScanFromInstall



// === SiwSchedPurgeScheduledScans ========================================
//
//
// ========================================================================
// ========================================================================

STATIC WORD SiwSchedPurgeScheduledScans(LPCTSTR    CommandText,
                                         DWORD      Flags)
{
    if (NULL != lpfnPurgeScheduledScans)
        {
        return (lpfnPurgeScheduledScans(CommandText, Flags));
        }
    else
        {
        return (ERR);
        }

}  //  SiwSchedPurgeScheduledScans




// === SiwSchedSchedulerIsRunning =========================================
//
//
// ========================================================================
// ========================================================================

STATIC BOOL SiwSchedSchedulerIsRunning(void)
{
    if (NULL != lpfnSchedulerIsRunning)
        {
        return (lpfnSchedulerIsRunning());
        }
    else
        {
        return (FALSE);
        }

}  //  SiwSchedSchedulerIsRunning




/*@API:**********************************************************************
@Declaration:
EXTERNC BOOL SYM_EXPORT WINAPI FridayScan (HWND          hWnd,
                                           LPSTR         lpInfline,
                                           LPINSTALLINFO lpInstallInfo);

@Description:
This function adds or removes the NT scheduler task for N32SCANW based
on switch81 SWITCH_DO_RUNSCHEDULE.

@Parameters:
$hWnd$
$lpInfline$         .INF file line that launched this command.
$lpInstallInfo$     Global install info - most importantly has TARGET dir.

@Returns:           TRUE on success, FALSE if the task couldn't be set
                    the way we want it.
@See:
@Include:
@Compatibility:
****************************************************************************/

extern "C" DllExport   BOOL    FridayScan(void)
{
    auto    BOOL    bSuccess = FALSE;
    auto    char    szTempBufOne[SYM_MAX_PATH];
    auto    char    szTempBufTwo[SYM_MAX_PATH];

    if (NOERR == SiwSchedInit())
        {
        LoadString(g_hInst, IDS_SCHEDULE_TASK_FORMAT, szTempBufOne
                                                    , sizeof(szTempBufOne));
        sprintf(szTempBufTwo, szTempBufOne, g_szTarget
                                                    , GetString(IDS_NAV_EXE_NAME));
        bSuccess = (NOERR == SiwSchedScheduleScanFromInstall(g_RunSchedule
                                                    , NTS_DEFAULT_SCANDAY
                                                    , NTS_DEFAULT_SCANHOUR
                                                    , NTS_DEFAULT_SCANMIN
                                                    , szTempBufTwo
                                                    , NTSCHED_CONVERTFROMANSI));
        SiwSchedEnd();
        }

    return (bSuccess);

} // FridayScan()




/*@API:**********************************************************************
@Declaration:
EXTERNC BOOL SYM_EXPORT WINAPI RemoveScheduledScans
                                          (HWND          hWnd,
                                           LPSTR         lpInfline,
                                           LPINSTALLINFO lpInstallInfo);

@Description:
This function removes NT scheduler tasks for N32SCANW.

@Parameters:
$hWnd$
$lpInfline$         .INF file line that launched this command.
$lpInstallInfo$     Global install info - most importantly has TARGET dir.

@Returns:           TRUE on success, FALSE if the task couldn't be set
                    the way we want it.
@See:
@Include:
@Compatibility:
****************************************************************************/

extern "C" DllExport   BOOL    RemoveScheduledScans(void)
{
    auto    BOOL    bSuccess = FALSE;
    auto    char    szTempBufOne[SYM_MAX_PATH];
    auto    char    szTempBufTwo[SYM_MAX_PATH];

    if (NOERR == SiwSchedInit())
        {
        LoadString(g_hInst, IDS_SCHEDULE_TASK_FORMAT, szTempBufOne
                                                    , sizeof(szTempBufOne));
        sprintf(szTempBufTwo, szTempBufOne, g_szTarget
                                                    , GetString(IDS_NAV_EXE_NAME));

        // use to call "NameReturnLongName" here...

        bSuccess = (NOERR == SiwSchedPurgeScheduledScans(szTempBufTwo
                                                    , NTSCHED_CONVERTFROMANSI));
        SiwSchedEnd();
        }

    return (bSuccess);

} // RemoveScheduledScans()




/*@API:**********************************************************************
@Declaration:
EXTERNC BOOL SYM_EXPORT WINAPI IsScheduleRunning (HWND          hWnd,
                                                  LPSTR         lpInfline,
                                                  LPINSTALLINFO lpInstallInfo);

@Description:
Returns whether or not the schedule service is running.

@Parameters:
$hWnd$
$lpInfline$         .INF file line that launched this command.
$lpInstallInfo$     Global install info - most importantly has TARGET dir.

@Returns:           TRUE if the NT schedule service is running.
                    FALSE otherwise
@See:
@Include:
@Compatibility:
****************************************************************************/

extern "C" DllExport   BOOL    IsScheduleRunning(void)
{
    auto    BOOL    bRunning = FALSE;
    if (NOERR == SiwSchedInit())
        {
        bRunning = SiwSchedSchedulerIsRunning();
        SiwSchedEnd();
        }

    return (bRunning);

} // IsScheduleRunning()

#endif SYM_WIN32