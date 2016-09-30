// Copyright 1993 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/VIRSCAN/VCS/scn_dta.cpv   1.12   13 Feb 1996 18:23:54   MKEATIN  $
//
// Description:
//      Contains the routine to load information on how to scan memory,
//      boot sectors, or files.
//
//      Currently, it is HIGHLY inefficient.  We need to clean it up, once
//      we get some spare time.  BK.
//
//      Used by ScanInit()
//
// Contains:
//      GetScanData()
//
// See Also:
//      SCN_INIT.C for VirScanInit()
//************************************************************************
// $Log:   S:/VIRSCAN/VCS/scn_dta.cpv  $
// 
//    Rev 1.12   13 Feb 1996 18:23:54   MKEATIN
// Added DX support
// 
//    Rev 1.11   12 Jan 1996 07:11:34   JWORDEN
// Port changes from Quake 8
// 
//    Rev 1.10.1.10   14 Dec 1995 20:29:06   MKEATIN
// added dwDosBytes to allocate mem instead of uDosOverlayBufferSize
// 
//    Rev 1.10.1.9   14 Dec 1995 18:14:48   MKEATIN
// Fixed memory allocation bug relating to uDosOverlayBuffer
// 
//    Rev 1.10.1.8   14 Dec 1995 02:44:38   BARRY
// Fixed undefined uDosOverlayBufferSize problem for non-DOS platforms
// 
//    Rev 1.10.1.7   13 Dec 1995 22:31:30   MKEATIN
// fixed memory signatures buffer if less that 30k
// 
//    Rev 1.10.1.6   15 Nov 1995 11:25:38   MKEATIN
// Added global variable uDosOverlayBufferSize for NavExInit() in scn_init.cpp
// 
//    Rev 1.10.1.5   14 Nov 1995 18:32:24   MKEATIN
// Made sure VIRSCAN_MEMORY buffer is at least 30k - for NavEx in Navboot
// 
//    Rev 1.10.1.4   05 Oct 1995 18:59:56   DCHI
// Fixed incorrect freeing of buffers in FreeScanFileData().
// 
//    Rev 1.10.1.3   18 Sep 1995 13:06:48   DCHI
// Added #ifdef SYM_TSR so that TSR does not get new 64K barrier code.
// 
//    Rev 1.10.1.2   12 Sep 1995 16:28:08   DCHI
// Added support for INSCANF.DAT being greater than 64K.
// 
//    Rev 1.10.1.1   16 Jul 1995 02:06:06   SZIADEH
// added support for INTSRx.dat
// 
// 
//    Rev 1.10.1.0   12 Jul 1995 18:59:54   MPYE
// Branch base for version QAK8
// 
//    Rev 1.10   07 Jun 1995 19:42:22   SZIADEH
// check the INSCANM.DAT header and recreate if invalid.
// 
//    Rev 1.9   02 Feb 1995 17:03:04   SZIADEH
// move CreateInScan() to CERTLIB.
// 
//    Rev 1.8   13 Jan 1995 11:04:08   SZIADEH
// added code to sort memory signatures by type.
// 
//    Rev 1.7   27 Dec 1994 19:05:54   DALLEE
// Use CERTLIBN for DOS platform now, too.
// 
//    Rev 1.6   19 Dec 1994 22:05:18   DALLEE
// Convert .WIN to CERTLIBN
// 
//    Rev 1.5   30 Nov 1994 12:25:46   DALLEE
// Use cts4.h for SYM_DOS, too.
// 
//    Rev 1.4   28 Nov 1994 15:29:14   MFALLEN
// include sts4.h if W16 platform.
// 
//    Rev 1.3   12 May 1994 11:04:00   MFALLEN
// cts4 2 ctsn
// 
//    Rev 1.1   22 Apr 1994 18:16:08   MFALLEN
// Changed _loadds to SYM_EXPORT. If the TSR will need _loadds then this must
// be a macro. Without this change the LINK32 will not generate an import lib.
// 
//    Rev 1.0   22 Apr 1994 10:59:00   MFALLEN
// Initial revision.
// 
//    Rev 1.30   26 Mar 1994 14:39:02   BRAD
// changed from DATETIME_ to TIME_
// 
//    Rev 1.29   26 Mar 1994 13:08:54   BRAD
// New date/time
// 
//    Rev 1.28   25 Mar 1994 19:29:52   BRAD
// New date/time
// 
//    Rev 1.27   15 Mar 1994 11:09:26   BRUCE
// Changed SYM_EXPORT to SYM_EXPORT
// 
//    Rev 1.26   02 Mar 1994 17:12:30   BRAD
// AWK script applied
// 
//    Rev 1.25   11 Feb 1994 15:55:02   MARKL
// Quake 2 changes moved to trunk.
// 
//    Rev 1.24.1.0   19 Oct 1993 08:56:20   SCOTTP
// Branch base for version QAK2
// 
//    Rev 1.24   12 Oct 1993 16:42:48   PATRICKM
// Baseline from QAK1 branch
// 
//    Rev 1.23.1.1   18 Sep 1993 18:52:14   CAREY
// Fixed crash when we have 0 bytes free on drive where we're creating inscans.
// 
//    Rev 1.23.1.0   03 Sep 1993 22:52:50   SCOTTP
// Branch base for version QAK1
// 
//    Rev 1.23   29 Aug 1993 19:11:00   BRAD
// Init 'hMem' to NULL, so if the file IS 0 bytes, it will return the
// correct value (NULL).
// 
//    Rev 1.22   24 Aug 1993 20:50:42   CAREY
// Fixed build problem!
// 
//    Rev 1.21   24 Aug 1993 18:19:54   CAREY
// Changed prototype on creation function.
// 
//    Rev 1.20   23 Aug 1993 01:51:18   BARRY
// Added MODULE_NAME and fixed MemUnlock params for VMM debugging
// 
//    Rev 1.19   16 Aug 1993 12:49:32   CAREY
// Added code to test for version number in VIRSCAN.DAT before building the
// INSCAN files.
// 
//    Rev 1.18   12 Aug 1993 17:46:20   MFALLEN
// Changed prototype to VirusScanInit()
// 
// 
//    Rev 1.17   02 Aug 1993 18:24:06   CAREY
// kill files when unable to create correctly.
// 
//    Rev 1.16   29 Jul 1993 11:54:42   CAREY
// The INSCANF creator function will now delete all INSCANX files if there are
// any errors writing them out (IE, out of disk space, etc).  Also fixed
// a problem with memory signatures.  The first *2* bytes of memory sigs need
// to be examined to determine if a sig is valid; if both are 0, then we skip
// over the sig.
// 
//    Rev 1.15   28 Jul 1993 18:35:06   DALLEE
// AllocBiggestChunk() was using the incorrect variable for size of 
// buffer to MemAlloc.
// 
// 
//    Rev 1.14   28 Jul 1993 16:49:30   MFALLEN
// The last changes were not compiling under Windows.
// 
//    Rev 1.13   28 Jul 1993 15:48:44   CAREY
// Updated so as to use as much memory necessary.  It needs to allocate a minimum
// of 96 bytes of VM in order to work.
// 
//    Rev 1.12   16 Jun 1993 23:44:02   BRAD
// Was always trying to set the date/time of the OVL file, even
// if it was already correct.
// 
//    Rev 1.11   16 Jun 1993 23:03:02   BRAD
// Closed the VIRSCAN.DAT file, after checking for it.  This is
// because the inscan?() routines also open this file.  This routine
// will fail on systems running SHARE.EXE.
// 
//    Rev 1.10   10 Jun 1993 02:51:04   CGUC
// Changed SYM_EXPORT to _loadds
// 
//    Rev 1.9   23 May 1993 19:27:12   BRAD
// Fixed bug, where it would try to set date/time for ovl file, even
// though it didn't exist (also would try to close the ovl file, but
// the handle would be 0.).
// 
//    Rev 1.8   21 May 1993 16:57:38   MFALLEN
// Major bug. Certlib functions were assuming CWD so we store CWD and switch
// to where the DLL is. Do our stuff, and switch back to the original CWD
// 
// 
//    Rev 1.7   28 Apr 1993 09:10:02   MFALLEN
// 
//    Rev 1.6   22 Apr 1993 10:39:08   MFALLEN
// N30Notes
// 
//    Rev 1.5   04 Apr 1993 17:49:30   BRAD
// 
//    Rev 1.4   04 Apr 1993 17:39:22   BRAD
// Removed all references to N30.*
// 
//    Rev 1.3   26 Mar 1993 14:52:40   DALLEE
// Support for DOS.
//************************************************************************

#include "platform.h"
#include "xapi.h"
#include "file.h"
#include "ctsn.h"
#include "virscan.h"

// local declarations
UINT FileCheckHeader( HFILE hFile );
HGLOBAL GetScanFileData(HFILE hFile);


#ifdef SYM_DOSX
#undef SYM_DOS
#endif

#ifdef SYM_DOS
    UINT uDosOverlayBufferSize; //used for NavExInit() in scn_init.cpp
#endif

MODULE_NAME;

//************************************************************************
// GetScanData()
//
// Obtains scan information from the appropriate .DAT file.
// If the file does not exist or the date differs from the virus def file,
// it is recreated from the virus def file.
//
// Parameters:
//      UINT    uTypeData;  type of scan information requested
//                          SCANDATA_FILE; file specific info
//                          SCANDATA_BOOT: boot specific info
//                          SCANDATA_MEM: memory specific info
// Returns:
//      Handle to a block of memory which should be freed
//      when not needed anymore. A NULL is returned if error.
//                                           
//************************************************************************
// 7/28/93 CAREY created
//************************************************************************

HGLOBAL SYM_EXPORT WINAPI GetScanData (UINT uTypeData, BOOL FAR *bReCreated)
{
    HGLOBAL    hMem = NULL;
    HPSTR      hpMem;
    char       lpN30[SYM_MAX_PATH+1];
    char       lpN30OVL[SYM_MAX_PATH+1];
    DWORD      dwBytes;
    HFILE      hN30 = HFILE_ERROR;
    HFILE      hN30OVL = HFILE_ERROR;
    FILETIME   fileTime, fileTimeOvl;
    char       *pStr;
    BOOL       bCreateInScan = FALSE;
    UINT       uInvalidHeader;
    BOOL       bCheckHeader=FALSE;

    VirusScanGetDatFile(lpN30, sizeof(lpN30) - 1);

    switch (uTypeData)
        {
        case VIRSCAN_FILE:
            hMem = GetScanData(VIRSCAN_TSRFILE, bReCreated); // Take care of TSR first
            if (hMem != NULL)
                {
                MemFree(hMem);
                hMem = NULL;            // Just a precaution
                }
            hMem = GetScanData(VIRSCAN_TSRBOOT, bReCreated); // Take care of TSR first
            if (hMem != NULL)
                {
                MemFree(hMem);
                hMem = NULL;            // Just a precaution
                }
            pStr = "INSCANF.DAT";
            break;

        case VIRSCAN_BOOT:
            pStr = "INSCANB.DAT";
            break;

        case VIRSCAN_MEMORY:
            pStr = "INSCANM.DAT";
            bCheckHeader=TRUE;
            break;

        case VIRSCAN_TSRFILE:
            pStr = "INTSRF.DAT";
            break;

        case VIRSCAN_TSRBOOT:
            pStr = "INTSRB.DAT";
            break;

        }

    STRCPY(lpN30OVL, lpN30);
    NameStripFile(lpN30OVL);
    NameAppendFile(lpN30OVL, pStr);

    // first try to open VIRSCAN.DAT.  If we can open it successfully, then
    // we want to verify that we can open/create the SPECIFIED INSCANX file
    // and then build all of the INSCANX files, if necessary.

    MEMSET(&fileTime, 0, sizeof(fileTime));
    MEMSET(&fileTimeOvl, 0, sizeof(fileTimeOvl));
    if ((hN30 = FileOpen (lpN30, READ_ONLY_FILE)) != HFILE_ERROR)
        {
        FileGetTime(hN30, TIME_LASTWRITE, &fileTime);
        FileClose(hN30);

        // we succesfully opened the VIRSCAN.DAT File.  Now it's time to
        // make sure we can open/create INSCANX files.

        if ((hN30OVL = FileOpen(lpN30OVL, READ_ONLY_FILE)) == HFILE_ERROR)
            if ((hN30OVL = FileCreate(lpN30OVL, FA_NORMAL)) == HFILE_ERROR)
                return(NULL);           // can't open or create the F/B/M file
            else
                bCreateInScan = TRUE;
        }
    else
        return(NULL);

    // hN30OVL refers to an opened INSCAN file (whichever one the calling
    // program requested that we open).  Obtain the file date and time of
    // the specified INSCANX file.

    // Here's the problem we've been experiencing during builds:
    // 1. The virscan.dat file is opened and it's date and time are obtained.
    //    this date and time is the current DAY, HOUR, MIN, SEC since the file
    //    was just created.
    // 2. Since the BUILD directory was empty (IE had no INSCAN files in it),
    //    the FileCreate above is called which created the *NEW*, 0 byte
    //    inscan file.  This inscan file *HAS* the same date and time as the
    //    virscan.dat file since it is created immeditately after virscan.dat
    //    in the build process.
    // 3. The dates & times of the newly created INSCANX.DAT and the
    //    VIRSCAN.DAT are the same, and we exit out with a 0 byte INSCAN!
    //
    // To remedy, if we created the INSCAN file, then set it's date and time
    // to zero so the date and time of virscan.dat is guaranteed to be diff.

    if (bCreateInScan == FALSE)
        FileGetTime(hN30OVL, TIME_LASTWRITE, &fileTimeOvl);

    // check for valid header file
    uInvalidHeader = bCheckHeader ? FileCheckHeader (hN30OVL) : FALSE;

    if ( MEMCMP(&fileTime, &fileTimeOvl, sizeof(fileTime)) ||
         uInvalidHeader )
        {
        *bReCreated = TRUE;

        // The INSCANX file is outdated.  Assume that all are outdated and
        // re-create each INSCAN file.

        FileClose (hN30OVL);

        if (CreateInScan(lpN30) == FALSE)
                return(FALSE);          // error creating INSCANX files!

        // success creating files.  re-open the specified INSCAN file

        if ((hN30OVL = FileOpen(lpN30OVL, READ_ONLY_FILE)) == HFILE_ERROR)
            return(NULL);

        }

#ifndef SYM_TSR
    if (uTypeData == VIRSCAN_FILE)
        {

        // DCHI: Special treatment for INSCANF data, added to support
        //  scan data exceeding 64K

        hMem = GetScanFileData(hN30OVL);

        }
    else
#endif

    if ((dwBytes = (WORD) FileLength(hN30OVL)) != 0)
        {

#ifdef SYM_DOS
        DWORD dwDosBytes;

        if (uTypeData == VIRSCAN_MEMORY) // mem buffer is used for NavEx
            {                            // make it at least 30k
            uDosOverlayBufferSize = dwBytes < 30000U ? 30000U : (UINT)dwBytes;
            dwDosBytes = uDosOverlayBufferSize;
            }
        else
            {
            dwDosBytes = dwBytes;  // not VIRSCAN_MEMORY so not uDosOverlayBuf
            }
        if ((hMem = MemAlloc(GHND | GMEM_SHARE, dwDosBytes + 10)) != NULL)
#else
        if ((hMem = MemAlloc(GHND | GMEM_SHARE, dwBytes + 10)) != NULL)
#endif
            {
            hpMem = (HPSTR)MemLock (hMem);

            if (FileHugeRead(hN30OVL, hpMem, dwBytes) != dwBytes)
                {

                // error reading INSCANX file.  Close & free memory!

                FileClose(hN30OVL);
                MemRelease( hMem );
                MemFree(hMem);
                return(NULL);

                }

            MemRelease ( hMem );
            }
        }

    FileClose (hN30OVL);

    return (hMem);

} // End GetScanData()

//----------------------------------------------------------------
// return 0 if header is OK
//        ErrorCode otherwise (at this time no error codes are
//                             defined)
//
// VER_NAV95 is defined in CTSN.H
//----------------------------------------------------------------
UINT FileCheckHeader( HFILE hFile )
{

    HEADER  InScanHeader = {0};
    UINT    uError = 1;
    UINT    uCountMemorySigs=0;

    FileSeek(hFile, 0, SEEK_SET);

    if (FileRead(hFile, &InScanHeader, sHEADER) != sHEADER)
        {
        return ( uError );
        }

    uCountMemorySigs = InScanHeader.uNumBOOs +
                       InScanHeader.uNumCRCs +
                       InScanHeader.uNumREMs;

                                // uNumALGs designates number of memory
                                // sigs in the file
    if ( uCountMemorySigs == InScanHeader.uNumALGs )
       uError = 0;

    FileSeek(hFile, 0, SEEK_SET);
    return ( uError );

}

//************************************************************************
// GetScanFileData()
//
// Obtains scan information from the appropriate .DAT file.
// If the file does not exist or the date differs from the virus def file,
// it is recreated from the virus def file.
//
// Parameters:
//      HFILE   hFile;      Handle to opened INSCANF.DAT file
//
// Returns:
//      Handle to a block of memory which should be freed
//      when not needed anymore. A NULL is returned if error.
//
// Comments:
//      This function was added to enable NAV 16-bit code to work
//      with file scan data greater than 64K.
//
//      The function returns a handle that points to a block of memory
//      in the following format:
//
//          struct
//          {
//              HEADER  InScanHeader;       // Header as read from the
//                                          //  INSCANF.DAT file
//
//              // The rest of the block contains the CRCs
//
//              WORD    crcs[];
//
//              // Note: The combined size of lpFirstALGBlk,
//              //  wNumBlks, and hHandles[] is 256.
//
//              LPSTR   lpFirstALGBlk;      // Pointer to first
//                                          //  block of memory
//                                          //  containing algorithmic
//                                          //  signatures
//              WORD    wNumBlks;           // Number of algorithmic
//                                          //  signature blocks allcoated
//              HGLOBAL hHandles[];         // Handles to memory blocks
//                                          //  containing ALG sigs
//          }
//
//      The lpFirstALGBlk pointer points to the first block of memory
//      containing algorithmic signatures.  The memory blocks
//      containing the algorithmic signatures are in a linked list.
//      The format of an algorithmic memory block is the following:
//
//          struct
//          {
//              LPSTR   lpNextALGBlk;       // Pointer to next
//                                          //  block of memory
//                                          //  containing algorithmic
//                                          //  signatures.
//                                          //  NULL if last block.
//              WORD    wNumSigs;           // Number of signatures
//                                          //  in this block
//              BYTE    lpSigs[][SIZE_SIG]; // The algorithmic sigs
//          }
//
//      Note: When the function returns, the header memory block is
//      unlocked, but the ALG sig blocks remain locked.
//                                           
//************************************************************************
// 9/7/95 DARREN created
//************************************************************************

HGLOBAL GetScanFileData(HFILE hFile)
{
    HGLOBAL         hMem;               // Handle to first header block
                                        //  containing handles to ALG
                                        //  blocks and CRCs
    HGLOBAL         hBlkMem;            // Handle to current block of
                                        //  memory being read into
    LPSTR           lpMem;
    LPSTR           lpBlkMem;
    LPSTR           lpPrevBlkMem;
    HEADER          InScanHeader;
    LONG            offsetBegBlk;
    WORD            i;
    WORD            begBlkI;            // The index of the first
                                        //  signature to be placed
                                        //  in the current block
    WORD            lastNonRetFalseI;   // The index of the signature
                                        //  after the last signature
                                        //  to be placed in the
                                        //  current block
    WORD            numSigsInBlk;       // Number of signatures so
                                        //  far to place in the
                                        //  current block
    WORD            maxSigsPerBlk;      // Maximum number of signatures
                                        //  in any one block
    WORD FAR   *    lpwNumBlks;
    HGLOBAL FAR *   lphNextHandle;
    BYTE            lpSig[SIZE_SIG];

    // Get header information

    FileSeek(hFile, 0, SEEK_SET);

    if (FileRead(hFile, &InScanHeader, sHEADER) != sHEADER)
        return (NULL);

    // Allocate memory for header and CRCs

    hMem = hBlkMem = MemAlloc(
        GHND | GMEM_SHARE,
        256 + sHEADER + 2 * InScanHeader.uNumCRCs);

    if (hMem == NULL)
        return (NULL);

    lpMem = (LPSTR)MemLock(hMem);
    lpPrevBlkMem = lpMem + sHEADER + 2 * InScanHeader.uNumCRCs;
    lpwNumBlks = (WORD FAR *)(lpPrevBlkMem + sizeof(LPSTR));
    lphNextHandle = (HGLOBAL FAR *)(lpPrevBlkMem +
        sizeof(LPSTR) + sizeof(WORD));

    // Initialize next ptr to NULL and number of blocks to zero

    *((LPSTR FAR *)lpPrevBlkMem) = NULL;
    *lpwNumBlks = 0;

    // Read header into just allocated memory block

    FileSeek(hFile, 0, SEEK_SET);
    FileRead(hFile, lpMem, sHEADER);

    // Note beginning of ALGs

    offsetBegBlk = FileSeek(hFile, 0, SEEK_CUR);

    maxSigsPerBlk = 65000 / SIZE_SIG;

    lastNonRetFalseI = 0;
    for (begBlkI=numSigsInBlk=i=0;i<InScanHeader.uNumALGs;i++)
        {
        FileRead(hFile, lpSig, SIZE_SIG);

        // The following check enables us to guarantee that a
        //  continued signature will not span across a memory
        //  block boundary

        if ((*((WORD *)(lpSig+1))) != 0x0EE0)
            {
            lastNonRetFalseI = i;
            }

        if (numSigsInBlk == maxSigsPerBlk)
            {
            hBlkMem = MemAlloc(
                GHND | GMEM_SHARE,
                sizeof(LPSTR) + sizeof(WORD) +
                    (lastNonRetFalseI - begBlkI) * SIZE_SIG);

            if (hBlkMem == NULL)
                break;

            lpBlkMem = (LPSTR)MemLock(hBlkMem);

            // Remember handle and pointer

            ++(*lpwNumBlks);
            *lphNextHandle++ = hBlkMem;
            *((LPSTR FAR *)lpPrevBlkMem) = lpBlkMem;
            lpPrevBlkMem = lpBlkMem;

            // Record as potential last block

            *((LPSTR FAR *)lpBlkMem) = NULL;

            // Read signatures

            FileSeek(hFile, offsetBegBlk, SEEK_SET);
            FileRead(
                hFile,
                lpBlkMem + sizeof(LPSTR) + sizeof(WORD),
                (lastNonRetFalseI - begBlkI) * SIZE_SIG);

            // Record number of signatures in block

            *((WORD FAR *)(lpBlkMem + sizeof(LPSTR))) =
                lastNonRetFalseI - begBlkI;

            // Note offset for beginning of next block

            offsetBegBlk = FileSeek(hFile, 0, SEEK_CUR);

            begBlkI = lastNonRetFalseI;
            numSigsInBlk = 0;
            }
        else
            ++numSigsInBlk;
        }

    if (i < InScanHeader.uNumALGs)
        {
        // Means an error occurred

        // Free all allocated memory

        FreeScanFileData(lpMem);
        MemRelease(hMem);
        MemFree(hMem);
        hMem = NULL;
        }
    else
        {
        // Get last block

        if (InScanHeader.uNumALGs > 0)
            {
            hBlkMem = MemAlloc(
                GHND | GMEM_SHARE,
                sizeof(LPSTR) + sizeof(WORD) +
                    (InScanHeader.uNumALGs - begBlkI) * SIZE_SIG);

            if (hBlkMem == NULL)
                {
                // Unable to allocate memory

                FreeScanFileData(lpMem);
                MemRelease(hMem);
                MemFree(hMem);
                return (NULL);

                }

            lpBlkMem = (HPSTR)MemLock(hBlkMem);

            // Remember handle and pointer

            ++(*lpwNumBlks);
            *lphNextHandle++ = hBlkMem;
            *((LPSTR FAR *)lpPrevBlkMem) = lpBlkMem;

            // Record as last block

            *((LPSTR FAR *)lpBlkMem) = NULL;

            // Read signatures

            FileSeek(hFile, offsetBegBlk, SEEK_SET);
            FileRead(
                hFile,
                lpBlkMem + sizeof(LPSTR) + sizeof(WORD),
                (InScanHeader.uNumALGs - begBlkI) * SIZE_SIG);

            // Record number of signatures in block

            *((WORD FAR *)(lpBlkMem + sizeof(LPSTR))) =
                InScanHeader.uNumALGs - begBlkI;
            }

        // Read CRCs

        FileRead(hFile, lpMem + sHEADER, 2 * InScanHeader.uNumCRCs);

        // Release lock on header block

        MemRelease(hMem);
        }

    return hMem;
}

//************************************************************************
// FreeScanFileData()
//
// Frees memory containing alg sigs.
//
// Parameters:
//      LPSTR   lpMem;      // Pointer to memory block containing
//                          //  pointer and handle information to
//                          //  memory blocks containing ALG sigs
// Comments:
//      Call this function to release the memory allocated
//      for algorithmic signatures.
//                                           
//************************************************************************
// 9/7/95 DARREN created
//************************************************************************

VOID SYM_EXPORT WINAPI FreeScanFileData(LPSTR lpMem)
{
    WORD            i;
    WORD            numBlks;
    HGLOBAL FAR *   lphBlkMem;

    // Skip past header and CRCs

    lpMem += sHEADER + 2 * ((LPHEADER)lpMem)->uNumCRCs;

    numBlks = *((WORD FAR *)(lpMem + sizeof(LPSTR)));
    lphBlkMem = (HGLOBAL FAR *)(lpMem + sizeof(LPSTR) + sizeof(WORD));
    for (i=0;i<numBlks;i++)
        {
        MemRelease(lphBlkMem[i]);
        MemFree(lphBlkMem[i]);
        }
}

