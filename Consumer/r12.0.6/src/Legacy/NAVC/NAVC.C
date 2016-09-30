/// Copyright 1992-1996 Symantec, Peter Norton Product Group
//***********************************************************************
//
// $Header:   S:/NAVC/VCS/navc.c_v   1.25   01 Feb 1999 14:03:34   MBROWN  $
//
// Description:
//  This is the command line scanner main program
//
// See Also:
//***********************************************************************
// $Log:   S:/NAVC/VCS/navc.c_v  $
// 
//    Rev 1.25   01 Feb 1999 14:03:34   MBROWN
// Backed out of large sector fix for Phoenix
//
//    Rev 1.23   14 Aug 1998 15:25:02   mdunn
// Changes to display the trial period length in the expiration message
//
//    Rev 1.22   27 Jul 1998 10:19:54   tcashin
// In CheckForFixedDisk(), make sure the ABSDISK.bIsExtInt13 field is set to
// ISINT13X_UNKNOWN (defect #119072).
//
//    Rev 1.21   11 May 1998 18:15:52   mdunn
// Fixed references to ABSDISK fields.
//
//    Rev 1.20   18 Feb 1998 15:40:30   mdunn
// Fixed a coupla compile errors - a mismatched prototype, and comparing
// fopen()'s return with HFILE_ERROR instead of NULL.
//
//    Rev 1.19   22 Aug 1997 22:13:22   TCASHIN
// Need to see if IsActivityEnabled() before opening the log file. #91912.
//
//    Rev 1.18   20 Aug 1997 00:25:42   TCASHIN
// Made changes to the code to disable logging to a write-protected floppy.
// It wasn't right for the /LOG and /APPENDLOG options.
//
//    Rev 1.17   18 Aug 1997 13:55:16   TCASHIN
// The new critical error handler should return 3 to fail the current
// system call.
//
//    Rev 1.16   13 Aug 1997 21:37:46   TCASHIN
// Make sure we are using our own critical error handler.
//
//    Rev 1.15   07 Aug 1997 15:34:48   BGERHAR
// Fix trial version (wouldn't compile because I merged changes wrong). Now dumps out in one happy place.
//
//    Rev 1.14   04 Aug 1997 22:05:22   BGERHAR
// Disable activity log if logging to write protected media
//
//    Rev 1.13   29 Jul 1997 11:36:10   MKEATIN
// Added a missing ')'.
//
//    Rev 1.12   29 Jul 1997 11:07:22   MKEATIN
// Do not setup of check trial version info if we're run on a floppy.  This
// avoids write protected disk issues.
//
//    Rev 1.11   21 Jul 1997 02:26:00   BGERHAR
// Turn off Start/End scan logging for /STARTUP scans (use only for full user scans)
//
//    Rev 1.10   10 Jul 1997 14:01:36   MKEATIN
// Added trial version code.
//
//    Rev 1.9   11 Jun 1997 14:52:44   MKEATIN
// Added /HIMEM functionality
//
//    Rev 1.8   10 Jun 1997 19:37:54   MKEATIN
// Converted some errant 'exern char *'s to 'extern []'s.
//
//    Rev 1.7   09 Jun 1997 18:35:28   BGERHAR
// Add AreDefsWild() and misc. clean-up
//
//    Rev 1.6   08 Jun 1997 23:24:20   BGERHAR
// Don't wrap for deep scan or def directories
//
//    Rev 1.5   06 Jun 1997 20:58:40   MKEATIN
// Uncommented ParseCommandLine().
//
//    Rev 1.3   06 Jun 1997 18:30:26   BGERHAR
// Fix errorlevel and other misc. bugs
//
//    Rev 1.2   01 Jun 1997 20:33:56   BGERHAR
// Update definition/options search logic
//
//    Rev 1.1   29 Apr 1997 12:47:40   JTAYLOR
// Removed calls to NavExInit()
//
//    Rev 1.0   06 Feb 1997 20:56:24   RFULLER
// Initial revision
//
//    Rev 1.8   27 Jan 1997 17:10:48   MKEATIN
// ON the /HELPERROR switch, display HELP_ERROR_NEC[] if we're on a NEC.
//
//    Rev 1.7   17 Jan 1997 15:40:44   MKEATIN
// Added the /PROMPT switch for navc.
//
//    Rev 1.6   15 Jan 1997 13:56:28   MKEATIN
// Added bypass key dectection for startup scanning.
//
//    Rev 1.5   14 Jan 1997 13:13:54   MKEATIN
// Turn off optimizations for this file - it seems to break on NEC machines
//
//    Rev 1.4   13 Jan 1997 18:19:22   MKEATIN
// If we compile for the NEC and are run on a non-NEC, display a message and
// exit.  Do likewise if we run on a NEC when we don't compile for that
// platform.
//
//
//    Rev 1.3   09 Jan 1997 15:17:44   MKEATIN
// Added Ctrl-C handling for the NEC.  Also, Don't run in a NEC DOS box.
//
//    Rev 1.2   09 Jan 1997 14:03:48   MKEATIN
// Removed, at run-time, boot scanning for the NEC.
//
//    Rev 1.1   06 Jan 1997 21:13:34   MKEATIN
//
//
//    Rev 1.0   31 Dec 1996 15:20:14   MKEATIN
// Initial revision.
//
//    Rev 1.27   26 Dec 1996 17:02:26   JBELDEN
// added newline to copyright message.
//
//    Rev 1.26   13 Dec 1996 10:29:32   JBELDEN
// changed memory size required in initmem to be 200k.
//
//    Rev 1.25   12 Dec 1996 18:57:54   JBELDEN
// added memory check for startup
// 
//
//    Rev 1.24   12 Dec 1996 17:22:42   JBELDEN
// fixed so scanning of mbr set to true if /boot used.
//
//    Rev 1.23   10 Dec 1996 18:42:30   JBELDEN
// added ctrl-break handler code
//
//    Rev 1.22   10 Dec 1996 16:48:54   JBELDEN
// added interrupt handler for ctrl-c to ensure it breaks out
// properly.
//
//    Rev 1.21   06 Dec 1996 15:18:26   JBELDEN
// added code to let the user know a reduced virus def set is being used.
//
//    Rev 1.20   06 Dec 1996 07:23:16   JBELDEN
// fixed am/pm stamp when writeing to log.
//
//    Rev 1.19   06 Dec 1996 11:43:58   JBELDEN
// added code so that virwild and the inwild* could be used
//
//    Rev 1.18   02 Dec 1996 19:07:12   JBELDEN
// added /appendlog switch
//
//    Rev 1.17   27 Nov 1996 10:14:00   JBELDEN
// changed some doswritearray's to printf's
// fixes crashes.
//
//    Rev 1.16   25 Nov 1996 12:08:22   JBELDEN
// added code so that time is put on top of log.
//
//    Rev 1.15   25 Nov 1996 11:17:16   JBELDEN
// fixed else if's to be if's in command line switches so they work
// correctly
//
//    Rev 1.14   21 Nov 1996 18:38:42   JBELDEN
// had to put back in a call to a function previously removed for
// /L and /A to work correctly
//
//    Rev 1.13   19 Nov 1996 14:09:56   JBELDEN
// implemented /nobeep and part of /nobreak
//
//    Rev 1.12   12 Nov 1996 16:00:18   JBELDEN
// changed navdlgerror to printf with the correct message and
// format.
//
//    Rev 1.11   07 Nov 1996 14:20:38   JBELDEN
// removed function not used from scandrv.c
//
//    Rev 1.10   17 Oct 1996 11:27:52   JBELDEN
// moved the logic for /? switch to main().
//
//    Rev 1.9   16 Oct 1996 12:47:34   JBELDEN
// removed items related to reloading the tsr since not used
// in ttyscan
//
//    Rev 1.8   16 Oct 1996 11:07:28   JBELDEN
// removed items that where in browstr.str and aren't needed.
//
//    Rev 1.7   15 Oct 1996 11:55:12   JBELDEN
// added the /ZIPS and /? switches
//
//    Rev 1.6   14 Oct 1996 09:45:10   JBELDEN
// added code for log switch
//
//    Rev 1.5   11 Oct 1996 14:27:06   JBELDEN
// cleaned up after removing some variables from navstr.str
//
//    Rev 1.4   08 Oct 1996 14:50:58   JBELDEN
// put in code so that summary screen would appear at the end
// of the scan.
//
//    Rev 1.3   07 Oct 1996 18:15:44   JBELDEN
// replaced locatesettings dialog with printf of error message.
//
//    Rev 1.2   03 Oct 1996 14:49:48   JBELDEN
// Made numerous changes for TTYSCAN project.  Mostly ifdefs
// and deleting items.
//
//    Rev 1.1   02 Oct 1996 18:18:52   JBELDEN
// removed stuff in #ifdef NAVNET  and removed some code specific
// to navboot.  Not needed for this project.
//
//    Rev 1.0   02 Oct 1996 11:51:12   JBELDEN
// Initial revision.
//
//***********************************************************************

#include "platform.h"
#include <process.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#ifdef SYM_DOSX
#include <dos16.h>
#endif
#include "xapi.h"
#include "dbcs.h"
#include "stddos.h"
#include "nlibcfg.h"
#include "stddlg.h"
#include "symnet.h"
#include "symcfg.h"
#include "hardware.h"
#include "file.h"
#include "disk.h"                       // DiskIsFixed
#include "tsr.h"
#include "logio.h"

#include "defs.h"
#include "navutil.h"
#include "navdprot.h"
#include "options.h"
#include "syminteg.h"
#include "nonav.h"
#include "exclude.h"
#include "inoc.h"
#include "common.h"
#include "scand.h"
#include "virscan.h"
#ifdef NAV_PREVIEW
  #include <time.h>
  #include "triald.h"
#endif

#pragma optimize("",off)

//************************************************************************
// DEFINES
//************************************************************************

// Set trial version terms here

#ifdef NAV_PREVIEW
  #define TRIAL_NUM_DAYS        30
  #define TRIAL_EXPIRATION_DATE 0L      // set to MM/DD/YY format to enable
#endif

#define VMM_HANDLETABLES        100     // Maximum VMM handle tables
                                        // about 32 handles per table
#define VMM_PREALLOC            350     // Number of handles we'll typically use
                                        // (reserved at start to avoid
                                        // fragmentation)
#define MAX_WINDOWS             10      // Max windows that can be stacked

#define CPL_MAX_ENTRIES         7       // How many control options are there?
#define CPL_SCANNER             0       // Which one has the Scanner options?
#define CPL_AUTOPROTECT         1       // Which one is the Auto Protect?
#define CPL_EXCLUSIONS          3       // Which one is the Exclusions List?
#define CPL_PASSWORD            5       // Which one is the password dialog?
#define CPL_ALERTS              1       // Alerting options after shifting

/*
 This program contains the code for both NAV and NAVBOOT.  A /NAVBOOT
 switch on the command-line dictates which personality to display.

 Here are the options for each "personality":

        NAV                 NAVBOOT
        ------------        ---------------
        SCANNER             SCANNER
        AUTOPROTECT         ALERTS
        ALERTS              ACTIVITY LOG
        ACTIVITY LOG        EXCLUSIONS
        EXCLUSIONS          INOCULATION
        INOCULATION         GENERAL
        PASSWORDS
        GENERAL
*/

// Index into dialog strings where the "Directory" label is at.
#define DIR_REFERENCE           1
#define NAV_CHD_DIR_LABEL       3
#define SELECT_DIR_REFERENCE    3
#define FILE_SELECT_REFERENCE   6

MODULE_NAME;

#ifdef __cplusplus
extern "C" {
#endif
//-----------------------------------------------------------------------
// The Library requires this variable.
//-----------------------------------------------------------------------

        WORD            trapping_enabled = TRUE;

        BOOL bNavboot;                          // So, should we act like
                                                //    the NAVBOOT program?


#ifdef __cplusplus
} // extern "C"
#endif

//-----------------------------------------------------------------------
// Function prototypes
//-----------------------------------------------------------------------

extern VOID ctrlc_install(VOID);
extern VOID break_install(VOID);
extern VOID PASCAL BuildScanList (LPSTR lpScanList, LPCSTR szSelectedDrives);
VOID   SYM_EXPORT PASCAL InitError(BOOL bUseErrDlg);
BOOL   SYM_EXPORT PASCAL IsActivityLogEnabled(VOID);
UINT   SYM_EXPORT PASCAL CritErrHandler(WORD wErrorInfo, WORD wErrorCode, LPSTR lpDeviceName);

#ifndef SYM_DOSX
VOID PASCAL RtlTerminate(VOID);         // Clean up VML before terminate
#endif

#ifdef NAV_PREVIEW
BOOL  SetupTrial(VOID);
BOOL  IsTrialValid(VOID);
#endif

//-----------------------------------------------------------------------
// GLOBAL VARIABLES
//-----------------------------------------------------------------------

static BOOL bErrorStarted = FALSE;       // Critical error handler
static BOOL bVmmStarted;


BOOL bAutoClearLog;                     // Automatically clear the log
BOOL bAutoExportLog;                    // Automatically export the log
BOOL bStartupMode;                      // In startup mode concealing screen.
BOOL bNoHideMode;                       // NoHide for startup switch -CG
BOOL bFixedDiskPresent = TRUE;          // Is there a fixed disk present?
BOOL bNavcPrompt = FALSE;
BOOL            bUpdateTSR          = FALSE;
BOOL            bUpdateExclusions   = FALSE;
BOOL            bNavOptionsRead     = FALSE;
NAVOPTIONS      navOptions;             // Options Settings
LPNAVOPTIONS    lpNavOptions;           // Pointer to new Options Settings

FILE            *fLogFile;
char            szLogFilename[SYM_MAX_PATH];


// I used this ugly hack to quickly get Atomic's trial version supporting
// varying-length trial periods. This variable is set in DOSTRIAL.LIB
// code and then used in a printf().
DWORD           g_dwTrialLength;

//-----------------------------------------------------------------------
// LOCAL PROTOTYPES
//-----------------------------------------------------------------------
BOOL    LOCAL PASCAL DisableLogIfWriteProtect(LPCSTR lpLogFile);
BOOL    LOCAL PASCAL InitMemory(VOID);
VOID    LOCAL PASCAL RestoreError(VOID);
BOOL    LOCAL PASCAL ParseCommandLine(char *args);
VOID    LOCAL PASCAL AutomatedEventLoop(LPSCANSTRUCT lpScanStruct);
HGLOBAL LOCAL PASCAL GatherCommandLineNames( LPBOOL lpbScanSubs ) ;
BOOL    LOCAL PASCAL ShouldScanMBR ( BOOL bMBRFlag , HGLOBAL hScanList ) ;
VOID    LOCAL PASCAL CheckForFixedDisk(VOID);
BOOL    LOCAL PASCAL CheckIfBypassed(LPNAVOPTIONS lpNavOptions);
VOID    LOCAL PASCAL ShutDown(VOID);
VOID    LOCAL PASCAL ShowHelp(VOID);

//-----------------------------------------------------------------------
// OTHER PROTOTYPES
//-----------------------------------------------------------------------
BOOL          PASCAL InitNavHelp(VOID); // Not LOCAL: Used in SCAND/SCANITEM.C

/*-----------------------------------------------------------------------
 *               +                                    +
 *               +                                    +
 *               +                                    +
 *               --------------------------------------
 *
 * Description:
 *
 * Parameters:
 *
 * Return Value:
 *
 * See Also:
 *
 *-----------------------------------------------------------------------
 * 11/09/1992 PETE Function Created.
 *  5/09/1993 DALLEE, Read exclusions into the global exclude struct.
 *----------------------------------------------------------------------*/

VOID main(int argc, char *args)
{
    extern      char *   HELPERR_SCREEN     [];
    extern      char *   HELPERR_SCREEN_NEC [];
    extern      char     SZ_OPTSDAT_FAILED [];
    extern      char     SZ_STARTDAT_FAILED [];
    extern      char     SZ_BYPASSED [];
    extern      char     SZ_HI_IM_ME [];
    extern      char     SZ_DO_NOT_RUN_IN_DOS_BOX [];
    extern      char     SZ_RUN_ON_NEC_ONLY [];
    extern      char     SZ_DO_NOT_RUN_ON_NEC [];
    extern      WORD            wCPLOptions;
    extern      CFGTEXTREC      rHelpErrorLevel;
    extern      CFGTEXTREC      rHelpScreen;
    extern      CFGTEXTREC      rStartup;   // /STARTUP auto-mode
#ifdef NAV_PREVIEW
    extern      char            SZ_LICENSEEXPIRED[];
#endif
    auto        CALLBACKREV1    rCallbacks;

    ErrorLevelSet ( ERRORLEVEL_NONE ) ; // Assume no error

#ifdef NCD
    if (!_HWIsNEC())
        {
        ErrorLevelSet(ERRORLEVEL_LOADING);
        printf(SZ_RUN_ON_NEC_ONLY);
        DOSExit();                      // Do not run on non NEC machines if
        }                               // we link to the NCX libs

    if (_HWIsNEC() && (TSRMultiTaskEnvironment() & MTE_WINDOWS))
        {                               
        ErrorLevelSet(ERRORLEVEL_LOADING);
        printf(SZ_DO_NOT_RUN_IN_DOS_BOX);
        DOSExit();                      // Do not run in an NEC DOS box
        }
#else
    if (_HWIsNEC())
        {
        ErrorLevelSet(ERRORLEVEL_LOADING);
        printf(SZ_DO_NOT_RUN_ON_NEC);
        DOSExit();                      // only run on NEC if we are linking
        }                               // to the NCX libs
#endif


#if TTYSCAN
    if (argc == 1)
         ShowHelp();
#endif


    InitCertlib(&rCallbacks, INIT_CERTLIB_USE_DEFAULT); // init lpgCallBacks

                                        // Here is the critical question:
                                        // Are we a startup scanner or not?

                                        // Check some special switches early

                                        // /HELPERR

    if (ConfigSwitchSet(&rHelpErrorLevel, NULL))
        {
        if (!_HWIsNEC())
            DOSWriteStringArray(HELPERR_SCREEN);
        else
            DOSWriteStringArray(HELPERR_SCREEN_NEC);

        ErrorLevelSet ( ERRORLEVEL_NONE );
        goto mainRet;
        }

    if (ConfigSwitchSet(&rHelpScreen,  NULL)) //  /?
        {
        ShowHelp();
        }

#ifdef NAV_PREVIEW
    if (DiskIsFixed(DiskGet()))          // No trial version on floppies
        {                                // Avoids write protected disk issues.
        SetupTrial();

        if(!IsTrialValid())
            {
            char szMsg [1024];

            sprintf ( szMsg, SZ_LICENSEEXPIRED, g_dwTrialLength );
            printf ( szMsg );

            ErrorLevelSet ( ERRORLEVEL_OTHER ) ;
            bStartupMode = FALSE;           // Don't build TSR, don't exec it,
            goto mainRet;
            }
        }
#endif
                                        // /STARTUP
    bStartupMode = ConfigSwitchSet( &rStartup, NULL );

    if ( bStartupMode )
        printf("%s\n", SZ_HI_IM_ME);

    InitError(TRUE);

    if (InitMemory())
        {
        ErrorLevelSet ( ERRORLEVEL_OUTOFMEM ) ;
        goto mainRet;
        }

                                        // So self check as early
                                        // as possible (we may be infected)
    if (NOERR != NavSelfCheck(bStartupMode))
        {
        ErrorLevelSet ( ERRORLEVEL_SELFCHECK ) ;
        bStartupMode = FALSE;           // Don't build TSR, don't exec it,
        goto mainRet;
        }

    wCPLOptions = 8;                    // &?: Full options available to NAV.EXE

                                        // Find Definitions & Options directories
                                        // NOTE: Do defs first (in case of disk swap)

    if (!InitDefinitionsDir() || !InitOptionsDir())
        {
        // ErrorLevel already set...
        bStartupMode = FALSE;
        goto mainRet;
        }

                                        // Read current settings
    if ( ReadNavBin(&navOptions) != NOERR ||
        (bStartupMode && ReadNavStart(&navOptions) != NOERR) )
        {
        ErrorLevelSet(ERRORLEVEL_MISSINGFILE);

        if (bStartupMode)
            printf( SZ_STARTDAT_FAILED );
        else
            printf( SZ_OPTSDAT_FAILED );

        bStartupMode = FALSE;           // Don't build TSR, don't exec it,
        goto mainRet;
        }
                                        // Check if we should bypass startup
    if ( CheckIfBypassed(&navOptions) )
        {
        ErrorLevelSet(ERRORLEVEL_INTERRUPT);
        printf ( SZ_BYPASSED ) ;
        bStartupMode = FALSE;           // Don't build TSR, don't exec it,
        goto mainRet;                   // just leave.
        }


    if (!_HWIsNEC())
        {
        CheckForFixedDisk();            // Safety Precaution - checks MBR
        }

    ExcludeInit(&navOptions.exclude);   // Load EXCLUDE.DAT

                                        // Load NETUSER.DAT
    LoadNetUserList(&navOptions.netUser);

    bNavOptionsRead = TRUE;

#ifndef NAVSCAN
    InocInit(&navOptions);
#endif

    ALogioInit();                       // Initialize the activity log.
    if (ParseCommandLine(args))
        goto mainRet;

mainRet:
    ShutDown();
}

/*-----------------------------------------------------------------------
 *               +                                    +
 *               +          InitMemory()              +
 *               +                                    +
 *               --------------------------------------
 *
 * Description:
 *      Checks if enough memory is available.
 *
 * Return Value:
 *      TRUE    An error occurred.  You should exit the application
 *      FALSE   Initialization complete without a problem.
 *
 *-----------------------------------------------------------------------
 * 11/09/1992 PETE Function Created.
 * 09/13/1993 BarryG Check for out of memory and report what to free.
 *----------------------------------------------------------------------*/


#define FREE_MIN           (115000)


BOOL LOCAL PASCAL InitMemory(VOID)
{
    extern      VersionRec      PROGRAM_NAME;
    extern      char            NOT_ENOUGH_MEMORY[];
    extern      char            MORE_MEMORY_REQUIRED[];
    extern      char            SZ_VMMINIT_FAILED[];

    auto        BOOL            bErr = FALSE;
    auto        DWORD           iDOSAvailPara;
    auto        DWORD           dwMore;

    // First check that enough memory is left for NAV

    iDOSAvailPara   = d16GetDOSAvail( );

    if ( iDOSAvailPara < FREE_MIN )
        {

        dwMore  = (DWORD) (FREE_MIN-iDOSAvailPara) ;

        DOSWriteString ( NOT_ENOUGH_MEMORY ) ;
        DOSWriteString ( (CONST BYTE *) PROGRAM_NAME.name ) ;
        DOSWriteCRLF ( ) ;
        DOSPrint ( MORE_MEMORY_REQUIRED , & dwMore ) ;

        Blip();

        return TRUE ;                   // Not enough memory
        }

    return(bErr);
}



//***************************************************************************
// InitNavHelp() -- initialize the Help system
//
// Description:
//      Abstracted to a function so SCAND/SCANITEM.C can close and reopen
//      the help file to free up an extra handle during scans.
//
//***************************************************************************
// 09/01/1994 BARRY Function Created.
//***************************************************************************

BOOL PASCAL InitNavHelp(VOID)
{
    return FALSE;
    //return HelpInit("NAV",HELPFILE_NAME, HELPFILE_VERSION, TRUE);
}

//***************************************************************************
// CheckForFixedDisk() -- Don't allow boot record stuff if there isn't a
//                        fixed disk.
//
//***************************************************************************
// 03/16/1995 DSACKING Function Created.
//***************************************************************************

VOID LOCAL PASCAL CheckForFixedDisk(VOID)
{
    ABSDISKREC  AbsDiskInfo;                // To see if a HD is present
    auto LPBYTE lpbyBuffer;

    lpbyBuffer = (LPBYTE)MemAllocPtr (GHND, 8 * PHYSICAL_SECTOR_SIZE);
    AbsDiskInfo.buffer     = lpbyBuffer;    // buffer to hold read info
    AbsDiskInfo.dn         = 0x80;          // Look at the first fixed drive
    AbsDiskInfo.dwHead     = 0;
    AbsDiskInfo.dwTrack    = 0;
    AbsDiskInfo.dwSector   = 1;
    AbsDiskInfo.numSectors = 1;
    AbsDiskInfo.bIsExtInt13    = ISINT13X_UNKNOWN;

    if (DiskAbsOperation (READ_COMMAND, &AbsDiskInfo))
        {                                   // Did an error occur?
        bFixedDiskPresent = FALSE;
        }
}

//***************************************************************************
// CheckIfBypassed()
//
// Description:
//      Returns true if the configuration has bypass keys enabled, the
//      proper bypass keys are down, and we're in startup mode.
//
// Parameters:
//
// Return Value:
//
// See Also:
//
//***************************************************************************
// 06/20/1995 BARRY Function Created.
//***************************************************************************

BOOL LOCAL PASCAL CheckIfBypassed(LPNAVOPTIONS lpNavOptions)
{
    UINT uKBShift = KBShiftFlags();
    BOOL bBypass = FALSE;

    if ( bStartupMode )
        {
        switch ( lpNavOptions->tsr.uBypassKey )
            {
            case KEYS_SHIFT:
                if ( (uKBShift & LEFT_SHIFT) ||
                     (uKBShift & RIGHT_SHIFT) )
                    bBypass = TRUE;

                break;

            case KEYS_ALT:
                if ( uKBShift & ALT_SHIFT )
                    bBypass = TRUE;

                break;

            case KEYS_CTRL:
                if ( uKBShift & CTL_SHIFT )
                    bBypass = TRUE;

                break;

            case KEYS_NONE:
            default:
                break;
            }
        }
    return bBypass;
}

/*-----------------------------------------------------------------------
 *               +                                    +
 *               +                                    +
 *               +                                    +
 *               --------------------------------------
 *
 * Description:
 *
 * Parameters:
 *
 * Return Value:
 *      TRUE  if we've run in auto mode (/STARTUP or some other cmd line)
 *
 *
 * See Also:
 *
 *-----------------------------------------------------------------------
 * 11/09/1992 PETE Function Created.
 *----------------------------------------------------------------------*/

BOOL LOCAL PASCAL ParseCommandLine(char *args)
{
    extern  CFGTEXTREC rScanAllDrives;
    extern  CFGTEXTREC rScanLocalDrvs;
    extern  CFGTEXTREC rScanBootOnly;
    extern  CFGTEXTREC rScanBootOnOff;
    extern  CFGTEXTREC rMemoryScan;
    extern  CFGTEXTREC rMemoryOnlyScan;
    extern  CFGTEXTREC rScanHighMem;
#ifndef NAVPROMO
    extern  CFGTEXTREC rRefreshInoc;
#endif
    extern  CFGTEXTREC rFoundPrompts;
    extern  CFGTEXTREC rFoundRepairs;
    extern  CFGTEXTREC rFoundDeletes;
    extern  CFGTEXTREC rFoundReports;
    extern  CFGTEXTREC rFoundHaltsCPU;
    extern  CFGTEXTREC rExportLog;
#ifdef TTYSCAN
    extern  CFGTEXTREC rDoAllFiles;
    extern  CFGTEXTREC rZips;
    extern  CFGTEXTREC rNoBeep;
    extern  CFGTEXTREC rNoBreak;
    extern  CFGTEXTREC rAppendLog;
#endif
    auto    UINT        uSwitchState;
    auto    BOOL        bAutomate = TRUE;
    auto    SCANSTRUCT  rScan;
    auto    char        szBuffer[SYM_MAX_PATH + 1];

    ConfigParamToUpper();

    MEMSET(&rScan, '\0', sizeof(rScan));
    rScan.lpNavOptions = &navOptions;

                                        // &? Probably doesn't belong here...
                                        // Resolve the activity log filename.
                                        // If not a full path, use as much
                                        // of the opts path as necessary to
                                        // resolve.

    if (navOptions.activity.szLogFile[0] == '\\')
        {
                                        // Don't change UNC's
        if (navOptions.activity.szLogFile[1] != '\\')
            {
                                        // Full path. Need drive letter
            STRCPY(szBuffer, navOptions.activity.szLogFile);
            GetStartDir(0, navOptions.activity.szLogFile, 
                        sizeof(navOptions.activity.szLogFile) - 1);

                                        // Append past <drive:>
            STRCPY(navOptions.activity.szLogFile + 2, szBuffer);
            }
        }
    else if (!DBCSIsLeadByte(navOptions.activity.szLogFile[0]) &&
             navOptions.activity.szLogFile[1] != ':')
        {
                                        // Relative path. Need full path.
        STRCPY(szBuffer, navOptions.activity.szLogFile);
        GetStartDir(0, navOptions.activity.szLogFile,
                    sizeof(navOptions.activity.szLogFile) - 1);
        NameAppendFile(navOptions.activity.szLogFile, szBuffer);
        }

                                        // Disable activity log when:
                                        // 1) Something is being logged
                                        // 2) Log is to removable media
                                        // 3) The media is write protected

    if ((navOptions.activity.bKnown      != FALSE ||
         navOptions.activity.bInocChange != FALSE ||
         navOptions.activity.bVirusLike  != FALSE ||
         navOptions.activity.bStartEnd   != FALSE ||
         navOptions.activity.bQuarantine != FALSE))
        {

        auto char  szTempLogFileName;

                                        // First check the file in the /LOG 
                                        // switch.

        if (ConfigSwitchGetText(&rExportLog, &szTempLogFileName, SYM_MAX_PATH + 1))
            DisableLogIfWriteProtect(&szTempLogFileName);
    
                                        // Next check the file in the /APPENDLOG
                                        // switch.

        if (ConfigSwitchGetText(&rAppendLog, &szTempLogFileName, SYM_MAX_PATH + 1))
            DisableLogIfWriteProtect(&szTempLogFileName);

                                        // And last, check the activity log
                                        // in NAVOPTS.DAT.

        DisableLogIfWriteProtect(navOptions.activity.szLogFile);
        }

                                        // Grab what to scan from the
                                        // startup settings...
    if (bStartupMode)                                           // /STARTUP
        {
        navOptions.scanner.bPresFlop = FALSE;
        navOptions.scanner.bPresHard = FALSE;
        navOptions.scanner.bPresNet  = FALSE;

        navOptions.scanner.bMemory = navOptions.tsr.bScanMem;
        navOptions.scanner.bMasterBootRec = navOptions.tsr.bScanMBoot;
        navOptions.scanner.bBootRecs = navOptions.tsr.bScanBootRecs;

        if ( (TRUE == navOptions.scanner.bMemory) ||
             (TRUE == navOptions.scanner.bMasterBootRec) ||
             (TRUE == navOptions.scanner.bBootRecs) ||
             (TRUE == navOptions.startup.bScanWindowsFiles) )
            {
            bAutomate = TRUE;           // Only set if we should scan.
            }

                                        // Turn off end-of scan logging
                                        // (It's too messy)
        navOptions.activity.bStartEnd = FALSE;
        }

    // Parse the exclusive scan switches from MOST SCANNED to LEAST SCANNED
    // /A -> /L -> /BOOT -> /MEM

    if (ConfigSwitchSet(&rScanAllDrives,&uSwitchState))         // /A
        {
        // ***************************************** //
        // /A: Scan all drives except drives A and B //
        //     Supposed to override others (even /L) //
        // ***************************************** //

        navOptions.scanner.bPresFlop = FALSE;
        navOptions.scanner.bPresHard = TRUE;
        navOptions.scanner.bPresNet  = TRUE;
        rScan.bScanSubs = TRUE;
        bAutomate = TRUE;
        }
    else if (ConfigSwitchSet(&rScanLocalDrvs,&uSwitchState))    // /L
        {
        // **************************************** //
        // /L: Scan all local drives except A and B //
        //     Supposed to override others (not /A) //
        // **************************************** //

        navOptions.scanner.bPresFlop = FALSE;
        navOptions.scanner.bPresHard = TRUE;
        navOptions.scanner.bPresNet  = FALSE;
        rScan.bScanSubs = TRUE;
        bAutomate = TRUE;
        }
    else if (ConfigSwitchSet(&rScanBootOnly,&uSwitchState)      // /BOOT
             && !_HWIsNEC() )           // No Boot scaning on NEC
        {
        // **************************************** //
        // /BOOT:                                   //
        // Scan only boot sectors of named drives   //
        // **************************************** //

        navOptions.scanner.bMemory    = FALSE;
        navOptions.scanner.bMasterBootRec = TRUE ;
        navOptions.scanner.bBootRecs      = TRUE;
        rScan.bBootOnly = TRUE;
        bAutomate = TRUE;
        }
    else if (ConfigSwitchSet(&rMemoryOnlyScan,&uSwitchState))   // /MEM
        {
        // **************************************** //
        // /MEM:                                    //
        // Scan only memory                         //
        // **************************************** //

        navOptions.scanner.bMemory  =   TRUE;
        navOptions.scanner.bMasterBootRec =     FALSE;
        navOptions.scanner.bBootRecs    =   FALSE;
        navOptions.scanner.bPresFlop    =   FALSE;
        navOptions.scanner.bPresHard    =   FALSE;
        navOptions.scanner.bPresNet     =       FALSE;
        rScan.uScanMemory = TRUE;
        rScan.bMemOnly = TRUE;
        bAutomate = TRUE;
        }

    if (ConfigSwitchSet(&TTY_SW_SUBDIR,&uSwitchState))      // /S+-
        {
        bAutomate = TRUE;
        switch (uSwitchState)
            {
            case CFG_SWITCH_SET:
            case CFG_SWITCH_PLUS:
                rScan.bScanSubs = TRUE;
                break;
            case CFG_SWITCH_MINUS:
                rScan.bScanSubs = FALSE;
                break;
            }
        }

    if (ConfigSwitchSet(&rMemoryScan,&uSwitchState))        //  /M+-
        {
        bAutomate = TRUE;
        switch (uSwitchState)
            {
            case CFG_SWITCH_SET:
            case CFG_SWITCH_PLUS:
                navOptions.scanner.bMemory = TRUE;
                break;
            case CFG_SWITCH_MINUS:
                navOptions.scanner.bMemory = FALSE;
                break;
            }
        }

    if (ConfigSwitchSet(&rScanBootOnOff,&uSwitchState)      //  /B+-
        && !_HWIsNEC() )                // No Boot scaning on NEC
        {
        bAutomate = TRUE;
        switch (uSwitchState)
            {
            case CFG_SWITCH_SET:
            case CFG_SWITCH_PLUS:
                navOptions.scanner.bMasterBootRec = TRUE;
                navOptions.scanner.bBootRecs    =   TRUE;
                    rScan.uScanMemory = TRUE;
                break;
            case CFG_SWITCH_MINUS:
                navOptions.scanner.bMasterBootRec = FALSE;
                navOptions.scanner.bBootRecs    =   FALSE;
                rScan.uScanMemory = FALSE;
                break;
            }
        }


    if (ConfigSwitchSet(&rScanHighMem,&uSwitchState))   // /HIMEM
        {                                               
        navOptions.scanner.bMemory         = TRUE;      // Only scan high
        navOptions.general.bScanHighMemory = TRUE;      // memory if this
        bAutomate = TRUE;                               // switch is set
        }
    else
        {
        navOptions.general.bScanHighMemory = FALSE;
        }



/*----SKURTZ-----05-25-93 06:39:10pm-----------------
 * What to do if found -- not applicable for NAVSCAN
 *---------------------------------------------------*/

#ifndef NAVSCAN
    // Parse the exclusive action switches from safest to scariest
    //&? (REPORTS is a bit of an anomoly -- it shouldn't replace the others)
    // /PROMPT -> REPAIR -> DELETE -> (&?) REPORTS -> HALT

    if (ConfigSwitchSet(&rFoundRepairs,&uSwitchState))   //  /REPAIR
        {
        navOptions.scanner.uAction = SCAN_REPAUTO;
        bAutomate = TRUE;
        }
    else if (ConfigSwitchSet(&rFoundDeletes,&uSwitchState))   //  /DELETE
        {
        navOptions.scanner.uAction = SCAN_DELAUTO;
        bAutomate = TRUE;
        }
    else if (ConfigSwitchSet(&rFoundHaltsCPU,&uSwitchState))   //  /HALT
        {
        navOptions.scanner.uAction = SCAN_HALTCPU;
        bAutomate = TRUE;
        }
    else if (ConfigSwitchSet(&rFoundPrompts,&uSwitchState))   //  /PROMPT
        {
        bNavcPrompt = TRUE;
        bAutomate = TRUE;
        }

#ifdef TTYSCAN
    if (ConfigSwitchSet(&rDoAllFiles, &uSwitchState))    //  /DOALLFILES
         {
           navOptions.scanner.bScanAll = TRUE;
           bAutomate = TRUE;
         }

    if (ConfigSwitchSet(&rZips, &uSwitchState))    //  /ZIPS
         {
           navOptions.general.bScanZipFiles  = TRUE;
           bAutomate = TRUE;
         }

    if (ConfigSwitchSet(&rNoBeep, &uSwitchState))    //  /NOBEEP
         {
           navOptions.alert.bBeep  = FALSE;
           bAutomate = TRUE;
         }


    if (ConfigSwitchSet(&rNoBreak, &uSwitchState))    //  /NOBREAK
         {
           bAutomate = TRUE;
         }


#endif
    if (ConfigSwitchSet(&rExportLog,NULL))
        {
        bAutomate = TRUE;               // We process switch below
        }

#endif  // !NAVSCAN

    if (bAutomate ||
        ConfigParamCount() > 0)
        {
        bAutomate = TRUE;
        rScan.hScanList = GatherCommandLineNames(&rScan.bScanSubs);

        // Kludge:  Decide if we want to scan the MBR afterall...
        if ( rScan.bBootOnly )
            {
            navOptions.scanner.bMasterBootRec =
                        ShouldScanMBR ( navOptions.scanner.bMasterBootRec,
                                        rScan.hScanList ) ;
            }

        ctrlc_install();                // install ctrl-c handler

        if (!_HWIsNEC())
            {
            break_install();            // install ctrl-break handler
            }
        else
            {                           // No boot record scanning on NEC
            navOptions.scanner.bMasterBootRec = FALSE;
            navOptions.scanner.bBootRecs      = FALSE;
            }

        AutomatedEventLoop(&rScan);
        }

    return(bAutomate || bStartupMode);

} // End ParseCommandLine()

//***************************************************************************
// ShouldScanMBR()
//
// Description:
//      Returns TRUE if the MBR should be scanned.
//      Only useful if /BOOT specified on the command line and after
//      the drive list is known.
//
// Parameters:
//      bMBRFlag        -- Current state of the MBR flag.  Will never
//                         return FALSE if already TRUE.
//      hScanList       -- Handle to szz list of drives to be scanned.
//
// Return Value:
//      TRUE  if bMBRFlag is TRUE or one of the drives in the list contains
//            a MBR.
//      FALSE otherwise.
//
//***************************************************************************
// 09/07/1993 BARRY Function Created.
//***************************************************************************

BOOL LOCAL PASCAL ShouldScanMBR ( BOOL bMBRFlag , HGLOBAL hScanList )
{
    auto LPSTR  lpszzScanList ;
    auto BOOL   bRv ;

    bRv = FALSE ;

    if ( bMBRFlag || hScanList == NULL )
        return bMBRFlag ;

    if ( (lpszzScanList = (char *) MemLock ( hScanList )) != NULL )
        {
        while ( * lpszzScanList )
            {
            if ( DiskIsFixed ( * lpszzScanList ) )
                {
                bRv = TRUE ;
                break ;                 // Only need one to be happy
                }
            lpszzScanList = SzzFindNext ( lpszzScanList ) ;
            }
        MemUnlock ( hScanList , lpszzScanList ) ;
        }

    return bRv ;
}

/**************************************************************************
 * GatherCommandLineNames
 *
 * DESCRIPTION  : Allocate and return an szz list of the filespecs on the
 *                command line.  All filenames will be fully qualified but
 *                may contain wildcards in the file portion.
 *        NOTE:  lpbScanSubs should only be touched if needed.
 *
 * IN      :      lpbScanSubs -- Pointer bScanSubs flag (in case we need to change)
 * OUT     :      handle to szz list.  This value should be stored in
 *                lpScanStruct->hScanList and must be freed by the caller.
 *************************************************************************
 * SKURTZ : 05-26-93 at 03:37:19pm Function created
 * BARRYG : 02-AUG-93 Pass preallocated handle and pointer to scansubs flag
 * BARRYG : 09-AUG-93 Now pass just bScanSubs.  Fully qualify filenames now.
 ************************************************************************* */
HGLOBAL LOCAL PASCAL GatherCommandLineNames( LPBOOL lpbScanSubs )
{
#define SIZE_COMMAND_LINE_LIST  (2 * SYM_MAX_PATH + 2)

    extern  char        szSelectedDrives[];
    auto    HGLOBAL     hScanList = NULL;
    auto    LPSTR       lpszzScanList;
    auto    LPSTR       lpszScanOne;
    auto    UINT        uCounter;
    auto    DWORD       dwStartupEntries = 0;
    auto    LONG        lNewLen;
    auto    LONG        lListSize;

    auto    char        szParamCurrent [ SYM_MAX_PATH ];
    auto    char        szFullCurrent [ SYM_MAX_PATH ];


    if ( bStartupMode && navOptions.startup.bScanWindowsFiles )
        {
                                        // Grab what to scan from NAVSTART.DAT
        if ( NOERR == ReadStartupFileList( FILELIST_MODIFIED,
                                           &hScanList,
                                           &dwStartupEntries,
                                           (LPDWORD)&lListSize) )
            {
            auto    HGLOBAL     hReAlloc;

                                        // Try to reallocate to read cmd line.
            if ( NULL == (hReAlloc = MemReAlloc(hScanList,
                                                lListSize + SIZE_COMMAND_LINE_LIST,
                                                GHND)) )
                {
                // Sorry... no more memory for command line targets.
                return (hScanList);
                }

            hScanList = hReAlloc;
            lpszzScanList = (LPSTR)MemLock( hScanList );
            lpszScanOne = lpszzScanList + (lListSize - 1);
                                        // Can use full size since we already
                                        // have an extra EOS for SZZ end.
            lListSize = SIZE_COMMAND_LINE_LIST;
            }
        //&? else { Error checking? }

        }

    // If not /STARTUP, or reading NAVSTART.DAT failed,
    // try to allocate at least the minimum buffer:
    if ( NULL == hScanList )
        {
        if ( NULL == (hScanList = MemAlloc(GHND , SIZE_COMMAND_LINE_LIST)) )
            return NULL ;

        // Past this point, we guarantee success
        // (but may need to drop a name or two)

                                        // Guaranteed to work after MemAlloc()
        lpszzScanList = (char *) MemLock(hScanList);
                                        // Leave room for szz terminator
        lListSize     = MemSize ( hScanList ) -1L;

        lpszScanOne = lpszzScanList;
        }

    lpszScanOne[0] = EOS;     // Guarantee szz is valid
    lpszScanOne[1] = EOS;

    for ( uCounter = 0; lListSize > 0L; uCounter += 1 )
        {
        if (NOERR != ConfigParamGet( uCounter,
                                     szParamCurrent,
                                     sizeof(szParamCurrent) - 1 ) )
            {
            break;
            }

        if ( szParamCurrent [ 1 ] == ':' && szParamCurrent [ 2 ] == '\0' )
            {                           // Special case "x:" -> "x:\"
            szFullCurrent [ 0 ] = szParamCurrent [ 0 ];
            STRCPY ( szFullCurrent + 1 , ":\\" );
            * lpbScanSubs = TRUE;
            }
        else if ( NameToFull(szFullCurrent , szParamCurrent) == ERR)
            {
            continue;                   // Bad filespec -- ignore
            }

        // &? BARRYG -- are we being too smart here?  Leave this to scanner?
        if ( ! NameIsWildcard ( szFullCurrent ) &&
             (NameGetType(szFullCurrent) & ~NGT_IS_FULL) == 0 )
            {
            continue;                   // Skip non-existant files
            }

                                        // Count EOS
        lNewLen = (LONG) STRLEN ( szFullCurrent ) + 1L;

        if ( lNewLen > lListSize )
            {                           // &? Should resize block
            continue;                   // For now, silently loose name
            }

        STRCPY ( lpszScanOne , szFullCurrent );
        lpszScanOne += lNewLen;
        lListSize   -= lNewLen;
        } // for()

    * lpszScanOne = EOS;                // Put second EOS at end

                                        // No parameters...scan default drives
    if ( (FALSE == bStartupMode) && (0 == uCounter) )
        {
        InitializeSelectedDrives();
        ScanDriveGetList();             // Update szSelectedDrives[];
        BuildScanList(lpszzScanList, szSelectedDrives);
        *lpbScanSubs = TRUE;
        }

    SzzStripDuplicates ( lpszzScanList );
    MemUnlock ( hScanList , lpszzScanList );

    return (hScanList);

} // End GatherCommandLineNames()


/**************************************************************************/

/**************************************************************************/
BOOL LOCAL PASCAL ActivityLogSwitchesOnly(VOID)
{
    extern  CFGTEXTREC      rExportLog;
    UINT        uSwitchCount;
    BOOL        bExportLogSet;

                                        // If any parameters, then process them
    if ( ConfigParamCount() )
        return(FALSE);
                                        // See if these are the only switches
                                        // on the command line.
    uSwitchCount  = ConfigSwitchCount();
    bExportLogSet = ConfigSwitchSet(&rExportLog,NULL);
    return ((BOOL)((uSwitchCount == 1) && (bExportLogSet)));

}


/**************************************************************************
 * IsActivityLogEnabled()
 *
 * DESCRIPTION:   Check if activity logging is enabled.
 *
 * IN      :      none
 * OUT     :      TRUE if one or more of the navOptions.activity.b*
 *                     fields are TRUE
 *                FALSE if all logging is turned off.
 *
 *************************************************************************
 * TCASHIN : 08-19-97  Function created
 ************************************************************************* */
BOOL SYM_EXPORT PASCAL IsActivityLogEnabled(VOID)
{
    if ((navOptions.activity.bKnown      == FALSE) &&
        (navOptions.activity.bUnknown    == FALSE) &&
        (navOptions.activity.bInocChange == FALSE) &&
        (navOptions.activity.bVirusLike  == FALSE) &&
        (navOptions.activity.bStartEnd   == FALSE) &&
        (navOptions.activity.bVirusList  == FALSE) &&
        (navOptions.activity.bQuarantine == FALSE) )

        return(FALSE);
    else
        return(TRUE);
}


/**************************************************************************
 * DisableLogIfWriteProtect()
 *
 * DESCRIPTION:   Check to see if the activity log file name
 *                is on write-protected removable media and disable logging
 *                if needed.
 *
 * IN      :      lpLogFile
 * OUT     :      TRUE if logging was disabled.
 *                FALSE if we we can write to this drive
 *
 *************************************************************************
 * TCASHIN : 08-19-97  Function created
 ************************************************************************* */
BOOL LOCAL PASCAL DisableLogIfWriteProtect(LPCSTR lpLogFile)
{
    auto HFILE  hFile;
    auto char   szBuffer[SYM_MAX_PATH+1];
    auto BOOL   bRetVal = FALSE;

    NameToFull(szBuffer, lpLogFile);

    if (DiskIsRemovable(szBuffer[0]))
        {
        szBuffer[3] = EOS;

        hFile = FileCreateTemporaryPath(szBuffer,szBuffer, FA_NORMAL);

        if (hFile != HFILE_ERROR)
            {
            if (FileClose(hFile) == HFILE_ERROR)
                hFile == HFILE_ERROR;

            FileDelete(szBuffer);
            }

        if (hFile == HFILE_ERROR)       // Write protected. Disable logging.      
            {
            navOptions.activity.bKnown      = FALSE;
            navOptions.activity.bUnknown    = FALSE;
            navOptions.activity.bInocChange = FALSE;
            navOptions.activity.bVirusLike  = FALSE;
            navOptions.activity.bStartEnd   = FALSE;
            navOptions.activity.bVirusList  = FALSE;
            navOptions.activity.bQuarantine = FALSE;

            bRetVal = TRUE;
            }
        }

    return(bRetVal);
}



/**************************************************************************
 * AutomatedEventLoop
 *
 * DESCRIPTION  : A command line switch is in use.  Execute the scan and leave
 *
 * IN      :      lp to SCANSTRUCT.
 *
 * RETURN VALUES:
 **************************************************************************
 * SKURTZ : 05-26-93 at 12:37:00pm Function created
 * BARRYG : 02-AUG-93 Do MemAlloc calls here
 **************************************************************************/

VOID LOCAL  PASCAL  AutomatedEventLoop(LPSCANSTRUCT lpScanStruct)
{
    extern      CFGTEXTREC      rExportLog;
    extern      CFGTEXTREC      rAppendLog;

    extern      char            szLogFileERR[];
    extern      char            szUsingVirWild[];

    char szFilename[SYM_MAX_PATH];
    struct tm *newtime;
    char  am_pm[] = "PM";
    time_t long_time;

    if (ConfigSwitchSet(&rExportLog,NULL))
        {
        bAutoExportLog = TRUE;
        ConfigSwitchGetText(&rExportLog,szFilename, sizeof(szFilename));
        NameToFull(szLogFilename, szFilename);        

        if (IsActivityLogEnabled() == FALSE ||
           ((fLogFile = fopen(szLogFilename, "wt")) == NULL ) )
           {
           DOSPrint(szLogFileERR);
           bAutoExportLog = FALSE;
           }
        else
           {
           time( &long_time );                 /* Get time as long integer. */
           newtime = localtime( &long_time );  /* Convert to local time. */

           if( newtime->tm_hour < 12 )         /* Set up extension. */
              strcpy( am_pm, "AM" );
           if( newtime->tm_hour > 12 )         /* Convert from 24-hour */
              newtime->tm_hour -=12;           /*   to 12-hour clock.  */

           fprintf(fLogFile, "%.19s %s\n\n", asctime( newtime ), am_pm );
           }
        }

     if (ConfigSwitchSet(&rAppendLog,NULL))
        {
        bAutoExportLog = TRUE;
        ConfigSwitchGetText(&rAppendLog,szFilename, sizeof(szFilename));
        NameToFull(szLogFilename, szFilename);

        if (IsActivityLogEnabled() == FALSE ||
           ((fLogFile = fopen(szLogFilename, "at")) == NULL ) )
           {
           DOSPrint(szLogFileERR);
           bAutoExportLog = FALSE;
           }
        else
           {
           time( &long_time );                 /* Get time as long integer. */
           newtime = localtime( &long_time );  /* Convert to local time. */
           if( newtime->tm_hour < 12 )         /* Set up extension. */
              strcpy( am_pm, "AM" );
           if( newtime->tm_hour > 12 )         /* Convert from 24-hour */
              newtime->tm_hour -=12;           /*   to 12-hour clock.  */
           fprintf(fLogFile, "%.19s %s\n\n", asctime( newtime ), am_pm );
           }
        }

    if (AreDefsWild())
        {
        if ( bAutoExportLog )
            fprintf(fLogFile, szUsingVirWild);
        }

                                        // Only scan, if not one of the special
                                        // activity log switches.
    if ( !ActivityLogSwitchesOnly() )
        {
        if ( lpScanStruct->hScanList )
            {
#ifdef TTYSCAN
            lpScanStruct->wCallWhenDone = PROGRAM_INTERACTIVE;
#else
            lpScanStruct->wCallWhenDone = PROGRAM_COMMANDLINE;
#endif
            ScanStart(lpScanStruct);
            }
        }

    if ( lpScanStruct->hScanList )
        {
        MemFree ( lpScanStruct->hScanList );
        lpScanStruct->hScanList = NULL;
        }
    
    if (IsActivityLogEnabled() && bAutoExportLog == TRUE)
        fclose(fLogFile);

} // End AutomatedEventLoop()


//#endif

/*-----------------------------------------------------------------------
 *               +                                    +
 *               +                                    +
 *               +                                    +
 *               --------------------------------------
 *
 * Description:
 *
 * Parameters:
 *
 * Return Value:
 *
 * See Also:
 *
 *-----------------------------------------------------------------------
 * 11/09/1992 PETE Function Created.
 *----------------------------------------------------------------------*/

VOID SYM_EXPORT PASCAL InitError(BOOL bUseErrDlg)
{

                                        // Turn it off first
    if ( bErrorStarted )
        RestoreError();

    bErrorStarted = TRUE;

    /* Install default critical error handler */
    /* Use ErrorTrapSmall because it is easier to overlay */
    DiskErrorOn((bUseErrDlg) ? (ERRORTRAPPROC) CritErrHandler : (ERRORTRAPPROC) -1);
}



/*-----------------------------------------------------------------------
 *               +                                    +
 *               +                                    +
 *               +                                    +
 *               --------------------------------------
 *
 * Description:
 *
 * Parameters:
 *
 * Return Value:
 *
 * See Also:
 *
 *-----------------------------------------------------------------------
 * 8/12/1997 TCASHIN Function Created.
 *----------------------------------------------------------------------*/

UINT SYM_EXPORT PASCAL CritErrHandler(WORD wErrorInfo, WORD wErrorCode, LPSTR lpDeviceName)
{
    return (3);
}


/*-----------------------------------------------------------------------
 *               +                                    +
 *               +                                    +
 *               +                                    +
 *               --------------------------------------
 *
 * Description:
 *
 * Parameters:
 *
 * Return Value:
 *
 * See Also:
 *
 *-----------------------------------------------------------------------
 * 11/09/1992 PETE Function Created.
 *----------------------------------------------------------------------*/

VOID LOCAL PASCAL RestoreError(VOID)
{
    if ( bErrorStarted )
        {
        bErrorStarted = FALSE;
                                        /* Release Ctrl-Break handler */
      //  DOSBreakOff();
                                        /* Release critical error handler */
        DiskErrorOff();
        }
}

//***************************************************************************
// ErrorLevelSet()
// ErrorLevelGet()
//
// Description:
//      Set/Get DOS ErrorLevel for program exit (preserves VIRUSFOUND status
//      above all others)
//
//***************************************************************************
// 10/25/1993 BARRY Function Created.
//***************************************************************************

VOID PASCAL ErrorLevelSet ( BYTE byErrorLevel )
{
    extern BYTE errorCode;

    switch ( errorCode )
        {
        case ERRORLEVEL_VIRUSFOUND:     // Never change once on VIRUSFOUND
            break;

        case ERRORLEVEL_SELFCHECK:      // Only change if going to VIRUSFOUND
            if ( byErrorLevel != ERRORLEVEL_VIRUSFOUND )
                break;

            // fall through

        default:
            errorCode = byErrorLevel;
            break;
        }
}

BYTE PASCAL ErrorLevelGet ( VOID )
{
    extern BYTE errorCode;

    return errorCode;
}

/*-----------------------------------------------------------------------
 *               +                                    +
 *               +                                    +
 *               +                                    +
 *               --------------------------------------
 *
 * Description:
 *
 * Parameters:
 *
 * Return Value:
 *
 * See Also:
 *      ErrorLevelSet -- Set DOS error level to return by DOSExit()
 *
 *-----------------------------------------------------------------------
 * 11/09/1992 PETE Function Created.
 *  5/09/1993 DALLEE, Added call to ExcludTerminate().
 *  6/15/1993 DALLEE, Update TSR.
 *----------------------------------------------------------------------*/

VOID LOCAL PASCAL ShutDown(VOID)
{
    extern  char    lpszUpdateExcludeError[];
    auto    BOOL    bLoadTSR = FALSE;

#ifndef NAVSCAN
    InocEnd();
#endif

    if (bNavOptionsRead)
        {
#ifndef NAVSCAN
        if (bUpdateExclusions)              // Save the changed exclusions...
            {
            bUpdateTSR = TRUE;
            if (ExcludeTerminate(&navOptions.exclude, TRUE) == ERR)
                {
                printf(lpszUpdateExcludeError);
                }
            }
        else                                // ...or just free the memory.
#endif  // *** WARNING! The next block is tied to the else before
        //     this #endif.  Do not move! ***
            {
            ExcludeKillCopy(&navOptions.exclude);
            }
        }


                                       // Free Net User memory
    if (navOptions.netUser.hNetUserItem)
        {
        MemRelease(navOptions.netUser.hNetUserItem);
        MemFree(navOptions.netUser.hNetUserItem);
        }

    RestoreError();

    if (bVmmStarted)
        {
        VMMTerminate();
        HelpDone();
        }

    DOSExit();
}


#ifdef TTYSCAN
/**************************************************************************
 * ShowHelp
 *
 * DESCRIPTION  : Shows the help screen
 *
 * IN      :      None
 * OUT     :      None
 * RETURN VALUES: None
 **************************************************************************
 * JBELDEN : 10-3-96 Function created
 **************************************************************************/

VOID LOCAL PASCAL ShowHelp(VOID)
{
    extern  char *      HELP_SCREEN[];
    extern  char *      HELP_SCREEN_NEC[];
    extern  char        SZ_PROGRAM_NAME[];
    extern  char        SZ_VERSION_STR[];
    extern  char        SZ_VERSION_LUIGI[];
    extern  char        PRODUCT_COPYRIGHT[];
    char                szBanner[160];

    STRCPY(szBanner, SZ_PROGRAM_NAME);
    STRCAT(szBanner, " ");
    STRCAT(szBanner, SZ_VERSION_STR);
    STRCAT(szBanner, ",\n");
    STRCAT(szBanner, PRODUCT_COPYRIGHT);
    STRCAT(szBanner, "\n");
    STRCAT(szBanner, SZ_VERSION_LUIGI);
    STRCAT(szBanner, "\n");
    DOSWriteString((BYTE *)szBanner);

    if (_HWIsNEC())
        DOSWriteStringArray(HELP_SCREEN_NEC);
    else
        DOSWriteStringArray(HELP_SCREEN);

    DOSExit();
}
#endif

#ifdef NAV_PREVIEW

BOOL SetupTrial()
{
	return (Initialize("Symantec", "Norton AntiVirus", 2, 0) &&
           Setup(TRIAL_NUM_DAYS, TRIAL_EXPIRATION_DATE));
}

BOOL IsTrialValid()
{
   return (Initialize("Symantec", "Norton AntiVirus", 2, 0) &&
           IsValid(TRUE, NULL));
}
#endif  // NAV_PREVIEW

#pragma optimize("",on)

