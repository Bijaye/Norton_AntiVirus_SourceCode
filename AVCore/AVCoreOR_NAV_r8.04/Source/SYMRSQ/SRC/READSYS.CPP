/* Copyright 1993 Symantec Corporation                                  */
/*----------------------------------------------------------------------*
 *                                                                      *
 * $Header:   S:/SYMRSQ/VCS/readsys.cpv   1.0   07 May 1996 17:09:04   DALLEE  $ *
 *                                                                      *
 * Description:                                                         *
 *   Reads the system files for the source system for SYMFMT.DLL        *
 *                                                                      *
 * Contains:                                                            *
 *                                                                      *
 * See Also:                                                            *
 *                                                                      *
 *----------------------------------------------------------------------*
 * $Log:   S:/SYMRSQ/VCS/readsys.cpv  $ *
// 
//    Rev 1.0   07 May 1996 17:09:04   DALLEE
// Initial revision.
// 
//    Rev 1.29.2.1   17 Jul 1995 22:39:42   GEORGEL
// Removed temporary comments
// 
//    Rev 1.29.2.0   27 Jun 1995 18:46:06   RJACKSO
// Branch base for version QAK8
// 
//    Rev 1.29   11 Apr 1995 17:42:40   Basil
// Merge of quake 6 changes
// 
//    Rev 1.28.1.2   20 Mar 1995 11:55:30   GEORGEL
// Fixed 32767
// 
//    Rev 1.28.1.1   03 Feb 1995 16:57:14   MARKK
// Removed all thunk stuff
// 
//    Rev 1.28.1.0   13 Jan 1995 20:22:04   SCOTTP
// Branch base for version QAK6
// 
//    Rev 1.28   17 Nov 1994 02:18:44   Basil
// Thunking changes
//
//    Rev 1.27   07 Oct 1994 15:07:06   BRAD
// Added NEC support
//
//    Rev 1.26   18 Sep 1994 17:00:02   GEORGEL
// Kludged it to work with SYMKRNL
//
//    Rev 1.25   31 May 1994 19:52:36   DAVID
// Fixed bug in drvspace.bin code.
//
//    Rev 1.24   17 May 1994 15:45:34   DAVID
// Added support for drvspace.bin (DOS 6.22)
// Don't use AnsiUpper() to uppercase a single character in DOS.
// Fixed MemUnlock() macro problem in DOS version.
// Don't call GetActiveWindow() in DOS version.
//
//    Rev 1.23   05 May 1994 20:04:50   BRAD
// Removed 2nd parameter from MemUnlock()
//
//    Rev 1.22   04 May 1994 01:10:44   BRAD
// Don't use NFileOpen()
//
//    Rev 1.21   03 May 1994 19:20:44   BRAD
// MemUnlock missing 2nd param
//
//    Rev 1.20   31 Mar 1994 11:10:26   BRAD
// Fixed typo
//
//    Rev 1.19   26 Mar 1994 23:39:32   BRAD
// Use portable windows messages
//
//    Rev 1.18   26 Mar 1994 14:28:20   BRAD
// New date/time
//
//    Rev 1.17   16 Mar 1994 11:33:52   BRAD
// Remove compiler warnings
//
//    Rev 1.16   15 Mar 1994 17:34:50   BRAD
// Cleaned up for WIN32
//
//    Rev 1.15   15 Mar 1994 11:09:00   BRUCE
// Changed EXPORT to SYM_EXPORT
//
//    Rev 1.14   12 Mar 1994 18:50:02   BRAD
// Cleaned up for WIN32
//
//    Rev 1.13   25 Feb 1994 15:21:38   BRUCE
// Removed instring.h and universl.h
//
//    Rev 1.12   25 Feb 1994 13:00:10   BRUCE
// Ran scripts to change to SYM_ constants and get ready for Win32
//
//    Rev 1.11   11 Feb 1994 19:00:26   BRAD
// Upgraded from QAK2.
//
//    Rev 1.10   08 Dec 1993 17:44:36   DAVID
// Created a DOS version of SYMFMTx.LIB containing SFReportSystemFiles().
//
//    Rev 1.9   24 Oct 1993 21:38:06   BASIL
// Use non-compressed boot drive when searching for system files.
//
//    Rev 1.8   21 Oct 1993 23:31:28   BASIL
// [FIX] Fixed the index to dblspace.bin in SYSTEM_FILES[] when searching for host
//
//    Rev 1.7   21 Oct 1993 20:30:12   BASIL
// Set doublespace flag when reporting system files.
//
//    Rev 1.6   14 Oct 1993 13:04:14   BASIL
// [FIX] if dblspace.bin is not found on boot drive, try looking
// for it on the host drive [10843,11108]
//
//    Rev 1.5   16 Sep 1993 22:50:00   DAVID
// Added search for command.com in path if not found in root.
// Added specific error message for cases where the system files
// can't be read (not found, out of memory, read errors, etc.)
//
//    Rev 1.4   07 Sep 1993 15:18:24   DAVID
// [FIX] GetOriginalDisk() was displaying an empty message box due to
// an uninitialized string buffer.  Also, made sure that all message
// boxes have an icon.
//
//    Rev 1.3   28 Jun 1993 20:27:04   BASIL
// Do not set bValidSysInfo if not reading system files.
//
//    Rev 1.2   25 Jun 1993 15:41:24   BASIL
// Added SFReportSystemFiles() to be used by rescue.
//
//    Rev 1.1   25 Jun 1993 11:26:20   BASIL
// Added support for dblspace.bin
//
//    Rev 1.0   17 Jun 1993 18:10:44   BASIL
// Initial revision.
 *----------------------------------------------------------------------*/

#include "platform.h"
//#include <windows.h>
#include <memory.h>
#include <string.h>
#include "xapi.h"

#include "undoc.h"
#include "disk.h"
#include "file.h"
#include "msgdisp.h"
#include "symsys.h"

#include "symfmt.h"
#include "format.h"
#include "ngui.h"                    /* Definitions for fill bar stuff            */

#include "formatst.h"


/*----------------------------------------------------------------------*/
/* Local Prototypes                                                     */
/*----------------------------------------------------------------------*/
BOOL LOCAL TestableDrive(WORD wOffsetSoFar, LPBYTE byBootDriveTable);
WORD LOCAL GetASystem(BYTE dl, BOOL bReadFiles);
WORD LOCAL ReadSystemFile(WORD fileNameIndex,
                          SFSysFileRec *sysFilePtr,
                          WORD wFileType,
                          BOOL bReadFiles);
WORD LOCAL ReadBootRecord(BYTE dl);
BOOL LOCAL UseDriveA(BYTE dl);
VOID LOCAL GetOriginalDisk(BOOL usingDriveA);


BOOL WINAPI NavFmt_CanSupportExtendedIO();  // Kluge in system.cpp

char COMSPEC_STR [] = "COMSPEC";


/*----------------------------------------------------------------------*/
/* UseDriveA ()                                                         */
/*        Prompts to insert a system disk into drive A: (or a specific  */
/*        drive (NEC).                                                  */
/*                                                                      */
/*        Returns TRUE if using drive A, else FALSE                     */
/*----------------------------------------------------------------------*/


#ifdef WIN16
//#ifdef SYM_WIN

BOOL LOCAL UseDriveA (BYTE dl)
{
    auto WORD  wResult;
    auto char  zCaption[MAX_CAPTION];
    auto char  zScratch[MAX_SCRATCH];
    auto char  szDrive[10];


    szDrive[0] = dl;
    szDrive[1] = EOS;

    NMsgGet(hinst_SYMFMT,
        FORMAT_INSERT_DOS_DISK,
        zScratch,
        MAX_SCRATCH,
        (LPSTR) szDrive);

    NStringLoad(FORMAT_CAPTION, zCaption, MAX_CAPTION);

    wResult = MessageDisplay(
        hFillDialog,
        zScratch,
        zCaption,
        MB_OKCANCEL | MB_ICONINFORMATION,
        NULL,
        NULL);

    if(wResult == IDOK)
    {
        return(TRUE);
    }
    else
    {
        return(FALSE);
    }
}

#else

BOOL LOCAL UseDriveA(BYTE dl)
{
    return (FALSE);
}

#endif

/*----------------------------------------------------------------------*/
/* GetOriginalDisk                                                      */
/*        If using DOS diskette from drive A:, then prompt for original */
/*        diskette we are formatting.                                   */
/*----------------------------------------------------------------------*/

#ifdef SYM_WIN

VOID LOCAL GetOriginalDisk (BOOL usingDriveA)
{
    extern DiskRec ExternDiskRec;
    auto   WORD    wResult;
    auto   BOOL    bHWIsNEC;
    auto   char    zCaption[MAX_CAPTION];
    auto   char    zScratch[MAX_SCRATCH];
    auto   char    szDrive[10];

                                        /* If formatting drive A:, but  */
                                        /*   system disk is currently   */
                                        /*   in drive A:, prompt for    */
                                        /*   original disk              */
    bHWIsNEC = HWIsNEC();
    if (((!bHWIsNEC && (ExternDiskRec.dl == 'A')) || bHWIsNEC) && usingDriveA)
        {
        szDrive[0] = ExternDiskRec.dl;
        szDrive[1] = EOS;
        NMsgGet(hinst_SYMFMT,
                FORMAT_INSERT_ORIGINAL_DISK,
                zScratch,
                MAX_SCRATCH,
                szDrive);

        NStringLoad(FORMAT_CAPTION, zCaption, MAX_CAPTION);

        wResult = MessageDisplay(
            hFillDialog,
            zScratch,
            zCaption,
            MB_OKCANCEL | MB_ICONINFORMATION,
            NULL,
            NULL);
        }
}

#else

VOID LOCAL GetOriginalDisk (BOOL usingDriveA)
{
}

#endif

/*----------------------------------------------------------------------*
 *                                                                      *
 * Description:                                                         *
 *   Determines if dblspace.bin or stacker.bin is needed by checking    *
 *   the operating system version number.                               *
 *   It also sets up the system filenames table entry for dblspace.bin. *
 *                                                                      *
 * Parameters:                                                          *
 *   NONE                                                               *
 *                                                                      *
 * Return Value:                                                        *
 *   TRUE if either dblspace or stacker files are needed                *
 *   FALSE if neither file is needed                                    *
 *                                                                      *
 * See Also:                                                            *
 *                                                                      *
 *----------------------------------------------------------------------*
 * 03/08/1994 BASIL Function Created.                                   *
 *----------------------------------------------------------------------*/

BOOL SYM_EXPORT WINAPI SFIsDblSpaceNeeded (VOID)
{
    extern   char         *SYSTEM_FILENAMES [];
    extern   char         szDBLSPACE_BIN[];
    extern   char         szSTACKER_BIN[];
    extern   char         szDRVSPACE_BIN[];
    WORD     wDblSpaceEntry;
    BOOL     bRetVal;
                                        //-------------------------------
                                        // Find the Doublespace.bin entry
                                        // in the SYSTEM_FILENAMES table
                                        //-------------------------------
    for (wDblSpaceEntry=0 ;
         SYSTEM_FILENAMES[wDblSpaceEntry] != NULL ;
         wDblSpaceEntry++)
        {
        }
    wDblSpaceEntry--;

                                        //-------------------------------
                                        // Check DR-DOS first
                                        //-------------------------------
    if (DOSGetDRDOSVersion() >= 0x0700)
        {
        SYSTEM_FILENAMES[wDblSpaceEntry] = szSTACKER_BIN;
        bRetVal = TRUE;
        }
                                        //-------------------------------
                                        // Check MS-DOS version
                                        //-------------------------------
    else if (DOSGetVersion() >= DOS_6_22)
        {
        SYSTEM_FILENAMES[wDblSpaceEntry] = szDRVSPACE_BIN;
        bRetVal = TRUE;
        }
    else if (DOSGetVersion() >= DOS_6_00)
        {
        SYSTEM_FILENAMES[wDblSpaceEntry] = szDBLSPACE_BIN;
        bRetVal = TRUE;
        }
    else
                                        //-------------------------------
                                        // Dblspace.bin not needed
                                        //-------------------------------
        bRetVal = FALSE;

    return (bRetVal);
}

/*----------------------------------------------------------------------*
 *                                                                      *
 * Description:                                                         *
 *                                                                      *
 * Parameters:                                                          *
 *                                                                      *
 * Return Value:                                                        *
 *                                                                      *
 * See Also:                                                            *
 *                                                                      *
 *----------------------------------------------------------------------*
 * 06/25/1993 BASIL Function Created.                                   *
 *----------------------------------------------------------------------*/

WORD SYM_EXPORT WINAPI  SFReportSystemFiles (LPSFSysInfoRec lpSysRec)
{
    extern   SFSysInfoRec  ExternSysInfo;
    extern   SFStateRec    ExternState;

                                        //-------------------------------
                                        // Set hFillDialog when not running
                                        // wformat, to prevent
                                        // MessageDisplay() problems.
                                        //-------------------------------
#ifdef WIN16
    hFillDialog = GetActiveWindow();
#endif

    ExternState.bDblSpace = SFIsDblSpaceNeeded();

    if (ReadSystemFiles (FALSE) == NOERR)
        {
        *lpSysRec = ExternSysInfo;
        return(NOERR);
        }

    return (ERR);
}



/*----------------------------------------------------------------------*/
/* ReadSystemFiles                                                      */
/*      Reads the system information off the system disk, so that we    */
/*      have it ready.  The information will be written out at the very */
/*      end of the format routine.                                      */
/*                                                                      */
/*      The order to get the system files is as follows:                */
/*              1) Use DOS 4.0+ call to determine boot drive.           */
/*              2) Use COMSPEC                                          */
/*              3) Use drive C:                                         */
/*              4) Use drive A:, if nothing else.                       */
/*                                                                      */
/*                                                                      */
/*      Parameters:                                                     */
/*              bReadFile: if true files are read into buffers, if false*/
/*              externsysinfo is filled with info about sys files found.*/
/*      Returns:                                                        */
/*              NOERR                        if no error                */
/*              DISK_MEMORY_ERROR        if memory problem              */
/*              SYS_FILES_NOT_FOUND        if not found                 */
/*----------------------------------------------------------------------*/

WORD ReadSystemFiles (BOOL  bReadFiles)
{
    extern   DiskRec       ExternDiskRec;
    extern   SFSysInfoRec  ExternSysInfo;
    extern   SFStateRec    ExternState;
    register WORD          i;
    register WORD          err;
    auto     BOOL          usingDriveA;
    auto     BYTE          byBootDrive;
    auto     BYTE          byBootDriveTable[4];
    auto     char          szComspec [70];
    auto     WORD          wReturnValue;
    auto     BOOL          bHWIsNEC;





    bHWIsNEC = HWIsNEC();
    usingDriveA = FALSE;                /* Not using drive A: for system*/
                                        /*   files.                     */

                                        /*------------------------------*/
                                        /* Initialize sysfile records   */
                                        /*------------------------------*/
    for (i = 0; i < MAX_SYS_FILES; i++)
        {
        ExternSysInfo.sysFile[i].allocated = FALSE;

        if (i == COMMAND_COM_FILE)
            ExternSysInfo.sysFile[i].wAttributes = FA_NORMAL + FA_ARCHIVE;
        else // if (i == BIOS_COM_FILE || DOS_COM_FILE | DBLSPACE_FILE)
            ExternSysInfo.sysFile[i].wAttributes =
                       FA_HIDDEN + FA_SYSTEM + FA_READ_ONLY + FA_ARCHIVE;
        }

    ExternSysInfo.bootRecordAllocated = FALSE;

                                        /*------------------------------*/
                                        /* Get Boot drive letter        */
                                        /*------------------------------*/
    byBootDriveTable[0] = (BYTE) DiskGetRealBootDrive(BOOTDRIVE_NON_COMPRESSED);

                                        /*------------------------------*/
                                        /* Get COMSPEC string           */
                                        /*------------------------------*/
    DOSGetEnvironment(COMSPEC_STR, szComspec, 64);
#ifdef WIN16
    byBootDriveTable[1] = (BYTE) (DWORD) AnsiUpper ((LPSTR) (DWORD) szComspec[0]);
#else
    byBootDriveTable[1] = CharToUpper(szComspec[0]);
#endif

                                        /*------------------------------*/
                                        /* Use Drive C:                 */
                                        /*------------------------------*/
    byBootDriveTable[2] = (bHWIsNEC) ? ExternDiskRec.dl : 'C';
                                        /*------------------------------*/
                                        /* Use Drive A:                 */
                                        /*------------------------------*/
    byBootDriveTable[3] = 'A';
                                        /*------------------------------*/
                                        /* Check for System files       */
                                        /*------------------------------*/
    wReturnValue = SYS_FILES_NOT_FOUND;
    for (i = 0; i < 4; i++)
    {
        byBootDrive = byBootDriveTable[i];
                                        /*------------------------------*/
                                        /* Have we already tested this  */
                                        /* drive or is it an invalid    */
                                        /* drive?                       */
                                        /*------------------------------*/
        if (TestableDrive(i, (LPBYTE)byBootDriveTable))
        {
                                        /*------------------------------*/
                                        /* If drive A:, prompt user.    */
                                        /*------------------------------*/
            if ((!bHWIsNEC && byBootDrive == 'A') ||
                (bHWIsNEC && byBootDrive == ExternDiskRec.dl))
                {
                usingDriveA = UseDriveA(byBootDrive);
                if (!usingDriveA)
                    {
                    break;
                    }
                }

            DeAllocateSystemFiles();    /* Free memory                  */

                                        /* Get a System File Set     */
            err = GetASystem(byBootDrive, bReadFiles);

            if (err == NOERR)
            {
                wReturnValue = NOERR;
                break;
            }
                                        /* Memory error, return error   */
            if (err == ERROR_MEMORY)
            {
                wReturnValue = DISK_MEMORY_ERROR;
                break;
            }
        }
    }

    if (wReturnValue != NOERR)
    {
        DeAllocateSystemFiles ();       /* If error, free memory        */
    }

    GetOriginalDisk(usingDriveA);       /* Get original disk in drive A:*/

    return (wReturnValue);
}


/*----------------------------------------------------------------------*/
/* TestableDrive                                                        */
/*        Test to see if we have previously tested this drive.          */
/*----------------------------------------------------------------------*/
BOOL LOCAL TestableDrive(WORD wOffsetSoFar, LPBYTE byBootDriveTable)
{
    auto WORD i;
    auto BYTE byCurrentDrive;


    byCurrentDrive = byBootDriveTable[wOffsetSoFar];
    if (byCurrentDrive == 0)
        return(FALSE);

    for (i = 0; i < wOffsetSoFar; i++)
    {
        if (byCurrentDrive == byBootDriveTable[i])
            return(FALSE);
    }

    return(TRUE);
}



/*----------------------------------------------------------------------*/
/* GetASystem                                                           */
/*        Reads in the IBMBIO, IBMDOS, COMMAND.COM and the Boot Record  */
/*        from a bootable disk.                                         */
/*----------------------------------------------------------------------*/

WORD LOCAL GetASystem (BYTE dl, BOOL bReadFiles)
{
    extern   SFSysInfoRec ExternSysInfo;
    extern   SFStateRec   ExternState;
    extern   char         *SYSTEM_FILENAMES [];
    extern   char         szDBLSPACE_BIN[];
    extern   char         szSTACKER_BIN[];
    extern   char         szDRVSPACE_BIN[];
    register WORD         i;
    register int          err;
    auto     int          index;
    auto     WORD         systemSets;
    auto     char         cBootDrive;


    ExternSysInfo.wSysFileCount = 0;

                                        /*------------------------------*/
                                        /* Set drive letter for SYS     */
                                        /* files                        */
                                        /*------------------------------*/
    for (i = 0; SYSTEM_FILENAMES[i]; i++)
        SYSTEM_FILENAMES [i][0] = dl;

    systemSets = (i - 2) >> 1;
                                        /*------------------------------*/
                                        /* Read in BIO and DOS system   */
                                        /* files.                       */
                                        /*------------------------------*/
    for (i = 0; i < systemSets; i++)
    {
        index = i << 1;
                                        /*------------------------------*/
                                        /* Try getting BIO file first   */
                                        /*------------------------------*/
        err = ReadSystemFile(index,
                             &(ExternSysInfo.sysFile [BIOS_COM_FILE]),
			     BIOS_COM_FILE,
                             bReadFiles);

                                        /*------------------------------*/
                                        /* If error, then can't use     */
                                        /* this file.                   */
                                        /*------------------------------*/
        if (err != NOERR)
        {
            DeAllocateSystemFiles();
            continue;
        }
                                        /*------------------------------*/
                                        /* Try reading DOS system file  */
                                        /*------------------------------*/
        err = ReadSystemFile ( index + 1,
                               &(ExternSysInfo.sysFile[DOS_COM_FILE]),
			       DOS_COM_FILE,
                               bReadFiles);

                                        /*------------------------------*/
                                        /* If error, then can't use     */
                                        /* this file.                   */
                                        /*------------------------------*/
        if (err != NOERR)
        {
            DeAllocateSystemFiles();
            continue;
        }
        else
        {
            break;                      /* No error, break from loop    */
        }
    }

                                        /*------------------------------*/
                                        /* If no error, read COMMAND.COM*/
                                        /* and boot record.             */
                                        /*------------------------------*/

    if (err == NOERR)                   /* If bad read, then return     */
    {
                                        /*------------------------------*/
                                        /* Try reading COMMAND.COM      */
                                        /*         system file          */
                                        /*------------------------------*/
        err = ReadSystemFile (systemSets << 1,
                              &(ExternSysInfo.sysFile [COMMAND_COM_FILE]),
	                      COMMAND_COM_FILE,
                              bReadFiles);

                                        //-------------------------------
                                        // Read dblspace.bin, if requested
                                        //-------------------------------

        if (err == NOERR)
            {

                                        // 3 files read so far
            ExternSysInfo.wSysFileCount = 3;

            if (ExternState.bDblSpace)
                {
                err = ReadSystemFile ((systemSets << 1) + 1,
                                      &(ExternSysInfo.sysFile [DBLSPACE_FILE]),
	                              DBLSPACE_FILE,
                                      bReadFiles);

                                        //-------------------------------
                                        // If at first you fail, try the
                                        // host drive, it may have been
                                        // swapped (#10843)
                                        //-------------------------------
                if (err != NOERR)
                    {
                                        //-------------------------------
                                        // Save boot drive, in case we
                                        // switch from stacker.bin to
                                        // dblspace.bin
                                        //-------------------------------
                    cBootDrive = *SYSTEM_FILENAMES[(systemSets << 1) + 1];

                    DiskGetHostDrive(SYSTEM_FILENAMES[(systemSets << 1) + 1]);
                    err = ReadSystemFile ((systemSets << 1) + 1,
                                          &(ExternSysInfo.sysFile [DBLSPACE_FILE]),
	                                  DBLSPACE_FILE,
                                          bReadFiles);
                    }

                                        //-------------------------------
                                        // If we failed searching for
                                        // stacker.bin or drvspace.bin,
                                        // try dblspace.bin
                                        //-------------------------------
                if ( err != NOERR &&

                     ( (DOSGetDRDOSVersion() >= 0x0700 &&
                        SYSTEM_FILENAMES[(systemSets << 1) + 1] == szSTACKER_BIN)

		       ||

                       (DOSGetVersion() >= DOS_6_22 &&
                        SYSTEM_FILENAMES[(systemSets << 1) + 1] == szDRVSPACE_BIN)
		     )
		   )
                    {
                                        //-------------------------------
                                        // Recover original boot drive
                                        // and look for dblspace.bin
                                        //-------------------------------
                    SYSTEM_FILENAMES[(systemSets << 1) + 1] = szDBLSPACE_BIN;
                    *SYSTEM_FILENAMES[(systemSets << 1) + 1] = cBootDrive;

                    err = ReadSystemFile ((systemSets << 1) + 1,
                                          &(ExternSysInfo.sysFile [DBLSPACE_FILE]),
                                          DBLSPACE_FILE,
                                          bReadFiles);

                                        //-------------------------------
                                        // Boot drive was swapped ?
                                        //-------------------------------
                    if (err != NOERR)
                        {
                        DiskGetHostDrive(SYSTEM_FILENAMES[(systemSets << 1) + 1]);
                        err = ReadSystemFile ((systemSets << 1) + 1,
                                              &(ExternSysInfo.sysFile [DBLSPACE_FILE]),
                                              DBLSPACE_FILE,
                                              bReadFiles);
                        }
                    }

                if (err == NOERR)
                    ExternSysInfo.wSysFileCount = 4;
                else
                    err = NOERR;        // Failed, continue anyway
                }
            }

        if (err == NOERR)
        {
                                        /*------------------------------*/
                                        /* Read the boot record from    */
                                        /*        the drive             */
                                        /*------------------------------*/
            err = ReadBootRecord (dl);

            if (err == NOERR && bReadFiles)
            {                           /* We have valid system info    */
                ExternSysInfo.validSysInfo = TRUE;
            }
        }
    }
                                        /*------------------------------*/
                                        /* Deallocate system files, if  */
                                        /* error                        */
                                        /*------------------------------*/
    if (err != NOERR)
        DeAllocateSystemFiles();

    return (err);                       /* Return the exact error type  */
}



/*----------------------------------------------------------------------*/
/* ReadSystemFile                                                       */
/*        Opens, reads, and closes the system file passed as the        */
/*        parameter, 'fileNameIndex'. Saves info in SysFilePtr          */
/*        wFileType is one of:                                          */
/*            BIOS_COM_FILE                                             */
/*            DOS_COM_FILE                                              */
/*            COMMAND_COM_FILE                                          */
/*            DBLSPACE_FILE                                             */
/*                                                                      */
/*        Returns:                                                      */
/*                ERROR_NOT_FOUND  if file not found                    */
/*                ERROR_OPENING    if error opening file                */
/*                ERROR_MEMORY     if memory error                      */
/*                ERROR_READING    if error reading file                */
/*                NOERR            if no error                          */
/*----------------------------------------------------------------------*/

WORD LOCAL ReadSystemFile (WORD fileNameIndex,
                           SFSysFileRec *sysFilePtr,
                           WORD wFileType,
                           BOOL  bReadFiles)
{
    extern char         *SYSTEM_FILENAMES [];
    auto   HFILE        handle;
    auto   WORD         date;
    auto   WORD         time;
    auto   HGLOBAL      hBuffer;
    auto   HPBYTE       hpBuffer;
    auto   OFSTRUCT     of;
    auto   WORD         returnValue;
    auto   FILETIME     fileTime;



    returnValue = NOERR;
                                        /*------------------------------*/
                                        /* Open system file             */
                                        /*------------------------------*/
    handle = OpenFile (SYSTEM_FILENAMES [fileNameIndex] , &of,
      OF_SHARE_COMPAT + OF_READ);

                                        /*------------------------------*/
                                        /* If COMMAND.COM not found in	*/
					/* root, search the path. We 	*/
					/* don't use COMSPEC because we	*/
					/* aren't prepared to handle	*/
					/* non COMMAND.COM shells (which*/
					/* requires adding a CONFIG.SYS	*/
					/* file with a SHELL= line).	*/
					/* Also Rescue wouldn't handle 	*/
					/* the case where the fileName	*/
					/* path is wrong and the file	*/
					/* is not in the path, current	*/
					/* dir, etc.			*/
                                        /*------------------------------*/
    if (handle == HFILE_ERROR &&
	of.nErrCode == 2 &&
        wFileType == COMMAND_COM_FILE)
	{
#if defined(SYM_WIN32)
        {
        char    szFile[SYM_MAX_PATH];
        LPTSTR  lpFilePart;
                                        // Don't use 'x:\' portion of fname
        if ( SearchPath(NULL, SYSTEM_FILENAMES[fileNameIndex] + 3, NULL,
                         sizeof(szFile), szFile, &lpFilePart))
                handle = OpenFile(szFile, &of, OF_SHARE_COMPAT + OF_READ);
        else
                handle = HFILE_ERROR;
        }
#else
	handle = OpenFile (SYSTEM_FILENAMES [fileNameIndex], &of,
				   OF_SHARE_COMPAT + OF_READ + OF_SEARCH);
#endif
	}

    if (handle == HFILE_ERROR)
	return ((of.nErrCode == 2) ? ERROR_NOT_FOUND : ERROR_OPENING);

                                        /*------------------------------*/
                                        /* Save ptr. to filename (as    */
					/* used on destination disk, NOT*/
					/* necessarily where found)	*/
                                        /*------------------------------*/
    sysFilePtr->fileName = SYSTEM_FILENAMES [fileNameIndex];

                                        /*------------------------------*/
                                        /* Get date/time of file        */
                                        /*------------------------------*/
    FileGetTime(handle, TIME_LASTWRITE, &fileTime);
    FileTimeToDosTime(&fileTime, &date, &time);

                                        /*------------------------------*/
                                        /* Save date/time               */
                                        /*------------------------------*/
    sysFilePtr->date = date;
    sysFilePtr->time = time;
                                        /*------------------------------*/
                                        /* Get size of file             */
                                        /*------------------------------*/
    sysFilePtr->size = FileSeek (handle, 0L, 2);


                                        //-------------------------------
                                        // If reporting-only, we're done
                                        //-------------------------------
    if (!bReadFiles)
        {
        FileClose(handle);
        return (NOERR);
        }

                                        /*------------------------------*/
                                        /* Allocate space for the       */
                                        /* system file data.            */
                                        /*------------------------------*/
    hBuffer = MemAlloc(GMEM_MOVEABLE, sysFilePtr->size);
    if (hBuffer == NULL)
    {
memoryError:
        FileClose(handle);
        return(ERROR_MEMORY);
    }
                                        /*------------------------------*/
                                        /* Lock the buffer              */
                                        /*------------------------------*/
    hpBuffer = (HPBYTE) MemLock(hBuffer);
    if ( hpBuffer == NULL )
    {
        MemFree(hBuffer);
        goto memoryError;
    }
                                        /*------------------------------*/
                                        /* Point to start of file       */
                                        /*------------------------------*/
    FileSeek (handle, 0L, 0);

    sysFilePtr->allocated = TRUE;       /* Mark as allocated            */
    sysFilePtr->hBuffer = hBuffer;      /* Save buffer handle           */

                                        /*------------------------------*/
                                        /* Read file data               */
                                        /*------------------------------*/
    if (FileHugeRead ( handle, hpBuffer, sysFilePtr->size) != sysFilePtr->size)
        returnValue = ERROR_READING;

#ifdef WIN16
    MemUnlock(hBuffer);
#else
    MemUnlock(hBuffer, (LPVOID FAR &)hpBuffer);
#endif
                                        /*------------------------------*/
                                        /* Close the system file        */
                                        /*------------------------------*/
    FileClose(handle);

    return(returnValue);
}

/*----------------------------------------------------------------------*/
/* ReadBootRecord ()                                                    */
/*      Reads the boot record and checks to see if it is valid.         */
/*                                                                      */
/*      Returns:                                                        */
/*              ERROR_MEMORY                if memory error             */
/*              ERROR_READING               if error reading file       */
/*              NOERR                       if no error                 */
/*----------------------------------------------------------------------*/

WORD LOCAL ReadBootRecord (BYTE dl)
{
    extern  SFSysInfoRec    ExternSysInfo;
    auto    GLOBALHANDLE    hBuffer;
    auto    LPBYTE          lpBuffer;
    auto    WORD            returnValue;
    auto    DiskRec         bootDisk;


                                        // Major NAV kluge alert:
                                        //-------------------------------
                                        // If extended IO supported
                                        // (possible FAT32 system) we
                                        // always write the pre-canned
                                        // boot sector. Return success
                                        // here, but don't read anything.
    if (NavFmt_CanSupportExtendedIO())
        {
        return (NOERR);
        }

                                        /*------------------------------*/
                                        /* Get disk Info                */
                                        /*------------------------------*/
    if (!DiskGetInfo(dl, &bootDisk))
        return(ERROR_READING);
                                        /*------------------------------*/
                                        /* Allocate space to hold a     */
                                        /* boot record.                 */
                                        /*------------------------------*/
    hBuffer = MemAlloc(GMEM_MOVEABLE, bootDisk.bytesPerSector);
    if (hBuffer == NULL)
        return(ERROR_MEMORY);

    lpBuffer = (LPBYTE)MemLock(hBuffer);
    if (lpBuffer == NULL)
        {
        MemFree(hBuffer);
        return(ERROR_MEMORY);
        }

    ExternSysInfo.bootRecordAllocated = TRUE;
    ExternSysInfo.hBootBuffer = hBuffer;

    returnValue = ERROR_READING;

    bootDisk.sector = 0;
    bootDisk.sectorCount = 1;
    bootDisk.buffer = lpBuffer;
    if (!DiskBlockDevice (BLOCK_READ, &bootDisk))
        {
                                        /*------------------------------*/
                                        /* Make sure boot is valid      */
                                        /*                              */
                                        /* Signature must be there and  */
                                        /* Number of Fats must be <= 2  */
                                        /*------------------------------*/
        if ((HWIsNEC() || (*((LPWORD)(lpBuffer + 510)) == (WORD) 0xAA55)) &&
            (lpBuffer [0x10] <= 2 ))
            {                           /* Make sure JMP is correct     */
            if ((*lpBuffer == 0xE9) ||
                (*lpBuffer == 0xEB && lpBuffer [2] == 0x90))
                returnValue = NOERR;
            }
        }

    return(returnValue);
}



/*----------------------------------------------------------------------*/
/* DeAllocateSystemFiles                                                */
/*        Frees up memory used by the system files.                     */
/*                                                                      */
/*        Returns nothing                                               */
/*----------------------------------------------------------------------*/

VOID DeAllocateSystemFiles (VOID)
{
    extern   SFSysInfoRec ExternSysInfo;
    register WORD         i;
    auto     SFSysFileRec *sysFilePtr;

                                        /*------------------------------*/
                                        /* If any files allocated,      */
                                        /*        deallocate them       */
                                        /*------------------------------*/
    sysFilePtr = &(ExternSysInfo.sysFile [0]);
    for (i = 0; i < MAX_SYS_FILES; i++, sysFilePtr++)
    {
        if (sysFilePtr->allocated)
        {
            MemFree(sysFilePtr->hBuffer);
            sysFilePtr->allocated = FALSE;
        }
    }

    if (ExternSysInfo.bootRecordAllocated)
    {
        MemFree(ExternSysInfo.hBootBuffer);
        ExternSysInfo.bootRecordAllocated = FALSE;
    }

    ExternSysInfo.validSysInfo = FALSE;
}

