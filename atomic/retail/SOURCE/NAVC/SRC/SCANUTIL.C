// Copyright 1993-1996 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/NAVC/VCS/scanutil.c_v   1.2   08 May 1997 15:09:26   TIVANOV  $
//
// Description:
//      These are the utility functions used by the scanner section.
//
// Contains:
//      AddVirusInfo()
//      RepairFile()
//      GetInfectedFile()
//      ExtInList()
//      ReadAllInfectedFiles()
//      ScanExcludeInfectedFile()
//      GetBytesToScan()
//      AddBytesScanned()
//      DoAllCheckWriteToFile()
//
// See Also:
//************************************************************************
// $Log:   S:/NAVC/VCS/scanutil.c_v  $
// 
//    Rev 1.2   08 May 1997 15:09:26   TIVANOV
// new char defines for the quarantine functionality
// 
//    Rev 1.1   07 Apr 1997 15:46:22   DALLEE
// Fixed typo(?) - Changed printf() to fprintf() for one of the autolog calls.
// Also, removed some unreferenced local variables.
// 
//    Rev 1.0   06 Feb 1997 20:56:22   RFULLER
// Initial revision
// 
//    Rev 1.0   31 Dec 1996 15:20:08   MKEATIN
// Initial revision.
// 
//    Rev 1.11   10 Dec 1996 18:54:16   JBELDEN
// fixed breaking out of scans by using gbcontinue
// 
//    Rev 1.10   27 Nov 1996 09:36:28   JBELDEN
// added fclose if log is open to scanhalt
// 
//    Rev 1.9   14 Nov 1996 11:54:04   JBELDEN
// removed unused code and cleaned up.
//
//    Rev 1.8   08 Nov 1996 16:56:06   JBELDEN
// removed call to mousehc
//
//    Rev 1.7   04 Nov 1996 14:34:16   JBELDEN
// removed dialog related code.
//
//    Rev 1.6   31 Oct 1996 14:45:32   JBELDEN
// removed code related to inoculation
//
//    Rev 1.5   16 Oct 1996 14:47:34   JBELDEN
// stubbed out routines not used by ttyscan.
//
//    Rev 1.4   15 Oct 1996 12:50:04   JBELDEN
// removed code related to bcontinue scan in ZipCallback
// so that scans of zips would complete
//
//    Rev 1.3   14 Oct 1996 11:25:42   JBELDEN
// changed DOSPrint's to printf and added fprintf for logging of
// drive errors.
//
//    Rev 1.2   09 Oct 1996 17:15:18   JBELDEN
// added code for drive read/write error for ttyscan
//
//    Rev 1.1   04 Oct 1996 15:49:06   JBELDEN
// added message for "system halted"
//
//    Rev 1.0   02 Oct 1996 12:58:40   JBELDEN
// Initial revision.
//************************************************************************

#include "platform.h"
#include "xapi.h"
#include "stddlg.h"
#include "stddos.h"
#include "file.h"
# include "disk.h"
#include "dosnet.h"

#include "navdprot.h"

#ifdef SYM_DOS
 #define BINDERY_OBJECT_TIME FAKE_BINDERY_OBJECT_TIME
#endif
#include "navutil.h"
#ifdef SYM_DOS
 #undef BINDERY_OBJECT_TIME
#endif

#include "options.h"
#include "syminteg.h"
#include "nonav.h"
#include "virscan.h"                    // For definition of ReLoad functions

#ifdef USE_ZIP
#include "symzip.h"
#endif
#include "scand.h"
#include "tsr.h"
#include "tsrcomm.h"
#include <stdio.h>
#include <time.h>

MODULE_NAME;

//************************************************************************
// DEFINES
//************************************************************************

#define ZIP_WARNING_NO_MEM      2

#ifdef USE_ZIP
// &? No longer true with LFN
#ifndef MAX_EXTENSION_SIZE
#define MAX_EXTENSION_SIZE  32
#endif

// &? No longer true with LFN
#ifndef MAX_FILE_SIZE
#define MAX_FILE_SIZE  12
#endif
#endif  // USE_ZIP

//************************************************************************
// TYPEDEFS
//************************************************************************

typedef struct tagSTATUS_STRING
    {
    WORD    wStatus;
    LPSTR   lpszMessage;
    LPSTR   lpszActionMessage;
    } STATUS_STRING, FAR *LPSTATUS_STRING;


//************************************************************************
// EXTERNS
//************************************************************************

extern char SZ_EMPTY                    [];
extern char SZ_INOC_NEW_FORMAT          [];
extern char SZ_INOC_CHANGE_FORMAT       [];
extern char SZ_INFECTED_FORMAT          [];
extern char SZ_ZIP_INFECTED_FORMAT      [];
extern char SZ_BOOT_INFECTED_FORMAT     [];
extern char SZ_MBR_INFECTED_FORMAT      [];
extern char SZ_MEMORY_INFECTED_FORMAT   [];
extern char SZ_SYSTEM_INOC_FILE_FORMAT  [];
extern char SZ_SYSTEM_INOC_BOOT_FORMAT  [];
extern char SZ_SYSTEM_INOC_MB_FORMAT    [];

extern char IDS_RAP_REPAIRED               [];
extern char IDS_RAP_NOTREPAIRED            [];
extern char IDS_RAP_DELETED                [];
extern char IDS_RAP_NOTDELETED             [];
extern char IDS_RAP_INFECTED               [];
extern char IDS_RAP_ZIPINFECTED            [];
extern char IDS_RAP_ERROR                  [];
extern char IDS_RAP_ERRORDELETED           [];
extern char IDS_RAP_ERRORNOTDELETED        [];
extern char IDS_RAP_INOCULATED             [];
extern char IDS_RAP_INOCULATEFAILED        [];
extern char IDS_RAP_REINOCULATED           [];
extern char IDS_RAP_NOTINOCULATED          [];
extern char IDS_RAP_INOCULATEDCHANGED      [];
extern char IDS_RAP_INOCDELETED            [];
extern char IDS_RAP_INOCNOTDELETED         [];
extern char IDS_RAP_BOOTINFECTED           [];
extern char IDS_RAP_BOOTREPAIRED           [];
extern char IDS_RAP_BOOTNOTREPAIRED        [];
extern char IDS_RAP_MASTERBOOTINFECTED     [];
extern char IDS_RAP_MASTERBOOTREPAIRED     [];
extern char IDS_RAP_MASTERBOOTNOTREPAIRED  [];
extern char IDS_RAP_INOCREPAIRED           [];
extern char IDS_RAP_INOCREPAIRFAILED       [];
extern char IDS_RAP_EXCLUDED_INOCULATION   [];
extern char IDS_RAP_EXCLUDED_VIRUSDETECT   [];
extern char IDS_RAP_EXCLUDE_FAILED         [];
extern char IDS_RAP_QUARANTINE             [];
extern char IDS_RAP_NOTQUARANTINE          [];


extern char IDS_FILESTATUS_INFECTED                [];
extern char IDS_FILESTATUS_ERROR                   [];
extern char IDS_FILESTATUS_ERRORDELETED            [];
extern char IDS_FILESTATUS_ERRORNOTDELETED         [];
extern char IDS_FILESTATUS_INOCULATED              [];
extern char IDS_FILESTATUS_REINOCULATED            [];
extern char IDS_FILESTATUS_NOTINOCULATED           [];
extern char IDS_FILESTATUS_INOCULATIONFAILED       [];
extern char IDS_FILESTATUS_INOCULATEREPAIRED       [];
extern char IDS_FILESTATUS_INOCULATEREPAIRFAILED   [];
extern char IDS_FILESTATUS_INOCULATECHANGED        [];
extern char IDS_FILESTATUS_REPAIRED                [];
extern char IDS_FILESTATUS_NOTREPAIRED             [];
extern char IDS_FILESTATUS_DELETED                 [];
extern char IDS_FILESTATUS_NOTDELETED              [];
extern char IDS_FILESTATUS_EXCLUDED                [];
extern char IDS_FILESTATUS_EXCLUDEFAILED           [];
extern char IDS_FILESTATUS_BOOTINFECTED            [];
extern char IDS_FILESTATUS_BOOTREPAIRED            [];
extern char IDS_FILESTATUS_BOOTNOTREPAIRED         [];
extern char IDS_FILESTATUS_MASTERBOOTINFECTED      [];
extern char IDS_FILESTATUS_MASTERBOOTREPAIRED      [];
extern char IDS_FILESTATUS_MASTERBOOTNOTREPAIRED   [];
extern char IDS_FILESTATUS_QUARANTINE_INFECTED       [];
extern char IDS_FILESTATUS_QUARANTINE_INFECTED_FAILED[];


                                        //********************************
                                        // Sorry for the extra include down
                                        // here, but it's not a normal
                                        // header.  It actually allocates
#include "filestat.h"                   // an array of FILESTATUSSTRUCT's
                                        // initialized with the addresses
                                        // of the strings above.
                                        // Done for compatability with the
                                        // windows code.
                                        //********************************

extern char IDS_BOOTREPAIRED        [];
extern char IDS_BOOTNOTREPAIRED     [];
extern char IDS_MBRREPAIRED         [];
extern char IDS_MBRNOTREPAIRED      [];
extern char IDS_FILENOTREPAIRED     [];
extern char IDS_FILEREPAIRED        [];
extern char IDS_FILEDELETED         [];
extern char IDS_FILENOTDELETED      [];
extern char IDS_FILEEXCLUDED        [];
extern char IDS_FILEEXCLUDED_INOC   [];
extern char IDS_FILENOTEXCLUDED     [];

extern BOOL  bAutoExportLog;
extern FILE  *fLogFile;


//************************************************************************
// STATIC VARIABLES
//************************************************************************

static BOOL bStopWatchEnabled = FALSE ; // Is it ok to do StopWatch stuff?

//************************************************************************
// LOCAL PROTOTYPES
//************************************************************************

BOOL STATIC PASCAL BuildListCallback (LPINFECTEDFILE lpInfected,
                                      LPNONAV        lpNoNav,
                                      WORD           wEntry,
                                      LPARAM         lParam     );

UINT LOCAL PASCAL SkipDriveDlgTimed ( BYTE  cDriveLetter,
                                      UINT  uTypeErr,
                                      DWORD dwTicks,
                                      UINT  uRvTimeout );

#ifdef USE_ZIP
int PASCAL LOADDS ZipCallBack(UINT           uAction,
                                     UINT           uError,
                                     LPZIPRECORD    lpZipRecord,
                                     DWORD          dwNoNav);
#endif
//************************************************************************
// AddVirusInfo()
//
// This routine addes the information from lpNoNav->Infected to
// the linked list of infected files in lpNoNav.
// If it can't it will set bContinueScan = FALSE and display an error msg.
//
// Parameters:
//      LPNONAV lpNoNav                 Info for this scan.
//
// Returns:
//      nothing (but will set gbContinueScan on failure)
//************************************************************************
// 4/22/93 DALLEE, copied from Martin's N_VIRUS.C
// 8/25/93 BarryG, now watching for memory errors and ending scan if needed
//************************************************************************

VOID PASCAL AddVirusInfo (LPNONAV lpNav)
{
    extern  BOOL            gbContinueScan ;

    auto    HGLOBAL         hNext;
    auto    LPINFARR        lpInfArr;
    auto    LPINFARR        lpInfArrLast ;
    auto    LPINFECTEDFILE  lpInfected;

    lpInfArr     = lpNav -> lpInfArr ;
    lpInfArrLast = NULL ;

    while ( lpInfArr )
        {
        if ( lpInfArr -> uNumUsed < MAX_INFECTSTRUCTS )
            break ;                     // Found room in existing link

        lpInfArrLast = lpInfArr ;
        lpInfArr     = (LPINFARR) lpInfArr -> lpNext ;
        }

    if ( lpInfArr == NULL )             // Need to add a link
        {
        hNext = MemAlloc ( GHND , sizeof ( INFARR ) ) ;

        if ( hNext != NULL )
            {
            lpInfArr = MemLock ( hNext ) ;

            lpInfArr -> hInf = MemAlloc ( GHND ,
                        (DWORD) sizeof (INFECTEDFILE) * MAX_INFECTSTRUCTS ) ;

            if ( lpInfArr -> hInf == NULL )
                {                       // Undo allocations if we didn't get
                                        // everything we needed.
                MemUnlock ( hNext, lpInfArr ) ;
                MemFree ( hNext ) ;
                lpInfArr = NULL ;       // Signal memory error for below
                }
            else
                {                       // If we got the link, fix up list
                if ( lpInfArrLast )
                    lpInfArrLast -> lpNext = (LPSTR) lpInfArr ;
                else
                    lpNav -> lpInfArr = lpInfArr ;
                }
            }
        }

    // At this point, we should have a pointer unless there as a memory error

    if ( lpInfArr )
        {                               // If we have somewhere to put it,
                                        // store the entry
        if ( NULL != (lpInfected = MemLock ( lpInfArr -> hInf )) )
            {
            MEMCPY(lpInfected + lpInfArr->uNumUsed,
                   &lpNav->Infected, sizeof(INFECTEDFILE));

            lpInfArr->uNumUsed++;
            MemUnlock(lpInfArr->hInf, lpInfected);
            }
        else
            {
            lpInfArr = NULL ;           // Signal error for below.
            }
        }

    // Scanner doesn't always listen the first time we tell it to stop
    // so don't keep popping up the dialog if stopping anyway.
    if ( lpInfArr == NULL && gbContinueScan )
        {                               // If NULL here, had a memory error
        gbContinueScan = FALSE ;
        }

} // End of AddVirusInfo()

//************************************************************************
// NAVDFreeLinkList()
//
// This routine frees the linked list of infected files in a NONAV struct.
//
// Parameters:
//      LPNONAV     lpNoNav
//
// Returns:
//      Nothing.
//************************************************************************
// 3/19/93 DALLEE lifted function from Martin N_VIRUS.C
//************************************************************************

VOID PASCAL NAVDFreeLinkList (LPNONAV lpNav)
{
    auto    LPINFARR    lpInfArr;
    auto    LPSTR       lpNextInfArr;

    lpNextInfArr = (LPSTR)lpNav->lpInfArr;

    while (lpNextInfArr)
        {
        lpInfArr = (LPINFARR)lpNextInfArr;
        lpNextInfArr = lpInfArr->lpNext;

                                        // Free the block of infected files
                                        // associated with this INFARR struct.
        MemRelease (lpInfArr->hInf);
        MemFree(lpInfArr->hInf);
                                        // Free this INFARR structure.
        MemFreePtr(lpInfArr);
        }
} // End NAVDFreeLinkList()


//************************************************************************
// GetInfectedFile()
//
// This routine copies the INFECTEDFILE information to the specified buffer.
//
// Parameters:
//      LPINFECTEDFILE  lpInfected      Pointer to buffer to place the
//                                          information.
//      LPNONAV         lpNoNav         Info on the current scan
//      WORD            dwIndex         The number of the INFECTEDFILE struct
//                                        The first struct is 0.
//
// Returns:
//      FALSE                           Could not retrieve INFECTEDFILE.
//      TRUE                            Copied the information.
//************************************************************************
// 3/30/93 DALLEE, Function created.
//************************************************************************

BOOL PASCAL GetInfectedFile (LPINFECTEDFILE lpDest,
                             LPNONAV        lpNoNav,
                             DWORD          dwIndex)
{
    auto    LPINFECTEDFILE  lpInfected = NULL;
    auto    LPINFARR        lpInfArr;
    auto    WORD            wArrayNum;
    auto    BOOL            bStatus = FALSE;


    lpInfArr = lpNoNav->lpInfArr;
                                        // wArrayNum is which INFARR the
                                        // INFECTEDFILE we want is in.
    for (wArrayNum = (WORD)(dwIndex / MAX_INFECTSTRUCTS);
         wArrayNum && lpInfArr;
         wArrayNum--)
        {
        lpInfArr = (LPINFARR) lpInfArr->lpNext;
        }

                                        // wArrayNum is now the index of
                                        // the INFECTEDFILE in the array
                                        // of INFECTEDFILE's.
    wArrayNum = (WORD)(dwIndex % MAX_INFECTSTRUCTS);

    if (lpInfArr && (wArrayNum < lpInfArr->uNumUsed))
        {
                                        // Lock the block of infected files
                                        // and copy the one we want.
        if (NULL != (lpInfected = MemLock(lpInfArr->hInf)))
            {
            MEMCPY(lpDest, lpInfected + wArrayNum, sizeof(INFECTEDFILE));
            MemUnlock(lpInfArr->hInf, lpInfected);
            bStatus = TRUE;
            }
        }
    return (bStatus);
} // End GetInfectedFile()


//************************************************************************
// PutInfectedFile()
//
// This routine updates the specified INFECTEDFILE record with the
// information in the buffer.
//
// Parameters:
//      LPINFECTEDFILE  lpInfected      Pointer to buffer with the new info.                                         information.
//      LPNONAV         lpNoNav         Info on the current scan.
//      WORD            dwIndex         The number of the INFECTEDFILE struct
//                                        The first struct is 0.
//
// Returns:
//      FALSE                           Could not retrieve INFECTEDFILE.
//      TRUE                            Copied the information.
//************************************************************************
// 3/30/93 DALLEE, Function created.
//************************************************************************

BOOL PASCAL PutInfectedFile (LPINFECTEDFILE lpSource,
                             LPNONAV        lpNoNav,
                             DWORD          dwIndex)
{
    auto    LPINFECTEDFILE  lpInfected = NULL;
    auto    LPINFARR        lpInfArr;
    auto    WORD            wArrayNum;
    auto    BOOL            bStatus = FALSE;


    lpInfArr = lpNoNav->lpInfArr;
                                        // wArrayNum is which INFARR the
                                        // INFECTEDFILE we want is in.
    for (wArrayNum = (WORD)(dwIndex / MAX_INFECTSTRUCTS);
         wArrayNum && lpInfArr;
         wArrayNum--)
        {
        lpInfArr = (LPINFARR) lpInfArr->lpNext;
        }

                                        // wArrayNum is now the index of
                                        // the INFECTEDFILE in the array
                                        // of INFECTEDFILE's.
    wArrayNum = (WORD)(dwIndex % MAX_INFECTSTRUCTS);

    if (lpInfArr && (wArrayNum < lpInfArr->uNumUsed))
        {
                                        // Lock the block of infected files and
                                        // copy the info to the one we want.
        if (NULL != (lpInfected = MemLock(lpInfArr->hInf)))
            {
            MEMCPY(lpInfected + wArrayNum, lpSource, sizeof(INFECTEDFILE));
            MemUnlock(lpInfArr->hInf, lpInfected);
            bStatus = TRUE;
            }
        }
    return (bStatus);
} // End PutInfectedFile()


//************************************************************************
// ExtInList()
//
// This routine determines whether the specified file should be scanned
// by checking whether or not Scan Program Files Only is set and if the
// extension is in the list of extensions in the NONAV struct.
//
// Parameters:
//      LPNONAV lpNoNav
//      LPSTR   lpFileName
//
// Returns:
//      TRUE                            Yes, it's in the list
//      FALSE                           Not in list.
//************************************************************************
// 3/24/93 DALLEE Function created.
// 7/29/93 DALLEE NameReturnFile() to make sure we have only a filename,
//              not a full path.
// 8/31/93 DALLEE, Save/restore action message at top of Scan Dialog.
//************************************************************************

BOOL PASCAL ExtInList (LPNONAV lpNoNav, LPSTR lpFileName)
{
#ifndef NAVSCAN
#ifdef USE_ZIP
    extern  LPSTR           LPSZ_ZIP_NO_MEM [];
    auto    char            szExt       [MAX_EXTENSION_SIZE];
    auto    UNZIP_STRUCT    UnZip;
    auto    char            szFullPath  [SYM_MAX_PATH + 1];
    auto    DWORD           dwTimedDlgTicks;
#endif
#endif  // !NAVSCAN
    auto    BOOL            bStatus = TRUE;
    auto    char            szFile      [SYM_MAX_PATH + 1];


    NameReturnFile(lpFileName, szFile);
#ifndef NAVSCAN
#ifdef USE_ZIP
    STRCPY(szFullPath, lpNoNav->szTheDirectory);
    NameAppendFile(szFullPath, szFile);

    if ( !lpNoNav->Zip.bInZip                 &&
         (lpNoNav->Gen.bScanZipFiles == TRUE) &&
         !ExcludeIs(lpNoNav->lpExclude, szFullPath, excVIRUS_FOUND) )
        {
        NameReturnExtension (szFile, szExt);

                                        // Scan within zip file if it matches
                                        // the zip extension, and we can
                                        // store the info for the dir, zipfile,
                                        // and compressed file.
        if ( !STRICMP(szExt, "ZIP") &&
             (STRLEN(lpNoNav->szTheDirectory) < SYM_MAX_PATH - MAX_FILE_SIZE) )
            {
            lpNoNav->Zip.bInZip = TRUE; // We are scanning a zip file

            lpNoNav->Zip.lpZipFileName = szFullPath;

            MEMSET (&UnZip, '\0', sizeof(UnZip));

            UnZip.uFlags          = UNZIPFLAG_TEMPNAME |
                                    UNZIPFLAG_FREE_BUFFERS |
                                    UNZIPFLAG_DELETE_AFTER;
            UnZip.dwUserData      = (DWORD)lpNoNav;
            UnZip.lpszZipFileName = szFullPath;
            UnZip.lpszMatchSpec   = NULL;
            UnZip.lpCallback      = ZipCallBack;


            ZipProcessFile (&UnZip); 

                                        // bScanZipFiles set in ZipCallback
                                        // due to memory error.  Call out of
                                        // memory dialog here when we can
                                        // actually alloc memory for it.
                                        // Kludgie, I know...
            if (ZIP_WARNING_NO_MEM == lpNoNav->Gen.bScanZipFiles)
                {
                dwTimedDlgTicks = lpNoNav->Alert.bRemoveAfter
                                  ? (DWORD)lpNoNav->Alert.uSeconds * TICKS_PER_SECOND
                                    : 0;

                lpNoNav->Gen.bScanZipFiles = StdDlgAlertTimed(LPSZ_ZIP_NO_MEM,
                                                 dwTimedDlgTicks,
                                                 TRUE,
                                                 lpNoNav->Zip.lpZipFileName);
                }

                                        // If a virus was found and we
                                        // should halt, do it here
            if ( lpNoNav->Infected.wStatus == FILESTATUS_ZIPINFECTED &&
                 lpNoNav->Opt.uAction == SCAN_HALTCPU )
                {
                ScanHalt ( lpNoNav );
                }

            lpNoNav->Zip.bInZip = FALSE;    // Done with the zip file

            }
        }
#endif  // USE_ZIP
#endif  // !NAVSCAN

    if (lpNoNav->Opt.bScanAll == FALSE)
        {
        bStatus = FIL4_IS_AT(szFile, (LPSTR)lpNoNav->Gen.szExt);
        }

    return (bStatus);
} // End ExtInList()

//************************************************************************
// ScanFoundCreateEntry()
//
// This routine creates the scanner Problems Found list entry given the
// infected object, name of the virus, and status.
//
// Parameters:
//      char *szEntry                   Buffer for completed entry
//      char *szName                    Infectee
//      char *szVirus                   Infector
//      BYTE byStatus                   Current status of this problem
//      WORD wIsFile                    if TRUE, treat szName as a filename
//                                       for truncating purposes.
//
// Returns:
//      Nothing
//************************************************************************
// 3/2/93 DALLEE Function created.
//************************************************************************

VOID PASCAL ScanFoundCreateEntry (char *szEntry, LPINFECTEDFILE lpInfected)
{
    extern  char    SZ_PHYSICAL_DRIVE [];
    auto    char    szLocalName     [SYM_MAX_PATH];
    auto    char    szLocalVirus    [SIZE_VIRUSNAME + 1];
    auto    char    *lpcZipFileName;

    if (lpInfected->wStatus == FILESTATUS_MASTERBOOTINFECTED
    ||  lpInfected->wStatus == FILESTATUS_MASTERBOOTREPAIRED
    ||  lpInfected->wStatus == FILESTATUS_MASTERBOOTNOTREPAIRED)
    {
        SPRINTF (szLocalName, SZ_PHYSICAL_DRIVE, *lpInfected->szFullPath);
    }
    else if (lpInfected->wStatus == FILESTATUS_ZIPINFECTED)
    {
        lpcZipFileName = lpInfected->szFullPath;
        while (*lpcZipFileName != EOS)
        {
            lpcZipFileName=AnsiNext(lpcZipFileName);
            if (*lpcZipFileName == ZIP_SEPARATOR)
            {
                lpcZipFileName=AnsiNext(lpcZipFileName);
                break;
            }
        }
        STRCPY(szLocalName, lpcZipFileName);
    }
    else
    {
        NameReturnFile(lpInfected->szFullPath, szLocalName);
    }

                                    // Kludge for boot records -
                                    // infected string has no filename.
    if (szLocalName[0] == EOS)
        {
        STRNCPY(szLocalName, lpInfected->szFullPath, 2);
        szLocalName[2] = EOS;
        }

    STRNCPY(szLocalVirus, lpInfected->Notes.lpVirName, SIZE_VIRUSNAME);
    szLocalVirus [sizeof(szLocalVirus) - 1] = EOS;

    FastStringPrint(szEntry,
                    " %12s1   %21s2 %18s3",
                    szLocalName,
                    szLocalVirus,
                    ScanFoundReturnStatusString(lpInfected->wStatus));

} // End ScanFoundCreateEntry()


//************************************************************************
// ScanFoundReturnStatusString()
//
// Returns pointer to the correct status string based on the
// WORD FILESTATUS_XXX passed.
//
// Parameters:
//      WORD wStatus                    One of the FILESTATUS_XXX #defines.
//
// Returns:
//      char *                          Pointer to correct status string
//                                      or zero-length string, if not in
//                                      look-up array.
//************************************************************************
// 7/12/93 DALLEE, Function created.
//************************************************************************

char * PASCAL ScanFoundReturnStatusString (WORD wStatus)
{
    return (FileStatus[wStatus].uFileStatusString);
} // End ScanFoundReturnStatusString()


//************************************************************************
// ScanFoundReturnMessageBoxString()
//
// Returns pointer to the correct problems found message box string
// based on the WORD FILESTATUS_XXX passed.
//
// Parameters:
//      WORD wStatus                    One of the FILESTATUS_XXX #defines.
//
// Returns:
//      char *                          Pointer to correct message box string.
//************************************************************************
// 7/29/93 DALLEE, Function created.
//************************************************************************

char * PASCAL ScanFoundReturnMessageBoxString (WORD wStatus)
{
    return (FileStatus[wStatus].uFollowHelp);
} // End ScanFoundReturnMessageBoxString()


//************************************************************************
// ReadAllInfectedFiles()
//
// This routine goes through the linked list of INFECTEDFILEs in the
// NONAV structure and calls the specified callback procedure for each one.
// The callback procedure should return FALSE to stop reading, or TRUE to
// continue.  wEntry holds the position of the INFECTEDFILE in the
// linked list.  First position = 0.
//
// Parameters:
//      LPNONAV     lpNoNav             Pointer to structure with scan and
//                                      infected file information.
//
//      BOOL PASCAL (*procCallback)(LPINFECTEDFILE, LPNONAV, WORD, LPARAM)
//                  if this callback returns FALSE, then process will stop.
//
//      LPARAM      lParam              Parameter for the callback procedure.
//
// Returns:
//      Nothing
//************************************************************************
// 4/7/93 DALLEE, Function created.
//************************************************************************

VOID PASCAL ReadAllInfectedFiles(
    LPNONAV lpNoNav,
    BOOL (PASCAL *procCallback)( LPINFECTEDFILE lpInfected,
                                 LPNONAV        lpNoNav,
                                 WORD           wEntry,
                                 LPARAM         lParam     ),
    LPARAM lParam)
{
    auto    LPINFARR        lpInfArr;
    auto    LPINFECTEDFILE  lpInfected;
    auto    WORD            wInfIndex;
    auto    WORD            wEntry = 0;

    lpInfArr = lpNoNav->lpInfArr;

                                        // This loops through all the arrays
                                        // of INFECTEDFILE structures.
    while (lpInfArr)
        {
                                        // Do the callback procedure for
                                        // each INFECTEDFILE in each array
                                        // of INFECTEDFILE's.
        if (NULL != (lpInfected = MemLock(lpInfArr->hInf)))
            {
            for (wInfIndex = 0; wInfIndex < lpInfArr->uNumUsed; wInfIndex++)
                {
                if (!(*procCallback)(lpInfected + wInfIndex,
                                lpNoNav, wEntry, lParam))
		    {
                    MemUnlock(lpInfArr->hInf, lpInfected);
		    return;
		    }
                wEntry++;
                }
            MemUnlock(lpInfArr->hInf, lpInfected);
                                        // If there are more INFARR's,
                                        // lpNext is the address. Otherwise,
                                        // it is NULL.
            lpInfArr = (LPINFARR) lpInfArr->lpNext;
            }
        }
} // End ReadAllInfectedFiles()


//***************************************************************************
// EnableStopWatch()
//
// Description:
//  Enables/Disables ScanStopWatch() tracking.  Use at start and stop
//  of regions you want to time.  Functions that interrupt things (i.e.,
//  dialog, etc.) will start and stop the StopWatch.  This function controls
//  whether they will work.
//
// Parameters:
//
// Return Value:
//
// See Also:
//
//***************************************************************************
// 09/16/1993 DALLEE Function Created.
//***************************************************************************

VOID PASCAL EnableStopWatch ( BOOL bEnabled )
{
    bStopWatchEnabled = bEnabled;
}

//************************************************************************
// ScanStopWatch()
//
// This routine is used to keep track of the time spent scanning.
// lpNav->Stats.lTimeScan should be set to zero before this function is
// called for the first time.
//
// Parameters:
//      LPNONAV     lpNav               Pointer to current scan info structure
//      BOOL        bStart              TRUE is start timer, FALSE stop.
//
// Returns:
//      nothing
//************************************************************************
// 4/28/93 DALLEE, Mostly borrowed from Martin ScanTime() in N_SCAN.C
//************************************************************************

VOID PASCAL ScanStopWatch (LPNONAV lpNav, BOOL bStart)
{
    static  BOOL    bRunning;

    auto    time_t  timeNow;

    if ( ! bStopWatchEnabled )
        return ;                        // Not even timing yet.

    timeNow = time(NULL);

    if (bStart)                         // Start the clock.
        {
        lpNav->Stats.lScanStart = timeNow - lpNav->Stats.lTimeScan;
        bRunning = TRUE;
        }
    else if (bRunning)                  // Stop the clock if it's been started.
        {
        lpNav->Stats.lTimeScan  = timeNow - lpNav->Stats.lScanStart;
        bRunning = FALSE;
        }

} // End ScanStopWatch()


//************************************************************************
// AddBytesScanned()
//
// This routine adds the number of bytes scanned for a file to the
// total kilobytes scanned, adjusting for cluster size.
//
// Parameters:
//      LPNONAV     lpNoNav             Pointer to info for this scan.
//      DWORD       dwBytes             Bytes to add.
//
// Returns:
//      Nothing
//************************************************************************
// 5/28/93 DALLEE, Function created.
//************************************************************************

VOID PASCAL AddBytesScanned (LPNONAV lpNoNav, DWORD dwBytes)
{
                                        // Make double sure we don't have
                                        // a zero here during division.
    if (0 == lpNoNav->dwClusterSize ||
        -1 == lpNoNav->dwClusterSize)
        {
        lpNoNav->dwClusterSize = 4096;
        }

    if (dwBytes)                        // Decrement dwBytes since we are
        {                               // going to fudge the number of
        dwBytes--;                      // clusters up by one after we
        }                               // divide to include unused space.
    dwBytes = (dwBytes/lpNoNav->dwClusterSize + 1) * lpNoNav->dwClusterSize;
    dwBytes /= 1024;

                                        // Don't trust byte counters to
                                        // not wrap.
    if ((lpNoNav->dwScannedKBytes + dwBytes) > lpNoNav->dwScannedKBytes)
        {
        lpNoNav->dwScannedKBytes = min(lpNoNav->dwTotalKBytes,
                                       lpNoNav->dwScannedKBytes + dwBytes);
        }
} // AddBytesScanned()




//************************************************************************
// ScanHalt()
//
// Displays infection message and halts the computer.
//
// Parameters:
//      LPNONAV     lpNoNav             Info on this scan
//
// Returns:
//      Nothing
//************************************************************************
// 6/1/93 DALLEE, Function created.
//************************************************************************

VOID PASCAL ScanHalt (LPNONAV lpNoNav)
{
    extern  char        szSystemHalted[];
    extern  FILE        *fLogFile;
    extern  BOOL        bAutoExportLog;


    if ( bAutoExportLog )
         {
         fclose(fLogFile);
         }


    printf(szSystemHalted);
    // Shutdown stuff and close files

    DiskErrorOn((ERRORTRAPPROC)-1);     // Don't let critical errors stop us
    VMMTerminate();                     // Remove .SWP file (do last!)

    DiskDisableCache();                 // Flush and turn off cache

    //&? SYS4_HALT();   Moved code here since I'm the only remaining client...
    _asm
        {
        mov     ah, 0Dh                 ; Flush buffers before we halt
        int     21h

        cli
    J_0:
        jmp     J_0
        }

} // End ScanHalt();

//************************************************************************
// GetBytesSkipDrive()
//
// This is the callback routine for GetBytesToScan() for removing a drive
// from the list of items to scan.
//
// Parameters:
//      LPSTR   *lpszzScanList          Pointer to address of items to scan.
//      DWORD   dwUnused                Unused user data parameter.
//
// Returns:
//      Nothing.
//************************************************************************
// 9/03/93 DALLEE, Function created.
//************************************************************************

VOID SYM_EXPORT WINAPI GetBytesSkipDrive (LPSTR *lpszzScanList, DWORD dwUnused)
{
    auto    char    szDriveToRemove [5];
                                        // Copy in "<drive>:\" and remove
                                        // any scan list entries on the
                                        // same drive.
    StringMaxCopy(szDriveToRemove, *lpszzScanList, 3);
    SzzStripMatches(*lpszzScanList, szDriveToRemove, TRUE);

} // End GetBytesSkipDrive()


//************************************************************************
// GetBytesDriveError()
//
// This the callback routine for GetBytesToScan() to handle errors accessing
// a drive.
//
// Parameters:
//      BYTE    byDrive                 Drive being accessed.
//      DWORD   dwNoNav                 Address of NONAV struct.
//
// Returns:
//      DRIVEERROR_RETRY                Keep trying to access this drive.
//      DRIVEERROR_CONTINUE             Ignore the error and go to the next.
//      DRIVEERROR_SKIP                 Abort this drive.
//************************************************************************
// 9/03/93 DALLEE, Function created.
//************************************************************************

UINT SYM_EXPORT WINAPI GetBytesDriveError (BYTE byDrive, DWORD dwNoNav)
{
    extern  BOOL            bScreenStarted;

    auto    UINT            uSkipResult;


    uSkipResult = SkipDriveTimedErrDialog (byDrive,
                                           SKIPDRIVE_DLG_ACCESS,
                                           SKIPDRIVE_ABORT,
                                           (LPNONAV)dwNoNav);

    switch (uSkipResult)
        {
        case SKIPDRIVE_IGNORE:
            uSkipResult = DRIVEERROR_CONTINUE;
            break;

        case SKIPDRIVE_ABORT:
            uSkipResult = DRIVEERROR_SKIP;
            break;

        default:
        case SKIPDRIVE_RETRY:
            uSkipResult = DRIVEERROR_RETRY;
            break;
        }

    return (uSkipResult);

} // End GetBytesDriveError();


//***************************************************************************
// SkipDriveTimedErrDialog
//
// Description:
//      Displays timed dialog allowing user to retry or remove drive from scan
//      Code core copied from StdDlgAlertTimed().  I needed to have 3 buttons
//      and a little different control so I just copied the code in.  This
//      means that StdDlgAlertTimed() is probably unused in DOSDLG:stdalert.c
//
// Parameters:
//      cDriveLetter    - Drive letter displayed in message
//      uTypeErr        - changes message displayed
//      dwTicks         - timeout in ticks (0=no timeout)
//      uRvTimeout      - If timeout occurs, return this value
//      lpNoNav         - Needed to stop and start timers
//
// Return Value: (same as critical error handler returns)
//      SKIPDRIVE_IGNORE -- Ignore the error and continue.
//      SKIPDRIVE_RETRY  -- Retry the operation
//      SKIPDRIVE_ABORT  -- Abort and drop drive from scan list (no disk, etc.)
//           (2 is the default if a timeout occurs)
//***************************************************************************
// 08/31/1993 BARRY Function Created.
//***************************************************************************

UINT PASCAL SkipDriveTimedErrDialog ( BYTE cDriveLetter,
                                      UINT uTypeErr,
                                      UINT uRvTimeout,
                                      LPNONAV lpNoNav )
{
    auto    DWORD           dwTimeout ;
    auto    UINT            uRv ;

    ScanStopWatch(lpNoNav, FALSE);

    dwTimeout = 0L ;

    if ( lpNoNav->Alert.bRemoveAfter )
        dwTimeout = (DWORD) ((lpNoNav->Alert.uSeconds * 182L)/10L) ;

    uRv = SkipDriveDlgTimed(cDriveLetter, uTypeErr, dwTimeout, uRvTimeout);

    ScanStopWatch(lpNoNav, TRUE) ;

    return uRv ;
}

//***************************************************************************
// SkipDriveDlgTimed()
//
// Description:
//      Template copied from DOSDLG:stdalert:StdDlgAlertTimed().
//      Added custom dialog and specific return values.
//
// Parameters:
//      cDriveLetter    - Drive letter displayed in message
//      bTypeErr        - changes message displayed
//      dwTicks         - timeout in ticks (0=no timeout)
//      uRvTimeout      - If timeout occurs, return this value
//
// Return Value:
//      SKIPDRIVE_IGNORE -- Ignore the error and continue.
//      SKIPDRIVE_RETRY  -- Retry the operation
//      SKIPDRIVE_ABORT  -- Abort and drop drive from scan list (no disk, etc.)
//
//***************************************************************************
// 09/01/1993 BARRY Function Created.
//***************************************************************************

UINT LOCAL PASCAL SkipDriveDlgTimed ( BYTE  cDriveLetter,
                                      UINT  uTypeErr,
                                      DWORD dwTicks,
                                      UINT  uRvTimeout )
{
    extern      char          szBootReadError[] ;
    extern      char          szGeneralReadError[] ;
    extern      char          szWriteError[];

    switch (uTypeErr)
        {
        case SKIPDRIVE_DLG_BOOT:
            printf(szBootReadError,cDriveLetter);
            if ( bAutoExportLog )
               fprintf(fLogFile, szBootReadError,cDriveLetter);
            break;

        case SKIPDRIVE_DLG_WRITE:
            printf(szWriteError,cDriveLetter);
            if ( bAutoExportLog )
               fprintf(fLogFile, szWriteError,cDriveLetter);
            break;

        default:
        case SKIPDRIVE_DLG_ACCESS:
            printf(szGeneralReadError,cDriveLetter);
            if ( bAutoExportLog )
               fprintf(fLogFile, szGeneralReadError,cDriveLetter);
            break;
        }

        return SKIPDRIVE_ABORT ;
} // End SkipDriveDlgTimed()



//************************************************************************
// CreateTemporaryRepairFile()
//
// Used during file repairs to create a temporary duplicate of a file
// being modified.  The file is created, written, and then closed.
//
// Parameters:
//      LPSTR   lpNewFile               Buffer to place name of new file.
//      LPCSTR  lpOldFile               Name of file being copied.
//
// Returns:
//      TRUE                            Successful.
//      FALSE                           File was not created.
//************************************************************************
// 6/7/93 DALLEE, Function created.
//************************************************************************

BOOL PASCAL CreateTemporaryRepairFile (LPSTR lpNewFile, LPCSTR lpOldFile)
{
    auto    HFILE   hNew;
    auto    BOOL    bResult = FALSE;

    NameReturnPath(lpOldFile, lpNewFile);

    if ((hNew = FileCreateTemporary(lpNewFile, FA_NORMAL)) != HFILE_ERROR)
        {
        FileClose(hNew);
        bResult = (COPY_SUCCESSFUL == FileCopy(lpOldFile, lpNewFile, NULL));

        if (FALSE == bResult)
            {
            FileDelete(lpNewFile);
            }
        }

    return (bResult);
} // End CreateTemporaryRepairFile()

#ifdef USE_ZIP
//************************************************************************
// ZipCallBack()
//
// Callback function for zip scanning.
//
// Parameters:
//      UINT        uAction             Message describing current action.
//      UINT        uError              Either error or percent done.
//      LPZIPRECORD lpZipRecord         Info on the zip file and record.
//      DWORD       dwNoNav             User Data - pointer to our NONAV struct.
//
// Returns:
//      int                             Action for Zip engine to take.
//************************************************************************
// 7/13/93 DALLEE, Taken from Martin's N_SCAN.C
// 8/03/93 DALLEE, NameConvertFwdSlashes for unix-type paths from zip engine.
// 8/12/93 DALLEE, Use only filename portion of lpFileName, discard path.
// 8/31/93 DALLEE, Display "Uncompressing file XXX from XXX.ZIP" message.
// 6/06/94 DALLEE, Limit zip child file to MAX_FILE_SIZE.
//************************************************************************

int PASCAL LOADDS ZipCallBack(UINT           uAction,
                              UINT           uError,
                              LPZIPRECORD    lpZipRecord,
                              DWORD          dwNoNav)
{
    extern  BOOL            gbContinueScan;
    extern  char            SZ_UNCOMPRESSING_FILE_FORMAT [];

    auto    char            szUncompressing [80 + 1];
    auto    char            szZipFile       [MAX_PATH_SIZE + 1];
    auto    LPNONAV         lpNav;
    auto    int             nReturn;


    lpNav = (LPNONAV)dwNoNav;           // Get our good ol' pointer

    switch (uAction)
        {
        case ZIP_ERROR:
            if (ENO_MEM_CE == uError)
                {
                lpNav->Gen.bScanZipFiles = ZIP_WARNING_NO_MEM;
                }
            nReturn = ZIP_SKIP;
            break;

        case ZIP_WARNING:
            if ( (WNO_MEM_C == uError) ||
                 (WNO_MEM_CPA == uError) ||
                 (WNO_MEM_B == uError) )
                {
                lpNav->Gen.bScanZipFiles = ZIP_WARNING_NO_MEM;
                }
            nReturn = ZIP_SKIP;
            break;

        case ZIP_FILESPEC:
                                        // -----------------------------------
                                        // Check if the extension of this file
                                        // is one of the extensions to scan.
                                        // Also, if it's excluded from virus
                                        // checking, we can ignore it.
                                        // -----------------------------------

                                        // Get filename.
            NameReturnFile(lpZipRecord->lpszBuffer, szZipFile);
                                        // Shorten to fit in buffer, if needed.
//            STRNCPY(lpNav->Zip.szZipChildFile, szZipFile,
//                    sizeof(lpNav->Zip.szZipChildFile)-1);

            STRNCPY(lpNav->Zip.szZipChildFile, szZipFile,
                    MAX_FILE_SIZE);

            lpNav->Zip.szZipChildFile[sizeof(lpNav->Zip.szZipChildFile)-1]=EOS;

//            if ( (ExtInList(lpNav, lpNav->Zip.szZipChildFile)) &&
//                 (!ExcludeIs(lpNav->lpExclude,lpNav->Zip.szZipChildFile,excVIRUS_FOUND)))
            if ( (ExtInList(lpNav, szZipFile)) &&
                 (!ExcludeIs(lpNav->lpExclude,szZipFile,excVIRUS_FOUND)))
                {
                NameReturnFile(lpNav->Zip.lpZipFileName, szZipFile);
                SPRINTF(szUncompressing,
                        SZ_UNCOMPRESSING_FILE_FORMAT,
                        lpNav->Zip.szZipChildFile,
                        szZipFile);

                nReturn = ZIP_EXTRACT;
                }
            else
                {
                nReturn = ZIP_SKIP;
                }
            break;

        case ZIP_EXTRACTED:
                                        // Save the zip filename on the
                                        // stack before swapping out the
                                        // ZIP Overlay.
            STRCPY (szZipFile, (LPSTR) lpZipRecord->lpszBuffer);

            StartScanFile(lpNav, szZipFile);

            nReturn = (gbContinueScan && lpNav->Gen.bScanZipFiles )
                      ? ZIP_EXTRACT : ZIP_ABORT;
            break;

        default:
            nReturn = ZIP_SKIP;
            break;
        }

    return (nReturn);
} // End ZipCallBack()
#endif  // USE_ZIP

//************************************************************************
// DoAllCheckWriteToFile()
//
// This routine checks the ability to write to a file and asks the user
// whether to retry, ignore, or skip writes to the drive on error.
//
// Parameters:
//      LPSTR   lpszFile                Full path (with drive) to file.
//      LPNONAV lpNoNav                 Scan info.
//
// Returns:
//      SKIPDRIVE_IGNORE                Skip this file only.
//      SKIPDRIVE_ABORT                 Skip entire drive.
//      SKIPDRIVE_RETRY                 Go ahead with this file.
//************************************************************************
// 9/15/93 DALLEE, Function created.
//************************************************************************

UINT PASCAL DoAllCheckWriteToFile (LPSTR lpszFile, LPNONAV lpNoNav)
{
    extern  WORD    trapping_enabled;

    auto    WORD    wOldTrap;
    auto    UINT    uAttributes;
    auto    UINT    uRetVal;

    wOldTrap         = trapping_enabled;
    trapping_enabled = FALSE;

    do
        {
        if (NOERR == FileGetAttr(lpszFile, &uAttributes))
            {
            do
                {
                TSR_OFF;
                uRetVal = FileSetAttr(lpszFile, uAttributes);
                TSR_ON;

                if (NOERR == uRetVal)
                    {                   // Write to file was successful,
                                        // go ahead with action on this file.
                    uRetVal = SKIPDRIVE_RETRY;
                    break;
                    }
                else
                    {                   // Ask user Retry, Ignore, Skip?
                    uRetVal = SkipDriveTimedErrDialog(lpszFile[0],
                                                      SKIPDRIVE_DLG_WRITE,
                                                      SKIPDRIVE_IGNORE,
                                                      lpNoNav);
                    }
                }                       // Retry as much as the user wants.
                while (uRetVal == SKIPDRIVE_RETRY);

                                        // We have our result, break and
            break;                      // return Retry, Ignore, Abort.
            }
                                        //--------------------------------
        else                            // Couldn't get attributes
            {                           //--------------------------------
                                        // Ask user Retry, Ignore?
            uRetVal = SkipDriveTimedErrDialog(lpszFile[0],
                                              SKIPDRIVE_DLG_ACCESS,
                                              SKIPDRIVE_IGNORE,
                                              lpNoNav);
            }
        }
        while (uRetVal == SKIPDRIVE_RETRY);

    trapping_enabled = wOldTrap;
    FileCheckError();

    return (uRetVal);

} // End DoAllCheckWriteToFile()
