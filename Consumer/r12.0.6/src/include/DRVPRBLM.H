// Copyright 1995-1996 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/include/VCS/drvprblm.h_v   1.0   06 Feb 1997 20:52:00   RFULLER  $
//
// Description:
//
// Contains:
//
// See Also:
//************************************************************************
// $Log:   S:/include/VCS/drvprblm.h_v  $
// 
//    Rev 1.0   06 Feb 1997 20:52:00   RFULLER
// Initial revision
// 
//    Rev 1.1   26 Jun 1996 17:01:02   DALLEE
// Port from Luigi Plus:
// Added new DRIVEREPAIR_xxx #defines for DriveAccessProblem() returns.
//************************************************************************

#ifndef _DRIVE_ERROR_
#define _DRIVE_ERROR_

// ---------------------------------------------------------------------------
// The following DRIVEERROR_xxxx defines are possible return values from
// DriveAccessProblem() function that needs a pointer to tagDRIVEPROBLEMSTRUCT
// ---------------------------------------------------------------------------

typedef UINT (WINAPI *LPFNDRIVEERROR)
   (BYTE byDrive,
    DWORD dwUserData);

typedef VOID (WINAPI *LPFNBADENTRY)
   (LPSTR FAR *lplp,
    DWORD dwUserData);

enum
    {
    DRIVEERROR_ERROR = 1,
    DRIVEERROR_RETRY,
    DRIVEERROR_CONTINUE,
    DRIVEERROR_SKIP,

    DRIVEACCESS_MSG_BOOTRECORD,
    DRIVEACCESS_MSG_DRIVE,
    DRIVEACCESS_MSG_DISKLOCKED,
    DRIVEACCESS_MSG_DISKLOCKED_SYMSTAT,
    DRIVEACCESS_MSG_DISKLOCKED_SYMSTAT_MBR,
    DRIVEACCESS_MSG_DISKLOCKED_SYMSTAT_BOOT,
    DRIVEACCESS_MSG_PHYS_BOOTRECORD,
    DRIVEACCESS_MSG_PHYS_DISKLOCKED,

    DRIVEREPAIR_MSG_DISKLOCKED_SYMSTAT,
    DRIVEREPAIR_MSG_DISKLOCKED_SYMSTAT_MBR,
    DRIVEREPAIR_MSG_DISKLOCKED_SYMSTAT_BOOT
    };

typedef struct tagDRIVEPROBLEMSTRUCT
{
    HWND        hParent;
    BYTE        byDrive;                // Drive we're complaining about
    BOOL        bHideContinue;          // Hide continue button
    UINT        uInfoMsgID;             // Message ID to throw up on screen.
    UINT        uTimeOutSeconds;        // Timeout after X seconds.
    HINSTANCE   hInstance;              // Calling prog instance
    char        szWho[SYM_MAX_PATH + 1];// Program with SymStat drive locked

} DRIVEPROBLEMSTRUCT, FAR * LPDRIVEPROBLEMSTRUCT;

#ifdef __cplusplus
extern "C" {
#endif

DWORD SYM_EXPORT WINAPI GetBytesToScan (
    LPSTR           lpScanList,         // [in] list of items to scan
    BOOL            bScanSubs,          // [in] scan sub directories
    LPFNBADENTRY    lpfnSkipDrive,      // function to call to skip a drive
    LPFNDRIVEERROR  lpfnDriveError,     // function to notify if drive error
    DWORD           dwUserData );       // data to pass on to callbacks

UINT SYM_EXPORT WINAPI DriveAccessProblem (
    LPDRIVEPROBLEMSTRUCT    lpDP );

#ifdef __cplusplus
}
#endif

#endif // _DRIVE_ERROR_
