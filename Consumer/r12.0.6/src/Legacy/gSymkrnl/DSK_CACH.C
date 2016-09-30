/* Copyright 1992 Symantec, Peter Norton Computing Group                    */
/*--------------------------------------------------------------------------*/
/* $Header:   S:/SYMKRNL/VCS/DSK_CACH.C_v   1.0   26 Jan 1996 20:21:52   JREARDON  $ */
/*     This module contains the processing for create a Norton Drive Cache.   */
/*     It also determines which are the valid drives on a system.           */
/*                                                                          */
/*--------------------------------------------------------------------------*/
/* $Log:   S:/SYMKRNL/VCS/DSK_CACH.C_v  $ */
// 
//    Rev 1.0   26 Jan 1996 20:21:52   JREARDON
// Initial revision.
// 
//    Rev 1.16   11 Apr 1995 12:43:30   HENRI
// Merge changes from branch 6
//
//    Rev 1.15   11 Nov 1994 16:13:10   BILL
// Volume label cache can be upto SYM_MAX_PATH in size, not 11
//
//    Rev 1.14   09 Nov 1994 15:52:42   BRAD
// Changed MemUnlock() to MemRelease(), when no ptr
//
//    Rev 1.13   23 Mar 1994 12:29:30   BRAD
// DiskGetType() NOW takes UINTs, not WORDs
//
//    Rev 1.12   21 Mar 1994 00:20:08   BRAD
// Cleaned up for WIN32
//
//    Rev 1.11   15 Mar 1994 12:34:12   BRUCE
// Changed EXPORT to SYM_EXPORT
//
//    Rev 1.10   25 Feb 1994 13:35:24   BRUCE
// Changed MAX_DOS_DRIVES to SYM_MAX_DOS_DRIVES
//
//    Rev 1.9   25 Feb 1994 12:22:16   BRUCE
// Ran scripts to change to SYM_ constants and get ready for Win32
//
//    Rev 1.8   02 Sep 1993 12:29:08   BARRY
// Changed so it would compile under DOSVMM Debug builds (/DMEMORY_TRACKING)
//
//    Rev 1.7   01 Sep 1993 15:17:38   BILL
// Changed caching logic so that drive label is only gotten when actually
// needed.  (Right before returning to caller) to prevent startup side
// effects such as forcing cdrom spin up and unvalidated network connections
// being forced to connect.  Delayed such activity until we actually need
// the data.
//
//    Rev 1.6   27 Aug 1993 14:52:56   HENRI
// Added the DiskCacheTerminate function
//
//    Rev 1.5   30 Mar 1993 01:33:38   BRUCE
// [FIX] Fixed DiskCacheGetInfo() to allocate the structure DDE_SHARE
//
//    Rev 1.4   16 Mar 1993 21:54:24   BRUCE
// [fix] Fixed DiskCacheGetDrivesByMask to always return number of drives
//       matching the specified mask as documented.  There was a bug such that
//       the number of drives would only be returned if lpDrvNames == NULL &&
//       wMask == NDRV_AVAILABLE.
//
//    Rev 1.3   12 Feb 1993 04:28:12   ENRIQUE
// No change.
//
//    Rev 1.2   06 Nov 1992 12:50:52   HENRI
// Added MODULE_NAME;
//
//    Rev 1.1   04 Nov 1992 18:07:22   MANSHAN
// Added the DiskCacheInit() macro.
//
//    Rev 1.0   04 Nov 1992 17:07:00   MANSHAN
// Initial revision.
/*--------------------------------------------------------------------------*/

#include "platform.h"
#include "xapi.h"
#include "disk.h"

MODULE_NAME;		// Used by VMM-DOS for memory tracking when
			// debugger is turned on.  Otherwise this is
			// a NOP.
typedef struct
{
    char    cDriveLetter;
    char    szVolumeLabel[SYM_MAX_PATH + 1];
    WORD    wMajorType;
    WORD    wMinorType;
    WORD    wMask;
    WORD    wFlags;
} DISKCACHE, FAR *LPDISKCACHE;
//typedef DISKCACHE FAR       *LPDISKCACHE;

#define DISKCACHE_TYPEREAD  0x0001
#define DISKCACHE_LABELREAD 0x0002
#define DISKCACHE_ALL       0x0003

/*
**  Local Variables:
*/
static HANDLE   s_hDiskCache = NULL;
static char     s_szAvailDrvNames[SYM_MAX_DOS_DRIVES + 1];
static WORD     s_szAvailDrvMasks[SYM_MAX_DOS_DRIVES];

/*
**  Local Procedures:
*/
WORD LOCAL DiskCacheSetInfo (UINT uDisk, LPDISKCACHE lpDiskCache, int nType);


/************************************************************************/
/*
**  Section I:  Initialization
*/
/*--------------------------------------------------------------------------*/
/* DiskCacheUpdate():                                                       */
/*    This routine is used to get the drive type for all drives associated  */
/*    with a specific system.  The information is cached.                   */
/*                                                                          */
/* INPUTS                                                                   */
/*    char  cDrive  Drive to be updated                                     */
/*              or  DISK_CACHE_INIT (has no effect if already initialized)  */
/*              or  DISK_CACHE_REINIT (forces initialization)               */
/*                                                                          */
/* RETURNS                                                                  */
/*    NOERR                                                                 */
/*--------------------------------------------------------------------------*/
WORD SYM_EXPORT WINAPI DiskCacheUpdate(char cDrive)
{
    if (s_hDiskCache == NULL  ||  cDrive != DISK_CACHE_INIT)
	{
	LPDISKCACHE     lpDiskCache;
	WORD            i;
	int             nAvailable;
	#ifdef SYM_WIN
	HCURSOR         hCursor;

	hCursor = SetCursor(LoadCursor(NULL, IDC_WAIT));
	#endif

					/* Allocate for Maximum number       */
					/* of possible drives.               */
	if (s_hDiskCache == NULL)
	    {                               //&? THIS NEVER GETS FREED!
					    //&? WE NEED A CLEANUP FOR THIS
					    //&? ROUTINE
	    s_hDiskCache = MemAlloc(GMEM_MOVEABLE | GMEM_ZEROINIT | GMEM_DDESHARE,
				    SYM_MAX_DOS_DRIVES * sizeof(DISKCACHE));
	    cDrive = DISK_CACHE_REINIT;
	    }

	lpDiskCache = (LPDISKCACHE)MemLock(s_hDiskCache);

	/*
	**  Saved the drive letters of all the available drives.
	*/
	nAvailable = 0;
	for (i = 0; i < SYM_MAX_DOS_DRIVES; i++, lpDiskCache++)
	    {
	    /*
	    **  Update only if it is the drive specified or if all drives.
	    */
	    if (cDrive == DISK_CACHE_REINIT  ||  cDrive == (char)('A' + i))
		DiskCacheSetInfo(i, lpDiskCache, DISKCACHE_TYPEREAD);

	    if (lpDiskCache->wMask  &&  lpDiskCache->wMask != NDRV_PHANTOM)
		{
		s_szAvailDrvNames[nAvailable] = lpDiskCache->cDriveLetter;
		s_szAvailDrvMasks[nAvailable] = lpDiskCache->wMask;
		nAvailable++;
		}
	    }
	s_szAvailDrvNames[nAvailable] = EOS;

	MemUnlock( s_hDiskCache, lpDiskCache );

	#ifdef SYM_WIN
	SetCursor(hCursor);
	#endif
	}

    return(NOERR);
} /* DiskCacheUpdate */


/*---------------------------------------------------------------------------*/
/* DiskCacheSetInfo                                                          */
/*    This routine is used to get information on a particular drive.         */
/*                                                                           */
/* INPUTS                                                                    */
/*    WORD          wDisk           the disk drive (0=A, 1=B, etc.)          */
/*    LPDISKCACHE  lpDiskCache    Drv struct for this drive                  */
/*                                                                           */
/* RETURNS                                                                   */
/*    NOERR                                                                  */
/*                                                                           */
/*---------------------------------------------------------------------------*/
WORD LOCAL DiskCacheSetInfo (UINT uDisk, LPDISKCACHE lpDiskCache, int nType)
{
    WORD    wPrevError;
    char    cDrive = (char)(uDisk + 'A');

    if((nType & DISKCACHE_TYPEREAD) || !(lpDiskCache->wFlags & DISKCACHE_TYPEREAD))
        {
        UINT    uMajorType, uMinorType;

        DiskGetType(cDrive, &uMajorType, &uMinorType);
        lpDiskCache->wMajorType = (WORD) uMajorType;
        lpDiskCache->wMinorType = (WORD) uMinorType;
        lpDiskCache->wMask = DiskGetMask(lpDiskCache->wMajorType,lpDiskCache->wMinorType);
        lpDiskCache->cDriveLetter = cDrive;
                                        // Nuke volume label flag so that it
                                        // will be reread later.
        lpDiskCache->wFlags = DISKCACHE_TYPEREAD;
        }

    if(nType & DISKCACHE_LABELREAD)
    {
        lpDiskCache->szVolumeLabel[0] = EOS;
        if (lpDiskCache->wMask && (lpDiskCache->wMask != NDRV_PHANTOM))
        {
            if (lpDiskCache->wMajorType != TYPE_REMOVEABLE &&
                lpDiskCache->wMajorType != TYPE_ENCRYPTED)
            {
                wPrevError = SetErrorMode(TRUE);
                VolumeLabelGet(cDrive,lpDiskCache->szVolumeLabel);
                SetErrorMode(wPrevError);
                AnsiLower(lpDiskCache->szVolumeLabel);
                lpDiskCache->wFlags |= DISKCACHE_LABELREAD;
            }
        }
    }


    return (NOERR);
} /* DiskCacheSetInfo */


/************************************************************************/
/*
**  Section II:  Get Information
*/
/*--------------------------------------------------------------------------*/
/* DiskCacheGetInfo():                                                      */
/*    This routine returns the Drive Types and the Drive Mask for the       */
/*      specified drive letter.                                             */
/*    It replaces DiskGetType() since it is more efficent by using the      */
/*      cached values.                                                      */
/*                                                                          */
/* INPUTS                                                                   */
/*    char      cDrive          the drive we need information about.        */
/*    LPWORD    lpwMajor        on exit contains the major type.            */
/*    LPWORD    lpwMinor        on exit contains the minor type.            */
/*    LPWORD    lpwMask         on exit contains the drive mask.            */
/*    LPSTR     lpszVolLabel    on exit contains the volume label.          */
/*                                                                          */
/* RETURNS                                                                  */
/*    NOERR                                                                 */
/*--------------------------------------------------------------------------*/
/*  10/16/91: MW created                                                    */
/*--------------------------------------------------------------------------*/
WORD SYM_EXPORT WINAPI DiskCacheGetInfo(char   cDrive,
                                    LPWORD lpwMajor,
                                    LPWORD lpwMinor,
                                    LPWORD lpwMask,
                                    LPSTR  lpszVolLabel)
{
    LPDISKCACHE lpDiskCache;
    BOOL        bFound;
    int         i;

    DiskCacheInit();

    lpDiskCache = (LPDISKCACHE)MemLock(s_hDiskCache);

    /*
    **  Search for the specified drive letter.
    */
    bFound = FALSE;
    cDrive = CharToUpper(cDrive);
    for (i = 0; i < SYM_MAX_DOS_DRIVES; i++, lpDiskCache++)
        if (lpDiskCache->cDriveLetter == cDrive)
            {
            if (lpwMajor)
                *lpwMajor = lpDiskCache->wMajorType;
            if (lpwMinor)
                *lpwMinor = lpDiskCache->wMinorType;
            if (lpwMask)
                *lpwMask = lpDiskCache->wMask;
            if (lpszVolLabel)
            {
                if(!(lpDiskCache->wFlags & DISKCACHE_LABELREAD))
                    DiskCacheSetInfo(lpDiskCache->cDriveLetter - 'A',
                        lpDiskCache, DISKCACHE_LABELREAD);
                STRCPY(lpszVolLabel,lpDiskCache->szVolumeLabel);
            }
            bFound = TRUE;
            break;
            }

    MemUnlock( s_hDiskCache, lpDiskCache );

    if ( ! bFound )
        {
        if (lpwMajor)
            *lpwMajor = TYPE_UNKNOWN;
        if (lpwMinor)
            *lpwMinor = TYPE_UNKNOWN;
        if (lpwMask)
            *lpwMask  = NDRV_AVAILABLE;
        if (lpszVolLabel)
            lpszVolLabel[0] = EOS;
        }

    return (NOERR);
} /* DiskCacheGetInfo */


/************************************************************************/
/*
**  Section III:  Available Drives
*/
/*--------------------------------------------------------------------------*/
/* DiskCacheGetDrivesByMask():                                         */
/*    This routine is used to get the drive name strings by the given mask. */
/*    The result will be null-terminated.                                   */
/*    To get the number of available drives without getting the string,     */
/*    pass NULL as the first argument, for example:                         */
/*          nTotal = DiskCacheGetDrivesByMask( NULL, NDRV_AVAILABLE );      */
/*                                                                          */
/* INPUTS                                                                   */
/*    LPSTR lpDrvNames  Buffer to store the result.                         */
/*    MASK  wMask       Mask for drives needed.                             */
/*                                                                          */
/* RETURNS                                                                  */
/*      number of drives currently available                                */
/*--------------------------------------------------------------------------*/
int SYM_EXPORT WINAPI DiskCacheGetDrivesByMask(LPSTR lpDrvNames, WORD wMask)
{
    char    szAvailDrvNames[SYM_MAX_DOS_DRIVES + 1];

    if ( lpDrvNames == NULL )
        lpDrvNames = szAvailDrvNames;

    DiskCacheInit();

    if ( wMask == NDRV_AVAILABLE )
        STRCPY( lpDrvNames, s_szAvailDrvNames );
    else
        {
        int     nAvailable;
        LPSTR   lpCurrent = lpDrvNames;

        for (nAvailable = 0; s_szAvailDrvNames[nAvailable]; nAvailable++)
            if (s_szAvailDrvMasks[nAvailable] & wMask)
                *lpCurrent++ = s_szAvailDrvNames[nAvailable];
        *lpCurrent = EOS;
        }

    return( STRLEN(lpDrvNames) );
} /* DiskCacheGetDrivesByMask */


//------------------------------------------------------------------------
// DiskCacheReconcileChanges
//
// This routine is called to reconcile the changes that have occured
// in the available drive list with what is specified in the selected
// drive list.  If something has connected or disconnected drives via
// a new drive connection dialog box (like the one in Windows 3.1 for
// novell) then our selected drive list is not correct any more.
//
// We have to compare both the available drive list with the selected
// drive list and then add or delete entries from the selected drive list.
//
// NOTE: We're assuming here that 's_szAvailDrvNames[]' is up-to-date with
//       a more current image of what's available than the 'lpszOldDrvList'.
//
// Returns: nothing
//
//------------------------------------------------------------------------
VOID SYM_EXPORT WINAPI DiskCacheReconcileChanges (
   LPSTR   lpszOldDrvList,              // (INPUT) Previous s_szAvailDrvNames
                                        //         list.
   RECONCILECALLBACK fpCallBack         // (INPUT) Call back function to
                                        //         call with changes to make!
)
{
   auto  BOOL  bOldDrive;
   auto  BOOL  bNewDrive;
   auto  LPSTR lpszNewDrvList;
   auto  int   i;

   lpszNewDrvList = s_szAvailDrvNames;

                                        // Loop through all the possible
                                        // drives and try to figure out what
                                        // changed...
   for (i = 0; i < SYM_MAX_DOS_DRIVES; ++i) {

                                        // assume no drive
      bOldDrive = bNewDrive = FALSE;

      if (*lpszOldDrvList == (char)(i+'A')) {
         bOldDrive = TRUE;              // Yes, this drive was valid...
         ++lpszOldDrvList;
      }

      if (*lpszNewDrvList == (char)(i+'A')) {
         bNewDrive = TRUE;              // Yes, this drive is still valid...
         ++lpszNewDrvList;
      }

                                        // If there was a drive, and now
                                        // there isn't, then delete it!
      if (bOldDrive && !bNewDrive) {
            (*fpCallBack)((char)(i+'A'),FALSE);
      }

                                        // If there wasn't a drive, and now
                                        // there is, then add it!
      if (!bOldDrive && bNewDrive) {
            (*fpCallBack)((char)(i+'A'),TRUE);
      }
   }
} /* DiskCacheReconcileChanges */



//------------------------------------------------------------------------
// DiskCacheTerminate
//
// This function is called by the WEP function to free the allocated disk
// type cache memory.
//------------------------------------------------------------------------
VOID SYM_EXPORT WINAPI DiskCacheTerminate(VOID)
{
    if (s_hDiskCache != NULL)
	{
	MemRelease(s_hDiskCache);
	MemFree( s_hDiskCache );
	s_hDiskCache = NULL;
	}
} /* DiskCacheTerminate */

