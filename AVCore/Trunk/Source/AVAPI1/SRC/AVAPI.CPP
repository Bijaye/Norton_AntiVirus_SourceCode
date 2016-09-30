// Copyright 1994 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/AVAPI1/VCS/avapi.cpv   1.184   09 Nov 1998 14:33:20   MKEATIN  $
//
// Description:
//
// Contains:
//
// See Also:
//************************************************************************
// $Log:   S:/AVAPI1/VCS/avapi.cpv  $
//
//    Rev 1.184   09 Nov 1998 14:33:20   MKEATIN
// Added a fix to VLRepairFile's recursive (linked-list) repair logic - as
// pointed out by Carey.
//
//    Rev 1.183   28 Aug 1998 14:22:26   JBRENNA
// Port QAKN changes to trunk:
// 1. Rev 1.182.1.3   22 Jul 1998 16:15:22   DBuches
//    Exported VLLoadN30.
// 2. Rev 1.182.1.2   21 Jul 1998 11:43:02   CNACHEN
//    One last change for infestation mode.
// 3. Rev 1.182.1.1   15 Jul 1998 12:06:26   CNACHEN
//    Added infestation mode support to VLScanFile function
//
//    Rev 1.182   13 May 1998 10:17:56   DDREW
// Nulled hEngineLib and hExportLib handles for NLM platform only in VLVirusInit
//
//    Rev 1.181   04 Mar 1998 17:55:08   DBuches
// Added VLDetermineVirusIndex.
//
//    Rev 1.180   19 Feb 1998 16:28:40   RStanev
// Added a placeholder for VLQuarantineFile().
//
//    Rev 1.179   09 Sep 1997 12:48:42   MKEATIN
// VLRegisterVirusDB() now has one more parameter: lpszNavexInfFile.  This
// is the full path to NAVEX15.INF.
//
//    Rev 1.178   09 Sep 1997 12:37:12   MKEATIN
// Added the szNavexInfFile member to HVCONTEXT.
//
//    Rev 1.177   05 Sep 1997 13:33:50   DDREW
// Removed the code Milind #ifdef'd as it was forgotten test code... sorry!
//
//    Rev 1.176   05 Sep 1997 11:55:10   MLELE
// Modified function VLScanFile. Put #if defined(SYM_NLM) around
// NLM_FileGetAttr, NLM_FileSetAttr and related variables. cf: function.
//
//    Rev 1.175   03 Sep 1997 11:48:50   DDREW
// More fixes for repair
//
//    Rev 1.174   26 Aug 1997 21:53:50   DDREW
// Missed the same one again
//
//    Rev 1.173   26 Aug 1997 21:48:40   DDREW
//
//    Rev 1.172   26 Aug 1997 20:24:50   DDREW
// Missed one
//
//    Rev 1.171   26 Aug 1997 20:17:42   DDREW
// Changed attribute calls around some more
//
//    Rev 1.170   26 Aug 1997 18:42:42   DDREW
// Modified all instances of FileGetAttrib/FileSetAttrib to VLFile(...) versions
//
//    Rev 1.169   26 Aug 1997 15:47:58   DDREW
// In VLCanRepairItem - we errantly reference the wInfo member instead of
// wControl.
//
//    Rev 1.168   22 Aug 1997 14:59:32   DDREW
// Exposed a ton of routines that were preveously #ifndef SYM_NLM'd out for repair
//
//    Rev 1.167   12 Aug 1997 16:40:46   DBUCHES
// Made modifications to VLScanFile().  We will now preserve the
// Last Accessed Date for files scanned on Win32 platforms.
//
//    Rev 1.166   07 Aug 1997 09:33:04   DDREW
// More changes for NLM - only - so don't worry!
//
//    Rev 1.165   31 Jul 1997 16:52:04   MKEATIN
// We now pass two addtional parameters to VLGetBootSectorLocation()
//
//    Rev 1.164   29 Jul 1997 13:24:04   MKEATIN
// If we are passed the boot sector in a buffer in VLScanBoot, set
// dwImageSize to SECTOR_SIZE.
//
//    Rev 1.163   22 Jul 1997 16:07:00   DDREW
// 1. Moved the code around a lot... Put all of the #ifndef SYM_NLM in one
//     area because code (for NLM) was too hard to follow. Put All #ifndef
//     SYM_NLM together and put all #if 0 (dead) code at the end of the file.
// 2. Put in some temporary ifdef's to isolate some routines for NLM platform
//     that will not work until we do some enhancments.
// 3. Said multi-religion prayer that I did not screw up another platform...
//
//    Rev 1.162   17 Jul 1997 18:06:50   DDREW
// Moved some "ifndef SYM_NLM's" around
//
//    Rev 1.161   16 Jul 1997 14:34:50   MKEATIN
// Removed a call to VLEngLocalClose() in VLRepairFile().
//
//    Rev 1.160   23 Jun 1997 12:04:38   MKEATIN
// Changed several calls from FileKill() to VLFileDelete().
//
//    Rev 1.159   18 Jun 1997 16:57:52   MKEATIN
// Removed the static szNAVPath and VLGetDefsLocation().
//
//    Rev 1.158   17 Jun 1997 14:45:10   MKEATIN
// Modified the API to take the ENGINE callbacks as a parameter in
// VLScanInit().
//
//    Rev 1.157   10 Jun 1997 20:55:10   MKEATIN
// Added virscan.inf functionality (comments and aliases for VIRLIST).
//
//    Rev 1.156   09 Jun 1997 03:59:10   BGERHAR
// Fix hard coded VIRSCAN2.DAT for wild sets
//
//    Rev 1.155   05 Jun 1997 18:24:38   MKEATIN
// Remember to call EngGlobalMemoryClose() and EngGlobalBootClose() in
// VLScanClose().
//
//    Rev 1.154   04 Jun 1997 13:23:56   MKEATIN
// Used _ConvertLongToString instead of ConvertLongToString in VLGetDBInfo().
//
//    Rev 1.153   04 Jun 1997 13:07:40   MKEATIN
// Implemented VLNumClaimedVirusEntries().
//
//    Rev 1.152   03 Jun 1997 18:11:24   MKEATIN
// Now we call the new EngGetNumClaimedViruses() in VLGetDBInfo(). This is
// currently called from Virlist.
//
//    Rev 1.151   03 Jun 1997 15:45:40   MKEATIN
// Fixed VLGetDBInfo to work with the new AVENGE header.  Also, fixed a bug
// in the VLFillListTable() loop.
//
//    Rev 1.150   31 May 1997 16:33:20   MKEATIN
// Added lpvCookie to the repair functions
//
//    Rev 1.149   27 May 1997 18:59:38   MKEATIN
// Added the NTK platform to NAVEX 1.5 technology
//
//    Rev 1.148   27 May 1997 17:21:04   RStanev
// Fixed VLMoveFile().  Replaced VLInocMoveFile() with VLMoveFileEx().
//
//    Rev 1.147   21 May 1997 16:06:08   MKEATIN
// Now calling EngCloseBulkLoad() in VLFillListTable().
//
//    Rev 1.146   20 May 1997 19:06:44   MKEATIN
// Fixed VLFillListTable() to work with AVENGE.  Also, enabled VLDefInfo().
//
//    Rev 1.145   20 May 1997 12:42:40   MKEATIN
// EngGlobalBootInit() now is passed BOOT_INIT_NO_DISK_ACCESS if
// the VLINIT_CREATE_BOOT_INFO_CACHE flag is set.  Also, removed
// VLCacheVirusInfo().
//
//    Rev 1.144   14 May 1997 11:40:24   RStanev
// VLScanInit() now initializes hContext->lpstCallback for VxD and NTK.
//
//    Rev 1.143   13 May 1997 18:32:10   MKEATIN
// Now passing wFlags to EngGlobalBootInit()
//
//    Rev 1.142   13 May 1997 16:34:22   TIVANOV
// fixed a "C" comment error in the log header
//
//    Rev 1.141   13 May 1997 13:47:18   TIVANOV
// new move file function to support the ImocMoveFIle
//
//    Rev 1.139   09 May 1997 17:40:30   MKEATIN
// More temp build fixes
//
//    Rev 1.138   09 May 1997 17:21:18   MKEATIN
// Made temporary Fixes for AP build issues.
//
//    Rev 1.137   09 May 1997 16:57:48   MKEATIN
// Added a missing ')'
//
//    Rev 1.136   08 May 1997 15:56:02   MKEATIN
// Removed a reference to lpgCallBack in VLMoveFile()
//
//    Rev 1.135   08 May 1997 15:48:08   MKEATIN
// Remove the dreaded lpgCallBack and upgraded to CALLBACKREV2 and NAVEX 1.5
// support
//
//************************************************************************

#include "platform.h"
#if !defined (UNICODE)
#include "dbcs.h"
#endif
#include "file.h"
#include "xapi.h"
#include "disk.h"
#include "repair.h"
#include "avapi.h"
#include "avcb.h"
#include "avenge.h"
#include "navcb.h"
#include "virusinf.h"

#ifndef _T                              // BEM - This is needed when this is compiled from
                                        // BEM - the context of NAVHPC in env of VIRLIB
    #ifdef SYM_WIN16
        #define __T(x) x
        #define _T(x) __T(x)
    #else
        #include <tchar.h>
    #endif
#endif

#include "navex15.h"
#include "pamapi.h"

//
// This stuff is needed for run-time linking to functionality
// only available on WinNT systems.
//
#if defined(SYM_WIN32) && !defined(SYM_VXD) && !defined(SYM_NTK)

typedef BOOL (WINAPI *LPFNGETFILEATTRIBUTESEX)( LPCTSTR, GET_FILEEX_INFO_LEVELS, LPVOID );
#define GETFILEATTRIBUTESEXW    _T("GetFileAttributesExW")
#define GETFILEATTRIBUTESEXA    _T("GetFileAttributesExA")

BOOL LOCAL VLGetLastAccessDateNT( LPCTSTR szFileName, FILETIME *lpFileTime );
#endif

//////////////////////////////////////////////////////////////////////////////

// --------------------------------------------------------------------------
//                      LOCAL FUNCTION PROTOTYPES
// --------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
// API support routines (not accessable to user)
///////////////////////////////////////////////////////////////////////////////


BOOL LOCAL VLLoadCachedN30 (
    HVCONTEXT hContext,
    UINT      uIndex,
    UINT      uN30ListEntries,
    LPN30E    lpN30E,
    LPN30     lpN30 );

VOID LOCAL VLSetHVLVIRUSBoot (
    HVLVIRUS      hVirus,
    HVCONTEXT   lpVContext,
    UINT        uDBEntryNum,
    DWORD       dwFlags,
    BYTE        byDriveNum,
    BYTE        byBootSecNum,
    BYTE        byBootFlags,
    BYTE        bySystemType );

VOID LOCAL VLSetHVLVIRUSFile (
    HVLVIRUS      hVirus,
    HVCONTEXT   lpVContext,
    UINT        uDBEntryNum,
    DWORD       dwFlags,
    LPTSTR      lpszFileName );

BOOL LOCAL VLGetVirusInfo (
    HVCONTEXT   hContext,
    WORD        wVirusID,
    LPTSTR      lpAlias,
    LPTSTR      lpComment );

BOOL LOCAL VLGetVirusText (
    HVCONTEXT   hContext,
    UINT        uDBEntryNum,
    LPTSTR      lpszVirusText );

BOOL LOCAL VLGetVirusAlias (
    HVCONTEXT   hContext,
    UINT        uDBEntryNum,
    LPTSTR      lpszVirusAlias );

BOOL LOCAL VLEngLocalInit (
    HVCONTEXT hContext,
    LPHLENG   lphLEng );

VOID LOCAL VLEngLocalClose (
    HLENG     hLEng );

WORD LOCAL VLGetInfectionDepth (
   HVLVIRUS hVirus);

BOOL WINAPI VLIsFileUpToDate(
    LPTSTR Path,
    LPTSTR FileName,
    FILETIME FileTime);

BOOL WINAPI VLVerifyDataFile(
    LPTSTR Path,
    LPTSTR FileName );

BOOL WINAPI BackupInfectedFile (
    LPTSTR lpszInfectedFile,
    LPTSTR lpszBackupExtension,
    LPTSTR lpszBackupFile);

BOOL WINAPI BuildUniqueFileName (
    LPCTSTR lpszBaseFile,
    LPCTSTR lpszExtension,
    LPTSTR  lpszUniqueFile );

/*@API:*************************************************************************
@Declaration:  HVCONTEXT  WINAPI VLScanInit(UINT uRevision,
LPCALLBACKREV1 lpCallBack, WORD wFlags)

@Description:
This function can be used to initialize the Virus Library API.  The user of
the API must fill in a structure of type CALLBACKREV1 and pass in a 1 as the
revision number.  The callback functions should take the same arguments and
return values as the QUAKE functions of the same name...

@Parameters:
$uRevision$ Should always be 1; it identifies which structure version we are
registering.

$lpCallBack$ is a far pointer to a structure which has been initialized.
If lpCallBack is NULL - the local gstNAVEXCallBacks structer is used.
Otherwise, lpCallback is assumed to be an initialized CALLBACKREV2 and is
used.

$wFlags$ is a word which contains configuration flags:

    VLINIT_HANDLE_NO_ACCESS:
       return a handle if an item is inaccessable.

@Returns:
This function returns a handle to a virus context (HVCONTEXT) structure on
success or NULL on failure.  The only reason it would return NULL is if
the registered memory allocation function was unable to allocate memory for
the context structure.

@Include: avapi.h
*******************************************************************************/

HVCONTEXT SYM_EXPORT WINAPI VLScanInit
(
    UINT                uRevision,      // [in] callback revision number
    LPAVGLOBALCALLBACKS lpCallBacks,    // [in] callback structure (may be NULL)
    WORD                wFlags          // [in] flags to specify options
)
{
    auto    HVCONTEXT    hContext;

    SYM_ASSERT(lpCallBacks);

    if (uRevision != VL_REVISION)
        return(NULL);

    hContext = (HVCONTEXT)lpCallBacks->
                          lpNAVEXCallBacks->
                          PermMemoryAlloc(sVCONTEXTTYPE);

    if (hContext == NULL)               // Error allocating context structure
    {
        return(NULL);
    }

    hContext->lpGlobalCallBacks = lpCallBacks;
    hContext->lpstCallBack      = lpCallBacks->lpNAVEXCallBacks;

    SYM_ASSERT(VLFileOpen);
    SYM_ASSERT( VLFileClose);
    SYM_ASSERT(VLFileRead);
    SYM_ASSERT(VLFileSeek);
    SYM_ASSERT(VLFileSize);

    ////////////////////////////////////////////////////////////////////////
    // Now initialize the szVirScanDataFile/szVirScanInfoFile filenames
    ////////////////////////////////////////////////////////////////////////

    hContext->szVirScanDataFile[0] = 0;
    hContext->szVirScanInfoFile[0] = 0;

#if defined(SYM_VXD)
    hContext->szAnsiVirScanDataFile[0] = 0;
    hContext->szAnsiVirScanInfoFile[0] = 0;
    *(PWCHAR)hContext->szUniVirScanDataFile = 0;
    *(PWCHAR)hContext->szUniVirScanInfoFile = 0;
#endif

    ////////////////////////////////////////////////////////////////////////
    // Now initialize the buffer pointers
    ////////////////////////////////////////////////////////////////////////

    hContext->hGEng                     = NULL;
    hContext->hGPAM                     = NULL;

#if !defined(AVAPI_MT)

    hContext->hLEng                     = NULL;
    hContext->hLPAM                     = NULL;

#endif

    hContext->prExportTable             = NULL;
    hContext->prEngineTable             = NULL;

#if defined(SYM_WIN32) || defined(SYM_NLM)

    hContext->hExportLib                = NULL;
    hContext->hEngineLib                = NULL;

#elif defined(SYM_DOSX)

    hContext->lpstNavexMemoryHandle     = NULL;
    hContext->lpstEngineMemoryHandle    = NULL;

#endif

#if defined(SYM_NLM)

    hContext->lpFastCheckDB             = NULL;

#endif

    hContext->uN30EFCount               = 0;
    hContext->uN30EBCount               = 0;

    hContext->lprN30EFBuffer            = NULL;
    hContext->lprN30EBBuffer            = NULL;

    //////////////////////////////////////////////////////////////////////////
    // Set up a signature for verification
    //////////////////////////////////////////////////////////////////////////

    hContext->dwSignature    = 0xDEADFECEL;

    hContext->wFlags         = wFlags;

    return(hContext);
}


/*@API:*************************************************************************
@Declaration:  VSTATUS WINAPI VLScanClose(HVCONTEXT hContext)

@Description:
This closes up a virus scanning session.  All HVLVIRUS and HVLVIRUSENTRY structures
must be released before closing the Anti Virus API.

@Parameters:
$hContext$ This is a valid handle to a context provided by VLScanInit.

@Returns:
This function returns a result of type VSTATUS (type definition found in
AVAPI.H).

    VS_OK           will be returned on success
    VS_INVALID_ARG  will be returned if the context is invalid

@Include: avapi.h
*******************************************************************************/

VSTATUS SYM_EXPORT WINAPI VLScanClose
(
    HVCONTEXT hContext                  // [in] a valid HVCONTEXT handle
)
{
    //////////////////////////////////////////////////////////////////////////
    // Verify the structure pointer and its signature
    //////////////////////////////////////////////////////////////////////////

    if (!VLValidContext(hContext))
        return(VS_INVALID_ARG);

    //////////////////////////////////////////////////////////////////////////
    // Close NavEx
    //////////////////////////////////////////////////////////////////////////

#if defined(SYM_NLM)
    if (EXTCloseNLM(hContext) != EXTSTATUS_OK)
    {
        SYM_ASSERT (FALSE);
    }
#elif defined(SYM_VXD)
    if (EXTCloseVxD(hContext) != EXTSTATUS_OK)
    {
        SYM_ASSERT (FALSE);
    }
#elif defined(SYM_NTK)
    if (EXTCloseNTK(hContext) != EXTSTATUS_OK)
    {
        SYM_ASSERT (FALSE);
    }
#elif defined(SYM_WIN32)
    if (EXTCloseWin(hContext) != EXTSTATUS_OK)
    {
        SYM_ASSERT (FALSE);
    }
#elif defined(SYM_DOSX)
    if (EXTCloseDX (hContext) != EXTSTATUS_OK)
    {
        SYM_ASSERT (FALSE);
    }
#endif

    //////////////////////////////////////////////////////////////////////////
    // Free temporary data structures
    //////////////////////////////////////////////////////////////////////////

#if defined(SYM_NLM)

    if (hContext->lpFastCheckDB != NULL)
    {
        VLPermMemoryFree( hContext->lpFastCheckDB );
    }

#endif

    //////////////////////////////////////////////////////////////////////////
    // Close Avenge Engine
    //////////////////////////////////////////////////////////////////////////

#if !defined(AVAPI_MT)
                                        // Only single threaded apps leave
    if (hContext->hLPAM != NULL)        // Local Handles open between scans,
        PAMLocalClose(hContext->hLPAM); // so only these apps need to close
                                        // them here.
    if (hContext->hLEng != NULL)
        EngLocalClose(hContext->hLEng);

#endif

    if (hContext->hGPAM != NULL)
        PAMGlobalClose(hContext->hGPAM);

    if (hContext->hGEng != NULL)
        EngGlobalFileClose(hContext->hGEng);

#ifndef SYM_NLM
    if (hContext->hGEng != NULL)
        EngGlobalBootClose(hContext->hGEng);
#endif

    if (hContext->hGEng != NULL)
        EngGlobalMemoryClose(hContext->hGEng);

    if (hContext->hGEng != NULL)
        EngGlobalClose(hContext->hGEng);

    //////////////////////////////////////////////////////////////////////////
    // Free Windows Libraries
    //////////////////////////////////////////////////////////////////////////

#if defined(SYM_WIN32)

    if (hContext->hExportLib != NULL)
        FreeLibrary(hContext->hExportLib);

    if (hContext->hEngineLib != NULL)
        FreeLibrary(hContext->hEngineLib);

#endif

    //////////////////////////////////////////////////////////////////////////
    // Free hContext
    //////////////////////////////////////////////////////////////////////////

    VLPermMemoryFree(hContext);

    return(VS_OK);
}



/*@API:*************************************************************************
@Declaration:  VSTATUS WINAPI VLRegisterVirusDB(HVCONTEXT hContext,
LPTSTR lpszDBFile, LPTSTR lpszInfoFile)

@Description:
This routine is used to "register" a virus database (VIRSCAN.DAT currently),
and its accompanying virus information database (VIRSCAN.INF currently).  This
must be done after VLScanInit and before any scanning functions are called.

@Parameters:
$hContext$ This is a valid handle to a context provided by VLScanInit.
$lpszDBFile$ This is the filename of the virus DB file (OEM for VxD!)
$lpszInfoFile$ This is the filename of the virus INF file (OEM for VxD!)
$lpszNavexInfFile$ This is the fullpath to NAVEX.INF - may be NULL)

@Returns:
This function returns a result of type VSTATUS (type definition found in
AVAPI.H).

    VS_OK           will be returned on success
    VS_INVALID_ARG  will be returned if the context is invalid
    VS_FILE_ERROR   will be returned if a specified data file can't be accessed
    VS_MEMALLOC     will be returned if memory can't be allocated

@Include: avapi.h
*******************************************************************************/

#ifdef SYM_DOS                          // MAJOR Kludge to support wild set for DOS
    extern "C" BOOL PASCAL AreDefsWild(VOID);
#endif

VSTATUS SYM_EXPORT WINAPI VLRegisterVirusDB
(
    HVCONTEXT hContext,                 // [in] a valid HVCONTEXT handle
    LPTSTR    lpszDBFile,               // [in] path & filename of VIRSCAN1.DAT
    LPTSTR    lpszInfoFile,             // [in] path & filename of VIRSCAN.INF
    LPTSTR    lpszNavexInfFile          // [in] path & filename of NAVEX15.INF
)
{
    TCHAR        szPAMFile[SYM_MAX_PATH +1];

    //////////////////////////////////////////////////////////////////////////
    // Verify the structure pointer and its signature
    //////////////////////////////////////////////////////////////////////////

    if (!VLValidContext(hContext))
        return(VS_INVALID_ARG);


    //////////////////////////////////////////////////////////////////////////
    // Copy our antivirus database name into our structure
    //////////////////////////////////////////////////////////////////////////

    STRCPY(hContext->szVirScanDataFile, lpszDBFile);
    STRCPY(hContext->szVirScanInfoFile, lpszInfoFile);

    if (lpszNavexInfFile)
        STRCPY(hContext->szNavexInfFile, lpszNavexInfFile);
    else
        STRCPY(hContext->szNavexInfFile, _T(""));

#if defined(SYM_VXD)
    OemToCharW(lpszDBFile, (PWSTR)hContext->szUniVirScanDataFile);
    OemToCharA(lpszDBFile, hContext->szAnsiVirScanDataFile);
    OemToCharW(lpszInfoFile, (PWSTR)hContext->szUniVirScanInfoFile);
    OemToCharA(lpszInfoFile, hContext->szAnsiVirScanInfoFile);
#endif

    STRCPY (szPAMFile, hContext->szVirScanDataFile );
    NameStripFile(szPAMFile);

#ifdef SYM_DOS
    if (AreDefsWild())
        NameAppendFile(szPAMFile, _T("VIRWILD2.DAT"));
    else                                // Belongs to clause outside of endif
#endif
        NameAppendFile (szPAMFile, _T("VIRSCAN2.DAT") );

    STRCPY(hContext->szPAMScanDataFile, szPAMFile);

#ifdef SYM_VXD
    OemToCharW ( szPAMFile, (PWSTR)hContext->szUniPAMScanDataFile );
    OemToCharA ( szPAMFile, hContext->szAnsiPAMScanDataFile );
#endif // SYM_VXD

    if (EngGlobalInit(&hContext->hGEng,
                      hContext->lpGlobalCallBacks->lpGeneralCallBacks,
                      hContext->lpGlobalCallBacks->lpProgressCallBacks,
                      hContext->lpGlobalCallBacks->lpDataFileCallBacks,
                      hContext->lpGlobalCallBacks->lpIPCCallBacks,
                      0xDEADBEEFUL) == ENGSTATUS_ERROR)
    {
        return(VS_FILE_ERROR);
    }

    if (EngGlobalMemoryInit(hContext->hGEng) == ENGSTATUS_ERROR)
    {
        return(VS_FILE_ERROR);
    }

#ifndef SYM_NLM
    if (EngGlobalBootInit(hContext->hGEng,
                          hContext->wFlags & VLINIT_CREATE_BOOT_INFO_CACHE
                          ? BOOT_INIT_NO_DISK_ACCESS : 0) == ENGSTATUS_ERROR)
    {
        return(VS_FILE_ERROR);
    }
#endif

    if (EngGlobalFileInit(hContext->hGEng,
                          GENG_INIT_FILE_FLAG_LOAD_ALL) == ENGSTATUS_ERROR)
    {
        return(VS_FILE_ERROR);
    }

    if (!(hContext->wFlags & VLINIT_NO_PAM) &&
        PAMGlobalInit(szPAMFile,
                      &hContext->hGPAM) != PAMSTATUS_OK)
    {
        return(VS_FILE_ERROR);
    }

#if !defined(AVAPI_MT)                // Single threaded apps only
    if (EngLocalInit(hContext->hGEng,
                     &hContext->hLEng,
                     0xDEADBEE0UL) == ENGSTATUS_ERROR)
    {
        return(VS_FILE_ERROR);
    }

    if (hContext->hGPAM != NULL &&
        PAMLocalInit(szPAMFile,
                     NULL,
                     hContext->hGPAM,
                     &hContext->hLPAM) != PAMSTATUS_OK)
    {
        return(VS_FILE_ERROR);
    }
#endif

    //////////////////////////////////////////////////////////////////////////
    // Initialize NavEx
    //////////////////////////////////////////////////////////////////////////

#if defined(SYM_NLM)
    if (EXTInitNLM(hContext) != EXTSTATUS_OK)
    {
        return(VS_FILE_ERROR);
    }
#elif defined(SYM_VXD)
    if (EXTInitVxD(hContext) != EXTSTATUS_OK)
    {
        return(VS_FILE_ERROR);
    }
#elif defined(SYM_NTK)
    if (EXTInitNTK(hContext) != EXTSTATUS_OK)
    {
        return(VS_FILE_ERROR);
    }
#elif defined(SYM_WIN)
    if (EXTInitWin(hContext) != EXTSTATUS_OK)
    {
        return(VS_FILE_ERROR);
    }
#elif defined(SYM_DOSX)
    if (EXTInitDX (hContext) != EXTSTATUS_OK)
    {
        return(VS_FILE_ERROR);
    }
#endif

    return(VS_OK);
}


// VLDetermineVirusIndex arguments:
//
// hContext             : Engine context to use
// wVirusID             : virus ID of the offending virus
// lpwVirusIndex        : changed by the function to provide the index
//                        into VIRSCAN.DAT where the record is.
//
// Returns:
//
//     VSTATUS          : VS_OK              if no virus found
//                        VS_FILE_ERROR      if file error occured
//
// If EXTSTATUS_OK is returned, and *lpwVirusIndex != 0, then we have the
// proper record.  If == 0, then a record containing the specified virus ID
// was not found.


VSTATUS SYM_EXPORT WINAPI VLDetermineVirusIndex(HVCONTEXT hContext,
                                     WORD   wVirusID,
                                     LPWORD lpwVirusIndex)
{
    BOOL bFound;

    if (EngVIDToHVIRUS(hContext->hGEng,
                       wVirusID,
                       lpwVirusIndex,
                       &bFound) == ENGSTATUS_ERROR)
    {
        return(VS_FILE_ERROR);
    }

    return(VS_OK);
}



//---------------------------------------------------------------------------
//
// BOOL WINAPI PAMGreenLight ( LPBYTE lpbyFStartBuffer )
//
// This function examines the NAVEX request buffer from the ALG scanner to
// decide whether or not to suspend PAM.
//
// Entry:
//      lpbyFStartBuffer + 768 is where the NAVEX request buffer is....
//
// Exit:
//      TRUE  if PAM should scan
//      FALSE if a request of 0 was found and PAM should not scan...
//
//---------------------------------------------------------------------------

BOOL WINAPI PAMGreenLight(LPBYTE lpbyFStartBuffer)
{
    LPWORD      lpwTemp;
    WORD        wCount, wMax;

    // point at our NAVEX request buffer

    lpwTemp = (LPWORD)(lpbyFStartBuffer + 768);

    wMax = *lpwTemp;

    lpwTemp++;

    // if we have a NAVEX request value of zero, then don't apply PAM!

    for (wCount=0;wCount<wMax;wCount++)
        if (*lpwTemp++ == 0)
            return(FALSE);

    return(TRUE);
}



///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//
// Virus Scanning Functions
//
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/*@API:*************************************************************************
@Declaration: VSTATUS SYM_EXPORT WINAPI VLScanFile ( HVCONTEXT hContext,
LPTSTR lpszFileName, LPHVLVIRUS lphVirus )

@Description:
This routine scans the specified file for *FILE* viruses.  If the specified
file can be accessed and does not has a virus, then the function will return
a VS_OK and lphVirus will point to a NULL.If the file can't be accessed, the
function will returned appropriate erro code.  If the specified file can be
accessed and does have a virus, the function will return a VS_OK and set
lphVirus to a valid non-NULL handle of type HVLVIRUS. This should not be used
on BOO files.

@Parameters:
$hContext$ This is a valid handle to a context provided by VLScanInit.

$lpszFileName$ This is a far pointer to a zero terminated string containing the
full path and filename of the file to be scanned.

$lphVirus$ This is a pointer to HVLVIRUS handle, and is set by this function
to a non-NULL value if a virus was found, otherwise it is set to NULL.
Information can be obtained about the handle by calling VLVirusInfo.

@Returns:

This function returns
     VS_OK if scanning was successfully completed whether or not a virus
           was found. A non-NULL value is lphVirus indicates infection.

     on error:
         VS_INVALID_ARG
         VS_MEMALLOC
         VS_NO_ACCESS
         VS_ENTRY_UNAVAIL
         VS_FILE_ERROR


@Include: avapi.h
*******************************************************************************/
VSTATUS SYM_EXPORT WINAPI VLScanFile
(
    HVCONTEXT hContext,                 // [in] a valid HVCONTEXT handle
    LPTSTR    lpszFileName,             // [in] a filename to scan for viruses
    LPHVLVIRUS  lphVirus                  // [in/out] HVLVIRUS to fill in or NULL
)
{
    HVIRUS       hVirusIndex = 0;
    HVLVIRUS     hVirus;
    FILEOBJECT_T stFileObject;
    FILEINFO_T   stFileInfo;
    BOOL         bFoundVirus;
    HLENG        hLEng = NULL;
    PAMLHANDLE   hLPAM = NULL;
    WORD         wInvalidVirID = 0;     // no virus found!


    //////////////////////////////////////////////////////////////////////////
     SYM_ASSERT(lphVirus);
     *lphVirus = NULL;

    //////////////////////////////////////////////////////////////////////////
    // Verify the structure pointer and its signature
    //////////////////////////////////////////////////////////////////////////

    if (!VLValidContext(hContext))
        return(VS_INVALID_ARG);

    //////////////////////////////////////////////////////////////////////////
    // create a file object for AVENGE and open the file
    //////////////////////////////////////////////////////////////////////////

    STRCPY(stFileInfo.lpstrPathName,lpszFileName);
    stFileObject.lpParent = NULL;
    stFileObject.lpvInfo = &stFileInfo;
    stFileObject.lpCallBacks = hContext->
                               lpGlobalCallBacks->
                               lpGeneralCallBacks->
                               lpFileCallBacks;

    stFileObject.lpszName = NULL;


    //////////////////////////////////////////////////////////////////////////
    // Open the file to scan for read
    //////////////////////////////////////////////////////////////////////////

#if defined(SYM_VXD) || defined(SYM_NTK)

    // Allow shared reading but not writing for VXD and NTK

    stFileInfo.hFile = VLFileOpen(stFileInfo.lpstrPathName,READ_ONLY_FILE | OF_SHARE_DENY_NONE);

#elif defined(SYM_WIN32)

    //////////////////////////////////////////////////////////////////////////
    // For WIN32 platforms, we need to preserve last access
    //////////////////////////////////////////////////////////////////////////

    // For NT systems, we need to get the last access date
    // *BEFORE* we open the file, since opening the file
    // modifies the last access date.
    FILETIME lastAccessTime;
    BOOL bGotLastAccess;

    if( SystemGetWindowsType() == SYM_SYSTEM_WIN_NT )
        {
        // Get current last access date before opening file
        bGotLastAccess = VLGetLastAccessDateNT( stFileInfo.lpstrPathName, &lastAccessTime );
        stFileInfo.hFile = VLFileOpen(stFileInfo.lpstrPathName, READ_WRITE_FILE );
        }
    else
        {
        // For Win9x systems, we must open the file prior to getting
        // the last access date.
        stFileInfo.hFile = VLFileOpen(stFileInfo.lpstrPathName, READ_WRITE_FILE );

        bGotLastAccess = GetFileTime( (HANDLE) stFileInfo.hFile,
                                       NULL,
                                       &lastAccessTime,
                                       NULL );
        }

    // Make sure we could open the file with write access.
    // If we cannot, try opening with Read-Only access
    if( stFileInfo.hFile == HFILE_ERROR )
        {
        stFileInfo.hFile = VLFileOpen(stFileInfo.lpstrPathName, READ_ONLY_FILE );

        // Since we need write access to modify the last access date
        // make sure we do not attempt to set the last access date.
        bGotLastAccess = FALSE;
        }

#else // #elif definedSYM_WIN32

    // Leave this the same for every other platform for fear of breakage

    stFileInfo.hFile = VLFileOpen(stFileInfo.lpstrPathName, READ_ONLY_FILE );

#endif // #if defined(SYM_VXD) || defined(SYM_NTK)

    if (stFileInfo.hFile == HFILE_ERROR)
    {
        if ( hContext->wFlags & VLINIT_HANDLE_NO_ACCESS )
        {
            hVirus = (HVLVIRUS)VLPermMemoryAlloc(sHVLVIRUS);

            if (hVirus == NULL)
                return ( VS_MEMALLOC );

            VLSetHVLVIRUSFile(hVirus,
                        hContext,
                        INVALID_ENTRY_NUM,
                        VIT_NO_ACCESS | VIT_FILE,
                        lpszFileName);

            *lphVirus = hVirus;

            DateGetCurrentDateTime(&hVirus->uDate,&hVirus->uTime);
         }
         return( VS_NO_ACCESS );
    }

    ///////////////////////////////////////////////////////////////////////////
    // Scan that file, BABY!
    // Avenge scanning first
    ///////////////////////////////////////////////////////////////////////////

    if (!VLEngLocalInit(hContext, &hLEng))
    {
        return(VS_NO_ACCESS);
    }

    if (EngScanFile(hLEng,
                    &stFileObject,
                    LENG_SCAN_FILE_FLAG_SCAN_ALL,
                    0xDEADBEEFUL,
                    &bFoundVirus,
                    &hVirusIndex) != ENGSTATUS_OK)
    {
        VLEngLocalClose(hLEng);
#if defined(SYM_WIN32) && !defined(SYM_VXD) && !defined(SYM_NTK)
        //////////////////////////////////////////////////////////////////////////
        // For WIN32 platforms, we need to preserve last access date.
        //////////////////////////////////////////////////////////////////////////
        if( bGotLastAccess )
            {
            SetFileTime( (HANDLE) stFileInfo.hFile,
                        NULL,
                        &lastAccessTime,
                        NULL );
            }
#endif
        VLFileClose(stFileInfo.hFile);
        return(VS_FILE_ERROR);
    }

    if (!bFoundVirus)
    {
        hVirusIndex = 0;
    }

    ///////////////////////////////////////////////////////////////////////////
    // Now, scan with PAM if no virus had been found with Avenge
    //
    ///////////////////////////////////////////////////////////////////////////

    if (bFoundVirus == FALSE &&
        hContext->hGPAM != NULL &&
        PAMGreenLight((LPBYTE)&hLEng->stAlgScanData))
    {
        PAMSTATUS   pamStatus;
        WORD        wVirusID;

#if defined (AVAPI_MT)

        pamStatus = PAMLocalInit(HVC_szPAMScanDataFile(hContext),
                                 NULL,                  // no swap file!
                                 (PAMGHANDLE)hContext->hGPAM,
                                 &hLPAM);

#else

        pamStatus = PAMSTATUS_OK;
        hLPAM = hContext->hLPAM;

#endif

        if (pamStatus == PAMSTATUS_OK)
        {
            pamStatus = PAMScanFile(hLPAM,
                                    stFileInfo.hFile,
                                    0,      // no flags for now; need one for SYS
                                    &wVirusID);

            if (PAMSTATUS_OK == pamStatus)
            {
                hVirusIndex = 0;
            }
            else if (PAMSTATUS_VIRUS_FOUND == pamStatus)
            {
                if (VLDetermineVirusIndex(hContext,
                                        wVirusID,
                                        &hVirusIndex) != VS_OK)
                {
                    hVirusIndex = 0;
                }

            }
            else
                hVirusIndex = 0;

#if defined(AVAPI_MT)
            PAMLocalClose(hLPAM);
#endif

        }
    }
    ///////////////////////////////////////////////////////////////////

    if (hVirusIndex == 0)                   // no virus was found in the file!
    {
        EXTSTATUS   extResult;
        WORD        wVirusID;

        //////////////////////////////////////////////////////////////////////
        // No virus found using Avenge or PAM.  Try "fail-safe" detections!
        //
        // &hLEng->stAlgScanData:
        //    First 256 bytes   = FSTART returned code
        //    Next  256 bytes   = first 64 bytes of host file
        //    Next  1024 bytes  = external scan requests
        //    Next 4K-1536 bytes = work buffer for whatever
        //////////////////////////////////////////////////////////////////////

        extResult = EXTScanFile(hContext,
                                lpszFileName,
                                stFileInfo.hFile,
                                (LPBYTE)&hLEng->stAlgScanData,
                                (LPBYTE)&hLEng->stAlgScanData + 1536,
                                EXTERNAL_DETECT_VERSION,
                                &wVirusID);

        if (EXTSTATUS_OK == extResult)
        {
            VLEngLocalClose(hLEng);

#if defined(SYM_WIN32) && !defined(SYM_VXD) && !defined(SYM_NTK)
            //////////////////////////////////////////////////////////////////////////
            // For WIN32 platforms, we need to preserve last access date.
            //////////////////////////////////////////////////////////////////////////
            if( bGotLastAccess )
                {
                SetFileTime( (HANDLE) stFileInfo.hFile,
                            NULL,
                            &lastAccessTime,
                            NULL );
                }
#endif
            VLFileClose(stFileInfo.hFile);


            // now report our lack of detection to NAVEX via this cheezy hack!
            // pass 0xFFFF as version #...
            // pass pointer to zeroed wInvalidVirID in 4rd argument

            EXTScanMemory(hContext, 0xFFFF, FALSE, &wInvalidVirID);

            return(VS_OK);              // no virus found
        }

        if (EXTSTATUS_VIRUS_FOUND == extResult)
        {
            if (VLDetermineVirusIndex(hContext,
                                    wVirusID,
                                    &hVirusIndex) != VS_OK)
            {
                hVirusIndex = 0xFFFFU;  // error
            }
        }
        else
            hVirusIndex = 0xFFFFU;      // error

    }

    //////////////////////////////////////////////////////////////////////////
    // Close the file and determine what to do!
    //////////////////////////////////////////////////////////////////////////

#if defined(SYM_WIN32) && !defined(SYM_VXD) && !defined(SYM_NTK)

    //////////////////////////////////////////////////////////////////////////
    // For WIN32 platforms, we need to preserve last access date.
    //////////////////////////////////////////////////////////////////////////
    if( bGotLastAccess )
        {
        SetFileTime( (HANDLE) stFileInfo.hFile,
                    NULL,
                    &lastAccessTime,
                    NULL );
        }
#endif

    VLFileClose(stFileInfo.hFile);

    VLEngLocalClose(hLEng);

    if (hVirusIndex == 0xFFFF)
    {
        if ( hContext->wFlags & VLINIT_HANDLE_NO_ACCESS )
        {
            hVirus = (HVLVIRUS)VLPermMemoryAlloc(sHVLVIRUS);
            if (hVirus == NULL)
               return ( VS_MEMALLOC );

            VLSetHVLVIRUSFile(hVirus,
                            hContext,
                            INVALID_ENTRY_NUM,
                            VIT_NO_ACCESS | VIT_FILE,
                            lpszFileName);

            *lphVirus = hVirus;

            DateGetCurrentDateTime(&hVirus->uDate,&hVirus->uTime);

        }
        return( VS_NO_ACCESS );
    }

    //////////////////////////////////////////////////////////////////////////
    // We have a virus!  Deal with it!
    //////////////////////////////////////////////////////////////////////////

    hVirus = (HVLVIRUS)VLPermMemoryAlloc(sHVLVIRUS);

    if (hVirus == NULL)
        return(VS_MEMALLOC);

    VLSetHVLVIRUSFile(hVirus,
                    hContext,
                    hVirusIndex,
                    VIT_SCANNED | VIT_VALID_N30 | VIT_HAS_A_VIRUS | VIT_FILE,
                    lpszFileName);

    if (!VLLoadN30(hContext,hVirusIndex,&hVirus->stVirusInfoBlock))
    {

        VLPermMemoryFree(hVirus);
        *lphVirus = NULL;
        return(VS_ENTRY_UNAVAIL );
    }

    ///////////////////////////////////////////////////////////////////////////
    // A-OK good buddy
    ///////////////////////////////////////////////////////////////////////////

    DateGetCurrentDateTime(&hVirus->uDate,&hVirus->uTime);

    *lphVirus = hVirus;

    // now report our detection to NAVEX via this cheezy hack!
    // pass 0xFFFF as version #...
    // pass pointer to filled in wVirID in 4rd argument

    EXTScanMemory(hContext, 0xFFFF, FALSE, &hVirus->stVirusInfoBlock.wVirID);

    return(VS_OK);
}


/*@API:*************************************************************************
@Declaration: VSTATUS SYM_EXPORT WINAPI VLScanMemory (HVCONTEXT hContext,
WORD wFlags, LPHVLVIRUS lphVirus)

@Description:
This function scan memory for viruses.  It always scans real memory (640K),
and optionally scans hi memory. VS_OK is returned to indicate successfull
scan whether or not a virus was found. If lphVirus is pointing to a valid
non-NULL handle, then infection was found, otherwise, no viruses where found
in memory.

@Parameters:
$hContext$ This is a valid handle to a context provided by VLScanInit.

$wFlags$:  1 Scans High and low Memory.  0 scans only low memory.

$lphVirus$ This is a pointer to HVLVIRUS handle, and is set by this function
to a non-NULL value if a virus was found, otherwise it is set to NULL.
Information can be obtained about the handle by calling VLVirusInfo.

@Returns:

This function returns
     VS_OK if scanning was successfully completed whether or not a virus
           was found. A non-NULL value is lphVirus indicates infection.

     on error:
         VS_INVALID_ARG
         VS_MEMALLOC
         VS_NO_ACCESS
         VS_ENTRY_UNAVAIL

@Include: avapi.h
*******************************************************************************/

VSTATUS SYM_EXPORT WINAPI VLScanMemory
(
    HVCONTEXT   hContext,
    WORD        wFlags,
    LPHVLVIRUS  lphVirus
)
{
    HVLVIRUS         hVirus;
    WORD             wResult;
    DWORD            dwEndAddress;
    BOOL             bFoundVirus;
    MEMORYOBJECT_T   stMemoryObject;
    HLENG            hLEng = NULL;

    ///////////////////////////////////////////////////////////////////////////
    // Make sure we have a valid context
    ///////////////////////////////////////////////////////////////////////////

    *lphVirus = NULL;

    if (!VLValidContext(hContext))
        return(VS_INVALID_ARG);

    //////////////////////////////////////////////////////////////////////////
    // Set up parameters for EngScanMemory()
    //////////////////////////////////////////////////////////////////////////

    stMemoryObject.lpCallBacks = hContext->
                                 lpGlobalCallBacks->
                                 lpMemoryCallBacks;

    dwEndAddress = wFlags ? 0x100000ul : 0xA0000ul;

    //////////////////////////////////////////////////////////////////////////
    // Scan memory with Avenge Engine
    //////////////////////////////////////////////////////////////////////////

    if (!VLEngLocalInit(hContext, &hLEng))
    {
        return(VS_NO_ACCESS);
    }

    if (EngScanMemory(hLEng,
                      &stMemoryObject,
                      0,
                      dwEndAddress,
                      LENG_SCAN_TOP_OF_MEMORY,
                      0,
                      &bFoundVirus,
                      (LPHVIRUS)&wResult) != ENGSTATUS_OK)
    {
        return(VS_NO_ACCESS);
    }

    VLEngLocalClose(hLEng);

    if (!bFoundVirus)                   // no virus found - scan with NavEx
    {
        EXTSTATUS   extResult;
        WORD        wVirusID;
        WORD        wVirusIndex;

        //////////////////////////////////////////////////////////////////////
        // No virus found using NAV 3 engine.  Try "fail-safe" detections!
        //
        //////////////////////////////////////////////////////////////////////

        extResult = EXTScanMemory(hContext,
                                  EXTERNAL_DETECT_VERSION,
                                  wFlags,   // Assume 0=don't scan high memory
                                  &wVirusID);

        if (EXTSTATUS_OK == extResult)
            return(VS_OK);              // no virus found

        if (EXTSTATUS_VIRUS_FOUND == extResult)
        {
            if (VS_OK ==
                VLDetermineVirusIndex(hContext,
                                    wVirusID,
                                    &wVirusIndex))
                wResult = wVirusIndex;
            else
                wResult = 0xFFFEU;      // FFFE -> Virus in memory but we
                                        // don't know its ID number, perhaps
                                        // we have an out of sync VIRSCAN.DAT
                                        // and AVEX.
        }
        else
            wResult = 0xFFFFU;          // error (VLLoadN30 will fail)
    }

    if (0xFFFFU == wResult)             // only = if we have an error return
        return(VS_NO_ACCESS);           // from EXTScanMemory


    ///////////////////////////////////////////////////////////////////////////
    // Virus found, return an hVirus structure...
    ///////////////////////////////////////////////////////////////////////////

    hVirus = (HVLVIRUS)VLPermMemoryAlloc(sHVLVIRUS);

    if (hVirus == NULL)
        return(VS_MEMALLOC);

    MEMSET (hVirus, 0, sizeof(VIRINFOTYPE));

    if (!VLLoadN30(hContext,wResult,&hVirus->stVirusInfoBlock))
    {
       hVirus->dwFlags |= VIT_UNKOWN_VIRUS;
    }

    hVirus->lpVContext  = hContext;
    hVirus->hNext       = NULL;
    hVirus->uDBEntryNum = wResult;
    hVirus->dwFlags     = VIT_SCANNED | VIT_UNREPAIRABLE | VIT_VALID_N30 |
                          VIT_FOUND_IN_MEMORY | VIT_HAS_A_VIRUS |
                          VIT_DISEMBODIED;

    DateGetCurrentDateTime(&hVirus->uDate,&hVirus->uTime);
    *lphVirus = hVirus;

    return(VS_OK);
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//
// Repair functions follow...
//
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////


//********************************************************************
//
//********************************************************************

WORD LOCAL VLGetInfectionDepth (
    HVLVIRUS hVirus )
{
    if (hVirus != NULL)
        return( 1 + VLGetInfectionDepth(hVirus->hNext));
    else
        return(0);
}


//***************************************************************************
// VLGetCountOfDefinitionsInFile()
//
// Description: calculates the number of definitions
//
// Parameters:
//
// Return Value:
//
// See Also:
//
//***************************************************************************
// 01/30/1995 JMILLARD Function Created.
//***************************************************************************
UINT SYM_EXPORT WINAPI VLGetCountOfDefinitionsInFile(
    HVCONTEXT          hContext,    // [in] a valid HVCONTEXT handle
    HFILE hFile )                   // [in] a handle for the file
{
    return (VLNumVirusEntries(hContext));
}



/*@API:*************************************************************************
@Declaration: UINT WINAPI   VLTrueNumVirusEntries( HVCONTEXT hvContext )

@Description:
This routine can be used to determine how many definitions there are in the
VIRSCAN.DAT database. This can be used to reliably size tables for
VLFillListTable.

@Parameters:
$hContext$ This is a valid handle to a context provided by VLScanInit.

@Returns:
This routine returns the actual number of virus definitions, empty or otherwise,
in the database.

@Include: avapi.h
*******************************************************************************/

UINT SYM_EXPORT WINAPI VLTrueNumVirusEntries
(
    HVCONTEXT          hContext             // [in] a valid HVCONTEXT handle
)
{
    return (VLNumVirusEntries(hContext));
}


/*@API:*************************************************************************
@Declaration: BOOL SYM_EXPORT WINAPI VLValidContext(HVCONTEXT hContext)

@Description:
    Verify that the hContext is valid.

@Parameters:
    $hContext$ This is a handle to a context provided
               by VLScanInit.
@Returns:
    TRUE  if hContext is valid
    FALSE otherwise
@Include: avapi.h
*******************************************************************************/
BOOL SYM_EXPORT WINAPI VLValidContext(HVCONTEXT hContext)
{
    if (hContext == NULL)
        return(FALSE);

    if (hContext->dwSignature != 0xDEADFECEL)
        return(FALSE);

    return(TRUE);
}


/*@API:*************************************************************************
@Declaration: BOOL WINAPI VLIsFileUpToDate(LPTSTR lpPath, LPTSTR FileName, FILETIME FileTime)

@Description:
   determines whether the target file is uptodate.
@Parameters:

LPSTR lpPath     : location of VIRSCAN.DAT, NAV, etc... (SYM_VXD: in OEM!)
LPSTR FileName   : file to check (SYM_VXD: in OEM!)
FILETIME FileTime: check against this these values

@Returns:
    TRUE  if up to date
    FALSE otherwise
@Include: avapi.h
*******************************************************************************/
BOOL WINAPI VLIsFileUpToDate(LPTSTR lpPath, LPTSTR lpFileName, FILETIME FileTime)
{
    HFILE     hFile;
    FILETIME  ft;
    TCHAR     szTargetFile[SYM_MAX_PATH + 1];

    STRCPY ( szTargetFile , lpPath );
    NameAppendFile ( szTargetFile, lpFileName );

#if defined(SYM_VXD)
    auto char szOSTargetFile[SYM_MAX_PATH * sizeof(WCHAR)];

    VxDGenericStringConvert (
        szTargetFile,
        THFLAG_OEM,
        szOSTargetFile,
        ((tcb_s*)Get_Cur_Thread_Handle())->TCB_Flags & THFLAG_CHARSET_MASK );

    if ( (hFile = FileOpen ( szOSTargetFile, READ_ONLY_FILE )) == HFILE_ERROR )
#else
    if ( (hFile = FileOpen ( szTargetFile, READ_ONLY_FILE )) == HFILE_ERROR )
#endif
       return ( FALSE );

    FileGetTime(hFile, TIME_LASTWRITE, &ft);
    FileClose(hFile);

    if ( MEMCMP ( &ft, &FileTime, sizeof(FILETIME) ) )
       return ( FALSE );

    return ( TRUE );

}

/*@API:*************************************************************************
@Declaration: BOOL WINAPI VLVerifyDataFile(LPTSTR lpPath, LPTSTR FileName )

@Description:
Verify that the file exists and has length greater than zero.

@Parameters:
LPSTR lpPath     : location of VIRSCAN.DAT, NAV, etc...
LPSTR FileName   : file to verify

@Returns:
    TRUE  if exists and size greater than zero (0)
    FALSE otherwise
@Include: avapi.h
*******************************************************************************/
BOOL WINAPI VLVerifyDataFile(LPTSTR lpPath, LPTSTR lpFileName )
{
    HFILE  hFile;
    TCHAR  szTargetFile[SYM_MAX_PATH + 1];
    DWORD  dwSize;

    STRCPY ( szTargetFile , lpPath );
    NameAppendFile ( szTargetFile, lpFileName );

#if defined(SYM_VXD)
    auto char szOSTargetFile[SYM_MAX_PATH * sizeof(WCHAR)];

    VxDGenericStringConvert (
        szTargetFile,
        THFLAG_OEM,
        szOSTargetFile,
        ((tcb_s*)Get_Cur_Thread_Handle())->TCB_Flags & THFLAG_CHARSET_MASK );

    if ( (hFile = FileOpen ( szOSTargetFile , READ_ONLY_FILE )) == HFILE_ERROR )
#else
    if ( (hFile = FileOpen ( szTargetFile , READ_ONLY_FILE )) == HFILE_ERROR )
#endif
       return ( FALSE );

    dwSize =  FileLength(hFile);

    FileClose ( hFile );

    return ( dwSize ? TRUE : FALSE );

}

///////////////////////////////////////////////////////////////////////////////
// TRUE = success, FALSE = failure
///////////////////////////////////////////////////////////////////////////////

BOOL SYM_EXPORT VLLoadN30
(
    HVCONTEXT hContext,                 // [in] a valid HVCONTEXT handle
    UINT uIndex,                        // [in] 1-based entry # in VIRSCAN.DAT
    LPN30 lpN30                         // [out] fill this buffer with N30
)
{
    HLENG       hLEng = NULL;

    //////////////////////////////////////////////////////////////////////////
    // get from cache, if available
    //////////////////////////////////////////////////////////////////////////

    if ( hContext->uN30EFCount &&
         VLLoadCachedN30 ( hContext,
                           uIndex,
                           hContext->uN30EFCount,
                           hContext->lprN30EFBuffer,
                           lpN30 )
       )
    {
        return ( TRUE );
    }

    if ( hContext->uN30EBCount &&
         VLLoadCachedN30 ( hContext,
                           uIndex,
                           hContext->uN30EBCount,
                           hContext->lprN30EBBuffer,
                           lpN30 )
       )
    {
        return ( TRUE );
    }

    if (!VLEngLocalInit(hContext, &hLEng))
    {
        return (FALSE);
    }

    if (EngGetN30HVIRUS ( hLEng,
                          uIndex,
                          lpN30 ) == ENGSTATUS_ERROR )
    {
        VLEngLocalClose (hLEng);
        return (FALSE);
    }

    VLEngLocalClose (hLEng);

    return(TRUE);
}

///////////////////////////////////////////////////////////////////////////////
// TRUE = success, FALSE = failure
///////////////////////////////////////////////////////////////////////////////

BOOL LOCAL VLLoadCachedN30
(
    HVCONTEXT hContext,                 // [in] a valid HVCONTEXT handle
    UINT      uIndex,                   // [in] 1-based entry # in VIRSCAN.DAT
    UINT      uN30EEntries,             // [in] count of entries in lpN30E
    LPN30E    lpN30E,                   // [in] pointer to a list of N30E's
    LPN30     lpN30                     // [out] fill this buffer with N30
)
{
                                        // Validate input parameters

    SYM_ASSERT ( uN30EEntries );
    SYM_ASSERT ( lpN30E );
    SYM_ASSERT ( lpN30 );

    (void)hContext;

    // Search for uIndex

    while ( uN30EEntries-- )
        {
        if ( lpN30E->uVirusEnumerator == uIndex )
            {
            *lpN30 = lpN30E->rN30;
            return ( TRUE );
            }

        lpN30E++;
        }

    return ( FALSE );
}

VOID LOCAL VLSetHVLVIRUSFile (
    HVLVIRUS      hVirus,
    HVCONTEXT   lpVContext,
    UINT        uDBEntryNum,
    DWORD       dwFlags,
    LPTSTR      lpszFileName )
{
    hVirus->lpVContext              = lpVContext;
    hVirus->uDBEntryNum             = uDBEntryNum;
    hVirus->dwFlags                 = dwFlags;
    hVirus->hNext                   = NULL;

#if defined(SYM_VXD)
    VxDGenericStringConvert ( lpszFileName, CHARSET_CURRENT,
                              hVirus->FileInfo.szFileName, CHARSET_OEM );

    VxDGenericStringConvert ( lpszFileName, CHARSET_CURRENT,
                              hVirus->FileInfo.szAnsiFileName, CHARSET_ANSI );

    VxDGenericStringConvert ( lpszFileName, CHARSET_CURRENT,
                              hVirus->FileInfo.szUniFileName, CHARSET_UNICODE );
#else
    STRCPY(hVirus->FileInfo.szFileName,lpszFileName);
#endif
}

///////////////////////////////////////////////////////////////////////////////
// TRUE = success, FALSE = failure
///////////////////////////////////////////////////////////////////////////////

BOOL LOCAL VLEngLocalInit (
    HVCONTEXT hContext,
    LPHLENG lphLEng )
{

#if defined (AVAPI_MT)

    if (EngLocalInit(hContext->hGEng,
                     lphLEng,
                     0xDEADBEE0UL) != ENGSTATUS_OK)
    {
        return (FALSE);
    }

#else

    *lphLEng = hContext->hLEng;

#endif

    return (TRUE);
}

VOID LOCAL VLEngLocalClose (
    HLENG hLEng )
{

#if defined (AVAPI_MT)

    if (hLEng != NULL)
        EngLocalClose(hLEng);

#endif

}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//
// Virus information functions...
//
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
BOOL LOCAL VLGetVirusInfo
(
    HVCONTEXT hContext,                 // [in] a valid HVCONTEXT handle
    WORD      wVirusID,                 // [in] 1 based virus entry number
    LPTSTR    lpAlias,                  // [out] alias of virus
    LPTSTR    lpComment                 // [out] comments about virus
)

{
    HFILE       hFile;
    LPSTR       lpMem;
    LONG        dwOffset;
    INF_HEADER  inf;
    LPTSTR      lpData;

    if ((hFile = VLFileOpen(HVC_szVirScanInfoFile(hContext), READ_ONLY_FILE))
                != HFILE_ERROR )
    {

        ///////////////////////////////////////////////////////////////////////////
        // Allocate a chunk of memory big enough to store a comment
        // and an alias string.
        ///////////////////////////////////////////////////////////////////////////

#if defined(UNICODE)
        lpMem = (LPSTR)VLTempMemoryAlloc((SIZE_ALIAS + SIZE_COMMENT + 10) +
                                         (SIZE_ALIAS + SIZE_COMMENT + 10) * sizeof(WCHAR));

        lpData = (LPTSTR)(lpMem + (SIZE_ALIAS + SIZE_COMMENT + 10));
#else
        lpMem = (LPSTR)VLTempMemoryAlloc(SIZE_ALIAS + SIZE_COMMENT + 10);
        lpData = lpMem;
#endif

        if (lpMem == NULL)
        {
            VLFileClose(hFile);

            return(FALSE);
        }

        VLFileSeek (hFile, SIZE_COPYRIGHT, SEEK_SET);
        VLFileRead (hFile, &inf, sizeof(INF_HEADER));

        if (inf._i_version != 0)
        {
            VLTempMemoryFree(lpMem);
            VLFileClose(hFile);

            return(FALSE);
        }

        if (wVirusID > inf._i_highest_vid)
            wVirusID = 0;

        VLFileSeek (hFile, (LONG)wVirusID * (LONG)inf._i_size, SEEK_CUR);
        VLFileRead (hFile, lpMem, inf._i_size);

// &? Do we need to check for read error here?

        if (4 == inf._i_size)
            dwOffset = *(LONG FAR *)lpMem;
        else
            dwOffset = *(WORD FAR *)lpMem;

#if 0
                                        // dwOffset == 0 means to use the
                                        // "No info" record.
        if ( ! dwOffset )
            {
            *lpAlias = *lpComment = EOS;
            return(FALSE);
            }
#endif


        dwOffset = (dwOffset * inf._i_multiplier) + inf._i_no_info;

        VLFileSeek (hFile, dwOffset, SEEK_SET);
        VLFileRead (hFile, lpMem, SIZE_ALIAS + SIZE_COMMENT + 2);

        ///////////////////////////////////////////////////////////////////////
        // First comes the alias field. If its length is longer
        // than the maximum possible alias field then dump it.
        ///////////////////////////////////////////////////////////////////////

#if defined(UNICODE)
        OemToCharW ( (LPSTR)lpMem, lpData );
#endif

        if (STRLEN(lpData) <= SIZE_ALIAS)
        {
            STRCPY(lpAlias, lpData);
#if defined(UNICODE)
            OemToCharW ( &lpMem[strlen(lpMem)+1], lpData );
#else
            lpData = ( lpMem + (STRLEN(lpMem) + 1) );
#endif

            ///////////////////////////////////////////////////////////////////
            // The same goes for the comment field
            ///////////////////////////////////////////////////////////////////

            if ( STRLEN ( lpData ) <= SIZE_COMMENT )
            {
                // Convert '&&' to just one.  This is
                // for Windows compatibility.

                for ( ; *lpData; lpData++)
                {
                                        // ----------------------------------
                                        // Skip double '&&'
                                        // ----------------------------------
#if !defined (UNICODE)
                                        // If it's a DBCS char it can't be "&&"
                    if (DBCSIsLeadByte (*lpData))
                    {
                        *lpComment++ = *lpData++;
                        *lpComment++ = *lpData;
                        continue;
                    }
#endif
                    if ( *lpData == _T('&')
                    &&   *(lpData + 1) == _T('&') )
                        continue;
                                        // ----------------------------------
                                        // Copy this character
                                        // ----------------------------------

                    *lpComment++ = *lpData;
                }
                *lpComment = EOS;
            }
            else
            {
                lpComment[0] = EOS;

                VLTempMemoryFree(lpMem);
                VLFileClose(hFile);

                return(FALSE);
            }
        }
        else
        {
            lpAlias[0] = EOS;
            lpComment[0] = EOS;

            VLTempMemoryFree(lpMem);
            VLFileClose(hFile);

            return(FALSE);
        }

        VLTempMemoryFree (lpMem);
        VLFileClose(hFile);

        return(TRUE);
    }

    return(FALSE);
}

/*@API:*************************************************************************
@Declaration: UINT WINAPI   VLNumVirusEntries( HVCONTEXT hvContext )

@Description:
This routine can be used to determine how many viruses there are in the
VIRSCAN.DAT database. This is a conceptual number of viruses, and may be
well less than the number of definitions. A virus may have several definitions,
but only be counted as one virus. This number is put in to the header from
the file vircount.dat. THE RESULT OF THIS FUNCTION SHOULD BE USED FOR DISPLAY
PURPOSES ONLY!

@Parameters:
$hContext$ This is a valid handle to a context provided by VLScanInit.

@Returns:
This routine returns the number of virus definitions in the database.

@Include: avapi.h
*******************************************************************************/


UINT SYM_EXPORT WINAPI VLNumVirusEntries
(
    HVCONTEXT hContext                  // [in] a valid HVCONTEXT handle
)
{
    WORD wNumVirusRecords = 0;

    EngGetNumVirusRecords (hContext->hGEng, &wNumVirusRecords);

    return ((UINT)wNumVirusRecords);
}


/*@API:*************************************************************************
@Declaration: BOOL WINAPI VLVirusInfo(HVLVIRUS hVirus, VLVIRINFO enumVirInfo,
LPTSTR lpszResultBuffer)

@Description:
This routine can be used to obtain virus information about the specified
HVLVIRUS handle.  This routine will return TRUE/FALSE for all boolean information
requests, and fill the lpszResultBuffer for all other requests.

@Parameters:
$hVirus$ This is a valid HVLVIRUS handle.

$enumVirInfo$ This is a value of type VLVIRINFO. (See AVAPI.H) for possible
arguments.  Exaples:

        VI_VIRUS_NAME,                  // obtain virus's name
        VI_VIRUS_ALIAS,                 // obtain virus's alias list
        VI_VIRUS_INFO,                  // obtain virus information
        VI_VIRUS_SIZE,                  // obtain virus's size

        VI_BOOL_INFECT_FILES,           // does this virus infect FILES in general? (MAC too)
        VI_BOOL_INFECT_EXE,             // does this virus infect EXE's?
        VI_BOOL_INFECT_COM,             // does this virus infect COM's?
        VI_BOOL_INFECT_COMCOM,          // does this virus infect COMMAND.COM?
        VI_BOOL_INFECT_SYS,             // does this virus infect SYS's?
        VI_BOOL_INFECT_MBR,             // does this virus infect MBR's?
        VI_BOOL_INFECT_FLOPPY_BOOT,     // does this virus infect floppies?
        VI_BOOL_INFECT_HD_BOOT,         // does this virus infect HD bootsecs?

        VI_BOOL_INFECT_WILD,            // is this virus in the wild?

        VI_BOOL_MEMORY_RES,             // can this virus go memory resident?
        VI_BOOL_SIZE_STEALTH,           // does this virus size stealth?
        VI_BOOL_FULL_STEALTH,           // does this virus full stealth?
        VI_BOOL_TRIGGER,                // does this virus have a trigger?
        VI_BOOL_ENCRYPTING,             // is this virus encrypting?
        VI_BOOL_POLYMORPHIC,            // is this virus polymorphic?
        VI_BOOL_MULTIPART,              // is this virus multipartite?
        VI_BOOL_ISMACVIRUS,             // macinsux virus.
        VI_BOOL_MACRO_VIRUS,            // macro virus
        VI_BOOL_WINDOWS_VIRUS,          // windows virus
        VI_BOOL_AGENT_VIRUS,            // agent virus

        ///////////////////////////////////////////////////////////////////////
        // item specific queries
        ///////////////////////////////////////////////////////////////////////

        VI_BOOL_REPAIRED,               // has this specific item been repaired?
        VI_BOOL_REPAIR_FAILED,          //
        VI_BOOL_REPAIRABLE,             // is this specific item repairable?
        VI_BOOL_DELETABLE,              // is this virus deletable?
        VI_BOOL_NO_ACCESS,              // check if we have access to this item
        VI_BOOL_DISEMBODIED,            // is the item disembodied (mem/boo)
        VI_BOOL_DELETED,                // has this file been deleted yet?
        VI_BOOL_DELETE_FAILED,          //
        VI_BOOL_INFO_VALID,             // is this information valid?
        VI_BOOL_MEMORY_DETECT,          // is this item a memory detection?
        VI_BOOL_FILE_ITEM,              // is this item a file detection?
        VI_BOOL_BOOT_ITEM,              // is this item a boot detection?
        VI_BOOL_PART_ITEM,              // is this item a part detection?

        VI_ITEM_NAME,                   // the item name (filename or otherwise)
        VI_DETECT_DATE,
        VI_DETECT_TIME,
        VI_FILENAME,                    // obtain filename of scanned item
        VI_PHYSICAL_DRIVE_NUM,          // get physical drive # of scanned item
        VI_PARTITION_NUM,               // get partition num of scanned item
        VI_PARTITION_TYPE,              // get partition type of scanned item
        VI_DRIVE_TYPE,                  // get drive type of scanned item

$lpszResultBuffer$ This is a far pointer to a buffer where numeric and textual
information results are placed.  All non BOOLEAN numerical/textual results are
converted to strings and placed in this buffer.

@Returns:

    This function returns:

        TRUE            if the query is a boolean query and the result is TRUE
                        OR if the query is a non-boolean query and the
                        information has been successfully retrieved.

        FALSE           if the query is a boolean query and the result is FALSE
                        OR if the  query is a non-boolean query and the
                        information has not been retrieved.

Note:
                        The MBCS version of this function will return FALSE
                        if STRLEN(virusname) > SIZE_VIRUSNAME (xref:
                        VI_VIRUS_NAME).  The WCHAR version of this function
                        will always return TRUE, but if STRLEN(virusname) >
                        SIZE_VIRUSNAME, only the first SIZE_VIRUSNAME - 1
                        characters will be copied to lpszResultBuffer.
@Include: avapi.h
*******************************************************************************/

BOOL SYM_EXPORT WINAPI VLVirusInfo
(
    HVLVIRUS        hVirus,               // [in] a valid HVLVIRUS handle
    WORD          wDepth,               // [in] level of infection
    VLVIRINFO     enumVirInfo,          // [in] which information to retrieve
    LPTSTR        lpszResultBuffer      // [out] place results in this buffer
)
{
    HVCONTEXT   hContext;

    if (wDepth > 0)
    {
        if (hVirus->hNext == NULL)
            return(FALSE);

        return(VLVirusInfo(hVirus->hNext,wDepth-1,enumVirInfo,lpszResultBuffer));
    }

    if (hVirus == NULL)
        return(FALSE);

    hContext = hVirus->lpVContext;

    switch (enumVirInfo)
    {
        case VI_INFECTION_DEPTH:
            SYM_ASSERT(lpszResultBuffer);
            wDepth = VLGetInfectionDepth(hVirus);
            _ConvertWordToString(wDepth,lpszResultBuffer, 10, 0);
            return(TRUE);

        case VI_VIRUS_NAME:
            SYM_ASSERT(lpszResultBuffer);
                               // this flag would have been set only if
                               // VLoadN30 failed to load virus info.
            if (hVirus->dwFlags & VIT_UNKOWN_VIRUS )
               return ( FALSE );

                               // check for valid virus name
#if defined(UNICODE)
            // mmendon, 12/06/99:  Adding 1 to the size of lpszResultBuffer because a virus name
            // can be N30_SIZE_VIRUSNAME chars.  Without the extra char there is no room for the
            // terminating null char in the unicode string when the virus name is N30_SIZE_VIRUSNAME chars.
            // Per the NAVAPI team, AP is the only user of this code.  AP has been searched and all buffers
            // being passed in are 23 wide chars.
            OemToCharBuffW ( hVirus->stVirusInfoBlock.lpVName,
                             lpszResultBuffer,
                             N30_SIZE_VIRUSNAME + 1 );
#else
            if (STRLEN ( hVirus->stVirusInfoBlock.lpVName)  > N30_SIZE_VIRUSNAME )
               return ( FALSE );

            STRCPY(lpszResultBuffer,hVirus->stVirusInfoBlock.lpVName);
#endif

            return(TRUE);

        case VI_VIRUS_ALIAS:
            SYM_ASSERT(lpszResultBuffer);
            return(VLGetVirusAlias(hVirus->lpVContext,
                                   hVirus->stVirusInfoBlock.wVirID,
                                   lpszResultBuffer));
        case VI_VIRUS_INFO:
            SYM_ASSERT(lpszResultBuffer);
            return(VLGetVirusText(hVirus->lpVContext,
                                  hVirus->stVirusInfoBlock.wVirID,
                                  lpszResultBuffer));

        case VI_VIRUS_SIZE:
            SYM_ASSERT(lpszResultBuffer);
            if (hVirus->stVirusInfoBlock.wType == BOOtype)
                _ConvertWordToString(SECTOR_SIZE,lpszResultBuffer, 10, 0);
            else
                _ConvertWordToString(hVirus->stVirusInfoBlock.wInfSize,
                                    lpszResultBuffer, 10, 0);
            return(TRUE);

        case VI_BOOL_INFECT_EXE:
            if (hVirus->stVirusInfoBlock.wInfo & bitINFO_EXEFILE)
                return(TRUE);
            else
                return(FALSE);

        case VI_BOOL_INFECT_FILES:
            if ((hVirus->stVirusInfoBlock.wInfo &
                 (bitINFO_EXEFILE | bitINFO_COMFILE | bitINFO_COMCOM | bitINFO_SYSFILE)) ||
                hVirus->stVirusInfoBlock.wType != BOOtype)
                return(TRUE);
            else
                return(FALSE);

        case VI_BOOL_INFECT_COM:
            if (hVirus->stVirusInfoBlock.wInfo & bitINFO_COMFILE)
                return(TRUE);
            else
                return(FALSE);

        case VI_BOOL_INFECT_COMCOM:
            if (hVirus->stVirusInfoBlock.wInfo & bitINFO_COMCOM)
                return(TRUE);
            else
                return(FALSE);

        case VI_BOOL_INFECT_SYS:
            if (hVirus->stVirusInfoBlock.wInfo & bitINFO_SYSFILE)
                return(TRUE);
            else
                return(FALSE);

        case VI_BOOL_INFECT_MBR:
            if (hVirus->stVirusInfoBlock.wInfo & bitINFO_MBOOTREC)
                return(TRUE);
            else
                return(FALSE);

        case VI_BOOL_INFECT_FLOPPY_BOOT:
            if (hVirus->stVirusInfoBlock.wInfo & bitINFO_FLOPBOOT)
                return(TRUE);
            else
                return(FALSE);

        case VI_BOOL_INFECT_HD_BOOT:
            if (hVirus->stVirusInfoBlock.wInfo & bitINFO_HDISKBOOT)
                return(TRUE);
            else
                return(FALSE);

        case VI_BOOL_INFECT_WILD:
            if (hVirus->stVirusInfoBlock.wInfo & bitINFO_INTHEWILD)
                return(TRUE);
            else
                return(FALSE);

        case VI_BOOL_MEMORY_RES:
            if (hVirus->stVirusInfoBlock.wInfo & bitINFO_RESIDENT)
                return(TRUE);
            else
                return(FALSE);

        case VI_BOOL_SIZE_STEALTH:
            if (hVirus->stVirusInfoBlock.wInfo & bitINFO_SIZESTLTH)
                return(TRUE);
            else
                return(FALSE);

        case VI_BOOL_FULL_STEALTH:
            if (hVirus->stVirusInfoBlock.wInfo & bitINFO_FULLSTLTH)
                return(TRUE);
            else
                return(FALSE);

        case VI_BOOL_TRIGGER:
            if (hVirus->stVirusInfoBlock.wInfo & bitINFO_TRIGGERED)
                return(TRUE);
            else
                return(FALSE);

        case VI_BOOL_ENCRYPTING:
            if (hVirus->stVirusInfoBlock.wInfo & bitINFO_ENCRYPT)
                return(TRUE);
            else
                return(FALSE);

        case VI_BOOL_POLYMORPHIC:
            if (hVirus->stVirusInfoBlock.wInfo & bitINFO_POLYMORPH)
                return(TRUE);
            else
                return(FALSE);

        case VI_BOOL_MACRO_VIRUS:            // macro virus
            if ( bitINFO_MACRO ==
                (hVirus->stVirusInfoBlock.wControl2 & bitINFO_MASKEXT))
                return(TRUE);
            else
                return(FALSE);

        case VI_BOOL_WINDOWS_VIRUS:          // windows virus
            if (bitINFO_WINDOWS ==
               (hVirus->stVirusInfoBlock.wControl2 & bitINFO_MASKEXT))
                return(TRUE);
            else
                return(FALSE);

        case VI_BOOL_AGENT_VIRUS:            // agent virus
            if (bitINFO_AGENT ==
               (hVirus->stVirusInfoBlock.wControl2 & bitINFO_MASKEXT))
                return(TRUE);
            else
                return(FALSE);

        case VI_BOOL_MULTIPART:
            if (hVirus->stVirusInfoBlock.wInfo & bitINFO_MULTIPARTY)
                return(TRUE);
            else
                return(FALSE);

        case VI_BOOL_MAC_VIRUS:
            if (hVirus->stVirusInfoBlock.wType == MACtype)
                return(TRUE);
            else
                return(FALSE);

        case VI_BOOL_REPAIRED:
            if (hVirus->dwFlags & VIT_REPAIRED)
                return(TRUE);
            else
                return(FALSE);

#ifndef SYM_NLM
        case VI_BOOL_DELETABLE:
            return(VLCanDeleteItem(hVirus));
#endif

        case VI_BOOL_REPAIRABLE:
            return(VLCanRepairItem(hVirus));

        case VI_BOOL_NO_ACCESS:
            if (hVirus->dwFlags & VIT_NO_ACCESS)
                return(TRUE);
            else
                return(FALSE);

        case VI_BOOL_DISEMBODIED:
            if (hVirus->dwFlags & VIT_DISEMBODIED)
                return(TRUE);
            else
                return(FALSE);

        case VI_BOOL_DELETED:
            if (hVirus->dwFlags & VIT_DELETED)
                return(TRUE);
            else
                return(FALSE);

        case VI_BOOL_INFO_VALID:
            if (hVirus->dwFlags & VIT_VALID_N30)
                return(TRUE);
            else
                return(FALSE);

        case VI_BOOL_MEMORY_DETECT:
            if (hVirus->dwFlags & VIT_FOUND_IN_MEMORY)
                return(TRUE);
            else
                return(FALSE);

        case VI_BOOL_FILE_ITEM:
            if (hVirus->dwFlags & VIT_FILE)
                return(TRUE);
            else
                return(FALSE);

        case VI_BOOL_BOOT_ITEM:
            if ((hVirus->dwFlags & VIT_BOOTPART) &&
                (hVirus->BootInfo.byBootFlags & BI_BOOT_SECTOR))
                return(TRUE);
            else
                return(FALSE);

        case VI_BOOL_PART_ITEM:
            if ((hVirus->dwFlags & VIT_BOOTPART) &&
                (hVirus->BootInfo.byBootFlags & BI_PARTITION_SECTOR))
                return(TRUE);
            else
                return(FALSE);

        case VI_FILENAME:
            SYM_ASSERT(lpszResultBuffer);
            if (hVirus->dwFlags & VIT_FILE)
            {
#if defined(SYM_VXD)
                                        // Not implemented for VxD
                SYM_ASSERT(FALSE);
                return(FALSE);
#else
                STRCPY(lpszResultBuffer,hVirus->FileInfo.szFileName);
                return(TRUE);
#endif
            }
            else
                return(FALSE);

        case VI_ITEM_NAME:
            SYM_ASSERT(lpszResultBuffer);
            if (hVirus->dwFlags & VIT_FILE)
            {
#if defined(SYM_VXD)
                                        // Not implemented for VxD
                SYM_ASSERT(FALSE);
                return(FALSE);
#else
                STRCPY(lpszResultBuffer,hVirus->FileInfo.szFileName);
                return(TRUE);
#endif
            }
            else if (hVirus->dwFlags & VIT_FOUND_IN_MEMORY )
            {
                STRCPY(lpszResultBuffer,_T("Memory"));
                return(TRUE);
            }
            else if (hVirus->dwFlags & VIT_BOOTPART )
            {
                TCHAR lpszTemp[20];

                _ConvertWordToString(hVirus->BootInfo.byDriveNum,
                                          lpszTemp, 10, 0);
                STRCAT (lpszTemp, _T(" ") );
                STRCPY(lpszResultBuffer, lpszTemp);

                _ConvertWordToString(hVirus->BootInfo.byBootSecNum,
                                        lpszTemp, 10, 0);   // partition number
                STRCAT (lpszTemp, _T(" ") );
                STRCAT (lpszResultBuffer, lpszTemp );

                _ConvertWordToString(hVirus->BootInfo.bySystemType,
                                        lpszTemp, 10, 0);
                STRCAT (lpszResultBuffer, lpszTemp );

            }
            else
            {
                SYM_ASSERT(0);
                return(FALSE);
            }

        case VI_DETECT_DATE:
            SYM_ASSERT(lpszResultBuffer);
            _ConvertLongToString ( (LONG)hVirus->uDate, lpszResultBuffer,
                                    10, 0 );
            return(TRUE);

        case VI_DETECT_TIME:
            SYM_ASSERT(lpszResultBuffer);
            _ConvertLongToString ( (LONG)hVirus->uTime, lpszResultBuffer,
                                   10, 0 );
            return(TRUE);

        case VI_BOOL_REPAIR_FAILED:
            if (hVirus->dwFlags & VIT_UNREPAIRABLE )
            {
                return(TRUE);
            }
            else
                return(FALSE);

        case VI_BOOL_DELETE_FAILED:
            if (hVirus->dwFlags & VIT_CANNOT_DELETE )
            {
                return(TRUE);
            }
            else
                return(FALSE);

        case VI_PHYSICAL_DRIVE_NUM:
            SYM_ASSERT(lpszResultBuffer);
            if (hVirus->dwFlags & VIT_BOOTPART)
            {
                _ConvertWordToString(hVirus->BootInfo.byDriveNum,
                                        lpszResultBuffer, 10, 0);
                return(TRUE);
            }
            else
                return(FALSE);

        case VI_PARTITION_NUM:
            SYM_ASSERT(lpszResultBuffer);
            if (hVirus->dwFlags & VIT_BOOTPART)
            {
                _ConvertWordToString(hVirus->BootInfo.byBootSecNum,
                                        lpszResultBuffer, 10, 0);
                return(TRUE);
            }
            else
                return(FALSE);

        case VI_PARTITION_TYPE:
            SYM_ASSERT(lpszResultBuffer);
            if (hVirus->dwFlags & VIT_BOOTPART)
            {
                _ConvertWordToString(hVirus->BootInfo.bySystemType,
                                        lpszResultBuffer, 10, 0);
                return(TRUE);
            }
            else
                return(FALSE);
    }

    return ( FALSE );
}

/*@API:*************************************************************************
@Declaration: VSTATUS WINAPI VLRepairItem(HVLVIRUS hVirus, LPVOID lpvCookie)

@Description:
This routine can be used to repair items (files, sectors) infected with
viruses.  It will automatically determine what type of repair is necessary
and apply it (multiple times, if necessary).  The HVLVIRUS handle will be updated to reflect
the repair status of the item.

@Parameters:
$hVirus$ This is a valid HVLVIRUS handle.
$lpvCookie& for NAVEX UI functions - determines whether UI is on or off

@Returns:
This function returns a result of type VSTATUS (type definition found in
AVAPI.H).

    VS_OK           will be returned on success
    VS_INVALID_ARG  will be returned if the context is invalid
    VS_CANT_REPAIR  will be returned if the item cannot be repaired
    VS_MEMALLOC     will be returned if memory cannot be allocated
    VS_FILE_ERROR   will be returned if there is an error accessing the item
    VS_CRIT_ERROR   will be returned on critical error
    VS_DRIVE_ERROR  will be returned on a drive access error

@Include: avapi.h
*******************************************************************************/

VSTATUS SYM_EXPORT WINAPI VLRepairItem
(
    HVLVIRUS hVirus,                       // [in] a valid HVLVIRUS handle
    LPVOID   lpvCookie
)
{
    if (hVirus == NULL || hVirus->lpVContext == NULL)
    {
        return(VS_INVALID_ARG);
    }
    else if (VLCanRepairItem(hVirus) == FALSE)
    {
        return(VS_CANT_REPAIR);
    }
    else if (hVirus->dwFlags & VIT_FILE)
    {
        return(VLRepairFile(hVirus, lpvCookie));
    }
    else if (hVirus->dwFlags & VIT_BOOTPART)
    {
#ifdef SYM_NLM
        return(VS_CANT_REPAIR);
#else
        return(VLRepairBootPart(hVirus, lpvCookie));
#endif
    }
    else
    {
        return(VS_CANT_REPAIR);
    }
}

/*@API:*************************************************************************
@Declaration: VSTATUS WINAPI VLRepairFile(HVLVIRUS hVirus, LPVOID lpvCookie)

@Description:
This routine can be used to repair files infected with viruses.  It will
automatically determine what type of repair is necessary and apply it
(multiple times, if necessary).  The HVLVIRUS handle will be updated to reflect
the repair status of the file.

@Parameters:
$hVirus$ This is a valid HVLVIRUS handle.
$lpvCookie& for NAVEX UI functions - determines whether UI is on or off

@Returns:
This function returns a result of type VSTATUS (type definition found in
AVAPI.H).

    VS_OK           will be returned on success
    VS_INVALID_ARG  will be returned if the context is invalid
    VS_CANT_REPAIR  will be returned if the item cannot be repaired
    VS_MEMALLOC     will be returned if memory cannot be allocated
    VS_FILE_ERROR   will be returned if there is an error accessing the item
    VS_CRIT_ERROR   will be returned on critical error

@Include: avapi.h
*******************************************************************************/

VSTATUS SYM_EXPORT WINAPI VLRepairFile
(
    HVLVIRUS hVirus,                       // [in] a valid HVLVIRUS handle
    LPVOID   lpvCookie
)
{
    UINT        uRepairTries;
    UINT        uResult;
    HVCONTEXT   hContext;
    HVLVIRUS    hTempVirus, hCurVirus;
    VSTATUS     vsScanResult;
    EXTSTATUS   extResult;
    UINT        uAttr;
    BOOL        bFileRepaired;
    FILEOBJECT_T stFileObject;
    FILEINFO_T  stFileInfo;
    HLENG       hLEng = NULL;
    PAMLHANDLE  hLPAM = NULL;

    if (hVirus == NULL || hVirus->lpVContext == NULL)
        return(VS_INVALID_ARG);

    hContext = hVirus->lpVContext;

    SYM_ASSERT(VLFileWrite);
    SYM_ASSERT(VLFileGetDateTime);
    SYM_ASSERT(VLFileSetDateTime);
    SYM_ASSERT(VLFileGetAttr);
    SYM_ASSERT(VLFileSetAttr);

    ///////////////////////////////////////////////////////////////////////////
    // Is the specified HVLVIRUS handle not pointing to a file OR have we already
    // tried to repair the file unsuccessfully?
    ///////////////////////////////////////////////////////////////////////////

    if ((hVirus->dwFlags & VIT_FILE) == 0 ||
        (hVirus->dwFlags & VIT_UNREPAIRABLE))
        return(VS_CANT_REPAIR);

    ///////////////////////////////////////////////////////////////////////////
    // Initialize local avenge engine
    ///////////////////////////////////////////////////////////////////////////

    if (!VLEngLocalInit(hContext, &hLEng))
    {
        return(VS_NO_ACCESS);
    }

    ///////////////////////////////////////////////////////////////////////////
    // Now go into a repair loop until the repairs are done...
    ///////////////////////////////////////////////////////////////////////////

    uRepairTries = MAX_FILE_REPAIR_TRIES;

    hCurVirus = hVirus;

    do
    {
        uResult = FALSE;

        ///////////////////////////////////////////////////////////////////////
        // first attempt an external repair and then go try normal repair if it
        // fails...
        ///////////////////////////////////////////////////////////////////////

        extResult = EXTRepairFile(hContext,
                                  EXTERNAL_DETECT_VERSION,
                                  &hCurVirus->stVirusInfoBlock,
                                  HV_FileInfo_szFileName(hVirus),
                                  (LPBYTE)&hLEng->stAlgScanData,
                                  lpvCookie);

        if (EXTSTATUS_OK == extResult)
        {
            uResult = TRUE;
        }
        else if (EXTSTATUS_FILE_ERROR == extResult)
        {
            uResult = FALSE;
        }

    //////////////////////////////////////////////////////////////////////////
    // PAM (No virus detected with NAV traditional scanner. Try PAM)
    //////////////////////////////////////////////////////////////////////////

        else if ( ((hCurVirus->stVirusInfoBlock).wControl2 & N30_CTRL2_BIT_FLAG2G) &&
                  (NULL != hContext->hGPAM) )
        {
            PAMSTATUS   pamStatus = PAMSTATUS_OK;
            if (FileGetAttr(HV_FileInfo_szFileName(hVirus),&uAttr) ||
                FileSetAttr(HV_FileInfo_szFileName(hVirus),0))
            {
                VLEngLocalClose(hLEng);
                hVirus->dwFlags |= VIT_NO_ACCESS;
                return(VS_FILE_ERROR);
            }

#if defined(AVAPI_MT)
            pamStatus = PAMLocalInit(HVC_szPAMScanDataFile(hContext),
                                     NULL,                  // no swap file!
                                     (PAMGHANDLE)hContext->hGPAM,
                                     &hLPAM);
#else
            hLPAM = hContext->hLPAM;
#endif
            if (pamStatus == PAMSTATUS_OK)
            {
                // successful init, now perform repair!

                BOOL        bResult;
                TCHAR       szBackup[SYM_MAX_PATH];

#if defined(SYM_VXD)
                //convert from the current character set to OEM for BackupInfectedFile()

                TCHAR szInfectedFile[SYM_MAX_PATH];

                VxDGenericStringConvert ( HV_FileInfo_szFileName(hVirus),
                                  CHARSET_CURRENT,
                                  szInfectedFile,
                                  CHARSET_OEM );

                bResult = BackupInfectedFile (szInfectedFile,
                                              _T("PAM"),
                                              szBackup);


#else

                bResult = BackupInfectedFile (HV_FileInfo_szFileName(hVirus),
                                              _T("PAM"),
                                              szBackup);

#endif

                if (FALSE == bResult)
                {
#if defined(AVAPI_MT)
                    PAMLocalClose(hLPAM);
#endif
                    FileSetAttr(HV_FileInfo_szFileName(hVirus),uAttr);

                    uResult = FALSE;
                }
                else
                {
                    // Successfully created the backup file,
                    // so call PAM's repair

#if defined(SYM_VXD)
                    auto char szConvertedBackup[SYM_MAX_PATH * sizeof(WCHAR)];

                    VxDGenericStringConvert ( szBackup,
                                              CHARSET_OEM,
                                              szConvertedBackup,
                                              CHARSET_CURRENT );

                    pamStatus = PAMRepairFile(
                                    hLPAM,
                                    HVC_szPAMScanDataFile(hContext),
                                    szConvertedBackup,               // temp swap file
                                    HV_FileInfo_szFileName(hVirus), // infected program
                                    (hCurVirus->stVirusInfoBlock).wBookMark, // which repair #?
                                    FALSE);                         // no SYS override

#else
                    pamStatus = PAMRepairFile(
                                    hLPAM,
                                    HVC_szPAMScanDataFile(hContext),
                                    szBackup,                       // temp swap file
                                    HV_FileInfo_szFileName(hVirus), // infected program
                                    (hCurVirus->stVirusInfoBlock).wBookMark, // which repair #?
                                    FALSE);                         // no SYS override

#endif

                    // PAMSTATUS_NO_REPAIR or PAMSTATUS_*_ERROR

                    if (PAMSTATUS_OK != pamStatus)
                    {
                        uResult  = FALSE;
                    }
                    else
                    {
                        uResult = TRUE;
                    }

#if defined(AVAPI_MT)
                    PAMLocalClose(hLPAM);
#endif

#if defined(SYM_VXD)
                    VLFileDelete(szConvertedBackup);
#else
                    VLFileDelete(szBackup);
#endif

                    // reset the attributes when we're done and kill our temp file!

                    if(FileSetAttr(HV_FileInfo_szFileName(hVirus),uAttr))
                    {
                        VLEngLocalClose(hLEng);
                        hVirus->dwFlags |= VIT_NO_ACCESS;
                        return(VS_FILE_ERROR);
                    }
                }
            }
        }

    //////////////////////////////////////////////////////////////////////////
    // Try AVENGE if repair has failed with navex and PAM
    //////////////////////////////////////////////////////////////////////////

        else // if (EXTSTATUS_NO_REPAIR == extResult)
        {

    //////////////////////////////////////////////////////////////////////////
    // create a file object for AVENGE and open the file
    //////////////////////////////////////////////////////////////////////////

            STRCPY(stFileInfo.lpstrPathName,HV_FileInfo_szFileName(hVirus));
            stFileObject.lpParent = NULL;
            stFileObject.lpvInfo = &stFileInfo;
            stFileObject.lpszName = HV_FileInfo_szFileName(hVirus);
            stFileObject.lpCallBacks = hContext->
                                       lpGlobalCallBacks->
                                       lpGeneralCallBacks->
                                       lpFileCallBacks;

            if (FileGetAttr(HV_FileInfo_szFileName(hVirus),&uAttr) ||
                FileSetAttr(HV_FileInfo_szFileName(hVirus),0))
            {
                VLEngLocalClose(hLEng);
                hVirus->dwFlags |= VIT_NO_ACCESS;
                return(VS_FILE_ERROR);
            }

            if (EngRepairFile(hLEng,
                              hCurVirus->uDBEntryNum,
                              &stFileObject,
                              &bFileRepaired) == ENGSTATUS_ERROR)
            {

                uResult = FALSE;
            }
            else
            {
                if (bFileRepaired == TRUE)
                    uResult = TRUE;
                else
                    uResult = FALSE;
            }

                if(FileSetAttr(HV_FileInfo_szFileName(hVirus),uAttr))
            {
                VLEngLocalClose(hLEng);
                hVirus->dwFlags |= VIT_NO_ACCESS;
                return(VS_FILE_ERROR);
            }
        }

        ///////////////////////////////////////////////////////////////////////
        // Check to see if the repair succeeded
        ///////////////////////////////////////////////////////////////////////

        if (uResult == TRUE)            // succesful repair so far...
        {
            vsScanResult = VLScanFile(hContext,
                                      HV_FileInfo_szFileName(hVirus),
                                      &hTempVirus);

            if (vsScanResult == VS_OK )
            {
                if ( hTempVirus == NULL )
                {
                    hVirus->dwFlags |= VIT_REPAIRED;
                    hVirus->dwFlags &= ~((DWORD)VIT_HAS_A_VIRUS);
                    VLEngLocalClose(hLEng);
                    return(VS_OK);
                }
                else
                {
                    hCurVirus->hNext = hTempVirus;
                    hCurVirus        = hCurVirus->hNext;
                }
            }
            else if ( vsScanResult == VS_NO_ACCESS)
            {
                if (hTempVirus)
                    VLReleaseVirusHandle(hTempVirus);
                hVirus->dwFlags |=  ( VIT_NO_ACCESS | VIT_UNREPAIRABLE );
                VLEngLocalClose(hLEng);
                return (VS_NO_ACCESS);
            }
            else
            {
                hVirus->dwFlags |=  VIT_UNREPAIRABLE;
                VLEngLocalClose(hLEng);
                return(vsScanResult);
            }
        }
        else
        {
            hVirus->dwFlags |= VIT_UNREPAIRABLE;
            VLEngLocalClose(hLEng);
            return(VS_CANT_REPAIR);
        }

        uRepairTries --;

    } while (uRepairTries > 0);


    hVirus->dwFlags |= VIT_UNREPAIRABLE;
    VLEngLocalClose(hLEng);
    return(VS_CANT_REPAIR);

}


/*@API:*************************************************************************
@Declaration: VSTATUS WINAPI VLDeleteFile(HVLVIRUS hVirus)

@Description:
This routine can be used to delete (and wipe) a file infected with a virus.
The HVLVIRUS handle will be updated to reflect the deletion status of the file.

@Parameters:
$hVirus$ This is a valid HVLVIRUS handle.

@Returns:

This function returns a result of type VSTATUS (type definition found in
AVAPI.H).

    VS_OK           will be returned on success
    VS_CANT_DELETE  will be returned if the file should/could not be deleted
    VS_FILE_ERROR   will be returned if there is a file error

@Include: avapi.h
*******************************************************************************/

VSTATUS SYM_EXPORT WINAPI VLDeleteFile
(
    HVLVIRUS hVirus                       // [in] a valid HVLVIRUS handle
)
{
    UINT        bSuccess;
    HVCONTEXT   hContext;

    if (hVirus == NULL)
        return(VS_INVALID_ARG);

    hContext = hVirus->lpVContext;

    SYM_ASSERT(VLFileDelete);
    SYM_ASSERT(VLFileGetAttr);
                                        // Check to see if we can delete the file
    if (!VLCanDeleteItem(hVirus))
        return(VS_CANT_DELETE);

    bSuccess = VLFileDelete(HV_FileInfo_szFileName(hVirus));

    if ( bSuccess )
        {
        hVirus->dwFlags |= VIT_DELETED;
        hVirus->dwFlags &= ~((DWORD)VIT_HAS_A_VIRUS);

        return(VS_OK);
        }
    else
        hVirus->dwFlags |= VIT_CANNOT_DELETE;


    return(VS_FILE_ERROR);
}

/*@API:*************************************************************************
@Declaration:  VSTATUS WINAPI VLReleaseVirusHandle(HVLVIRUS hInfo)

@Description:
This routine is used to release a handle to a virus which was obtained during
scanning.

@Parameters:
$hInfo$ This is a virus handle.

@Returns:
This function returns a result of type VSTATUS (type definition found in
AVAPI.H).

    VS_OK           will be returned on success
    VS_INVALID_ARG  will be returned if the context is invalid

@Include: avapi.h
*******************************************************************************/

VSTATUS SYM_EXPORT WINAPI VLReleaseVirusHandle
(
    HVLVIRUS hInfo                        // [in] a valid HVLVIRUS handle
)
{
    VOID (WINAPI *PermMemoryFree)(LPVOID lpBuffer);

    if (hInfo != NULL)
    {
       VLReleaseVirusHandle(hInfo->hNext);
       PermMemoryFree = hInfo->lpVContext->lpstCallBack->PermMemoryFree;
       hInfo->lpVContext = NULL;
       PermMemoryFree(hInfo);
    }

    return(VS_OK);
}

/*@API:*************************************************************************
@Declaration: BOOL WINAPI VLCanRepairItem(HVLVIRUS hVirus)


@Description:
This function determines whether or not the specified item can be repaired.
Disembodied items, already repaired items, deleted items, and inaccessable
items cannot be repaired.

@Parameters:
$hVirus$ This is a valid HVLVIRUS handle.

@Returns:
    This function returns

        TRUE        if the specified item has a "chance" of being repaired.

        FALSE       if there is no way the specified item could be repaired.

@Include: avapi.h
*******************************************************************************/
BOOL SYM_EXPORT WINAPI VLCanRepairItem
(
    HVLVIRUS hVirus                     // [in] a valid HVLVIRUS handle
)
{
    ///////////////////////////////////////////////////////////////////////////
    // for now don't allow DIR2 repairs...
    ///////////////////////////////////////////////////////////////////////////

    if ( (hVirus->stVirusInfoBlock.wControl & bitCTRL_NOTCLEAN) ||
         ((hVirus->stVirusInfoBlock.wInfo & bitINFO_DIR2) &&
           !(hVirus->dwPOFlags & fPO_DIR2_REPAIR)) )
        return(FALSE);

    ///////////////////////////////////////////////////////////////////////////
    // already tried repairing this file!
    ///////////////////////////////////////////////////////////////////////////

    if (hVirus->dwFlags & (VIT_UNREPAIRABLE | VIT_DISEMBODIED | VIT_DELETED
                            | VIT_NO_ACCESS))
        return(FALSE);

    if (hVirus->dwFlags & (VIT_FILE | VIT_BOOTPART))
        return(TRUE);

    return(FALSE);
}


/*@API:*************************************************************************
@Declaration: BOOL WINAPI VLCanDeleteItem(HVLVIRUS hVirus)


@Description:
This function determines whether or not the specified item can be deleted.
Disembodied items, already deleted items, and inaccessable items cannot be
deleted.  The DIR2 virus is an example of a virus which should not be
deleted from infected files.

@Parameters:
$hVirus$ This is a valid HVLVIRUS handle.

@Returns:
    This function returns

        TRUE        if the specified item may be deleted.

        FALSE       if the specified item cannot or should not be deleted

@Include: avapi.h
*******************************************************************************/

BOOL SYM_EXPORT WINAPI VLCanDeleteItem
(
    HVLVIRUS hVirus                       // [in] a valid HVLVIRUS handle
)
{
    if ( (hVirus->stVirusInfoBlock.wInfo & bitINFO_DIR2) &&
         !(hVirus->dwPOFlags & fPO_DIR2_DELETE) )
        return(FALSE);

    if (hVirus->dwFlags & (VIT_DELETED | VIT_NO_ACCESS | VIT_DISEMBODIED))
        return(FALSE);

    if (hVirus->dwFlags & VIT_FILE)
        return(TRUE);

    return(FALSE);
}


BOOL LOCAL VLGetVirusText (
    HVCONTEXT   hContext,
    UINT        uDBEntryNum,
    LPTSTR      lpszVirusText )
{
    TCHAR   szAlias[SIZE_ALIAS+1];      // 1 for EOS

    return(VLGetVirusInfo(hContext,
                          uDBEntryNum,
                          szAlias,
                          lpszVirusText));
}

BOOL LOCAL VLGetVirusAlias (
    HVCONTEXT   hContext,
    UINT        uDBEntryNum,
    LPTSTR      lpszVirusAlias )
{
    TCHAR   szText[SIZE_COMMENT];      // EOS included in SIZE_COMMENT

    return(VLGetVirusInfo(hContext,
                          uDBEntryNum,
                          lpszVirusAlias,
                          szText));
}

//---------------------------------------------------------------------------
//
// BOOL WINAPI BackupInfectedFile (
//      LPTSTR  lpszInfectedFile,       // Infected file (Input)
//      LPTSTR  lpszBackupExtension,    // Extension for backup file (Input)
//      LPTSTR  lpszBackupFile          // Name of backup file (Output)
//      );
//
// This function backs up an infected file, applying the extension
// specified by the user.
//
// Entry:
//      lpszInfectedFile
//      lpszBackupExtension
//      lpszBackupFile
//
// Exit:
//      TRUE  if the backup was successful.
//      FALSE if the backup was not successful.
//
// Note:
//      SYM_VXD: all strings in OEM!
//      Some of the code in this function was derived from
//      BackupInfectedFile().
//
//---------------------------------------------------------------------------
BOOL WINAPI BackupInfectedFile (LPTSTR lpszInfectedFile,
                                LPTSTR lpszBackupExtension,
                                LPTSTR lpszBackupFile)
    {

    if ( BuildUniqueFileName ( lpszInfectedFile,
                               lpszBackupExtension,
                               lpszBackupFile ) )
        {
#if defined(SYM_VXD)
        auto char szInfected[SYM_MAX_PATH*sizeof(WCHAR)];
        auto char szBackup[SYM_MAX_PATH*sizeof(WCHAR)];

        VxDGenericStringConvert ( lpszInfectedFile,
                                  CHARSET_OEM,
                                  szInfected,
                                  CHARSET_CURRENT );

        VxDGenericStringConvert ( lpszBackupFile,
                                  CHARSET_OEM,
                                  szBackup,
                                  CHARSET_CURRENT );

        return ( !FileCopy(szInfected, szBackup, NULL) );
#else
        return ( !FileCopy(lpszInfectedFile, lpszBackupFile, NULL) );
#endif
        }

    return ( FALSE );
    }


//---------------------------------------------------------------------------
//
// BOOL WINAPI BuildUniqueFileName (
//      LPCTSTR lpszBaseFile,
//      LPCTSTR lpszExtension,
//      LPTSTR  lpszUniqueFile
//      );
//
// This function builds a new filename based on the filename specified by
// lpszBaseFile.  First, this function will replace the extension of the
// base filename with lpszExtension.  If the produced file exists, then
// this function will start increasing numbers at the end of the new
// filename's extension until a unique filename is found, or until it runs
// out of numbers.
//
// Entry:
//      lpszBaseFile   - base filename
//      lpszExtension  - extension to replace lpszBaseFile's extension with;
//                       if NULL, lpszBaseFile's extension will be preserved
//      lpszUniqueFile - buffer to place the resulting filename in;
//                       SYM_MAX_PATH * sizeof(TCHAR) bytes long
//
// Exit:
//      TRUE  if unique filename was created in lpszUniqueFile
//      FALSE if error (the contents of lpszUniqueFile is undefined)
//
// Note:
//      SYM_VXD: all strings in OEM!
//      Some of the code in this function was derived from
//      BackupInfectedFile().
//
//---------------------------------------------------------------------------
BOOL WINAPI BuildUniqueFileName (
    LPCTSTR lpszBaseFile,
    LPCTSTR lpszExtension,
    LPTSTR  lpszUniqueFile
    )
    {
    auto UINT  i;
    auto int   nExtLen, nCountLen, nExtensionLength;
    auto TCHAR szCount[10];
    auto TCHAR szNewExtension[SYM_MAX_PATH+1];
    auto TCHAR szExtension[SYM_MAX_PATH];

                                        // Validate input parameters

    SYM_ASSERT ( lpszBaseFile );
    SYM_VERIFY_STRING ( lpszBaseFile );
    SYM_ASSERT ( lpszUniqueFile );
    SYM_VERIFY_BUFFER ( lpszUniqueFile, SYM_MAX_PATH * sizeof(TCHAR) );

                                        // Create a valid extension in
                                        // szExtension.  This is based on
                                        // what the user gave us in
                                        // lpszExtension.

    if ( lpszExtension )
        {
        SYM_VERIFY_STRING ( lpszExtension );

        if ( STRLEN ( lpszExtension ) >= SYM_MAX_PATH )
            {
            return ( FALSE );
            }

        STRCPY ( szExtension, lpszExtension );
        }
    else
        {
        if ( NameReturnExtensionEx ( lpszBaseFile,
                                     szExtension,
                                     SYM_MAX_PATH ) != NOERR )
            {
            return ( FALSE );
            }
        }

    nExtensionLength = STRLEN ( szExtension );

                                        // Create initial unique filename

    STRCPY ( lpszUniqueFile, lpszBaseFile );

    NameStripExtension ( lpszUniqueFile );

    if ( STRLEN ( lpszUniqueFile ) +
         ( nExtensionLength ? nExtensionLength + STRLEN ( _T(".") ) : 0 ) >=
         SYM_MAX_PATH
       )
        {
        return ( FALSE );
        }

    NameAppendExtension ( lpszUniqueFile, szExtension );

    SYM_ASSERT ( STRLEN ( lpszUniqueFile ) < SYM_MAX_PATH );

                                        // Adjust nExtensionLength to contain
                                        // not the current length of
                                        // szExtension, but the minimum
                                        // length of the extension of the
                                        // produced unique filename.  The
                                        // minimum length is determined by
                                        // the length of the largest number
                                        // in an extension that describes a
                                        // unique file.

    if ( nExtensionLength < 3 )
        {
        nExtensionLength = 3;
        }

                                        // Start the loop for searching for
                                        // unique filename.

    for ( i = 0; i <= 999; i++ )
        {
                                        // Check if the current filename
                                        // exists.  If it doesn't, then
                                        // return success.

#if defined(SYM_VXD)
        auto char szUniqueFile[SYM_MAX_PATH*sizeof(WCHAR)];

        VxDGenericStringConvert ( lpszUniqueFile,
                                  CHARSET_OEM,
                                  szUniqueFile,
                                  CHARSET_CURRENT );

        if ( !FileExists ( szUniqueFile ) )
#else
        if ( !FileExists ( lpszUniqueFile ) )
#endif
            {
            return ( TRUE );
            }


                                        // If the current filename exists,
                                        // then generate a number to replace
                                        // part of its extension.

        _ConvertWordToString ( i, szCount, 10, 0 );
        nCountLen = STRLEN ( szCount );

                                        // Validate that the textual value of
                                        // i can fit in a buffer
                                        // nExtensionLength characters long.

        SYM_ASSERT ( nCountLen <= nExtensionLength );

                                        // Copy the first few szExtension
                                        // characters to szNewExtension, such
                                        // that after appending the newly
                                        // generated number to
                                        // szNewExtension, the produced
                                        // string is not longer than
                                        // szExtension.

        STRNCPY ( szNewExtension, szExtension, nExtensionLength - nCountLen );
        szNewExtension[nExtensionLength - nCountLen] = EOS;

                                        // Pad szNewExtension with 0's if the
                                        // newly generated string (after
                                        // appending the number) would
                                        // contain less than nExtensionLength
                                        // characters.

        nExtLen = STRLEN ( szNewExtension );

        if ( nCountLen + nExtLen < nExtensionLength )
            {
            StringAppendNChars ( szNewExtension,
                                 _T('0'),
                                 nExtensionLength - ( nCountLen + nExtLen ) );

            nExtLen = STRLEN ( szNewExtension );
            }

                                        // Concatenate szNewExtension with
                                        // the new number.

        STRCAT ( szNewExtension, szCount );

        SYM_ASSERT ( STRLEN ( szNewExtension ) == (size_t)nExtensionLength );

                                        // Replace the old extension of the
                                        // unique file with the new
                                        // extension.

        NameStripExtension ( lpszUniqueFile );

        if ( STRLEN ( lpszUniqueFile ) +
             ( nExtLen ? nExtLen + STRLEN ( _T(".") ) : 0 ) >=
             SYM_MAX_PATH
           )
            {
            return ( FALSE );
            }

        NameAppendExtension ( lpszUniqueFile, szNewExtension );
        }

    return ( FALSE );
    }

// ==========================================================================
#ifndef SYM_NLM // beginning of all blocks of ifndef SYM_NLM
// ==========================================================================

/*@API:*************************************************************************
@Declaration: BOOL WINAPI VLDefInfo(HVLVIRUSENTRY hVirus, VLVIRINFO enumVirInfo,
LPTSTR lpszResultBuffer)

@Description:
This routine can be used to obtain virus information about the specified
HVLVIRUSENTRY handle.  This routine will return TRUE/FALSE for all boolean
information requests, and fill the lpszResultBuffer for all other requests.

@Parameters:
$hVirus$ This is a valid HVLVIRUSENTRY handle.

$enumVirInfo$ This is a value of type VLVIRINFO. (See AVAPI.H) for possible
arguments.  Exaples:

        VI_VIRUS_NAME,                  // obtain virus's name
        VI_VIRUS_ALIAS,                 // obtain virus's alias list
        VI_VIRUS_INFO,                  // obtain virus information
        VI_VIRUS_SIZE,                  // obtain virus's size

        VI_BOOL_INFECT_FILES,           // does this virus infect FILES in general? (MAC too)
        VI_BOOL_INFECT_EXE,             // does this virus infect EXE's?
        VI_BOOL_INFECT_COM,             // does this virus infect COM's?
        VI_BOOL_INFECT_COMCOM,          // does this virus infect COMMAND.COM?
        VI_BOOL_INFECT_SYS,             // does this virus infect  SYS's?
        VI_BOOL_INFECT_MBR,             // does this virus infect MBR's?
        VI_BOOL_INFECT_FLOPPY_BOOT,     // does this virus infect floppies?
        VI_BOOL_INFECT_HD_BOOT,         // does this virus infect HD bootsecs?

        VI_BOOL_INFECT_WILD,            // is this virus in the wild?

        VI_BOOL_MEMORY_RES,             // can this virus go memory resident?
        VI_BOOL_SIZE_STEALTH,           // does this virus size stealth?
        VI_BOOL_FULL_STEALTH,           // does this virus full stealth?
        VI_BOOL_TRIGGER,                // does this virus have a trigger?
        VI_BOOL_ENCRYPTING,             // is this virus encrypting?
        VI_BOOL_POLYMORPHIC,            // is this virus polymorphic?
        VI_BOOL_MULTIPART,              // is this virus multipartite?
        VI_BOOL_MAC_VIRUS,              // macintosh virus
        VI_BOOL_MACRO_VIRUS,            // macro virus
        VI_BOOL_WINDOWS_VIRUS,          // windows virus
        VI_BOOL_AGENT_VIRUS,            // agent virus

        ///////////////////////////////////////////////////////////////////////
        // item specific queries
        ///////////////////////////////////////////////////////////////////////

        VI_BOOL_DEF_DELETED,            // def been deleted from VIRSCAN.DAT?
        VI_BOOL_INFO_VALID              // is this information valid?


$lpszResultBuffer$ This is a far pointer to a buffer where numeric and textual
information results are placed.  All non BOOLEAN numerical/textual results are
converted to strings and placed in this buffer.

@Returns:

    This function returns:

        TRUE            if the query is a boolean query and the result is TRUE
                        OR if the query is a non-boolean query and the
                        information has been successfully retrieved.

        FALSE           if the query is a boolean query and the result is FALSE
                        OR if the  query is a non-boolean query and the
                        information has not been retrieved.

@Include: avapi.h
*******************************************************************************/
BOOL SYM_EXPORT WINAPI VLDefInfo
(
    HVLVIRUSENTRY hVirusEntry,          // [in] a valid HVLVIRUS handle
    VLVIRINFO     enumVirInfo,          // [in] which information to retrieve
    LPTSTR        lpszResultBuffer      // [out] place results in this buffer
)
{
    HVCONTEXT       hContext;
    VIRINFOTYPE     stVirInfo;

    if (hVirusEntry == NULL)
        return(FALSE);

    hContext = hVirusEntry->lpVContext;

    stVirInfo.lpVContext            = hContext;
    stVirInfo.uDBEntryNum           = hVirusEntry->uDBEntryNum;
    stVirInfo.stVirusInfoBlock      = hVirusEntry->stVirusInfoBlock;
    stVirInfo.uDate                 = 0;
    stVirInfo.uTime                 = 0;
    stVirInfo.dwFlags               = VIT_VALID_N30;

    switch (enumVirInfo)
    {
        case VI_BOOL_DEF_DELETED:
            if (hVirusEntry->wFlags & VET_DEF_DELETED)
                return(TRUE);
            else
                return(FALSE);

        default:
            return(VLVirusInfo(&stVirInfo,0,enumVirInfo,lpszResultBuffer));
    }
}


/*@API:*************************************************************************
@Declaration: VSTATUS WINAPI VLOverrideDIR2RepairProtection (HVLVIRUS hVirus)

@Description:
This routine is used to force VLCanRepairItem() and VLRepairFile() to allow
repair of DIR2 infections.  Use this function only when you are sure that
the repair will not destroy any data.

@Parameters:
$hVirus$ This is a valid HVLVIRUS handle.  If this handle does not point to
a DIR2 virus, the function will still return VS_OK.

@Returns:

This function returns a result of type VSTATUS (type definition found in
AVAPI.H).

    VS_OK           will be returned on success

@Include: avapi.h
*******************************************************************************/


VSTATUS SYM_EXPORT WINAPI VLOverrideDIR2RepairProtection
(
    HVLVIRUS hVirus                       // [in] a valid HVLVIRUS handle
)
    {
                                        // If hVirus does not point to a
                                        // DIR2 virus, return VS_OK

    if (!(hVirus->stVirusInfoBlock.wInfo & bitINFO_DIR2))
        return(VS_OK);

                                        // Set flags to force DIR2 repair

    hVirus->dwPOFlags |= fPO_DIR2_REPAIR;

                                        // Return to caller

    return(VS_OK);
    }


/*@API:*************************************************************************
@Declaration: VSTATUS WINAPI VLOverrideDIR2DeleteProtection (HVLVIRUS hVirus)

@Description:
This routine is used to force VLCanDeleteItem() and VLDeleteFile() to allow
deletion of DIR2 infections.  Use this function only when you are sure that
the deletion will not destroy any data.

@Parameters:
$hVirus$ This is a valid HVLVIRUS handle.  If this handle does not point to
a DIR2 virus, the function will still return VS_OK.

@Returns:

This function returns a result of type VSTATUS (type definition found in
AVAPI.H).

    VS_OK           will be returned on success

@Include: avapi.h
*******************************************************************************/


VSTATUS SYM_EXPORT WINAPI VLOverrideDIR2DeleteProtection
(
    HVLVIRUS hVirus                       // [in] a valid HVLVIRUS handle
)
    {
                                        // If hVirus does not point to a
                                        // DIR2 virus, return VS_OK

    if (!(hVirus->stVirusInfoBlock.wInfo & bitINFO_DIR2))
        return(VS_OK);

                                        // Set flags to force DIR2 repair

    hVirus->dwPOFlags |= fPO_DIR2_DELETE;

                                        // Return to caller

    return(VS_OK);
    }


/*@API:*************************************************************************
@Declaration: UINT WINAPI   VLNumVirusEntries( HVCONTEXT hvContext )

@Description:
This routine can be used to determine how many viruses there are in the
VIRSCAN.DAT database. This is a conceptual number of viruses, and may be
well less than the number of definitions. A virus may have several definitions,
but only be counted as one virus. This number is put in to the header from
the file vircount.dat. THE RESULT OF THIS FUNCTION SHOULD BE USED FOR DISPLAY
PURPOSES ONLY!

@Parameters:
$hContext$ This is a valid handle to a context provided by VLScanInit.

@Returns:
This routine returns the number of virus definitions in the database.

@Include: avapi.h
*******************************************************************************/


DWORD SYM_EXPORT WINAPI VLNumClaimedVirusEntries
(
    HVCONTEXT hContext                  // [in] a valid HVCONTEXT handle
)
{
    DWORD dwNumVirusRecords;

if (EngGetNumClaimedViruses(hContext->hGEng,
                                &dwNumVirusRecords) != ENGSTATUS_OK)
    {
        dwNumVirusRecords = 0;
    }

    return (dwNumVirusRecords);
}

#endif      // #ifndef SYM_NLM

#ifndef SYM_NLM

/*@API:*************************************************************************
@Declaration: VSTATUS WINAPI VLRepairBootPart(HVLVIRUS hVirus, LPVOID lpvCookie)

@Description:
This routine can be used to repair boot sectors and MBRs infected with
viruses.  It will automatically determine what type of repair is necessary
and apply it (multiple times, if necessary).  The HVLVIRUS handle will be updated to reflect
the repair status of the item.

@Parameters:
$hVirus$ This is a valid HVLVIRUS handle.
$lpvCookie& for NAVEX UI functions - determines whether UI is on or off

@Returns:
This function returns a result of type VSTATUS (type definition found in
AVAPI.H).

    VS_OK           will be returned on success
    VS_INVALID_ARG  will be returned if the context is invalid
    VS_CANT_REPAIR  will be returned if the item cannot be repaired
    VS_MEMALLOC     will be returned if memory cannot be allocated
    VS_DRIVE_ERROR  will be returned on a drive access error
    VS_CRIT_ERROR   will be returned on critical error

@Include: avapi.h
*******************************************************************************/

VSTATUS SYM_EXPORT WINAPI VLRepairBootPart
(
    HVLVIRUS hVirus,                       // [in] a valid HVLVIRUS handle
    LPVOID   lpvCookie
)
{
    UINT        uRepairTries;
    UINT        uResult;
    VSTATUS     vsResult;
    HVCONTEXT   hContext;
    HVLVIRUS    hTempVirus, hCurrent;
    EXTSTATUS   extResult;
    BOOL        bBootRepaired;
    HLENG       hLEng = NULL;
    LINEAROBJECT rLinear;

    if (hVirus == NULL || hVirus->lpVContext == NULL)
        return(VS_INVALID_ARG);

    hContext = hVirus->lpVContext;

    rLinear.lpvInfo     = (LPVOID)hVirus->BootInfo.byDriveNum;
    rLinear.lpCallBacks = hContext->
                          lpGlobalCallBacks->
                          lpGeneralCallBacks->
                          lpLinearCallBacks;

    ///////////////////////////////////////////////////////////////////////////
    // Is the specified HVLVIRUS handle not pointing to a file OR have we already
    // tried to repair the file unsuccessfully?
    ///////////////////////////////////////////////////////////////////////////

    if ((hVirus->dwFlags & VIT_BOOTPART) == 0 ||
        (hVirus->dwFlags & VIT_UNREPAIRABLE))
        return(VS_CANT_REPAIR);

    if (!VLEngLocalInit(hContext, &hLEng))
    {
        return(VS_NO_ACCESS);
    }

    uRepairTries = MAX_BOOT_REPAIR_TRIES;
    hCurrent = hVirus;
    do
    {
        if (hCurrent->BootInfo.byBootFlags & BI_BOOT_SECTOR)
        {
            WORD wCylinder = 0;
            BYTE bySector = 1, byHead = 0, bySystemType;

            if (hVirus->BootInfo.byDriveNum >= 0x80)
            {
                // read in the MBR so we can determine where the BS resides

                uResult = VLSDiskReadPhysical((LPBYTE)&hLEng->stAlgScanData,
                                              1,
                                              0,
                                              1,
                                              0,
                                              hVirus->BootInfo.byDriveNum);
                BOOL bPartitioned;

                CBSTATUS cbStatus = rLinear.lpCallBacks->LinearIsPartitionedMedia((LPVOID)hVirus->BootInfo.byDriveNum, &bPartitioned);

                if (uResult != 0 || cbStatus != CBSTATUS_OK)
                {
                    // failure
                    wCylinder = 0xFFFF;
                    bySector = byHead = 0xFF;
                }
                else if (bPartitioned)
                {
                    VLGetBootSectorLocation (hContext,
                                             hVirus->BootInfo.byDriveNum,
                                             (LPBYTE)&hLEng->stAlgScanData,
                                             (UINT)hVirus->BootInfo.byBootSecNum,
                                             &wCylinder,
                                             &bySector,
                                             &byHead,
                                             &bySystemType);
                }
            }

            extResult = EXTRepairBoot(hContext,
                                      EXTERNAL_DETECT_VERSION,
                                      &hCurrent->stVirusInfoBlock,
                                      hVirus->BootInfo.byDriveNum,
                                      hVirus->BootInfo.byBootSecNum,
                                      bySector,wCylinder,byHead,
                                      (LPBYTE)&hLEng->stAlgScanData,
                                      lpvCookie);
        }
        else if (hCurrent->BootInfo.byBootFlags & BI_PARTITION_SECTOR)
            extResult = EXTRepairPart(hContext,
                                      EXTERNAL_DETECT_VERSION,
                                      &hCurrent->stVirusInfoBlock,
                                      hVirus->BootInfo.byDriveNum,
                                      (LPBYTE)&hLEng->stAlgScanData,
                                      lpvCookie);
        else
            extResult = EXTSTATUS_NO_REPAIR;

        if (EXTSTATUS_OK == extResult)
        {
            uResult = TRUE;
        }
        else //if (EXTSTATUS_NO_REPAIR == extResult)
        {

            if (EngRepairBoot (hLEng,
                               hVirus->uDBEntryNum,
                               &rLinear,
                               hVirus->BootInfo.byBootSecNum,
                               &bBootRepaired) == ENGSTATUS_ERROR)
            {

                uResult = FALSE;
            }
            else
            {
                uResult = bBootRepaired;
            }

        }

        ///////////////////////////////////////////////////////////////////////
        // check to see if the repair failed?!?
        ///////////////////////////////////////////////////////////////////////

        if (uResult == FALSE)
        {
            hVirus->dwFlags |= VIT_UNREPAIRABLE;
            VLEngLocalClose(hLEng);
            return(VS_CANT_REPAIR);
        }

        if (hVirus->BootInfo.byBootFlags & BI_BOOT_SECTOR)

            vsResult =   VLScanBoot(hContext,
                                    hVirus->BootInfo.byDriveNum,
                                    hVirus->BootInfo.byBootSecNum,
                                    NULL,
                                    &hTempVirus);
        else
            vsResult = VLScanPart(hContext,
                                  hVirus->BootInfo.byDriveNum,
                                  NULL,
                                  &hTempVirus);
        if (vsResult == VS_OK)
        {
            if ( hTempVirus == NULL)
            {
                hVirus->dwFlags |= VIT_REPAIRED;
                hVirus->dwFlags &= ~((DWORD)VIT_HAS_A_VIRUS);
                VLEngLocalClose(hLEng);
                return(VS_OK);
            }
            else
            {
                hCurrent->hNext = hTempVirus;
                hCurrent        = hCurrent->hNext;
            }
        }
        else if (vsResult == VS_NO_ACCESS)
        {
            if (hTempVirus)
            {
                VLReleaseVirusHandle(hTempVirus);
            }
            hVirus->dwFlags |=  ( VIT_NO_ACCESS | VIT_UNREPAIRABLE );
            VLEngLocalClose(hLEng);
            return(VS_NO_ACCESS);
        }
        else
        {
            hVirus->dwFlags |= VIT_UNREPAIRABLE;
            VLEngLocalClose(hLEng);
            return(vsResult);
        }

        uRepairTries--;
    } while (uRepairTries > 0);

    hVirus->dwFlags |= VIT_UNREPAIRABLE;

    VLEngLocalClose(hLEng);
    return(VS_CANT_REPAIR);
}

#endif      // #ifndef SYM_NLM


#ifndef SYM_NLM

/*@API:*************************************************************************
@Declaration:

VSTATUS SYM_EXPORT WINAPI VLScanBoot ( HVCONTEXT  hContext, BYTE byDrive,
BYTE byBootSecNum, LPBYTE lpBooBuffer, LPHVLVIRUS lphVirus )

@Description:
If lpBooBuffer == NULL:
    This routine scans the boot sector of the specified partition on the
    specified drive.

If lpBooBuffer != NULL:
    This routine scans the sector buffer pointed to by lpBooBuffer for Boot
    sector viruses.  HVLVIRUS handle will indicate that this is a "disembodied"
    infection and no repair can be applied to the specified handle.

    A valid HVLVIRUS handle is returned in lphVirus if a virus is found.

@Parameters:
$hContext$ This is a valid handle to a context provided by VLScanInit.

$byDrive$ This is the physical drive number to scan. 0x00=A:, 0x80=first
physical drive.

$byBootSecNum$ This is the partition number (0 to 3) which contains the boot
sector to scan on the specified physical drive.

$lpBooBuffer$ This is a far pointer to a sector-sized buffer which contains
a boot sector image.  If lpBooBuffer != NULL, then byDrive and byBootSecNum
are ignored, otherwise lpBooBuffer is ignored.

$lphVirus$ This is a pointer to HVLVIRUS handle, and is set by this function
to a non-NULL value if a virus was found, otherwise it is set to NULL.
Information can be obtained about the handle by calling VLVirusInfo.

@Returns:
This function returns
     VS_OK if scanning was successfully completed whether or not a virus
           was found. A non-NULL value is lphVirus indicates infection.

     on error:
         VS_INVALID_ARG
         VS_MEMALLOC
         VS_NO_ACCESS
         VS_ENTRY_UNAVAIL

@Include: avapi.h
*******************************************************************************/

VSTATUS SYM_EXPORT WINAPI VLScanBoot
(
    HVCONTEXT  hContext,                // [in] a valid HVCONTEXT handle
    BYTE       byDrive,                 // [in] physical drive # to scan
    BYTE       byBootSecNum,            // [in] partition # to scan (0-3)
    LPBYTE     lpBooBuffer,             // [in] disembodied buffer to scan
    LPHVLVIRUS   lphVirus

)
{
    LPBYTE      lpImage;
    DWORD       dwImageSize;
    WORD        wResult;
    WORD        wCylinder = 0;
    BYTE        bySector = 1;
    BYTE        byHead = 0;
    BYTE        bySystemType;
    HVLVIRUS    hVirus;
    BOOL        bDisembodied;
    BOOL        bFoundVirus;
    UINT        uResult;
    HLENG       hLEng = NULL;

    SYM_ASSERT(VLSDiskReadPhysical);
    *lphVirus = NULL;

    //////////////////////////////////////////////////////////////////////////
    // Verify the structure pointer and its signature
    //////////////////////////////////////////////////////////////////////////

    if (!VLValidContext(hContext))
        return(VS_INVALID_ARG);

    dwImageSize = lpBooBuffer == NULL ? SECTOR_SIZE * 8 : SECTOR_SIZE;

    if (lpBooBuffer == NULL)
    {
        ///////////////////////////////////////////////////////////////////////
        // Allocate memory for the boot sector image
        ///////////////////////////////////////////////////////////////////////

        lpImage = (LPBYTE)VLTempMemoryAlloc(dwImageSize);
        if (lpImage == NULL)
            return(VS_MEMALLOC );

        ///////////////////////////////////////////////////////////////////////
        // Get the partition table so we can determine where boot sectors are!
        // If this is a floppy drive, the sector we retrieve is the boot sector
        ///////////////////////////////////////////////////////////////////////
        //
        // 1) Get the drive type (on floppies, we don't have a partition table)
        // 2) Read 1 sec from Track 0, Sector 1, Side 0 from the proper drive
        //
        ///////////////////////////////////////////////////////////////////////

        uResult = VLSDiskReadPhysical(lpImage, 1, 0, 1, 0, byDrive);

        ///////////////////////////////////////////////////////////////////////
        // uResult == 0 on success
        ///////////////////////////////////////////////////////////////////////

        BOOL bPartitioned;

        CBSTATUS cbStatus = 
                            hContext->
                            lpGlobalCallBacks->
                            lpGeneralCallBacks->
                            lpLinearCallBacks->
            LinearIsPartitionedMedia((LPVOID)byDrive, &bPartitioned);

        if (uResult == 0 && cbStatus == CBSTATUS_OK && bPartitioned)
        {
            ///////////////////////////////////////////////////////////////////
            // The drive we're scanning is a hard drive AND we were able to
            // successfully read the partition table.  Now, determine where the
            // desired boot sector is and scan it!
            ///////////////////////////////////////////////////////////////////

            VLGetBootSectorLocation (
                                 hContext,
                                 byDrive,
                                 lpImage,
                                 (UINT)byBootSecNum,
                                 &wCylinder,
                                 &bySector,
                                 &byHead,
                                 &bySystemType);

            ///////////////////////////////////////////////////////////////////
            // Now we know where the boot sector is.  Get it!
            ///////////////////////////////////////////////////////////////////

            uResult = VLSDiskReadPhysical(lpImage,
                                          1,
                                          wCylinder,
                                          bySector,
                                          byHead,
                                          byDrive);
        }

        if (uResult != 0)
        {
            VLTempMemoryFree(lpImage);      // free our temp. sector buffer

            if (hContext->wFlags & VLINIT_HANDLE_NO_ACCESS )
            {
               hVirus = (HVLVIRUS)VLPermMemoryAlloc(sHVLVIRUS);

               if (hVirus == NULL)
                  return(VS_MEMALLOC);

               VLSetHVLVIRUSBoot(hVirus,
                               hContext,
                               INVALID_ENTRY_NUM,
                               VIT_NO_ACCESS | VIT_BOOTPART,
                               byDrive,
                               byBootSecNum,
                               BI_BOOT_SECTOR,
                               bySystemType);

               DateGetCurrentDateTime(&hVirus->uDate,&hVirus->uTime);

              *lphVirus = hVirus;
            }
            return ( VS_NO_ACCESS);
        }
    }
    else
        lpImage = lpBooBuffer;          // user passed in BOO buffer


    ///////////////////////////////////////////////////////////////////////////
    // At this point, the proper boot sector is in lpImage buffer
    //
    ///////////////////////////////////////////////////////////////////////////

    if (!VLEngLocalInit(hContext, &hLEng))
    {
        return(VS_NO_ACCESS);
    }

    if (EngScanBoot(hLEng,
                    lpImage,
                    dwImageSize,
                    &bFoundVirus,
                    (LPHVIRUS)&wResult) != ENGSTATUS_OK)
    {
        return(VS_NO_ACCESS);
    }

    VLEngLocalClose(hLEng);

    if (bFoundVirus == FALSE)
    {
        EXTSTATUS   extResult;
        WORD        wVirusID;
        WORD        wVirusIndex;
        BYTE        byTempDrive;

        //////////////////////////////////////////////////////////////////////
        // No virus found using Avenge engine.  Try "fail-safe" detections!
        //
        //////////////////////////////////////////////////////////////////////

        // first, if lpBooBuffer is passed in as opposed to the Cyl,Sec,Head,
        // tell the EXT engine that the buffer is all we have to give it!

        if (lpBooBuffer)
            byTempDrive = 0xFF;
        else
            byTempDrive = byDrive;

        extResult = EXTScanBoot(hContext,
                                byTempDrive,
                                byBootSecNum,
                                bySector, wCylinder, byHead,
                                lpImage,
                                (LPBYTE)NULL,
                                EXTERNAL_DETECT_VERSION,
                                &wVirusID);

        if (EXTSTATUS_OK == extResult)
        {
            if (lpBooBuffer == NULL)            // if BOO buffer wasn't passed in
                VLTempMemoryFree(lpImage);      // free our temp. sector buffer

            return(VS_OK);              // no virus found
        }

        if (EXTSTATUS_VIRUS_FOUND == extResult)
        {
            if (VS_OK ==
                VLDetermineVirusIndex(hContext,
                                    wVirusID,
                                    &wVirusIndex))
                wResult = wVirusIndex;
            else
                wResult = 0xFFFFU;      // error
        }
        else
            wResult = 0xFFFFU;

    }

    if (lpBooBuffer == NULL)            // if BOO buffer wasn't passed in
        VLTempMemoryFree(lpImage);      // free our temp. sector buffer

    if (wResult == 0xFFFFU)
    {
        if ( hContext->wFlags & VLINIT_HANDLE_NO_ACCESS )
        {
            hVirus = (HVLVIRUS)VLPermMemoryAlloc(sHVLVIRUS);

            if (hVirus == NULL)
                return(VS_MEMALLOC);

            VLSetHVLVIRUSBoot(hVirus,
                            hContext,
                            INVALID_ENTRY_NUM,
                            VIT_NO_ACCESS | VIT_BOOTPART,
                            byDrive,
                            byBootSecNum,
                            BI_BOOT_SECTOR,
                            bySystemType);

            DateGetCurrentDateTime(&hVirus->uDate,&hVirus->uTime);

            *lphVirus = hVirus;
        }
        return ( VS_NO_ACCESS);
    }

    hVirus = (HVLVIRUS)VLPermMemoryAlloc(sHVLVIRUS);

    if (hVirus == NULL)
        return(VS_MEMALLOC);

    if (lpBooBuffer == NULL)
        bDisembodied = 0;
    else
    {
        bDisembodied = VIT_DISEMBODIED;

        bySystemType    = 0;
        byDrive         = 0;
        byBootSecNum    = 0;
    }

    VLSetHVLVIRUSBoot(hVirus,
                    hContext,
                    wResult,
                    VIT_SCANNED |
                    VIT_BOOTPART |
                    VIT_VALID_N30 |
                    VIT_HAS_A_VIRUS |
                    bDisembodied,
                    byDrive,
                    byBootSecNum,
                    BI_BOOT_SECTOR,
                    bySystemType);

    DateGetCurrentDateTime(&hVirus->uDate,&hVirus->uTime);

    if (!VLLoadN30(hContext,wResult,&hVirus->stVirusInfoBlock))
    {

        VLPermMemoryFree(hVirus);
        return(VS_ENTRY_UNAVAIL);

    }

    ///////////////////////////////////////////////////////////////////////////
    // Return our HVLVIRUS handle
    ///////////////////////////////////////////////////////////////////////////

    *lphVirus = hVirus;
    return(VS_OK);
}

/*@API:*************************************************************************
@Declaration: VSTATUS SYM_EXPORT WINAPI VLScanPart ( HVCONTEXT hContext,
BYTE byDrive, LPBYTE lpPartBuffer, LPHVLVIRUS lphVirus )

@Description:
If lpPartBuffer == NULL:
    This routine scans the MBR sector of the specified drive.  If a virus is
    found, VS_OK is returned, and lphVirus will point to a valid HVLVIRUS handle.

If lpPartBuffer != NULL:
    This routine scans the sector buffer pointed to by lpPartBuffer for
    BOOT viruses.  If a virus is found VS_OK is returned, and lphVirus will
    point to a valid HVLVIRUS handle.  This handle will be "disembodied"
    infection.  In other words, no repair can be applied to the specified handle.

@Parameters:
$hContext$ This is a valid handle to a context provided by VLScanInit.

$byDrive$ This is the physical hard drive number to scan. 0x80=first physical
drive, etc.

$lpPartBuffer$ This is a far pointer to a sector-sized buffer which contains
a partition sector image.  If lpPartBuffer != NULL, then byDrive is ignored,
otherwise lpPartBuffer is ignored.

$lphVirus$ This is a pointer to HVLVIRUS handle, and is set by this function
to a non-NULL value if a virus was found, otherwise it is set to NULL.
Information can be obtained about the handle by calling VLVirusInfo.

@Returns:

This function returns
     VS_OK if scanning was successfully completed whether or not a virus
           was found. A non-NULL value is lphVirus indicates infection.

     on error:
         VS_INVALID_ARG
         VS_MEMALLOC
         VS_NO_ACCESS
         VS_ENTRY_UNAVAIL


@Include: avapi.h
*******************************************************************************/

VSTATUS SYM_EXPORT WINAPI VLScanPart
(
    HVCONTEXT hContext,                 // [in] a valid HVCONTEXT handle
    BYTE byDrive,                       // [in] the physical drive # to scan
    LPBYTE lpPartBuffer,                // [in] disembodied buffer to scan
    LPHVLVIRUS lphVirus

)
{
    LPBYTE      lpImage;
    DWORD       dwImageSize;
    WORD        wResult = 0;
    HVLVIRUS    hVirus;
    BOOL        bDisembodied;
    UINT        uResult;
    BOOL        bFoundVirus;
    HLENG       hLEng = NULL;

    SYM_ASSERT(VLSDiskReadPhysical);
    *lphVirus = NULL;

    //////////////////////////////////////////////////////////////////////////
    // Verify the structure pointer and its signature
    //////////////////////////////////////////////////////////////////////////

    if (!VLValidContext(hContext))
        return(VS_INVALID_ARG);

    dwImageSize = (SECTOR_SIZE * 8);

    if (lpPartBuffer == NULL)
    {
        ///////////////////////////////////////////////////////////////////////
        // Allocate memory for the boot sector image
        ///////////////////////////////////////////////////////////////////////

        lpImage = (LPBYTE)VLTempMemoryAlloc(dwImageSize);
        if (lpImage == NULL)
            return(VS_MEMALLOC);

        ///////////////////////////////////////////////////////////////////////
        // Are we trying to scan the MBR on a floppy?? I hope not!
        ///////////////////////////////////////////////////////////////////////

        if (byDrive < 0x80)
        {
            VLTempMemoryFree(lpImage);      // free our temp. sector buffer
            return(VS_DRIVE_ERROR);
        }

        uResult = VLSDiskReadPhysical(lpImage, 1, 0, 1, 0, byDrive);

        // uResult == 0 means success

        if (uResult != 0)
        {
            VLTempMemoryFree(lpImage);      // free our temp. sector buffer

            if (hContext->wFlags & VLINIT_HANDLE_NO_ACCESS)
            {

                hVirus = (HVLVIRUS)VLPermMemoryAlloc(sHVLVIRUS);

                if (hVirus == NULL)
                    return(VS_MEMALLOC);

                VLSetHVLVIRUSBoot(hVirus,
                                hContext,
                                INVALID_ENTRY_NUM,
                                VIT_NO_ACCESS | VIT_BOOTPART,
                                byDrive,
                                0,
                                BI_PARTITION_SECTOR,
                                0);

                DateGetCurrentDateTime(&hVirus->uDate,&hVirus->uTime);
                *lphVirus = hVirus;
            }
            return(VS_NO_ACCESS);
        }
    }
    else
        lpImage = lpPartBuffer;

    //////////////////////////////////////////////////////////////////////////
    // At this point, the proper boot sector is in lpImage buffer
    //
    // The FALSE argument (last arg) should be removed and is used for DOS
    // callbacks.
    //////////////////////////////////////////////////////////////////////////

    if (!VLEngLocalInit(hContext, &hLEng))
    {
        return(VS_NO_ACCESS);
    }

    if (EngScanBoot(hLEng,
                    lpImage,
                    dwImageSize,
                    &bFoundVirus,
                    (LPHVIRUS)&wResult) != ENGSTATUS_OK)
    {
        return(VS_NO_ACCESS);
    }

    VLEngLocalClose(hLEng);

    if (bFoundVirus == FALSE)
    {
        EXTSTATUS   extResult;
        WORD        wVirusID;
        WORD        wVirusIndex;
        BYTE        byTempDrive;

        //////////////////////////////////////////////////////////////////////
        // No virus found using NAV 3 engine.  Try "fail-safe" detections!
        //////////////////////////////////////////////////////////////////////

        // first, if lpPartBuffer is passed in as opposed to the Cyl,Sec,Head,
        // tell the EXT engine that the buffer is all we have to give it!

        if (lpPartBuffer)
            byTempDrive = 0xFF;
        else
            byTempDrive = byDrive;

        extResult = EXTScanPart(hContext,
                                byTempDrive,
                                lpImage,
                                (LPBYTE)NULL,
                                EXTERNAL_DETECT_VERSION,
                                &wVirusID);

        if (EXTSTATUS_OK == extResult)
        {
            if (lpPartBuffer == NULL)       // if PAR buffer wasn't passed in
                VLTempMemoryFree(lpImage);  // free our temp. sector buffer

            return(VS_OK);              // no virus found
        }

        if (EXTSTATUS_VIRUS_FOUND == extResult)
        {
            if (VS_OK ==
                VLDetermineVirusIndex(hContext,
                                    wVirusID,
                                    &wVirusIndex))
                wResult = wVirusIndex;
            else
                wResult = 0xFFFFU;      // error
        }
        else
            wResult = 0xFFFFU;          // error

    }

    if (lpPartBuffer == NULL)
        VLTempMemoryFree(lpImage);      // free our temp. sector buffer

    if (wResult == 0xFFFF)
    {
        if ( hContext->wFlags & VLINIT_HANDLE_NO_ACCESS )
        {
            hVirus = (HVLVIRUS)VLPermMemoryAlloc(sHVLVIRUS);

            if (hVirus == NULL)
                return(VS_MEMALLOC);

            VLSetHVLVIRUSBoot(hVirus,
                            hContext,
                            INVALID_ENTRY_NUM,
                            VIT_NO_ACCESS | VIT_BOOTPART,
                            byDrive,
                            0,
                            BI_PARTITION_SECTOR,
                            0);

            DateGetCurrentDateTime(&hVirus->uDate,&hVirus->uTime);
            *lphVirus = hVirus;
        }
        return(VS_NO_ACCESS);
    }

    if (wResult == 0)
        return(VS_OK);

    hVirus = (HVLVIRUS)VLPermMemoryAlloc(sHVLVIRUS);

    if (hVirus == NULL)
        return(VS_MEMALLOC);

    if (lpPartBuffer == NULL)
        bDisembodied = 0;
    else
    {
        bDisembodied = VIT_DISEMBODIED;

        byDrive     = 0;
    }

    VLSetHVLVIRUSBoot(hVirus,
                    hContext,
                    wResult,
                    VIT_SCANNED |
                    VIT_BOOTPART |
                    VIT_VALID_N30 |
                    VIT_HAS_A_VIRUS |
                    bDisembodied,
                    byDrive,
                    0,
                    BI_PARTITION_SECTOR,
                    0);

    DateGetCurrentDateTime(&hVirus->uDate,&hVirus->uTime);

    if (!VLLoadN30(hContext,wResult,&hVirus->stVirusInfoBlock))
    {
        VLPermMemoryFree(hVirus);
        return(VS_ENTRY_UNAVAIL);
    }

    ///////////////////////////////////////////////////////////////////////////
    // Return our HVLVIRUS handle
    ///////////////////////////////////////////////////////////////////////////

    *lphVirus = hVirus;
    return(VS_OK);
}

#endif      // #ifndef SYM_NLM

#ifndef SYM_NLM

/*@API:*************************************************************************
@Declaration:  VSTATUS WINAPI VLReleaseEntry(HVLVIRUSENTRY hInfo)

@Description:
This routine is used to release a handle to a virus information entry used in
virus information-list displays.

@Parameters:
$hInfo$ This is a virus information entry handle.

@Returns:
This function returns a result of type VSTATUS (type definition found in
AVAPI.H).

    VS_OK           will be returned on success
    VS_INVALID_ARG  will be returned if the context is invalid

@Include: avapi.h
*******************************************************************************/

VSTATUS SYM_EXPORT WINAPI VLReleaseEntry
(
    HVLVIRUSENTRY hInfo                   // [in] a valid HVLVIRUSENTRY handle
)
{
    VOID (WINAPI *TempMemoryFree)(LPVOID lpBuffer);

    if (hInfo == NULL)
        return(VS_INVALID_ARG);

    ///////////////////////////////////////////////////////////////////
    // Obtain a function pointer to the permanent memory free callback
    ///////////////////////////////////////////////////////////////////

    TempMemoryFree = hInfo->lpVContext->lpstCallBack->TempMemoryFree;

    ///////////////////////////////////////////////////////////////////
    // Invalidate our HVLVIRUS structure by NULLing the context pointer
    ///////////////////////////////////////////////////////////////////

    hInfo->lpVContext = NULL;

    TempMemoryFree(hInfo);

    return(VS_OK);
}
#endif      // #ifndef SYM_NLM


#ifndef SYM_NLM

/*@API:*************************************************************************
@Declaration: VSTATUS SYM_EXPORT WINAPI VLQuarantineFile ( HVLVIRUS hVirus,
LPCTSTR pszQuarantineLocation );

@Description:
This routine can be used to quarantine (and wipe the original) file infected
with a virus.  The HVLVIRUS handle will be updated to reflect that the
original file has actually been deleted.

@Parameters:
$hVirus$ This is a valid HVLVIRUS handle containing the full pathname of the
file to quarantine.
$pszNewLocation$ A string containing the directory where to quarantine the
file.  This string should be in OEM in SYM_VXD!

@Returns:

This function returns a result of type VSTATUS (type definition found in
AVAPI.H).

    VS_OK           will be returned on success
    VS_CANT_DELETE  will be returned if the file should/could not be deleted after the quarantine
    VS_FILE_ERROR   will be returned if there is a file error
    VS_INVALID_ARG  will be returned if arguments to the function are invalid

If this function completes with VS_CANT_DELETE, the file will still be
quarantined (just the original infected file will still be around).

@Include: avapi.h
*******************************************************************************/
VSTATUS SYM_EXPORT WINAPI VLQuarantineFile (
    HVLVIRUS hVirus,
    LPCTSTR  pszQuarantineLocation
    )
    {
    HVCONTEXT   hContext;

    if ( !hVirus || !pszQuarantineLocation )
        return(VS_INVALID_ARG);

    hContext = hVirus->lpVContext;

                                        // Quarantine the file.

// Jacob, put the code for quarantine in here.  Let the code below delete
// the infected file.  Refer to VLMoveFile() on how to access the
// filenames, and character conversion routines that you will have to use

                                        // ------------------------------------
                                        // Check to see if we can delete the file
                                        // ------------------------------------

    if (VLCanDeleteItem(hVirus))
        {
                                        // Delete the infected file.

        if ( VLFileDelete(HV_FileInfo_szFileName(hVirus)) )
            {
            hVirus->dwFlags |= VIT_DELETED;
            hVirus->dwFlags &= ~((DWORD)VIT_HAS_A_VIRUS);

            return(VS_OK);
            }
        else
            hVirus->dwFlags |= VIT_CANNOT_DELETE;
        }

    return(VS_CANT_DELETE);
    }

/*@API:*************************************************************************
@Declaration: VSTATUS SYM_EXPORT WINAPI VLMoveFile ( HVLVIRUS hVirus,
LPCTSTR pszNewLocation, LPCTSTR pszNewExtension, LPTSTR pszMovedFile );

@Description:
This routine can be used to move (and wipe the original) file infected with a
virus.  The HVLVIRUS handle will be updated to reflect that the original file
has actually been deleted.

@Parameters:
$hVirus$ This is a valid HVLVIRUS handle containing the full pathname of the
file to move.
$pszNewLocation$ A string containing the directory where to move the file.
This string should be in OEM in SYM_VXD!
$pszFileExtension$ A string to replace the destination file's extension when
moving. If this parameter is NULL, the extension will be preserved.
$pszMovedFile$ A buffer (SYM_MAX_PATH characters) where this function will
copy the full path of the target file upon successful completion.  SYM_VXD: in OEM!

@Returns:

This function returns a result of type VSTATUS (type definition found in
AVAPI.H).

    VS_OK           will be returned on success
    VS_CANT_DELETE  will be returned if the file should/could not be deleted after the move
    VS_FILE_ERROR   will be returned if there is a file error

If this function does not complete with VS_OK, the contents of pszMovedFile
will be undefined.

@Include: avapi.h
*******************************************************************************/
VSTATUS SYM_EXPORT WINAPI VLMoveFile (
    HVLVIRUS hVirus,
    LPCTSTR  pszNewLocation,
    LPCTSTR  pszFileExtension,
    LPTSTR   pszMovedFile
    )
    {
    BOOL        bSuccess;
    HVCONTEXT   hContext;
    TCHAR       szNewTarget[SYM_MAX_PATH];
#if defined(SYM_VXD)
    auto char   szTarget[SYM_MAX_PATH*sizeof(WCHAR)];
#endif

    if (hVirus == NULL)
        return(VS_INVALID_ARG);

    hContext = hVirus->lpVContext;

    SYM_ASSERT(VLFileDelete);
    SYM_ASSERT(VLFileGetAttr);

                                        // ------------------------------------
                                        // Check to see if we can delete the file
                                        // ------------------------------------

    if (!VLCanDeleteItem(hVirus))
        return(VS_CANT_DELETE);

                                        // ------------------------------------
                                        // Create a full pathname for the
                                        // target file and make sure it is
                                        // unique.
                                        // ------------------------------------

                                        // Copy the new location into our
                                        // local work buffer and make sure
                                        // the trailing backslash is removed.

    STRCPY ( szNewTarget, pszNewLocation );
    NameAppendFile ( szNewTarget, _T("?") );
    NameStripFile ( szNewTarget );

                                        // Get the name of the original file.

    if ( NameReturnFile ( hVirus->FileInfo.szFileName,
                          pszMovedFile ) != NOERR )
        {
        return ( VS_FILE_ERROR );
        }

                                        // Make sure that when we concatenate
                                        // the path and the name we will not
                                        // overflow our buffers.

    if ( STRLEN ( szNewTarget ) +
         STRLEN ( _T("\\") ) +
         STRLEN ( pszMovedFile ) >= SYM_MAX_PATH )
        {
        return ( VS_FILE_ERROR );
        }

                                        // Create a full specification of the
                                        // target file.

    NameAppendFile ( szNewTarget, pszMovedFile );

                                        // Adjust the filename so that it is
                                        // unique.

    if ( !BuildUniqueFileName ( szNewTarget, pszFileExtension, pszMovedFile ) )
        {
        return ( VS_FILE_ERROR );
        }

                                        // ------------------------------------
                                        // Copy the original file to the
                                        // unique file.
                                        // ------------------------------------

#if defined(SYM_VXD)
    VxDGenericStringConvert ( pszMovedFile,
                              CHARSET_OEM,
                              szTarget,
                              CHARSET_CURRENT );

    if ( FileCopy ( HV_FileInfo_szFileName(hVirus), szTarget, NULL ) )
#else
    if ( FileCopy ( hVirus->FileInfo.szFileName, pszMovedFile, NULL ) )
#endif
        {
        return ( VS_FILE_ERROR );
        }

    bSuccess = VLFileDelete(HV_FileInfo_szFileName(hVirus));

    if ( bSuccess )
        {
        hVirus->dwFlags |= VIT_DELETED;
        hVirus->dwFlags &= ~((DWORD)VIT_HAS_A_VIRUS);

        return(VS_OK);
        }
    else
        hVirus->dwFlags |= VIT_CANNOT_DELETE;

#if defined(SYM_VXD)
    VLFileDelete(szTarget);
#else
    VLFileDelete(pszMovedFile);
#endif

    return(VS_FILE_ERROR);
    }

/*@API:*************************************************************************
@Declaration: VSTATUS SYM_EXPORT WINAPI VLMoveFileEx ( LPCTSTR pszFileName,
LPCTSTR pszNewLocation, LPCTSTR pszNewExtension, LPTSTR pszMovedFile );

@Description:
This routine can be used to move a file with an option to change the
extension of the destination file.

@Parameters:
$pszFileName$ A string containing the full name of the file to move.  SYM_VXD: in OEM!
$pszNewLocation$ A string containing the directory where to move the file.
This string should be in OEM in SYM_VXD!
$pszFileExtension$ A string to replace the destination file's extension when
moving. If this parameter is NULL, the extension will be preserved.
$pszMovedFile$ A buffer (SYM_MAX_PATH characters) where this function will
copy the full path of the target file upon successful completion.  SYM_VXD: in OEM!

@Returns:

This function returns a result of type VSTATUS (type definition found in
AVAPI.H).

    VS_OK           will be returned on success
    VS_FILE_ERROR   will be returned if there is a file error

If this function does not complete with VS_OK, the contents of pszMovedFile
will be undefined.

@Include: avapi.h
*******************************************************************************/
VSTATUS SYM_EXPORT WINAPI VLMoveFileEx (
    LPCTSTR pszFileName,
    LPCTSTR pszNewLocation,
    LPCTSTR pszFileExtension,
    LPTSTR  pszMovedFile
    )
    {
    TCHAR       szNewTarget[SYM_MAX_PATH];
#if defined(SYM_VXD)
    auto char   szTarget[SYM_MAX_PATH*sizeof(WCHAR)];
    auto char   szSource[SYM_MAX_PATH*sizeof(WCHAR)];
#endif

                                        // ------------------------------------
                                        // Create a full pathname for the
                                        // target file and make sure it is
                                        // unique.
                                        // ------------------------------------

                                        // Copy the new location into our
                                        // local work buffer and make sure
                                        // the trailing backslash is removed.

    STRCPY ( szNewTarget, pszNewLocation );
    NameAppendFile ( szNewTarget, _T("?") );
    NameStripFile ( szNewTarget );

                                        // Get the name of the original file.

    if ( NameReturnFile ( pszFileName, pszMovedFile ) != NOERR )
        {
        return ( VS_FILE_ERROR );
        }

                                        // Make sure that when we concatenate
                                        // the path and the name we will not
                                        // overflow our buffers.

    if ( STRLEN ( szNewTarget ) +
         STRLEN ( _T("\\") ) +
         STRLEN ( pszMovedFile ) >= SYM_MAX_PATH )
        {
        return ( VS_FILE_ERROR );
        }

                                        // Create a full specification of the
                                        // target file.

    NameAppendFile ( szNewTarget, pszMovedFile );

                                        // Adjust the filename so that it is
                                        // unique.

    if ( !BuildUniqueFileName ( szNewTarget, pszFileExtension, pszMovedFile ) )
        {
        return ( VS_FILE_ERROR );
        }

                                        // ------------------------------------
                                        // Copy the original file to the
                                        // unique file.
                                        // ------------------------------------

#if defined(SYM_VXD)
    VxDGenericStringConvert ( pszMovedFile,
                              CHARSET_OEM,
                              szTarget,
                              CHARSET_CURRENT );

    VxDGenericStringConvert ( (PTSTR)pszFileName,
                              CHARSET_OEM,
                              szSource,
                              CHARSET_CURRENT );

    if ( FileCopy ( szSource, szTarget, NULL ) )
        {
        return ( VS_FILE_ERROR );
        }

    if ( FileKill(szSource) )
        {
        FileKill(szTarget);
        return(VS_FILE_ERROR);
        }

#else

    if ( FileCopy ( pszFileName, pszMovedFile, NULL ) )
        {
        return ( VS_FILE_ERROR );
        }

    if ( FileKill(pszFileName) )
        {
        FileKill(pszMovedFile);
        return(VS_FILE_ERROR);
        }

#endif

    return(VS_OK);
    }

/*@API:*************************************************************************
@Declaration: VSTATUS SYM_EXPORT WINAPI VLRenameFile ( HVLVIRUS hVirus,
LPCTSTR pszNewFileName, LPTSTR pszRenamedFile );

@Description:
This routine can be used to rename a file infected with a virus.  The HVLVIRUS
handle will be updated to reflect that the original file has actually
disappeared (deleted).

@Parameters:
$hVirus$ This is a valid HVLVIRUS handle.
$pszNewFileName$ A string containing the full name of the new file.  This
string should be in OEM in SYM_VXD!
$pszRenamedFile$ A buffer (SYM_MAX_PATH characters) where this function will
copy the full path of the renamed file upon successful completion.  SYM_VXD:
in OEM!

@Returns:

This function returns a result of type VSTATUS (type definition found in
AVAPI.H).

    VS_OK           will be returned on success
    VS_CANT_DELETE  will be returned if the file should/could not be renamed/deleted
    VS_FILE_ERROR   will be returned if there is a file error

If this function does not complete with VS_OK, the contents of pszRenamedFile
will be undefined.

@Include: avapi.h
*******************************************************************************/
VSTATUS SYM_EXPORT WINAPI VLRenameFile (
    HVLVIRUS  hVirus,
    LPCTSTR pszNewFileName,
    LPTSTR  pszRenamedFile
    )
    {
    HVCONTEXT   hContext;
#if defined(SYM_VXD)
    auto char   szTarget[SYM_MAX_PATH*sizeof(WCHAR)];
#endif

    if (hVirus == NULL)
        return(VS_INVALID_ARG);

    hContext = hVirus->lpVContext;

    SYM_ASSERT(VLFileGetAttr);

                                        // ------------------------------------
                                        // Check to see if we can delete the file
                                        // ------------------------------------

    if (!VLCanDeleteItem(hVirus))
        return(VS_CANT_DELETE);

                                        // ------------------------------------
                                        // Create a unique version of the
                                        // target file.
                                        // ------------------------------------

    if ( !BuildUniqueFileName ( pszNewFileName, NULL, pszRenamedFile ) )
        {
        return ( VS_FILE_ERROR );
        }

                                        // ------------------------------------
                                        // Rename the file.
                                        // ------------------------------------

#if defined(SYM_VXD)
    VxDGenericStringConvert ( pszRenamedFile,
                              CHARSET_OEM,
                              szTarget,
                              CHARSET_CURRENT );

    if ( FileRename ( HV_FileInfo_szFileName(hVirus), szTarget ) == NOERR )
#else
    if ( FileRename ( hVirus->FileInfo.szFileName, pszRenamedFile ) == NOERR )
#endif
        {
        hVirus->dwFlags |= VIT_DELETED;
        hVirus->dwFlags &= ~((DWORD)VIT_HAS_A_VIRUS);

        return(VS_OK);
        }
    else
        hVirus->dwFlags |= VIT_CANNOT_DELETE;

    return(VS_FILE_ERROR);
    }

#endif // #ifndef SYM_NLM

#ifndef SYM_NLM
/*@API:*************************************************************************
@Declaration: HVLVIRUSENTRY WINAPI VLGetVirusEntry(HVCONTEXT hContext,
UINT uEntryNum)

@Description:
This routine obtains a single virus entry from the virus database.  The
specified entry number for NAV 3.0/4.0 is 1 based.  If the entry has been
deleted then NULL will be returned.

@Parameters:
$hContext$ This is a valid handle to a context provided by VLScanInit.

$uEntryNum$ Which entry should be retrieved from the database?

@Returns:
On success, this function returns a HVLVIRUSENTRY handle.  If there is an error,
or the entry has been deleted, then the function returns NULL, and the
specific error condition can be retrieved by calling the VLGetError() function.

@Include: avapi.h
*******************************************************************************/

HVLVIRUSENTRY SYM_EXPORT WINAPI VLGetVirusEntry
(
    HVCONTEXT hContext,                 // [in] a valid HVCONTEXT handle
    UINT uEntryNum                      // [in] which entry num to retrieve?
)
{
    HVLVIRUSENTRY hVirusEntry;

    hVirusEntry = (HVLVIRUSENTRY)VLPermMemoryAlloc(sizeof(VIRUSENTRYTYPE));

    if (hVirusEntry == NULL)
        return(NULL);

    ///////////////////////////////////////////////////////////////////////////
    // make sure we point to our parent context.
    ///////////////////////////////////////////////////////////////////////////

    hVirusEntry->lpVContext     = hContext;
    hVirusEntry->uDBEntryNum    = uEntryNum;
    hVirusEntry->wFlags         = 0;

    if (!VLLoadN30(hContext,uEntryNum,&hVirusEntry->stVirusInfoBlock))
    {
        VLPermMemoryFree(hVirusEntry);

        return(NULL);
    }

    if ( (hVirusEntry->stVirusInfoBlock.wType == ALGtype ||
          hVirusEntry->stVirusInfoBlock.wType == BOOtype ||
          hVirusEntry->stVirusInfoBlock.wType == MACtype ||
          hVirusEntry->stVirusInfoBlock.wType == CRCtype) &&
         (hVirusEntry->stVirusInfoBlock.wControl != DEAD_VIRUS_ENTRY) )
        return(hVirusEntry);

    VLPermMemoryFree(hVirusEntry);

    return(NULL);
}


/*@API:*************************************************************************
@Declaration: VSTATUS WINAPI VLGetDBInfo(HVCONTEXT hContext, VLDBINFO enumInfo,
LPTSTR lpszResultBuffer)

@Description:
This routine provides the caller with specific information about the
VIRSCAN.DAT database.

    DB_TOTAL_VIRUS_COUNT            : How many viruses do we claim we detect?
    DB_MONTH_STAMP                  : Month # of the virus database (1-12)
    DB_DAY_STAMP                    : Day # of the virus database (1-31)
    DB_YEAR_STAMP                   : Year # of the virus database (1900+)


@Parameters:
$hContext$ This is a valid handle to a context provided by VLScanInit.

$enumInfo$ Specifies what information to return to the user.

$lpszResultbuffer$ is a far pointer to a buffer where the result will be
placed.  It will have been converted to a string.

@Returns:

    VS_OK           on success
    VS_FILE_ERROR   on file error
    VS_INVALID_ARG  if an invalid enumInfo value is passed

@Include: avapi.h
*******************************************************************************/
VSTATUS SYM_EXPORT WINAPI VLGetDBInfo
(
    HVCONTEXT   hContext,               // [in] a valid HVCONTEXT handle
    VLDBINFO    enumInfo,               // [in] which info should we return?
    LPTSTR      lpszResultBuffer        // [out] string converted result
)
{
    lpszResultBuffer[0] = EOS;

    DATA_FILE_HEADER_T      stHeader;
    HFILE                   hFile;
    UINT                    uResult;


    ///////////////////////////////////////////////////////////////////////////
    // open the VIRSCAN.DAT file to determine the number of viruses "detected"
    ///////////////////////////////////////////////////////////////////////////

    hFile = VLFileOpen(HVC_szVirScanDataFile(hContext),READ_ONLY_FILE);
    if (hFile == HFILE_ERROR)
        return(VS_FILE_ERROR);

    ///////////////////////////////////////////////////////////////////////////
    // Now read in the VIRSCAN.DAT header.
    ///////////////////////////////////////////////////////////////////////////

    uResult = VLFileRead(hFile,&stHeader,sizeof(stHeader));

    VLFileClose(hFile);

    if (uResult != sizeof(stHeader))
        return(VS_FILE_ERROR);

    switch(enumInfo)
    {
        case DB_TOTAL_VIRUS_COUNT:
            _ConvertLongToString(VLNumClaimedVirusEntries(hContext),lpszResultBuffer,10,0);
            break;
        case DB_MONTH_STAMP:
            _ConvertWordToString((UINT)stHeader.byMon,lpszResultBuffer,10,0);
            break;
        case DB_DAY_STAMP:
            _ConvertWordToString((UINT)stHeader.byDay,lpszResultBuffer,10,0);
            break;
        case DB_YEAR_STAMP:
            _ConvertWordToString((UINT)stHeader.byYear+1900,lpszResultBuffer,10,0);
            break;
        default:
            return(VS_INVALID_ARG);
    }

    return(VS_OK);
}

/*@API:*************************************************************************
@Declaration: UINT WINAPI VLFillListTable(HVCONTEXT hContext,
HVIRUSENTRYRABLE lpFillTable)


@Description:
This function fills a (previously allocated) array of HVIRUSENTRY pointers with
valid pointers to HVIRUSENTRies.

@Parameters:
$hContext$ This is a valid handle to a context provided by VLScanInit.

$lpFillTable$ This is a table whose size is greater than or equal to

                    (sizeof(FAR *) * VLTrueNumVirusEntries(hContext))

@Returns:

    The number of table entries actually filled in.  These pointers which are
placed in the table must be explicitly freed by the user.

@Include: avapi.h
*******************************************************************************/
UINT SYM_EXPORT WINAPI VLFillListTable
(
    HVCONTEXT          hContext,        // [in] a valid HVCONTEXT handle
    HVLVIRUSENTRYTABLE lpFillTable      // [out] prealloc'd array of pointers
)
{
    UINT                uCurDefNum = 0,
                        uTotalDefsInMemory = 0;
    N30                 stN30;
    BULK_LOAD_INFO_T    stBulkInfo;
    BOOL                bDone = FALSE;

    // make sure there is actually a table

    if (lpFillTable == NULL)
    {
        return(0);
    }

    if (EngInitBulkLoad(hContext->hGEng,
                        &stBulkInfo) != ENGSTATUS_OK)
    {
        return(0);
    }

    for(;;)
    {
        if (EngGetNextN30(hContext->hGEng,
                          &stBulkInfo,
                          &stN30,
                          &bDone) != ENGSTATUS_OK)
        {
            EngCloseBulkLoad(hContext->hGEng, &stBulkInfo);
            return(uTotalDefsInMemory);
        }

        if(bDone)
             break;

        if ((stN30.wType == ALGtype ||
             stN30.wType == BOOtype ||
             stN30.wType == MACtype ||
             stN30.wType == CRCtype) &&
            (stN30.wControl != DEAD_VIRUS_ENTRY))
        {
            lpFillTable[uTotalDefsInMemory] =
                (HVLVIRUSENTRY)VLTempMemoryAlloc(sVIRUSENTRYTYPE);

            if (lpFillTable[uTotalDefsInMemory] == NULL)
            {
                EngCloseBulkLoad(hContext->hGEng, &stBulkInfo);
                return(uTotalDefsInMemory);
            }

            ///////////////////////////////////////////////////////////////
            // Create a new virus database entry in memory
            ///////////////////////////////////////////////////////////////

            lpFillTable[uTotalDefsInMemory]->lpVContext       = hContext;
            lpFillTable[uTotalDefsInMemory]->wFlags           = 0;
            lpFillTable[uTotalDefsInMemory]->uDBEntryNum      = uCurDefNum;
            lpFillTable[uTotalDefsInMemory]->stVirusInfoBlock = stN30;

            uTotalDefsInMemory++;
        }

        uCurDefNum++;
    }

    EngCloseBulkLoad(hContext->hGEng, &stBulkInfo);
    return(uTotalDefsInMemory);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//
// AVAPI internal support routines!
//
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

#if defined(SYM_WIN32) && !defined(SYM_VXD) && !defined(SYM_NTK)

/*@API:*************************************************************************
@Declaration: BOOL LOCAL VLGetLastAccessDateNT(LPCTSTR szFileName,
                                               FILETIME *lpFileTime )

@Description:
This function will return the last access date for a given filename

@Parameters:
$szFileName$ Filename to retrieve last access info for.

$lpFileTime$ Pointer to FILETIME structure used for returned data.

@Returns:

    TRUE if successful
    FALSE otherwise

*******************************************************************************/
BOOL LOCAL VLGetLastAccessDateNT( LPCTSTR szFileName, FILETIME *lpFileTime )
{

    WIN32_FILE_ATTRIBUTE_DATA attribData;
    static LPFNGETFILEATTRIBUTESEX pGetFileAttributes = NULL;

    // Get pointer to GetFileAttributesEx function
    if( pGetFileAttributes == NULL )
        {
        HMODULE hKernel32 = GetModuleHandle( _T("KERNEL32"));

        if( hKernel32 == NULL )
            return FALSE;

#ifdef UNICODE
        pGetFileAttributes = (LPFNGETFILEATTRIBUTESEX)
                             GetProcAddress( hKernel32,
                                             GETFILEATTRIBUTESEXW );
#else
        pGetFileAttributes = (LPFNGETFILEATTRIBUTESEX)
                             GetProcAddress( hKernel32,
                                             GETFILEATTRIBUTESEXA );

#endif
        if( pGetFileAttributes == NULL )
            return FALSE;
        }


    // Get extended file attributes
    if( pGetFileAttributes( szFileName,
                            GetFileExInfoStandard,
                            &attribData ) )
        {
        // Save off last file access.
        *lpFileTime = attribData.ftLastAccessTime;
        return TRUE;
        }

    return FALSE;
}


#endif // #if defined(SYM_WIN32) && !defined(SYM_VXD) && !defined(SYM_NTK)


///////////////////////////////////////////////////////////////////////////////
// Returns TRUE if O.K., FALSE otherwise
///////////////////////////////////////////////////////////////////////////////

void LOCAL VLSetHVLVIRUSBoot (
    HVLVIRUS      hVirus,
    HVCONTEXT   lpVContext,
    UINT        uDBEntryNum,
    DWORD       dwFlags,
    BYTE        byDriveNum,
    BYTE        byBootSecNum,
    BYTE        byBootFlags,
    BYTE        bySystemType )
{
    hVirus->lpVContext              = lpVContext;
    hVirus->uDBEntryNum             = uDBEntryNum;
    hVirus->dwFlags                 = dwFlags;
    hVirus->BootInfo.byDriveNum     = byDriveNum;
    hVirus->BootInfo.byBootSecNum   = byBootSecNum;
    hVirus->BootInfo.byBootFlags    = byBootFlags;
    hVirus->BootInfo.bySystemType   = bySystemType;
    hVirus->hNext                   = NULL;
}


// ============================================================================
#endif	  // end of all ifndef SYM_NLM
// ============================================================================




// =====================================================================
// Dead code below here
// =====================================================================

#if 0
/*@API:*************************************************************************
@Declaration:  VSTATUS WINAPI VLRegisterFastVirusDB(HVCONTEXT hContext,
LPSTR lpszDBFile, LPSTR lpszInfoFile)

@Description:
This routine is used to "register" a virus database for fast access - it
preloads the virus names and verification information.
and its accompanying virus information database (VIRSCAN.INF currently).  This
must be done after VLScanInit and before any scanning functions are called.

@Parameters:
$hContext$ This is a valid handle to a context provided by VLScanInit.

@Returns:
This function returns a result of type VSTATUS (type definition found in
AVAPI.H).

    VS_OK           will be returned on success
    VS_INVALID_ARG  will be returned if the context is invalid
    VS_FILE_ERROR   will be returned if a specified data file can't be accessed
    VS_MEMALLOC     will be returned if memory can't be allocated

@Include: avapi.h
*******************************************************************************/

VSTATUS SYM_EXPORT WINAPI VLRegisterFastVirusDB
(
    HVCONTEXT hContext,                 // [in] a valid HVCONTEXT handle
    LPSTR     lpszDBFile,               // [in] path & filename of VIRSCAN.DAT
    LPSTR     lpszInfoFile              // [in] path & filename of VIRSCAN.INF
)
{
    UINT                uDefCount;
    LPFASTVIRUSCHECK    lpFastVirusCheck = NULL;


    (void) lpszDBFile;
    (void) lpszInfoFile;

    //////////////////////////////////////////////////////////////////////////
    // Verify the structure pointer and its signature
    //////////////////////////////////////////////////////////////////////////

    if (!VLValidContext(hContext))
        return(VS_INVALID_ARG);

    // find out how many defs there are

    uDefCount = VLTrueNumVirusEntries( hContext )+1;

    // allocate an array to hold all the definition check information

    hContext->lpFastCheckDB =
        (LPFASTVIRUSCHECK) VLPermMemoryAlloc(
                        uDefCount * sizeof(*lpFastVirusCheck));

    if ( hContext->lpFastCheckDB )
        {

// stub to get NLM on AVCORE working ==dad
        hContext->uTotalDefs = 0;

//        hContext->uTotalDefs =
//            VLFillFastCheckDB( hContext, uDefCount, hContext->lpFastCheckDB );
        }
    else
        {
        return(VS_MEMALLOC);
        }

    return(VS_OK);
}
#endif // ifdef 0

#if 0
/*@API:*************************************************************************
@Declaration: UINT WINAPI VLFillFastCheckDB(
    HVCONTEXT hContext,
    LPFASTVIRUSCHECK lpFastCheckDB)


@Description:
This function fills a previously allocated array of FASTVIRUSCHECK structs
with entries for all definitions, valid or otherwise

@Parameters:
$hContext$ This is a valid handle to a context provided by VLScanInit.

$lpFastCheckDB$ This is a table whose size is greater than or equal to

                    (sizeof(FASTVIRUSCHECK) * VLTrueNumVirusEntries(hContext))

@Returns:

    The number of table entries actually filled in.

@Include: avapi.h
*******************************************************************************/
UINT SYM_EXPORT WINAPI VLFillFastCheckDB
(
    HVCONTEXT           hContext,           // [in] a valid HVCONTEXT handle
    int                 nMaxEntries,        // [in] size of prealloc'd array
    LPFASTVIRUSCHECK    lpFastCheckDB       // [out] prealloc'd array
)
{
    HFILE               hFile;
    UINT                uTotalRecords,
                        uCount,
                        uRecordsToRead,
                        uCurDefNum,
                        uTotalDefsInMemory,
                        uBytesToRead;
    LPN30               lpDefInMemory;
    LPN30               lpDefPtr;


    // make sure there is actually a table

    if (lpFastCheckDB == NULL)
        {
        return(0);
        }

    ///////////////////////////////////////////////////////////////////////////
    // Open the VIRSCAN.DAT database
    ///////////////////////////////////////////////////////////////////////////

    hFile = VLFileOpen(hContext->szVirScanDataFile, READ_ONLY_FILE);

    if (hFile == HFILE_ERROR )
        return(0);

    uTotalRecords = VLGetCountOfDefinitionsInFile( hContext, hFile );

    ///////////////////////////////////////////////////////////////////////////
    // Allocate some memory for reading in a bunch of N30 entries at a time...
    ///////////////////////////////////////////////////////////////////////////

    lpDefInMemory = ( LPN30 )VLTempMemoryAlloc(CHUNK_N30_ENTRIES * sizeof(N30));

    if (lpDefInMemory == NULL)
    {
        VLFileClose(hFile);

        return(0);
    }

    ///////////////////////////////////////////////////////////////////////////
    // Seek past the header and start reading 100 records at a time...
    ///////////////////////////////////////////////////////////////////////////

    if (VLFileSeek(hFile,sizeof(VIRSCANDATHEADER),SEEK_SET) !=
        sizeof(VIRSCANDATHEADER))
    {
        VLFileClose(hFile);
        VLTempMemoryFree(lpDefInMemory);

        return(0);
    }

    ///////////////////////////////////////////////////////////////////////////
    // Init count of total defs processed (to keep track of entry numbers) and
    // of how many we've loaded into memory.
    ///////////////////////////////////////////////////////////////////////////

    uTotalDefsInMemory = 0;
    uCurDefNum = 1;                     // 1-based start

    while (uTotalRecords)
    {
        uRecordsToRead = (uTotalRecords > CHUNK_N30_ENTRIES) ?
                          CHUNK_N30_ENTRIES : uTotalRecords;

        uBytesToRead = uRecordsToRead * sizeof(N30);

        if (FileRead(hFile, lpDefInMemory, uBytesToRead) != uBytesToRead)
        {
            VLFileClose(hFile);
            VLTempMemoryFree(lpDefInMemory);

            return(uTotalDefsInMemory);
        }


        for (uCount = 0, lpDefPtr = lpDefInMemory;
             uCount < uRecordsToRead;
             uCount++,lpDefPtr++,uCurDefNum++)
        {

        // check for table overflow!

        if (uTotalDefsInMemory > nMaxEntries)
            {
            return (0);
            }

        ///////////////////////////////////////////////////////////////
        // Create a new fast check entry
        ///////////////////////////////////////////////////////////////

        // apply the same filter as VLFIllListTable

        lpFastCheckDB[uTotalDefsInMemory].wVirID   = lpDefPtr->wVirID;

        if ( (lpDefPtr->wType == ALGtype ||
              lpDefPtr->wType == BOOtype ||
              lpDefPtr->wType == MACtype ||
              lpDefPtr->wType == CRCtype)
              && (lpDefPtr->wControl != DEAD_VIRUS_ENTRY) )
            {

            // decrypt check data, and save it

            lpFastCheckDB[uTotalDefsInMemory].wCheckData =
                ((BYTE) (lpDefPtr->lpSig[1] - 1)) << 8 |
                ((BYTE) (lpDefPtr->lpSig[0] - 1));

            lpFastCheckDB[uTotalDefsInMemory].wControl = lpDefPtr->wControl;

            // save virus name - still encrypted

            MEMCPY( lpFastCheckDB[uTotalDefsInMemory].szVName,
                    lpDefPtr->lpVName,
                    sizeof( lpFastCheckDB[uTotalDefsInMemory].szVName ) );
            }
        else
            {
            lpFastCheckDB[uTotalDefsInMemory].wControl = DEAD_VIRUS_ENTRY;
            lpFastCheckDB[uTotalDefsInMemory].szVName[0] = EOS;
            }

        uTotalDefsInMemory++;

        }
        uTotalRecords -= uRecordsToRead;
    }

    VLFileClose(hFile);
    VLTempMemoryFree(lpDefInMemory);

    return(uTotalDefsInMemory);
}
#endif // ifdef 0

#if 0
/*@API:*************************************************************************
@Declaration: DWORD SYM_EXPORT WINAPI  VLCheckFastDB(
    HVCONTEXT hContext,
    WORD      wFirstWord,
    WORD      wVirusId,
    LPTSTR    szVirusName )

@Description:
This function verifies a virus hit, and returns lpVirusData and the virus name

@Parameters:
$hContext$      This is a valid handle to a context provided by VLScanInit.
$wFirstWord$    First word of virus code - use to double check CRC defs
$wVirusId$      virus id to verify
$szVirusName$   pointer to return virus name

@Returns:

a DWORD containing the wControl value in the HIWORD and the virus id in
the LOWORD

@Include: avapi.h
*******************************************************************************/
DWORD SYM_EXPORT WINAPI  VLCheckFastDB
(
    HVCONTEXT hContext,         // [in] a valid HVCONTEXT handle
    WORD      wFirstWord,       // [in] check value for CRC defs
    WORD      wVirusId,         // [in] virus id to check
    LPTSTR    lpszVirusName     // [out] virus name returned
)
{
    WORD    wZeroBaseId;
    WORD    wControl;

    WORD    i;
    LPTSTR  lpVirName;

    TCHAR   cCurrChar,
            cPrevChar;

    LPFASTVIRUSCHECK    lpFastVirusCheck = NULL;


    // validity check incoming virus id

    if (wVirusId == 0)
        {
        goto BailOut;
        }

    // if I have fast look up, do it, otherwise return error

    if (hContext->lpFastCheckDB)
        {

        // make sure it falls in the table

        if (wVirusId > hContext->uTotalDefs)
            {
            wVirusId = 0;
            goto BailOut;
            }

        // I need zero-base for my table lookups

        wZeroBaseId = wVirusId - 1;
        lpFastVirusCheck = &hContext->lpFastCheckDB[wZeroBaseId];

        if (lpFastVirusCheck->wControl == DEAD_VIRUS_ENTRY)
            {
            wVirusId = 0;
            goto BailOut;
            }

        if (wFirstWord != lpFastVirusCheck->wCheckData)
            {
            wVirusId = 0;           // mark no virus here
            goto BailOut;
            }


        // virus here - return control bits and name

        wControl = lpFastVirusCheck->wControl;

        if (lpszVirusName != NULL)
            {
            lpVirName = lpFastVirusCheck->szVName;

            cPrevChar = 0;

            for (i = 0; i < SIZE_VIRUSNAME; i++)
                {
                                        // Copy the current char into the
                                        // output string and save it in
                                        // cCurrChar. Bump the pointer for
                                        // the output but not the input yet
                cCurrChar = *lpVirName;
                *lpszVirusName = cCurrChar;
                lpszVirusName++;

#if !defined (UNICODE)
                                        // If the current char is a lead byte
                                        // copy the trail byte into the output
                                        // Note how the source pointer is bumped
                if (DBCSIsLeadByte (*lpVirName))
                    {
                    lpVirName++;
                    *lpszVirusName = *lpVirName;
                    lpszVirusName++;
                    }
#endif
                lpVirName++;

                                        // If this was a double byte it could
                                        // not have been a blank therefore this
                                        // test is still valid
                if (cCurrChar == _T(' '))
                    {
                    if (cPrevChar == _T(' '))      // double blank is end of name
                        {
                        lpszVirusName[-2] = 0;    // null terminate at last blank
                        break;
                        }
                    }

                cPrevChar = cCurrChar;
                }
            }
        }
    else
        {
        wVirusId = 0;
        }

BailOut:

    return (((DWORD)wControl << 16) + (DWORD)wVirusId);     // merge return codes
}
#endif // ifdef 0

#if 0
/*@API:*************************************************************************
@Declaration: DWORD SYM_EXPORT WINAPI  VLGetNameDB(
    HVCONTEXT hContext,
    WORD      wVirusId,
    LPTSTR    szVirusName )

@Description:
This function returns lpVirusData and the virus name

@Parameters:
$hContext$      This is a valid handle to a context provided by VLScanInit.
$wVirusId$      virus id
$szVirusName$   pointer to return virus name

@Returns:

a DWORD containing the wControl value in the HIWORD and the virus id in
the LOWORD

@Include: avapi.h
*******************************************************************************/
DWORD SYM_EXPORT WINAPI  VLGetNameDB
(
    HVCONTEXT hContext,         // [in] a valid HVCONTEXT handle
    WORD      wVirusId,         // [in] virus id
    LPTSTR    szVirusName       // [out] virus name returned
)
{
    WORD    wZeroBaseId;
    WORD    wControl;

    WORD    i;
    LPTSTR  lpVirName;
    TCHAR   byChar, byLastChar;

    LPFASTVIRUSCHECK    lpFastVirusCheck = NULL;


    // validity check incoming virus id

    if (wVirusId == 0)
        {
        goto BailOut;
        }

    // if I have fast look up, do it, otherwise return error

    if (hContext->lpFastCheckDB)
        {

        // make sure it falls in the table

        if (wVirusId > hContext->uTotalDefs)
            {
            wVirusId = 0;
            goto BailOut;
            }

        // I need zero-base for my table lookups

        wZeroBaseId = wVirusId - 1;
        lpFastVirusCheck = &hContext->lpFastCheckDB[wZeroBaseId];

        if (lpFastVirusCheck->wControl == DEAD_VIRUS_ENTRY)
            {
            wVirusId = 0;
            goto BailOut;
            }

        // virus here - return control bits and name

        wControl = lpFastVirusCheck->wControl;

        if (szVirusName != NULL)
            {
            lpVirName = lpFastVirusCheck->szVName;

            byLastChar = 0;

            for (i=0; i<SIZE_VIRUSNAME; i++)
                {
                byChar = *lpVirName++;      // decrypt char ^ 0x47
                *szVirusName++ = byChar;

                if (byChar == _T(' '))
                    {
                    if (byLastChar == _T(' '))  // double blank is end of name
                        {
                        szVirusName[-2] = 0;    // null terminate at last blank
                        break;
                        }
                    }

                byLastChar = byChar;
                }
            }
        }
    else
        {
        wVirusId = 0;
        }

BailOut:

    return (((DWORD)wControl << 16) + (DWORD)wVirusId);     // merge return codes
}

#endif // ifdef 0


#if 0
/*@API:*************************************************************************
@Declaration:
    BOOL SYM_EXPORT VLSaveMemoryInfectionData ( HVLVIRUS hVirus );
@Description:
this function is called after scanning memory with VLScamMemory(),
and a virus was found in memory.  It checks whether the infection was caused
by the One_half virus and if so, it save repair information.

@Parameters:
$hVirus$    virus handle as returned by VLScanMemory()
$uFlags$    for future use

@Returns:
TRUE  if success
FALSE otherwise

@Include: avapi.h
*******************************************************************************/
BOOL SYM_EXPORT VLSaveMemoryInfectionData ( HVLVIRUS hVirus, UINT uFlags )
{
     LPBYTE      lpBuffer;
     HVCONTEXT   hContext;

     hContext = hVirus->lpVContext;

     SYM_ASSERT ( hContext );

     lpBuffer = (LPBYTE)VLTempMemoryAlloc(2048);

     if (lpBuffer == NULL)
         return ( FALSE );

     if ( hVirus->uDBEntryNum == VL_ONE_HALF_ID )
         {
         GetInfectionData ( lpBuffer, NULL, 0 );
         }

     VLTempMemoryFree(lpBuffer);
     return ( TRUE);
}

#endif // if 0


// =====================================================================
// end of Dead code below here
// =====================================================================
