// Copyright 1994-1996 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/NAVWORK/VCS/n_virus.cpv   1.2   01 Jul 1998 16:22:26   tcashin  $
//
// Description:
//
// Contains:
//
// See Also:
//************************************************************************
// $Log:   S:/NAVWORK/VCS/n_virus.cpv  $
// 
//    Rev 1.2   01 Jul 1998 16:22:26   tcashin
// Added the ST_FILE bit to FILESTATUS_ZIPINFECTED in the arStatus array used
// in the IsMatchingStatusType() function.
// 
//    Rev 1.1   28 Jun 1998 11:49:42   tcashin
// Added another file status to status array in IsMatchingStatusType() to
// catch quarantine failures.
// 
//    Rev 1.0   06 Feb 1997 21:02:48   RFULLER
// Initial revision
// 
//    Rev 1.5   01 Nov 1996 12:33:04   JBRENNA
// 1. Add GetDialogParent().
// 2. Change SetProblemsFoundTabStops() and SetFillProblemTabStops() to use
//    CalcTabStops() instead of hard coding the tab stops.
// 
//    Rev 1.4   24 Oct 1996 19:08:24   JBRENNA
// Add SetProblemsFoundTabStops() and SetFillProblemTabStop() routines in
// order to correctly determine the tab stop on Japanese, Korean, Chinese,
// and English systems in either Large or Small font mode.
// 
//    Rev 1.3   27 Sep 1996 19:51:12   JBRENNA
// Add TabStops for when DBCSInstalled() returns TRUE. On DBCS machines the
// font is 9pt. Causing the ListBox tab stops to be different than on
// English machines.
// 
//    Rev 1.2   30 Jul 1996 18:27:54   djesch
// Plugged a memory leak, STS #60518.
//
//    Rev 1.1   09 May 1996 18:00:02   GWESTER
// Ported in Repair Wizard from Luigi Plus.
//
//    Rev 1.0   30 Jan 1996 15:55:52   BARRY
// Initial revision.
//
//    Rev 1.0   30 Jan 1996 14:50:54   BARRY
// Initial revision.
//
//    Rev 1.11   08 Dec 1995 13:58:10   GWESTER
// In both Win95 and NT, fixed 46166, in which file names over 15 chars were
// filling the Problems Found box all wrong.
//
//    Rev 1.10   24 Oct 1995 19:33:28   JWORDEN
// Separate processing of boot records and MBR's
// Process MBR filename as a single byte drive number
//
//    Rev 1.9   03 Jul 1995 12:02:26   KEITH
// Fix a buffer overwrite problem. A maxpath + some strings were
// stored in a maxpath size buffer.
//
//    Rev 1.8   09 May 1995 10:30:52   KEITH
// Correct some nouns
//
//************************************************************************

#include "platform.h"
#include "xapi.h"
#include "windowsx.h"
#include "cmbstr.h"
#include "symcfg.h"
#include "navutil.h"
#include "options.h"

#ifdef SYM_WIN16
    #include "syminteg.h"
#endif

#include "stub.h"
#include "nonav.h"
#include "nunavdlg.h"
#include "nunavstb.h"
#include "dbcs.h"
#include "CalcTabs.h"

                                        // In the Repair Wizard, we fill a custom listbox
                                        // full of viruses only. When the listbox is being
                                        // loaded (with one item per virus), we need to keep
                                        // track of which ones have already been loaded into
                                        // the listbox w/o using windows messages. We use
                                        // a temporary list of INFECTEDFILE:
#define SIZE_INFECTIONLISTARRAY 50
typedef struct tagVIRUSINFECTIONLIST
    {
    INFECTEDFILE    VirusInfection [SIZE_INFECTIONLISTARRAY];
    LPSTR           lpNext;             // this pointer contains another VIRUSINFECTIONLIST
    DWORD           dwInfectionsInStruct;   // Counts the number of INFECTEDFILEs used in VirusInfection []
    } VIRUSINFECTIONLIST, FAR *LPVIRUSINFECTIONLIST;
                                        // The first one is static; the rest are dynamic:
static VIRUSINFECTIONLIST vilStaticInfectionsList;

VOID LOCAL InfectionsListInit ( VOID );
BOOL LOCAL IsInfectionInList ( LPINFECTEDFILE lpNewItem );
VOID LOCAL AddInfectionToList ( LPINFECTEDFILE lpNewItem );
VOID LOCAL InfectionListDestroy ( VOID );



VOID SYM_EXPORT WINAPI AddVirusInfo (LPNONAV lpNav)
{
    auto  LPINFECTEDFILE lpInf;
    auto  LPINFARR lpInfArr;
    auto  DWORD    dwBunch;

    // ************************************ //
    // If this pointer seems valid continue //
    // ************************************ //

    if (lpNav->lpInfArr)
        {
        lpInfArr = (LPINFARR)lpNav->lpInfArr;

        while (lpInfArr->uNumUsed >= MAX_INFECTSTRUCTS)
            {
            if (lpInfArr->lpNext)
                lpInfArr = (LPINFARR)lpInfArr->lpNext;
            else
                {
                // **************************************************** //
                // Allocate memory to hold MAX_INFECTSTRUCTS structures //
                // **************************************************** //

                lpInfArr->lpNext = (LPSTR)MemAllocPtr (GHND, sizeof(INFARR));
                lpInfArr = (LPINFARR)lpInfArr->lpNext;

                dwBunch = (DWORD)sizeof(INFECTEDFILE) * MAX_INFECTSTRUCTS;
                lpInfArr->hInf = MemAlloc (GHND, dwBunch);
                }
            }

        // ********************************************** //
        // Copy the INFECTEDFILE structure into the array //
        // ********************************************** //

        if (lpInf = (LPINFECTEDFILE)MemLock(lpInfArr->hInf))
            {
            MEMCPY (&lpInf[lpInfArr->uNumUsed], &lpNav->Infected,
                    sizeof(INFECTEDFILE));

            MemUnlock(lpInfArr->hInf);
            }

        lpInfArr->uNumUsed++;
        }
    else
        {
        // **************************************************** //
        // Allocate memory to hold MAX_INFECTSTRUCTS structures //
        // **************************************************** //

        lpNav->lpInfArr = (LPINFARR)MemAllocPtr (GHND, sizeof(INFARR));
        dwBunch = (DWORD)sizeof(INFECTEDFILE) * MAX_INFECTSTRUCTS;
        lpNav->lpInfArr->hInf = MemAlloc(GHND, dwBunch);

        lpNav->lpInfArr->uNumUsed = 1;

        // ********************************************** //
        // Copy the INFECTEDFILE structure into the array //
        // ********************************************** //

        if (lpInf = (LPINFECTEDFILE)MemLock(lpNav->lpInfArr->hInf))
            {
            MEMCPY (lpInf, &lpNav->Infected, sizeof(INFECTEDFILE));
            MemUnlock(lpNav->lpInfArr->hInf);
            }
        }
}

VOID SYM_EXPORT WINAPI FreeLinkList(LPNONAV lpNav)
{
    auto  LPINFARR lpInfArr;
    auto  LPSTR    lpDoomed;

    lpDoomed = (LPSTR)lpNav->lpInfArr;

    while (lpDoomed)
        {
        lpInfArr = (LPINFARR)lpDoomed;

        if (lpInfArr->hInf)
            MemFree(lpInfArr->hInf);

        lpDoomed = lpInfArr->lpNext;

        if (lpInfArr)
            MemFreePtr(lpInfArr);
        }
}

//************************************************************************
// GetDialogParent()
//
// This routine finds the dialog parent of hCurWin. This is accomplished
// by walking up the parents of hCurWin.
//
// Parameters:
//   hWnd        [in] Starting window to look at
//************************************************************************
// 10/24/96 JBRENNA - created.
//************************************************************************
static HWND GetDialogParent (HWND hCurWin)
{
    auto   char   szClassName[32];

    GetClassName (hCurWin, szClassName, sizeof (szClassName));

    while (NULL != GetParent (hCurWin) &&
           0    != STRCMP (szClassName, "#32770"))
    {
        hCurWin = GetParent (hCurWin);
        GetClassName (hCurWin, szClassName, sizeof (szClassName));
    }

    return hCurWin;
}

//************************************************************************
// SetProblemsFoundTabStops()
//
// This routine sets the tab stops for the Problems Found listbox. Note
// that there are different tab stops for Japanese, Korean, Chinese, and
// English. This routine also accounts for Small / Large font modes.
//
// Parameters:
//   hDlg          [in] Dialog handle
//   hListBox      [in] Handle to the list box.
//************************************************************************
// 10/24/96 JBRENNA - created.
// 11/01/96 JBRENNA - Updated to use CalcTabStops().
//************************************************************************
static void SetProblemsFoundTabStops (const HWND hDlg, const HWND hListBox)
{
    auto   HWND  pCtrls[2]  = { GetDlgItem (hDlg, ID_SR_LISTVIRUSNAME),
                                GetDlgItem (hDlg, ID_SR_STATUS) };
    const  int   nTabs      = sizeof (pCtrls) / sizeof (pCtrls[0]);
    auto   int   pTabs[nTabs];

    CalcTabStops (hListBox, pCtrls, pTabs, nTabs);

    ListBox_SetTabStops (hListBox, nTabs, pTabs);
}

//************************************************************************
// ShowInfected()
//
// BOOL SYM_EXPORT WINAPI ShowInfected (hListbox, lpNav)
//  HWND    hListbox
//  LPNONAV lpNav
//
// This routine fills the given Problems Found listbox with all problem
// items found during a scan. It also sets each list entry's item data
// to the address of the infected item.
//
// Returns:
//  TRUE    on success
//  FALSE   if error adding to list box.
//************************************************************************
// ???
//************************************************************************

BOOL SYM_EXPORT WINAPI ShowInfected (HWND hListbox, LPNONAV lpNav)
{
    auto     UINT     i;
    auto     BOOL     bStatus = TRUE;
    auto     LPINFARR lpInfArr;
    auto     LPINFECTEDFILE lpInfected;
    auto     HGLOBAL  hBlock;

    SetProblemsFoundTabStops (GetDialogParent (hListbox), hListbox);

    if (lpNav->lpInfArr)
        {
        lpInfArr = lpNav->lpInfArr;

        do
            {
            if (lpInfected = (LPINFECTEDFILE)MemLock (lpInfArr->hInf))
                {
                for (i = 0; i < lpInfArr->uNumUsed && bStatus; i++)
                    bStatus = ShowInfectFile (hListbox, &lpInfected[i], 0xFFFF);

                                        // ------------------------------------
                                        // Out of memory
                                        // ------------------------------------
                if (bStatus == FALSE)
                    break;

                if (lpInfArr->uNumUsed == MAX_INFECTSTRUCTS)
                    {
                                        // ------------------------------------
                                        // Obtain next pointer before unlocking
                                        // ------------------------------------
                    hBlock = lpInfArr->hInf;
                    lpInfArr = (LPINFARR)lpInfArr->lpNext;
                    MemUnlock (hBlock);
                    }
                else
                    {
                                        // ------------------------------------
                                        // Unlock before zeroing out the pointer
                                        // ------------------------------------
                    MemUnlock (lpInfArr->hInf);
                    lpInfArr = NULL;
                    }
                }
            }
        while (lpInfArr);
        }

    return (bStatus);
}

//************************************************************************
// ShowInfectedFile()
//
// BOOL SYM_EXPORT WINAPI ShowInfectedFile
//    HWND            hListbox,
//    LPINFECTEDFILE  lpInfected,
//    UINT            uIndex
//
// Fills in the listbox with a virus name and other information.
//
// Returns:
//  TRUE    on success
//  FALSE   if error adding to listbox
//************************************************************************
// 07/30/96 DJESCH, freed the lpWork pointer when we're done with it.
//************************************************************************

BOOL SYM_EXPORT WINAPI ShowInfectFile (
    HWND            hListbox,
    LPINFECTEDFILE  lpInfected,
    UINT            uIndex
    )
{
                                        // If you tamper with this function, check
                                        // ShowVirusName() below, which does some of
                                        // the same things.

                                        // Return false if we can't alloc
                                        // memory or add to listbox for some
                                        // reason.
    auto    int     nIndex = LB_ERR;
    auto    char    szZipPath   [ SYM_MAX_PATH + 1 ];
    auto    LPSTR   lpWork;

                                        // lpWork can have a SYM_MAX_PATH
                                        // plus a virus name, plus some
                                        // additional string data.
    lpWork = (LPSTR) MemAllocPtr ( GHND, SYM_MAX_PATH*2 );

    if ( lpWork != NULL )
        {
        if (lpInfected->wStatus == FILESTATUS_BOOTINFECTED ||
            lpInfected->wStatus == FILESTATUS_BOOTREPAIRED ||
            lpInfected->wStatus == FILESTATUS_BOOTNOTREPAIRED)
            {
            STRCPY (lpWork, lpInfected->szFullPath);
            }
        else
            if (lpInfected->wStatus == FILESTATUS_MASTERBOOTINFECTED ||
                lpInfected->wStatus == FILESTATUS_MASTERBOOTREPAIRED ||
                lpInfected->wStatus == FILESTATUS_MASTERBOOTNOTREPAIRED)
                {
                SPRINTF (lpWork, String_Ptr (IDS_PHYS_DRIVE), lpInfected->szFullPath[0]);
                }
            else
                if (lpInfected->wStatus == FILESTATUS_ZIPINFECTED)
                    ParseZipFileName (lpInfected->szFullPath, szZipPath, lpWork);
                else
                    {
                    NameReturnFile(lpInfected->szFullPath, lpWork);
                    }

        NameShortenFileName ( lpWork, szZipPath, 13 );
        STRCPY ( lpWork, szZipPath );

                                        // ----------------------------------
                                        // Now append the virus name
                                        // ----------------------------------

        STRCAT (lpWork, "\t");
        STRCAT (lpWork, lpInfected->Notes.lpVirName);

                                        // ----------------------------------
                                        // And finally the completion status
                                        // ----------------------------------

        STRCAT (lpWork, "\t");

        FileStatusString(lpInfected->wStatus, &lpWork[STRLEN(lpWork)]);

        if (uIndex == 0xFFFF)
            nIndex = ListBox_AddString (hListbox, lpWork);
        else
            nIndex = ListBox_InsertString (hListbox, uIndex, lpWork);
        MemFreePtr(lpWork);

        if (nIndex != LB_ERR)
            SendMessage (hListbox, LB_SETITEMDATA, (WPARAM)nIndex,
                        (LPARAM)lpInfected);
        }

    return ((nIndex == LB_ERR || nIndex == LB_ERRSPACE) ? FALSE : TRUE);
}

//************************************************************************
// SetFillProblemTabStops()
//
// This routine sets the tab stops for the Problems Found listbox. Note
// that there are different tab stops for Japanese, Korean, Chinese, and
// English. This routine also accounts for Small / Large font modes.
//
// Parameters:
//   hDlg          [in] Dialog handle
//   hListBox      [in] Handle to the list box.
//************************************************************************
// 10/24/96 JBRENNA - created.
// 11/01/96 JBRENNA - Updated to use CalcTabStops().
//************************************************************************
static void SetFillProblemTabStops (const HWND hDlg, const HWND hListBox)
{
    auto   HWND  pCtrls[1]  = { GetDlgItem (hDlg, ID_SR_LISTVIRUSNAME) };
    const  int   nCtrls     = sizeof (pCtrls) / sizeof (pCtrls[0]);
    const  int   nTabs      = nCtrls + 1;
    auto   int   pTabs[nTabs];

    CalcTabStops (hListBox, pCtrls, pTabs, nCtrls);
    pTabs[1] = 250;

    ListBox_SetTabStops (hListBox, nTabs, pTabs);
}

//************************************************************************
// FillProblemListBox()
//
// BOOL WINAPI FillProblemListBox(hListbox, lpNav, dwItemTypes)
//  HWND    hListBox
//  LPNONAV lpNav
//  DWORD   dwItemTypes
//
// Fills the given list box from the list of problem items in the NONAV
// structure passed. dwItemTypes can be any of the following combined with
// a bitwise OR:
//      ST_VIRUS
//      ST_INOCULATION
//      ST_CAN_REPAIR
//      ST_CAN_DELETE
//      ST_CAN_EXCLUDE
//      ST_CAN_INOCULATE
//      ST_HAS_INFO
//      ST_LIVEVIRUS
//
//      ST_ALL
//  An item must match all criteria for to be added to the listbox.
//  ST_ALL will fill the listbox with all items.
//
// Returns:
//  TRUE    on success (may be zero matching items...)
//  FALSE   if error adding to listbox
//************************************************************************
// 04/11/96 DALLEE, pulled from ShowInfected()
//************************************************************************

BOOL WINAPI FillProblemListBox (HWND hListbox, LPNONAV lpNav, DWORD dwItemTypes)
{
    auto     UINT     i;
    auto     BOOL     bStatus = TRUE;
    auto     LPINFARR lpInfArr;
    auto     LPINFECTEDFILE lpInfected;
    auto     HGLOBAL  hBlock;

                                        // Tab stops should be passed in as a parameter.
                                        // For right now, since only the Repair Wizard will
                                        // use this function, use Repair Wizard values:
    SetFillProblemTabStops (GetDialogParent (hListbox), hListbox);

    if (lpNav->lpInfArr)
        {
        lpInfArr = lpNav->lpInfArr;

        do
            {
            if (lpInfected = (LPINFECTEDFILE)MemLock (lpInfArr->hInf))
                {
                for (i = 0; i < lpInfArr->uNumUsed && bStatus; i++)
                    {
                    if (IsMatchingStatusType(lpInfected[i].wStatus, dwItemTypes))
                        bStatus = ShowInfectFile (hListbox, &lpInfected[i], 0xFFFF);

                                        // ------------------------------------
                                        // Out of memory
                                        // ------------------------------------
                    if (bStatus == FALSE)
                        break;
                    }

                if (lpInfArr->uNumUsed == MAX_INFECTSTRUCTS)
                    {
                                        // ------------------------------------
                                        // Obtain next pointer before unlocking
                                        // ------------------------------------
                    hBlock = lpInfArr->hInf;
                    lpInfArr = (LPINFARR)lpInfArr->lpNext;
                    MemUnlock (hBlock);
                    }
                else
                    {
                                        // ------------------------------------
                                        // Unlock before zeroing out the pointer
                                        // ------------------------------------
                    MemUnlock (lpInfArr->hInf);
                    lpInfArr = NULL;
                    }
                }
            else
                {
                bStatus = FALSE;
                break;
                }
            }
        while (lpInfArr);
        }

    return (bStatus);
} // FillProblemListBox()


//************************************************************************
// IsMatchingStatusType()
//
// BOOL WINAPI IsMatchingStatusType (wStatus, dwTypes)
//  WORD    wStatus
//  DWORD   dwTypes
//
// This function checks whether the status passed is one of the specified
// types. dwTypes can be any of the following combined with a bitwise OR:
//      ST_VIRUS
//      ST_INOCULATION
//      ST_CAN_REPAIR
//      ST_CAN_DELETE
//      ST_CAN_EXCLUDE
//      ST_CAN_INOCULATE
//      ST_HAS_INFO
//      ST_LIVEVIRUS
//      ST_ZIP
//      ST_FILE
//      ST_MBR
//      ST_BOOT
//
//      ST_ALL
//  wStatus must match all criteria for this function to return true.
//  ST_ALL will return TRUE always.
//
// Returns:
//      TRUE    if matches
//      FALSE   otherwise
//************************************************************************
// 04/11/96 DALLEE, created.
//************************************************************************

BOOL WINAPI IsMatchingStatusType (WORD wStatus, DWORD dwTypes)
{
    typedef struct tagSTATUSTYPE
        {
        WORD    wStatus;
        DWORD   dwFlags;
        } STATUSTYPE;
                                           //  VIRUS           REPAIR         EXCLUDE           INFO            ZIP             MBR
                                           //       INOCULATION        DELETE         INOCULATE      LIVE VIRUS        FILE             BOOT
    static  STATUSTYPE  arStatuses [] =    // --------------------------------------------------------------------------------------------------
    {
       {FILESTATUS_INFECTED,                0 +(1<<0)         +(1<<2) +(1<<3) +(1<<4)         +(1<<6) +(1<<7)         +(1<<9)                 },
       {FILESTATUS_REPAIRED,                0 +(1<<0)                                         +(1<<6)                 +(1<<9)                 },
       {FILESTATUS_NOTREPAIRED,             0 +(1<<0)                 +(1<<3) +(1<<4)         +(1<<6) +(1<<7)         +(1<<9)                 },
       {FILESTATUS_DELETED,                 0 +(1<<0)                                         +(1<<6)                 +(1<<9)                 },
       {FILESTATUS_NOTDELETED,              0 +(1<<0)                         +(1<<4)         +(1<<6) +(1<<7)         +(1<<9)                 },

       {FILESTATUS_EXCLUDE_INOCULATION,     0         +(1<<1)                                                         +(1<<9)                 },
       {FILESTATUS_EXCLUDE_VIRUSDETECTION,  0 +(1<<0)                                         +(1<<6)                 +(1<<9)                 },
       {FILESTATUS_EXCLUDE_FAILED,          0 +(1<<0)                                                                 +(1<<9)                 },

       {FILESTATUS_INOCULATED,              0         +(1<<1)                                                         +(1<<9)                 },
       {FILESTATUS_INOCULATIONFAILED,       0         +(1<<1)         +(1<<3) +(1<<4)                                 +(1<<9)                 },
       {FILESTATUS_INOCULATEREPAIRED,       0         +(1<<1)                                                         +(1<<9)                 },
       {FILESTATUS_INOCULATEREPAIR_FAILED,  0         +(1<<1)         +(1<<3)                                         +(1<<9)                 },
       {FILESTATUS_INOCULATEDELETED,        0         +(1<<1)                                                         +(1<<9)                 },
       {FILESTATUS_INOCULATEDELETE_FAILED,  0         +(1<<1)                                                         +(1<<9)                 },

       {FILESTATUS_INOC_CHANGED_DELETED,    0         +(1<<1)                                                         +(1<<9)                 },
       {FILESTATUS_INOC_CHANGED_NOTDELETED, 0         +(1<<1)                                                         +(1<<9)                 },

       {FILESTATUS_REINOCULATED,            0         +(1<<1)                                                         +(1<<9)                 },
       {FILESTATUS_REINOCULATIONFAILED,     0         +(1<<1)         +(1<<3) +(1<<4)                                 +(1<<9)                 },

       {FILESTATUS_NOTINOCULATED,           0         +(1<<1)         +(1<<3) +(1<<4) +(1<<5)                         +(1<<9)                 },
       {FILESTATUS_INOCULATEDCHANGED,       0         +(1<<1) +(1<<2) +(1<<3) +(1<<4) +(1<<5)                         +(1<<9)                 },

       {FILESTATUS_BOOTINFECTED,            0 +(1<<0)         +(1<<2)                         +(1<<6) +(1<<7)                         +(1<<11)},
       {FILESTATUS_BOOTREPAIRED,            0 +(1<<0)                                         +(1<<6)                                 +(1<<11)},
       {FILESTATUS_BOOTNOTREPAIRED,         0 +(1<<0)                                         +(1<<6) +(1<<7)                         +(1<<11)},

       {FILESTATUS_MASTERBOOTINFECTED,      0 +(1<<0)         +(1<<2)                         +(1<<6) +(1<<7)                 +(1<<10)        },
       {FILESTATUS_MASTERBOOTREPAIRED,      0 +(1<<0)                                         +(1<<6)                         +(1<<10)        },
       {FILESTATUS_MASTERBOOTNOTREPAIRED,   0 +(1<<0)                                         +(1<<6) +(1<<7)                 +(1<<10)        },

       {FILESTATUS_ZIPINFECTED,             0 +(1<<0)                                         +(1<<6) +(1<<7) +(1<<8)+(1<<9)                  },

       {FILESTATUS_QUARANTINE_INFECTED_FAILED,0 +(1<<0)                                       +(1<<6) +(1<<7)                                 },

       {0, 0}
    };

    auto    int     i;


    if (ST_ALL == dwTypes)
        {
        return (TRUE);
        }

                                        // Check FILESTATUS matches for
                                        // each bitflag in dwTypes.
    for (i = 0; 0 != arStatuses[i].wStatus; i++)
        {
        if (arStatuses[i].wStatus == wStatus)
            {
            return (dwTypes == (arStatuses[i].dwFlags & dwTypes));
            }
        }

    return (FALSE);
} // IsMatchingStatusType()


//************************************************************************
// IsStatusTypeInList()
//
// BOOL WINAPI IsStatusTypeInList (lpNav, dwType)
//  LPNONAV lpNav
//  DWORD   dwType
//
// This routine checks whether at least one item was found during scanning
// whose state now matches the types passed in.
// dwType consists of any valid ST_xxx defines that can be passed to
// IsMatchingStatusType().
//
// Returns:
//      TRUE    if at least one item in the problems found list matches
//                  all flags specified.
//      FALSE   otherwise
//************************************************************************
// 04/18/96 DALLEE, created.
//************************************************************************

BOOL WINAPI IsStatusTypeInList (LPNONAV lpNav, DWORD dwType)
{
    auto     UINT     i;
    auto     BOOL     bFound = FALSE;
    auto     LPINFARR lpInfArr;
    auto     LPINFECTEDFILE lpInfected;
    auto     HGLOBAL  hBlock;

    if (lpNav->lpInfArr)
        {
        lpInfArr = lpNav->lpInfArr;

        do                              // For each block of items.
            {
            lpInfected = (LPINFECTEDFILE)MemLock (lpInfArr->hInf);

            if (NULL == lpInfected)
                {
                break;
                }
            else
                {
                                        // Check each item in block
                for (i = 0; i < lpInfArr->uNumUsed && !bFound; i++)
                    {
                    if (IsMatchingStatusType(lpInfected[i].wStatus, dwType))
                        bFound = TRUE;
                    }

                                        // Temp save handle to current
                                        // block's data.
                hBlock = lpInfArr->hInf;

                                        // Obtain next pointer before unlock.
                if ( bFound ||
                     (lpInfArr->uNumUsed != MAX_INFECTSTRUCTS) )
                    {
                    lpInfArr = NULL;    // we're done.
                    }
                else
                    {
                    lpInfArr = (LPINFARR)lpInfArr->lpNext;
                    }
                                        // Unlock current block's data.
                MemUnlock (hBlock);
                }
            }
        while (lpInfArr && !bFound);
        }

    return (bFound);
} // IsStatusTypeInList()


//************************************************************************
// FillVirusListBox()
//
// BOOL WINAPI FillVirusListBox(hListbox, lpNav)
//  HWND    hListBox
//  LPNONAV lpNav
//
// Fills the given list box with the names of viruses infecting the items in the NONAV
// structure passed, with no repeats. Only items matching the ST_LIVEVIRUS criteria
// (see above) will be checked.
//
// This is used by the Initial panel in the Repair Wizard, where a listbox is loaded
// with the names of viruses detected in the last scan.
//
// Since we need to know what we've already loaded into the listbox, and we can't use
// Windows messages to determine this, we keep a temporary list of INFECTEDFILE
// structs (one per virus name loaded).
//
// Returns:
//  TRUE    on success (may be zero viruses loaded...)
//  FALSE   if error adding to listbox
//************************************************************************
// 04/24/96 GWESTER, Initial Revision
//************************************************************************
BOOL WINAPI FillVirusListBox (HWND hListbox, LPNONAV lpNav)
{
                                        // Don't need tabstops in the Virus listbox:
    //static   int      lpTabStops[] = {93, 250};
    auto     UINT     i;
    auto     BOOL     bStatus = TRUE;
    auto     LPINFARR lpInfArr;
    auto     LPINFECTEDFILE lpInfected;
    auto     HGLOBAL  hBlock;

                                        // Don't need tabstops in the Virus listbox:
    //ListBox_SetTabStops(hListbox, 2, lpTabStops);

                                        // Initialize the temporary list:
    InfectionsListInit();

    if (lpNav->lpInfArr)
        {
        lpInfArr = lpNav->lpInfArr;

        do
            {
            if (lpInfected = (LPINFECTEDFILE)MemLock (lpInfArr->hInf))
                {
                for (i = 0; i < lpInfArr->uNumUsed && bStatus; i++)
                    {
                    if (IsMatchingStatusType(lpInfected[i].wStatus, ST_LIVEVIRUS))
                        {
                                        // If this Infected Item is not in our
                                        // virus array, add it in. Also add it to
                                        // the listbox. (We keep a separate list
                                        // so that we don't need to use Windows
                                        // messages to find out what viruses we've
                                        // already stored in the listbox.)
                        if ( ! ( IsInfectionInList ( &(lpInfected[i])) ) )
                            {
                            AddInfectionToList ( &(lpInfected[i]) ) ;
                            bStatus = ShowVirusName (hListbox, &lpInfected[i], 0xFFFF);
                            }


                        }

                                        // ------------------------------------
                                        // Out of memory
                                        // ------------------------------------
                    if (bStatus == FALSE)
                        break;
                    }

                if (lpInfArr->uNumUsed == MAX_INFECTSTRUCTS)
                    {
                                        // ------------------------------------
                                        // Obtain next pointer before unlocking
                                        // ------------------------------------
                    hBlock = lpInfArr->hInf;
                    lpInfArr = (LPINFARR)lpInfArr->lpNext;
                    MemUnlock (hBlock);
                    }
                else
                    {
                                        // ------------------------------------
                                        // Unlock before zeroing out the pointer
                                        // ------------------------------------
                    MemUnlock (lpInfArr->hInf);
                    lpInfArr = NULL;
                    }
                }
            else
                {
                bStatus = FALSE;
                break;
                }
            }
        while (lpInfArr);
        }

                                        // Free the temporary list:
    InfectionListDestroy ();

    return (bStatus);
}                                       // FillVirusListBox()



//************************************************************************
// InfectionsListInit()
//
// VOID LOCAL InfectionsListInit()
//
// Initialize the temporary list (see comments at the top of this file, and
// those in FillVirusListBox() ).
//
//************************************************************************
// 04/24/96 GWESTER, Initial Revision
//************************************************************************
VOID LOCAL InfectionsListInit ( VOID )
{
vilStaticInfectionsList.dwInfectionsInStruct = 0 ;
vilStaticInfectionsList.lpNext = NULL ;
}                                       // End InfectionsListInit ()


//************************************************************************
// IsInfectionInList()
//
// BOOL LOCAL IsInfectionInList(LPINFECTEDFILE lpNewItem)
//
// Look for a virus name in the temporary list (see comments at the top of this file, and
// those in FillVirusListBox() ).
//
// Returns:
//  TRUE    Virus is already in the temporary list
//  FALSE   Virus is not in the temporary list
//************************************************************************
// 04/24/96 GWESTER, Initial Revision
//************************************************************************
BOOL LOCAL IsInfectionInList ( LPINFECTEDFILE lpNewItem )
{
LPVIRUSINFECTIONLIST lpListBlock = &vilStaticInfectionsList;
DWORD dwInfectionCounter;

if (!lpNewItem) { return FALSE ; }

while ( lpListBlock )
    {
                                        // Check this list block:
    dwInfectionCounter = 0;
    while ( dwInfectionCounter < (lpListBlock->dwInfectionsInStruct) )
        {
                                        // Check this item for a match:
        if ( ( lpNewItem->Notes.wVirID ) == (((lpListBlock->VirusInfection)[dwInfectionCounter]).Notes.wVirID) )
            { return TRUE ; }
        dwInfectionCounter++;
        }
                                        // Move to the next one:
    lpListBlock = (LPVIRUSINFECTIONLIST)(lpListBlock->lpNext);
    }
                                        // Not found
return FALSE;
}                                       // End IsInfectionInList()



//************************************************************************
// AddInfectionToList()
//
// VOID LOCAL AddInfectionToList ( LPINFECTEDFILE lpNewItem )
//
// Add a virus name to the temporary list (see comments at the top of this file, and
// those in FillVirusListBox() ).
//
//************************************************************************
// 04/24/96 GWESTER, Initial Revision
//************************************************************************
VOID LOCAL AddInfectionToList ( LPINFECTEDFILE lpNewItem )
{
LPVIRUSINFECTIONLIST lpListBlock = &vilStaticInfectionsList;
LPVIRUSINFECTIONLIST lpFollowPtr = lpListBlock;
if (!lpNewItem) { return ; }

while ( lpListBlock )
    {
                                        // Is this one full?
    if (( lpListBlock->dwInfectionsInStruct ) < SIZE_INFECTIONLISTARRAY )
        {
                                        // Add it in this block:
        MEMCPY ( &((lpListBlock->VirusInfection)[lpListBlock->dwInfectionsInStruct]), lpNewItem, sizeof(INFECTEDFILE) );
        ( lpListBlock->dwInfectionsInStruct ) ++;
        return ;
        }
                                        // Move to the next block:

                                        // Save a following pointer in case we have to add
                                        // a block:
    lpFollowPtr = lpListBlock;
    lpListBlock = (LPVIRUSINFECTIONLIST)(lpListBlock->lpNext);
    }                                   // End while


                                        // Add a block:
    lpFollowPtr->lpNext = (LPSTR) MemAllocPtr ( GHND, sizeof ( VIRUSINFECTIONLIST ) );
    if ( ! ( lpFollowPtr->lpNext ) ) {return;}
    lpListBlock = (LPVIRUSINFECTIONLIST)(lpFollowPtr->lpNext) ;

    lpListBlock->dwInfectionsInStruct = 1;
    lpListBlock->lpNext = NULL;

    MEMCPY ( &((lpListBlock->VirusInfection)[0]), lpNewItem, sizeof(INFECTEDFILE) );

}                                       // AddInfectionToList()



//************************************************************************
// InfectionListDestroy()
//
// VOID LOCAL InfectionListDestroy ( VOID )
//
// Destroy the temporary list (see comments at the top of this file, and
// those in FillVirusListBox() ).
//
//************************************************************************
// 04/24/96 GWESTER, Initial Revision
//************************************************************************
VOID LOCAL InfectionListDestroy ( VOID )
{
vilStaticInfectionsList.dwInfectionsInStruct = 0 ;
LPVIRUSINFECTIONLIST lpWalkingPtr = (LPVIRUSINFECTIONLIST)(vilStaticInfectionsList.lpNext);
LPVIRUSINFECTIONLIST lpVictimPtr;

                                        // The first one is static; the rest are dynamic:
while ( lpWalkingPtr != NULL )
    {
    lpVictimPtr = lpWalkingPtr ;
    lpWalkingPtr = (LPVIRUSINFECTIONLIST)(lpVictimPtr->lpNext);
    MemFree ( lpVictimPtr );
    }
}                                       // End InfectionListDestroy ()


//************************************************************************
// ShowVirusName()
//
// BOOL SYM_EXPORT WINAPI ShowVirusName
//    HWND            hListbox,
//    LPINFECTEDFILE  lpInfected,
//    UINT            uIndex
//
// Fills the listbox with a virus name only. Doesn't do anything fancy with tabs (like
// ShowInfectFile() does). This function does, however, attach a link back to
// the NONAV struct with LB_SETITEMDATA.
//
// Returns:
//  TRUE    on success
//  FALSE   if error adding to listbox
//************************************************************************
// 04/25/96 GWESTER, Initial Revision
// 07/30/96 DJESCH, freed the lpWork pointer when we're done with it.
//************************************************************************
BOOL SYM_EXPORT WINAPI ShowVirusName (
    HWND            hListbox,
    LPINFECTEDFILE  lpInfected,
    UINT            uIndex
    )
{
                                        // Return false if we can't alloc
                                        // memory or add to listbox for some
                                        // reason.
    auto    int     nIndex = LB_ERR;
    auto    LPSTR   lpWork;

                                        // lpWork will have a virus name
    lpWork = (LPSTR)MemAllocPtr ( GHND, SYM_MAX_PATH );

    if ( lpWork != NULL )
        {
                                        // ----------------------------------
                                        // Now append the virus name
                                        // ----------------------------------
        STRCPY (lpWork, lpInfected->Notes.lpVirName);

        if (uIndex == 0xFFFF)
            nIndex = ListBox_AddString (hListbox, lpWork);
        else
            nIndex = ListBox_InsertString (hListbox, uIndex, lpWork);
        MemFreePtr(lpWork);

        if (nIndex != LB_ERR)
            SendMessage (hListbox, LB_SETITEMDATA, (WPARAM)nIndex,
                        (LPARAM)lpInfected);
        }

    return ((nIndex == LB_ERR || nIndex == LB_ERRSPACE) ? FALSE : TRUE);
}                                       // End ShowVirusName()


