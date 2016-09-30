// Copyright 1994 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/NAVXUTIL/VCS/startup.cpv   1.1   18 Feb 1998 17:25:04   mdunn  $
//
// Description:
//
// WARNING: This is cross compiled in the LUIGI:INST32 project.
//          Avoid adding additional dependencies until Dave Allee
//          fixes this mess.
//
// Contains:
//
// See Also:
//************************************************************************
// $Log:   S:/NAVXUTIL/VCS/startup.cpv  $
// 
//    Rev 1.1   18 Feb 1998 17:25:04   mdunn
// Ported NEC stuff from ROSNEC.
// 
//    Rev 1.0   06 Feb 1997 21:05:24   RFULLER
// Initial revision
// 
//    Rev 1.6   23 Jan 1997 13:43:04   JBRENNA
// Use DiskGetBootDrive() instead of DiskGetFirstFixedDrive() when looking for
// the boot disk. On NEC machines, the DiskGetFirstFixedDrive can return the
// wrong boot drive letter.
// 
//    Rev 1.5   10 Dec 1996 15:59:34   JBRENNA
// CreateSubstitutionArray(): No longer hard code the "C:\" as the boot drive.
// We now lookup the first drive letter that is a fixed disk. On NEC machines
// the A: drive can be the hard drive.
// 
//    Rev 1.4   15 Oct 1996 17:25:38   JBRENNA
// Ensure that when ReadNavStartFromOpenFile() is called, the returned
// startup options always have bLoadTSR set FALSE. This is because
// Kirin and beyond no longer have the TSR included in the product.
// 
//    Rev 1.3   17 Jul 1996 10:57:08   jworden
// Change WriteNavStartToDir () to create NAVSTART.DAT file if it doesn't
// already exist.
// 
//    Rev 1.2   27 Jun 1996 16:24:50   MKEATIN
// No change.
// 
//    Rev 1.1   04 Apr 1996 17:32:08   YVANG
// Ported from Kirin.
// 
//    Rev 1.1   03 Apr 1996 15:25:34   YVANG
// DBCS enable.
// 
//    Rev 1.0   30 Jan 1996 15:56:38   BARRY
// Initial revision.
// 
//    Rev 1.0   30 Jan 1996 14:51:40   BARRY
// Initial revision.
// 
//    Rev 1.9   29 Jun 1995 10:12:58   BARRY
// Fix FileRead() bug introduced in previous rev (duh)
// 
//    Rev 1.8   28 Jun 1995 22:07:02   BARRY
// Check FileRead against number of bytes expected -- not HFILE_ERROR
// 
//    Rev 1.7   28 Jun 1995 13:52:34   KEITH
// Return an error rather than improperly create a NAVSTART.DAT 
// file. The information to create the file is not currently 
// available to NAV. It is included in the install files.
// 
//    Rev 1.6   17 Apr 1995 15:47:18   MFALLEN
// sym_max_path+1
// 
//    Rev 1.5   28 Feb 1995 11:23:54   DALLEE
// Added routines to read/write lists of system files in NAVSTART.DAT.
// Read is non-VXD platforms.  Write is WIN platform only.
// 
//    Rev 1.4   01 Feb 1995 17:31:26   RSTANEV
// ReadNavStartFromOpenFile() now returns BOOL.
//
//    Rev 1.3   01 Feb 1995 16:54:40   RSTANEV
// Not defining some of the functions for VxD platform.
//
//    Rev 1.2   01 Feb 1995 15:51:50   RSTANEV
// Added ReadNavStartFromOpenFile().
//
//    Rev 1.1   23 Jan 1995 17:43:28   DALLEE
// Moved archive from NAVBOOT area and renamed from .C to .CPP.
// Changed function names from NavBootXXX to NavStartXXX.
//
//    Rev 1.0   19 Dec 1994 13:39:04   DALLEE
// Initial revision.
//************************************************************************

#include "platform.h"
#include "xapi.h"
#include "file.h"

#include "options.h"
#include "navutil.h"
#include "tsr.h"
#include "tsrcomm.h"
#include "stbexts.h"
#include "disk.h"

//************************************************************************
// LOCAL PROTOTYPES
//************************************************************************

#if !defined(SYM_VXD)
  STATUS LOCAL PASCAL MoveTailOfFile ( HFILE  hFile,
                                       DWORD  dwNewOffset,
                                       DWORD  dwStartOffset );
  DWORD LOCAL PASCAL SeekToStartupBlock ( HFILE  hFile,
                                          DWORD  dwBlockID );
  DWORD LOCAL PASCAL SeekToStartupBlockData ( HFILE  hFile,
                                              DWORD  dwBlockID );
#endif  // !SYM_VXD

#if defined(SYM_WIN)
  STATUS LOCAL PASCAL CreateSubstitutionArray ( LPHGLOBAL lphList );
  STATUS LOCAL PASCAL CreateSubstitutionFileList ( LPHGLOBAL  lphList,
                                                   LPDWORD    lpdwNumEntries,
                                                   LPDWORD    lpdwSizeEntries );
  STATUS LOCAL PASCAL CreateModifiedFileList ( LPHGLOBAL  lphList,
                                               LPDWORD    lpdwNumEntries,
                                               LPDWORD    lpdwSizeEntries );
  STATUS LOCAL PASCAL CreateModifiedEntry (
                              LPSTR           lpszModified,
                              LPCSTR          lpszStatic,
                              char      (FAR *lpSubstitutions) [SYM_MAX_PATH] );
#endif  // SYM_WIN


/*@API:**********************************************************************
@Declaration:
STATUS SYM_EXPORT WINAPI ReadNavStart (LPNAVOPTIONS lpNavOptions)

@Description:
Reads the startup options from NAVSTART.DAT found in the program directory.

@Parameters:
$lpNavOptions$      load options here.

@Returns:   On success, NOERR.  Otherwise, ERR.
@See:
@Include:   navutil.h
@Compatibility:
****************************************************************************/
#if !defined(SYM_VXD)
STATUS SYM_EXPORT WINAPI ReadNavStart (LPNAVOPTIONS lpNavOptions)
{
    auto        char            szBuffer [SYM_MAX_PATH+1];
#ifdef SYM_WIN
    extern      HINSTANCE       hInstance;
#else
    auto        HINSTANCE       hInstance;  // bogus!
    hInstance;                              // Uninitialized warning.
#endif

    GetStartDir(hInstance, szBuffer, sizeof(szBuffer) - 1);
    return (ReadNavStartFromDir(szBuffer, lpNavOptions));
}
#endif


/*@API:**********************************************************************
@Declaration:
STATUS SYM_EXPORT WINAPI ReadNavStartFromDir (LPSTR          lpszDir,
                                              LPNAVOPTIONS   lpNavOptions)

@Description:
Reads startup options from NAVSTART.DAT found in the specified directory.

@Parameters:
$lpszDir$       Search here for NAVSTART.DAT
$lpNavOptions$  Load options here.

@Returns:   On success, NOERR.  Otherwise, ERR.
@See:
@Include:   navutil.h
@Compatibility:
****************************************************************************/
#if !defined(SYM_VXD)
STATUS SYM_EXPORT WINAPI ReadNavStartFromDir (LPSTR          lpszDir,
                                              LPNAVOPTIONS   lpNavOptions)
{
    auto        STATUS          uStatus = ERR;
    auto        HFILE           hFile;
    auto        char            szFilename [SYM_MAX_PATH+1];
    auto        char            szPath [SYM_MAX_PATH+1];
#if HAVE_DEFAULT_NAVSTART_DAT           //&?
 #ifdef SYM_WIN
    extern      HINSTANCE       hInstance;
 #else
    auto        HINSTANCE       hInstance;  // bogus
 #endif
    auto        char            szDefault [80];
    auto        CFGTEXTREC      cfgRec;
#endif

    STRCPY(szPath, lpszDir);
    NameAppendFile (szPath, GetNavStartFilename(szFilename));

#if HAVE_DEFAULT_NAVSTART_DAT           //&? No default file yet.
                                        // Special DEFAULT switch.
                                        // Will use the alternative
                                        // NAVDEF.DAT file
    LoadString(hInstance, IDS_DEFAULT, szDefault, sizeof(szDefault));

    cfgRec.lpSwitch = szDefault;
    cfgRec.wFlags = CFG_SW_EXACT;
    if ( ConfigSwitchSet(&cfgRec, NULL) )
        {
        auto    char    szDefaultPath[SYM_MAX_PATH+1];


        STRCPY(szDefaultPath, lpszDir);
        NameAppendFile (szDefaultPath, "NAV???.DAT"); //&? Need default name.
                                                      //&? Please fix NAVAP
                                                      //&? when you add one.

        FileDelete(szPath);             // Delete the old file
        FileCopy(szDefaultPath, szPath, NULL);
        }
#endif // HAVE_DEFAULT_NAVSTART_DAT

    if ((hFile = FileOpen(szPath,OF_READ)) != HFILE_ERROR )
        {
        if ( ReadNavStartFromOpenFile ( hFile, lpNavOptions ) != FALSE )
            {
            uStatus = NOERR;
            }
        FileClose(hFile);
        }

    return(uStatus);
} // ReadNavStartFromDir()
#endif


/*@API:**********************************************************************
@Declaration:
BOOL SYM_EXPORT WINAPI ReadNavStartFromOpenFile (HFILE        hFile,
                                                 LPNAVOPTIONS lpNavOptions)

@Description:
Reads startup options from an open NAVSTART.DAT file.  The file should
be seeked at 0.

@Parameters:
$hFile$         Handle to an open NAVSTART.DAT file.
$lpNavOptions$  Load options here.

@Returns:   On success, TRUE.  Otherwise, FALSE.
@See:
@Include:   navutil.h
@Compatibility:
****************************************************************************/

BOOL SYM_EXPORT WINAPI ReadNavStartFromOpenFile (HFILE        hFile,
                                                 LPNAVOPTIONS lpNavOptions)
{
    auto        BOOL            bResult = FALSE;
    auto        NAVSTARTHEADER  rStartHeader;

                                        // Skip over header
    if (FileRead(hFile, &rStartHeader, sizeof(rStartHeader)) == 
        sizeof(rStartHeader))
        {
                                        // Read option settings
        if ( (rStartHeader.dwVersion == NAVSTART_VERSION) &&
             (sizeof(STARTOPT) == FileRead(hFile,
                                      &lpNavOptions->startup,
                                      sizeof(STARTOPT))) )
            {
            bResult = TRUE;
            }
        }

                                    // Starting with Kirin there is no TSR.
                                    // Ensure that no one expects the TSR to
                                    // loaded.
    lpNavOptions->startup.bLoadTSR = FALSE;

    return(bResult);
}  // ReadNavStartFromOpenFile()


/*@API:**********************************************************************
@Declaration:
STATUS SYM_EXPORT WINAPI WriteNavStart (LPNAVOPTIONS lpNavOptions)

@Description:
Write the startup options to NAVSTART.DAT in the program directory.

@Parameters:
$lpNavOptions$  Write these options.

@Returns:       On success, NOERR.  Otherwise, ERR.
@See:
@Include:       navutil.h
@Compatibility:
****************************************************************************/
#if !defined(SYM_VXD)
STATUS SYM_EXPORT WINAPI WriteNavStart (LPNAVOPTIONS lpNavOptions)
{
    auto        char            szBuffer [SYM_MAX_PATH+1];
#ifdef SYM_WIN
    extern      HINSTANCE       hInstance;
#else
    auto        HINSTANCE       hInstance;  // bogus!
    hInstance;                              // Uninitialized warning.
#endif

    GetStartDir(hInstance, szBuffer, sizeof(szBuffer) - 1);
    return (WriteNavStartToDir(szBuffer, lpNavOptions));
}
#endif


/*@API:**********************************************************************
@Declaration:
STATUS SYM_EXPORT WINAPI WriteNavStartToDir (LPSTR           lpszDir,
                                             LPNAVOPTIONS    lpNavOptions)

@Description:
Write the startup options to NAVSTART.DAT in the specified directory.
Disables TSR while writing to file.

@Parameters:
$lpszDir$       NAVSTART.DAT in this dir.
$lpNavOptions$  These startup options.

@Returns:       On success, NOERR.  Otherwise, ERR.
@See:
@Include:       navutil.h
@Compatibility:
****************************************************************************/
#if !defined(SYM_VXD)
STATUS SYM_EXPORT WINAPI WriteNavStartToDir (LPSTR          lpszDir,
                                             LPNAVOPTIONS   lpNavOptions)
{
    auto    STATUS          uStatus = ERR;
    auto    HFILE           hFile = HFILE_ERROR;
    auto    NAVSTARTHEADER  rStartHeader = DEFAULT_NAVSTART_HEADER;
    auto    NAVSTARTBINDATA rStartData = { 0 };
    auto    char            szFilename [SYM_MAX_PATH+1];
    auto    char            szPath [SYM_MAX_PATH+1];

    // TSR_OFF;                         // Disable TSR around file write.

    STRCPY(szPath, lpszDir);
    NameAppendFile(szPath, GetNavStartFilename(szFilename));

//#if 0
                                        // This used to attempt to create the 
                                        // file if it didn't exist. There is 
                                        // no current code or feature to
                                        // create the list of startup files
                                        // to scan except during install
                                        // (by copying a file). Since this
                                        // code would be a feature change, it
                                        // is removed and the customer
                                        // informed to reinstall.
                                        //
                                        // The above is true except for the
                                        // network where we can read one set
                                        // of files (the defaults) and write
                                        // a new set.
                                        // 07/08/96 JWORDEN

    if ( HFILE_ERROR == (hFile = FileOpen(szPath,OF_WRITE)) )
        {
        if ( HFILE_ERROR != (hFile = FileCreate(szPath, FA_NORMAL)) )
            {
            FileWrite (hFile, &rStartHeader, sizeof(rStartHeader));
            FileWrite (hFile, &rStartData, sizeof(rStartData));
            }
        }

    if (HFILE_ERROR != hFile)
//#endif

//    if ( (hFile = FileOpen(szPath,OF_WRITE)) != HFILE_ERROR )
        {
                                        // Skip over header
        if (HFILE_ERROR != FileSeek(hFile, sizeof(NAVSTARTHEADER), SEEK_SET))
            {
            if ( HFILE_ERROR != FileWrite(hFile,
                                          &lpNavOptions->startup,
                                          sizeof(lpNavOptions->startup)) )
                {
                uStatus = NOERR;
                }
            }
        FileClose(hFile);
        }

    // TSR_ON;                          // Reenable TSR

    return(uStatus);
} // WriteNavStartToDir()
#endif


/*@API:**********************************************************************
@Declaration:
STATUS SYM_EXPORT WINAPI ReadStartupFileList ( DWORD        dwListID,
                                               LPHGLOBAL    lphFileList,
                                               LPDWORD      lpdwNumEntries,
                                               LPDWORD      lpdwSizeEntries )

@Description:
Allocate memory for and read in one of three file lists from NAVSTART.DAT.

@Parameters:
$dwListID$          [in] Id of the list to read in.
                        ~FILELIST_STATIC~       Base list of system files.
                        ~FILELIST_SUBSTITUTION~ List of directories to sub.
                        ~FILELIST_MODIFIED~     System files on this machine.
$lphFileList$       [out] Handle to SZZ list is returned here on success.
                          If NULL, only number and size of entries is returned.
$lpdwNumEntries$    [out] Number of entries returned here.
                          May be NULL, if this data is not required.
$lpdwSizeEntries$   [out] Size of entries from 1st byte of SZZ to 2nd EOS.
                          May be NULL, if this data is not required.

@Returns:   On success, NOERR.
            Possible error returns are:
                ERR
                            Out of memory.
                            Couldn't open NAVSTART.DAT
                            Invalid NAVSTART.DAT
@See:
@Include:   navstart.h
@Compatibility: DOS, WIN, W32
****************************************************************************/
#if !defined(SYM_VXD)
STATUS SYM_EXPORT WINAPI ReadStartupFileList ( DWORD        dwListID,
                                               LPHGLOBAL    lphFileList,
                                               LPDWORD      lpdwNumEntries,
                                               LPDWORD      lpdwSizeEntries )
{
    auto    char                szNavstart [SYM_MAX_PATH+1];
    auto    char                szFilename [SYM_MAX_PATH+1];
    auto    HFILE               hNavstart;
    auto    DWORD               dwBytesRead;

    auto    FILELIST            rFileList;
    auto    STATUS              uStatus = NOERR;

    auto    HGLOBAL             hFileList = NULL;
    auto    LPSTR               lpszzFileList = NULL;
#ifdef SYM_WIN
    extern  HINSTANCE           hInstance;
#else
    auto    HINSTANCE           hInstance;  // bogus!
    hInstance;                              // Uninitialized warning.
#endif

    SYM_ASSERT( (FILELIST_STATIC        == dwListID) ||
                (FILELIST_SUBSTITUTION  == dwListID) ||
                (FILELIST_MODIFIED      == dwListID) );

    GetStartDir(hInstance, szNavstart, sizeof(szNavstart) - 1);
    NameAppendFile(szNavstart, GetNavStartFilename(szFilename));

    hNavstart = FileOpen(szNavstart, OF_READ | OF_SHARE_DENY_NONE);
    if (HFILE_ERROR == hNavstart)
        {
        uStatus = ERR;                  // Couldn't access.
        }
    else
        {
        if ((DWORD)-1 == SeekToStartupBlockData(hNavstart, dwListID))
            {
            uStatus = ERR;
            }
        }

    if (NOERR == uStatus)
        {
        dwBytesRead = FileRead(hNavstart, &rFileList, sizeof(FILELIST));

                                        // Check valid FILELIST: must read it
                                        // and size of SZZ must be at least 2.
        if ( (sizeof(FILELIST) != dwBytesRead) ||
             (2 > rFileList.dwSizeEntries) )
            {
            uStatus = ERR;              // end with invalid navstart.dat
            }
        else
            {
                                        // Read the data in only if we're
                                        // supposed to return the list.
            if (NULL != lphFileList)
                {
                                        // Allocate mem based on header.
                if ( (NULL == (hFileList = MemAlloc(GHND, rFileList.dwSizeEntries)))
                     || (NULL == (lpszzFileList = (LPSTR)MemLock(hFileList))) )
                    {
                    uStatus = ERR;      // end with out of memory.
                    }
                else
                    {
                    dwBytesRead = FileRead(hNavstart,
                                           lpszzFileList,
                                           (UINT)rFileList.dwSizeEntries);

                                        // Verify read correct length and
                                        // it's an SZZ.
                    if ( (rFileList.dwSizeEntries != dwBytesRead) ||
                         (0 != *(LPWORD)(lpszzFileList + rFileList.dwSizeEntries - 2)) )
                        {
                        uStatus = ERR;  // end with invalid navstart.dat
                        }
                    else
                        {
#if defined (SYM_WIN)                   // Need OemToAnsi() for WIN.
    //&? Don't know if we're guaranteed OEM is same size as ANSI, but...
                        for ( ;
                              EOS != *lpszzFileList;
                              lpszzFileList = SzzFindNext(lpszzFileList) )
                            {
                            OemToAnsi(lpszzFileList, lpszzFileList);
                            }
#endif
                        *lphFileList = hFileList;
                        }
                    }
                }

            if (NOERR == uStatus)
                {
                if (NULL != lpdwNumEntries)
                    *lpdwNumEntries = rFileList.dwNumEntries;

                if (NULL != lpdwSizeEntries)
                    *lpdwSizeEntries = rFileList.dwSizeEntries;
                }
            }
        }

                                        // Clean up and exit.
    if (NULL != lpszzFileList)
        MemRelease(hFileList);

    if (HFILE_ERROR != hNavstart)
        FileClose(hNavstart);

    if ( (NULL != hFileList) &&
         (NOERR != uStatus) )
        MemFree(hFileList);

    return (uStatus);
} // ReadStartupFileList()
#endif


/*@API:**********************************************************************
@Declaration:
STATUS SYM_EXPORT WINAPI WriteStartupFileList ( DWORD       dwListID,
                                                LPSTR       lpszzEntries,
                                                DWORD       dwNumEntries,
                                                DWORD       dwSizeEntries )

@Description:
Write one of three file lists to NAVSTART.DAT.

@Parameters:
$dwListID$          Id of the list to write.
                        ~FILELIST_STATIC~       Base list of system files.
                        ~FILELIST_SUBSTITUTION~ List of directories to sub.
                        ~FILELIST_MODIFIED~     System files on this machine.
$lpszzEntries$      SZZ of entries.
$dwNumEntries$      Number of entries in SZZ.
$dwSizeEntries$     Size of entries from 1st byte of SZZ to 2nd EOS.

@Returns:   On success, NOERR.
            Possible error returns are:
                ERR
                            Out of memory.
                            Couldn't open NAVSTART.DAT
                            Invalid NAVSTART.DAT
@See:
@Include:   navstart.h
@Compatibility: DOS, WIN, W32
****************************************************************************/
#if !defined(SYM_VXD)
STATUS SYM_EXPORT WINAPI WriteStartupFileList ( DWORD       dwListID,
                                                LPSTR       lpszzEntries,
                                                DWORD       dwNumEntries,
                                                DWORD       dwSizeEntries )
{
    auto    char                szNavstart [SYM_MAX_PATH+1];
    auto    char                szFilename [SYM_MAX_PATH+1];
    auto    HFILE               hNavstart;
    auto    NAVSTARTBLOCKHEADER rBlockHead;
    auto    FILELIST            rFileList = {0};
    auto    DWORD               dwBlockLocation;
    auto    DWORD               dwBlockToMove;
    auto    DWORD               dwNewLocation;
    auto    STATUS              uStatus = ERR;
#ifdef SYM_WIN
    auto    LPSTR               lpOemEntries = NULL;
    extern  HINSTANCE           hInstance;
#else
    auto    HINSTANCE           hInstance;  // bogus!
    hInstance;                              // Uninitialized warning.
#endif

                                        // Must be SZZ.
    SYM_ASSERT( 2 <= dwSizeEntries );
    SYM_ASSERT( NULL != lpszzEntries );
    SYM_ASSERT( 0 == *(LPWORD)(lpszzEntries + dwSizeEntries - 2) );

    SYM_ASSERT( (FILELIST_STATIC        == dwListID) ||
                (FILELIST_SUBSTITUTION  == dwListID) ||
                (FILELIST_MODIFIED      == dwListID) );

    GetStartDir(hInstance, szNavstart, sizeof(szNavstart) - 1);
    NameAppendFile(szNavstart, GetNavStartFilename(szFilename));

    hNavstart = FileOpen(szNavstart, OF_READWRITE | OF_SHARE_EXCLUSIVE);
    if (HFILE_ERROR == hNavstart)
        {
        goto BailOut;                   // Couldn't access.
        }

    dwBlockLocation = SeekToStartupBlock(hNavstart, dwListID);
    //&? Need error return from Seek()'s, not just -1 = not found...

                                        // Need to resize block...
    if ((DWORD)-1 != dwBlockLocation)
        {
                                        // Read block header for size info.
        if (sizeof(NAVSTARTBLOCKHEADER) != FileRead(hNavstart, &rBlockHead,
                                                    sizeof(NAVSTARTBLOCKHEADER)))
            {
            goto BailOut;               // Access error?  not too likely.
            }

                                        // Adjust placement of following blocks.
        dwBlockToMove = dwBlockLocation + sizeof(NAVSTARTBLOCKHEADER)
                        + rBlockHead.dwSize;

        dwNewLocation = dwBlockToMove - rBlockHead.dwSize
                        + (dwSizeEntries + sizeof(FILELIST));

        if (NOERR != MoveTailOfFile(hNavstart, dwNewLocation, dwBlockToMove))
            {
            goto BailOut;
            }
        }
    else                                // Or, add block at end of file.
        {                               // Don't trust FileLength's() err code.
        dwBlockLocation = FileSeek(hNavstart, 0, SEEK_END);
        if ((DWORD)-1 == dwBlockLocation)
            {
            goto BailOut;
            }
        }

                                        // Set size of new block.
    rBlockHead.dwSize = dwSizeEntries + sizeof(FILELIST);
    FileSeek(hNavstart, dwBlockLocation, SEEK_SET);

    if ( sizeof(NAVSTARTBLOCKHEADER) != FileWrite(hNavstart, &rBlockHead,
                                                  sizeof(NAVSTARTBLOCKHEADER)) )
        {
        goto BailOut;
        }

                                        // Write out the new file list.
    rFileList.dwNumEntries = dwNumEntries;
    rFileList.dwSizeEntries = dwSizeEntries;

    if (sizeof(FILELIST) != FileWrite(hNavstart, &rFileList, sizeof(FILELIST)))
        {
        goto BailOut;
        }

#if defined (SYM_WIN)                   // Do AnsiToOem() before writing.
    //&? Don't know if we're guaranteed OEM is same size as ANSI, but...
    if (NULL == (lpOemEntries = (LPSTR) MemAllocPtr(GHND, dwSizeEntries)))
        {
        goto BailOut;
        }
    MEMCPY(lpOemEntries, lpszzEntries, dwSizeEntries);

    for ( lpszzEntries = lpOemEntries;
          EOS != *lpszzEntries;
          lpszzEntries = SzzFindNext(lpszzEntries) )
        {
        AnsiToOem(lpszzEntries, lpszzEntries);
        }
    lpszzEntries = lpOemEntries;
#endif

    if (dwSizeEntries == FileWrite(hNavstart, lpszzEntries, (UINT)dwSizeEntries))
        {
        uStatus = NOERR;
        }

BailOut:
#if defined (SYM_WIN)
    if (NULL != lpOemEntries)
        MemFreePtr(lpOemEntries);
#endif

    if (HFILE_ERROR != hNavstart)
        {
        if (HFILE_ERROR == FileClose(hNavstart))
            {
            uStatus = ERR;
            }
        }

    return (uStatus);
} // WriteStartupFileList()
#endif  // !SYM_VXD


/*@API:**********************************************************************
@Declaration:
BOOL SYM_EXPORT WINAPI NeedToUpdateStartupFileLists (VOID)

@Description:
This function determines whether the path substitutions in NAVSTART.DAT
are correct for the current machine/configuration.

@Returns:   TRUE if update is needed or error accessing.
            FALSE otherwise.

@See:
@Include:   navstart.h
@Compatibility: W16, W32
****************************************************************************/
#if defined(SYM_WIN)
BOOL SYM_EXPORT WINAPI NeedToUpdateStartupFileLists (VOID)
{
    auto    HGLOBAL     hShouldBe = NULL;
    auto    LPCSTR      lpszzShouldBe = NULL;
    auto    DWORD       dwSizeShouldBe;

    auto    HGLOBAL     hCurrent = NULL;
    auto    LPCSTR      lpszzCurrent = NULL;
    auto    DWORD       dwSizeCurrent;

    auto    BOOL        bNeedToUpdate = TRUE;

                                        // Get list as it should be.
    if ( (NOERR != CreateSubstitutionFileList(&hShouldBe, NULL, &dwSizeShouldBe))
         || (NULL == (lpszzShouldBe = (LPCSTR)MemLock(hShouldBe))) )
        {
        goto BailOut;
        }

                                        // Get list as it is in NAVSTART.DAT
    if ( (NOERR != ReadStartupFileList(FILELIST_SUBSTITUTION,
                                       &hCurrent,
                                       NULL,
                                       &dwSizeCurrent))
         || (NULL == (lpszzCurrent = (LPCSTR)MemLock(hCurrent))) )
        {
        goto BailOut;
        }

                                        // Compare size and contents.
    if ( (dwSizeShouldBe == dwSizeCurrent) &&
         (0 == MEMCMP(lpszzShouldBe, lpszzCurrent, (UINT)dwSizeCurrent)) )
        {
        bNeedToUpdate = FALSE;
        }

BailOut:
    if (NULL != lpszzShouldBe)
        MemRelease(hShouldBe);

    if (NULL != hShouldBe)
        MemFree(hShouldBe);

    if (NULL != lpszzCurrent)
        MemRelease(hCurrent);

    if (NULL != hCurrent)
        MemFree(hCurrent);

    return (bNeedToUpdate);
} // NeedToUpdateStartupFileLists()
#endif  // SYM_WIN


/*@API:**********************************************************************
@Declaration:
STATUS SYM_EXPORT WINAPI UpdateStartupFileLists (VOID)

@Description:
This function updates the substitution list and modified system file list
in NAVSTART.DAT

@Returns:   NOERR on success.
            ERR otherwise.

@See:
@Include:   navstart.h
@Compatibility: W16, W32
****************************************************************************/
#if defined(SYM_WIN)
STATUS SYM_EXPORT WINAPI UpdateStartupFileLists (VOID)
{
    auto    HGLOBAL     hList;
    auto    LPSTR       lpList;
    auto    DWORD       dwEntries;
    auto    DWORD       dwSize;
    auto    STATUS      uStatus;

    uStatus = CreateSubstitutionFileList(&hList, &dwEntries, &dwSize);

    if ( NOERR != uStatus )
        return (uStatus);

    if ( NULL == (lpList = (LPSTR) MemLock(hList)) )
        {
        MemFree(hList);
        return (ERR);
        }

    uStatus = WriteStartupFileList( FILELIST_SUBSTITUTION,
                                    lpList,
                                    dwEntries,
                                    dwSize );
    MemRelease(hList);
    MemFree(hList);

    if ( NOERR != uStatus )
        return (uStatus);

    uStatus = CreateModifiedFileList(&hList, &dwEntries, &dwSize);

    if ( NOERR != uStatus )
        return (uStatus);

    if ( NULL == (lpList = (LPSTR) MemLock(hList)) )
        {
        MemFree(hList);
        return (ERR);
        }

    uStatus = WriteStartupFileList( FILELIST_MODIFIED,
                                    lpList,
                                    dwEntries,
                                    dwSize );
    MemRelease(hList);
    MemFree(hList);

    return (uStatus);
} // UpdateStartupFileLists()
#endif  // SYM_WIN


//************************************************************************
// MoveTailOfFile()
//
// This function moves the tail of an open file (from dwStartOffset to EOF)
// to location dwNewOffset.
// Used to expand/shrink the filelist blocks at the end of NAVSTART.DAT.
//
// Parameters:
//      hFile               Handle to file opened R/W.
//      dwNewOffset         Move "tail" to this offset.
//      dwStartOffset       "Tail" defined as dwStartOffset to EOF.
//
// Returns:
//      NOERR on success.  Otherwise ERR.
//************************************************************************
// 02/24/94 DALLEE, created.
//************************************************************************
#if !defined(SYM_VXD)
STATUS LOCAL PASCAL MoveTailOfFile ( HFILE  hFile,
                                     DWORD  dwNewOffset,
                                     DWORD  dwStartOffset )
{
    auto    LPSTR       lpCopyBuffer = NULL;
    auto    UINT        uSizeCopyBuffer;
    auto    DWORD       dwFileLength;
    auto    DWORD       dwBytesToMove;
    auto    DWORD       dwTruncateOffset;
    auto    int         nDirection;

    // Not using FileLength() since its error return is WORD!! 0xFFFF and
    // this result is confused with real length 0xFFFF.
    // dwFileLength = FileLength(hFile);

    dwFileLength = (LONG) FileSeek(hFile, 0, SEEK_END);

                                        // Validate the tail we're moving.
    if ( ((DWORD)-1 == dwFileLength ) ||
         (dwStartOffset > dwFileLength) )
        {
        return (ERR);
        }
    // else if dwStartOffset == dwFileLength, not an error --
    // we still want to truncate.

    dwBytesToMove = dwFileLength - dwStartOffset;

                                        // Try to allocate copy buffer.
    for ( uSizeCopyBuffer = 0x8000;
          uSizeCopyBuffer > 0x400;
          uSizeCopyBuffer /= 2 )
        {
        lpCopyBuffer = (LPSTR) MemAllocPtr(GHND, uSizeCopyBuffer);
        if (NULL != lpCopyBuffer)
            {
            break;
            }
        }

    if (NULL == lpCopyBuffer)
        {
        return (ERR);                   // Out of mem.
        }

                                        // Save location to truncate while
                                        // we can calculate it easily.
    dwTruncateOffset = dwNewOffset + dwBytesToMove;

                                        // Get copy direction.
                                        // NOTE: nDirection indicates the
                                        // direction the end of the file must
                                        // shift.  However, to prevent overlap,
                                        // the actual copy loop must run in the
                                        // opposite direction.
    if (dwStartOffset < dwNewOffset)
        {
                                        // Moving forward, start at end.
        nDirection = 1;
        dwStartOffset = dwFileLength - min(uSizeCopyBuffer, dwBytesToMove);
        dwNewOffset += dwBytesToMove - min(uSizeCopyBuffer, dwBytesToMove);
        }
    else
        {
                                        // Moving backward, start at beginning.
        nDirection = -1;
        }

    uSizeCopyBuffer = (UINT) min(uSizeCopyBuffer, dwBytesToMove);

    while ( dwBytesToMove )
        {
        FileSeek(hFile, dwStartOffset, SEEK_SET);
        FileRead(hFile, lpCopyBuffer, uSizeCopyBuffer);
        //&? error?

        FileSeek(hFile, dwNewOffset, SEEK_SET);
        FileWrite(hFile, lpCopyBuffer, uSizeCopyBuffer);
        //&? error?

        if ( -1 == nDirection)
            {
            dwStartOffset += uSizeCopyBuffer;
            dwNewOffset += uSizeCopyBuffer;
            }

        dwBytesToMove -= uSizeCopyBuffer;
        uSizeCopyBuffer = (UINT) min(uSizeCopyBuffer, dwBytesToMove);

        if ( 1 == nDirection)
            {
            dwStartOffset -= uSizeCopyBuffer;
            dwNewOffset -= uSizeCopyBuffer;
            }
        }

    MemFreePtr(lpCopyBuffer);

                                        // Remember to terminate the file.
    FileSeek(hFile, dwTruncateOffset, SEEK_SET);
    return (HFILE_ERROR != FileWrite(hFile, lpCopyBuffer, 0) ? NOERR : ERR);
} // MoveTailOfFile()
#endif  // !SYM_VXD


//************************************************************************
// SeekToStartupBlock()
//
// Position NAVSTART.DAT file pointer at beginning of the block specified.
//
// Parameters:
//      hFile               NAVSTART.DAT opened for reading.
//      dwBlockID           ID code of block to find.
//
// Returns:
//      Offset of block on success.
//      On error, (DWORD)-1
//************************************************************************
// 02/24/94 DALLEE, created.
//************************************************************************
#if !defined(SYM_VXD)
DWORD LOCAL PASCAL SeekToStartupBlock ( HFILE  hFile,
                                        DWORD  dwBlockID )
{
    if ((DWORD)-1 != SeekToStartupBlockData(hFile, dwBlockID))
        {
        return (FileSeek(hFile, 0 - (LONG)sizeof(NAVSTARTBLOCKHEADER), SEEK_CUR));
        }
    else
        {
        return ( (DWORD)-1 );
        }
} // SeekToStartupBlock()
#endif  // !SYM_VXD


//************************************************************************
// SeekToStartupBlockData()
//
// Read NAVSTARTBLOCKHEADER with ID passed and position file pointer at
// beginning of the block data.
//
// Parameters:
//      hFile               NAVSTART.DAT opened for reading.
//      dwBlockID           ID code of block to find.
//
// Returns:
//      Offset of block's data on success.
//      On error, (DWORD)-1
//************************************************************************
// 02/24/94 DALLEE, created.
//************************************************************************
#if !defined(SYM_VXD)
DWORD LOCAL PASCAL SeekToStartupBlockData ( HFILE  hFile,
                                            DWORD  dwBlockID )
{
    auto    NAVSTARTHEADER      rHead;
    auto    NAVSTARTBLOCKHEADER rBlockHead;
    auto    DWORD               dwBytesRead;
    auto    DWORD               dwPosition;
    auto    STATUS              uStatus = NOERR;

    FileSeek(hFile, 0, SEEK_SET);

    dwBytesRead = FileRead(hFile, &rHead, sizeof(NAVSTARTHEADER));

    if ( (sizeof(NAVSTARTHEADER) != dwBytesRead) ||
         (NAVSTART_VERSION != rHead.dwVersion) )
        {
        uStatus = ERR;              // Invalid NAVSTART.DAT
        }
    else
        {
                                        // Seek past binary data to start of
                                        // free form block data.
                                        // See navstart.h for format.
        FileSeek(hFile, sizeof(NAVSTARTBINDATA), SEEK_CUR);

                                        // Walk blocks at end of NAVSTART.DAT
                                        // until our ID, or error, or end.
        while (NOERR == uStatus)
            {
            dwBytesRead = FileRead(hFile,
                                   &rBlockHead,
                                   sizeof(NAVSTARTBLOCKHEADER));

                                        // Check end of file.
            if (sizeof(NAVSTARTBLOCKHEADER) != dwBytesRead)
                {
                uStatus = ERR;          // end with not found.
                }
                                        // Check valid block header.
            else if (NAVSTARTBLOCKID != rBlockHead.dwID)
                {
                uStatus = ERR;          // end with invalid navstart.dat
                }
                                        // Is this the block we want?
            else if (dwBlockID == rBlockHead.dwBlockType)
                {
                break;                  // We're done!
                }

                                        // Advance to next block.
            dwPosition = FileSeek(hFile, rBlockHead.dwSize, SEEK_CUR);
            } // while()
        }

    return (NOERR == uStatus ? dwPosition : (DWORD)-1 );
} // SeekToStartupBlockData()
#endif  // !SYM_VXD


//************************************************************************
// CreateSubstitutionFileList()
//
// Creates an SZZ of folders to substitue for codes in static system
// file list in NAVSTART.DAT.
//
// Parameters:
//      lphList             [out] Handle to list returned here.
//      lpdwNumEntries      [out] Number of entries placed here.
//                                May be NULL if information not required.
//      lpdwSizeEntries     [out] Size from 1st bytes SZZ to 2nd EOS.
//                                May be NULL if information not required.
//
// Returns:
//      NOERR on success.  ERR otherwise.
//************************************************************************
// 02/24/95 DALLEE, created.
//************************************************************************
#if defined (SYM_WIN)
STATUS LOCAL PASCAL CreateSubstitutionFileList ( LPHGLOBAL  lphList,
                                                 LPDWORD    lpdwNumEntries,
                                                 LPDWORD    lpdwSizeEntries )
{
    auto    char        (FAR *lpArray) [SYM_MAX_PATH];
    auto    LPSTR       lpszzList;
    auto    HGLOBAL     hList;
    auto    STATUS      uStatus = ERR;
    auto    UINT        uStringLength;
    auto    UINT        uIndex;

    SYM_ASSERT(NULL != lphList);

    if (NOERR == CreateSubstitutionArray(&hList))
        {
        lpArray = (char (FAR *) [SYM_MAX_PATH]) MemLock(hList);

        if (NULL != lpArray)
            {
            lpszzList = lpArray[0];
                                        // Copy from array into same buffer.
            for (uIndex = 0; uIndex < MAX_SUBSTITUTIONS; uIndex++)
                {
                uStringLength = STRLEN(lpArray[uIndex]);

                MEMMOVE(lpszzList, lpArray[uIndex], uStringLength + 1);
                lpszzList += uStringLength + 1;
                }
            *lpszzList = EOS;           // 2nd EOS for SZZ

            *lphList = hList;

            if (NULL != lpdwNumEntries)
                *lpdwNumEntries = uIndex;

            if (NULL != lpdwSizeEntries)
                *lpdwSizeEntries = (lpszzList - lpArray[0]) + 1;

            uStatus = NOERR;
            }
        else
            {
            MemFree(hList);
            }
        }

    return (uStatus);
} // CreateSubstitutionFileList()
#endif  // SYM_WIN


//************************************************************************
// CreateSubstitutionArray()
//
// Creates an array of folders to substitute for codes in static
// system file list in NAVSTART.DAT.
//
// Parameters:
//      lphList             [out] Handle to list returned here.
//
// Returns:
//      NOERR on success.  ERR otherwise.
//************************************************************************
// 02/24/95 DALLEE, created.
//************************************************************************
#if defined(SYM_WIN)
STATUS LOCAL PASCAL CreateSubstitutionArray ( LPHGLOBAL lphList )
{
    auto    BYTE        BootDrive = DiskGetBootDrive();
    auto    HGLOBAL     hList;
    auto    char        (FAR *lpList) [SYM_MAX_PATH] = NULL;
    auto    UINT        uDirLength;
    auto    STATUS      uStatus = ERR;

    SYM_ASSERT(NULL != lphList);

    hList = MemAlloc(GHND, MAX_SUBSTITUTIONS * SYM_MAX_PATH);

    if (NULL == hList)
        {
        goto BailOut;                   // Out of memory.
        }

    lpList = (char (FAR *) [SYM_MAX_PATH]) MemLock(hList);

                                        // Init the boot drive string with
                                        // a reasonable default.  The actual
                                        // drive letter will be replaced
                                        // in the if stmt below.
    STRCPY(lpList[SUB_ROOT_FOLDER], HWIsNEC() ? "A:\\" : "C:\\");

                                        // Update the drive letter
    if (BootDrive != 0)
        lpList[SUB_ROOT_FOLDER][0] = BootDrive;

    uDirLength = GetWindowsDirectory(lpList[SUB_WINDOWS_FOLDER], SYM_MAX_PATH - 1);
    if ( (0 == uDirLength) || (SYM_MAX_PATH <= uDirLength) )
        {
        goto BailOut;
        }

    uDirLength = GetSystemDirectory(lpList[SUB_SYSTEM_FOLDER], SYM_MAX_PATH - 1);
    if ( (0 != uDirLength) && (SYM_MAX_PATH > uDirLength) )
        {
        uStatus = NOERR;
        *lphList = hList;
        }

BailOut:
    if ( NULL != lpList )
        {
        MemRelease(hList);
        }

    if ( (NOERR != uStatus) && (NULL != hList) )
        {
        MemFree(hList);
        }

    return (uStatus);
} // CreateSubstitutionArray()
#endif  // SYM_WIN


//************************************************************************
// CreateModifiedFileList()
//
// Creates an SZZ of list of files to scan for the machine we're running on.
//
// Parameters:
//      lphList             [out] Handle to list returned here.
//      lpdwNumEntries      [out] Number of entries placed here.
//                                May be NULL if information not required.
//      lpdwSizeEntries     [out] Size from 1st bytes SZZ to 2nd EOS.
//                                May be NULL if information not required.
//
// Returns:
//      NOERR on success.  ERR otherwise.
//************************************************************************
// 02/26/95 DALLEE, created.
//************************************************************************
#if defined (SYM_WIN)
STATUS LOCAL PASCAL CreateModifiedFileList ( LPHGLOBAL  lphList,
                                             LPDWORD    lpdwNumEntries,
                                             LPDWORD    lpdwSizeEntries )
{
    auto    HGLOBAL         hSubstitutions = NULL;
    auto    char            (FAR *lpszSubstitutions) [SYM_MAX_PATH] = NULL;

    auto    HGLOBAL         hStatic = NULL;
    auto    LPSTR           lpszzStatic = NULL;
    auto    DWORD           dwNumStatic;
    auto    DWORD           dwSizeStatic;
    auto    LPSTR           lpszStaticEntry;

    auto    HGLOBAL         hModified = NULL;
    auto    LPSTR           lpszzModified = NULL;
    auto    DWORD           dwNumModified;
    auto    DWORD           dwSizeModified;
    auto    char            szModifiedEntry [SYM_MAX_PATH+1];

    auto    STATUS          uStatus = ERR;

    SYM_ASSERT(NULL != lphList);
    SYM_ASSERT(NULL != lpdwNumEntries);
    SYM_ASSERT(NULL != lpdwSizeEntries);

    CreateSubstitutionArray(&hSubstitutions);

    if ( (NULL == hSubstitutions) ||
         (NULL == (lpszSubstitutions = (char(FAR *)[SYM_MAX_PATH])MemLock(hSubstitutions))) )
        {
        goto BailOut;
        }

    ReadStartupFileList(FILELIST_STATIC, &hStatic, &dwNumStatic, &dwSizeStatic);

    if ( (NULL == hStatic) ||
         (NULL == (lpszzStatic = (LPSTR)MemLock(hStatic))) )
        {
        goto BailOut;
        }

                                        // Compute size for MemAlloc...
    for ( dwSizeModified = 0, lpszStaticEntry = lpszzStatic;
          lpszStaticEntry[0] != EOS;
          lpszStaticEntry = SzzFindNext(lpszStaticEntry) )
        {
        if ( NOERR == CreateModifiedEntry(szModifiedEntry,
                                          lpszStaticEntry,
                                          lpszSubstitutions) )
            {
            dwSizeModified += STRLEN(szModifiedEntry) + 1;
            }
        }
                                        // Handle zero entries case, or
                                        // add additional EOS to make SZZ.
    dwSizeModified = max(2, dwSizeModified + 1);

    if ( (NULL == (hModified = MemAlloc(GHND, dwSizeModified))) ||
         (NULL == (lpszzModified = (LPSTR)MemLock(hModified))) )
        {
        goto BailOut;
        }

                                        // Guarantee SZZ for 0 entry case.
    lpszzModified [0] = lpszzModified [1] = EOS;

    for ( dwNumModified=0, lpszStaticEntry=lpszzStatic;
          lpszStaticEntry[0] != EOS;
          lpszStaticEntry = SzzFindNext(lpszStaticEntry) )
        {
        CreateModifiedEntry(lpszzModified, lpszStaticEntry, lpszSubstitutions);
        if ( NOERR == CreateModifiedEntry(lpszzModified,
                                          lpszStaticEntry,
                                          lpszSubstitutions) )
            {
            dwNumModified ++;
            lpszzModified += STRLEN(lpszzModified) + 1;
            }
        }

    *lpszzModified = EOS;               // Add 2nd EOS for SZZ.

                                        // Save return info.
    *lphList = hModified;

    if (NULL != lpdwNumEntries)
        *lpdwNumEntries = dwNumModified;

    if (NULL != lpdwSizeEntries)
        *lpdwSizeEntries = dwSizeModified;

    uStatus = NOERR;

BailOut:
    if (NULL != lpszSubstitutions)
        MemRelease(hSubstitutions);

    if (NULL != hSubstitutions)
        MemFree(hSubstitutions);

    if (NULL != lpszzStatic)
        MemRelease(hStatic);

    if (NULL != hStatic)
        MemFree(hStatic);

    if (NULL != lpszzModified)
        MemRelease(hModified);

    if ( (NOERR != uStatus) && (NULL != hModified) )
        MemRelease(hModified);

    return (uStatus);
} // CreateModifiedFileList()
#endif  // SYM_WIN


//************************************************************************
// CreateModifiedEntry()
//
// Read a static sys file entry and substitute folder from subst list.
// If the new entry is > SYM_MAX_PATH characters or an invalid subst code
// is encountered, it is treated as an error and the return buffer is
// unmodified.
//
// Parameters:
//      lpszModified        [out] Entry returned here.
//      lpszStatic          [in] Sys file entry with possible subst codes.
//      lpSubstitutions     [in] Array of substitution folders.
//
// Returns:
//      On success NOERR.
//      ERR on overrun, or invalid substitution code.
//************************************************************************
// 02/26/95 DALLEE, created.
//************************************************************************
#if defined (SYM_WIN)
STATUS LOCAL PASCAL CreateModifiedEntry (
                            LPSTR           lpszModified,
                            LPCSTR          lpszStatic,
                            char      (FAR *lpSubstitutions) [SYM_MAX_PATH] )
{
    auto    LPSTR   lpszStaticAtSub;
    auto    LPSTR   lpszSubstitution;
    auto    UINT    uModifiedLength;

    SYM_ASSERT(NULL != lpszModified);
    SYM_ASSERT(NULL != lpszStatic);
    SYM_ASSERT(NULL != lpSubstitutions);

    if (SYM_MAX_PATH <= STRLEN(lpszStatic))
        {
        return (ERR);
        }
                                        // Check for substitution character.
                                        // Assumes only one per static list.
    if ( NULL == (lpszStaticAtSub = SYMstrchr(lpszStatic, SUBSTITUTION_CHAR)) )
        {
                                        // Already guaranteed we're short
                                        // enough, so just copy it.
        STRCPY(lpszModified, lpszStatic);
        }
    else                                // Need to do substitution.
        {
        switch (lpszStaticAtSub[1])
            {
            case SUB_ROOT_CHAR:
                lpszSubstitution = lpSubstitutions[SUB_ROOT_FOLDER];
                break;

            case SUB_WINDOWS_CHAR:
                lpszSubstitution = lpSubstitutions[SUB_WINDOWS_FOLDER];
                break;

            case SUB_SYSTEM_CHAR:
                lpszSubstitution = lpSubstitutions[SUB_SYSTEM_FOLDER];
                break;

            default:
                return (ERR);
            }

                                        //&? DBCS -- SUBSTITUTION_CHAR +
                                        // SUB_XXX_CHAR better be equal to
                                        // 2 or this is broken.
        if ( SYM_MAX_PATH <= STRLEN(lpszStatic) - 2 + STRLEN(lpszSubstitution) )
            {
            return (ERR);
            }

                                        // Copy portion before substitution.
        uModifiedLength = lpszStaticAtSub - lpszStatic;
        STRNCPY(lpszModified, lpszStatic, uModifiedLength);

                                        // Substitute folder.
        STRCPY(lpszModified + uModifiedLength, lpszSubstitution);

                                        // Copy portion trailing substitution.
        NameAppendFile(lpszModified, lpszStaticAtSub + 2);
        }

    return (NOERR);
} // CreateModifiedEntry()
#endif  // SYM_WIN

