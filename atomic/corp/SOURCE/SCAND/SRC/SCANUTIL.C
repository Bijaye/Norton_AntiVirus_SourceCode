// Copyright 1993 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/SCAND/VCS/scanutil.c_v   1.1   13 May 1997 11:29:48   KSACKIN  $
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
// $Log:   S:/SCAND/VCS/scanutil.c_v  $
// 
//    Rev 1.1   13 May 1997 11:29:48   KSACKIN
// Added externs for new Quarentine string definitions.
//
//    Rev 1.0   06 Feb 1997 21:08:54   RFULLER
// Initial revision
//
//    Rev 1.5   18 Nov 1996 15:54:54   MKEATIN
// We now wait a second for the Dialog to display before halting the system.
// The Korean driver appears to be a bit slow.
//
//    Rev 1.4   30 Sep 1996 15:59:22   JALLEE
// DBCS work.
//
//    Rev 1.3   24 Sep 1996 14:12:22   JALLEE
// Removed alert message from the ScanActionMessage dialog.
//
//    Rev 1.2   08 Aug 1996 15:44:16   JBRENNA
// LuigiPlus Port:
//   DALLEE: Bullet-proof status string routines to not return NULL ptrs.
//
//   YVANG: Corrected the line-up problem under the headings in the
//     'Problems Found' screen when HD MBR is infected.  (Defect No. 58212)
//
//    Rev 1.1   08 Aug 1996 13:26:02   JBRENNA
// Ported from DOSNAV environment
//
//
//    Rev 1.106   02 Jul 1996 11:34:46   JALLEE
// Added SZ_ZIP_INFECTED_FORMAT to StatusStrings, fixes bug in notify dialog for zips.
//
//    Rev 1.105   26 Jun 1996 09:43:40   JALLEE
// Corrected Problems Found list generation for entries containing infected
// zip files.
//
//    Rev 1.104   06 May 1996 09:08:10   MZAREMB
// Initial Pegasus update: added zip and navscan support via #ifdefs.
//
//    Rev 1.103   17 Nov 1995 19:32:54   MKEATIN
// Made changes to scan messages - uses five line instead of three.
//
//    Rev 1.102   01 Nov 1995 11:25:02   JWORDEN
// Use new MBR repaired and not repaired strings for MBR instead of
// just the boot record strings
// Use new lower case hard drive string for sentence creation
//
//    Rev 1.101   26 Oct 1995 10:33:54   JWORDEN
// Fix Problems Found list for master boot drive label
//
//    Rev 1.100   24 Oct 1995 20:41:40   JWORDEN
// Process MBR file name as a single digit drive number
//
//    Rev 1.99   07 Jun 1995 10:27:28   BARRY
// Remove ZIP dependencies
//
//    Rev 1.98   24 Mar 1995 15:49:36   DALLEE
// Added debug/prerelease only switch "/NOHALT" which allows continue after
// the system halted dialog.
//
//    Rev 1.97   21 Mar 1995 20:25:56   DALLEE
// Moved SYS4_HALT routine into ScanHalt() since NAVBOOT seems to be the
// only client for it now, and it's been removed from CTSN.H
//
//    Rev 1.96   15 Mar 1995 17:46:18   DALLEE
// On write log error, make sure we pop up if screen IO was hidden.
//
//    Rev 1.95   15 Mar 1995 14:52:38   DALLEE
// Init the display if hidden for the GetBytesToScan() error handler.
//
//    Rev 1.94   14 Mar 1995 17:46:00   DALLEE
// Hide display during startup util problems are found.
//
//    Rev 1.93   16 Feb 1995 21:35:56   DALLEE
// Clean up the mem infected dialog.
// Still need to replace the sys4_halt call.
//
//    Rev 1.92   29 Dec 1994 16:46:22   DALLEE
// Include syminteg.h before nonav.h
//
//    Rev 1.91   28 Dec 1994 14:15:58   DALLEE
// Latest NAVBOOT revision.
//
    //    Rev 1.5   27 Dec 1994 19:36:50   DALLEE
    // Convert to use CERTLIBN.
    //
    //    Rev 1.4   13 Dec 1994 15:41:42   DALLEE
    // Put the Problems Found list entry format string back to match
    // with FastStringPrint().
    //
    //    Rev 1.3   12 Dec 1994 16:15:36   DALLEE
    // Changed SPRINTF's to FastStringPrints() so format strings wouldn't have
    // to be modified (also preserves %p functionality).
    //
    //    Rev 1.2   09 Dec 1994 19:46:16   DALLEE
    // Fixed format problem in Problems Found list.
    //
    //    Rev 1.1   05 Dec 1994 18:10:26   DALLEE
    // CVT1 script.
    //
//    Rev 1.90   28 Dec 1994 13:53:34   DALLEE
// Basis for NAVBOOT code from NAV v3.05.
// First (.DX) port to trunk is previous version labeled "PRENAVBOOT".
//
    //    Rev 1.0   23 Nov 1994 15:38:50   DALLEE
    // Initial revision.
    //
    //    Rev 1.84   01 Nov 1994 22:19:16   DALLEE
    // Added action messages for Inoc Changed - deleted.
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

#define CREATE_MESSAGE_BUFFER   511
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

extern char IDS_FILESTATUS_INFECTED                   [];
extern char IDS_FILESTATUS_ERROR                      [];
extern char IDS_FILESTATUS_ERRORDELETED               [];
extern char IDS_FILESTATUS_ERRORNOTDELETED            [];
extern char IDS_FILESTATUS_INOCULATED                 [];
extern char IDS_FILESTATUS_REINOCULATED               [];
extern char IDS_FILESTATUS_NOTINOCULATED              [];
extern char IDS_FILESTATUS_INOCULATIONFAILED          [];
extern char IDS_FILESTATUS_INOCULATEREPAIRED          [];
extern char IDS_FILESTATUS_INOCULATEREPAIRFAILED      [];
extern char IDS_FILESTATUS_INOCULATECHANGED           [];
extern char IDS_FILESTATUS_REPAIRED                   [];
extern char IDS_FILESTATUS_NOTREPAIRED                [];
extern char IDS_FILESTATUS_DELETED                    [];
extern char IDS_FILESTATUS_NOTDELETED                 [];
extern char IDS_FILESTATUS_EXCLUDED                   [];
extern char IDS_FILESTATUS_EXCLUDEFAILED              [];
extern char IDS_FILESTATUS_BOOTINFECTED               [];
extern char IDS_FILESTATUS_BOOTREPAIRED               [];
extern char IDS_FILESTATUS_BOOTNOTREPAIRED            [];
extern char IDS_FILESTATUS_MASTERBOOTINFECTED         [];
extern char IDS_FILESTATUS_MASTERBOOTREPAIRED         [];
extern char IDS_FILESTATUS_MASTERBOOTNOTREPAIRED      [];
extern char IDS_FILESTATUS_QUARANTINE_INFECTED        [];
extern char IDS_FILESTATUS_QUARANTINE_INFECTED_FAILED [];


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
extern char IDS_FILEINOCULATED      [];
extern char IDS_FILEINOCREPAIRED    [];
extern char IDS_FILEINOCNOTREPAIRED [];
extern char IDS_FILEINOCDELETED     [];
extern char IDS_FILEINOCNOTDELETED  [];
extern char IDS_FILENOTINOCULATED   [];
extern char IDS_FILEREINOCULATED    [];
extern char IDS_FILEEXCLUDED        [];
extern char IDS_FILEEXCLUDED_INOC   [];
extern char IDS_FILENOTEXCLUDED     [];


//************************************************************************
// STATIC VARIABLES
//************************************************************************

                                        // This array is used to look up
                                        // appropriate status text and
                                        // message formats based on an
                                        // infected file's filestatus.
//  ***FILESTATUS WORD***                 ***MESSAGE FORMAT STRING***
STATIC STATUS_STRING StatusStrings [] =
 {
  {FILESTATUS_INFECTED,                   SZ_INFECTED_FORMAT,       SZ_EMPTY},
  {FILESTATUS_BOOTINFECTED,               SZ_BOOT_INFECTED_FORMAT,  SZ_EMPTY},
  {FILESTATUS_MASTERBOOTINFECTED,         SZ_MBR_INFECTED_FORMAT,   SZ_EMPTY},
  {FILESTATUS_ZIPINFECTED,                SZ_ZIP_INFECTED_FORMAT,   SZ_EMPTY},
  {FILESTATUS_MEMORYINFECTED,             SZ_MEMORY_INFECTED_FORMAT,SZ_EMPTY},
#ifndef NAVSCAN
  {FILESTATUS_REPAIRED,                   SZ_EMPTY,                 IDS_FILEREPAIRED},
  {FILESTATUS_NOTREPAIRED,                SZ_INFECTED_FORMAT,       IDS_FILENOTREPAIRED},
  {FILESTATUS_DELETED,                    SZ_EMPTY,                 IDS_FILEDELETED},
  {FILESTATUS_NOTDELETED,                 SZ_INFECTED_FORMAT,       IDS_FILENOTDELETED},
  {FILESTATUS_EXCLUDE_INOCULATION,        SZ_EMPTY,                 IDS_FILEEXCLUDED_INOC},
  {FILESTATUS_EXCLUDE_VIRUSDETECTION,     SZ_EMPTY,                 IDS_FILEEXCLUDED},
  {FILESTATUS_EXCLUDE_FAILED,             SZ_INFECTED_FORMAT,       IDS_FILENOTEXCLUDED},
  {FILESTATUS_INOCULATED,                 SZ_EMPTY,                 IDS_FILEINOCULATED},
  {FILESTATUS_INOCULATIONFAILED,          SZ_INOC_NEW_FORMAT,       IDS_FILENOTINOCULATED},
  {FILESTATUS_INOCULATEREPAIRED,          SZ_EMPTY,                 IDS_FILEINOCREPAIRED},
  {FILESTATUS_INOCULATEREPAIR_FAILED,     SZ_INOC_CHANGE_FORMAT,    IDS_FILENOTREPAIRED},
  {FILESTATUS_INOCULATEDELETED,           SZ_EMPTY,                 IDS_FILEINOCDELETED},
  {FILESTATUS_INOCULATEDELETE_FAILED,     SZ_INOC_CHANGE_FORMAT,    IDS_FILEINOCNOTDELETED},
  {FILESTATUS_REINOCULATED,               SZ_EMPTY,                 IDS_FILEREINOCULATED},
  {FILESTATUS_REINOCULATIONFAILED,        SZ_INOC_CHANGE_FORMAT,    IDS_FILENOTINOCULATED},
  {FILESTATUS_NOTINOCULATED,              SZ_INOC_NEW_FORMAT,       SZ_EMPTY},
  {FILESTATUS_INOCULATEDCHANGED,          SZ_INOC_CHANGE_FORMAT,    SZ_EMPTY},
  {FILESTATUS_INOC_CHANGED_DELETED,       SZ_EMPTY,                 IDS_FILEDELETED},
  {FILESTATUS_INOC_CHANGED_NOTDELETED,    SZ_INOC_CHANGE_FORMAT,    IDS_FILENOTDELETED},
#endif
  {FILESTATUS_BOOTREPAIRED,               SZ_EMPTY,                 IDS_BOOTREPAIRED},
  {FILESTATUS_BOOTNOTREPAIRED,            SZ_BOOT_INFECTED_FORMAT,  IDS_BOOTNOTREPAIRED},
  {FILESTATUS_MASTERBOOTREPAIRED,         SZ_EMPTY,                 IDS_MBRREPAIRED},
  {FILESTATUS_MASTERBOOTNOTREPAIRED,      SZ_MBR_INFECTED_FORMAT,   IDS_MBRNOTREPAIRED},
#ifndef NAVSCAN
  {FILESTATUS_ERROR,                      SZ_EMPTY,                 SZ_EMPTY},
  {FILESTATUS_INTEG_MBREPAIRED,           SZ_EMPTY,                 SZ_EMPTY},
  {FILESTATUS_INTEG_MBREPAIRFAILED,       SZ_EMPTY,                 SZ_EMPTY},
  {FILESTATUS_INTEG_BOOTREPAIRED,         SZ_EMPTY,                 SZ_EMPTY},
  {FILESTATUS_INTEG_BOOTREPAIRFAILED,     SZ_EMPTY,                 SZ_EMPTY},
  {FILESTATUS_INTEG_SYSFILEREPAIRED,      SZ_EMPTY,                 SZ_EMPTY},
  {FILESTATUS_INTEG_SYSFILEREPAIRFAILED,  SZ_EMPTY,                 SZ_EMPTY},
  {FILESTATUS_INTEG_MBCHANGED,           SZ_SYSTEM_INOC_MB_FORMAT,  SZ_EMPTY},
  {FILESTATUS_INTEG_BOOTCHANGED,         SZ_SYSTEM_INOC_BOOT_FORMAT,SZ_EMPTY},
  {FILESTATUS_INTEG_SYSFILECHANGED,      SZ_SYSTEM_INOC_FILE_FORMAT,SZ_EMPTY},
  {FILESTATUS_INTEG_REINOCULATED,         SZ_EMPTY,                 SZ_EMPTY},
  {FILESTATUS_INTEG_REINOCULATEFAILED,    SZ_EMPTY,                 SZ_EMPTY},
  {FILESTATUS_INTEG_CREATED,              SZ_EMPTY,                 SZ_EMPTY},
  {FILESTATUS_INTEG_CREATEFAILED,         SZ_EMPTY,                 SZ_EMPTY},
  {FILESTATUS_INTEG_NOTEXIST,             SZ_EMPTY,                 SZ_EMPTY},
#endif  // !NAVSCAN
  {NULL,                                  NULL,                     NULL},
 };


static BOOL bStopWatchEnabled = FALSE ; // Is it ok to do StopWatch stuff?

//************************************************************************
// LOCAL PROTOTYPES
//************************************************************************

VOID LOCAL PASCAL LogFullTimedErrDialog ( VOID ) ;
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
        ScanStopWatch(lpNav, FALSE);
        LogFullTimedErrDialog ( ) ;
        ScanStopWatch(lpNav, TRUE );
        }

} // End of AddVirusInfo()

//***************************************************************************
// LogFullTimedErrDialog
//
// Description: Displays timed message that the log is full and scan is ending
//
//***************************************************************************
// 08/26/1993 BARRY Function Created.
//***************************************************************************

VOID LOCAL PASCAL LogFullTimedErrDialog ( VOID )
{
    extern      DIALOG          dlgMessageLogFullOk ;
    extern      NAVOPTIONS      navOptions;
    extern      COLORS          alert_colors ;
    extern      BOOL            bScreenStarted;

    auto        DIALOG_RECORD   *dialog ;
    auto        WORD            wResult ;
    auto        DWORD           dwStartTime ;
    auto        DWORD           dwTimeout ;


    if (!bScreenStarted)
        {
        InitDisplay(TRUE);
        }

    if ( NULL == (dialog = DialogOpenC(&dlgMessageLogFullOk, & alert_colors)) )
        return ;                        // Couldn't open dialog ;

    if ( navOptions.alert.bRemoveAfter )
        {
        dwStartTime = TimerTicks ( ) ;
        dwTimeout   = (DWORD) navOptions.alert.uSeconds * TICKS_PER_SECOND ;
        }

    do
	{
	wResult = DialogGetEvent();

	if (wResult != NULL)
	    wResult = DialogProcessEvent(dialog,wResult);

	if (navOptions.alert.bRemoveAfter)
	    {
            if ( TimerElapsedTicks ( dwStartTime ) >= dwTimeout )
                break ;
	    }

	} while ((wResult != ACCEPT_DIALOG) && (wResult != ABORT_DIALOG));

    DialogClose(dialog,wResult);

} // End LogFullTimedErrDialog()


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
    extern  OutputFieldRec  ofScanOrInoculate;
#ifndef NAVSCAN
#ifdef USE_ZIP
    extern  LPSTR           LPSZ_ZIP_NO_MEM [];
    auto    char            szExt       [MAX_EXTENSION_SIZE];
    auto    UNZIP_STRUCT    UnZip;
    auto    char            szFullPath  [SYM_MAX_PATH + 1];
    auto    LPVOID          lpOldOutputData;
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

            lpOldOutputData = ofScanOrInoculate.data;

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

            DialogOutput(&ofScanOrInoculate, lpOldOutputData);
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
// ScanFoundCreateMessage()
//
// This routine creates the three line messages used by the action
// dialogs called from the Problems Found dialog.
//
// Parameters:
//      LPSTR   lpszMessage[5]          Message buffer(s)
//      LPINFECTEDFILE  lpInfected      Infected File information
//
// Returns:
//      Nothing.
//************************************************************************
// 4/6/93 DALLEE, Function created.
//************************************************************************

VOID PASCAL ScanFoundCreateMessage(LPSTR            *lpszMessage,
                                   LPINFECTEDFILE   lpInfected)
{
    extern  char    SZ_HARD_DRIVE_LC [];

    auto    char    *lpszFormat;
    auto    char    *lpszNextLine;
    auto    char    szBigBuffer [CREATE_MESSAGE_BUFFER + 1];
    auto    char    szFile      [SYM_MAX_PATH];
    auto    char    *lpszZipFile;
    auto    char    szLocalVirus[SIZE_VIRUSNAME + 1];
    auto    WORD    wCounter;

                                        // Pick up the correct message
                                        // format string from the StatusStrings
                                        // array.
    for (wCounter = 0, lpszFormat = SZ_EMPTY;
         StatusStrings[wCounter].wStatus;
         wCounter++)
        {
        if (StatusStrings[wCounter].wStatus == lpInfected->wStatus)
            {
            lpszFormat = StatusStrings[wCounter].lpszMessage;
            break;
            }
        }

    if (lpInfected->wStatus == FILESTATUS_MASTERBOOTINFECTED
    ||  lpInfected->wStatus == FILESTATUS_MASTERBOOTREPAIRED
    ||  lpInfected->wStatus == FILESTATUS_MASTERBOOTNOTREPAIRED)
    {
        SPRINTF (szFile, SZ_HARD_DRIVE_LC, *lpInfected->szFullPath);
    }
    else
    {
        STRCPY(szFile, lpInfected->szFullPath);
    }

                                        // If it's a zip file, split the
                                        // name into zip portion and
                                        // compressed file portion.
    if (lpInfected->wStatus == FILESTATUS_ZIPINFECTED)
        {
        lpszZipFile=szFile;
        while(*lpszZipFile != EOS)
            {
            if (*lpszZipFile == ZIP_SEPARATOR)
                {
                *lpszZipFile = EOS;
                lpszZipFile++;
                break;
                }
            lpszZipFile = AnsiNext(lpszZipFile);
            }
        }

    STRNCPY(szLocalVirus, lpInfected->Notes.lpVirName, SIZE_VIRUSNAME);
    szLocalVirus [sizeof(szLocalVirus) - 1] = EOS;

    FastStringPrint(szBigBuffer,
                    lpszFormat,
                    szLocalVirus,
                    szFile,
                    &szFile[wCounter]);

                                        // Cut the big message into
                                        // useable pieces.
    lpszNextLine = szBigBuffer;

    for (wCounter = 0; wCounter < SCAN_PROMPT_MESSAGE_LINES; wCounter++)
        {
        lpszNextLine = CutString(lpszMessage[wCounter],
                                 lpszNextLine,
                                 SCAN_PROMPT_MESSAGE_WIDTH);
        }
} // End ScanFoundCreateMessage()


//************************************************************************
// ScanCreateActionMessage()
//
// This routine creates the three line messages used to report success
// or failure of repairs, inoculations, deletes, etc.
//
// Parameters:
//      LPSTR   lpszMessage[5]          Message buffer(s)
//      LPINFECTEDFILE  lpInfected      Infected File information
//
// Returns:
//      Nothing.
//************************************************************************
// 8/27/93 DALLEE, Function created.
//************************************************************************

VOID PASCAL ScanCreateActionMessage (LPSTR            *lpszMessage,
                                     LPINFECTEDFILE   lpInfected)
{
    extern  char    SZ_HARD_DRIVE_LC [];

    auto    char    *lpszFormat;
    auto    char    *lpszNextLine;
    auto    char    szBigBuffer [CREATE_MESSAGE_BUFFER + 1];
    auto    char    szLocalVirus[SIZE_VIRUSNAME + 1];
    auto    UINT    uCounter;
    auto    char    szFile      [SYM_MAX_PATH];

                                        // Pick up the correct message
                                        // format string from the StatusStrings
                                        // array.
    for (uCounter = 0, lpszFormat = SZ_EMPTY;
         StatusStrings[uCounter].wStatus;
         uCounter++)
        {
        if (StatusStrings[uCounter].wStatus == lpInfected->wStatus)
            {
            lpszFormat = StatusStrings[uCounter].lpszActionMessage;
            break;
            }
        }

    if (lpInfected->wStatus == FILESTATUS_MASTERBOOTINFECTED
    ||  lpInfected->wStatus == FILESTATUS_MASTERBOOTREPAIRED
    ||  lpInfected->wStatus == FILESTATUS_MASTERBOOTNOTREPAIRED)
        SPRINTF (szFile, SZ_HARD_DRIVE_LC, *lpInfected->szFullPath);
    else
        STRCPY(szFile, lpInfected->szFullPath);

    STRNCPY(szLocalVirus, lpInfected->Notes.lpVirName, SIZE_VIRUSNAME);
    szLocalVirus [sizeof(szLocalVirus) - 1] = EOS;

    FastStringPrint(szBigBuffer,
                    lpszFormat,
                    szFile,
                    szLocalVirus);

                                        // Cut the big message into
                                        // useable pieces.
    lpszNextLine = szBigBuffer;

    for (uCounter = 0; uCounter < SCAN_PROMPT_MESSAGE_LINES; uCounter++)
        {
        lpszNextLine = CutString(lpszMessage[uCounter],
                                 lpszNextLine,
                                 SCAN_PROMPT_MESSAGE_WIDTH);
        }
} // End ScanCreateActionMessage()


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
            lpcZipFileName++;
            if (*lpcZipFileName == ZIP_SEPARATOR)
            {
                lpcZipFileName++;
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
                    " %13s1  %21s2 %18s3",
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
    return ( NULL != FileStatus[wStatus].uFileStatusString ?
             FileStatus[wStatus].uFileStatusString :
             "" );
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
    return ( NULL != FileStatus[wStatus].uFollowHelp ?
             FileStatus[wStatus].uFollowHelp :
             "" );
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
// ScanNotify()
//
// Displays a
//
// Parameters:
//      LPNONAV     lpNoNav             Pointer to this scan's info
//
// Returns:
//      Nothing
//************************************************************************
// 6/1/93 DALLEE, Function created.
//************************************************************************

VOID PASCAL ScanNotify (LPNONAV lpNoNav)
{
    extern  BOOL            bScreenStarted;
    extern  DIALOG_RECORD   *glpScanFilesDialog;

    extern  DIALOG          dlVirusFound;
    extern  ButtonsRec      okButton;
    extern  char            *lpszVirusFoundStrings  [];

    auto    DIALOG_RECORD   *lpdrDialog;
    auto    time_t          timeStart;
    auto    WORD            wResult;

    auto    LPSTR           lpszAlertMessageNext;
    auto    char            lpAlertMessageBuffer [2][50 + 1];
    auto    char            lpMessageBuffer [5][50 + 1];
    auto    LPSTR           lpszMessage [5] =
                                {
                                lpMessageBuffer[0],
                                lpMessageBuffer[1],
                                lpMessageBuffer[2],
                                lpMessageBuffer[3],
                                lpMessageBuffer[4]
                                };

                                        // Make sure screen IO started.
    if (!bScreenStarted)
        {
        InitDisplay(TRUE);
        glpScanFilesDialog = OpenScanFilesDialog(lpNoNav);
        }

                                        // Set up the dialog.
    dlVirusFound.buttons = &okButton;
    dlVirusFound.strings = lpszVirusFoundStrings;

    if ( lpNoNav->Alert.bDispAlertMsg )
        {
        lpszAlertMessageNext = CutString(lpAlertMessageBuffer[0],
                                     lpNoNav->Alert.szAlertMsg,
                                     50);
        CutString(lpAlertMessageBuffer[1],
              lpszAlertMessageNext,
              50);
        }
    else
        {
        lpAlertMessageBuffer[0][0] = lpAlertMessageBuffer[1][0] = EOS;
        }

    ScanFoundCreateMessage(lpszMessage, &lpNoNav->Infected);

    lpdrDialog = DialogOpen2( &dlVirusFound,
                              lpszMessage[0],
                              lpszMessage[1],
                              lpszMessage[2],
                              lpszMessage[3],
                              lpszMessage[4],
                              lpAlertMessageBuffer[0],
                              lpAlertMessageBuffer[1]);

    timeStart = time(NULL);
    do
        {
        wResult = DialogProcessEvent(lpdrDialog, DialogGetEvent());

        if ( lpNoNav->Alert.bRemoveAfter &&
             (time(NULL) - timeStart > (time_t) lpNoNav->Alert.uSeconds) )
            {
            wResult = ACCEPT_DIALOG;
            }
        }
        while ((wResult != ACCEPT_DIALOG) && (wResult != ABORT_DIALOG));

    DialogClose(lpdrDialog, wResult);
} // End ScanNotify()


//************************************************************************
// ScanActionMessage()
//
// This routine displays the result of an action (repair, delete, exclude)
// taken on a problem file.
//
// Parameters:
//      LPINFECTEDFILE  lpInfected      File and status.
//
// Returns:
//      Nothing
//************************************************************************
// 8/27/93 DALLEE, Function created.
//************************************************************************

VOID PASCAL ScanActionMessage (LPINFECTEDFILE lpInfected)
{
    extern  BOOL            bScreenStarted;
    extern  DIALOG_RECORD   *glpScanFilesDialog;

    extern  DIALOG          dlScanActionMessage;
    extern  LPNONAV         glpNoNav;

    auto    DIALOG_RECORD   *lpdrDialog;
    auto    time_t          timeStart;
    auto    WORD            wResult;

//    auto    LPSTR           lpszAlertMessageNext;
//    auto    char            lpAlertMessageBuffer [2][50 + 1];
    auto    char            lpMessageBuffer [5][50 + 1];
    auto    LPSTR           lpszMessage [5] =
                                {
                                lpMessageBuffer[0],
                                lpMessageBuffer[1],
                                lpMessageBuffer[2],
                                lpMessageBuffer[3],
                                lpMessageBuffer[4]
                                };

                                        // Make sure screen IO started.
    if (!bScreenStarted)
        {
        InitDisplay(TRUE);
        glpScanFilesDialog = OpenScanFilesDialog(glpNoNav);
        }


    ScanCreateActionMessage(lpszMessage, lpInfected);

    lpdrDialog = DialogOpen2( &dlScanActionMessage,
                              lpszMessage[0],
                              lpszMessage[1],
                              lpszMessage[2],
                              lpszMessage[3],
                              lpszMessage[4]);

    timeStart = time(NULL);
    do
        {
        wResult = DialogProcessEvent(lpdrDialog, DialogGetEvent());

        if ( navOptions.alert.bRemoveAfter &&
             (time(NULL) - timeStart > (time_t) navOptions.alert.uSeconds) )
            {
            wResult = ACCEPT_DIALOG;
            }
        }
        while ((wResult != ACCEPT_DIALOG) && (wResult != ABORT_DIALOG));

    DialogClose(lpdrDialog, wResult);

} // End ScanActionMessage()


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
    extern  DIALOG      dlVirusFound;
    extern  char        *lpszVirusFoundInMemoryStrings  [];

    auto    DIALOG_RECORD   *lpdrDialog;

    auto    LPSTR           lpszAlertMessageNext;
    auto    char            lpAlertMessageBuffer [2][50 + 1];
    auto    char            lpMessageBuffer [5][50 + 1];
    auto    LPSTR           lpszMessage [5] =
                                {
                                lpMessageBuffer[0],
                                lpMessageBuffer[1],
                                lpMessageBuffer[2],
                                lpMessageBuffer[3],
                                lpMessageBuffer[4]
                                };

                                        // Set up the dialog.
    dlVirusFound.buttons = NULL;
    dlVirusFound.strings = lpszVirusFoundInMemoryStrings;

    if ( lpNoNav->Alert.bDispAlertMsg )
        {
        lpszAlertMessageNext = CutString(lpAlertMessageBuffer[0],
                                     lpNoNav->Alert.szAlertMsg,
                                     50);
        CutString(lpAlertMessageBuffer[1],
              lpszAlertMessageNext,
              50);
        }
    else
        {
        lpAlertMessageBuffer[0][0] = lpAlertMessageBuffer[1][0] = EOS;
        }

    ScanFoundCreateMessage(lpszMessage, &lpNoNav->Infected);

    InitDisplay(TRUE);                  // Make sure display is initialized.

    lpdrDialog = DialogOpen2( &dlVirusFound,
                              lpszMessage[0],
                              lpszMessage[1],
                              lpszMessage[2],
                              lpszMessage[3],
                              lpszMessage[4],
                              lpAlertMessageBuffer[0],
                              lpAlertMessageBuffer[1]);

   TimerDelay(18);                      // Wait a second for Dialog to display
                                        // (Korean DOS needs this delay)

#if defined(SYM_DEBUG) || defined(PRERELEASE)
    {
    auto    CFGTEXTREC  rNoHalt = {"NOHALT", CFG_SW_EXACT};

    if (ConfigSwitchSet(&rNoHalt, NULL))
        {
        DialogLoop(lpdrDialog, NULL);
        DialogClose(lpdrDialog, ACCEPT_DIALOG);

        return;
        }
    }
#endif

    // Shutdown stuff and close files

    MouseHC ( ) ;                       // Hide mouse
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
    extern  DIALOG_RECORD  *glpScanFilesDialog;
    extern  BOOL            bScreenStarted;

    auto    UINT            uSkipResult;

    if (!bScreenStarted)
        {
        InitDisplay(TRUE);
        glpScanFilesDialog = OpenScanFilesDialog((LPNONAV)dwNoNav);
        }

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
    extern      DIALOG          dlgMessageBootError ;
    extern      DIALOG          dlgMessageReadError ;
    extern      DIALOG          dlgMessageWriteError;
    extern      COLORS          alert_colors;

    auto        DIALOG          * lpdlgUse ;
    auto        DIALOG_RECORD   *dialog;
    auto	WORD		wResult;
    auto	DWORD		dwStartTime;
    auto        BOOL            bTimeoutFlag ;

    switch (uTypeErr)
        {
        case SKIPDRIVE_DLG_BOOT:
            lpdlgUse = &dlgMessageBootError;
            break;

        case SKIPDRIVE_DLG_WRITE:
            lpdlgUse = &dlgMessageWriteError;
            break;

        default:
        case SKIPDRIVE_DLG_ACCESS:
            lpdlgUse = &dlgMessageReadError;
            break;
        }

    if ( (dialog = DialogOpenC(lpdlgUse, &alert_colors, cDriveLetter)) == NULL )
        {
        return (uRvTimeout);
        }

    dwStartTime  = TimerTicks();
    bTimeoutFlag = FALSE ;

    do
	{
	wResult = DialogGetEvent();

	if (wResult != NULL)
	    wResult = DialogProcessEvent(dialog,wResult);

	if ( dwTicks != 0 && TimerElapsedTicks ( dwStartTime ) >= dwTicks )
            bTimeoutFlag = TRUE ;

	} while ( (!bTimeoutFlag) &&
                  (wResult != ACCEPT_DIALOG) && (wResult != ABORT_DIALOG));

    wResult = DialogClose(dialog,wResult);

    if ( bTimeoutFlag )
        return uRvTimeout ;

    if ( (uTypeErr == SKIPDRIVE_DLG_ACCESS)  && wResult == 1 )
        wResult = 2 ;                   // Kludge:  Patch return code for 2 button dlg

    switch ( wResult )
        {                               // Map return to SKIPDRIVE_ value
        case ESC :
        case 2 :                        // Skip Drive
            return SKIPDRIVE_ABORT ;
        case 1 :                        // Continue
            return SKIPDRIVE_IGNORE ;
        case 0 :                        // Retry
        default :
            return SKIPDRIVE_RETRY ;
        }
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
    extern  DIALOG_RECORD   *glpScanFilesDialog;
    extern  OutputFieldRec  ofScanOrInoculate;
    extern  char            SZ_UNCOMPRESSING_FILE_FORMAT [];

    auto    char            szUncompressing [80 + 1];
    auto    char            szZipFile       [MAX_PATH_SIZE + 1];
    auto    LPNONAV         lpNav;
    auto    int             nReturn;
    auto    UINT            uEvent;


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
                DialogOutput(&ofScanOrInoculate, szUncompressing);

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

                                        // If a virus is found and we should
                                        // halt, gbContinueScan is FALSE
                                        // So don't bother with updates, etc.
            if ( gbContinueScan )
                {
                UpdateScanDialog(lpNav);
                                        // Check for Enter, ESC, or button
                                        // hit to stop scan
                uEvent = DialogProcessEvent(glpScanFilesDialog, DialogGetEvent());
                if ( ((uEvent == ACCEPT_DIALOG) || (uEvent == ABORT_DIALOG))
                     && (lpNav->Opt.bAllowScanStop) )
                    {
                    gbContinueScan = FALSE;
                    }
                }

            nReturn = (gbContinueScan && lpNav->Gen.bScanZipFiles)
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

