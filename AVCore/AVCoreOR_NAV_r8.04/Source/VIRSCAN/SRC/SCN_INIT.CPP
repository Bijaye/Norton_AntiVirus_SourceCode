// Copyright 1993 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/VIRSCAN/VCS/scn_init.cpv   1.25   07 Aug 1996 10:38:56   MKEATIN  $
//
// Description:
//      Contains the initialization routines for scanning (and the scan DLL).
//      VirScanEnd() should be called to clean up after the initialization
//      and scanning procedures are done.
//
// Contains:
//      VirScanInit()
//      VirusScanSetPath()
//      LibMain()
//      WEP()
//
// See Also:
//      SCN_END.C for VirScanEnd()
//************************************************************************
// $Log:   S:/VIRSCAN/VCS/scn_init.cpv  $
// 
//    Rev 1.25   07 Aug 1996 10:38:56   MKEATIN
// Ported code from Virlib 8
// 
//    Rev 1.24   28 Jun 1996 11:26:36   JBRENNA
// DAlle - Kluge for VirusScanGetPath() to use valid SFN for ANSI LFN's
// that don't translate to OEM. (Add NavEx32Init and NavEx32End)
// 
//    Rev 1.23   13 Jun 1996 10:43:14   JALLEE
// Ported DX changes from virlib 8.  (Null ptr checks and Navex export table)
// 
//    Rev 1.22   22 May 1996 16:49:38   RHAYES
// Round # 2 of Dave Allee's OEM<->Ansi mod.'s in Luigi.
// 
//    Rev 1.21   21 May 1996 14:37:58   RHAYES
// Merged Luigi OEM<->Ansi code for more robust support of
// Non-Ansi directory and filenames.
// 
//    Rev 1.20   16 Feb 1996 11:38:18   CNACHEN
// Changed hLocal to ScanData...
// 
//    Rev 1.19   15 Feb 1996 16:23:22   MKEATIN
// Added Wrapper functions for navex to the DX platform (SYM_DOSX)
// 
//    Rev 1.18   14 Feb 1996 12:44:18   MKEATIN
// Made changes for navex/dx platform
// 
//    Rev 1.17   14 Feb 1996 10:03:28   CNACHEN
// Updated for PAM support.
// 
//    Rev 1.16   02 Feb 1996 14:26:20   MKEATIN
// oops - fixed syntax error
// 
//    Rev 1.15   02 Feb 1996 14:23:52   MKEATIN
// Disabled NavEx on the DX platform for now
// 
//    Rev 1.14   12 Jan 1996 07:11:42   JWORDEN
// Port changes from Quake 8
// 
//    Rev 1.13.1.11   17 Dec 1995 18:49:42   MKEATIN
// Made Changes to NavExInit()
// 
//    Rev 1.13.1.10   01 Dec 1995 18:55:46   DCHI
// In DOSEXTLoad, SS now equals DS and SP is adjusted accordingly.
// 
//    Rev 1.13.1.9   15 Nov 1995 11:32:04   MKEATIN
// Changed DOS_OVERLAY_BUFFER_SIZE constant to uDosOverlayBufferSize which is
// initialized in scn_dta.cpp.
// 
//    Rev 1.13.1.8   14 Nov 1995 18:41:22   MKEATIN
// Created NavExInit() by a cut, paste and revising code from VirusScanInit()
// Uses buffer that held memory signatures for NavEx.
// 
//    Rev 1.13.1.7   31 Oct 1995 13:06:12   CNACHEN
// Added (IFDEF'd) PAM support for W32
// 
//    Rev 1.13.1.6   19 Oct 1995 13:46:50   DCHI
// Changed DOS NAVEX external functions from LOADDS to NLOADDS.
// 
//    Rev 1.13.1.5   19 Oct 1995 11:22:08   DCHI
// Modified DOS NAVEX loader to support tiny model NAVEX modules.
// Added LOADDS to DOS NAVEX functions.
// 
//    Rev 1.13.1.4   13 Oct 1995 14:38:22   DCHI
// Added DOS NAVEX external overlay code and changed avex.h to navex.h.
// 
//    Rev 1.13.1.3   12 Oct 1995 14:18:04   DCHI
// EXTScanMemory takes bScanHighMemory parameter.
// 
//    Rev 1.13.1.2   07 Sep 1995 16:17:02   DALLEE
// Changed Loader.h to Avexload.h
// 
//    Rev 1.13.1.1   07 Sep 1995 13:49:22   CNACHEN
// Added external virus detection code.
// 
//    Rev 1.13.1.0   12 Jul 1995 18:59:52   MPYE
// Branch base for version QAK8
// 
//    Rev 1.13   07 Jul 1995 11:39:50   KEITH
// Remove an inappropriate OemToAnsi on a GetModuleFilename 
// returned path.
// 
//    Rev 1.12   06 Mar 1995 14:51:34   RSTANEV
// Added VirusScanSetPath() and modified VirusScanGetPath() to use it.
//
//    Rev 1.11   01 Feb 1995 16:55:18   MFALLEN
// Added default flag to InitCertlib() function.
//
//    Rev 1.10   28 Dec 1994 10:26:54   MFALLEN
// Found weird memory bug.
//
//    Rev 1.9   27 Dec 1994 19:06:00   DALLEE
// Use CERTLIBN for DOS platform now, too.
//
//    Rev 1.8   19 Dec 1994 22:11:16   DALLEE
// Had to #include "callback.h" for .DOS
//
//    Rev 1.7   19 Dec 1994 22:05:16   DALLEE
// Convert .WIN to CERTLIBN
//
//    Rev 1.6   01 Dec 1994 08:44:50   MFALLEN
// VirusScanDatFileGetComment.
//
//    Rev 1.5   30 Nov 1994 12:35:38   DALLEE
// Use CTS4.H for SYM_DOS, too.
//
//    Rev 1.4   28 Nov 1994 15:29:16   MFALLEN
// include sts4.h if W16 platform.
//
//    Rev 1.3   12 May 1994 11:03:54   MFALLEN
// cts4 2 ctsn
//
//    Rev 1.2   27 Apr 1994 10:56:42   BRAD
// Use SYM_MAX_PATH only
//
//    Rev 1.1   22 Apr 1994 18:16:02   MFALLEN
// Changed _loadds to SYM_EXPORT. If the TSR will need _loadds then this must
// be a macro. Without this change the LINK32 will not generate an import lib.
//
//    Rev 1.0   22 Apr 1994 10:59:02   MFALLEN
// Initial revision.
//
//    Rev 1.23   05 Apr 1994 18:54:52   BRAD
// Added DllMain
//
//    Rev 1.22   15 Mar 1994 11:09:36   BRUCE
// Changed SYM_EXPORT to SYM_EXPORT
//
//    Rev 1.21   12 Mar 1994 22:46:06   BRAD
// Cleaned up for SYM_WIN32
//
//    Rev 1.20   02 Mar 1994 17:12:38   BRAD
// AWK script applied
//
//    Rev 1.19   11 Feb 1994 15:54:58   MARKL
// Quake 2 changes moved to trunk.
//
//    Rev 1.16.1.2   02 Nov 1993 16:05:50   DALLEE
// #ifdef SYM_WIN around SetAnsiFlag() call.
//
//    Rev 1.16.1.1   01 Nov 1993 17:53:50   MFALLEN
// Call to SetAnsiFlag()
//
//    Rev 1.16.1.0   19 Oct 1993 08:56:20   SCOTTP
// Branch base for version QAK2
//
//    Rev 1.16   12 Oct 1993 16:42:44   PATRICKM
// Baseline from QAK1 branch
//
//    Rev 1.15.1.1   07 Oct 1993 17:55:48   MFALLEN
// Make sure that we only deal with ANSI strings
//
//    Rev 1.15.1.0   03 Sep 1993 22:52:48   SCOTTP
// Branch base for version QAK1
//
//    Rev 1.15   23 Aug 1993 01:51:18   BARRY
// Added MODULE_NAME and fixed MemUnlock params for VMM debugging
//
//    Rev 1.14   13 Aug 1993 15:14:32   MFALLEN
// Changed possible return statuses from TRUE/FALSE to ERR/NOERR
//
//    Rev 1.13   12 Aug 1993 17:46:00   MFALLEN
// Changed prototype to VirusScanInit()
//
//
//    Rev 1.12   28 Jul 1993 12:03:58   MFALLEN
// Allocate memory DDE_SHARE
//
//    Rev 1.11   01 Jul 1993 17:23:46   BRAD
// Added 'uUser', so multiple programs can use the VIRSCAN.DLL.
//
//    Rev 1.10   16 Jun 1993 23:02:48   BRAD
//
//    Rev 1.9   10 Jun 1993 02:51:14   CGUC
// Changed SYM_EXPORT to _loadds
//
//    Rev 1.8   17 May 1993 15:44:32   DALLEE
// Use VIRSCAN.szVirscanFile instead of VIRSCAN.lpScannerBuffer.
// Added VirusScanGetBootFile().
//
//    Rev 1.7   28 Apr 1993 09:10:20   MFALLEN
//
//    Rev 1.6   21 Apr 1993 10:58:16   MFALLEN
// Carlton specific stuff is not needed.
//
//    Rev 1.5   04 Apr 1993 19:27:48   BRAD
//
//    Rev 1.4   04 Apr 1993 17:49:32   BRAD
//
//    Rev 1.3   04 Apr 1993 17:39:24   BRAD
// Removed all references to N30.*
//
//    Rev 1.2   26 Mar 1993 14:53:32   DALLEE
//************************************************************************

#include "platform.h"
#include "xapi.h"
#include "file.h"

#include "ctsn.h"

#include "virscan.h"
#include "callback.h"

#ifdef SYM_WIN32
 #include "navex.h"
#endif

#ifdef SYM_DOSX
 #include "navex.h"
 #undef SYM_DOS
#endif

#ifdef SYM_DOS

#include "navex.h"
#include "avexload.h"                   // for DOS loader code

#endif

MODULE_NAME;

VIRSCAN ScanData;
UINT    uUsers;                         // Count of users of this DLL

#ifdef SYM_WIN
HINSTANCE hInstance;
#endif  // #ifdef SYM_WIN



//----------------------------------------------------
// Error Codes for InitNavEx32 ()
//----------------------------------------------------

#ifdef SYM_WIN32

// NOERR or...
#define ERR_NAVEX_DLL_NOT_FOUND       (STATUS) 1
#define ERR_NAVEX_FUNCTION_NOT_FOUND  (STATUS) 2
#define ERR_NAVSTAT_DLL_NOT_FREED       (STATUS) 3

#endif


#if defined (SYM_DOSX) || defined (SYM_WIN32)

//-----------------------------------------------------
// Navex global function pointer struct declared here
//-----------------------------------------------------

EXPORT_TABLE_TYPE export_table;


//-----------------------------------------------------
// Wrapper functions for DX and WIN32 defined here
//-----------------------------------------------------
EXTSTATUS FAR WINAPI NLOADDS EXTScanFile(LPCALLBACKREV1 a,
                        LPTSTR         b,
                        HFILE          c,
                        LPBYTE         d,
			LPBYTE         e,
                        WORD           f,
                        LPWORD         g)
{
    if ( export_table.EXTScanFile == NULL )
        return (EXTSTATUS_OK);
    else
        return(export_table.EXTScanFile(a,b,c,d,e,f,g));
}


EXTSTATUS FAR WINAPI NLOADDS EXTScanBoot(LPCALLBACKREV1 a,
                        BYTE       b,
                        BYTE       c,
                        WORD d, WORD e, WORD f,
                        LPBYTE     g,
                        LPBYTE     h,
                        WORD       i,
                        LPWORD     j)
{
    if ( export_table.EXTScanBoot == NULL )
        return (EXTSTATUS_OK);
    else
        return(export_table.EXTScanBoot(a,b,c,d,e,f,g,h,i,j));
}


EXTSTATUS FAR WINAPI NLOADDS EXTScanPart(LPCALLBACKREV1 a,
                        BYTE       b,
                        LPBYTE     c,
                        LPBYTE     d,
                        WORD       e,
                        LPWORD     f)
{
    if ( export_table.EXTScanPart == NULL )
        return (EXTSTATUS_OK);
    else
        return(export_table.EXTScanPart(a,b,c,d,e,f));
}


EXTSTATUS FAR WINAPI NLOADDS EXTScanMemory(LPCALLBACKREV1 a,
                          WORD   b,
                          BOOL   c,
                          LPWORD d)
{
    if ( export_table.EXTScanMemory == NULL )
        return (EXTSTATUS_OK);
    else
        return(export_table.EXTScanMemory(a,b,c,d));
}


EXTSTATUS FAR WINAPI NLOADDS EXTRepairFile(LPCALLBACKREV1 a,
                          WORD       b,
                          LPN30      c,
                          LPTSTR     d,
                          LPBYTE     e)
{
    if ( export_table.EXTRepairFile == NULL )
        return (EXTSTATUS_OK);
    else
        return(export_table.EXTRepairFile(a,b,c,d,e));
}


EXTSTATUS FAR WINAPI NLOADDS EXTRepairBoot(LPCALLBACKREV1 a,
                          WORD       b,
                          LPN30      c,
                          BYTE       d,
                          BYTE       e,
                          WORD f, WORD g, WORD h,
                          LPBYTE     i)
{
    if ( export_table.EXTRepairBoot == NULL )
        return (EXTSTATUS_OK);
    else
        return(export_table.EXTRepairBoot(a,b,c,d,e,f,g,h,i));
}


EXTSTATUS FAR WINAPI NLOADDS EXTRepairPart(LPCALLBACKREV1 a,
                          WORD       b,
                          LPN30      c,
                          BYTE       d,
                          LPBYTE     e)
{
    if ( export_table.EXTRepairPart == NULL )
        return (EXTSTATUS_OK);
    else
        return(export_table.EXTRepairPart(a,b,c,d,e));
}

#endif // SYM_DOSX || SYM_WIN32


#ifdef SYM_DOS

// global variables for EXTernal code module follow

GLOBAL_LOADER_INFO_TYPE     gLoaderInfo;

LPSTR gpszNAVEXFileName = "NAVEX.DAT";

// functions for EXTernal code module follow

//
// DOSEXTGetModuleInfo()
//
//  Assumptions:
//      - Calling function has the file open.
//      - File pointer is at the beginning of the module.
//      - Calling function closes the file.
//

EXTSTATUS DOSEXTGetModuleInfo(HFILE     hFile,
                              LPBYTE    lpbySwapArea,
                              WORD      wSwapSize,
                              LPWORD    lpwImageSize,
                              LPDWORD   lpdwOffset,
                              LPDWORD   lpdwEntrypoint)
{
    WORD i;

    if (FileRead(hFile,lpwImageSize,sizeof(WORD)) != sizeof(WORD))
        return(EXTSTATUS_FILE_ERROR);

	// determine where the IMAGE starts

    *lpdwOffset = FileSeek(hFile,0,SEEK_CUR);

    if (*lpwImageSize > wSwapSize)
    {
        // external code is too large to fit!

        return(EXTSTATUS_MEM_ERROR);
    }

    if (*lpwImageSize != FileRead(hFile, lpbySwapArea, *lpwImageSize))
        return(EXTSTATUS_FILE_ERROR);

    for (i=0;i<*lpwImageSize;i++)
    {
		if (0 == MEMCMP(lpbySwapArea+i,EXPORT_STRING,16))
        {
            *lpdwEntrypoint = (DWORD)(((PMODULE_EXPORT_TABLE_TYPE)
                (lpbySwapArea + i))->wEntrypoint);

			break;
		}
	}

    if (i == *lpwImageSize)
		return(EXTSTATUS_MEM_ERROR);

	return(EXTSTATUS_OK);
}


EXTSTATUS DOSEXTLoad(LPSTR lpszPath, LPVOID lpbySwapArea, WORD wSwapSize)
{
	HFILE               hFile;
    WORD                wResult;
    DWORD               dwTemp, *dwPtr;

    // Align on paragraph boundary

    dwTemp = ((DWORD)lpbySwapArea >> 16) +
             (((DWORD)lpbySwapArea & 0xFFFF) >> 4);
	dwTemp = ((dwTemp+1) << 16);
    gLoaderInfo.lpSwapArea = (LPVOID)dwTemp;

    wSwapSize -= 16;

    hFile = FileOpen(lpszPath,READ_ONLY_FILE);
	if ((HFILE)-1 == hFile)
		return(EXTSTATUS_FILE_ERROR);

    // Seek past header

    if (FileSeek(hFile,256,SEEK_SET) != 256)
    {
        FileClose(hFile);
        return(EXTSTATUS_FILE_ERROR);
    }

    // Get size, offset, and entrypoint for all modules

    wResult = DOSEXTGetModuleInfo(hFile,
                (LPBYTE)gLoaderInfo.lpSwapArea,
                wSwapSize,
                &(gLoaderInfo.oiEXTScanFile.wImageSize),
                &(gLoaderInfo.oiEXTScanFile.dwOffset),
                (LPDWORD)&(gLoaderInfo.ExportTable.EXTScanFile));

    if (wResult == EXTSTATUS_OK)
        wResult = DOSEXTGetModuleInfo(hFile,
                    (LPBYTE)gLoaderInfo.lpSwapArea,
                    wSwapSize,
                    &(gLoaderInfo.oiEXTScanBoot.wImageSize),
                    &(gLoaderInfo.oiEXTScanBoot.dwOffset),
                    (LPDWORD)&(gLoaderInfo.ExportTable.EXTScanBoot));

    if (wResult == EXTSTATUS_OK)
        wResult = DOSEXTGetModuleInfo(hFile,
                    (LPBYTE)gLoaderInfo.lpSwapArea,
                    wSwapSize,
                    &(gLoaderInfo.oiEXTScanPart.wImageSize),
                    &(gLoaderInfo.oiEXTScanPart.dwOffset),
                    (LPDWORD)&(gLoaderInfo.ExportTable.EXTScanPart));

    if (wResult == EXTSTATUS_OK)
        wResult = DOSEXTGetModuleInfo(hFile,
                    (LPBYTE)gLoaderInfo.lpSwapArea,
                    wSwapSize,
                    &(gLoaderInfo.oiEXTScanMemory.wImageSize),
                    &(gLoaderInfo.oiEXTScanMemory.dwOffset),
                    (LPDWORD)&(gLoaderInfo.ExportTable.EXTScanMemory));

    if (wResult == EXTSTATUS_OK)
        wResult = DOSEXTGetModuleInfo(hFile,
                    (LPBYTE)gLoaderInfo.lpSwapArea,
                    wSwapSize,
                    &(gLoaderInfo.oiEXTRepairFile.wImageSize),
                    &(gLoaderInfo.oiEXTRepairFile.dwOffset),
                    (LPDWORD)&(gLoaderInfo.ExportTable.EXTRepairFile));

    if (wResult == EXTSTATUS_OK)
        wResult = DOSEXTGetModuleInfo(hFile,
                    (LPBYTE)gLoaderInfo.lpSwapArea,
                    wSwapSize,
                    &(gLoaderInfo.oiEXTRepairBoot.wImageSize),
                    &(gLoaderInfo.oiEXTRepairBoot.dwOffset),
                    (LPDWORD)&(gLoaderInfo.ExportTable.EXTRepairBoot));

    if (wResult == EXTSTATUS_OK)
        wResult = DOSEXTGetModuleInfo(hFile,
                    (LPBYTE)gLoaderInfo.lpSwapArea,
                    wSwapSize,
                    &(gLoaderInfo.oiEXTRepairPart.wImageSize),
                    &(gLoaderInfo.oiEXTRepairPart.dwOffset),
                    (LPDWORD)&(gLoaderInfo.ExportTable.EXTRepairPart));

    FileClose(hFile);

    if (wResult != EXTSTATUS_OK)
        return(wResult);

    // Insert proper segment value for entrypoints

    dwPtr = (DWORD *)&(gLoaderInfo.ExportTable.EXTScanFile);

    *dwPtr = ((DWORD)gLoaderInfo.ExportTable.EXTScanFile & 0xFFFFU) +
                ((DWORD)(FP_SEG(gLoaderInfo.lpSwapArea) - 0x10)<<16);

    dwPtr = (DWORD *)&(gLoaderInfo.ExportTable.EXTScanBoot);

    *dwPtr = ((DWORD)gLoaderInfo.ExportTable.EXTScanBoot & 0xFFFFU) +
                ((DWORD)(FP_SEG(gLoaderInfo.lpSwapArea) - 0x10)<<16);

    dwPtr = (DWORD *)&gLoaderInfo.ExportTable.EXTScanPart;

    *dwPtr = ((DWORD)gLoaderInfo.ExportTable.EXTScanPart & 0xFFFFU) +
                ((DWORD)(FP_SEG(gLoaderInfo.lpSwapArea) - 0x10)<<16);

    dwPtr = (DWORD *)&gLoaderInfo.ExportTable.EXTScanMemory;

    *dwPtr = ((DWORD)gLoaderInfo.ExportTable.EXTScanMemory & 0xFFFFU) +
                ((DWORD)(FP_SEG(gLoaderInfo.lpSwapArea) - 0x10)<<16);

    dwPtr = (DWORD *)&gLoaderInfo.ExportTable.EXTRepairFile;

    *dwPtr = ((DWORD)gLoaderInfo.ExportTable.EXTRepairFile & 0xFFFFU) +
                ((DWORD)(FP_SEG(gLoaderInfo.lpSwapArea) - 0x10)<<16);

    dwPtr = (DWORD *)&gLoaderInfo.ExportTable.EXTRepairBoot;

    *dwPtr = ((DWORD)gLoaderInfo.ExportTable.EXTRepairBoot & 0xFFFFU) +
                ((DWORD)(FP_SEG(gLoaderInfo.lpSwapArea) - 0x10)<<16);

    dwPtr = (DWORD *)&gLoaderInfo.ExportTable.EXTRepairPart;

    *dwPtr = ((DWORD)gLoaderInfo.ExportTable.EXTRepairPart & 0xFFFFU) +
                ((DWORD)(FP_SEG(gLoaderInfo.lpSwapArea) - 0x10)<<16);

    // SP for our local stack

    gLoaderInfo.wExportSP = wSwapSize + 0x100;

    // SS for our local stack

    gLoaderInfo.wExportSS = FP_SEG(gLoaderInfo.lpSwapArea) - 0x10;

    gLoaderInfo.wResidentModule = EXTRESIDENT_NONE;

	return(EXTSTATUS_OK);
}


EXTSTATUS DOSEXTLoadModule(WORD wModule)
{
    HFILE   hFile;
    WORD    wResult = EXTSTATUS_FILE_ERROR;
    char    szN30[MAX_PATH_SIZE+1];

    VirusScanGetDatFile(szN30,sizeof(szN30) - 1);
    NameStripFile(szN30);
    NameAppendFile(szN30,gpszNAVEXFileName);

    hFile = FileOpen(szN30,READ_ONLY_FILE);
	if ((HFILE)-1 == hFile)
        return(EXTSTATUS_FILE_ERROR);

    switch (wModule)
    {
        case EXTRESIDENT_SCANFILE:
            if ((FileSeek(hFile,
                          gLoaderInfo.oiEXTScanFile.dwOffset,
                          SEEK_SET)
                    == (LONG)gLoaderInfo.oiEXTScanFile.dwOffset)
                &&
                (FileRead(hFile,
                          gLoaderInfo.lpSwapArea,
                          gLoaderInfo.oiEXTScanFile.wImageSize)
                    == gLoaderInfo.oiEXTScanFile.wImageSize))
            {
                wResult = EXTSTATUS_OK;
            }
            break;

        case EXTRESIDENT_SCANBOOT:
            if ((FileSeek(hFile,
                          gLoaderInfo.oiEXTScanBoot.dwOffset,
                          SEEK_SET)
                    == (LONG)gLoaderInfo.oiEXTScanBoot.dwOffset)
                &&
                (FileRead(hFile,
                          gLoaderInfo.lpSwapArea,
                          gLoaderInfo.oiEXTScanBoot.wImageSize)
                    == gLoaderInfo.oiEXTScanBoot.wImageSize))
            {
                wResult = EXTSTATUS_OK;
            }
            break;

        case EXTRESIDENT_SCANPART:
            if ((FileSeek(hFile,
                          gLoaderInfo.oiEXTScanPart.dwOffset,
                          SEEK_SET)
                    == (LONG)gLoaderInfo.oiEXTScanPart.dwOffset)
                &&
                (FileRead(hFile,
                          gLoaderInfo.lpSwapArea,
                          gLoaderInfo.oiEXTScanPart.wImageSize)
                    == gLoaderInfo.oiEXTScanPart.wImageSize))
            {
                wResult = EXTSTATUS_OK;
            }
            break;

        case EXTRESIDENT_SCANMEMORY:
            if ((FileSeek(hFile,
                          gLoaderInfo.oiEXTScanMemory.dwOffset,
                          SEEK_SET)
                    == (LONG)gLoaderInfo.oiEXTScanMemory.dwOffset)
                &&
                (FileRead(hFile,
                          gLoaderInfo.lpSwapArea,
                          gLoaderInfo.oiEXTScanMemory.wImageSize)
                    == gLoaderInfo.oiEXTScanMemory.wImageSize))
            {
                wResult = EXTSTATUS_OK;
            }
            break;

        case EXTRESIDENT_REPAIRFILE:
            if ((FileSeek(hFile,
                          gLoaderInfo.oiEXTRepairFile.dwOffset,
                          SEEK_SET)
                    == (LONG)gLoaderInfo.oiEXTRepairFile.dwOffset)
                &&
                (FileRead(hFile,
                          gLoaderInfo.lpSwapArea,
                          gLoaderInfo.oiEXTRepairFile.wImageSize)
                    == gLoaderInfo.oiEXTRepairFile.wImageSize))
            {
                wResult = EXTSTATUS_OK;
            }
            break;

        case EXTRESIDENT_REPAIRBOOT:
            if ((FileSeek(hFile,
                          gLoaderInfo.oiEXTRepairBoot.dwOffset,
                          SEEK_SET)
                    == (LONG)gLoaderInfo.oiEXTRepairBoot.dwOffset)
                &&
                (FileRead(hFile,
                          gLoaderInfo.lpSwapArea,
                          gLoaderInfo.oiEXTRepairBoot.wImageSize)
                    == gLoaderInfo.oiEXTRepairBoot.wImageSize))
            {
                wResult = EXTSTATUS_OK;
            }
            break;

        case EXTRESIDENT_REPAIRPART:
            if ((FileSeek(hFile,
                          gLoaderInfo.oiEXTRepairPart.dwOffset,
                          SEEK_SET)
                    == (LONG)gLoaderInfo.oiEXTRepairPart.dwOffset)
                &&
                (FileRead(hFile,
                          gLoaderInfo.lpSwapArea,
                          gLoaderInfo.oiEXTRepairPart.wImageSize)
                    == gLoaderInfo.oiEXTRepairPart.wImageSize))
            {
                wResult = EXTSTATUS_OK;
            }
            break;

        default:
            break;
    }

    FileClose(hFile);

    if (wResult == EXTSTATUS_OK)
        gLoaderInfo.wResidentModule = wModule;
    else
        gLoaderInfo.wResidentModule = EXTRESIDENT_NONE;

    return(wResult);
}


LPCALLBACKREV1  ESFlpCallBack;
LPSTR           ESFlpszFileName;
HFILE           ESFhFile;
LPBYTE          ESFlpbyInfectionBuffer;
LPBYTE          ESFlpbyWorkBuffer;
WORD			ESFwVersionNumber;
LPWORD			ESFlpwVID;


EXTSTATUS FAR WINAPI NLOADDS EXTScanFile(LPCALLBACKREV1 lpCallBack,
                                         LPSTR          lpszFileName,
                                         HFILE          hFile,
                                         LPBYTE         lpbyInfectionBuffer,
                                         LPBYTE         lpbyWorkBuffer,
                                         WORD           wVersionNumber,
                                         LPWORD         lpwVID)
{

    if (FALSE == gLoaderInfo.bInitialized)
        return(EXTSTATUS_OK);

    if (gLoaderInfo.wResidentModule != EXTRESIDENT_SCANFILE)
    {
        // Module was not resident

        WORD wResult = DOSEXTLoadModule(EXTRESIDENT_SCANFILE);

        if (wResult != EXTSTATUS_OK)
            return wResult;
    }

    ESFlpCallBack = lpCallBack;
    ESFlpszFileName = lpszFileName;
    ESFhFile = hFile;
    ESFlpbyInfectionBuffer = lpbyInfectionBuffer;
    ESFlpbyWorkBuffer = lpbyWorkBuffer;
    ESFwVersionNumber = wVersionNumber;
    ESFlpwVID = lpwVID;

    _asm
        {
        mov ax, seg gLoaderInfo.wSS
        mov es, ax
        mov di, offset gLoaderInfo.wSS
        mov es:[di], ss

        mov ax, seg gLoaderInfo.wSP
        mov es, ax
        mov di, offset gLoaderInfo.wSP
        mov es:[di], sp

        cli
        mov ax, seg gLoaderInfo.wExportSS
        mov es, ax
        mov di, offset gLoaderInfo.wExportSS
        mov ss, es:[di]
        mov ax, seg gLoaderInfo.wExportSP
        mov es, ax
        mov di, offset gLoaderInfo.wExportSP
        mov sp, es:[di]
        sti
        }

    gLoaderInfo.result =
			 gLoaderInfo.ExportTable.EXTScanFile(ESFlpCallBack,
                                                 ESFlpszFileName,
												 ESFhFile,
                                                 ESFlpbyInfectionBuffer,
												 ESFlpbyWorkBuffer,
												 ESFwVersionNumber,
												 ESFlpwVID);

    _asm
        {
        cli
        mov ax, seg gLoaderInfo.wSS;
        mov es, ax
        mov di, offset gLoaderInfo.wSS;
        mov ss, es:[di]

        mov ax, seg gLoaderInfo.wSP;
        mov es, ax
        mov di, offset gLoaderInfo.wSP;
        mov sp, es:[di]
        sti
        }


    return(gLoaderInfo.result);
}


LPCALLBACKREV1  ESBlpCallBack;
BYTE            ESBbyDrive;
BYTE            ESBbyBootSecNum;
WORD            ESBwSector;
WORD            ESBwCylinder;
WORD            ESBwHead;
LPBYTE          ESBlpbyBootBuffer;
LPBYTE          ESBlpbyWorkBuffer;
WORD            ESBwVersionNumber;
LPWORD          ESBlpwVID;

EXTSTATUS FAR WINAPI NLOADDS EXTScanBoot(LPCALLBACKREV1 lpCallBack,
                                         BYTE           byDrive,
                                         BYTE           byBootSecNum,
                                         WORD           wSector,
                                         WORD           wCylinder,
                                         WORD           wHead,
                                         LPBYTE         lpbyBootBuffer,
                                         LPBYTE         lpbyWorkBuffer,
                                         WORD           wVersionNumber,
                                         LPWORD         lpwVID)
{
    if (FALSE == gLoaderInfo.bInitialized)
        return(EXTSTATUS_OK);

    if (gLoaderInfo.wResidentModule != EXTRESIDENT_SCANBOOT)
    {
        // Module was not resident

        WORD wResult = DOSEXTLoadModule(EXTRESIDENT_SCANBOOT);

        if (wResult != EXTSTATUS_OK)
            return wResult;
    }

    ESBlpCallBack = lpCallBack;
    ESBbyDrive = byDrive;
    ESBbyBootSecNum = byBootSecNum;
    ESBwSector = wSector;
    ESBwCylinder = wCylinder;
    ESBwHead = wHead;
    ESBlpbyBootBuffer = lpbyBootBuffer;
    ESBlpbyWorkBuffer = lpbyWorkBuffer;
    ESBwVersionNumber = wVersionNumber;
    ESBlpwVID = lpwVID;

    _asm
        {
        mov ax, seg gLoaderInfo.wSS
        mov es, ax
        mov di, offset gLoaderInfo.wSS
        mov es:[di], ss

        mov ax, seg gLoaderInfo.wSP
        mov es, ax
        mov di, offset gLoaderInfo.wSP
        mov es:[di], sp

        cli
        mov ax, seg gLoaderInfo.wExportSS
        mov es, ax
        mov di, offset gLoaderInfo.wExportSS
        mov ss, es:[di]
        mov ax, seg gLoaderInfo.wExportSP
        mov es, ax
        mov di, offset gLoaderInfo.wExportSP
        mov sp, es:[di]
        sti
        }



    gLoaderInfo.result = gLoaderInfo.ExportTable.EXTScanBoot(ESBlpCallBack,
                                                             ESBbyDrive,
                                                             ESBbyBootSecNum,
                                                             ESBwSector,
                                                             ESBwCylinder,
                                                             ESBwHead,
                                                             ESBlpbyBootBuffer,
                                                             ESBlpbyWorkBuffer,
                                                             ESBwVersionNumber,
															 ESBlpwVID);

    _asm
        {
        cli
        mov ax, seg gLoaderInfo.wSS;
        mov es, ax
        mov di, offset gLoaderInfo.wSS;
        mov ss, es:[di]

        mov ax, seg gLoaderInfo.wSP;
        mov es, ax
        mov di, offset gLoaderInfo.wSP;
        mov sp, es:[di]
        sti
        }


	return(gLoaderInfo.result);
}


LPCALLBACKREV1  ESPlpCallBack;
BYTE            ESPbyDrive;
LPBYTE          ESPlpbyPartBuffer;
LPBYTE          ESPlpbyWorkBuffer;
WORD            ESPwVersionNumber;
LPWORD          ESPlpwVID;


EXTSTATUS FAR WINAPI NLOADDS EXTScanPart(LPCALLBACKREV1 lpCallBack,
                                         BYTE           byDrive,
                                         LPBYTE         lpbyPartBuffer,
                                         LPBYTE         lpbyWorkBuffer,
                                         WORD           wVersionNumber,
                                         LPWORD         lpwVID)
{
    if (FALSE == gLoaderInfo.bInitialized)
        return(EXTSTATUS_OK);

    if (gLoaderInfo.wResidentModule != EXTRESIDENT_SCANPART)
    {
        // Module was not resident

        WORD wResult = DOSEXTLoadModule(EXTRESIDENT_SCANPART);

        if (wResult != EXTSTATUS_OK)
            return wResult;
    }

    ESPlpCallBack = lpCallBack;
    ESPbyDrive = byDrive;
    ESPlpbyPartBuffer = lpbyPartBuffer;
    ESPlpbyWorkBuffer = lpbyWorkBuffer;
    ESPwVersionNumber = wVersionNumber;
    ESPlpwVID = lpwVID;

    _asm
        {
        mov ax, seg gLoaderInfo.wSS
        mov es, ax
        mov di, offset gLoaderInfo.wSS
        mov es:[di], ss

        mov ax, seg gLoaderInfo.wSP
        mov es, ax
        mov di, offset gLoaderInfo.wSP
        mov es:[di], sp

        cli
        mov ax, seg gLoaderInfo.wExportSS
        mov es, ax
        mov di, offset gLoaderInfo.wExportSS
        mov ss, es:[di]
        mov ax, seg gLoaderInfo.wExportSP
        mov es, ax
        mov di, offset gLoaderInfo.wExportSP
        mov sp, es:[di]
        sti
        }



	gLoaderInfo.result = gLoaderInfo.ExportTable.EXTScanPart(ESPlpCallBack,
															 ESPbyDrive,
															 ESPlpbyPartBuffer,
															 ESPlpbyWorkBuffer,
															 ESPwVersionNumber,
															 ESPlpwVID);

    _asm
        {
        cli
        mov ax, seg gLoaderInfo.wSS;
        mov es, ax
        mov di, offset gLoaderInfo.wSS;
        mov ss, es:[di]

        mov ax, seg gLoaderInfo.wSP;
        mov es, ax
        mov di, offset gLoaderInfo.wSP;
        mov sp, es:[di]
        sti
        }

	return (gLoaderInfo.result);
}

LPCALLBACKREV1  ESMlpCallBack;
WORD            ESMwVersionNumber;
BOOL            ESMbScanHighMemory;
LPWORD          ESMlpwVID;

EXTSTATUS WINAPI NLOADDS EXTScanMemory(LPCALLBACKREV1 lpCallBack,
                                       WORD           wVersionNumber,
                                       BOOL           bScanHighMemory,
                                       LPWORD         lpwVID)
{
    if (FALSE == gLoaderInfo.bInitialized)
        return(EXTSTATUS_OK);

    if (gLoaderInfo.wResidentModule != EXTRESIDENT_SCANMEMORY)
    {
        // Module was not resident

        WORD wResult = DOSEXTLoadModule(EXTRESIDENT_SCANMEMORY);

        if (wResult != EXTSTATUS_OK)
            return wResult;
    }

    ESMlpCallBack = lpCallBack;
    ESMwVersionNumber = wVersionNumber;
    ESMbScanHighMemory = bScanHighMemory;
    ESMlpwVID = lpwVID;

    _asm
        {
        mov ax, seg gLoaderInfo.wSS
        mov es, ax
        mov di, offset gLoaderInfo.wSS
        mov es:[di], ss

        mov ax, seg gLoaderInfo.wSP
        mov es, ax
        mov di, offset gLoaderInfo.wSP
        mov es:[di], sp

        cli
        mov ax, seg gLoaderInfo.wExportSS
        mov es, ax
        mov di, offset gLoaderInfo.wExportSS
        mov ss, es:[di]
        mov ax, seg gLoaderInfo.wExportSP
        mov es, ax
        mov di, offset gLoaderInfo.wExportSP
        mov sp, es:[di]
        sti
        }


    gLoaderInfo.result = gLoaderInfo.ExportTable.EXTScanMemory(ESMlpCallBack,
                                                               ESMwVersionNumber,
                                                               ESMbScanHighMemory,
                                                               ESMlpwVID);

    _asm
        {
        cli
        mov ax, seg gLoaderInfo.wSS;
        mov es, ax
        mov di, offset gLoaderInfo.wSS;
        mov ss, es:[di]

        mov ax, seg gLoaderInfo.wSP;
        mov es, ax
        mov di, offset gLoaderInfo.wSP;
        mov sp, es:[di]
        sti
        }


	return (gLoaderInfo.result);
}


LPCALLBACKREV1  ERFlpCallBack;
WORD            ERFwVersionNumber;
LPN30           ERFlpsN30;
LPSTR           ERFlpszFileName;
LPBYTE          ERFlpbyWorkBuffer;

EXTSTATUS FAR WINAPI NLOADDS EXTRepairFile(LPCALLBACKREV1 lpCallBack,
                                           WORD           wVersionNumber,
                                           LPN30          lpsN30,
                                           LPSTR          lpszFileName,
                                           LPBYTE         lpbyWorkBuffer)
{
    if (FALSE == gLoaderInfo.bInitialized)
        return(EXTSTATUS_NO_REPAIR);

    if (gLoaderInfo.wResidentModule != EXTRESIDENT_REPAIRFILE)
    {
        // Module was not resident

        WORD wResult = DOSEXTLoadModule(EXTRESIDENT_REPAIRFILE);

        if (wResult != EXTSTATUS_OK)
            return wResult;
    }

    ERFlpCallBack = lpCallBack;
    ERFwVersionNumber = wVersionNumber;
    ERFlpsN30 = lpsN30;
    ERFlpszFileName = lpszFileName;
	ERFlpbyWorkBuffer = lpbyWorkBuffer;

    _asm
        {
        mov ax, seg gLoaderInfo.wSS
        mov es, ax
        mov di, offset gLoaderInfo.wSS
        mov es:[di], ss

        mov ax, seg gLoaderInfo.wSP
        mov es, ax
        mov di, offset gLoaderInfo.wSP
        mov es:[di], sp

        cli
        mov ax, seg gLoaderInfo.wExportSS
        mov es, ax
        mov di, offset gLoaderInfo.wExportSS
        mov ss, es:[di]
        mov ax, seg gLoaderInfo.wExportSP
        mov es, ax
        mov di, offset gLoaderInfo.wExportSP
        mov sp, es:[di]
        sti
        }



    gLoaderInfo.result = gLoaderInfo.ExportTable.EXTRepairFile(ERFlpCallBack,
                                                               ERFwVersionNumber,
                                                               ERFlpsN30,
                                                               ERFlpszFileName,
															   ERFlpbyWorkBuffer);

    _asm
        {
        cli
        mov ax, seg gLoaderInfo.wSS;
        mov es, ax
        mov di, offset gLoaderInfo.wSS;
        mov ss, es:[di]

        mov ax, seg gLoaderInfo.wSP;
        mov es, ax
        mov di, offset gLoaderInfo.wSP;
        mov sp, es:[di]
        sti
        }

	return(gLoaderInfo.result);
}


LPCALLBACKREV1 ERBlpCallBack;
WORD           ERBwVersionNumber;
LPN30          ERBlpsN30;
BYTE           ERBbyDrive;
BYTE           ERBbyBootSecNum;
WORD           ERBwSector;
WORD           ERBwCylinder;
WORD           ERBwHead;
LPBYTE         ERBlpbyWorkBuffer;


EXTSTATUS FAR WINAPI NLOADDS EXTRepairBoot(LPCALLBACKREV1 lpCallBack,
                                           WORD           wVersionNumber,
                                           LPN30          lpsN30,
                                           BYTE           byDrive,
                                           BYTE           byBootSecNum,
                                           WORD           wSector,
                                           WORD           wCylinder,
                                           WORD           wHead,
                                           LPBYTE         lpbyWorkBuffer)
{
    if (FALSE == gLoaderInfo.bInitialized)
        return(EXTSTATUS_NO_REPAIR);

    if (gLoaderInfo.wResidentModule != EXTRESIDENT_REPAIRBOOT)
    {
        // Module was not resident

        WORD wResult = DOSEXTLoadModule(EXTRESIDENT_REPAIRBOOT);

        if (wResult != EXTSTATUS_OK)
            return wResult;
    }

    ERBlpCallBack = lpCallBack;
    ERBwVersionNumber = wVersionNumber;
    ERBlpsN30 = lpsN30;
    ERBbyDrive = byDrive;
    ERBbyBootSecNum = byBootSecNum;
    ERBwSector = wSector;
    ERBwCylinder = wCylinder;
    ERBwHead = wHead;
    ERBlpbyWorkBuffer = lpbyWorkBuffer;

    _asm
        {
        mov ax, seg gLoaderInfo.wSS
        mov es, ax
        mov di, offset gLoaderInfo.wSS
        mov es:[di], ss

        mov ax, seg gLoaderInfo.wSP
        mov es, ax
        mov di, offset gLoaderInfo.wSP
        mov es:[di], sp

        cli
        mov ax, seg gLoaderInfo.wExportSS
        mov es, ax
        mov di, offset gLoaderInfo.wExportSS
        mov ss, es:[di]
        mov ax, seg gLoaderInfo.wExportSP
        mov es, ax
        mov di, offset gLoaderInfo.wExportSP
        mov sp, es:[di]
        sti
        }



    gLoaderInfo.result = gLoaderInfo.ExportTable.EXTRepairBoot(ERBlpCallBack,
                                                               ERBwVersionNumber,
                                                               ERBlpsN30,
                                                               ERBbyDrive,
                                                               ERBbyBootSecNum,
                                                               ERBwSector,
                                                               ERBwCylinder,
                                                               ERBwHead,
                                                               ERBlpbyWorkBuffer);

    _asm
        {
        cli
        mov ax, seg gLoaderInfo.wSS;
        mov es, ax
        mov di, offset gLoaderInfo.wSS;
        mov ss, es:[di]

        mov ax, seg gLoaderInfo.wSP;
        mov es, ax
        mov di, offset gLoaderInfo.wSP;
        mov sp, es:[di]
        sti
        }

	return(gLoaderInfo.result);
}


LPCALLBACKREV1 ERPlpCallBack;
WORD           ERPwVersionNumber;
LPN30          ERPlpsN30;
BYTE           ERPbyDrive;
LPBYTE         ERPlpbyWorkBuffer;



EXTSTATUS FAR WINAPI NLOADDS EXTRepairPart(LPCALLBACKREV1 lpCallBack,
                                           WORD           wVersionNumber,
                                           LPN30          lpsN30,
                                           BYTE           byDrive,
                                           LPBYTE         lpbyWorkBuffer)
{
    if (FALSE == gLoaderInfo.bInitialized)
        return(EXTSTATUS_NO_REPAIR);

    if (gLoaderInfo.wResidentModule != EXTRESIDENT_REPAIRPART)
    {
        // Module was not resident

        WORD wResult = DOSEXTLoadModule(EXTRESIDENT_REPAIRPART);

        if (wResult != EXTSTATUS_OK)
            return wResult;
    }

    ERPlpCallBack = lpCallBack;
	ERPwVersionNumber = wVersionNumber;
	ERPlpsN30 = lpsN30;
	ERPbyDrive = byDrive;
	ERPlpbyWorkBuffer = lpbyWorkBuffer;

    _asm
        {
        mov ax, seg gLoaderInfo.wSS
        mov es, ax
        mov di, offset gLoaderInfo.wSS
        mov es:[di], ss

        mov ax, seg gLoaderInfo.wSP
        mov es, ax
        mov di, offset gLoaderInfo.wSP
        mov es:[di], sp

        cli
        mov ax, seg gLoaderInfo.wExportSS
        mov es, ax
        mov di, offset gLoaderInfo.wExportSS
        mov ss, es:[di]
        mov ax, seg gLoaderInfo.wExportSP
        mov es, ax
        mov di, offset gLoaderInfo.wExportSP
        mov sp, es:[di]
        sti
        }



    gLoaderInfo.result = gLoaderInfo.ExportTable.EXTRepairPart(ERPlpCallBack,
                                                               ERPwVersionNumber,
                                                               ERPlpsN30,
                                                               ERPbyDrive,
                                                               ERPlpbyWorkBuffer);

    _asm
        {
        cli
        mov ax, seg gLoaderInfo.wSS;
        mov es, ax
        mov di, offset gLoaderInfo.wSS;
        mov ss, es:[di]

        mov ax, seg gLoaderInfo.wSP;
        mov es, ax
        mov di, offset gLoaderInfo.wSP;
        mov sp, es:[di]
        sti
        }


    return(gLoaderInfo.result);
}

#endif      // SYM_DOS


//************************************************************************
// VirScanInit()
//
//  Initializes all the scanning specific buffers,
//  which implies that this function should be the
//  first to be called before scanning begins.
//  If this function returns FALSE, don't even try
//  to scan, just call VirScanEnd() to clean up.
//
//
// Parameters: none
//
// Returns:
//  TRUE; OK
//  FALSE; Error
//
//************************************************************************
// 3/\93 Martin created.
//************************************************************************

UINT SYM_EXPORT WINAPI VirusScanInit (VOID)
{                                                           
    auto    UINT        uStatus = TRUE;
    auto    BOOL        bReCreated = FALSE;
#ifdef SYM_WIN32
    BOOL bApisWereAnsi = AreFileApisANSI ();
#endif

///////////////////////////////////////////////////////////////////////
// PAM
///////////////////////////////////////////////////////////////////////
#ifdef USE_PAM
    char        szPAM[SYM_MAX_PATH+1];
    PAMSTATUS   pamStatus;
#endif
///////////////////////////////////////////////////////////////////////


#ifdef SYM_WIN32
    if (bApisWereAnsi)
        SetFileApisToOEM ();
#endif

    if ( uUsers == 0 )
        {

#ifdef SYM_WIN32
        if (NavEx32Init() != NOERR)
            {
            if (bApisWereAnsi)
                SetFileApisToANSI ();
            return(ERR);
            }
#endif

        MEMSET (&ScanData, '\0', sizeof(VIRSCAN));

//////////////////////////////////////////////////////////////////////////////
// PAM
//////////////////////////////////////////////////////////////////////////////
#ifdef USE_PAM

        ScanData.hLPAM = NULL;

        VirusScanGetDatFile(szPAM, sizeof(szPAM) - 1);
        NameStripFile(szPAM);
        NameAppendFile(szPAM, "VIRSCAN2.DAT");

        pamStatus = PAMGlobalInit(szPAM,&ScanData.hGPAM);
        if (PAMSTATUS_OK == pamStatus)
            {
            // need to get temporary file name for swap file!

            pamStatus = PAMLocalInit(szPAM,
                                     NULL,      // no swap file!
                                     ScanData.hGPAM,
                                     &ScanData.hLPAM);

            if (PAMSTATUS_OK != pamStatus)
                {
                PAMGlobalClose(ScanData.hGPAM);
                ScanData.hGPAM = NULL;

#ifdef SYM_WIN32
                if (bApisWereAnsi)
                    SetFileApisToANSI ();
#endif
                return(ERR);
                }
            }
        else
            {
            ScanData.hGPAM = NULL;

#ifdef SYM_WIN32
            if (bApisWereAnsi)
                SetFileApisToANSI ();
#endif
            return(ERR);
            }
#endif
/////////////////////////////////////////////////////////////////////////////



        if ((ScanData.hWorkMem = MemAlloc(GHND | GMEM_SHARE, SIZE_WORK_BUFFER)) != NULL)
            {
            ScanData.lpWorkMem = (LPSTR)MemLock(ScanData.hWorkMem);

        // ************************************** //
        // Obtain the file scanning specific data //
        // ************************************** //

            if ((ScanData.hFileScanData = GetScanData(VIRSCAN_FILE, &bReCreated))
                                        != NULL)
                ScanData.lpFileScanData = (LPSTR)MemLock (ScanData.hFileScanData);
            else
                uStatus = FALSE;

            if (uStatus == TRUE)
                {
                ScanData.hMemoryScanData = GetScanData(VIRSCAN_MEMORY,
                                                        &bReCreated);

                if ( ScanData.hMemoryScanData != NULL )
                    {
                    ScanData.lpMemoryScanData =
                                    (LPSTR)MemLock (ScanData.hMemoryScanData);
                    }
                else
                    uStatus = FALSE;
                }

            if (uStatus == TRUE)
                {
                ScanData.hBootScanData = GetScanData(VIRSCAN_BOOT,
                                                     &bReCreated);

                if ( ScanData.hBootScanData != NULL )
                    {
                    ScanData.lpBootScanData =
                                    (LPSTR)MemLock (ScanData.hBootScanData);
                    }
                else
                    uStatus = FALSE;
                }

            if (TRUE == uStatus)
                {
                ScanData.hCallbacks = MemAlloc(GHND, sizeof(CALLBACKREV1));

                if (NULL != ScanData.hCallbacks)
                    {
                    ScanData.lpCallbacks = (LPSTR)MemLock(ScanData.hCallbacks);

                    uStatus = InitCertlib (
                                        (LPCALLBACKREV1)ScanData.lpCallbacks,
                                        INIT_CERTLIB_USE_DEFAULT
                                        );
                    }
                else
                    {
                    uStatus = FALSE;
                    }
                }
            }
        else
            {
            uStatus = FALSE;
            }

        if (uStatus == TRUE)
            {
                                // ******************************** //
                                // Get the full path to VIRSCAN.DAT //
                                // ******************************** //
            VirusScanGetDatFile (ScanData.szVirscanFile, SYM_MAX_PATH);
            }
        }

    if ( uStatus == TRUE )
        uUsers++;                       // Another user

    if ( uStatus == TRUE && bReCreated == TRUE)
        uStatus = NOERR_DAT_CHANGED;
    else if ( uStatus == TRUE )
        uStatus = NOERR;
    else
        uStatus = ERR;

//////////////////////////////////////////////////////////////////////////////
// PAM
//////////////////////////////////////////////////////////////////////////////
#ifdef USE_PAM

    if (uStatus == ERR)
    {
        PAMLocalClose(ScanData.hLPAM);
        ScanData.hLPAM = NULL;
        PAMGlobalClose(ScanData.hGPAM);
        ScanData.hGPAM = NULL;
    }
#endif

#ifdef SYM_WIN32
    if (bApisWereAnsi)
        SetFileApisToANSI ();
#endif

    return (uStatus);
} // End VirScanInit()



//************************************************************************
// NavExInit
//
// Initializes NavEx for DOS. Cut and revised from VirusScanInit().
// Note: Uses Buffer that held memory signatures - lpMemoryScanData 
//       Should be called only once before VirusScanEnd() is called.
//
// Parameters: None.
//
// Returns: TRUE if successful, ERR if unsuccessful
//************************************************************************
// 11/14/95 MKEATIN, Function created.
//************************************************************************

#ifdef SYM_DOS

UINT SYM_EXPORT WINAPI NavExInit (VOID)
{                                                           
    auto    UINT        uStatus = TRUE;
    auto    EXTSTATUS   extResult;
    extern  UINT        uDosOverlayBufferSize; // found in scn_dta.cpp   

    if (TRUE == gLoaderInfo.bInitialized)
        {
        return(TRUE);    // Call only once before calling VirusScanEnd()
        }
    

    gLoaderInfo.lpOverlayPtr = ScanData.lpMemoryScanData;    

    if (NULL == gLoaderInfo.lpOverlayPtr)    
        {    
        gLoaderInfo.bInitialized = FALSE;    
        uStatus = ERR;
        }    
    else    
        {    
        char szN30[SYM_MAX_PATH+1];    

        VirusScanGetDatFile(szN30, sizeof(szN30) - 1);    
        NameStripFile(szN30);    
        NameAppendFile(szN30, gpszNAVEXFileName);    


        extResult = DOSEXTLoad(szN30,    
                               gLoaderInfo.lpOverlayPtr,    
                               uDosOverlayBufferSize);    

        if (EXTSTATUS_OK != extResult)    
            {    
            gLoaderInfo.bInitialized = FALSE;    
            uStatus = ERR;
            }    
        else    
            {    
            gLoaderInfo.bInitialized = TRUE;    
            uStatus = TRUE;
            }    
        }
    return (uStatus); 
}
#endif //SYM_DOS


//************************************************************************
// NavExInit() - for the DX platform
//
// Always returns TRUE.  This function is obsolete for the DX platform.
// We define it here so we don't have to 'ifdef' the calling code (ugly).
//
// Parameters: None.
//
// Returns: TRUE 
//************************************************************************
// 11/14/95 MKEATIN, Function created.
//************************************************************************

#ifdef SYM_DOSX

UINT SYM_EXPORT WINAPI NavExInit (VOID)
{
    return(TRUE);
}

#endif // SYM_DOSX


//************************************************************************
// VirusScanSetPath
//
// Sets the full path to the Virus Definitions file, without the filename
// This function is available to all but SYM_WIN platforms.
//
// Parameters:
//          lpPath - pointer to an ASCIIZ string containing path to
//                   VIRSCAN.DAT.
//
// Returns:
//          -
//
//************************************************************************
// 03/06/95 RAD created.
//************************************************************************

#if !defined(SYM_WIN)

static char  _szVirusScanPath[SYM_MAX_PATH];
static LPSTR _lpszVirusScanPath = NULL;

VOID SYM_EXPORT WINAPI VirusScanSetPath (
    LPSTR lpszFileName
    )
    {
    if ( STRLEN ( lpszFileName ) < sizeof ( _szVirusScanPath ) )
        {
        STRNCPY ( _szVirusScanPath, lpszFileName, sizeof ( _szVirusScanPath ) );
        _lpszVirusScanPath = _szVirusScanPath;
        }
    }
#endif

//************************************************************************
// VirScanGetPath
//
//  Returns the full path of the Virus Definitions file, without the filename
//  Note that it is dependent on where the VIRSCAN.DLL is located.
//
//  ANSI/OEM mess: This function seems to be called with file api's in
//  both ANSI and OEM modes. The string returned must be in the corresponding
//  character set.
//  GetModuleFileName() only returns LFN's. Depending on the mode of the
//  file api's and whether the filename has non-translatable characters,
//  we may get an unusable path.
//  This is kluged to work for ANSI names that don't map to OEM.
//  (It's possible to fix the other case as well, but I'm assuming a normal
//  install -running in ANSI mode- was used to select the program directory)
//
// Parameters: none
//
// Returns:
//
//************************************************************************
// 04/04/93 BRAD created.
// 05/20/96 DALLEE, kluge to return valid OEM path if a valid ANSI path
//      exists.
//************************************************************************

VOID SYM_EXPORT WINAPI VirusScanGetPath(LPSTR lpFileName, UINT uSize)
{
#ifdef SYM_WIN
    auto    char    szShortFileName [SYM_MAX_PATH + 1];

    GetModuleFileName (hInstance, lpFileName, uSize);

                                        // If we get a bogus OEM name...
    if (FALSE == FileExists(lpFileName))
        {
        if (FALSE == AreFileApisANSI())
            {
                                        // Set api's to ANSI and try to
                                        // get a valid SFN. This should
                                        // contain only ANSI-OEM mappable
                                        // characters.
            SetFileApisToANSI();
            GetModuleFileName(hInstance, lpFileName, uSize);

            NameReturnShortName(lpFileName, szShortFileName);
            if ( (STRLEN(szShortFileName) < uSize) &&
                 FileExists(szShortFileName) )
                {
                NAnsiToOem(szShortFileName, lpFileName);
                }
            SetFileApisToOEM();
            }
        }

    NameStripFile(lpFileName);
#else
    if ( _lpszVirusScanPath )
        {
        STRNCPY ( lpFileName, _lpszVirusScanPath, uSize );
        }
    else
        {
        *lpFileName = EOS;
        }
#endif
}

//************************************************************************
// VirScanGetDatFile
//
//  Returns the full path of the Virus Definitions file.
//  Note that it is dependent on where the VIRSCAN.DLL
//
// Parameters: none
//
// Returns:
//
//************************************************************************
// 04/04/93 BRAD created.
//************************************************************************

VOID SYM_EXPORT WINAPI VirusScanGetDatFile(LPSTR lpFileName, UINT uSize)
{
    VirusScanGetPath(lpFileName, uSize);
    NameAppendFile(lpFileName, "VIRSCAN.DAT");
}


//************************************************************************
// VirScanGetInfFile
//
//  Returns the full path of the Virus Information file.
//  Note that it is dependent on where the VIRSCAN.DLL
//
// Parameters: none
//
// Returns:
//
//************************************************************************
// 04/04/93 BRAD created.
//************************************************************************

VOID SYM_EXPORT WINAPI VirusScanGetInfFile(LPSTR lpFileName, UINT uSize)
{
    VirusScanGetPath(lpFileName, uSize);
    NameAppendFile(lpFileName, "VIRSCAN.INF");
}


//************************************************************************
// VirusScanGetBootFile
//
// Returns the full path of the Generic Boot Repair file.
// Note that is is dependent on the location of VIRSCAN.DLL
//
// Parameters:
//      LPSTR   lpFileName              Buffer to receive the filename.
//      UINT    uSize                   Maximum length of the filename.
//************************************************************************
// 05/17/93 DALLEE, Function created.
//************************************************************************

VOID SYM_EXPORT WINAPI VirusScanGetBootFile(LPSTR lpFileName, UINT uSize)
{
    VirusScanGetPath(lpFileName, uSize);
    NameAppendFile(lpFileName, "BOOT.DAT");
} // End VirusScanGetBootFile()


// ******************************************************************** //
// This old function was retrieved from the old virus list dll that
// doesn't exist anymore. All this will be obsolete once we go with the
// new virus scan engine.
// ******************************************************************** //

BOOL SYM_EXPORT WINAPI VirusScanDatFileGetComment (
    LPSTR   lpFile,                     // [in] .INF file to pull out from
    UINT    uVirusID,                   // [in] virus ID to query
    LPSTR   lpAlias,                    // [out] alias info placed here
    LPSTR   lpComment )                 // [out] comment
{
   auto     DWORD    dwOffset;
   auto     LPSTR    lpMem;
   auto     BOOL     bStatus = TRUE;
   auto     UINT     hFile;
   auto     INF_HEADER  inf;


   // ************************************************************** //
   // Now try to open the complemetary file where we expect it to be //
   // ************************************************************** //

   if ((hFile = FileOpen(lpFile, READ_ONLY_FILE)) != HFILE_ERROR)
      {
      // ******************************************************** //
      // Allocate a chunk of memory big enough to store a comment //
      // and an alias string.                                     //
      // ******************************************************** //

      lpMem = (LPSTR)MemAllocPtr (GHND, SIZE_ALIAS + SIZE_COMMENT + 10);

      LPSTR lpFreeThis = lpMem;

      if ( lpMem )
         {
         FileSeek (hFile, SIZE_COPYRIGHT, SEEK_SET);
         FileRead (hFile,&inf, sizeof(INF_HEADER));

         if (inf._i_version != 0)
             {
             bStatus = FALSE;
             goto freeMem;              // Need to free memory/close file
             }

         if (uVirusID > inf._i_highest_vid)
             uVirusID = 0;


         FileSeek (hFile, uVirusID * inf._i_size, SEEK_CUR);
         FileRead (hFile, lpMem, inf._i_size);
         if (4 == inf._i_size)
             {
             dwOffset = *(DWORD FAR *)lpMem;
             }
         else
             {
             dwOffset = *(UINT FAR *)lpMem;
             }
         dwOffset = (dwOffset * inf._i_multiplier) + inf._i_no_info;

         FileSeek (hFile, dwOffset, SEEK_SET);

         FileRead (hFile, lpMem, SIZE_ALIAS + SIZE_COMMENT + 2);

         // **************************************************** //
         // First comes the alias field. If its length is longer //
         // than the maximum possible alias field then dump it.  //
         // **************************************************** //
         if (STRLEN(lpMem) <= SIZE_ALIAS)
            {
           #ifdef SYM_WIN
            OemToAnsi (lpMem, lpAlias);
           #else
            STRCPY(lpAlias, lpMem);
           #endif

            lpMem += (STRLEN(lpMem) + 1);

            // *********************************** //
            // The same goes for the comment field //
            // *********************************** //
            if (STRLEN(lpMem) <= SIZE_COMMENT)
                {
               #ifdef SYM_WIN
                OemToAnsi (lpMem, lpComment);
               #else
                STRCPY(lpAlias, lpMem);
               #endif
                }
            else
                {
                lpComment[0] = EOS;
                bStatus = FALSE;
                }
            }
         else
            {
            lpAlias[0] = EOS;
            lpComment[0] = EOS;
            bStatus = FALSE;
            }
freeMem:
         MemFreePtr (lpFreeThis);
         }

      FileClose (hFile);
      }
   else
      bStatus = FALSE;

   return (bStatus);
}

//------------------------------------------------------------------------
// WINDOWS ROUTINES
//------------------------------------------------------------------------

#if defined(SYM_WIN32)

/****************************************************************************
   FUNCTION: DllMain

   PURPOSE:  Called when a new process is either using or not using
             this DLL anymore.

*******************************************************************************/
BOOL WINAPI DllMain(HINSTANCE hInstDLL, DWORD dwReason, LPVOID lpvReserved)
{
    hInstance = hInstDLL;
    return(TRUE);
}

#elif defined(SYM_WIN)

//************************************************************************
// LibMain()
//
// Parameters:
//
// Returns:
//
//************************************************************************
// 3/\93 Martin created.
//************************************************************************

int WINAPI LibMain (HINSTANCE hInst, WORD wDataSeg, WORD wHeapSize,
                    LPSTR lpszCmdLine)
{
    if (wHeapSize != 0)
        UnlockData(0);

    hInstance = hInst;

    return (1);
}

//************************************************************************
// WEP()
//
// Parameters:
//
// Returns:
//
//************************************************************************
// 3/\93 Martin created.
//************************************************************************

int SYM_EXPORT WINAPI WEP (int bSystemExit)
{
    return(1);
}

#endif

#ifdef SYM_WIN32

// Statics for loading S32STAT and functions

STATIC int          nLoadedCount = 0;
STATIC HINSTANCE    hNavEx32 = NULL;
STATIC CHAR         szNavEx32 [] = "NAVEX32.DLL";



/*@API:**********************************************************************
@Declaration:
STATUS  WINAPI NavEx32Init (VOID)

@Description:
This routine dynamically loads NAVEX32.DLL and sets up NAVEX's internal
function pointers.

NavEx32End() should be called to free the library, but only if this call
succeeds.

@Parameters:
None.

@Returns:
NOERR if successful.
ERR_NAVEX_DLL_NOT_FOUND      if S32STAT couldn't be loaded.
ERR_NAVEX_FUNCTION_NOT_FOUND if a required function wasn't found.

@See:
@Include:   navex.h
@Compatibility: W32
****************************************************************************/
// 10/14/95 DALLEE, created.
// 04/18/96 MKEATIN, Modified from DALLE's NavStatInit ()
//**************************************************************************

STATUS SYM_EXPORT WINAPI NavEx32Init (VOID)
{
                                        // Check if first time.
                                        //&? Should create a mutex for
                                        // accessing nLoadedCount.
    if (0 == nLoadedCount)
        {
                                        // Load NAVEX32.DLL.
        hNavEx32 = (HINSTANCE)LoadLibrary(szNavEx32);
        if ((HINSTANCE)HINSTANCE_ERROR > hNavEx32)
            {
            hNavEx32 = NULL;
            return (ERR_NAVEX_DLL_NOT_FOUND);
            }


        // Get function pointers.
            
        export_table.EXTScanFile   = 
            (unsigned int(__stdcall *)(CALLBACKREV1*,char*,int,unsigned char*,unsigned char*,unsigned short,unsigned short*))
            GetProcAddress(hNavEx32, "EXTScanFile");
        export_table.EXTScanBoot   = 
            (unsigned int(__stdcall *)(CALLBACKREV1*,unsigned char,unsigned char,unsigned short,unsigned short,unsigned short,unsigned char*,unsigned char*,unsigned short,unsigned short*))
            GetProcAddress(hNavEx32, "EXTScanBoot");
        export_table.EXTScanPart   = 
            (unsigned int(__stdcall *)(CALLBACKREV1*,unsigned char,unsigned char*,unsigned char*,unsigned short,unsigned short*))
            GetProcAddress(hNavEx32, "EXTScanPart");
        export_table.EXTScanMemory = 
            (unsigned int(__stdcall *)(CALLBACKREV1*,unsigned short,int,unsigned short*))
            GetProcAddress(hNavEx32, "EXTScanMemory");
        export_table.EXTRepairFile = 
            (unsigned int(__stdcall *)(CALLBACKREV1*,unsigned short,N30*,char*,unsigned char*))
            GetProcAddress(hNavEx32, "EXTRepairFile");
        export_table.EXTRepairBoot = 
            (unsigned int(__stdcall *)(CALLBACKREV1*,unsigned short,N30*,unsigned char,unsigned char,unsigned short,unsigned short,unsigned short,unsigned char*))
            GetProcAddress(hNavEx32, "EXTRepairBoot");
        export_table.EXTRepairPart = 
            (unsigned int(__stdcall *)(CALLBACKREV1*,unsigned short,N30*,unsigned char,unsigned char*))
            GetProcAddress(hNavEx32, "EXTRepairPart");

        if ( (NULL == export_table.EXTScanFile)   ||
             (NULL == export_table.EXTScanBoot)   ||
             (NULL == export_table.EXTScanPart)   ||
             (NULL == export_table.EXTScanMemory) ||
             (NULL == export_table.EXTRepairFile) ||
             (NULL == export_table.EXTRepairBoot) ||
             (NULL == export_table.EXTRepairPart) )
            {
            //&? Would call FreeProcInstance() first in W16.
            export_table.EXTScanFile            = NULL;
            export_table.EXTScanBoot            = NULL;
            export_table.EXTScanPart            = NULL;
            export_table.EXTScanMemory          = NULL;
            export_table.EXTRepairFile          = NULL;
            export_table.EXTRepairBoot          = NULL;
            export_table.EXTRepairPart          = NULL;
            FreeLibrary(hNavEx32);
            hNavEx32 = NULL;

            return (ERR_NAVEX_FUNCTION_NOT_FOUND);
            }
        }

    nLoadedCount++;

    return (NOERR);
} // NavEx32Init()


/*@API:**********************************************************************
@Declaration:
STATUS  WINAPI NavEx32End (VOID)

@Description:
This routine unloads NAVEX32.DLL and zero's NAVEX's internal
function pointers.

@Parameters:
None.

@Returns:
NOERR                       if successful or NavStat never initialized.
ERR_NAVSTAT_DLL_NOT_FREED   if FreeLibrary() failed.

@See:
@Include:   navex.h
@Compatibility: W32
****************************************************************************/
// 10/14/95 DALLEE, created.
//**************************************************************************

STATUS SYM_EXPORT WINAPI NavEx32End (VOID)
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

            export_table.EXTScanFile            = NULL;
            export_table.EXTScanBoot            = NULL;
            export_table.EXTScanPart            = NULL;
            export_table.EXTScanMemory          = NULL;
            export_table.EXTRepairFile          = NULL;
            export_table.EXTRepairBoot          = NULL;
            export_table.EXTRepairPart          = NULL;

            if (FALSE == FreeLibrary(hNavEx32))
                {
                //&? DALLEE - I don't think this'll ever fail, but if it does...
                // hSymStat is set = NULL regardless, and a new handle
                // retrieved if NavExInit() is called again.  This is
                // on the assumption that the FreeLibrary() failed based
                // on a bad handle(?).
                nReturn = ERR_NAVSTAT_DLL_NOT_FREED;
                }
            hNavEx32 = NULL;
            }
        }

    return (nReturn);
} // NavStatEnd()

#endif // SYM_WIN32
