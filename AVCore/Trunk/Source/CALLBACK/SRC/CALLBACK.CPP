// Copyright 1996 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/CALLBACK/VCS/callback.cpv   1.27   28 Aug 1998 14:54:54   JBRENNA  $
//
// Description:
//  NAV callback implementation for AVAPI 1.0.
//
// Contains:
//  gstNAVFileCallBacks;
//  gstNAVGeneralCallBacks;
//  gstNAVProgressCallBacks;
//  gstNAVDataFileCallBacks;
//  gstNAVMemoryCallBacks;
//  gstNAVLinearCallBacks;
//  gstNAVEXCallBacks;
//
// See Also:
//
//************************************************************************
// $Log:   S:/CALLBACK/VCS/callback.cpv  $
// 
//    Rev 1.27   28 Aug 1998 14:54:54   JBRENNA
// Port changes from QAKN to trunk:
// 1. Rev 1.26.1.3   28 Jul 1998 19:05:54   DBuches
//    Ported changes from Roswell branch to fix NAVXSFReadPhysical.
// 2. Rev 1.26.1.2   28 Jul 1998 18:49:52   DBuches
//    Ported changes from Roswell branch to fix NAVXSFReadPhysical.
// 3. Rev 1.26.1.1   21 Jul 1998 19:25:26   DBuches
//    Added Rekha's NEC changes.
// 
//    Rev 1.26   29 May 1998 14:59:28   DBuches
// Fixed problem with my porting.  Missed an #else.
//
//    Rev 1.25   04 May 1998 20:21:04   DBuches
// Ported changes from branch.
//
//    Rev 1.24   17 Mar 1998 12:15:46   MKEATIN
// made disk.h fixes for VXD.
//
//    Rev 1.23   16 Mar 1998 16:13:04   MKEATIN
// Made changes to match those in the new disk.h
//
//    Rev 1.22   18 Feb 1998 13:20:28   mdunn
// Ported NEC code from the ROSNEC area.
//
//    Rev 1.21   14 Nov 1997 12:32:20   DDREW
// Changed the file open mode for NAVEX callbacks to conform with Avenge
//
//    Rev 1.20   06 Nov 1997 17:51:14   DDREW
// Inverted sense of test for "heur" string compare
//
//    Rev 1.19   10 Oct 1997 18:13:10   DDREW
// Added a callback for RelinquishControl in NAVEX callbacks
//
//    Rev 1.18   18 Sep 1997 17:14:06   DDREW
// Dennis, Please learn how to spell "HeuristicLevel" korectly
//
//    Rev 1.17   18 Sep 1997 17:02:14   DDREW
//
//    Rev 1.16   17 Sep 1997 18:06:16   DDREW
// Added ability to send Heuristic level to NAVEX15
//
//    Rev 1.15   12 Sep 1997 14:32:22   KSACKIN
// Ported changes from QAKG.
//
//    Rev 1.14   03 Sep 1997 11:52:14   DDREW
// Modify NAVEXDeletFile()
//
//    Rev 1.13   25 Aug 1997 11:05:02   DDREW
// Added stub for NAVEX callabackes
//
//    Rev 1.12.1.2   21 Aug 1997 12:40:16   MKEATIN
// Fixed NAVSetEOF().
//
//    Rev 1.12.1.1   13 Aug 1997 22:27:20   RStanev
// Fixed NAVEXGetTempFile() callback one more time.
//
//    Rev 1.12   06 Aug 1997 15:34:50   RStanev
// Rewritten GetTempFIle for SYM_NTK and SYM_VXD.
//
//    Rev 1.11   01 Aug 1997 16:52:02   DDREW
// NLM platform enables
//
//    Rev 1.10   24 Jul 1997 13:26:40   DDREW
// A lot of changes to enable NAVNLM to interface to AVAPI1
//
//    Rev 1.9   23 Jun 1997 16:03:36   MKEATIN
// Fixed a bug in InfoBoxUpdate under SYM_DOSX.
//
//    Rev 1.8   20 Jun 1997 23:20:00   MKEATIN
// NAVEXFileDelete() now calls FileWipe() in SYMKRNL.
//
//    Rev 1.7   20 Jun 1997 21:28:12   MKEATIN
// NAVEXFileDelete() now writes zeros to the file and truncates the file
// before deleting it.
//
//    Rev 1.6   20 Jun 1997 14:08:16   MKEATIN
// Changed FA_ARCHIVE to FA_NORMAL.
//
//    Rev 1.5   20 Jun 1997 13:59:34   MKEATIN
// Changed the file attribute in the call to FileCreateTemporary from
// FA_TEMORARY to FA_ARCHIVE.  FA_TEMPORARY would fail under DX.
//
//    Rev 1.4   18 Jun 1997 22:18:52   JBRENNA
// Put _T() wrapper around s_szNAVPath initialization.
//
//    Rev 1.3   18 Jun 1997 18:31:50   MKEATIN
// Removed VLGetDefsLocation() from NAVGetTempFile().
//
//    Rev 1.2   18 Jun 1997 16:34:00   MKEATIN
// Fixed paramters for NAVSetDefsPath().
//
//    Rev 1.1   18 Jun 1997 16:26:48   MKEATIN
// Added NAVSetDefsPath().
//
//    Rev 1.0   18 Jun 1997 13:16:30   MKEATIN
// Initial revision.
//
//    Rev 1.2   17 Jun 1997 16:05:52   MKEATIN
// Fixed NAVGetTempFile for the VXD platform.
//
//    Rev 1.1   17 Jun 1997 15:37:38   MKEATIN
// gstNAVCallBacks is only extern "C"-ed for SYM_DOSX.
//
//    Rev 1.0   17 Jun 1997 15:08:34   MKEATIN
// Initial revision.
//
//    Rev 1.19   13 Jun 1997 20:20:12   MKEATIN
// Fixed a parameter name.
//
//    Rev 1.18   13 Jun 1997 20:17:10   MKEATIN
// Fixed NAVGetConfigInfo() so NAVEX can call it to get any command line
//
//    Rev 1.17   12 Jun 1997 19:09:40   MKEATIN
// Updated NAVEXGetConfigInfo to check for command line switches.
//
//    Rev 1.16   12 Jun 1997 14:13:04   MKEATIN
//
//    Rev 1.15   09 Jun 1997 03:18:12   BGERHAR
// AreWildDefs as C function
//
//    Rev 1.14   09 Jun 1997 02:48:34   BGERHAR
// Support wild def set for DOS platform
//
//    Rev 1.13   06 Jun 1997 15:27:58   MKEATIN
// The new RelinquishControl function is initialized to NULL in
// gstNAVEXCallBacks.
//
//    Rev 1.12   03 Jun 1997 14:36:44   RStanev
// Fixed a typo.
//
//    Rev 1.11   03 Jun 1997 14:32:48   RStanev
// NAVEXGetTempFile() now calls FileCreate() for SYM_VXD and SYM_NTK.
//
//    Rev 1.10   02 Jun 1997 14:38:18   MKEATIN
// Updated NAVEXMessageBox for the VXD platform.
//
//    Rev 1.9   02 Jun 1997 10:54:46   MKEATIN
// Fixed a temporary problem with HFILE in the VXD/NTK platform.
//
//    Rev 1.8   30 May 1997 18:57:14   MKEATIN
// Implemented more CALLBACKREV2 callback funtions.
//
//    Rev 1.7   23 May 1997 15:29:46   MKEATIN
// It turns out that we shouldn't be allocating Perm or Fixed memory
// from NAVEX. So, NAVEXPermMemoryAlloc() should just pass GHND to
// MemAllocPtr().
//
//    Rev 1.6   23 May 1997 14:43:48   MKEATIN
// Made sure NAVEXPermMemoryAlloc() is allocating locked memory.
// Appearantly MemAllocPtr() always allocates locked memory in SYM_WIN
// platforms.  SYM_VXD and SYM_NTK platforms, however, must pass GMEM_FIXED
// to the uflags parameter.
//
//    Rev 1.5   23 May 1997 14:15:52   MKEATIN
// Our mutex is now allocatated in locked memory.  Also Added NTK platform
// to mutex callbacks.
//
//    Rev 1.4   09 May 1997 18:11:12   MKEATIN
// Rename LocalNT* to NAVEX_NT* to avoid conficts with avglob in certlib.
//
//    Rev 1.3   09 May 1997 11:31:12   MKEATIN
// Changed VirusScanLockUnlock... to ScanLockUnlock... -avoids linker
// conflicts with certlib in the DX platform.
//
//    Rev 1.2   08 May 1997 15:49:20   MKEATIN
// Remove the dreaded lpgCallBack and upgraded to CALLBACKREV2 and NAVEX 1.5
// support
//
//    Rev 1.1   01 May 1997 11:25:16   MKEATIN
// Made string handling unicode compatable
//
//    Rev 1.0   28 Apr 1997 13:44:02   MKEATIN
// Initial revision.
//************************************************************************

#include "platform.h"
#include "file.h"
#include "disk.h"
#include "xapi.h"
#include "symsync.h"
#include "symcfg.h"

#include "stdio.h"
#include "conio.h"

#include "avtypes.h"
#include "avcb.h"
#include "pamapi.h"
#include "avenge.h"
#include "callbk15.h"
#include "callw32.h"

#define BUILDING_CALLBACK

#include "navcb.h"
#include "avapi.h"

#ifdef SYM_NLM
#include "navnlmex.h"
#include "symserve.h"
#endif


#if defined(SYM_WIN32) && !defined(SYM_VXD) && !defined(SYM_NTK)
  #include "winioctl.h"
  #include "NAVEX_NT.h"
#elif defined (SYM_VXD)
  #include "symevent.h"
#elif defined(SYM_DOSX)
  #include "ResqLoc.h"
#endif

#ifdef SYM_DOS
  #define DOS_LOADDS    LOADDS
#else
  #define DOS_LOADDS
#endif


// TEMPORARY FIX ... Long term fix is to remove this #if block ...
// The Callback project, like the rest of AVCore, uses only MSVC 5.0; we need 6.0
// to use these additional WINIOCTL.H #defines (which are strictly for Windows 2000). 
// The long-term fix is to change AVCore over to MSVC 6 (after Iliad ships), and then 
// cut out everything down to 'End TEMPORARY FIX' below.
#if defined(SYM_WIN32) && !defined(SYM_VXD) && !defined(SYM_NTK)

    #define IOCTL_STORAGE_GET_DEVICE_NUMBER CTL_CODE(IOCTL_STORAGE_BASE, 0x0420, METHOD_BUFFERED, FILE_ANY_ACCESS)

    //
    // IOCTL_STORAGE_GET_DEVICE_NUMBER
    //
    // input - none
    //
    // output - STORAGE_DEVICE_NUMBER structure
    //          The values in the STORAGE_DEVICE_NUMBER structure are guaranteed
    //          to remain unchanged until the system is rebooted.  They are not
    //          guaranteed to be persistant across boots.
    //

    typedef struct _STORAGE_DEVICE_NUMBER {

        //
        // The FILE_DEVICE_XXX type for this device.
        //
    
        DEVICE_TYPE DeviceType;
    
        //
        // The number of this device
        //

        DWORD       DeviceNumber;

        //
        // If the device is partitionable, the partition number of the device.
        // Otherwise -1
        //

        DWORD       PartitionNumber;
    } STORAGE_DEVICE_NUMBER, *PSTORAGE_DEVICE_NUMBER;
#endif
// End TEMPORARY FIX

static TCHAR s_szNAVPath[SYM_MAX_PATH +1] = _T("");

//=======================================================================
//
//  Local Prototypes
//
//=======================================================================
CBSTATUS NAVLinearConvertToPhysical
(
    LPVOID      lpvInfo,                // [in] Linear object info
    DWORD       dwSectorLow,            // [in] Linear sector low dword
    LPBYTE      lpbySide,               // [out] Translated side, ...
    LPWORD      lpwCylinder,            // [out] cylinder, and ...
    LPBYTE      lpbySector              // [out] sector
);

BOOL WINAPI ScanLockUnlockDiskP
(
    UINT    uOperation,                 // [in] 0 - lock, 1 - unlock
    BYTE    byDrive,                    // [in] drive to lock/unlock
    UINT    uRequestedAccess            // [in] requested access right
);

UINT DOS_LOADDS SYM_EXPORT WINAPI NAVEXSDiskReadPhysical
(
    LPVOID  lpBuffer,
    BYTE    bCount,
    WORD    wCylinder,
    BYTE    bSector,
    BYTE    bHead,
    BYTE    bDrive
);

UINT DOS_LOADDS WINAPI NAVEXSDiskWritePhysical
(
    LPVOID  lpBuffer,
    BYTE    bCount,
    WORD    wCylinder,
    BYTE    bSector,
    BYTE    bHead,
    BYTE    bDrive
);

                                        // local defines
#define LOCK_DRIVE          0
#define UNLOCK_DRIVE        1

#define DISK_READ_WRITE     1
#define DISK_READ_ONLY      2

CBSTATUS NAVFileOpen
(
    LPVOID      lpvFileInfo,
    WORD        wOpenMode
)
{
    LPFILEINFORMATION  lpFileInfo = (LPFILEINFORMATION)lpvFileInfo;
    HFILE hFile;

#ifndef SYM_NLM
    hFile = FileOpen(lpFileInfo->lpstrPathName,wOpenMode);

#if defined(SYM_VXD)
    if ( hFile == HFILE_ERROR ) {
        auto char szFilename[SYM_MAX_PATH * sizeof(WCHAR)];

        VxDGenericStringConvert ( lpFileInfo->lpstrPathName, CHARSET_OEM,
                                  szFilename, CHARSET_CURRENT );

        hFile = FileOpen( szFilename,wOpenMode);
        }
#elif defined(SYM_DOSX)

    // If we just failed to open the file.  See if we're looking for a def file
    // on a rescue disk - the def set may span multiple disks.

    if (hFile == HFILE_ERROR)
    {
        char szResqLocFile[SYM_MAX_PATH];
        NameReturnPath(lpFileInfo->lpstrPathName, szResqLocFile);
        NameAppendFile(szResqLocFile, RESQLOC_INI_FILE);

        if (!DiskIsFixed(DiskGet()) &&  // this only applies to floppies with
            FileExists(szResqLocFile)); // ResqLoc.dat present
        {
            // look for the disk label listed in ResqLoc.dat

            char szDefFileName[SYM_MAX_PATH];
            NameReturnFile(lpFileInfo->lpstrPathName, szDefFileName);

            char szDiskLabel[SYM_MAX_PATH];
            GetPrivateProfileString(RESQDEFSLOC_SECTION,
                                    szDefFileName,
                                    KEY_MISSING,
                                    szDiskLabel,
                                    sizeof(szDiskLabel),
                                    szResqLocFile);

            // If we have a disk label to prompt the user with, do so

            if (STRCMP(szDiskLabel, KEY_MISSING))
            {
                extern char SZ_INSERT_RESCUE_DISK[];
                extern char SZ_PRESS_ENTER[];

                for (;;)
                {
                    printf("\n\n%s \"%s\"\n\n%s", SZ_INSERT_RESCUE_DISK, szDiskLabel, SZ_PRESS_ENTER);

                    int ch = getch();
                    if (ch == 27)       // ESCape
                    {
                        break;
                    }
                    else if (ch == 13)  // ENTER
                    {
                        hFile = FileOpen(lpFileInfo->lpstrPathName,wOpenMode);

                        if (hFile != HFILE_ERROR)
                        {
                            // We found and successfully opened to data file!!

                            break;
                        }
                    }
                }
            }
        }
    }
#elif defined(SYM_WIN32)
	// This is evil, but appears to be neccessary.  Problem here is that
	// while NAVW is performing a scan, file APIs are in OEM.  This is
	// a problem because on some international platforms, the directory
	// containing our definitions may contain characters without OEM
	// equivilants.  The kludge is that when a file open fails, and we
	// are in OEM mode, attempt to open the file in ANSI mode.
	if( hFile == HFILE_ERROR )
		{
		// Q: are we in OEM mode?
		if( !AreFileApisANSI() )
			{
			// Yes, set APIs to ANSI and attempt to open file again
			SetFileApisToANSI();

			hFile = FileOpen(lpFileInfo->lpstrPathName,wOpenMode);

			// Set back to OEM
			SetFileApisToOEM();
			}
		}
#endif

#else
    hFile = NV_FileOpen(lpFileInfo->lpstrPathName,wOpenMode);
#endif
    if (hFile == HFILE_ERROR)
        return(CBSTATUS_ERROR);

    lpFileInfo->hFile = hFile;

    return(CBSTATUS_OK);
}

CBSTATUS NAVFileClose
(
    LPVOID      lpvFileInfo
)
{
    HFILE hFile;
    LPFILEINFORMATION  lpFileInfo = (LPFILEINFORMATION)lpvFileInfo;

    hFile = FileClose(lpFileInfo->hFile);
    if (hFile == HFILE_ERROR)
        return(CBSTATUS_ERROR);

    return(CBSTATUS_OK);
}

CBSTATUS NAVFileSeek
(
    LPVOID      lpvFileInfo,
    long        lOffset,
    WORD        wWhence,
    LPDWORD     lpdwNewOffset
)
{
    LPFILEINFORMATION  lpFileInfo = (LPFILEINFORMATION)lpvFileInfo;

    *lpdwNewOffset = FileSeek(lpFileInfo->hFile,lOffset,wWhence);

    if (*lpdwNewOffset == -1)
        return(CBSTATUS_ERROR);

    return(CBSTATUS_OK);
}

CBSTATUS NAVFileRead
(
    LPVOID      lpvFileInfo,
    LPVOID      lpvBuffer,
    DWORD       dwNumBytes,
    LPDWORD     lpdwNumActualBytes
)
{
    LPFILEINFORMATION  lpFileInfo = (LPFILEINFORMATION)lpvFileInfo;

    // Only attempt read if bytes to read is non-zero

    if (dwNumBytes != 0)
    {

        UINT    uiBytesRead = FileRead(lpFileInfo->hFile,
                                       lpvBuffer,
                                       (WORD)dwNumBytes);
        if (uiBytesRead == -1)
            return(CBSTATUS_ERROR);

        *lpdwNumActualBytes = uiBytesRead;
    }
    else
        *lpdwNumActualBytes = 0;

    return(CBSTATUS_OK);
}

CBSTATUS NAVFileWrite
(
    LPVOID      lpvFileInfo,
    LPVOID      lpvBuffer,
    DWORD       dwNumBytes,
    LPDWORD     lpdwNumActualBytes
)
{
    LPFILEINFORMATION  lpFileInfo = (LPFILEINFORMATION)lpvFileInfo;
    ;

    // Only attempt write if bytes to write is non-zero

    if (dwNumBytes != 0)
    {
        UINT    uiBytesRead = FileWrite(lpFileInfo->hFile,
                                        lpvBuffer,
                                        (WORD)dwNumBytes);

        if (uiBytesRead == -1)
            return(CBSTATUS_ERROR);

        *lpdwNumActualBytes = uiBytesRead;
    }
    else
        *lpdwNumActualBytes = 0;

    return(CBSTATUS_OK);
}

CBSTATUS NAVFileTell
(
    LPVOID      lpvFileInfo,
    LPDWORD     lpdwOffset
)
{
    LPFILEINFORMATION  lpFileInfo = (LPFILEINFORMATION)lpvFileInfo;
    DWORD       dwOffset;

    dwOffset = FileSeek(lpFileInfo->hFile,0,SEEK_CUR);

    if (dwOffset == -1)
        return(CBSTATUS_ERROR);

    *lpdwOffset = dwOffset;

    return(CBSTATUS_OK);
}

CBSTATUS NAVFileSetEOF
(
    LPVOID      lpvFileInfo
)
{
    LPFILEINFORMATION  lpFileInfo = (LPFILEINFORMATION)lpvFileInfo;

#if defined(SYM_WIN32)                  // truncate the file

    if (!SetEndOfFile((HANDLE) lpFileInfo->hFile))
    {
        return(CBSTATUS_ERROR);
    }
                                        // writing 0 bytes will truncate
#else                                   // the file

    if (FileWrite(lpFileInfo->hFile, "", 0) == HFILE_ERROR)
    {
        return(CBSTATUS_ERROR);
    }

#endif

    return(CBSTATUS_OK);
}

CBSTATUS NAVFileEOF
(
    LPVOID lpvFileInfo,
    LPBOOL lpbEOF
)
{
//    LPFILEINFORMATION  lpFileInfo = (LPFILEINFORMATION)lpvFileInfo;
//    int iStatus;
//
//    iStatus = _eof(lpFileInfo->hFile);
//
//    if (iStatus == -1)
//        return(CBSTATUS_ERROR);
//
//    *lpbEOF = iStatus;

//    return(CBSTATUS_OK);

      return(CBSTATUS_ERROR);
}

CBSTATUS NAVFileGetAttr
(
    LPVOID  lpvFileInfo,
    WORD    wType,
    LPDWORD lpdwInfo
)
{
    return(CBSTATUS_ERROR);
}

CBSTATUS NAVFileSetAttr
(
    LPVOID  lpvFileInfo,
    WORD    wType,
	DWORD   dwInfo
)
{
    return(CBSTATUS_ERROR);
}

CBSTATUS NAVFileGetDateTime
(
    LPVOID  lpvFileInfo,
    WORD    wType,
    LPWORD  lpwDate,
    LPWORD  lpwTime
)
{
    return(CBSTATUS_OK);
}

CBSTATUS NAVFileSetDateTime
(
	LPVOID  lpvFileInfo,
	WORD    wType,
    WORD    wDate,
    WORD    wTime
)
{
    return(CBSTATUS_OK);
}

CBSTATUS NAVFileFullNameAndPath
(
    LPVOID  lpvFileInfo,
    LPWSTR  lpwstrFileName,
    WORD    wBufferLen
)
{
	(void)lpvFileInfo;
    (void)lpwstrFileName;
	(void)wBufferLen;

    return(CBSTATUS_ERROR);
}

CBSTATUS NAVFileName
(
    LPVOID  lpvFileInfo,
    LPWSTR  lpwstrFileName,
    WORD    wBufferLen
)
{
	(void)lpvFileInfo;
    (void)lpwstrFileName;
	(void)wBufferLen;

    return(CBSTATUS_ERROR);
}

CBSTATUS NAVFilePath
(
	LPVOID  lpvFileInfo,
    LPWSTR  lpwstrFilePath,
	WORD    wBufferLen
)
{
	(void)lpvFileInfo;
    (void)lpwstrFilePath;
	(void)wBufferLen;

    return(CBSTATUS_ERROR);
}

CBSTATUS NAVFileLogicalDrive
(
	LPVOID  lpvFileInfo,
    LPWSTR  lpwstrDriveLetter,
	WORD    wBufferLen
)
{
	(void)lpvFileInfo;
    (void)lpwstrDriveLetter;
	(void)wBufferLen;

    return(CBSTATUS_ERROR);
}

CBSTATUS NAVFileLength
(
    LPVOID  lpvFileInfo,
    LPDWORD lpdwFileLength
)
{
    LPFILEINFORMATION  lpFileInfo = (LPFILEINFORMATION)lpvFileInfo;
    DWORD       dwLength;

    dwLength = FileLength(lpFileInfo->hFile);

    if (dwLength == (DWORD)-1)
        return(CBSTATUS_ERROR);

    *lpdwFileLength = dwLength;

    return(CBSTATUS_OK);
}

CBSTATUS NAVFileDelete
(
	LPVOID  lpvFileInfo
)
{
    return(CBSTATUS_ERROR);
}

CBSTATUS NAVFileCopy
(
    LPVOID  lpvSrcFileInfo,
    LPVOID  lpvDstFileInfo
)
{
    return(CBSTATUS_ERROR);
}

CBSTATUS NAVFileRename
(
    LPVOID  lpvOldFileInfo,
    LPWSTR  lpwstrNewName
)
{
    return(CBSTATUS_ERROR);
}

CBSTATUS NAVFileExists
(
    LPVOID  lpvFileInfo,
    LPBOOL	lpbExists
)
{
    return(CBSTATUS_ERROR);
}

CBSTATUS NAVFileGetErrorCondition
(
    LPVOID  lpvFileInfo,
    LPDWORD lpdwErrorCondition
)
{
    (void)lpvFileInfo;

    *lpdwErrorCondition = 0;

    return(CBSTATUS_OK);
}


CBSTATUS NAVMemoryAlloc
(
    DWORD        dwSize,
    WORD         wMemType,
    LPLPVOID     lplpvBuffer
)
{
    LPVOID	lpvData;

    (void)wMemType;

    lpvData = MemAllocPtr(GHND, dwSize);

    if (lpvData == NULL)
    	return(CBSTATUS_ERROR);

    *lplpvBuffer = lpvData;

    return(CBSTATUS_OK);
}

CBSTATUS NAVMemoryFree
(
    LPVOID lpvBuffer
)
{
    MemFreePtr (lpvBuffer);

    return(CBSTATUS_OK);
}

#ifdef SYM_DOS                          // MAJOR Kludge to support wild set for DOS
    extern "C" BOOL PASCAL AreDefsWild(VOID);
#endif

CBSTATUS NAVGetDataFileInfo
(
    LPLPVOID        lplpvFileInfo,
    WORD            wDataFileNum
)
{
    LPFILEINFORMATION   lpFileInfo;
    BOOL                bWildDefs = FALSE;

#ifdef SYM_DOS
    bWildDefs = AreDefsWild();
#endif

    lpFileInfo = (LPFILEINFORMATION)MemAllocPtr(GHND,sizeof(FILEINFO_T));
    if (lpFileInfo == NULL)
        return(CBSTATUS_ERROR);

    lpFileInfo->hFile = NULL;

    STRCPY(lpFileInfo->lpstrPathName, s_szNAVPath);

    switch (wDataFileNum)
    {
        case DATA_FILE_ID_VIRSCAN1DAT:
            if (bWildDefs)
                NameAppendFile(lpFileInfo->lpstrPathName,_T("VIRWILD1.DAT"));
            else
                NameAppendFile(lpFileInfo->lpstrPathName,_T("VIRSCAN1.DAT"));
            break;
        case DATA_FILE_ID_VIRSCAN2DAT:
            if (bWildDefs)
                NameAppendFile(lpFileInfo->lpstrPathName,_T("VIRWILD2.DAT"));
            else
                NameAppendFile(lpFileInfo->lpstrPathName,_T("VIRSCAN2.DAT"));
            break;
        case DATA_FILE_ID_VIRSCAN3DAT:
            if (bWildDefs)
                NameAppendFile(lpFileInfo->lpstrPathName,_T("VIRWILD3.DAT"));
            else
                NameAppendFile(lpFileInfo->lpstrPathName,_T("VIRSCAN3.DAT"));
            break;
        case DATA_FILE_ID_VIRSCAN4DAT:
            if (bWildDefs)
                NameAppendFile(lpFileInfo->lpstrPathName,_T("VIRWILD4.DAT"));
            else
                NameAppendFile(lpFileInfo->lpstrPathName,_T("VIRSCAN4.DAT"));
            break;
        default:
            return(CBSTATUS_ERROR);
    }

    *lplpvFileInfo = (LPVOID)lpFileInfo;
    return(CBSTATUS_OK);
}


CBSTATUS NAVCloseDataFileInfo
(
    LPVOID lpvFileInfo
)
{
    MemFreePtr(lpvFileInfo);

    return(CBSTATUS_OK);
}

CBSTATUS NAVMemGetPtrToMem
(
    LPVOID       lpvInfo,
    DWORD        dwMemAddress,
    LPBYTE       lpbyMemBuf,
    DWORD        dwMemSize,
    LPLPBYTE     lplpbyMemPtr,
    LPCOOKIETRAY lpCookieTray
)
{
    WORD        wSeg;
    WORD        wOff;
    DWORD       dwSegOffPtr;

    wSeg = (WORD)(dwMemAddress >> 4);
    wOff = (WORD)(dwMemAddress & 0xF);

    dwSegOffPtr = ((((DWORD)wSeg) << 16) | (DWORD)(wOff));

#if defined(SYM_DOSX) || defined(SYM_WIN32) || defined(SYM_VXD)

    *lplpbyMemPtr = (LPBYTE)GetProtModePtr((LPVOID)dwSegOffPtr);

    return(CBSTATUS_OK);

#else

    return(CBSTATUS_ERROR);

#endif
}

CBSTATUS NAVMemReleasePtrToMem
(
    LPVOID       lpvInfo,
    LPBYTE       lpbyMemPtr,
    LPCOOKIETRAY lpCookieTray
)
{

#if defined(SYM_DOSX)

    SelectorFree(FP_SEG(lpbyMemPtr));

#endif

    return(CBSTATUS_OK);
}

//********************************************************************
//
// Function:
//  NAVProgressInit()
//
// Parameters:
//  lptstrObjectName        String containing the object name being worked on
//  dwLevel                 What level of processing is this object?
//  dwProgressValueMax      What's our MAX progress value? (100%, etc?)
//  lpCookieTray            Pointer to a structure of our global, local, and
//                          transient cookies.
//  lplpvData               Set to point to the application's progress data
//
// Description:
//  This function is called to initialize a progress context.  This progress
//  context is used during scanning or any other task that takes noticable
//  time.  The progress context is passed along through the engine and back
//  to the ProgressUpdate and ProgressClose functions below to update the
//  application (and the user) as to the progress of the engine.
//
// Returns:
//  CBSTATUS_OK         On successful progress context creation
//  CBSTATUS_ERROR      If unable to create a progress context
//
//********************************************************************

CBSTATUS NAVProgressInit
(
    LPWSTR       lpwstrObjectName,
    DWORD        dwLevel,
    DWORD        dwProgressValueMax,
    LPCOOKIETRAY lpCookieTray,
    LPLPVOID     lplpvData
)
{
    return(CBSTATUS_OK);
}


//********************************************************************
//
// Function:
//  NAVProgressUpdate()
//
// Parameters:
//  lpvData             Point to the application's progress data
//  dwProgressValue     Value between 0 and dwProgressValueMax
//
// Description:
//  This function is called by the engine to update the application on the
//  progress of a given task.  The dwProgressValue is used to indicate how
//  much longer the engine must work before it has completed its task.
//
// Returns:
//  CBSTATUS_OK         If the engine should continue on its merry way
//  CBSTATUS_ABORT      If the application/user wants to abort the current
//                      engine taks.
//  CBSTATUS_ERROR      If there is an error (engine should close immediately)
//
//********************************************************************

CBSTATUS NAVProgressUpdate
(
    LPVOID       lpvData,
    DWORD        dwProgressValue
)
{
#ifdef SYM_NLM

    RelinquishControl();

#endif
    return(CBSTATUS_OK);
}

//********************************************************************
//
// Function:
//  NAVProgressClose()
//
// Parameters:
//  lpvData             Point to the application's progress data
//
// Description:
//  This function is called by the engine once a task is completed. It frees
//  all context information and returns.
//
// Returns:
//  CBSTATUS_OK         If the engine should continue on its merry way
//  CBSTATUS_ERROR      If there is an error (engine should close immediately)
//
//********************************************************************


CBSTATUS NAVProgressClose
(
    LPVOID       lpvData
)
{
    return(CBSTATUS_OK);
}


#ifndef SYM_NLM

CBSTATUS NAVLinearOpen
(
    LPVOID  lpvInfo,                // [in] Linear object info
    WORD    wOpenMode               // [in] filler
)
{
    return(CBSTATUS_OK);
}


CBSTATUS NAVLinearClose
(
    LPVOID  lpvInfo                 // [in] Linear object info
)
{
    return(CBSTATUS_OK);
}


CBSTATUS NAVLinearRead
(
    LPVOID  lpvInfo,                // [in] Linear object info
    DWORD   dwSectorLow,            // [in] Sector to begin reading
    DWORD   dwSectorHigh,           // [in] Sector's (high dword)
    LPBYTE  lpbyBuffer,             // [out] Buffer to read into
    DWORD   dwLength,               // [in] Sectors to read
    LPDWORD lpdwActualLength        // [out] Sectors read
)
{
    BYTE bySide;
    WORD wCylinder;
    BYTE bySector;

    UINT i, uResult;

    if (NAVLinearConvertToPhysical (lpvInfo,
                                    dwSectorLow,
                                    &bySide,
                                    &wCylinder,
                                    &bySector ) == CBSTATUS_ERROR)
    {
        return(CBSTATUS_ERROR);
    }

    for (i = 0; i < 2; i++)             // two attempts
    {
        // This NAVEXSDiskReadPhysical aligns lpbyBuffer on a 4k bounder before calling
        // DiskAbsOperation().  This is necessary for Promise and MO drives.

        uResult = NAVEXSDiskReadPhysical(lpbyBuffer, 1, wCylinder, bySector, bySide, (BYTE)lpvInfo);

        if (!uResult)                   // Success
        {
            *lpdwActualLength = dwLength;
            break;
        }
    }

    return (!uResult ? CBSTATUS_OK : CBSTATUS_ERROR);
}

CBSTATUS NAVLinearWrite
(
    LPVOID  lpvInfo,                    // [in]  Linear object info
    DWORD   dwSectorLow,                // [in]  Sector to begin writing
    DWORD   dwSectorHigh,               // [in]  Sector (high dword)
    LPBYTE  lpbyBuffer,                 // [out] Buffer to write from
    DWORD   dwLength,                   // [in]  Sectors to write
    LPDWORD lpdwActualLength            // [in/out] Sectors written
)
{
    BYTE bySide;
    WORD wCylinder;
    BYTE bySector;

    UINT i, uResult;

    if (NAVLinearConvertToPhysical (lpvInfo,
                                    dwSectorLow,
                                    &bySide,
                                    &wCylinder,
                                    &bySector ) == CBSTATUS_ERROR)
    {
        return(CBSTATUS_ERROR);
    }

    for (i = 0; i < 2; i++)             // two attempts
    {
        // This NAVEXSDiskReadPhysical aligns lpbyBuffer on a 4k bounder before calling
        // DiskAbsOperation().  This is necessary for Promise and MO drives.

        uResult = NAVEXSDiskWritePhysical(lpbyBuffer, 1, wCylinder, bySector, bySide, (BYTE)lpvInfo);

        if (!uResult)                   // Success
        {
            *lpdwActualLength = dwLength;
            break;
        }
    }

    return (!uResult ? CBSTATUS_OK : CBSTATUS_ERROR);
}

CBSTATUS NAVLinearGetDimensions
(
    LPVOID  lpvInfo,                    // [in]  Linear object info
    LPBYTE  lpbySides,                  // [out] Total Sides
    LPWORD  lpwCylinders,               // [out] Total Cylinders
    LPBYTE  lpbySectors                 // [out] Sectors per cylinder
)
{
    ABSDISKREC  stDiskRec = { 0 };

    stDiskRec.dn = (BYTE)lpvInfo;

    if ( !HWIsNEC() )
        {
        if (DiskGetPhysicalInfo (&stDiskRec))
            {
            return(CBSTATUS_ERROR);
            }

        *lpbySides    = (BYTE)stDiskRec.dwTotalHeads;
        *lpwCylinders = (WORD)stDiskRec.dwTotalTracks;
        *lpbySectors  = (BYTE)stDiskRec.dwSectorsPerTrack;
        }
    else                                // this IS a NEC PC98xx  
    {                                   // NOTE: floppy disk is assumed!
        BPBBootRec *lpBPBBootRec;       
        // Increasing this buffer to 4KB caused stack checking (__chkstk) in IRUK platform
        // which caused an unresolved external error when building NAVAP.SYS. Since this is
        // for NEC floppy, 2KB will suffice.
        BYTE       BootSector[4 * SECTOR_SIZE];

        stDiskRec.dwHead     = 0;
        stDiskRec.dwTrack    = 0;
        stDiskRec.dwSector   = 1;
        stDiskRec.numSectors = 1;
        stDiskRec.buffer     = (HPBYTE)&BootSector;

                                        // Read the Boot record

        if (DiskAbsOperation (READ_COMMAND, &stDiskRec))
        {
            return(CBSTATUS_ERROR);
        }

        lpBPBBootRec = (BPBBootRec *)&BootSector;     
                                        
                                        // Determine the disk dimensions 
                                        // from the floppy type

        switch (lpBPBBootRec->BPB.mediaDescriptor)
        {
            case 0xFD:                  
                                        // 360k diskette
                *lpwCylinders = 40;
                *lpbySectors  = 9;
                *lpbySides    = 2;

                break;

            case 0xF9:
                                        // 720k or 1.2M diskette
                *lpwCylinders = 80;
                *lpbySectors  = (BYTE)lpBPBBootRec->BPB.sectorsPerTrack;
                *lpbySides    = 2;

                break;

            case 0xF0:
                                        // 1.44M or 2.88M diskette
                *lpwCylinders = 80;
                *lpbySectors  = (BYTE)lpBPBBootRec->BPB.sectorsPerTrack;
                *lpbySides    = 2;

                break;

            case 0xFE:
                                        // 1.2M (1k/sector) diskette
                *lpwCylinders = 80;
                *lpbySectors  = 8;
                *lpbySides    = 2;

                break;

            case 0xFC:
                                        // 180K diskette
                *lpwCylinders = 40;
                *lpbySectors  = 9;
                *lpbySides    = 1;

                break;

            case 0xFF:
                                        // 320K diskette
                *lpwCylinders = 40;
                *lpbySectors  = 8;
                *lpbySides    = 2;

                break;

            case 0xFB:
                                        // 640K diskette
                *lpwCylinders = 80;
                *lpbySectors  = 8;
                *lpbySides    = 2;

                break;

            default:

                return(CBSTATUS_ERROR);
        }
    }

    return (CBSTATUS_OK);
}

CBSTATUS NAVLinearConvertFromPhysical
(
    LPVOID      lpvInfo,                // [in] Linear object info
    BYTE        bySide,                 // [in] Physical side, ...
    WORD        wCylinder,              // [in] cylinder, and ...
    BYTE        bySector,               // [in] sector to translate
    LPDWORD     lpdwSectorLow           // [out] Translated sector low dword
)
{
    BYTE bySides;
    WORD wCylinders;
    BYTE bySectors;

    CBSTATUS cbResult;

    cbResult = NAVLinearGetDimensions (lpvInfo,
                                       &bySides,
                                       &wCylinders,
                                       &bySectors);

    if (CBSTATUS_OK == cbResult)
    {
        *lpdwSectorLow = ((DWORD) wCylinder * bySectors * bySides ) +
                         ((DWORD) bySide * bySectors ) +
                         ((DWORD) bySector - 1 );
    }

    return(cbResult);
}

CBSTATUS NAVLinearConvertToPhysical
(
    LPVOID      lpvInfo,                // [in] Linear object info
    DWORD       dwSectorLow,            // [in] Linear sector low dword
    LPBYTE      lpbySide,               // [out] Translated side, ...
    LPWORD      lpwCylinder,            // [out] cylinder, and ...
    LPBYTE      lpbySector              // [out] sector
)
{
    BYTE bySides;
    WORD wCylinders;
    BYTE bySectors;

    CBSTATUS cbResult;

    cbResult = NAVLinearGetDimensions ( lpvInfo, &bySides, &wCylinders, &bySectors );

    if (CBSTATUS_OK == cbResult)
    {
        *lpwCylinder = (WORD) (dwSectorLow / ( (WORD) bySectors * (WORD) bySides ));
        dwSectorLow -= (DWORD) *lpwCylinder * ( (WORD) bySectors * (WORD) bySides );
        *lpbySide    = (BYTE) (dwSectorLow / bySectors);
        *lpbySector  = (BYTE) (dwSectorLow % bySectors) + 1;
    }

    return(cbResult);
}

CBSTATUS SYM_EXPORT NAVLinearIsPartitionedMedia
(
    LPVOID lpvInfo,
    LPBOOL lpbPartitioned
)
#if defined(SYM_VXD) || defined(SYM_NTK)
// This is a temporary fix, since DiskIsValidBPB() is not readily available
// for these platforms. So for right now this function has been rolled back
// to it's Feb. 99 state for just these two platforms:
{
    *lpbPartitioned = (BYTE)lpvInfo < 0x80 || HWIsNEC() ? FALSE : TRUE;

    return(CBSTATUS_OK);
}
#else
{
    if (HWIsNEC())
    {
        *lpbPartitioned = FALSE;
        return CBSTATUS_OK;
    }

    // if this is a floppy drive, it definitely is not partitioned media.

    if ((BYTE)lpvInfo < 0x80)
    {
        *lpbPartitioned = FALSE;
        return CBSTATUS_OK;
    }
    
    // we may still be not be partitionable even though we are greater
    // than drive 0x80 - but we must be careful before assuming
    // we're not paritionable at this point.

    // NOTE: if we err in the function, it should err on the side of
    // assuming paritionable media.  This function always gets called when
    // repairing boot records.  If we assume that the boot record is
    // at physical sector zero, when an MBR is really threre, we will
    // likely corrupt the MBR and render the user's hard drive unusable

    // allocate a buffer

    LPVOID lpMBRBuffer = MemAllocPtr(GHND, 8 * PHYSICAL_SECTOR_SIZE);

    if (!lpMBRBuffer)
    {
        return CBSTATUS_ERROR;
    }

    // read the first sector of the drive

    if (NAVEXSDiskReadPhysical(lpMBRBuffer, 1, 0, 1, 0, (BYTE)lpvInfo))
    {
        MemFreePtr(lpMBRBuffer);
        return CBSTATUS_ERROR;
    }

    //  A invalid BPB means that we assume we are partitionable
    //  since this is is not a boot reocord at physical sector zero

    FAT32BPBREC BPB;

    //  Get the possible BPB

    BPB = *((FAT32BPBREC *)((LPBYTE)lpMBRBuffer + 11));

    if (!DiskIsValidBPB((LPBPBREC)&BPB))
    {
        *lpbPartitioned = TRUE;
        MemFreePtr(lpMBRBuffer);

        return CBSTATUS_OK;
    }

    // this looks like a boot record at this point - but we want to be
    // absolutely sure this is not an MBR before setting *lpbParitioned to FALSE.
    // Remember that the Intruder and Newboot viruses setup valid BPB records
    // in the MBR of a hard drive.

    // if we can be assured that this does not have a valid partition table
    // we'll allow a FALSE value in *lpbParitioned

    // look for overlapping partitions

    LPPARTENTRYREC lpPartEntryRecI;
    LPPARTENTRYREC lpPartEntryRecJ;
    for (int i = 0; i < 4; ++i)
    {
        for (int j = 0; j < 4; ++j)
        {
            if (i == j)
            {
                continue;
            }

            lpPartEntryRecI = (LPPARTENTRYREC)((LPSTR)lpMBRBuffer + PART_TABLE_OFFSET) + i;
            lpPartEntryRecJ = (LPPARTENTRYREC)((LPSTR)lpMBRBuffer + PART_TABLE_OFFSET) + j;

            // only check for overlapping partitions on used entries

            if (lpPartEntryRecI->system == SYSTEM_NOT_USED || lpPartEntryRecJ->system == SYSTEM_NOT_USED)
            {
                continue;
            }

            if ((lpPartEntryRecJ->relativeSectors >= lpPartEntryRecI->relativeSectors) &&
                (lpPartEntryRecJ->relativeSectors < lpPartEntryRecI->relativeSectors +
                                                     lpPartEntryRecI->numSectors))
            {
                // woops - we found an overlapping partition.  This is not
                // a valid partition table

                *lpbPartitioned = FALSE;
                MemFreePtr(lpMBRBuffer);

                return CBSTATUS_OK;
            }
        }
    }

    // no overlapping partitions
    // assume *lpbPartitoned is TRUE at this point until proven FALSE

    *lpbPartitioned = TRUE;

    WORD  wActivePartitions  = 0;
    WORD  wUsedPartitions    = 0;

    LPPARTENTRYREC lpPartEntryRec;
    for (int index = 0; index < 4; ++index)
    {
        lpPartEntryRec = (LPPARTENTRYREC)((LPSTR)lpMBRBuffer + PART_TABLE_OFFSET) + index;

        if (lpPartEntryRec->system == SYSTEM_NOT_USED)
        {
            continue;
        }

        ++wUsedPartitions;

        // it is not valid to have more that one active partition

        if (lpPartEntryRec->bootable && (++wActivePartitions > 1))
        {
            *lpbPartitioned = FALSE;
            break;
        }
    }

    // we must have at least one used parition to be valid

    if (!wUsedPartitions)
    {
        *lpbPartitioned = FALSE;
    }

    MemFreePtr(lpMBRBuffer);

    return CBSTATUS_OK;
}
#endif    // #else case

#endif    // #ifndef SYM_NLM


// Mutex

typedef struct tagMUTEX_INFO
{
    DWORD           dwReserved;

#if defined(SYM_WIN32)

    HANDLE          hMutex;

#elif defined(SYM_VXD) || defined(SYM_NTK)

    SYMMUTEX        stMutex;

#endif

} MUTEX_INFO_T, FAR *LPMUTEX_INFO;


CBSTATUS NAVMutexCreate
(
    LPLPVOID        lplpvMutexInfo
)
{
    LPMUTEX_INFO    lpstMutexInfo;

    lpstMutexInfo = (LPMUTEX_INFO)MemAllocPtr(GMEM_ZEROINIT | GMEM_FIXED,
                                              sizeof(MUTEX_INFO_T));
    if (lpstMutexInfo == NULL)
        return(CBSTATUS_OK);

#if defined(SYM_WIN32)

    lpstMutexInfo->hMutex = CreateMutex(NULL,   // Default security attributes
                                        FALSE,  // No initial owner
                                        NULL);  // Don't care about its name

    if (lpstMutexInfo->hMutex == NULL)
    {
        MemFreePtr(lpstMutexInfo);
        return(CBSTATUS_ERROR);
    }

#elif defined(SYM_VXD) || defined(SYM_NTK)

    if (!SymCreateMutex(&lpstMutexInfo->stMutex))
    {
        MemFreePtr(lpstMutexInfo);
        return(CBSTATUS_ERROR);
    }

#endif

    *lplpvMutexInfo = (LPVOID)lpstMutexInfo;

    return(CBSTATUS_OK);
}

CBSTATUS NAVMutexDestroy
(
    LPVOID          lpvMutexInfo
)
{
    CBSTATUS        cbStatus;
    cbStatus = CBSTATUS_OK;

#if defined(SYM_WIN32)

    if (CloseHandle(((LPMUTEX_INFO)lpvMutexInfo)->hMutex) == FALSE)
    {
        // Failed to close mutex

        cbStatus = CBSTATUS_ERROR;
    }

#elif defined(SYM_VXD) || defined(SYM_NTK)

    SymDestroyMutex (&(((LPMUTEX_INFO)lpvMutexInfo)->stMutex));

#endif

    MemFreePtr(lpvMutexInfo);
    return(CBSTATUS_OK);
}

CBSTATUS NAVMutexWait
(
    LPVOID          lpvMutexInfo,
    DWORD           dwTimeoutMS
)
{
#if defined(SYM_WIN32)

    if (WaitForSingleObject(((LPMUTEX_INFO)lpvMutexInfo)->hMutex,
                            dwTimeoutMS) != WAIT_OBJECT_0)
    {
        // Failure getting mutex

        return(CBSTATUS_ERROR);
    }

#elif defined(SYM_VXD) || defined(SYM_NTK)

    SymWaitForMutex (&(((LPMUTEX_INFO)lpvMutexInfo)->stMutex));

#endif

    return(CBSTATUS_OK);
}

CBSTATUS NAVMutexRelease
(
    LPVOID          lpvMutexInfo
)
{
#if defined(SYM_WIN32)

    if (ReleaseMutex(((LPMUTEX_INFO)lpvMutexInfo)->hMutex) == FALSE)
    {
        // Failure releasing mutex

        return(CBSTATUS_ERROR);
    }

#elif defined(SYM_VXD) || defined(SYM_NTK)

    SymReleaseMutex (&(((LPMUTEX_INFO)lpvMutexInfo)->stMutex));

#endif

    return(CBSTATUS_OK);
}

// Semaphore

CBSTATUS NAVSemaphoreCreate
(
    LPLPVOID        lplpvSemaphoreInfo,
    DWORD           dwInitialCount,
    DWORD           dwMaximumCount
)
{
    return(CBSTATUS_ERROR);
}

CBSTATUS NAVSemaphoreDestroy
(
    LPVOID          lpvSemaphoreInfo
)
{
    return(CBSTATUS_ERROR);
}

CBSTATUS NAVSemaphoreWait
(
    LPVOID          lpvSemaphoreInfo,
    DWORD           dwTimeoutMS
)
{
    return(CBSTATUS_ERROR);
}

CBSTATUS NAVSemaphoreRelease
(
    LPVOID          lpvSemaphoreInfo,
    LPDWORD         lpdwPrevCount
)
{
    return(CBSTATUS_ERROR);
}

FILECALLBACKS_T gstNAVFileCallBacks =
{
    NAVFileOpen,
    NAVFileClose,
    NAVFileSeek,
    NAVFileRead,
    NAVFileWrite,
    NAVFileTell,
    NAVFileSetEOF,
    NAVFileEOF,
    NAVFileGetAttr,
    NAVFileSetAttr,
    NAVFileGetDateTime,
    NAVFileSetDateTime,
    NAVFileFullNameAndPath,
    NAVFileName,
    NAVFilePath,
    NAVFileLogicalDrive,
    NAVFileLength,
    NAVFileDelete,
    NAVFileRename,
    NAVFileGetErrorCondition,
};

LINEARCALLBACKS gstNAVLinearCallBacks =
{
#ifndef SYM_NLM
    NAVLinearOpen,
    NAVLinearClose,
    NAVLinearRead,
    NAVLinearWrite,
    NAVLinearGetDimensions,
    NAVLinearConvertFromPhysical,
    NAVLinearConvertToPhysical,
    NAVLinearIsPartitionedMedia
#else
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
#endif
};

GENERALCALLBACKS_T gstNAVGeneralCallBacks =
{
    &gstNAVLinearCallBacks,
    &gstNAVFileCallBacks,
    NAVMemoryAlloc,
    NAVMemoryFree,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL
};

PROGRESSCALLBACKS_T gstNAVProgressCallBacks =
{
#ifndef SYM_NLM
    NAVProgressInit,
    NAVProgressUpdate,
    NAVProgressClose,
    NULL
#else
    NV_ProgressInit,
    NV_ProgressUpdate,
    NV_ProgressClose,
    NULL
#endif
};

DATAFILECALLBACKS_T gstNAVDataFileCallBacks =
{
    &gstNAVFileCallBacks,
    NAVGetDataFileInfo,
    NAVCloseDataFileInfo
};

IPCCALLBACKS_T gstNAVIPCCallBacks =
{
    NAVMutexCreate,
    NAVMutexDestroy,
    NAVMutexWait,
    NAVMutexRelease,
    NAVSemaphoreCreate,
    NAVSemaphoreDestroy,
    NAVSemaphoreWait,
    NAVSemaphoreRelease
};

MEMORYCALLBACKS_T gstNAVMemoryCallBacks =
{
    NULL,
    NULL,
    NAVMemGetPtrToMem,
    NAVMemReleasePtrToMem
};


//=========================================================================//
//                                                                         //
//      NAVEX Callbacks - taken from the old avglob.cpp in Certlibn        //
//                                                                         //
//=========================================================================//

#ifndef SYM_NLM

/*@API:*************************************************************************
@Declaration:

BOOL WINAPI ScanLockUnlockDiskP ( // Locks / unlock physical drive
    UINT    uOperation,                 // [in] 0 - lock, 1 - unlock
    BYTE    byDrive,                    // [in] drive to lock/unlock
    UINT    uRequestedAccess            // [in] requested access right
    );

@Description:
This function is used to prepare a physical drive for disk read/write.

Note: this function does very little in SYM_DOS platform.

@Parameters:
$uOperation$ This parameter should contain 0 or 1.  If 0, the function will
             lock the drive, if 1, the function will unlock the drive.
$byDrive$ Should be an int-13 drive ID
$uRequestedAccess$ The possible settings can be:
        DISK_READ_WRITE If the code should lock/unlock write access
        DISK_READ_ONLY  If the code should lock/unlock read only access

@Returns:
    TRUE    if the operation was successful
    FALSE   if somebody has a lock on the drive for the requested access, or
            error.

@Include: virscan.h

*******************************************************************************/
#if defined(SYM_DOS)

BOOL WINAPI ScanLockUnlockDiskP (
    UINT    uOperation,                 // [in] 0 - lock, 1 - unlock
    BYTE    byDrive,                    // [in] drive to lock/unlock
    UINT    uRequestedAccess            // [in] requested access right
    )
{
    return (TRUE);
}

#elif defined(SYM_WIN32) && !defined(SYM_VXD)

BOOL WINAPI ScanLockUnlockDiskP (
    UINT    uOperation,                 // [in] 0 - lock, 1 - unlock
    BYTE    byDrive,                    // [in] drive to lock/unlock
    UINT    uRequestedAccess            // [in] requested access right
    )
{
    if (SystemGetWindowsType() != SYM_SYSTEM_WIN_95)
        return(TRUE);

    switch ( uRequestedAccess )
        {
        case DISK_READ_ONLY:

            switch ( uOperation )
                {
                case 0:

                    if ( DiskLockPhysicalVolume ( byDrive, 1, 1 ) )
                        {
                        return ( FALSE );
                        }

                    if ( DiskLockPhysicalVolume ( byDrive, 2, 1 ) )
                        {
                        DiskUnlockPhysicalVolume ( byDrive );
                        return ( FALSE );
                        }

                    break;

                case 1:

                    DiskUnlockPhysicalVolume ( byDrive );
                    DiskUnlockPhysicalVolume ( byDrive );

                    break;

                default:
                    return(FALSE);
                }

            break;

        case DISK_READ_WRITE:

            switch ( uOperation )
                {
                case 0:

                    if ( !ScanLockUnlockDiskP ( 0, byDrive, DISK_READ_ONLY ) )
                        {
                        return ( FALSE );
                        }

                    if ( DiskLockPhysicalVolume ( byDrive, 3, 1 ) )
                        {
                        ScanLockUnlockDiskP ( 1, byDrive, DISK_READ_ONLY );
                        return ( FALSE );
                        }

                    break;

                case 1:

                    DiskUnlockPhysicalVolume ( byDrive );

                    ScanLockUnlockDiskP ( 1, byDrive, DISK_READ_ONLY );

                    break;

                default:
                    return(FALSE);
                }

            break;

        default:
            return ( FALSE );
        }

    return ( TRUE );
}
#endif

/*@API:*************************************************************************
@Declaration:

BOOL WINAPI ScanLockUnlockDiskL ( // Locks / unlock logical drive
    UINT    uOperation,                 // [in] 0 - lock, 1 - unlock
    BYTE    byDrive,                    // [in] drive to lock/unlock
    UINT    uRequestedAccess            // [in] requested access right
    );

@Description:
This function is used to prepare a logical drive for disk read/write.

Note: this function does very little in SYM_DOS platform.

@Parameters:
$uOperation$ This parameter should contain 0 or 1.  If 0, the function will
             lock the drive, if 1, the function will unlock the drive.
$byDrive$ Should be a drive ID - A, B, C, etc
$uRequestedAccess$ The possible settings can be:
        DISK_READ_WRITE If the code should lock/unlock write access
        DISK_READ_ONLY  If the code should lock/unlock read only access

@Returns:
    TRUE    if the operation was successful
    FALSE   if somebody has a lock on the drive for the requested access, or
            error.

@Include: virscan.h

*******************************************************************************/
#if defined(SYM_DOS)

BOOL WINAPI ScanLockUnlockDiskL (
    UINT    uOperation,                 // [in] 0 - lock, 1 - unlock
    BYTE    byDrive,                    // [in] drive to lock/unlock
    UINT    uRequestedAccess            // [in] requested access right
    )
{
    return (TRUE);
}

#elif defined(SYM_WIN32) && !defined(SYM_VXD)

BOOL WINAPI ScanLockUnlockDiskL (
    UINT    uOperation,                 // [in] 0 - lock, 1 - unlock
    BYTE    byDrive,                    // [in] drive to lock/unlock
    UINT    uRequestedAccess            // [in] requested access right
    )
{
    if (SystemGetWindowsType() != SYM_SYSTEM_WIN_95)
        return(TRUE);

    switch ( uRequestedAccess )
        {
        case DISK_READ_ONLY:

            switch ( uOperation )
                {
                case 0:

                    if ( DiskLockLogicalVolume ( byDrive, 1, 1 ) )
                        {
                        return ( FALSE );
                        }

                    if ( DiskLockLogicalVolume ( byDrive, 2, 1 ) )
                        {
                        DiskUnlockLogicalVolume ( byDrive );
                        return ( FALSE );
                        }

                    break;

                case 1:

                    DiskUnlockLogicalVolume ( byDrive );
                    DiskUnlockLogicalVolume ( byDrive );

                    break;

                default:
                    return(FALSE);
                }

            break;

        case DISK_READ_WRITE:

            switch ( uOperation )
                {
                case 0:

                    if ( !ScanLockUnlockDiskL ( 0, byDrive, DISK_READ_ONLY ) )
                        {
                        return ( FALSE );
                        }

                    if ( DiskLockLogicalVolume ( byDrive, 3, 1 ) )
                        {
                        ScanLockUnlockDiskL ( 1, byDrive, DISK_READ_ONLY );
                        return ( FALSE );
                        }

                    break;

                case 1:

                    DiskUnlockLogicalVolume ( byDrive );

                    ScanLockUnlockDiskL ( 1, byDrive, DISK_READ_ONLY );

                    break;

                default:
                    return(FALSE);
                }

            break;

        default:
            return ( FALSE );
        }

    return ( TRUE );
}
#endif

// This is being added as a local function to minimize the code changes.
// -- RCHINTA
#if defined(SYM_WIN32)

#define		NEC_125MB_CYLINDERS		77
#define		NEC_125MB_SECTORS_PER_TRACK 8
#define		NEC_125MB_TRACKS_PER_CYLINDER 2

BOOL IsNEC12MBFloppy (LPABSDISKREC lpAbsDisk)
{
    char              szDrive[SYM_MAX_PATH];
    HANDLE            hFile;
    DWORD             dwBytesReturned;
	BOOL			  bResult = FALSE;
	DISK_GEOMETRY	  dg;

    if (SystemGetWindowsType() == SYM_SYSTEM_WIN_NT)
	{
		wsprintf(szDrive, "\\\\.\\%c:", 'A'+ lpAbsDisk->dn);

        hFile = CreateFile(szDrive, GENERIC_READ | GENERIC_WRITE,
                                    FILE_SHARE_READ | FILE_SHARE_WRITE,
                                    NULL,
                                    OPEN_EXISTING,
                                    0,
                                    NULL);

		if (hFile == INVALID_HANDLE_VALUE)
			return bResult;

		if ( DeviceIoControl(hFile, IOCTL_DISK_GET_DRIVE_GEOMETRY,
				NULL, 0, &dg, sizeof(DISK_GEOMETRY), &dwBytesReturned, NULL) )
			{
			if(	(dg.Cylinders.LowPart == NEC_125MB_CYLINDERS) &&
				(dg.TracksPerCylinder == NEC_125MB_TRACKS_PER_CYLINDER)  &&
				(dg.SectorsPerTrack == NEC_125MB_SECTORS_PER_TRACK)  &&
				(dg.BytesPerSector == PHYSICAL_SECTOR_SIZE98) )
				{
				bResult = TRUE;
				lpAbsDisk->sectorLenCode = 3;
				// TODO: fill in the rest of the disk_geometry parameters
				// For now keep the changes to a minimum - RCHINTA
				}
			}

		CloseHandle(hFile);
		}
	return bResult;
}
#endif	//(SYM_WIN32)

//--------------------------------------------------------------------
//--------------------------------------------------------------------
UINT DOS_LOADDS SYM_EXPORT WINAPI NAVEXSDiskReadPhysical(
    LPVOID  lpBuffer,
    BYTE    bCount,
    WORD    wCylinder,
    BYTE    bSector,
    BYTE    bHead,
    BYTE    bDrive )
{
    BYTE    byResult;
    BYTE    byCounter = 0;

    ABSDISKREC  stDiskRec = { 0 };

    stDiskRec.dn = bDrive;

#if defined(SYM_DOS) || defined(SYM_WIN32) || defined(SYM_VXD)

    if ( !HWIsNEC() )
        {
        // DiskGetPhysicalInfo() does Bad Things to the NEC floppy
        // drive, so skip it if we're on a NEC.  This doesn't have any
        // readily apparent side-effects (but NAV is only scanning the
        // boot record, which is always in the same location).

        byResult = DiskGetPhysicalInfo(&stDiskRec);
                                        // These MUST be filled in for
                                        // W32 DiskAbsOperation() to calculate
                                        // the correct offset.
        if (    (0 == stDiskRec.dwSectorsPerTrack)
             || (0 == stDiskRec.dwTotalTracks)
             || (0 == stDiskRec.dwTotalHeads) )
            {
            return ( 1 );
            }
        }
	else
		{
#if defined(SYM_WIN32)
			if ( SystemGetWindowsType() == SYM_SYSTEM_WIN_NT )
				{
					// Fill in the sectorLenCode if it is a NEC 1.25MB Floppy
					// -- RCHINTA
					IsNEC12MBFloppy (&stDiskRec);
				}
#endif // SYM_WIN32
		}

#endif // #if defined(SYM_DOS) || defined(SYM_WIN32) || defined(SYM_VXD)

#if defined(SYM_WIN32) && !defined(SYM_VXD)
    if (!(wCylinder == 0 && bHead == 0))  // don't lock first partition table
        {
        if ( ScanLockUnlockDiskP ( LOCK_DRIVE,
                                        bDrive,
                                        DISK_READ_ONLY ) == FALSE)
            {
            return(1);                  // error!
            }
        }
#endif

#if defined(SYM_WIN32) || defined(SYM_VXD)
    // Win32 platform needs special handling for Promise EIDE 4030+ controller.
    // Buffer must not straddle two 4K memory pages.
    // To keep things simple, we'll only allow reads of up to 4K.

    byResult = SYM_ERROR_INVALID_FUNCTION;  // assume failure.

    if (bCount * PHYSICAL_SECTOR_SIZE <= 4096)
        {
        auto    LPBYTE  lpLocalBuffer;

        lpLocalBuffer = (LPBYTE)MemAllocPtr(GHND, 4096 * 2);

        if (NULL != lpLocalBuffer)
            {
                                        // Get 4K aligned buffer.
            stDiskRec.buffer = (LPBYTE)((DWORD)lpLocalBuffer + 4096
                                        - ((DWORD)lpLocalBuffer % 4096));

            stDiskRec.dwHead      = bHead;
            stDiskRec.dwTrack     = wCylinder;
            stDiskRec.dwSector    = bSector;
            stDiskRec.numSectors  = bCount;

            do
                {
                byResult = DiskAbsOperation(READ_COMMAND, &stDiskRec);
                byCounter++;
                }
            while ( (byResult != SYM_ERROR_NO_ERROR) && (byCounter < 3) );

                                    // On success, copy these sectors over.
            if (byResult == SYM_ERROR_NO_ERROR)
                {
				if( HWIsNEC() )
					{
#if !defined(SYM_VXD)
                    if( (SystemGetWindowsType() == SYM_SYSTEM_WIN_NT) )
						{
							if (stDiskRec.sectorLenCode == 3)
								MEMCPY(lpBuffer, stDiskRec.buffer, bCount * PHYSICAL_SECTOR_SIZE98);
							else
								MEMCPY(lpBuffer, stDiskRec.buffer, bCount * PHYSICAL_SECTOR_SIZE);
						}
					else
#endif
						// On Windows 95 copy 1024K buffer regardless of the type of floppy
						// since we cannot identify easily if it is a 1.2MB floppy or not.
						MEMCPY(lpBuffer, stDiskRec.buffer, bCount * PHYSICAL_SECTOR_SIZE);
					}
				else // non NEC's
					{
					MEMCPY(lpBuffer, stDiskRec.buffer, bCount * PHYSICAL_SECTOR_SIZE);
					}
                }

            MemFreePtr(lpLocalBuffer);
            }
         }
#else
    stDiskRec.dwHead        = bHead;
    stDiskRec.dwTrack       = wCylinder;
    stDiskRec.dwSector      = bSector;
    stDiskRec.numSectors    = bCount;
    stDiskRec.buffer        = (BYTE huge *) lpBuffer;

    do
        {
        byResult = DiskAbsOperation(READ_COMMAND, &stDiskRec);
        byCounter++;
        }
    while ( (byResult != SYM_ERROR_NO_ERROR) && (byCounter < 3) );

#endif // #if defined(SYM_WIN32) || defined(SYM_VXD)


#if defined(SYM_WIN32) && !defined(SYM_VXD)
    if (!(wCylinder == 0 && bHead == 0))
        {
        ScanLockUnlockDiskP ( UNLOCK_DRIVE,
                                   bDrive,
                                   DISK_READ_ONLY );
        }
#endif

    return( (byResult == SYM_ERROR_NO_ERROR) ? 0 : 1 );
}

UINT DOS_LOADDS WINAPI NAVEXSDiskWritePhysical (
    LPVOID  lpBuffer,
    BYTE    bCount,
    WORD    wCylinder,
    BYTE    bSector,
    BYTE    bHead,
    BYTE    bDrive )
{

#if defined(SYM_WIN32) && !defined(SYM_VXD) && !defined(SYM_NTK)

                                        // The following block fixes the
                                        // overwrites that were happening
                                        // on NT FAT partitions.
    if ( SystemGetWindowsType() == SYM_SYSTEM_WIN_NT &&
         bDrive >= FIRST_HD )
        {
        BYTE    byDriveLetter;
        BYTE    byPartNum;
        DWORD   dwLinearOffset;
        DWORD   dwBytes;

                                        // Do these sectors lie within a
                                        // valid partition?
        byPartNum = NAVEX_NTDeterminePartNum( bDrive,
                                             bHead,
                                             wCylinder,
                                             bSector,
                                             bCount,
                                             &dwLinearOffset );

        // Is this partition currently mapped to a drive letter?
        // Note that NAVEX_NTDetermineDriveLetter will return 0 for
        // floppy drives. In which case, floppy writes will occur through
        // the DiskAbsOperation below.
         if ( MAX_PARTITIONS != byPartNum &&
              0 != ( byDriveLetter = NAVEX_NTDetermineDriveLetter( bDrive,
                                                                  byPartNum )))
            {
                dwBytes = bCount * SECTOR_SIZE;

                                        // Write to the logical drive
                return ( NAVEX_NTDiskWriteLogical ( byDriveLetter,
                                                   dwLinearOffset,
                                                   lpBuffer,
                                                   &dwBytes ));
            }
        }
                                        // Otherwise, continue as before...
#endif  // SYM_WIN32 && !SYM_VXD && !SYM_NTK


    BYTE    byResult;
    BYTE    byCounter = 0;

    ABSDISKREC  stDiskRec = { 0 };

    stDiskRec.dn = bDrive;

#if defined(SYM_DOS) || defined(SYM_WIN32)

    if ( !HWIsNEC() )
        {
        // DiskGetPhysicalInfo() does Bad Things to the NEC floppy
        // drive, so skip it if we're on a NEC.  This doesn't have any
        // readily apparent side-effects (but NAV is only scanning the
        // boot record, which is always in the same location).

        byResult = DiskGetPhysicalInfo(&stDiskRec);

                                        // These MUST be filled in for
                                        // W32 DiskAbsOperation() to calculate
                                        // the correct offset.
        if (   (0 == stDiskRec.dwSectorsPerTrack)
            || (0 == stDiskRec.dwTotalTracks)
            || (0 == stDiskRec.dwTotalHeads) )
            {
            return ( 1 );
            }
        }
#endif

#if defined(SYM_WIN32) && !defined(SYM_VXD)
    if (!(wCylinder == 0 && bHead == 0 && bSector == 1))  // don't lock MBR
        {
        if ( ScanLockUnlockDiskP ( LOCK_DRIVE,
                                        bDrive,
                                        DISK_READ_WRITE ) == FALSE)
            {
            return(1);                  // error!
            }
        }
#endif

    stDiskRec.dwHead       = bHead;
    stDiskRec.dwTrack      = wCylinder;
    stDiskRec.dwSector     = bSector;
    stDiskRec.numSectors   = bCount;
    stDiskRec.buffer       = (BYTE huge *) lpBuffer;

    do
        {
        byResult = DiskAbsOperation(WRITE_COMMAND, &stDiskRec);
        byCounter++;
        }
    while ( (byResult != SYM_ERROR_NO_ERROR) && (byCounter < 3) );

#if defined(SYM_WIN32) && !defined(SYM_VXD)
    if (!(wCylinder == 0 && bHead == 0 && bSector == 1))  // don't lock MBR
        {
        ScanLockUnlockDiskP ( UNLOCK_DRIVE,
                                   bDrive,
                                   DISK_READ_WRITE );
        }
#endif

    return( (byResult == SYM_ERROR_NO_ERROR) ? 0 : 1 );
}



#if defined(SYM_WIN32) && !defined(SYM_VXD) && !defined(SYM_NTK)
                                        // These functions are helpers for
                                        // LocalDiskWritePhysical under NT

//--------------------------------------------------------------------
// NAVEX_NTDeterminePartNum()
//
// Given a series of physical sectors (drive number, starting coordinates,
// and a sector count) this function returns the partition number (0-3) in
// which the specified sectors reside.
//
// If these sectors lies outside of all partitions, stradle a partition
// boundary, or if an error is encountered while determining the partition
// number, MAX_PARTITIONS (4) is returned.
//--------------------------------------------------------------------
BYTE NAVEX_NTDeterminePartNum
(
    BYTE    byDrive,
    BYTE    byHead,
    WORD    wCylinder,
    BYTE    bySector,
    BYTE    bySectors,
    LPDWORD lpdwLinearOffset
)
{
    BYTE    byResult;
    BYTE    byCounter = 0;
    DWORD   dwLinearOffset;
    WORD    wMBROffset;
    BYTE    byPartNum;
    BYTE    abyMBR [ 8 * SECTOR_SIZE ];

    ABSDISKREC  stDiskRec = { 0 };

    stDiskRec.dn = byDrive;

    byResult = DiskGetPhysicalInfo(&stDiskRec);

    if (    (0 == stDiskRec.dwSectorsPerTrack)
         || (0 == stDiskRec.dwTotalTracks)
         || (0 == stDiskRec.dwTotalHeads) )
        {
        return ( MAX_PARTITIONS );
        }

    dwLinearOffset = (((DWORD) wCylinder *
                       stDiskRec.dwTotalHeads *
                       stDiskRec.dwSectorsPerTrack ) +
                      ((DWORD) byHead * stDiskRec.dwSectorsPerTrack ) +
                      (bySector - 1));

    // Read MBR for this physical disk

    stDiskRec.dwHead      = 0;
    stDiskRec.dwTrack     = 0;
    stDiskRec.dwSector    = 1;
    stDiskRec.numSectors  = 1;
    stDiskRec.buffer      = (BYTE huge *) abyMBR;

    do
        {
        byResult = DiskAbsOperation( READ_COMMAND, &stDiskRec );
        byCounter++;
        }
    while ( (byResult != SYM_ERROR_NO_ERROR) && (byCounter < 3) );

    if ( byResult != SYM_ERROR_NO_ERROR )
        return ( MAX_PARTITIONS );

    for ( byPartNum = 0, wMBROffset = 0x1BE;
          byPartNum < MAX_PARTITIONS;
          ++byPartNum, wMBROffset += 0x10 )
        {
        if ( 0 != *((LPBYTE) abyMBR + wMBROffset + 4 ) &&
             dwLinearOffset >= *((LPDWORD) ( abyMBR + wMBROffset + 8 )) &&
             ( dwLinearOffset + bySectors ) <= *((LPDWORD) ( abyMBR + wMBROffset + 8 ))
                               + *((LPDWORD) ( abyMBR + wMBROffset + 12 )))
            {
            // byPartNum now indicates the first partition entry with a
            // valid non-zero filesystem byte which contains (inclusively)
            // the sector to be written.

            break;
            }
        }

    if ( NULL != lpdwLinearOffset )
        {
        dwLinearOffset -= *((LPDWORD) ( abyMBR + wMBROffset + 8 ));
        dwLinearOffset *= SECTOR_SIZE;
        *lpdwLinearOffset = dwLinearOffset;
        }

    return ( byPartNum );
}


//--------------------------------------------------------------------
// NAVEX_NTDetermineDriveLetterWin2000Plus()
//
// Given a drive and partition number, this function returns a drive
// letter ('C', 'D', etc.) that is currently mapped to this partition.
// It only works for hard drives. This function works only on 
// Windows 2000 and post-Windows 2000 systems. 
//
// If there is no drive letter mapped to this partition, or an error
// is encountered while determining the drive letter, 0 is returned.
//--------------------------------------------------------------------
BYTE NAVEX_NTDetermineDriveLetterWin2000Plus (BYTE    byDrive,
                                              BYTE    byPartNum)
{
    // Translate our input parameters a little:
    // 1. Translate the physical drive number to an NT based device number.
    //    Generatally, byDrive could by 0x80 and the NT device number is 0.
    // 2. Translate the specified partition number to a one based 
    //    partition number as specified in the STORAGE_DEVICE_NUMBER struct.
    const BYTE    byNeedNtDeviceNum    = byDrive - FIRST_HD;
    const BYTE    byNeedStoragePartNum = byPartNum + 1;

    const TCHAR   c_szDeviceHarddrive[] = "\\Device\\Harddisk";
    const DWORD   c_dwDrivesChars       = 256;
    auto  TCHAR   szzDrives[c_dwDrivesChars+1];
    auto  LPTSTR  lpszCurDrive          = szzDrives;
    auto  BYTE    byFoundDriveLtr       = 0 /*failure*/;

    // This function does not work when byDrive is less than FIRST_HD.
    // See byNeedNtDeviceNum variable initialization.
    if (byDrive < FIRST_HD)
        goto FuncEnd;

    // Get all of the possible logical drive letters.
    if (0 == GetLogicalDriveStrings (c_dwDrivesChars, szzDrives))
        goto FuncEnd;

    // Walk through all of the logical drives. For each one, determine
    // if the physical drive number and partition number match the
    // caller's.
    while (byFoundDriveLtr == 0 && lpszCurDrive[0] != _T('\0'))
    {
        const DWORD    c_dwDIOCBufferChars   = SYM_MAX_PATH*2;
        auto  TCHAR    szDrive[16];
        auto  TCHAR    szDIOCBuffer[c_dwDIOCBufferChars];
        auto  BOOL     bSuccess;

        // Copy only the drive specification. Eg. "C:".
        szDrive[0] = lpszCurDrive[0];
        szDrive[1] = lpszCurDrive[1];
        szDrive[2] = '\0';

        // First query for a DOS name for this device.
        bSuccess = (0 != QueryDosDevice (szDrive, szDIOCBuffer, c_dwDIOCBufferChars));

        // Only process devices that are hard drives.
        if (0 == STRNICMP (c_szDeviceHarddrive, szDIOCBuffer, sizeof (c_szDeviceHarddrive)-1))
        {
            auto  TCHAR                 szDevice[16];
            auto  DWORD                 dwBytesReturned;
            auto  STORAGE_DEVICE_NUMBER stStorageDeviceInfo;
            auto  HANDLE                hDevice;

            SPRINTF (szDevice, "\\\\.\\%c:", lpszCurDrive[0]);

            // Open the device for query.
            hDevice = CreateFile (szDevice, 0 /*Device Query Only*/,
                                 FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,
                                 OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

            // Fail out of this function when the CreateFile fails.
            if (hDevice == INVALID_HANDLE_VALUE)
                goto FuncEnd;

            // Attempt to read the storage information.
            bSuccess = DeviceIoControl (hDevice, IOCTL_STORAGE_GET_DEVICE_NUMBER,
                                        NULL, 0, &stStorageDeviceInfo,
                                        sizeof (stStorageDeviceInfo), &dwBytesReturned,
                                        NULL);

            // Only attempt to match when we where able to get the storage 
            // information.
            if (bSuccess)
            {
                // This this logical drive has a matching physical device number
                // and partition number, we have found the logical drive the 
                // caller is looking for.
                if (stStorageDeviceInfo.DeviceNumber    == byNeedNtDeviceNum &&
                    stStorageDeviceInfo.PartitionNumber == byNeedStoragePartNum)
                {
                    byFoundDriveLtr = lpszCurDrive[0];
                }
            }

            // Remember to close our drive handle.
            CloseHandle (hDevice);
            hDevice = NULL;
        }

        // Skip to the next drive listing in this SZZ string.
        lpszCurDrive += STRLEN (lpszCurDrive) + 1;
    }

FuncEnd:
    return byFoundDriveLtr;
}

//--------------------------------------------------------------------
// NAVEX_NTDetermineDriveLetterPreWin2000()
//
// Given a drive and partition number, this function returns a drive
// letter ('C', 'D', etc.) that is currently mapped to this partition.
// It only works for hard drives. This function works only on 
// pre-Windows 2000 systems.
//
// If there is no drive letter mapped to this partition, or an error
// is encountered while determining the drive letter, 0 is returned.
//--------------------------------------------------------------------
BYTE NAVEX_NTDetermineDriveLetterPreWin2000
(
    BYTE    byDrive,
    BYTE    byPartNum
)
{
    const DWORD   c_dwDrivesChars       = 256;
    auto  TCHAR   szzDrives[c_dwDrivesChars+1];
    auto  TCHAR   szThisDevice[SYM_MAX_PATH];
    auto  BYTE    byResult = 0;
    auto  BYTE    abyDIOCBuffer[ SYM_MAX_PATH * 2 ];
    auto  BOOL    bDriveLetterFound = FALSE;
    auto  LPTSTR  lptstrDevice;
    auto  TCHAR   szDrive[8];

    // This function does not work when byDrive is less than FIRST_HD.
    // See SPRINTF below.
    if (byDrive < FIRST_HD)
        return 0;

    // create the string we're trying to match
    SPRINTF (szThisDevice, "\\Device\\Harddisk%c\\Partition%c",
             '0' + (byDrive - FIRST_HD), '0' + byPartNum + 1);

    // Get all of the possible logical drive letters.
    if (0 == GetLogicalDriveStrings (c_dwDrivesChars, szzDrives))
        return 0;

    lptstrDevice = szzDrives;
    
    szDrive[1] = ':';
    szDrive[2] = '\0';

    while (lptstrDevice[0] != 0)
    {
        if (lptstrDevice[1] == ':')
        {
            // Copy the drive letter to szDrive.
            szDrive[0] = lptstrDevice[0];

            if (0 != QueryDosDevice (szDrive, (TCHAR*) abyDIOCBuffer, SYM_MAX_PATH*2) &&
                !STRICMP ((TCHAR*) abyDIOCBuffer, szThisDevice))
            {
                bDriveLetterFound = TRUE;
                break;
            }
        }
        lptstrDevice += STRLEN( lptstrDevice ) + 1;
    }

    if (TRUE == bDriveLetterFound)
    {
        byResult = (BYTE)lptstrDevice[0];
    }

    return byResult;
}



//--------------------------------------------------------------------
// NAVEX_NTDetermineDriveLetter()
//
// Given a drive and partition number, this function returns a drive
// letter ('C', 'D', etc.) that is currently mapped to this partition.
// It only works for hard drives.
//
// If there is no drive letter mapped to this partition, or an error
// is encountered while determining the drive letter, 0 is returned.
//--------------------------------------------------------------------
BYTE NAVEX_NTDetermineDriveLetter
(
    BYTE    byDrive,
    BYTE    byPartNum
)
{
    OSVERSIONINFO   stVersion;
    BOOL            bSuccess;
    BYTE   byResult = 0;

    memset (&stVersion, 0, sizeof (stVersion));
    stVersion.dwOSVersionInfoSize = sizeof(stVersion);

    bSuccess = GetVersionEx (&stVersion);
    if (!bSuccess)
        goto FuncEnd;

    // This function only works for NT based OSes.
    SYM_ASSERT (stVersion.dwPlatformId == VER_PLATFORM_WIN32_NT);

    // Windows 2000 does its drive letter mapping differently than
    // Windows NT 4. Call the Windows 2000 version when we are on a
    // Windows 2000 or newer OS. In all other cases, we revert to the
    // pre-Windows 2000 method of mapping. This works for NT4.
    if (stVersion.dwMajorVersion >= 5)
        byResult = NAVEX_NTDetermineDriveLetterWin2000Plus (byDrive, byPartNum);
    else
        byResult = NAVEX_NTDetermineDriveLetterPreWin2000 (byDrive, byPartNum);

FuncEnd:
    return byResult;
}


//--------------------------------------------------------------------
// NAVEX_NTDiskWriteLogical()
//
// Given a drive letter, a linear byte offset, some data, and a length,
// this function writes length bytes of the data to the logical drive
// starting at the linear byte offset.  The return values are identical
// to LocalDiskWritePhysical():
//
// 0 == success
// 1 == failure
//--------------------------------------------------------------------
BYTE NAVEX_NTDiskWriteLogical
(
    BYTE    byDriveLetter,
    DWORD   dwLinearOffset,
    LPVOID  lpvBuffer,
    LPDWORD lpdwSectors
)
{
    TCHAR  tszThisLogicalDrive[] = _T( "\\\\.\\X:" );
    HANDLE hLogicalDrive;
    BOOL   bLocked;
    BOOL   bDismounted;
    BOOL   bWritten;
    DWORD  dwDummy;

    tszThisLogicalDrive[4] =(TCHAR) byDriveLetter;

    hLogicalDrive = CreateFile( tszThisLogicalDrive,
                                GENERIC_READ | GENERIC_WRITE,
                                FILE_SHARE_READ | FILE_SHARE_WRITE,
                                NULL,
                                OPEN_EXISTING,
                                FILE_ATTRIBUTE_NORMAL,
                                NULL );

    if( INVALID_HANDLE_VALUE == hLogicalDrive )
        {
        return( 1 );
        }

    dwDummy = SetFilePointer( hLogicalDrive, dwLinearOffset, NULL, FILE_BEGIN );

    if( dwDummy != dwLinearOffset )
        {
        CloseHandle( hLogicalDrive );
        return( 1 );
        }

    bLocked = DeviceIoControl ( hLogicalDrive,
                                FSCTL_LOCK_VOLUME,
                                NULL,
                                0,
                                NULL,
                                0,
                                &dwDummy,
                                NULL );

    bWritten = WriteFile( hLogicalDrive, lpvBuffer, *lpdwSectors, lpdwSectors, NULL );

    if( bLocked )
        {
        bDismounted = DeviceIoControl ( hLogicalDrive,
                                        FSCTL_DISMOUNT_VOLUME,
                                        NULL,
                                        0,
                                        NULL,
                                        0,
                                        &dwDummy,
                                        NULL );

        bLocked = !DeviceIoControl ( hLogicalDrive,
                                     FSCTL_UNLOCK_VOLUME,
                                     NULL,
                                     0,
                                     NULL,
                                     0,
                                     &dwDummy,
                                     NULL );
        }

    CloseHandle( hLogicalDrive );

    if ( !bWritten )
        {
        return( 1 );
        }

    return( 0 );
}

#endif // SYM_WIN32 && !SYM_VXD && !SYM_NTK

#endif // ifndef SYM_NLM


HFILE DOS_LOADDS WINAPI NAVEXFileOpen
(
    LPTSTR  lpName,
    UINT    uOpenMode
)
{
    SYM_ASSERT ( lpName );

#ifndef SYM_NLM


    HFILE hFile = FileOpen(lpName, uOpenMode);

  #ifdef SYM_DOSX
    // If we just failed to open the file.  See if we're looking for a def file
    // on a rescue disk - the def set may span multiple disks.

    if (hFile == HFILE_ERROR)
    {
        char szResqLocFile[SYM_MAX_PATH];
        NameReturnPath(lpName, szResqLocFile);
        NameAppendFile(szResqLocFile, RESQLOC_INI_FILE);

        if (!DiskIsFixed(DiskGet()) &&  // this only applies to floppies with
            FileExists(szResqLocFile)); // ResqLoc.dat present
        {
            // look for the disk label listed in ResqLoc.dat

            char szDefFileName[SYM_MAX_PATH];
            NameReturnFile(lpName, szDefFileName);

            char szDiskLabel[SYM_MAX_PATH];
            GetPrivateProfileString(RESQDEFSLOC_SECTION,
                                    szDefFileName,
                                    KEY_MISSING,
                                    szDiskLabel,
                                    sizeof(szDiskLabel),
                                    szResqLocFile);

            // If we have a disk label to prompt the user with, do so

            if (STRCMP(szDiskLabel, KEY_MISSING))
            {
                extern char SZ_INSERT_RESCUE_DISK[];
                extern char SZ_PRESS_ENTER[];

                for (;;)
                {
                    printf("\n\n%s \"%s\"\n\n%s", SZ_INSERT_RESCUE_DISK, szDiskLabel, SZ_PRESS_ENTER);

                    int ch = getch();
                    if (ch == 27)       // ESCape
                    {
                        break;
                    }
                    else if (ch == 13)  // ENTER
                    {
                        hFile = FileOpen(lpName, uOpenMode);

                        if (hFile != HFILE_ERROR)
                        {
                            // We found and successfully opened to data file!!

                            break;
                        }
                    }
                }
            }
        }
    }
  #endif                                // SYM_DOSX

    return (hFile);

#else
    return (NV_FileOpen(lpName, uOpenMode));
#endif

}

HFILE DOS_LOADDS WINAPI NAVEXFileClose
(
    HFILE   hHandle
)
{
    SYM_ASSERT ( hHandle );

    return (FileClose(hHandle));
}

UINT DOS_LOADDS WINAPI NAVEXFileRead
(
    HFILE   hHandle,
    LPVOID  lpBuffer,
    UINT    uBytes
)
{
    SYM_ASSERT ( lpBuffer );
    SYM_ASSERT ( uBytes );
    SYM_ASSERT ( hHandle );

    return (FileRead(hHandle, lpBuffer, uBytes));
}

UINT DOS_LOADDS WINAPI NAVEXFileWrite
(
    HFILE   hHandle,
    LPVOID  lpBuffer,
    UINT    uBytes
)
{
    SYM_ASSERT ( lpBuffer || (0 == uBytes) );
    SYM_ASSERT ( hHandle );

    return (FileWrite(hHandle, lpBuffer, uBytes));
}

DWORD DOS_LOADDS WINAPI NAVEXFileSeek
(
    HFILE   hHandle,
    LONG    lOffset,
    int     nFlag
)
{
    SYM_ASSERT ( hHandle );

    return (FileSeek(hHandle, lOffset, nFlag));
}

DWORD DOS_LOADDS WINAPI NAVEXFileLength
(
    HFILE   hHandle
)
{
    return (FileLength(hHandle));
}

BOOL DOS_LOADDS WINAPI NAVEXFileGetDateTime
(
    HFILE   hHandle,
    UINT    uType,
    LPWORD  lpwDate,
    LPWORD  lpwTime
)
{
    WORD        wDosDate = 0;
    WORD        wDosTime = 0;
    FILETIME    FileTime = { 0 };

    (void) uType;

    SYM_ASSERT ( hHandle );
    SYM_ASSERT ( lpwDate );
    SYM_ASSERT ( lpwTime );

    FileGetTime (hHandle, TIME_LASTWRITE, &FileTime);
    FileTimeToDosTime(&FileTime, &wDosDate, &wDosTime);

    *lpwDate = wDosDate;
    *lpwTime = wDosTime;

    return (TRUE);
}

BOOL DOS_LOADDS WINAPI NAVEXFileSetDateTime
(
    HFILE   hHandle,
    UINT    uType,
    WORD    wDate,
    WORD    wTime
)
{
    FILETIME    FileTime = { 0 };

    (void) uType;

    SYM_ASSERT ( hHandle );

    DosTimeToFileTime (wDate, wTime, &FileTime);
    FileSetTime (hHandle, TIME_LASTWRITE, &FileTime);
    return (TRUE);
}


//========================================================================
//
// Function:
//  NAVEXFileDelete()
//
// Parameters:
//  lpFilename          Full Path of the file to Delete.
//
// Description:
//  This function:
//                 1. Overwrites the entire file with zeros.
//                 2. Truncates the file to 0 byes in size.
//                 3. Deletes the file.
//
// Returns:
//  TRUE if successful, FALSE otherwise.
//
//========================================================================
BOOL DOS_LOADDS WINAPI NAVEXFileDelete
(
    LPTSTR  lpFilename
)
{
    SYM_ASSERT (lpFilename);

#ifndef SYM_NLM
    return (FileWipe(lpFilename, 1) == NOERR);
#else
    // for the time enabling FileWipe() in CORE opens too many problems
    return (FileDelete(lpFilename) == NOERR);
#endif

}

UINT DOS_LOADDS WINAPI NAVEXFileGetAttr
(
    LPTSTR  lpFilename,
    LPUINT  lpuAttr
)
{
    SYM_ASSERT ( lpFilename );

    return (FileGetAttr(lpFilename, lpuAttr));
}

UINT DOS_LOADDS WINAPI NAVEXFileSetAttr
(
    LPTSTR  lpFilename,
    UINT    uAttr )
{
    SYM_ASSERT ( lpFilename );

    return (FileSetAttr(lpFilename, uAttr));
}

HFILE DOS_LOADDS WINAPI NAVEXFileCreate
(
    LPTSTR  lpFilename,
    UINT    uAttr
)
{
    SYM_ASSERT ( lpFilename );

    return (FileCreate(lpFilename, uAttr));
}

LPVOID DOS_LOADDS WINAPI NAVEXTempMemoryAlloc
(
    DWORD    dwSize
)
{
    return (MemAllocPtr(GHND, dwSize));
}

VOID DOS_LOADDS WINAPI NAVEXTempMemoryFree
(
    LPVOID  lpBuffer
)
{
    MemFreePtr (lpBuffer);
}

LPVOID DOS_LOADDS WINAPI NAVEXPermMemoryAlloc
(
    DWORD   dwSize
)
{
    return (MemAllocPtr(GHND, dwSize));
}

VOID DOS_LOADDS WINAPI NAVEXPermMemoryFree
(
    LPVOID  lpBuffer
)
{
    MemFreePtr (lpBuffer);
}

UINT DOS_LOADDS WINAPI NAVEXCreateMutex
(
    LPLPVOID        lplpvMutexInfo
)
{
    return (NAVMutexCreate(lplpvMutexInfo) == CBSTATUS_OK);
}

UINT DOS_LOADDS WINAPI NAVEXDestroyMutex
(
    LPVOID          lpvMutexInfo
)
{
    return (NAVMutexDestroy(lpvMutexInfo) == CBSTATUS_OK);
}

UINT DOS_LOADDS WINAPI NAVEXWaitMutex
(
    LPVOID          lpvMutexInfo,
    DWORD           dwTimeoutMS
)
{
    return (NAVMutexWait(lpvMutexInfo, dwTimeoutMS) == CBSTATUS_OK);
}

UINT DOS_LOADDS WINAPI NAVEXReleaseMutex
(
    LPVOID          lpvMutexInfo
)
{
    return (NAVMutexRelease(lpvMutexInfo) == CBSTATUS_OK);
}

HFILE DOS_LOADDS WINAPI NAVEXGetTempFile
(
    LPTSTR          lpszFileName,
    LPVOID          lpvCookie
)
{

#if defined(SYM_DOS) || defined(SYM_WIN)

    return (FileCreateTemporary(lpszFileName, FA_NORMAL));

#elif defined(SYM_VXD) || defined(SYM_NTK)

    auto   HFILE     hFile;
    auto   TCHAR     szGeneratedFileName[13];
    auto   int       i,j, iNum;
#if defined(SYM_VXD)
    auto   BOOL bConvertOemToAnsi = FALSE;
#endif                    

    for (i = 0; i < 9999; i++)
    {
        for ( j = 3, iNum = i;
              j >= 0;
              szGeneratedFileName[j--] = _T('0') + (iNum % 10), iNum /= 10 );

        STRCPY(&szGeneratedFileName[4], _T("NAV~.TMP"));
        STRCPY(lpszFileName, s_szNAVPath);
        NameAppendFile(lpszFileName, szGeneratedFileName);

#if defined(SYM_VXD)
        if(bConvertOemToAnsi)
        {
            OemToCharA(lpszFileName, lpszFileName);
        }
#endif

        hFile = FileCreate(lpszFileName,FA_NORMAL|0x80000000L);
        if (hFile != HFILE_ERROR)
        {
            return (hFile);
        }
#if defined(SYM_VXD)
                                                // Error code 3 = path not found
                                                // if a collision occurs, then
                                                // error code will be 50=file exists
                                                // NAVAP.VXD sets the s_szNAVPath to 
                                                // an OEM string
            DWORD dwLastError = GetLastError();
            if(dwLastError == 0x03 && VxDGetCurrentCHARSET() == CHARSET_ANSI )
            {
                bConvertOemToAnsi = TRUE;
        }
#endif //#if defined(SYM_VXD)
    }

#endif

   return (HFILE_ERROR);
}

BOOL DOS_LOADDS WINAPI NAVEXProgress
(
    WORD            wProgress,
    DWORD           dwCookie )
{
#ifdef SYM_NLM

    RelinquishControl();

#endif
    return(0);
}

UINT DOS_LOADDS WINAPI NAVEXMessageBox
(
    DWORD   dwBoxID,
    LPTSTR  lpszMessage,
    DWORD   dwButtonFlags,
    LPDWORD lpdwResult,
    DWORD   dwMSTimeOut,
    LPVOID  lpvCookie
)
{
                                        // if lpCookie is NULL we don't
                                        // display any UI - just return
                                        // NAVEX_UI_TIMEOUT so the caller
                                        // will perform the default action.

    if (!lpvCookie)
    {
        return (NAVEX_UI_TIMEOUT);
    }

#if defined(SYM_WIN32)

    return (NAVEXMessageBox32(dwBoxID,
                              lpszMessage,
                              dwButtonFlags,
                              lpdwResult,
                              dwMSTimeOut,
                              lpvCookie));


#elif defined(SYM_DOSX)

    DWORD dwButtonArray[6];
    WORD  wButtonCount = 0, ch;

    extern char SZ_OK [];
    extern char SZ_CANCEL [];
    extern char SZ_CONTINUE [];
    extern char SZ_YES [];
    extern char SZ_NO [];
    extern char SZ_PREVIOUS [];
    extern char SZ_ENTER_A_NUMBER [];

                                        // display the provided message

    printf("\n\n%s\n\n", lpszMessage);

                                        // dynamically build a user menu and
                                        // a corresponding array of possible
                                        // results

    if (dwButtonFlags & BUTTON_FLAG_OK)
    {
        dwButtonArray[wButtonCount] = BUTTON_FLAG_OK;
        printf("%3d. %s\n", ++wButtonCount, SZ_OK);
    }

    if (dwButtonFlags & BUTTON_FLAG_CANCEL)
    {
        dwButtonArray[wButtonCount] = BUTTON_FLAG_CANCEL;
        printf("%3d. %s\n", ++wButtonCount, SZ_CANCEL);
    }

    if (dwButtonFlags & BUTTON_FLAG_CONTINUE)
    {
        dwButtonArray[wButtonCount] = BUTTON_FLAG_CONTINUE;
        printf("%3d. %s\n", ++wButtonCount, SZ_CONTINUE);
    }

    if (dwButtonFlags & BUTTON_FLAG_YES)
    {
        dwButtonArray[wButtonCount] = BUTTON_FLAG_YES;
        printf("%3d. %s\n", ++wButtonCount, SZ_YES);
    }

    if (dwButtonFlags & BUTTON_FLAG_NO)
    {
        dwButtonArray[wButtonCount] = BUTTON_FLAG_NO;
        printf("%3d. %s\n", ++wButtonCount, SZ_NO);
    }

    if (dwButtonFlags & BUTTON_FLAG_PREVIOUS)
    {
        dwButtonArray[wButtonCount] = BUTTON_FLAG_PREVIOUS;
        printf("%3d. %s\n", ++wButtonCount, SZ_PREVIOUS);
    }

    printf("\n%s: ", SZ_ENTER_A_NUMBER);

                                        // wait for user input

    while ((ch = getch()) < '1' || ch > (wButtonCount + '0'));

    printf("\n");
                                        // assign user's the selection to
                                        // *lpdwResult

    *lpdwResult = dwButtonArray[ch - '0' - 1];

#elif defined(SYM_NTK)

    return (NAVEX_UI_TIMEOUT);

#elif defined(SYM_VXD)

    RECT rMessageBoxRect =
    {
        5, 8, 75, 18
    };

    auto   TCHAR szButtonString[128];
    auto   UINT  uResult;
    extern TCHAR *apszNAVEXButtons[];
    extern TCHAR apszNAVEXMessageBoxTitle[];
    szButtonString[0] = EOS;
    PMESSAGE_BOX_COOKIE pstMessageBoxCookie = (PMESSAGE_BOX_COOKIE)lpvCookie;

                                        // dynamically build a ButtonString
                                        // for AsyncMsgBox

    if (!pstMessageBoxCookie->bUserInterventionAllowed)
    {
        return (NAVEX_UI_TIMEOUT);
    }

    if (dwButtonFlags & BUTTON_FLAG_OK)
    {
        STRCAT(szButtonString, _T("1"));
        STRCAT(szButtonString, apszNAVEXButtons[1]);
    }

    if (dwButtonFlags & BUTTON_FLAG_CANCEL)
    {
        STRCAT(szButtonString, _T("2"));
        STRCAT(szButtonString, apszNAVEXButtons[2]);
    }

    if (dwButtonFlags & BUTTON_FLAG_CONTINUE)
    {
        STRCAT(szButtonString, _T("3"));
        STRCAT(szButtonString, apszNAVEXButtons[3]);
    }

    if (dwButtonFlags & BUTTON_FLAG_YES)
    {
        STRCAT(szButtonString, _T("4"));
        STRCAT(szButtonString, apszNAVEXButtons[4]);
    }

    if (dwButtonFlags & BUTTON_FLAG_NO)
    {
        STRCAT(szButtonString, _T("5"));
        STRCAT(szButtonString, apszNAVEXButtons[5]);
    }

    if (dwButtonFlags & BUTTON_FLAG_PREVIOUS)
    {
        STRCAT(szButtonString, _T("6"));
        STRCAT(szButtonString, apszNAVEXButtons[6]);
    }

    uResult = AsyncMsgBox(&rMessageBoxRect,
                          pstMessageBoxCookie->uTimeOut,
                          0,
                          AMB_COLOR_BLUE,
                          apszNAVEXMessageBoxTitle,
                          lpszMessage,
                          NULL,
                          szButtonString);

                                        // assign the user's selection to
                                        // *lpdwResult

    switch (uResult)
    {
        case 1:
            *lpdwResult = BUTTON_FLAG_OK;
            break;
        case 2:
            *lpdwResult = BUTTON_FLAG_CANCEL;
            break;
        case 3:
            *lpdwResult = BUTTON_FLAG_CONTINUE;
            break;
        case 4:
            *lpdwResult = BUTTON_FLAG_YES;
            break;
        case 5:
            *lpdwResult = BUTTON_FLAG_NO;
            break;
        case 6:
            *lpdwResult = BUTTON_FLAG_PREVIOUS;
            break;
        default:
            *lpdwResult = 0;
            return (NAVEX_UI_TIMEOUT);
    }

#endif

    return (NAVEX_UI_OK);
}

UINT DOS_LOADDS WINAPI NAVEXInfoBoxInit
(
    LPTSTR   lpszTitle,
    DWORD    dwButtonFlags,
    LPLPVOID lplpvInfoBoxInfo,
    LPVOID   lpvCookie
)
{

#if defined(SYM_WIN32)

    return (NAVEXInfoBoxInit32(lpszTitle,
                               dwButtonFlags,
                               lplpvInfoBoxInfo,
                               lpvCookie));

#elif defined(SYM_DOSX)

    printf("\n\n");

#endif

    return (NAVEX_UI_OK);
}

UINT DOS_LOADDS WINAPI NAVEXInfoBoxUpdate
(
    LPVOID  lpvInfoBoxInfo,
    LPTSTR  lpszText,
    LPDWORD lpdwResultFlags,
    LPVOID  lpvCookie
)
{

#if defined(SYM_WIN32)

    return (NAVEXInfoBoxUpdate32(lpvInfoBoxInfo,
                                 lpszText,
                                 lpdwResultFlags,
                                 lpvCookie));

#elif defined(SYM_DOSX)

                                        // erase the old text with 80 spaces
                                        // (This limits the text length to
                                        // 80 characters). Then display the
                                        // the new text.

    printf("\r                                                                                ");
    printf("\r%s", lpszText);

#endif

    return (NAVEX_UI_OK);
}

UINT DOS_LOADDS WINAPI NAVEXInfoBoxClose
(
    LPVOID lpvInfoBoxInfo,
    LPVOID lpvCookie
)
{

#if defined(SYM_WIN32)

    return (NAVEXInfoBoxClose32(lpvInfoBoxInfo,
                                lpvCookie));
#elif defined(SYM_DOSX)

    printf("\n\n");

#endif

    return (NAVEX_UI_OK);
}

UINT DOS_LOADDS WINAPI NAVEXTextEditBox
(
    DWORD  dwBoxID,
    LPTSTR lpszPrompt,
    LPTSTR lpszSeedText,
    LPTSTR lpszPutHere,
    UINT   nMaxLength,
    DWORD  dwMSTimeOut,
    LPVOID lpvCookie
)
{

#if defined(SYM_WIN32)

    return (NAVEXTextEditBox32(dwBoxID,
                               lpszPrompt,
                               lpszSeedText,
                               lpszPutHere,
                               nMaxLength,
                               dwMSTimeOut,
                               lpvCookie));

#else

    return (NAVEX_UI_TIMEOUT);

#endif

}

VOID DOS_LOADDS WINAPI NAVEXGetConfigInfo
(
    LPTSTR lpszID,
    LPVOID lpvResult,
    UINT   uMaxResultLen,
    LPBOOL lpbImplemented,
    LPVOID lpvCookie
)
{
#if defined(SYM_DOSX) || defined (SYM_WIN32)

    CFGTEXTREC rSwitch = {lpszID, CFG_SW_EXACT,};

    *lpbImplemented = TRUE;

    if (ConfigSwitchSet(&rSwitch, NULL))
    {
        ConfigSwitchGetText(&rSwitch, (LPSTR)lpvResult, uMaxResultLen);
    }
    else
    {
       lpvResult = NULL;
    }

#elif defined (SYM_NLM)
    EXTERN BYTE byHeuristicLevel[];

    // ##IPE Do Not translate
    if( !(STRCMP( lpszID, "heur" )) )
        {
        *lpbImplemented = TRUE;
        STRCPY( lpvResult, &byHeuristicLevel );
        }
    else
        {
        *lpbImplemented = FALSE;
        lpvResult = NULL;
        }
#else

    *lpbImplemented = FALSE;

#endif
}


VOID DOS_LOADDS WINAPI NAVEXRelinquishControl(VOID)

{
#ifdef SYM_NLM

    RelinquishControl();

#endif
}


CALLBACKREV2 gstNAVEXCallBacks =
{
    NAVEXFileOpen,  // HFILE  (WINAPI FAR *FileOpen)(LPTSTR lpName, UINT uOpenMode);
    NAVEXFileClose, // HFILE  (WINAPI FAR *FileClose)(HFILE hHandle);
    NAVEXFileRead,  // UINT   (WINAPI FAR *FileRead)(HFILE hHandle, LPVOID lpBuffer, UINT uBytes);
    NAVEXFileWrite, // UINT   (WINAPI FAR *FileWrite)(HFILE hHandle, LPVOID lpBuffer, UINT uBytes);
    NAVEXFileSeek,  // DWORD  (WINAPI FAR *FileSeek)(HFILE hHandle, LONG dwOffset, int nFlag);
    NAVEXFileLength,// DWORD  (WINAPI FAR *FileSize)(HFILE hHandle);
    NAVEXFileGetDateTime,// BOOL (WINAPI FAR *FileGetDateTime)(HFILE hHandle, UINT uType, WORD FAR *lpuDate, WORD FAR *lpuTime);
    NAVEXFileSetDateTime,// BOOL (WINAPI FAR *FileSetDateTime)(HFILE hHandle, UINT uType, WORD uDate, WORD uTime);
    NAVEXFileDelete,// BOOL   (WINAPI FAR *FileDelete)(LPTSTR lpFilename);
    NAVEXFileGetAttr,// UINT  (WINAPI FAR *FileGetAttr)(LPTSTR lpFilename, UINT FAR *lpuAttr);
    NAVEXFileSetAttr,// UINT  (WINAPI FAR *FileSetAttr)(LPTSTR lpFilename, UINT uAttr);
    NAVEXFileCreate,// HFILE  (WINAPI FAR *FileCreate)(LPTSTR lpFilename, UINT uAttr);
    NULL,           // BOOL   (WINAPI FAR *FileLock)(HFILE hHandle, DWORD dwStart, DWORD dwLength);
    NULL,           // BOOL   (WINAPI FAR *FileUnlock)(HFILE hHandle, DWORD dwStart, DWORD dwLength);
    NAVEXProgress,  // BOOL   (WINAPI FAR *Progress)(WORD wProgress, DWORD dwCookie);
#ifndef SYM_NLM
    NAVEXSDiskReadPhysical, // UINT   (WINAPI FAR *SDiskReadPhysical)(LPVOID lpBuffer, BYTE bCount, WORD wCylinder, BYTE  bSector, BYTE bHead,  BYTE bDrive);
    NAVEXSDiskWritePhysical,// UINT   (WINAPI FAR *SDiskWritePhysical)(LPVOID lpBuffer, BYTE bCount, WORD wCylinder, BYTE  bSector, BYTE bHead,  BYTE bDrive);
#else
    NULL,                // UINT   (WINAPI FAR *SDiskReadPhysical)(LPVOID lpBuffer, BYTE bCount, WORD wCylinder, BYTE  bSector, BYTE bHead,  BYTE bDrive);
    NULL,                // UINT   (WINAPI FAR *SDiskWritePhysical)(LPVOID lpBuffer, BYTE bCount, WORD wCylinder, BYTE  bSector, BYTE bHead,  BYTE bDrive);
#endif
    NAVEXTempMemoryAlloc,// LPVOID (WINAPI FAR *TempMemoryAlloc)(DWORD uSize);
    NAVEXTempMemoryFree, // VOID   (WINAPI FAR *TempMemoryFree)(LPVOID lpBuffer);
    NAVEXPermMemoryAlloc,// LPVOID (WINAPI FAR *PermMemoryAlloc)(DWORD uSize);
    NAVEXPermMemoryFree, // VOID   (WINAPI FAR *PermMemoryFree)(LPVOID lpBuffer);
    NAVEXCreateMutex,    // UINT   (WINAPI FAR *CreateMutex)(LPLPVOID lplpMutex);
    NAVEXDestroyMutex,   // UINT   (WINAPI FAR *DestroyMutex)(LPVOID lpMutex);
    NAVEXWaitMutex,      // UINT   (WINAPI FAR *WaitMutex)(LPVOID lpMutex, DWORD dwMSTicks);
    NAVEXReleaseMutex,   // UINT   (WINAPI FAR *ReleaseMutex)(LPVOID lpMutex);
    NAVEXGetTempFile,    // UINT   (WINAPI FAR *GetTempFile)(LPTSTR lpszFileName);
    NAVEXMessageBox,     // UINT   (WINAPI FAR *MessageBox)(DWORD dwBoxID, LPTSTR lpszMessage, DWORD dwButtonFlags, LPDWORD lpdwResultFlags, DWORD dwMSTimeOut);
    NAVEXInfoBoxInit,    // UINT   (WINAPI FAR *InfoBoxInit)(LPTSTR lpszTitle, DWORD dwButtonFlags, LPLPVOID lplpvInfoBoxInfo);
    NAVEXInfoBoxUpdate,  // UINT   (WINAPI FAR *InfoBoxUpdate)(LPVOID lpvInfoBoxInfo, LPTSTR lpszText, LPDWORD lpdwResultFlags);
    NAVEXInfoBoxClose,   // UINT   (WINAPI FAR *InfoBoxClose)(LPVOID lpvInfoBoxInfo);
    NAVEXTextEditBox,    // UINT   (WINAPI FAR *TextEditBox)(DWORD dwBoxID, LPTSTR lpszPrompt, LPTSTR lpszSeedText, LPTSTR lpszPutHere, int nMaxLength, DWORD dwMSTimeOut);
    NAVEXGetConfigInfo,  // VOID   (WINAPI FAR *GetConfigInfo)(LPTSTR lpszID, LPVOID lpvResult, UINT uMaxResultLen, LPBOOL lpbImplemented, LPVOID lpvCookie);
    NAVEXRelinquishControl, // VOID   (WINAPI FAR *RelinquishControl)(VOID);
};

#if defined(SYM_DOSX) || defined (SYM_NLM)
EXTERNC
#endif
SYM_EXPORT AVGLOBALCALLBACKS gstNAVCallBacks =
{
    &gstNAVGeneralCallBacks,
    &gstNAVProgressCallBacks,
    &gstNAVDataFileCallBacks,
    &gstNAVIPCCallBacks,
    &gstNAVMemoryCallBacks,
    &gstNAVEXCallBacks,
};
        

VOID SYM_EXPORT WINAPI NAVSetDefsPath(LPCTSTR szPath)
{
    STRNCPY(s_szNAVPath, szPath, SYM_MAX_PATH);
}
