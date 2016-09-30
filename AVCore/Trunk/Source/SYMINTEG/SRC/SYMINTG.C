// Copyright 1993 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/SYMINTEG/VCS/SYMINTG.C_v   1.35   29 Jun 1998 13:34:42   DBuches  $
//
// Description:
//      Used to verify if the system has been modified, or any of the
//      protected files in the INTEG database.
//
// Contains:
//      IntegSelfTest()
//      IntegCreate()
//      IntegVerify()
//      IntegRepair()
//      IntegRepairEntry()
//      IntegGetSystemNames()
//
// See Also:
//************************************************************************
// $Log:   S:/SYMINTEG/VCS/SYMINTG.C_v  $
// 
//    Rev 1.35   29 Jun 1998 13:34:42   DBuches
// Removed call to DiskCacheGetDrivesByMask().
// 
//    Rev 1.34   11 Jun 1998 13:15:20   DBuches
// Made IntegRepair FAT32 aware.
// 
//    Rev 1.33   12 Sep 1997 11:42:58   KSACKIN
// Ported changes from QAKG.
// 
//    Rev 1.32.1.1   23 Aug 1997 01:00:52   DBUCHES
// Fixed problem with GetBufferCRC().  For FAT32 volumes, we now ignore
// the Root Starting Cluster field.  This is needed because disk optimization
// tools such as our SpeedDisk will move the Root Directory.  Needless to say,
// bad things result when NAV decides to replace the boot record with it's
// innoculated copy...
//
//    Rev 1.32.1.0   12 Aug 1997 22:45:38   DALVIDR
// Branch base for version QAKG
//
//    Rev 1.32   11 Aug 1997 14:18:24   MKEATIN
// SECTORS_PER_TRACT_OFFSET should have been called SECTORS_PER_FAT_OFFSET.
//
//    Rev 1.31   30 Jul 1997 21:12:24   MKEATIN
// We now ingore the volume label on FAT32 as well as FAT16.
//
//    Rev 1.30   07 May 1997 15:57:42   BILL
// Alpha changes
//
//    Rev 1.29   12 Sep 1996 12:50:16   JBRENNA
// STS 67890 - The file attributes saved by Sys Integ were wrong because the
// filename passed to FIL4_ATTR in IntegAddEntry() was not a real filename.
// For instance, C:\COMMAND.COM becomes C:\COMMAND.W40. I added a parameter
// so the original filename (C:\COMMAND.COM) is passed in IntegAddEntry().
//
//    Rev 1.28   12 Jan 1996 12:57:44   JWORDEN
// Port changes from Quake 8
//
//    Rev 1.26.1.3   27 Oct 1995 12:28:04   JWORDEN
// Alter IntegCreate and IntegVerify to pass actual file size or 1024,
// whichever is smaller, to IntegAddEntry and IntegVerifyEntry for files
// like MSDOS.SYS, IO.SYS, etc.
//
//    Rev 1.26.1.2   24 Oct 1995 07:59:48   DALLEE
// Removed NT check on IntegGetSystemNames().
// System Inoculation is being removed from Gargoyle entirely.
// Better to leave the behavior unchanged until we need to change it.
//
// Note: Last rev I forgot to mention I changed the buffer allocations to
// use standard allocations and removed all the #ifdefs.
// DiskAbsOperation() and DIS4_READ_PHYSICAL() no longer require real-mode
// pointers for WIN16 platform.
//
//    Rev 1.26.1.1   17 Oct 1995 20:23:50   DALLEE
// Disabled IntegGetSystemNames() for NT.
// Should plug in later once extra NT items are determined.
//
//    Rev 1.26.1.0   12 Jul 1995 19:04:24   RJACKSO
// Branch base for version QAK8
//
//    Rev 1.26   20 Jun 1995 10:23:56   BARRY
// Ignore volume labels when evaluating boot record integ data
//
//    Rev 1.25   06 Jun 1995 18:07:12   SZIADEH
// took out the dec for MKDIR_INOC_DB(). it's now in CTSN.H
//
//    Rev 1.24   16 May 1995 17:45:04   SZIADEH
// fixed IntegRepair() so that it repairs a single item at a time.
//
//    Rev 1.23   29 Apr 1995 19:06:36   SZIADEH
// Reworked the logic that checks for the current state of the machine (
// whether in DOS or WINDOWS)
// Use the extension W40 and DOS for WINDOWS and DOS files respectively.
//
//
//    Rev 1.22   07 Apr 1995 08:58:36   SZIADEH
// modified GetIntegId() so that the id's match in DOS and WIN95.
// Only the high order dword of the file time&date is common between
// dos and win95.
//
//    Rev 1.21   01 Feb 1995 16:58:12   SZIADEH
// use INIT_CERTLIB_USE_DEFAULT in InitCertlib() call so that Quake
// functions are used.
//
//    Rev 1.20   30 Jan 1995 19:59:24   MFALLEN
// Function prototypes that used some kinda file handle were completelly wrong.
// The file handles were being defined as WORD or UINT and this is causing
// problems when checking for conditions such as HFILE_ERROR. This has been
// fixed and all TAB's that somebody put it were removed.
//
//    Rev 1.19   27 Jan 1995 19:56:50   SZIADEH
// modified to work under WIN32.
//
//    Rev 1.18   12 Dec 1994 20:13:12   MFALLEN
// Fixed all compile errors except for the globaldosalloc family functions that
// do not work in W32. The rest is Sami's.
//
//    Rev 1.17   01 Dec 1994 11:01:10   SZIADEH
// modified code to handle WIN95 system files.
//
//    Rev 1.5.1.2   18 Nov 1993 15:47:06   DALLEE
// Removed 1k static buffer in DOS.  Use MemAlloc().
// Fixed error - unmatched else (if was commented out).
//************************************************************************
#include "platform.h"
#include "symsys.h"                     // FlagTest
#include "xapi.h"
#include "ctsn.h"
#include "syminteg.h"
#include "file.h"
#include "callback.h"
#include "disk.h"

#define MaskOutMinutes  0xFFFFFF00   //dword value
#define IN_WINDOWS       0x0001
#define IN_DOS           0x0002
#define WIN_EXT          "W40"
#define DOS_EXT          "DOS"

VOID WINAPI ChangeExtension ( LPSTR FileName, LPSTR NewExtension );
WORD WINAPI ChangeSystemFileExtension ( LPSTR FileName );
WORD WINAPI GetCurrentState(VOID);
WORD LOCAL  GetBufferCRC(LPSTR lpBuffer, WORD uBufSize, BOOL bIgnoreVolume);
void LOCAL SaveFAT32RootSCN( LPBYTE lpMemory, 
                             WORD wCylinder,
                             WORD wHead,  
                             WORD wSector,
                             BYTE driveNum );


WORD           wAttribute;
CALLBACKREV1   LocalCallBack;

#ifdef SYM_WIN
   HINSTANCE hInstance;
#endif  // #ifdef SYM_WIN


//**************************************************************************
// IntegSelfTest
//
//  Return if the CRC stamp of this program has been modified.
//
// WORD WINAPI IntegSelfTest(LPSTR lpFilename,BYTE Flag,LPSTR Buffer);
//
// Input:
//      1)  Pointer to filename
//      2)  Flag = 0, Do TEST
//          Flag = 1, Do CREATE (STAMP of file)
//      3)  Buffer of mim 512 bytes
// Output:
//      Flag = 0
//       1)  0 = Success, matched and all ok.
//       2)  1 = File was never CRC stamped.
//       2)  2 = Bad CRC, didn't match
//       3)  3 = File IO error.
//      Flag = 1
//       1)  0 = Success, properly STAMPED file.
//       2)  -1 = Error.
//
//**************************************************************************
WORD SYM_EXPORT WINAPI IntegSelfTest(LPSTR lpFilename,BYTE bFlag,LPSTR lpBuffer)
{
#ifdef _ALPHA_
	//SymAssert(FALSE);
	return 0;
#else
    WORD    nStatus;


                                        // Initialize CERTLIBN
    InitCertlib ( &LocalCallBack, INIT_CERTLIB_USE_DEFAULT );

    if (bFlag)
        nStatus = FIL4_CRCFILE(lpFilename,lpBuffer);
    else
        nStatus = FIL4_SELF_TEST(lpFilename,lpBuffer);

    return (nStatus);
#endif
}

//**************************************************************************
// IntegCreate
//
//  Used to create the INTEG database.
//
// WORD WINAPI IntegCreate(LPSTR StoreFile,LPINTEG Array[]);
//
//  Input:
//      1)  StoreFile - Name of INTEG database.
//      2)  Array - Array of elements to add to database.
//
//  Output:
//      1)  True - Success
//      2)  False - Fail (never happens anymore -- returns 2 instead)
//          If failed, examine the Array to determine what failed
//      3)  2 Unable to create StoreFile
//      4)  3 Unable to allocate memory
//
// 9/18/93 BarryG Now deletes StoreFile if any error while writing.
//                This will make it harder for corrupt INTEG databases to
//                exist.
//**************************************************************************
// 10/17/95 DALLEE, Don't use real-mode pointer for SYM_WIN anymore.
//              DiskAbsOperation() expects prot mode pointer.
//**************************************************************************
WORD SYM_EXPORT WINAPI IntegCreate(LPSTR lpStore,LPINTEG lpArray)
{
#ifdef _ALPHA_
	//SymAssert(FALSE);
	return 1;
#else
    int     nStatus = TRUE;
    HFILE   hFile;
    HFILE   hFile2;
    DWORD   dwBoot;
    char    szTemp[SYM_MAX_PATH];

    LPSTR   lpMemory;                   // Work buffer.

    DWORD   dwFileSize;

                                        // Initialize CERTLIBN
    InitCertlib ( &LocalCallBack , INIT_CERTLIB_USE_DEFAULT);

    if (NULL != (lpMemory = MemAllocPtr(GHND, 1024)))
        {
        MKDIR_INOC_DB(lpStore);         // Create database directory

        hFile = FileCreate(lpStore,FA_NORMAL);

        if ( hFile == HFILE_ERROR )
            nStatus = 2;
        else
            {
            while (*lpArray->byFilename && (nStatus == TRUE || nStatus == INTEG_FAIL_GET))
                {
                lpArray->wStatus = 0 ;      // Guarantee initialized

                if (!STRICMP(lpArray->byFilename,"MBR80"))
                    {
                    if (DIS4_READ_MBR(0x80,lpMemory))
                        lpArray->wStatus = INTEG_FAIL_GET;
                    else
                        {
                        if ((nStatus =
                                 IntegAddEntry (hFile, 0,
                                                lpArray->byFilename, lpArray->byFilename,
                                                lpMemory,512, INTEG_FLAG_NONE)) == FALSE)
                            lpArray->wStatus = INTEG_FAIL_STORE;
                        else
                            lpArray->wStatus = INTEG_SUCCESS_STORE;
                        }
                    }

                if (!STRICMP(lpArray->byFilename,"BOOT800"))
                    {
                    dwBoot = DIS4_LOC_BOOTSECTOR_NUM(0x80,0,lpMemory);
                    if (LOBYTE(LOWORD(dwBoot)) == 0)
                        {
                        lpArray->wStatus = INTEG_INVALID_BOOT;
                        }
                    else if (LOWORD(dwBoot) == -1)
                        {
                        lpArray->wStatus = INTEG_FAIL_GET;
                        }
                    else
                        {
                        if (DIS4_READ_PHYSICAL(lpMemory,               // Buffer
                                            1,                      // Number of sectors
                                            HIWORD(dwBoot),         // Cylinder number
                                            LOBYTE(LOWORD(dwBoot)), // Sector number
                                            HIBYTE(LOWORD(dwBoot)), // Head
                                            0x80))                  // Drive
                            {
                            lpArray->wStatus = INTEG_FAIL_GET;
                            }
                        else
                            {
                            if ((nStatus =
                                   IntegAddEntry(hFile,0,
                                                 lpArray->byFilename, lpArray->byFilename,
                                                 lpMemory,512,INTEG_FLAG_SKIP_VOL)) == FALSE)
                                lpArray->wStatus = INTEG_FAIL_STORE;
                            else
                                lpArray->wStatus = INTEG_SUCCESS_STORE;
                            }
                        }
                    }

                if (!STRICMP(lpArray->byFilename,"BOOT801"))
                    {
                    dwBoot = DIS4_LOC_BOOTSECTOR_NUM(0x80,1,lpMemory);
                    if (LOBYTE(LOWORD(dwBoot)) == 0)
                        {
                        lpArray->wStatus = INTEG_INVALID_BOOT;
                        }
                    else if (LOWORD(dwBoot) == -1)
                        {
                        lpArray->wStatus = INTEG_FAIL_GET;
                        }
                    else
                        {
                        if (DIS4_READ_PHYSICAL(lpMemory,               // Buffer
                                            1,                      // Number of sectors
                                            HIWORD(dwBoot),         // Cylinder number
                                            LOBYTE(LOWORD(dwBoot)), // Sector number
                                            HIBYTE(LOWORD(dwBoot)), // Head
                                            0x80))                  // Drive
                            {
                            lpArray->wStatus = INTEG_FAIL_GET;
                            }
                        else
                            {
                            if ((nStatus =
                                   IntegAddEntry(hFile,0,
                                                 lpArray->byFilename, lpArray->byFilename,
                                                 lpMemory,512,INTEG_FLAG_SKIP_VOL)) == FALSE)
                                lpArray->wStatus = INTEG_FAIL_STORE;
                            else
                                lpArray->wStatus = INTEG_SUCCESS_STORE;
                            }
                        }
                    }

                if (!STRICMP(lpArray->byFilename,"BOOT802"))
                    {
                    dwBoot = DIS4_LOC_BOOTSECTOR_NUM(0x80,2,lpMemory);
                    if (LOBYTE(LOWORD(dwBoot)) == 0)
                        {
                        lpArray->wStatus = INTEG_INVALID_BOOT;
                        }
                    else if (LOWORD(dwBoot) == -1)
                        {
                        lpArray->wStatus = INTEG_FAIL_GET;
                        }
                    else
                        {
                        if (DIS4_READ_PHYSICAL(lpMemory,               // Buffer
                                            1,                      // Number of sectors
                                            HIWORD(dwBoot),         // Cylinder number
                                            LOBYTE(LOWORD(dwBoot)), // Sector number
                                            HIBYTE(LOWORD(dwBoot)), // Head
                                            0x80))                  // Drive
                            {
                            lpArray->wStatus = INTEG_FAIL_GET;
                            }
                        else
                            {
                            if ((nStatus =
                                   IntegAddEntry(hFile,0,
                                                 lpArray->byFilename, lpArray->byFilename,
                                                 lpMemory,512,INTEG_FLAG_SKIP_VOL)) == FALSE)
                                lpArray->wStatus = INTEG_FAIL_STORE;
                            else
                                lpArray->wStatus = INTEG_SUCCESS_STORE;
                            }
                        }
                    }

                if (!STRICMP(lpArray->byFilename,"BOOT803"))
                    {
                    dwBoot = DIS4_LOC_BOOTSECTOR_NUM(0x80,3,lpMemory);
                    if (LOBYTE(LOWORD(dwBoot)) == 0)
                        {
                        lpArray->wStatus = INTEG_INVALID_BOOT;
                        }
                    else if (LOWORD(dwBoot) == -1)
                        {
                        lpArray->wStatus = INTEG_FAIL_GET;
                        }
                    else
                        {
                        if (DIS4_READ_PHYSICAL(lpMemory,               // Buffer
                                            1,                      // Number of sectors
                                            HIWORD(dwBoot),         // Cylinder number
                                            LOBYTE(LOWORD(dwBoot)), // Sector number
                                            HIBYTE(LOWORD(dwBoot)), // Head
                                            0x80))                  // Drive
                            {
                            lpArray->wStatus = INTEG_FAIL_GET;
                            }
                        else
                            {
                            if ((nStatus =
                                   IntegAddEntry(hFile,0,
                                                 lpArray->byFilename, lpArray->byFilename,
                                                 lpMemory,512,INTEG_FLAG_SKIP_VOL)) == FALSE)
                                lpArray->wStatus = INTEG_FAIL_STORE;
                            else
                                lpArray->wStatus = INTEG_SUCCESS_STORE;
                            }
                        }
                    }

                if (!lpArray->wStatus)
                    {
                    hFile2 = FileOpen(lpArray->byFilename,READ_ONLY_FILE);

                    if ( hFile2 != HFILE_ERROR )
                        {
                        dwFileSize = FIL4_SIZE (hFile2);
                        if (dwFileSize != 0)
                            {

                            STRCPY ( szTemp, lpArray->byFilename );
                            ChangeSystemFileExtension( szTemp );

                            if (dwFileSize > 1024)
                                dwFileSize = 1024;

                            if ((nStatus = IntegAddEntry (hFile,
                                                          hFile2,
                                                          szTemp,
                                                          lpArray->byFilename,
                                                          lpMemory,
                                                          (WORD)dwFileSize,
                                                          INTEG_FLAG_NONE)) == FALSE)
                                lpArray->wStatus = INTEG_FAIL_STORE;
                            else
                                lpArray->wStatus = INTEG_SUCCESS_STORE;
                            }
                        else
                            lpArray->wStatus = INTEG_SUCCESS_STORE;

                        FIL4_CLOSE(hFile2);
                        }
                    else
                        lpArray->wStatus = INTEG_FAIL_GET;
                    }

                lpArray ++;
                } // while()
            } // if() else

        if ( FIL4_CLOSE(hFile) != 0 || nStatus == FALSE )
            {
            nStatus = 2 ;           // Error writing file.
            FIL4_DELETE(lpStore);
            }

        MemFreePtr(lpMemory);
        }
    else
        nStatus = 3;

    return (nStatus);
#endif
}

//**************************************************************************
// IntegVerify
//
//  Verify the array of integrity items to database, if
//  we find a difference, will return in the array what elements
//  have changed.
//
// WORD WINAPI IntegVerify(LPSTR lpStore,LPINTEG lpArray);
//
//  Input:
//      1)  Filename that has integ information.
//      2)  Array of integrity items.
//  Ouput:
//      1)  TRUE, they all matched.
//      2)  FALSE, one or more failed to match
//      3)  2 Unable to open StoreFile
//      4)  3 Unable to allocate memory
//      5)  4 All existing entries matched but some entries missing
//
//**************************************************************************
// 10/17/95 DALLEE, Don't use real-mode pointer for SYM_WIN anymore.
//              DiskAbsOperation() expects prot mode pointer.
//**************************************************************************

WORD SYM_EXPORT WINAPI IntegVerify(LPSTR lpStore,LPINTEG lpArray)
{
#ifdef _ALPHA_
	//SymAssert(FALSE);
	return 1;
#else
    int     nStatus = TRUE;
    HFILE   hFile;
    HFILE   hFile2;
    DWORD   dwBoot;
    BOOL    bAnyMissing=FALSE;
    char    szTemp[SYM_MAX_PATH];

    LPSTR   lpMemory;

    DWORD dwFileSize;

                                        // Initialize CERTLIBN
    InitCertlib ( &LocalCallBack , INIT_CERTLIB_USE_DEFAULT);

    if (NULL != (lpMemory = MemAllocPtr(GHND, 1024)))
        {
        hFile = FileOpen(lpStore,READ_ONLY_FILE);

        if ( hFile == HFILE_ERROR )
            nStatus = 2;
        else
            {
            while (*lpArray->byFilename)
                {

                lpArray->wStatus = 0 ;      // Guarantee initialized

                if (!STRICMP(lpArray->byFilename,"MBR80"))
                    {
                    if (DIS4_READ_MBR(0x80,lpMemory))
                        lpArray->wStatus = INTEG_FAIL_GET;
                    else
                        {
                        lpArray->wStatus =
                              IntegVerifyEntry(hFile,0,lpArray->byFilename,
                                               lpMemory,512, INTEG_FLAG_NONE);
                        if ( lpArray->wStatus == INTEG_FAIL_GET )
                            bAnyMissing = TRUE;
                        }
                    }

                if (!STRICMP(lpArray->byFilename,"BOOT800"))
                    {
                    dwBoot = DIS4_LOC_BOOTSECTOR_NUM(0x80,0,lpMemory);
                    if (LOBYTE(LOWORD(dwBoot)) == 0)
                        {
                        lpArray->wStatus = INTEG_INVALID_BOOT;
                        }
                    else if (LOWORD(dwBoot) == -1)
                        {
                        lpArray->wStatus = INTEG_FAIL_GET;
                        }
                    else
                        {
                        if (DIS4_READ_PHYSICAL(lpMemory,               // Buffer
                                            1,                      // Number of sectors
                                            HIWORD(dwBoot),         // Cylinder number
                                            LOBYTE(LOWORD(dwBoot)), // Sector number
                                            HIBYTE(LOWORD(dwBoot)), // Head
                                            0x80))                  // Drive
                            {
                            lpArray->wStatus = INTEG_FAIL_GET;
                            }
                        else
                            {
                            lpArray->wStatus =
                                  IntegVerifyEntry(hFile,0,lpArray->byFilename,
                                                   lpMemory,512,
                                                   INTEG_FLAG_SKIP_VOL);
                            if ( lpArray->wStatus == INTEG_FAIL_GET )
                                bAnyMissing = TRUE;
                            }
                        }
                    }

                if (!STRICMP(lpArray->byFilename,"BOOT801"))
                    {
                    dwBoot = DIS4_LOC_BOOTSECTOR_NUM(0x80,1,lpMemory);
                    if (LOBYTE(LOWORD(dwBoot)) == 0)
                        {
                        lpArray->wStatus = INTEG_INVALID_BOOT;
                        }
                    else if (LOWORD(dwBoot) == -1)
                        {
                        lpArray->wStatus = INTEG_FAIL_GET;
                        }
                    else
                        {
                        if (DIS4_READ_PHYSICAL(lpMemory,               // Buffer
                                            1,                      // Number of sectors
                                            HIWORD(dwBoot),         // Cylinder number
                                            LOBYTE(LOWORD(dwBoot)), // Sector number
                                            HIBYTE(LOWORD(dwBoot)), // Head
                                            0x80))                  // Drive
                            {
                            lpArray->wStatus = INTEG_FAIL_GET;
                            }
                        else
                            {
                            lpArray->wStatus =
                                  IntegVerifyEntry(hFile,0,lpArray->byFilename,
                                                   lpMemory,512,
                                                   INTEG_FLAG_SKIP_VOL);
                            if ( lpArray->wStatus == INTEG_FAIL_GET )
                                bAnyMissing = TRUE;
                            }
                        }
                    }

                if (!STRICMP(lpArray->byFilename,"BOOT802"))
                    {
                    dwBoot = DIS4_LOC_BOOTSECTOR_NUM(0x80,2,lpMemory);
                    if (LOBYTE(LOWORD(dwBoot)) == 0)
                        {
                        lpArray->wStatus = INTEG_INVALID_BOOT;
                        }
                    else if (LOWORD(dwBoot) == -1)
                        {
                        lpArray->wStatus = INTEG_FAIL_GET;
                        }
                    else
                        {
                        if (DIS4_READ_PHYSICAL(lpMemory,               // Buffer
                                            1,                      // Number of sectors
                                            HIWORD(dwBoot),         // Cylinder number
                                            LOBYTE(LOWORD(dwBoot)), // Sector number
                                            HIBYTE(LOWORD(dwBoot)), // Head
                                            0x80))                  // Drive
                            {
                            lpArray->wStatus = INTEG_FAIL_GET;
                            }
                        else
                            {
                            lpArray->wStatus =
                                  IntegVerifyEntry(hFile,0,lpArray->byFilename,
                                                   lpMemory,512,
                                                   INTEG_FLAG_SKIP_VOL);
                            if ( lpArray->wStatus == INTEG_FAIL_GET )
                                bAnyMissing = TRUE;
                            }
                        }
                    }

                if (!STRICMP(lpArray->byFilename,"BOOT803"))
                    {
                    dwBoot = DIS4_LOC_BOOTSECTOR_NUM(0x80,3,lpMemory);
                    if (LOBYTE(LOWORD(dwBoot)) == 0)
                        {
                        lpArray->wStatus = INTEG_INVALID_BOOT;
                        }
                    else if (LOWORD(dwBoot) == -1)
                        {
                        lpArray->wStatus = INTEG_FAIL_GET;
                        }
                    else
                        {
                        if (DIS4_READ_PHYSICAL(lpMemory,               // Buffer
                                            1,                      // Number of sectors
                                            HIWORD(dwBoot),         // Cylinder number
                                            LOBYTE(LOWORD(dwBoot)), // Sector number
                                            HIBYTE(LOWORD(dwBoot)), // Head
                                            0x80))                  // Drive
                            {
                            lpArray->wStatus = INTEG_FAIL_GET;
                            }
                        else
                            {
                            lpArray->wStatus =
                                  IntegVerifyEntry(hFile,0,lpArray->byFilename,
                                                   lpMemory,512,
                                                   INTEG_FLAG_SKIP_VOL);
                            if ( lpArray->wStatus == INTEG_FAIL_GET )
                                bAnyMissing = TRUE;
                            }
                        }
                    }

                if (!lpArray->wStatus)
                    {
                    hFile2 = FileOpen(lpArray->byFilename,READ_ONLY_FILE);

                    if ( hFile2 != HFILE_ERROR )
                        {

                        dwFileSize = FIL4_SIZE (hFile2);
                        if (dwFileSize != 0)
                            {

                            STRCPY ( szTemp, lpArray->byFilename );
                            ChangeSystemFileExtension( szTemp );

                            if (dwFileSize > 1024)
                                dwFileSize = 1024;

                            lpArray->wStatus =
                                   IntegVerifyEntry (hFile,
                                                     hFile2,
                                                     szTemp,
                                                     lpMemory,
                                                     (WORD)dwFileSize,
                                                     INTEG_FLAG_NONE);

                            if ( lpArray->wStatus == INTEG_FAIL_GET )
                                bAnyMissing = TRUE;
                            }
                        else
                            lpArray->wStatus = INTEG_SUCCESS_VERIFY;

                        FIL4_CLOSE(hFile2);
                        }
                    else
                        lpArray->wStatus = INTEG_FAIL_GET;
                    }

                if (lpArray->wStatus == INTEG_FAIL_VERIFY)
                    nStatus = FALSE;

                lpArray ++;
                } // while()

            FIL4_CLOSE(hFile);
            } // if() else

        MemFreePtr(lpMemory);
        }
    else
        nStatus = 3;

    if ( nStatus == TRUE && bAnyMissing )
        nStatus = 4 ;

    return (nStatus);
#endif
}

//**************************************************************************
// IntegRepair
//
//  Repair the array of integrity items to database, if
//  we find a difference, will return in the array what elements
//  have changed.
//
// WORD WINAPI IntegRepair(LPSTR lpStore,LPINTEG lpArray);
//
//  Input:
//      1)  Filename that has integ information.
//      2)  Array of integrity items.
//  Ouput:
//      1)  TRUE, they all matched.
//      2)  FALSE, one or more failed to match
//      3)  2 Unable to open StoreFile
//      4)  3 Unable to allocate memory
//
//**************************************************************************
// 10/17/95 DALLEE, Don't use real-mode pointer for SYM_WIN anymore.
//              DiskAbsOperation() expects prot mode pointer.
//**************************************************************************

WORD SYM_EXPORT WINAPI IntegRepair(LPSTR lpStore,LPINTEG lpArray)
{
#ifdef _ALPHA_
	//SymAssert(FALSE);
	return 1;
#else
    int     nStatus = TRUE;
    HFILE   hFile;
    HFILE   hFile2;
    DWORD   dwBoot;
    char    szTemp[SYM_MAX_PATH];

    LPSTR   lpMemory;

                                        // Initialize CERTLIBN
    InitCertlib ( &LocalCallBack , INIT_CERTLIB_USE_DEFAULT );

    if (NULL != (lpMemory = MemAllocPtr(GHND, 1024)))
        {
        hFile = FileOpen(lpStore,READ_ONLY_FILE);

        if ( hFile == HFILE_ERROR )
            nStatus = 2;
        else
            {
            if (!STRICMP(lpArray->byFilename,"MBR80") && lpArray->wStatus == INTEG_FAIL_VERIFY)
                {
                if (IntegRepairEntry(hFile,0,lpArray->byFilename,lpMemory,512, 0))
                    {
                    if (DIS4_WRITE_PHYSICAL(lpMemory,1,0,1,0,0x80) == 0)
                        {
                        lpArray->wStatus = INTEG_SUCCESS_REPAIR;
                        }
                    else
                        lpArray->wStatus = INTEG_FAIL_REPAIR;
                    }
                }

            if (!STRICMP(lpArray->byFilename,"BOOT800") && lpArray->wStatus == INTEG_FAIL_VERIFY)
                {
                dwBoot = DIS4_LOC_BOOTSECTOR_NUM(0x80,0,lpMemory);
                if (LOBYTE(LOWORD(dwBoot)) == 0)
                    {
                    lpArray->wStatus = INTEG_INVALID_BOOT;
                    }
                else if (LOWORD(dwBoot) == -1)
                    {
                    lpArray->wStatus = INTEG_FAIL_GET;
                    }
                else
                    {
                    if (IntegRepairEntry(hFile,0,lpArray->byFilename,lpMemory,512, 0))
                        {
                        SaveFAT32RootSCN( lpMemory, HIWORD(dwBoot),           // Cylinder     
                                                    HIBYTE(LOWORD(dwBoot)),   // Side
                                                    LOBYTE(LOWORD(dwBoot)),   // Sector
                                                    0x80 );                   // Drive number  
                        
                        if (DIS4_WRITE_PHYSICAL(lpMemory,          // Buffer
                                        1,                      // Number of sectors
                                        HIWORD(dwBoot),         // Cylinder number
                                        LOBYTE(LOWORD(dwBoot)), // Sector number
                                        HIBYTE(LOWORD(dwBoot)), // Head
                                        0x80) == 0)             // Drive
                            {
                            lpArray->wStatus = INTEG_SUCCESS_REPAIR;
                            }
                        else
                            lpArray->wStatus = INTEG_FAIL_REPAIR;

                        }
                    }
                }

            if (!STRICMP(lpArray->byFilename,"BOOT801") && lpArray->wStatus == INTEG_FAIL_VERIFY)
                {
                dwBoot = DIS4_LOC_BOOTSECTOR_NUM(0x80,1,lpMemory);
                if (LOBYTE(LOWORD(dwBoot)) == 0)
                    {
                    lpArray->wStatus = INTEG_INVALID_BOOT;
                    }
                else if (LOWORD(dwBoot) == -1)
                    {
                    lpArray->wStatus = INTEG_FAIL_GET;
                    }
                else
                    {
                    if (IntegRepairEntry(hFile,0,lpArray->byFilename,lpMemory,512, 0))
                        {
                        SaveFAT32RootSCN( lpMemory, HIWORD(dwBoot),           // Cylinder     
                                                    HIBYTE(LOWORD(dwBoot)),   // Side
                                                    LOBYTE(LOWORD(dwBoot)),   // Sector
                                                    0x80 );                   // Drive number  
                        
                        if (DIS4_WRITE_PHYSICAL(lpMemory,          // Buffer
                                        1,                      // Number of sectors
                                        HIWORD(dwBoot),         // Cylinder number
                                        LOBYTE(LOWORD(dwBoot)), // Sector number
                                        HIBYTE(LOWORD(dwBoot)), // Head
                                        0x80) == 0)             // Drive
                            {
                            lpArray->wStatus = INTEG_SUCCESS_REPAIR;
                            }
                        else
                            lpArray->wStatus = INTEG_FAIL_REPAIR;
                        }
                    }
                }

            if (!STRICMP(lpArray->byFilename,"BOOT802") && lpArray->wStatus == INTEG_FAIL_VERIFY)
                {
                dwBoot = DIS4_LOC_BOOTSECTOR_NUM(0x80,2,lpMemory);
                if (LOBYTE(LOWORD(dwBoot)) == 0)
                    {
                    lpArray->wStatus = INTEG_INVALID_BOOT;
                    }
                else if (LOWORD(dwBoot) == -1)
                    {
                    lpArray->wStatus = INTEG_FAIL_GET;
                    }
                else
                    {
                    if (IntegRepairEntry(hFile,0,lpArray->byFilename,lpMemory,512, 0))
                        {
                        SaveFAT32RootSCN( lpMemory, HIWORD(dwBoot),           // Cylinder     
                                                    HIBYTE(LOWORD(dwBoot)),   // Side
                                                    LOBYTE(LOWORD(dwBoot)),   // Sector
                                                    0x80 );                   // Drive number  

                        if (DIS4_WRITE_PHYSICAL(lpMemory,          // Buffer
                                        1,                      // Number of sectors
                                        HIWORD(dwBoot),         // Cylinder number
                                        LOBYTE(LOWORD(dwBoot)), // Sector number
                                        HIBYTE(LOWORD(dwBoot)), // Head
                                        0x80) == 0)             // Drive
                            {
                            lpArray->wStatus = INTEG_SUCCESS_REPAIR;
                            }
                        else
                            lpArray->wStatus = INTEG_FAIL_REPAIR;
                        }
                    }
                }

            if (!STRICMP(lpArray->byFilename,"BOOT803") && lpArray->wStatus == INTEG_FAIL_VERIFY)
                {
                dwBoot = DIS4_LOC_BOOTSECTOR_NUM(0x80,3,lpMemory);
                if (LOBYTE(LOWORD(dwBoot)) == 0)
                    {
                    lpArray->wStatus = INTEG_INVALID_BOOT;
                    }
                else if (LOWORD(dwBoot) == -1)
                    {
                    lpArray->wStatus = INTEG_FAIL_GET;
                    }
                else
                    {
                    if (IntegRepairEntry(hFile,0,lpArray->byFilename,lpMemory,512, 0))
                        {
                        SaveFAT32RootSCN( lpMemory, HIWORD(dwBoot),           // Cylinder     
                                                    HIBYTE(LOWORD(dwBoot)),   // Side
                                                    LOBYTE(LOWORD(dwBoot)),   // Sector
                                                    0x80 );                   // Drive number  
                        
                        if (DIS4_WRITE_PHYSICAL(lpMemory,          // Buffer
                                        1,                      // Number of sectors
                                        HIWORD(dwBoot),         // Cylinder number
                                        LOBYTE(LOWORD(dwBoot)), // Sector number
                                        HIBYTE(LOWORD(dwBoot)), // Head
                                        0x80) == 0)             // Drive
                            {
                            lpArray->wStatus = INTEG_SUCCESS_REPAIR;
                            }
                        else
                            lpArray->wStatus = INTEG_FAIL_REPAIR;
                        }
                    }
                }


            if (lpArray->wStatus == INTEG_FAIL_VERIFY)
                {
                FIL4_ATTR(lpArray->byFilename,0,SET_INFO);

                hFile2 = FileCreate(lpArray->byFilename,FA_NORMAL);

                if ( hFile2 != HFILE_ERROR )
                    {

                    STRCPY ( szTemp, lpArray->byFilename );
                    ChangeSystemFileExtension( szTemp );

                    if (IntegRepairEntry(hFile,hFile2,szTemp,lpMemory,1024, 0) == 0)
                        lpArray->wStatus = INTEG_FAIL_REPAIR;
                    else
                        lpArray->wStatus = INTEG_SUCCESS_REPAIR;

                    FIL4_CLOSE(hFile2);
                    FIL4_ATTR(lpArray->byFilename,wAttribute,SET_INFO);
                    }
                else
                    lpArray->wStatus = INTEG_FAIL_GET;
                }

            if (lpArray->wStatus == INTEG_FAIL_REPAIR)
                nStatus = FALSE;

            FIL4_CLOSE(hFile);
            }

        MemFreePtr(lpMemory);
        }
    else
        nStatus = 3;

    return (nStatus);
#endif
}

//**************************************************************************
// IntegRepairEntry
//
//  Repair if an entry is in the database and if it matches.
//
// WORD WINAPI IntegRepairEntry(WORD hFile,WORD hFile2,LPSTR Name,
//                              LPSTR Buffer,WORD uBuffer);
//
// Input:
//      1) Handle to database
//      2) Handle to file (if 0 then in Buffer already)
//      3) Name of element
//      4) Buffer
//      5) Size of buffer
// Output:
//      1) TRUE - Found / Repaired
//      2) FALSE - Not found / Repaired
//**************************************************************************
WORD SYM_EXPORT WINAPI IntegRepairEntry (
    HFILE   hFile,
    HFILE   hFile2,
    LPSTR   lpName,
    LPSTR   lpBuffer,
    WORD    uBuffer,
    WORD    FilesToUse
    )

{
#ifdef _ALPHA_
	//SymAssert(FALSE);
	return 1;
#else
    DWORD       dwCurrent,dwSize;
    int         nStatus = TRUE;
    INTEGRECORD IntegRecord;
    INTEGHEADER IntegHeader;
    WORD        wBytesRead;

                                        // Initialize CERTLIBN
    InitCertlib ( &LocalCallBack , INIT_CERTLIB_USE_DEFAULT );

    FIL4_SEEK(hFile,0l,fSEEK_BEG);
    if ((FIL4_READ(hFile,(LPSTR)&IntegHeader,sINTEGHEADER)) != sINTEGHEADER)
        nStatus = INTEG_FAIL_GET;

    while ((nStatus == TRUE) && FIL4_READ(hFile,(LPSTR)&IntegRecord,sINTEGRECORD) == sINTEGRECORD)
        {
        if (!STRICMP(IntegRecord.lpName, lpName))
            {
            if (hFile2)
                {
                dwSize = IntegRecord.dwNextEntry;
                dwCurrent = FIL4_SEEK(hFile,0l,fSEEK_FOR);
                dwSize -= dwCurrent;

                while (dwSize && (wBytesRead = FIL4_READ(hFile,lpBuffer,uBuffer)))
                    {
                    if (dwSize > (DWORD)wBytesRead)
                        {
                        FIL4_WRITE(hFile2,lpBuffer,wBytesRead);
                        dwSize = dwSize - (DWORD)wBytesRead;
                        }
                    else
                        {
                        FIL4_WRITE(hFile2,lpBuffer,LOWORD(dwSize));
                        dwSize = 0;
                        }
                    }

                FIL4_DATETIME(hFile2,IntegRecord.wDate,IntegRecord.wTime,SET_INFO);
                wAttribute = IntegRecord.wAttr;
                nStatus = 2;
                }
            else
                {
                FIL4_READ(hFile,lpBuffer,uBuffer);
                nStatus = 2;
                }
            }
        else
            {
            FIL4_SEEK(hFile,IntegRecord.dwNextEntry,fSEEK_BEG);
            }
        }

    if (nStatus = 2)
        nStatus = TRUE;

    return(nStatus);
#endif
}

//**************************************************************************
// IntegVerifyEntry
//
//  Verify if an entry is in the database and if it matches.
//
// WORD WINAPI IntegVerifyEntry(WORD hFile,WORD hFile2,LPSTR Name,
//                              LPSTR Buffer,WORD uBuffer);
//
// Input:
//      1) Handle to database
//      2) Handle to file (if 0 then in Buffer already)
//      3) Name of element
//      4) Buffer
//      5) Size of buffer
// Output:
//      1) INTEG_FAIL_GET    = Element not in database.
//      2) INTEG_FAIL_VERIFY = Element has failed verification.
//      3) INTEG_SUCCESS_VERIFY = Element matches perfectly.
//**************************************************************************

WORD SYM_EXPORT WINAPI IntegVerifyEntry (
    HFILE   hFile,
    HFILE   hFile2,
    LPSTR   lpName,
    LPSTR   lpBuffer,
    WORD    uBuffer,
    WORD    wFlags
    )
{
#ifdef _ALPHA_
	//SymAssert(FALSE);
	return 1;
#else
    WORD        wCRC;
                                        // Assume we didn't find
    int         nStatus = INTEG_FAIL_GET;
    INTEGRECORD IntegRecord;
    INTEGHEADER IntegHeader;

                                        // Initialize CERTLIBN
    InitCertlib ( &LocalCallBack , INIT_CERTLIB_USE_DEFAULT );

    if (hFile2)
        {
        FIL4_READ(hFile2,lpBuffer,uBuffer);
        }

    wCRC = GetBufferCRC( lpBuffer, uBuffer,
                         FlagTest(wFlags, INTEG_FLAG_SKIP_VOL) );

    FIL4_SEEK(hFile,0l,fSEEK_BEG);
    if ((FIL4_READ(hFile,(LPSTR)&IntegHeader,sINTEGHEADER)) != sINTEGHEADER)
        return INTEG_FAIL_GET;

    while ((nStatus == INTEG_FAIL_GET) && FIL4_READ(hFile,(LPSTR)&IntegRecord,sINTEGRECORD) == sINTEGRECORD)
        {
        if (!STRICMP(IntegRecord.lpName,lpName))
            {
            if (IntegRecord.wCRC == wCRC)
                nStatus = INTEG_SUCCESS_VERIFY;
            else
                nStatus = INTEG_FAIL_VERIFY;
            }
        else
            {
            FIL4_SEEK(hFile,IntegRecord.dwNextEntry,fSEEK_BEG);
            }
        }

    return(nStatus);
#endif
}

//**************************************************************************
// IntegGetSystemNames
//
//  Get the names of up to 6 (+ terminating entry) system files that
//  this computer has booted.
//
// WORD WINAPI IntegGetSystemNames(LPINTEG lpArray);
//
// Input:
//      1) An array that can hold seven entries
// Output:
//      1) Number of entries used out of six (7th is terminator)
//      2) NULL = Error
//**************************************************************************
WORD SYM_EXPORT WINAPI IntegGetSystemNames(LPINTEG lpArray)
{
#ifdef _ALPHA_
	//SymAssert(FALSE);
	return 1;
#else
    auto    int     nStatus = 0;

    auto    HGLOBAL hBuffer;
    auto    LPSTR   lpMemory;

                                        // Initialize CERTLIBN
    InitCertlib ( &LocalCallBack , INIT_CERTLIB_USE_DEFAULT );

    if ((hBuffer = MemAlloc(GPTR,1024)) != 0)
        {
        lpMemory = MemLock(hBuffer);

        DIS4_READ_PHYSICAL (lpMemory, 1, 0, 1, 0, 0x80 );

        if (SYS4_GET_NAMES(lpMemory))
            {
            while (*lpMemory)
                {
                STRCPY(lpArray->byFilename,lpMemory);
                lpArray->wStatus = 0;

                lpMemory += STRLEN(lpMemory) + 1;
                lpArray++;
                nStatus++;
                }
            }

        MemUnlock(hBuffer,lpMemory);
        MemFree(hBuffer);
        }

    lpArray->byFilename[0] = '\0';  // Terminate list

    return (nStatus);
#endif
}

//**************************************************************************
// IntegAddEntry
//
//  Add entry to the integ database.
//
//  BOOL WINAPI IntegAddEntry(WORD hFile,WORD hFile2,LPSTR Name,
//                            LPSTR lpBuffer, WORD uBuffer);
//
//  Input:
//      1) Handle to database file to write to.
//      2) Handle to file to read, then write to database file.
//      3) Name of file being added
//      4) Original Name of file being added. When system files are saved
//         the "lpName" parameter is potentially not the real file name. We
//         need the real filename in order to get its file attributes.
//      5) Buffer to use or write
//      6) Size of buffer
//
//      If Handle to read (2) is NULL, then the BUFFER already
//      contains the information to write with uBuffer size
//
//  Output:
//      1) TRUE - Success
//      2) FALSE - Fail
//
//**************************************************************************
// 09/12/96 JBRENNA: Add the lpOrigName parameter so that FIL4_ATTR would
//          be passed a filename that actually exists.
//**************************************************************************

BOOL SYM_EXPORT WINAPI IntegAddEntry (  // Adds entry to integ database
    HFILE   hFile,                      // [in] handle to database to write to
    HFILE   hFile2,                     // [in] handle to read file
    LPSTR   lpName,                     // [in] name of file being added
    LPSTR   lpOrigName,                 // [in] original version of "lpName"
    LPSTR   lpBuffer,                   // [in] buffer to use to write
    WORD    uBuffer ,                   // [in] size of buffer
    WORD    wFlags )                    // [in] INTEG_FLAG_* (syminteg.h)
{
#ifdef _ALPHA_
	//SymAssert(FALSE);
	return 1;
#else
    DWORD       dwDateTime;
    BYTE        bFirst = TRUE;
    WORD        wBytesRead;
    DWORD       dwNewRecord;
    int         nStatus = TRUE;
    INTEGRECORD IntegRecord;
    INTEGHEADER IntegHeader;

                                        // Initialize CERTLIBN
    InitCertlib ( &LocalCallBack , INIT_CERTLIB_USE_DEFAULT );

    MEM4_SET((LPSTR)&IntegHeader,0,sINTEGHEADER);
    MEM4_SET((LPSTR)&IntegRecord,0,sINTEGRECORD);

    FIL4_SEEK(hFile,0l,fSEEK_BEG);      // Always seek to beginning
    if ((FIL4_READ(hFile,(LPSTR)&IntegHeader,sINTEGHEADER)) != sINTEGHEADER)
        {
        STRCPY(IntegHeader.lpName,"SYMINTEG");
        IntegHeader.wEntries = 0;
        IntegHeader.dwStartEntry = sINTEGHEADER;
        FIL4_SEEK(hFile,0l,fSEEK_BEG);
        FIL4_WRITE(hFile,(LPSTR)&IntegHeader,sINTEGHEADER);
        FIL4_SEEK(hFile,0l,fSEEK_BEG);
        if (FIL4_READ(hFile,(LPSTR)&IntegHeader,sINTEGHEADER) != sINTEGHEADER)
            nStatus = FALSE;
        }

    if (nStatus)
        {
        while (FIL4_READ(hFile,(LPSTR)&IntegRecord,sINTEGRECORD) == sINTEGRECORD)
            {
            FIL4_SEEK(hFile,IntegRecord.dwNextEntry,fSEEK_BEG);
            }

        dwNewRecord = FIL4_SEEK(hFile,0l,fSEEK_FOR);

        STRCPY(IntegRecord.lpName,lpName);

        FIL4_WRITE(hFile,(LPSTR)&IntegRecord,sINTEGRECORD);

        if (hFile2)
            {
            IntegRecord.dwSize = FIL4_SEEK(hFile2,0,fSEEK_END);
            FIL4_SEEK(hFile2,0,fSEEK_BEG);

            while ((wBytesRead = FIL4_READ(hFile2,lpBuffer,uBuffer)) != 0)
                {
                if (bFirst)
                    {
                    IntegRecord.wCRC = GetBufferCRC(lpBuffer, wBytesRead, FALSE);

                    dwDateTime = FIL4_DATETIME(hFile2,0,0,GET_INFO);
                    IntegRecord.wDate = HIWORD(dwDateTime);
                    IntegRecord.wTime = LOWORD(dwDateTime);
                    IntegRecord.wAttr = FIL4_ATTR(lpOrigName,0,GET_INFO);

                    bFirst = FALSE;
                    }
                FIL4_WRITE(hFile,lpBuffer,wBytesRead);
                }
            }
        else
            {
            IntegRecord.dwSize = 512;
            IntegRecord.wCRC = GetBufferCRC(lpBuffer, uBuffer,
                                            FlagTest(wFlags, INTEG_FLAG_SKIP_VOL));
            IntegRecord.wDate = 0;
            IntegRecord.wTime = 0;
            FIL4_WRITE(hFile,lpBuffer,uBuffer);
            }

        IntegRecord.dwNextEntry = FIL4_SEEK(hFile,0l,fSEEK_FOR);

        FIL4_SEEK(hFile,dwNewRecord,fSEEK_BEG);

        FIL4_WRITE(hFile,(LPSTR)&IntegRecord,sINTEGRECORD);

        FIL4_SEEK(hFile,0,fSEEK_BEG);

        IntegHeader.wEntries++;

        GetIntegId (IntegHeader.szID);
        FIL4_WRITE(hFile,(LPSTR)&IntegHeader,sINTEGHEADER);
        }

    return (nStatus);
#endif
}


//**************************************************************************
//
//**************************************************************************
//
WORD WINAPI ChangeSystemFileExtension (
    LPSTR FileName )
{
#ifdef _ALPHA_
	//SymAssert(FALSE);
	return 1;
#else
    if (!STRICMP (FileName, szWinbootSys ) )
        {
        STRCPY ( FileName, szIoSys );
        ChangeExtension ( FileName, WIN_EXT );
        return(TRUE);
        }

    if (  !STRICMP ( FileName, szIoSys     ) ||
          !STRICMP ( FileName, szMsdosSys  ) ||
          !STRICMP ( FileName, szCommandCom) ||
          !STRICMP ( FileName, szIbmbioSys ) ||
          !STRICMP ( FileName, szIbmdosSys ) )
        {
        switch ( GetCurrentState() )
            {
            case IN_WINDOWS:
                 ChangeExtension ( FileName, WIN_EXT );
                 break;
            case IN_DOS:
                 ChangeExtension ( FileName, DOS_EXT );
                 break;
            default:
                 break;
            }
        }
    return ( TRUE );
#endif
}


//**************************************************************************
// CompareBytes
//
//      Compare bytes for an exact match for a count
//
// CompareBytes (LPSTR,LPSTR,WORD)
//
//  Input:
//      1)  First string
//      2)  Second string
//      3)  Length to compare
//  Output:
//      1)  TRUE - Match for entire length
//      2)  FALSE - Didn't match for entire length
//
//**************************************************************************

BOOL SYM_EXPORT WINAPI CompareBytes(LPSTR lpSource1, LPSTR lpSource2, WORD wLength)
{
#ifdef _ALPHA_
	//SymAssert(FALSE);
	return TRUE;
#else
    BOOL    nStatus = TRUE;
    WORD    i;

    for (i = 0; i < wLength; i++)
        {
        if (lpSource1[i] != lpSource2[i])
            nStatus = FALSE;
        }

    return (nStatus);
#endif
}

//**************************************************************************
// IntegVerifyDatabaseMatch
//
//      Verify if the database passed in matches the ID of this
//      computer.  If it does we then can process the verification
//      code.
//
//  IntegVerifyDatabaseMatch(LPSTR lpDatabase);
//
// Enter:
//      1) Database name
// Exit:
//      1) TRUE - They match.
//      2) FALSE - They dont match.
//
//**************************************************************************
BOOL SYM_EXPORT WINAPI IntegVerifyDatabaseMatch(LPSTR lpDatabase)
{
#ifdef _ALPHA_
	//SymAssert(FALSE);
	return 1;
#else
    auto    INTEGHEADER IntegHeader;
    auto    BOOL        nStatus = FALSE;
    auto    HFILE       hFile;
    auto    char        lpszID[ID_SIZE];

                                        // Initialize CERTLIBN
    InitCertlib ( &LocalCallBack , INIT_CERTLIB_USE_DEFAULT );

    MEM4_SET((LPSTR)&IntegHeader,0,sINTEGHEADER);

    hFile = FileOpen(lpDatabase,READ_ONLY_FILE);

    if ( hFile != HFILE_ERROR )
        {
        if ((FIL4_READ(hFile,(LPSTR)&IntegHeader,sINTEGHEADER)) == sINTEGHEADER)
            {
            GetIntegId (lpszID);

            if ( CompareBytes (lpszID, IntegHeader.szID, ID_SIZE) )
                nStatus = TRUE;
            }

        FIL4_CLOSE(hFile);
        }

    return (nStatus);
#endif
}

//**************************************************************************
// GetIntegId
//
//  We'll use the date and time of the NCDTREE directory since
//  it is impossible (almost) to get an valid ID from the BIOS.
//  This problem is due to QEMM and Windows.
//  Return the 16 bytes from FE00:0000 as an ID for this SYS.DAT
//  file.
//
//  (sz) modified the code so that it works under W32 as well.
//
//
//**************************************************************************
BOOL SYM_EXPORT WINAPI GetIntegId(LPSTR lpStoreHere)
{
#ifdef _ALPHA_
	//SymAssert(FALSE);
	return TRUE;
#else
    FINDDATA FileInfo;
    HFIND    hFind;
    DWORD    dwhiDateTime;
    //DWORD    dwloDateTime;

    char     szDateTime[ID_SIZE]={0};

                                        // Initialize CERTLIBN
    InitCertlib ( &LocalCallBack , INIT_CERTLIB_USE_DEFAULT );

    MEM4_SET(lpStoreHere,0, ID_SIZE);

    hFind = FileFindFirstAttr ( "C:\\NCDTREE", 0x17, &FileInfo);

    if ( hFind != INVALID_HANDLE_VALUE )
       {

       //dwloDateTime = FileInfo.ftLastWriteTime.dwLowDateTime;

       // because this is the only piece of data that's the same
       // in both dos and Windows95.
       dwhiDateTime = FileInfo.ftLastWriteTime.dwHighDateTime;
       dwhiDateTime &= MaskOutMinutes;
       MEMCPY ( lpStoreHere, &dwhiDateTime, sizeof(DWORD));

       //MEMCPY ( lpStoreHere, &(FileInfo.ftLastWriteTime), sizeof (FILETIME) );
       //FileGetDateTimeString( FileInfo.ftLastWriteTime, szDateTime, ID_SIZE-1);
       }

    return (TRUE);
#endif
}

//***************************************************************************
// GetBufferCRC()
//
// Description:
//      Compute the CRC on the buffer ignoring volume label if told.
//
// Parameters:
//      LPSTR   lpBuffer        Pointer to buffer
//      WORD    uBufSize        Size of buffer in bytes
//      BOOL    bIgnoreVolume   If TRUE, assumes buffer contains a boot
//                              sector and ignores volume label bytes
// Return Value:
//      WORD    wCRC            CRC value
//
// See Also:
//      MEM4_CRC
//
//***************************************************************************
// 06/19/1995 BARRY Function Created.
//***************************************************************************

WORD LOCAL GetBufferCRC (LPSTR lpBuffer, WORD uBufSize, BOOL bIgnoreVolume)
{
#ifdef _ALPHA_
	//SymAssert(FALSE);
	return 0;
#else
#define VOLUME_SIZE                    11     // Volume is 11 bytes long
#define VOLUME_OFFSET_FAT16            0x2B   // Starts at byte 43 in sector
#define VOLUME_OFFSET_FAT32            0x47   // Starts at byte 71 in sector
#define SECTORS_PER_FAT_OFFSET         0x16   // Starts at byte 22 in sector
#define ROOT_STARTING_CLUSTER_OFFSET   0x2C

    BYTE        abyVolume [ VOLUME_SIZE ] ;
    WORD        wCRC;
    WORD        wVolumeOffset;

                                        // Save volume and then zero out
    if (bIgnoreVolume)
        {
                                        // In FAT32, sectors per fat is zero

        wVolumeOffset = lpBuffer[SECTORS_PER_FAT_OFFSET] ?
                        VOLUME_OFFSET_FAT16 : VOLUME_OFFSET_FAT32;

        MEMCPY(abyVolume, &lpBuffer[wVolumeOffset], VOLUME_SIZE);
        MEMSET(&lpBuffer[wVolumeOffset], 0, VOLUME_SIZE);
        }

    /*-----------------08/23/97 00:54am-----------------
    // If we are on a FAT32 volume, we need to ignore //
    // the root dir starting cluster number. (SCN)    //
    //    By not handleing this, we could cause users //
    // much pain and discomfort; We'll render his/her //
    // drive unbootable).                             //
    //    Pay no attention to the late hour at which  //
    // this fix is being applied ;)                   //
    --------------------------------------------------*/
    if ( lpBuffer[SECTORS_PER_FAT_OFFSET] == 0 )
        {
        MEMSET( &lpBuffer[ROOT_STARTING_CLUSTER_OFFSET], 0, sizeof(DWORD) );
        }

    if ( uBufSize >= 512 )
        wCRC = MEM4_CRC(0, lpBuffer, 512 );
    else
        wCRC = MEM4_CRC(0, lpBuffer, uBufSize );

    if (bIgnoreVolume)
        {
        MEMCPY(&lpBuffer[wVolumeOffset], abyVolume, VOLUME_SIZE);
        }

    return wCRC;
#endif
}

//*************************************************************************
// SaveFAT32RootSCN()
//
// SaveFAT32RootSCN(
//      LPBYTE lpMemory 
//      WORD wCylinder
//      WORD wHead
//      WORD wSector
//      BYTE driveNum )
//
// Description:
//
// Returns: nothing 
//
//*************************************************************************
// 6/11/98 DBUCHES, created - header added.
//*************************************************************************

void LOCAL SaveFAT32RootSCN( LPBYTE lpMemory, 
                             WORD wCylinder,
                             WORD wHead,  
                             WORD wSector,
                             BYTE driveNum )
{
    auto    char                szDrives[SYM_MAX_DOS_DRIVES +1];
    auto    char*               p;
    auto    INT13REC            int13rec;
    auto    DISKREC             disk;
    auto    BOOL                bFound = FALSE;
    auto    FAT32BPBBOOTREC*    pBootSect = ( FAT32BPBBOOTREC* ) lpMemory;   
    auto    UINT                uMajor, uMinor;
    auto    BYTE                dl;
    auto    int                 iCount = 0;

    
    // 
    // First, examine the boot sector.  A zero in the sectors per
    // FAT field indicates a FAT32 volume.
    //
    if( pBootSect->BPB.sectorsPerFat != 0 )
        return;

    //
    // Build list of fixed drives.  Only need to do this once.
    //
    MEMSET( szDrives, 0, SYM_MAX_DOS_DRIVES + 1 );
    for( dl = 'A'; dl <= 'Z';dl++ )
        {
        uMajor = TYPE_UNKNOWN;
        DiskGetType( dl, &uMajor, &uMinor );
        if( uMajor == TYPE_FIXED )
            {
            szDrives[iCount++] = (char) dl;
            }
        }
    
    //
    // Next, figure out which logical volume this sector belongs to.
    //
    p = szDrives;
    while( *p )
        {
        MEMSET( &int13rec, 0, sizeof( INT13REC ) );
        
        // 
        // Map the first physical sector of the volume.
        //
        DiskMapLogToPhyParams( 0, *p, &int13rec );


        //
        // Make sure this is the volume we want.
        //
        if( int13rec.generated && int13rec.dn == driveNum )
            {   
            if( wHead == ( WORD ) int13rec.dwHead &&
                wCylinder == ( WORD )int13rec.dwTrack &&
                wSector == ( WORD )int13rec.dwSector )
                {
                bFound = TRUE;
                break;
                }
            }
        
        //
        // Move to next drive.
        //
        p++;
        }

    // 
    // Get root SCN from the OS.
    //
    MEMSET( &disk, 0, sizeof( DISKREC ) );
    DiskGetInfo( *p, &disk );   

    // 
    // Save off root SCN to boot record memory
    //
    pBootSect->BPB.dwRootSCN = DiskGetRootSCN( &disk );
} 



//**************************************************************************
//**************************************************************************

WORD WINAPI GetCurrentState (void )
{
#ifdef _ALPHA_
	//SymAssert(FALSE);
	return IN_DOS;
#else
    if (FileExists ( szWinbootSys ) )
       return (IN_DOS);
    else
       return (IN_WINDOWS );
#endif
}


//**************************************************************************
// WINDOWS ROUTINES
//**************************************************************************

#ifndef _ALPHA_
#ifdef SYM_WIN

//**************************************************************************
// LibMain()
//
// Parameters:
//
// Returns:
//
//**************************************************************************
// 3/\93 Martin created.
//**************************************************************************

int WINAPI LibMain (HINSTANCE hInst, WORD wDataSeg, WORD wHeapSize,
                    LPSTR lpszCmdLine)
{
#if defined SYM_WIN16
    if (wHeapSize != 0)
        UnlockData(0);
#endif

    hInstance = hInst;

    return (1);
}

//**************************************************************************
// WEP()
//
// Parameters:
//
// Returns:
//
//**************************************************************************
// 3/\93 Martin created.
//**************************************************************************

int SYM_EXPORT WINAPI WEP (int bSystemExit)
{
    return(1);
}

#endif // #ifdef SYM_WIN
#endif // #ifndef _ALPHA_
