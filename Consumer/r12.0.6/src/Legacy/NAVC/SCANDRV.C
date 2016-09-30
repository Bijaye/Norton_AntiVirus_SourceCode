// Copyright 1993-1996 Symantec Corporation
//========================================================================
//
// $Header:   S:/NAVC/VCS/scandrv.c_v   1.2   13 Aug 1997 21:26:56   TCASHIN  $
//
// Description:
//  Select drives to scan for viruses.
//
// See Also:
//
//------------------------------------------------------------------------
// $Log:   S:/NAVC/VCS/scandrv.c_v  $
// 
//    Rev 1.2   13 Aug 1997 21:26:56   TCASHIN
// Make InitError an extern to make sure we get the right one.
// 
//    Rev 1.1   08 Jun 1997 23:24:20   BGERHAR
// Don't wrap for deep scan or def directories
// 
//    Rev 1.0   06 Feb 1997 20:56:18   RFULLER
// Initial revision
// 
//    Rev 1.0   31 Dec 1996 15:19:52   MKEATIN
// Initial revision.
// 
//    Rev 1.5   21 Nov 1996 18:38:20   JBELDEN
// had to put some code back in for /A and /L switches to work
// 
//    Rev 1.4   08 Nov 1996 16:52:36   JBELDEN
// 
//    Rev 1.3   07 Nov 1996 14:18:48   JBELDEN
// removed code related to dialog boxes and cleaned it up a bit
// 
//    Rev 1.2   09 Oct 1996 14:29:10   JBELDEN
// stubed out a lot of unneed functions
// 
//    Rev 1.1   03 Oct 1996 14:56:16   JBELDEN
// removed DOSHelp that is in events.c and navboot since it
// isn't used.
// 
//    Rev 1.0   02 Oct 1996 12:30:30   JBELDEN
// Initial revision.
//
//------------------------------------------------------------------------

#include    "platform.h"
#include    "xapi.h"
#include    "stddos.h"
#include    "stddlg.h"
# include    "disk.h"
#include    "file.h"

#include    "navutil.h"
#include    "navdprot.h"

#include    "options.h"


#ifdef __cplusplus
extern "C" {
#endif
    extern      int                firstRow;
    extern      int                firstCol;
    extern      char            *FAR DRIVE_TYPE_NAMES[];
#ifdef __cplusplus
}
#endif

#define SCAN_DRIVES_NOW     0
#define SCAN_DRIVES_EXIT    1
#define SCAN_DRIVES_HELP    2

typedef struct
    {
    char        dl;
    BYTE	type;
    BYTE        byFlopType;
    char 	driveIcon[4];
    BYTE	selected;
    char        szLabel[12];
    } ScanSelectDriveRec;

char szSelectedDrives[SYM_MAX_DOS_DRIVES+1];

UINT PASCAL NAVDriveListToString (
    ScanSelectDriveRec      *driveList,
    UINT                wDrives,
    char                *pszDrives);

UINT PASCAL NAVCreateDriveArray(
    ScanSelectDriveRec      *driveList,             // the array to build
    char                *pszDrvLet,             // the current drive letters
    char                *pszSelLet);            // current selections

extern VOID SYM_EXPORT PASCAL InitError(BOOL bUseErrDlg);

/************************************************************************
 *  LOCAL PROTOTYPES      LOCAL PROTOTYPES      LOCAL PROTOTYPES        *
 *  LOCAL PROTOTYPES      LOCAL PROTOTYPES      LOCAL PROTOTYPES        *
 ************************************************************************/
VOID LOCAL PASCAL DoTag (ListRec *list, UINT mode);
VOID LOCAL PASCAL ScanIniRead(VOID);
VOID LOCAL PASCAL ScanInitDriveList(VOID);
VOID LOCAL PASCAL ScanWriteSelectedDrives(VOID);

VOID LOCAL PASCAL UpdateCheckboxes (
    ScanSelectDriveRec      *driveList,
    UINT                wDrives,
    DIALOGREC           dr);

VOID LOCAL PASCAL ToggleState (
    ScanSelectDriveRec      *driveList,
    UINT                wDrives,
    BYTE                byType,
    BYTE                byState);

BOOL LOCAL PASCAL TestForCheck (
    DIALOGREC           dr,
    ScanSelectDriveRec      *driveList,
    UINT                wDrives,
    BYTE                type);

STATIC int WINAPI DriveListFilter(char byLetter);

WORD SelDriveEventFilter(ListRec *list, WORD *c);

VOID SelDriveWriteEntry(ListRec *list,
                              VOID far *listEntry,
                        int attr,
                        DWORD dwEntryNum);

STATIC int SelDriveCheckMatch(ListRec *list,
                              VOID far *listEntry,
                              WORD c,
                              DWORD u);


VOID LOCAL PASCAL PreselectDrives (
    char                *pszSelections,
    ScanSelectDriveRec      *driveList,
    UINT                wDrives);

/************************************************************************
 *      STATIC VARS             STATIC VARS             STATIC VARS     *
 ************************************************************************/
static BOOL bTagMode = TAG_TOGGLE;
static BOOL bTagging = FALSE;

static BYTE bDisableNet;
static BYTE bDisableLocal;
static BYTE bDisableFloppy;
static BYTE bFloppies;
static BYTE bLocal;
static BYTE bNet;
static UINT wDrives;
static BOOL bNetworkDrivesExist;

static ScanSelectDriveRec      selDriveList[SYM_MAX_DOS_DRIVES];
static char                szDriveLetters[SYM_MAX_DOS_DRIVES];

static ScrollBarRec listScrollBar =
{
    0,0,0,
    SB_NEEDS_REDRAW,
    THIN_VERTICAL,
    0L,
    0L,
    0,0,0
};

extern char szDriveListTitle[];

static ListRec  rScanDriveList =
{
   NULL
};

static char            szDriveLettersFmt[FIELD_SIZE + 1];
#ifndef TTYSCAN
extern OutputFieldRec  ofDrivesField;
#endif

//************************************************************************
// EXTERNAL VARIABLES
//************************************************************************
extern  NAVOPTIONS      navOptions;


//************************************************************************
// InitializeSelectedDrives()
//
// Sets up the list box, and the selected drives strings based on the
// preselected types of drives.
// Called when opening the drive select dialog, or when preselecting types
// of drives from the command line.
//
// Parameters:
//      None
//
// Returns:
//      Nothing
//************************************************************************
// 9/12/93 DALLEE, Function created.
//************************************************************************

VOID PASCAL InitializeSelectedDrives()
{
    extern      CheckBoxRec         rAllNetCB;
    extern      CheckBoxRec         rAllHardCB;
    extern      CheckBoxRec         rAllFlopCB;

    ScanIniRead();
    bNetworkDrivesExist = TRUE;         // Assume they exist

    FlagCopy(rAllNetCB.flags,  CB_DISABLED, bDisableNet);
    FlagCopy(rAllHardCB.flags, CB_DISABLED, bDisableLocal);
    FlagCopy(rAllFlopCB.flags, CB_DISABLED, bDisableFloppy);

    ScanInitDriveList();
    ToggleState(selDriveList, wDrives, DTYPE_FLOPPY, rAllFlopCB.value);
    ToggleState(selDriveList, wDrives, DTYPE_LOCAL, rAllHardCB.value);
    ToggleState(selDriveList, wDrives, DTYPE_NETWORK, rAllNetCB.value);

    NAVDriveListToString(selDriveList, wDrives, szDriveLetters);

} // End InitializeSelectedDrives()


/**************************************************************************
 * ScanInitDriveList
 *
 * DESCRIPTION  : Initializes the drive list.
 *
 * IN      : none
 *
 * RETURN VALUES: none
 *************************************************************************
 * SKURTZ : 04-27-93 at 01:33:42pm Function created
 ************************************************************************* */
VOID LOCAL PASCAL ScanInitDriveList(VOID)
{
                                        // We don't want the UI popping up,
                                        // when determing drive types.  Turn
                                        // off the UI error handler for a while
    InitError(FALSE);                   // Don't use err dlg.

    wDrives = StdGetDriveList (szDriveLetters,
                               FILTER_CUSTOM,
                               DriveListFilter);

    rScanDriveList.active_entry =        // set the default drive in the list
                StdDlgGetCurrentDriveEntry (DiskGet(), szDriveLetters);

    /*
    ** setup the list box for displaying the drive letters
    */
    wDrives = NAVCreateDriveArray(selDriveList, szDriveLetters, szSelectedDrives);

    NAVDriveListToString (selDriveList, wDrives, szDriveLetters);

    StdDlgFormatDriveStr(szDriveLetters, szDriveLettersFmt);

    rScanDriveList.num = wDrives;
    rScanDriveList.entries = selDriveList;

                                        // set the drives that have been
                                        // preselected (just pass in a
                                        // string of drive letters)
    if (*szSelectedDrives)
        PreselectDrives (szSelectedDrives, selDriveList, wDrives);

    InitError(TRUE);                   // Use err dlg.
}


/**************************************************************************
 * ScanIniRead
 *
 * DESCRIPTION  : Sets initial checkboxes, based upon settings in NAV.BIN.
 *
 * IN      : none
 *
 * RETURN VALUES: none
 *************************************************************************
 * SKURTZ : 04-26-93 at 07:04:19pm Function created
 ************************************************************************* */
VOID LOCAL PASCAL ScanIniRead(VOID)
{

    extern      CheckBoxRec     rAllNetCB;
    extern      CheckBoxRec     rAllHardCB;
    extern      CheckBoxRec     rAllFlopCB;

    bDisableNet      = (BYTE) !navOptions.scanner.bAllowNetScan;
    rAllFlopCB.value = navOptions.scanner.bPresFlop;
    rAllHardCB.value = navOptions.scanner.bPresHard;
    rAllNetCB.value  = navOptions.scanner.bPresNet;

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
 * 01/06/1993 PETE Function Created.
 *----------------------------------------------------------------------*/

VOID PASCAL ScanDriveGetList(VOID)
{
    NAVDriveListToString (selDriveList, wDrives, szDriveLetters);
                                        // If no drives selected, then
                                        // default to the current active
                                        // one.
    if ( STRLEN(szDriveLetters) == 0 )
        {
        szDriveLetters[0] =
            selDriveList[rScanDriveList.active_entry].dl;
        szDriveLetters[1] = EOS;
        }

    STRCPY (szSelectedDrives, szDriveLetters);
}


/************************************************************************
 *                                                                      *
 * Description:                                                         *
 *                                                                      *
 * Parameters:                                                          *
 *                                                                      *
 * Return Value:                                                        *
 *                                                                      *
 * See Also:                                                            *
 *                                                                      *
 ************************************************************************
 * 09/18/1991 ???? Function Created.                                    *
 ************************************************************************/
VOID LOCAL PASCAL ToggleState (
    ScanSelectDriveRec      *driveList,
    UINT                wDrives,
    BYTE                byType,
    BYTE                byState)
{
    auto        UINT    i;

    for (i = 0; i < wDrives; i++)
      {
           if (driveList[i].type == byType)
               driveList[i].selected = byState;
      }

}


/************************************************************************
 *                                                                      *
 * Description:                                                         *
 *                                                                      *
 * Parameters:                                                          *
 *                                                                      *
 * Return Value:                                                        *
 *                                                                      *
 * See Also:                                                            *
 *                                                                      *
 ************************************************************************
 * 09/16/1991 DOUG Function Created.                                    *
 ************************************************************************/
UINT PASCAL NAVCreateDriveArray(
    ScanSelectDriveRec      *driveList,             // the array to build
    char                *pszDrvLet,             // the current drive letters
    char                *pszSelLet)             // current selections
{
    auto        UINT    wDriveType, wMinorType;
    auto        UINT    wDriveCount;
    auto        UINT    wIndex;
    auto        BOOL    bGetLabel;


    wDriveCount = 0;
    wIndex = 0;
    while (wIndex < STRLEN(pszDrvLet))
        {
        driveList[wIndex].dl = pszDrvLet[wDriveCount];
        if(pszSelLet != NULL)
            {
            driveList[wIndex].selected = (BYTE) (_fstrchr((CONST LPSTR)pszSelLet, pszDrvLet[wDriveCount]) != NULL);
            }
        else
            driveList[wIndex].selected = FALSE;

        DiskGetType(driveList[wIndex].dl, &wDriveType, &wMinorType);

        bGetLabel = FALSE;

        switch (wDriveType)
            {
            case TYPE_CDROM:
            case TYPE_REMOVEABLE:
                driveList[wIndex].type = DTYPE_FLOPPY;
                driveList[wIndex].byFlopType = (BYTE)DiskType(driveList[wIndex].dl);
                wDriveCount++;
                break;

            case TYPE_ASSIGNED:
            case TYPE_SUBST:
                break;

            case TYPE_FIXED:
            case TYPE_LOGICAL:
            case TYPE_ENCRYPTED:
            case TYPE_RAMDISK:
                driveList[wIndex].type = DTYPE_LOCAL;
                bGetLabel = TRUE;
                wDriveCount++;
                break;

            case TYPE_REMOTE:
                bGetLabel = TRUE;       // Fall through to DTYPE_NETWORK
                                        // Don't get label for INTERLINK
            case TYPE_INTERLINK:
                driveList[wIndex].type = DTYPE_NETWORK;
                wDriveCount++;
                break;

            case TYPE_PHANTOM:
                break;
            }

        if ( bGetLabel )
            VolumeLabelGet(driveList[wIndex].dl, (LPSTR)driveList[wIndex].szLabel);

        IconGetDisk (driveList[wIndex].dl, driveList[wIndex].driveIcon);

        wIndex++;
        }

    return(wDriveCount);
}

/************************************************************************
 *                                                                      *
 * Description:                                                         *
 *                                                                      *
 * Parameters:                                                          *
 *                                                                      *
 * Return Value:                                                        *
 *                                                                      *
 * See Also:                                                            *
 *                                                                      *
 ************************************************************************
 * 09/18/1991 DOUG Function Created.                                    *
 ************************************************************************/
UINT PASCAL NAVDriveListToString (ScanSelectDriveRec *driveList,
                                    UINT           wDrives,
                                    char           *pszDrives)
{
    register    UINT    wIndex;

    *pszDrives = EOS;

    for(wIndex = 0; wIndex < wDrives; wIndex++)
        {
        if (driveList[wIndex].selected)
            StringAppendChar(pszDrives, driveList[wIndex].dl);
        }

    return(STRLEN(pszDrives));

}


/************************************************************************
 *     LIST ROUTINES       LIST ROUTINES               LIST ROUTINES    *
 *     LIST ROUTINES       LIST ROUTINES               LIST ROUTINES    *
 *     LIST ROUTINES       LIST ROUTINES               LIST ROUTINES    *
 ************************************************************************/


/************************************************************************
 *                                                                      *
 * Description: Checks for key matches in the drive list.               *
 *                                                                      *
 * Parameters:                                                          *
 *                                                                      *
 * Return Value:                                                        *
 *                                                                      *
 * See Also:                                                            *
 *                                                                      *
 ************************************************************************
 * 09/16/1991 ???? Function Created.                                    *
 ************************************************************************/
STATIC int SelDriveCheckMatch(ListRec *list,
                              VOID far *listEntry,
                              WORD c,
                              DWORD u)
{
    auto    ScanSelectDriveRec far *entry = (ScanSelectDriveRec far *)listEntry;
    auto    BYTE     bValid;

    bValid = (BYTE) ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z'));

    if (!bValid || entry->dl < ' ')
        return (FALSE);

    return (entry->dl == (BYTE) CharToUpper ((UINT) c));
}

/************************************************************************
 *                                                                      *
 * Description: DoTag () -- Tag or untag a file in the program listbox  *
 *                                                                      *
 * Parameters:    driveRec = drive selection record                     *
 *                mode     = current bTagging mode                       *
 *                                                                      *
 *                              TAG_TOGGLE = toggle                     *
 *                              TAG_ON     = turn on                    *
 *                              TAG_OFF    = turn off                   *
 *                                                                      *
 * Return Value:  TRUE  = list display should be updated                *
 *                FALSE = list was not changed                          *
 *                                                                      *
 * See Also:                                                            *
 *                                                                      *
 * Note: I didn't create this function.  I cleaned up A-LOT of crap.    *
 *                                                                      *
 ************************************************************************
 * 09/19/1991 DOUG Function Created.                                    *
 ************************************************************************/
VOID LOCAL PASCAL DoTag (ListRec *list, UINT mode)
{
    auto        BYTE         bDisplay;
    auto        ScanSelectDriveRec  far *driveRec;

    bDisplay = FALSE;
    driveRec = &((ScanSelectDriveRec far *)list->entries)[list->active_entry];

    if (_IsTagged(driveRec))
        {
        if ((mode == TAG_TOGGLE) || (mode == TAG_OFF))
            {
            _TagClear(driveRec);
            bDisplay = TRUE;
            }
        }
    else
        {
        if (mode == TAG_TOGGLE || mode == TAG_ON)
            {
            _TagSet(driveRec);
            bDisplay = TRUE;
            }
        }

    if (bDisplay)
        ListDisplayEntry(list, list->active_entry, INVERSE);
}

/*-----------------------------------------------------------------------
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
 * 11/05/1991 PETERD Function Created.
 *----------------------------------------------------------------------*/

STATIC int WINAPI DriveListFilter(char byLetter)
{
    auto        UINT    wDriveType, wMinorType;

    DiskGetType(byLetter, &wDriveType, &wMinorType);

    switch (wDriveType)
        {
        case TYPE_CDROM:
        case TYPE_REMOVEABLE:
            return((BOOL)!bDisableFloppy);

        case TYPE_ASSIGNED:
        case TYPE_SUBST:
            break;

        case TYPE_FIXED:
        case TYPE_LOGICAL:
        case TYPE_ENCRYPTED:
        case TYPE_RAMDISK:
            return((BOOL)!bDisableLocal);

        case TYPE_REMOTE:
        case TYPE_INTERLINK:
            return((BOOL)!bDisableNet);
        }

    return(FALSE);
}

/*-----------------------------------------------------------------------
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
 * 11/14/1991 ED Function Created.
 *----------------------------------------------------------------------*/

VOID LOCAL PASCAL PreselectDrives (
    char                *pszSelections,
    ScanSelectDriveRec      *driveList,
    UINT                wDrives)
{
    register    UINT    wIndex;

    for (wIndex = 0; wIndex < wDrives; wIndex++)
        {
        driveList[wIndex].selected =
                (_fstrchr ((LPCSTR) pszSelections, driveList [wIndex].dl))
                ? TRUE : FALSE;
        }
}
