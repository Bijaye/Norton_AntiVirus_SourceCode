// Copyright 1992-1993 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/VIRLIST/VCS/v_info.cpv   1.49.1.4   09 Jul 1998 17:23:12   DBuches  $
//
// Description:
//      The function that presents the virus list for viewing or editing
//
// Contains (as exports):
//      VirusList()
//      VirusListProc()
//      BuildVirusListForPrinting()
//
//************************************************************************
// $Log:   S:/VIRLIST/VCS/v_info.cpv  $
// 
//    Rev 1.49.1.4   09 Jul 1998 17:23:12   DBuches
// Removed HowTo menus.
// 
//    Rev 1.49.1.3   02 Jul 1998 13:01:18   mdunn
// Removed reference to Delete button help ID.
// 
//    Rev 1.49.1.2   29 Jun 1998 13:41:44   DBuches
// I wish these people would make up thier minds about date formats.
// 
//    Rev 1.49.1.1   26 Jun 1998 18:19:32   mdunn
// Now #includeing navwhlp5.h
// 
//    Rev 1.49.1.0   22 Jun 1998 20:02:48   dwilson
// Branch base for version QAKN
// 
//    Rev 1.49   18 Jun 1998 13:53:36   DBuches
// Display date in users local date format.
// 
//    Rev 1.48   29 May 1998 14:02:44   DBuches
// Now, correctly classify macro viruses.
//
//    Rev 1.47   12 Sep 1997 11:47:50   KSACKIN
// Ported changes from QAKG.
//
//    Rev 1.46.1.2   04 Sep 1997 17:08:18   KSACKIN
// Changed a TabStop variable to be static versus auto, as NT3.51 needs this
// variable to be around later on.  Their function to set tab stops uses this
// later on, so we need it to be static.
//
//    Rev 1.46.1.1   18 Aug 1997 10:03:34   DBUCHES
// Fixed problem with displaying international dates.
//
//    Rev 1.46.1.0   12 Aug 1997 22:43:48   DALVIDR
// Branch base for version QAKG
//
//    Rev 1.46   20 May 1997 18:58:08   MKEATIN
// We're now linking to an avenged api.
//
//    Rev 1.45   03 Dec 1996 14:30:20   MKEATIN
// Ported CORE F
//
//    Rev 1.44.1.2   01 Nov 1996 12:22:10   JBRENNA
// Use CalcTabStops() instead of hard coding the tab stop locations.
//
//    Rev 1.44.1.1   24 Oct 1996 18:59:54   JBRENNA
// Add GetTabStop() in order to correctly determine the tab stop on Japanese,
// Korean, Chinese, and English systems in either Large or Small font mode.
//
//    Rev 1.44.1.0   23 Oct 1996 13:13:04   RJACKSO
// Branch base for version QAKF
//
//    Rev 1.44   30 Sep 1996 17:55:36   JBRENNA
// Update the ListBox's tab stop for Japanese Win95. The column title did
// not line up with the column text. Also, moved the whole column left
// (both Japanese and English) so that the "Boot/Programs" infects column
// does not get clipped.
//
//    Rev 1.43   12 Aug 1996 17:13:22   RPULINT
// Added macro, windows and agent virus types
//
//    Rev 1.42   23 Jul 1996 07:36:28   TIVANOV
// fix the dialog font initilization
//
//    Rev 1.41   17 Jul 1996 17:06:16   PVENKAT
//
//    Rev 1.40   27 Jun 1996 11:27:18   JBRENNA
// Merge LuigiPlus into Gryphon:
// Modified to get the path of a file to be printed by using functions of
// 'GetStartDir()' and 'NameReturnLongName()' instead of functions of
// 'GetModuleFileName()' and 'NameStripFile()' so that the path returned will
// not be all uppercases.  The function 'GetStartDir()' was extracted from
// LUIGI\NAVXUTIL\winmisc.cpp. (Defect No. 58204) ... YVANG
//
//    Rev 1.39   11 Jun 1996 15:07:18   PVENKAT
// 57243:  Modified the code to have a fixed array of ids for Help.
//
//    Rev 1.38   07 Mar 1996 12:55:40   jworden
// Move list box tab to the right to avoid loss of virus type info in
// international fonts
//
//    Rev 1.37   01 Mar 1996 12:02:46   JWORDEN
// DBCS enable
//
//    Rev 1.35   24 Jan 1996 17:36:04   GDZIECI
// Include STDHELP.H for WIN16 platform.
//
//    Rev 1.34   16 Jan 1996 17:00:32   JWORDEN
// Port Quake 6 to trunk
//
//    Rev 1.14.1.9   29 Nov 1995 15:31:50   DSACKIN
// Make sure that when we are using the main virus list that we display
// the total number of viruses detected.
//
//    Rev 1.14.1.8   21 Nov 1995 13:52:06   DSACKIN
// Changed behavior for the ComboBox to not update the virus list until the
// user finishes selecting from the dropdown list.
//
//    Rev 1.14.1.7   13 Nov 1995 21:54:14   DSACKIN
// Modified to use the VLIST virtual list box.
//
//    Rev 1.14.1.6   06 Jul 1995 18:25:20   KEITH
// Make the list show the count of viruses detected from the .DAT
// file rather than the listbox definitions count. Remove a memory
// overwrite that was apparently benign.
//
//    Rev 1.14.1.5   09 Mar 1995 13:36:18   MFALLEN
//
//    Rev 1.14.1.4   09 Mar 1995 12:29:22   KEITH
// Allow NavNlmW to load the virus list without the INSCAN?.DAT files.
//
//    Rev 1.14.1.3   08 Mar 1995 19:22:46   MFALLEN
// Stronger checking on what the AVAPI functions provide to us.
//
//    Rev 1.14.1.2   28 Feb 1995 17:53:26   KEITH
// Add an hour glass cursor for when the virus list is closing down.
//
//    Rev 1.14.1.1   22 Feb 1995 19:55:50   KEITH
// Undo last change due to bogisity.
//
//    Rev 1.15   18 Feb 1995 18:36:04   KEITH
// When OK button has been renamed (to Cancel) move BS_DEFPUSHBUTTON to Info.
//
//    Rev 1.14   07 Dec 1994 15:45:48   KEITH
// Changed the return value of VirusList to be an int, this is what was found
// in the prototype in VIRLIST.H so that did not need to change. Along the same
// line, changed bDatUpdated to nDatUpdated, and removed the "!!" from nDatUpdated
// so that other values may be returned.
//
// Removed the MakeProcInstance for VirusListProc from the Win16 compile and made
// the Win32 and the Win16 code match. Since VirusListProc is contained in a DLL
// (this DLL), the return value of MakeProcInstance is simply the address of
// VirusListProc itself.
//
//
//    Rev 1.13   06 Dec 1994 16:28:10   MFALLEN
// Fixed double click bug.
//
//    Rev 1.12   02 Dec 1994 13:15:32   MFALLEN
// Fixed deletion of virus definitions problem.
//
//    Rev 1.11   22 Nov 1994 15:19:46   KEITH
// Set the title of the OK button to the value of VIRLIST.szOKButtonTitle
// if the string is not empty. Also if szOKButtonTitle is not empty,
// VirusList() will return one of the following:
//         VIR_OK_CHANGED, VIR_OK_UNCHANGED, or VIR_OK_CANCELED.
//
//************************************************************************

#include "platform.h"
#include "ngui.h"
#include "xapi.h"
#include "symintl.h"
#include "npt.h"
#include <ctl3d.h>
#include "file.h"
#include "dbcs.h"

#include "stdhelp.h"
#include "navwhlp5.h"

#include "virusdbf.h"                   // this includes virlist.h

#include "vlist.h"                      // for specialized virtual list box
#include "CalcTabs.h"


#include <winuser.h>

extern  HINSTANCE   hInstance;
TCHAR            g_sNAVDir[SYM_MAX_PATH+1];
TCHAR            g_sTasksDir[SYM_MAX_PATH+1];
TCHAR              g_szDefsDir [SYM_MAX_PATH+1];
BOOL               g_bOSIsNT;

//to fix defect in Virus list
HINSTANCE   hinstNavalog = NULL;
typedef int (CALLBACK* LPFN_TEMPACTIVITYLOG)( HWND, LPSTR ) ;
LPFN_TEMPACTIVITYLOG  lpfnTempActivityLog;


LPTSTR glpSearchstr = (LPTSTR)MemAllocPtr(GHND, 255);

typedef struct tagSEARCH
{
    UINT                uIndex;         // Index in moma listbox
    UINT                uNumItems;      // Number of items in moma listbox
    HVLVIRUSENTRY       hEntry;         // entry currently displayed
    LPVIRSEARCH         lpVirSearch;      // init structure from moma window
  
} SEARCH, FAR * LPSEARCH;


static UINT uCombo[] =
    {
    IDS_ALLVIRUSES,
    IDS_COMMONVIRUSES,
    IDS_PROGINFECTORS,
    IDS_BOOTINFECTORS,
    IDS_STEALTH,
    IDS_POLYMORPHIC,
    IDS_MULTIPARTY,
    IDS_MACRO,
    IDS_WINDOWS,
    IDS_AGENT,
    IDS_MACVIRUSES,
    0
    };

TCHAR szWork [ SYM_MAX_PATH + 1];

#define INFECTS_FILE        1
#define INFECTS_BOOT        2
#define INFECTS_FILEANDBOOT 3

// --------------------------------------------------------------------------
//                      LOCAL FUNCTION PROTOTYPES
// --------------------------------------------------------------------------

BOOL SYM_EXPORT WINAPI VirusListProc (  // process messages for virus list
    HWND    hDlg,                       // [in] window handle
    UINT    uMessage,                   // [in] message to process
    WPARAM  wParam,                     // [in] wparam
    LPARAM  lParam );                   // [in] lparam

VOID LOCAL OnCommand (                  // Process the WM_COMMAND messages
    HWND    hDlg,                       // [in] from this dialog box
    int     nID,                        // [in] and this control ID
    HWND    hwndCtl,                    // [in] control window handle
    UINT    codeNotify );               // [in] control notification code

VOID LOCAL DoPrint (                    // Process printing from
    HWND        hDlg,                   // [in] this dialog box
    LPVIRUSDBF  lpVir );                // [in] virus database pointer



VOID WINAPI VirusSearch (LPVIRSEARCH   lpVirSearch 
	 ) ;

BOOL  DoSearch( HWND hDlg, LPSEARCH     lpSearch, LPTSTR lpStr ) ;

BOOL DoSearchAgain( HWND        hDlg, LPVIRUSDBF lpVir ) ;

BOOL SYM_EXPORT WINAPI VirusSearchProc (  // Handle message to Search window
    HWND    hDlg,                       // [in] info window handle
    UINT    uMessage,                   // [in] message to process
    WPARAM  wParam,                     // [in] wparam
    LPARAM  lParam );                   // [in] lparam


BOOL InitGlobals() ;

VOID LOCAL FreeVirusHandles (           // Free all the virus handles
    LPVIRUSDBF  lpVir );                // [in] pointer to virus list

VOID LOCAL FillOneListBox (             // Fill the specified listbox
    HWND        hDlg,                   // [in] parent window handle
    UINT        nIndex,                 // [in] type of virus requested
    LPVIRUSDBF  lpVir );                // [in] virus database pointer

BOOL LOCAL ValidVirusType (             // Validate this virus entry
    HVLVIRUSENTRY hEntry,                 // [in] virus entry
    WORD        wTypeRequested,         // [in] type requested
    BOOL        bMacViruses );          // [in] TRUE to consider Mac viruses

WORD LOCAL GetVirusInfects (            // Validate this virus entry
    HVLVIRUSENTRY hEntry );               // [in] virus entry

VOID LOCAL DoInitDialog (               // Process WM_INITDIALOG message
    HWND        hDlg,                   // [in] from this window
    LPARAM      lParam );               // [in] user param

VOID LOCAL FixButtons (                 // Hide required buttons
    HWND        hDlg,                   // [in] window handle
    LPVIRUSDBF  lpVir );                // [in] pointer

VOID LOCAL FixButtons (                 // Hide required buttons
    HWND        hDlg,                   // [in] window handle
    LPVIRUSDBF  lpVir );                // [in] pointer

VOID LOCAL SetupIndexLists (            // Setup Indexes for our VList
    LPVIRUSDBF  lpVir,                  // [in/out] indexes are set here.
    LPHVLVIRUSENTRY lphVirusArray,        // [in] data that has been read in
    BOOL bShowMacViruses );             // [in] do we display mac viruses?

VOID WINAPI SortVirusList (             // We have to sort for ourselves with VList
    LPHVLVIRUSENTRY lphVirusArray,        // List to sort.
    UINT          uEntriesLoaded);      // Number of entries in the list

VOID LOCAL GetStartDir (HINSTANCE hInstance, LPSTR lpPath, UINT uSize);

#ifdef SYM_WIN32


VOID LOCAL OnContextMenu (              // Help on right mouse button click
    HWND    hWnd,                       // [in] window that needs help
	UINT	msg,
	WPARAM	wParam,
    LPARAM  lParam );                   // [in] mouse coordinates

BOOL LOCAL ClickOnListbox (             // Returns TRUE if the user clicked
    HWND    hWnd,                       // on a listbox within this dialog box.
    LPARAM  lParam );                   // [in] point where the user clicked.

#endif

// --------------------------------------------------------------------------
// --------------------------------------------------------------------------

int SYM_EXPORT WINAPI VirusList (       // Display the virus list
    LPVIRLIST   lpVir )                 // [in] initialization structure
{
    extern  HINSTANCE   hInstance;
    auto    int         nDatUpdated = FALSE;
    auto    VIRUSDBF    Vir = { 0 };

                                        // ----------------------------------
                                        // Make a copy for us so the data is
                                        // in our address space.
                                        // ----------------------------------
    SYM_ASSERT ( lpVir );
    MEMCPY ( &Vir.VirList, lpVir, sizeof(Vir.VirList) );

    if ( ( Vir.hContext = InitVirusLibrary ( Vir.VirList.szDatFile,
                                             Vir.VirList.szInfFile )) == NULL )
        {
        ErrorHandler ( Vir.VirList.hDlg, VERR_DATAFILES );
        }
    else
        {
        int nStatus = DialogBoxParam (
                                 hInstance,
                                 MAKEINTRESOURCE(IDD_VIRUSLIST),
                                 lpVir->hDlg,
                                 VirusListProc,
                                 (LPARAM)&Vir
                                 );

        SYM_ASSERT ( nStatus != -1 );
        nDatUpdated = nStatus;
        }

    return ( nDatUpdated );
}

int SYM_EXPORT WINAPI VirusListOnly (   // Display the virus list
    LPVIRLIST   lpVir )                 // [in] initialization structure
{
    extern  HINSTANCE   hInstance;
    auto    int         nDatUpdated = FALSE;
    auto    VIRUSDBF    Vir = { 0 };

                                        // ----------------------------------
                                        // Make a copy for us so the data is
                                        // in our address space.
                                        // ----------------------------------
    SYM_ASSERT ( lpVir );
    MEMCPY ( &Vir.VirList, lpVir, sizeof(Vir.VirList) );

    if ( ( Vir.hContext = InitVirusList ( Vir.VirList.szDatFile,
                                          Vir.VirList.szInfFile )) == NULL )
        {
        ErrorHandler ( Vir.VirList.hDlg, VERR_DATAFILES );
        }
    else
        {
        int nStatus = DialogBoxParam (
                                 hInstance,
                                 MAKEINTRESOURCE(IDD_VIRUSLIST),
                                 lpVir->hDlg,
                                 VirusListProc,
                                 (LPARAM)&Vir
                                 );

        SYM_ASSERT ( nStatus != -1 );
        nDatUpdated = nStatus;
        }

    return ( nDatUpdated );
}

// --------------------------------------------------------------------------
// --------------------------------------------------------------------------

BOOL SYM_EXPORT WINAPI VirusListProc (  // process messages for virus list
    HWND    hDlg,                       // [in] window handle
    UINT    uMessage,                   // [in] message to process
    WPARAM  wParam,                     // [in] wparam
    LPARAM  lParam )                    // [in] lparam
{
    auto    LPVLBSTRUCT   lpvlbInStruct;
    auto    LPVIRUSDBF    lpVir;

    switch ( uMessage )
        {
#ifdef SYM_WIN32
        case WM_HELP:
        case WM_CONTEXTMENU:
            OnContextMenu ( hDlg, uMessage, wParam, lParam );
            break;
#endif
        case WM_INITDIALOG:
            DoInitDialog (hDlg, lParam);
            return (TRUE);

        case WM_ACTIVATE:
            if ( GET_WM_ACTIVATE_STATE ( wParam, lParam ) )
                {
                lpVir = (LPVIRUSDBF)GetWindowLong (hDlg, DWL_USER );
                SYM_ASSERT ( lpVir );

                StdHelpSetContext ( lpVir->VirList.uHelpVirusList,
                                    lpVir->VirList.uHelpVirusList );
                }
            break;

        case WM_COMMAND:
            HANDLE_WM_COMMAND ( hDlg, wParam, lParam, OnCommand );
            break;

        case WM_DESTROY:
            DoOnDestroy ( hDlg );
            break;

                                        // Start of VLIST support
        case VLB_PREV:
            lpVir = (LPVIRUSDBF)GetWindowLong (hDlg, DWL_USER );
            SYM_ASSERT ( lpVir );
            lpvlbInStruct = (LPVLBSTRUCT)lParam;

            if ( lpvlbInStruct->lIndex > 0 )
                {
                lpvlbInStruct->nStatus = VLB_OK;
                lpvlbInStruct->lIndex--;
                lpvlbInStruct->lData = lpvlbInStruct->lIndex;
                GetListBoxString(lpVir, (UINT) lpvlbInStruct->lIndex, szWork);
                lpvlbInStruct->lpTextPointer = szWork;
                return TRUE;
                }
            else
                {
                lpvlbInStruct->nStatus = VLB_ENDOFFILE;
                return TRUE;
                }
            break;

        case VLB_FINDPOS:
            lpVir = (LPVIRUSDBF)GetWindowLong (hDlg, DWL_USER );
            SYM_ASSERT ( lpVir );
            lpvlbInStruct = (LPVLBSTRUCT)lParam;

            if ( lpvlbInStruct->lIndex == 0L )
                {
                goto First;
                }
            else if ( lpvlbInStruct->lIndex == 100L )
                {
                goto Last;
                }
            else
                {
                lpvlbInStruct->lIndex = (lpvlbInStruct->lData * lpVir->ListboxArray[lpVir->uActiveEntry].uNumEntries) / 100;
                lpvlbInStruct->nStatus = VLB_OK;
                GetListBoxString(lpVir, (UINT) lpvlbInStruct->lIndex, szWork);
                lpvlbInStruct->lpTextPointer = szWork;
                return TRUE;
                }

            break;

        case VLB_FINDITEM:
            lpVir = (LPVIRUSDBF)GetWindowLong (hDlg, DWL_USER );
            SYM_ASSERT ( lpVir );
            lpvlbInStruct = (LPVLBSTRUCT)lParam;

            lpvlbInStruct->lIndex = lpvlbInStruct->lData;
            lpvlbInStruct->nStatus = VLB_OK;
            GetListBoxString(lpVir, (UINT) lpvlbInStruct->lIndex, szWork);
            lpvlbInStruct->lpTextPointer = szWork;
            return TRUE;

            break;

//        case VLB_FINDSTRING:
//        case VLB_FINDSTRINGEXACT:
//        case VLB_SELECTSTRING:
//        case VLBR_FINDSTRING:
//        case VLBR_FINDSTRINGEXACT:
//        case VLBR_SELECTSTRING:
//            {
//            lpvlbInStruct = (LPVLBSTRUCT)lParam;
//
//            _fstrcpy(szText,lpvlbInStruct->lpFindString);
//            lpvlbInStruct->lIndex = atol(szText);
//            sprintf(szText,"Item\t%ld\tSome Text to Scroll\tMMMMMMMMMMMMMMMMMMMMM",lpvlbInStruct->lIndex);
//            lpvlbInStruct->lpTextPointer = szText;
//            lpvlbInStruct->lData = lpvlbInStruct->lIndex;
//            lpvlbInStruct->nStatus = VLB_OK;
//            return TRUE;
//            }
//            break;

        case VLB_RANGE:
            lpVir = (LPVIRUSDBF)GetWindowLong (hDlg, DWL_USER );
            SYM_ASSERT ( lpVir );

            lpvlbInStruct = (LPVLBSTRUCT)lParam;
            lpvlbInStruct->lIndex = (long) lpVir->ListboxArray[lpVir->uActiveEntry].uNumEntries;
            lpvlbInStruct->nStatus = VLB_OK;
            return TRUE;

            break;

        case VLB_NEXT:
            lpVir = (LPVIRUSDBF)GetWindowLong (hDlg, DWL_USER );
            SYM_ASSERT ( lpVir );

            lpvlbInStruct = (LPVLBSTRUCT)lParam;

            if ( lpvlbInStruct->lIndex < (long) lpVir->ListboxArray[lpVir->uActiveEntry].uNumEntries - 1)
                {
                lpvlbInStruct->nStatus = VLB_OK;
                lpvlbInStruct->lIndex++;
                lpvlbInStruct->lData = lpvlbInStruct->lIndex;
                GetListBoxString(lpVir, (UINT) lpvlbInStruct->lIndex, szWork);
                lpvlbInStruct->lpTextPointer = szWork;
                return TRUE;
                }
             else
                {
                lpvlbInStruct->nStatus = VLB_ENDOFFILE;
                return TRUE;
                }
             break;

        case VLB_FIRST:
First:
            lpVir = (LPVIRUSDBF)GetWindowLong (hDlg, DWL_USER );
            SYM_ASSERT ( lpVir );

            lpvlbInStruct = (LPVLBSTRUCT)lParam;

            lpvlbInStruct->nStatus = VLB_OK;
            lpvlbInStruct->lIndex = 0L;
            GetListBoxString(lpVir, (UINT) lpvlbInStruct->lIndex, szWork);
            lpvlbInStruct->lpTextPointer = szWork;
            lpvlbInStruct->lData = lpvlbInStruct->lIndex;
            return TRUE;

        case VLB_LAST:
Last:
            lpVir = (LPVIRUSDBF)GetWindowLong (hDlg, DWL_USER );
            SYM_ASSERT ( lpVir );

            lpvlbInStruct = (LPVLBSTRUCT)lParam;

            lpvlbInStruct->nStatus = VLB_OK;
            lpvlbInStruct->lIndex = lpVir->ListboxArray[lpVir->uActiveEntry].uNumEntries - 1;
            GetListBoxString(lpVir, (UINT) lpvlbInStruct->lIndex, szWork);
            lpvlbInStruct->lpTextPointer = szWork;
            lpvlbInStruct->lData = lpvlbInStruct->lIndex;
            return TRUE;

            break;

         case VLB_GETITEMDATA:
         case VLBR_GETITEMDATA:
             {
             LPHVLVIRUSENTRY lphEntry;
             UINT       uIndex;

                                         // This info will create the item
                                         // data on the fly.
             lpVir = (LPVIRUSDBF)GetWindowLong (hDlg, DWL_USER );
             SYM_ASSERT ( lpVir );
             lpvlbInStruct = (LPVLBSTRUCT)lParam;

             lphEntry = lpVir->lphVirusArray;
             uIndex = lpVir->ListboxArray[lpVir->uActiveEntry].lpuListBoxIndex[lpvlbInStruct->lIndex];

             lpvlbInStruct->lData = (long) (LPVOID) lphEntry[uIndex];
             lpvlbInStruct->nStatus = VLB_OK;
             return TRUE;
             }

             break;

        case VLB_GETTEXT:
        case VLBR_GETTEXT:
            lpVir = (LPVIRUSDBF)GetWindowLong (hDlg, DWL_USER );
            SYM_ASSERT ( lpVir );
            lpvlbInStruct = (LPVLBSTRUCT)lParam;

            lpvlbInStruct->nStatus = VLB_OK;
            lpvlbInStruct->lData = lpvlbInStruct->lIndex;
            GetListBoxString(lpVir, (UINT) lpvlbInStruct->lIndex, szWork);
            lpvlbInStruct->lpTextPointer = szWork;
            return TRUE;

            break;

        case VLB_GETTEXTLEN:
        case VLBR_GETTEXTLEN:
            lpVir = (LPVIRUSDBF)GetWindowLong (hDlg, DWL_USER );
            SYM_ASSERT ( lpVir );
            lpvlbInStruct = (LPVLBSTRUCT)lParam;

            lpvlbInStruct->nStatus = VLB_OK;
            lpvlbInStruct->lData = lpvlbInStruct->lIndex;
            GetListBoxString(lpVir, (UINT) lpvlbInStruct->lIndex, szWork);
            return STRLEN(szWork);
            break;
        }

    return (FALSE);
}

// ******************************************************************** //
// ******************************************************************** //

VOID LOCAL DoPrint (                    // Process printing from
    HWND        hDlg,                   // [in] this dialog box
    LPVIRUSDBF  lpVir )                 // [in] virus database pointer
{
    auto    PRINTDLGSTRUCT  printDlgStruct = {0};
    auto    TCHAR           szTitle [ SYM_MAX_PATH+1 ];
    auto    TCHAR           szFile  [ SYM_MAX_PATH+1 ];
    auto    TCHAR           szPath  [ SYM_MAX_PATH+1 ];

    SYM_ASSERT ( hDlg );
    SYM_ASSERT ( lpVir );
                                        // ---------------------------------
                                        // Build a full path to the default
                                        // print file.
                                        // ---------------------------------
    LoadString (
                hInstance,
                IDS_DEF_VIRUSFILE,
                szTitle, sizeof(szTitle)
                );

    GetStartDir (hInstance, szPath, sizeof(szPath));
    NameReturnLongName (szPath, szFile ,TRUE);
    NameAppendFile( szFile, szTitle );
    STRLWR( szFile );

    LoadString (hInstance, IDS_PRINTVIRUSLST,
                szTitle, sizeof(szTitle) );

    STRCPY ( printDlgStruct.szTitle, szTitle );
    STRCPY ( printDlgStruct.szFile,  szFile  );

    printDlgStruct.dwSize           = sizeof(printDlgStruct);
    printDlgStruct.hParent          = hDlg;
    printDlgStruct.lpfnBuild        = BuildVirusListForPrinting;
    printDlgStruct.wHelpID          = lpVir->VirList.uHelpPrint;
    printDlgStruct.wHelpIDOverwrite = lpVir->VirList.uHelpPrintOverwrite;
    printDlgStruct.wHelpIDBrowse    = lpVir->VirList.uHelpPrintBrowse;

    printDlgStruct.uHelpRightClickPrintFileButton =
            lpVir->VirList.uHelpPrintRightClickPrintFileButton;

    printDlgStruct.uHelpRightClickPrintPrinterButton =
            lpVir->VirList.uHelpPrintRightClickPrintPrinterButton;

    printDlgStruct.uHelpRightClickSendUserButton =
            lpVir->VirList.uHelpPrintRightClickSendUserButton;

    printDlgStruct.uHelpRightClickPrintFileTunnel =
            lpVir->VirList.uHelpPrintRightClickPrintFileTunnel;

    printDlgStruct.uHelpRightClickPrintPrinterTunnel =
            lpVir->VirList.uHelpPrintRightClickPrintPrinterTunnel;

            printDlgStruct.lpdwHelpHowToPrintArray = NULL;

                                        // -------------------------------
                                        // See how user wants the output
                                        // directed and do so.
                                        // -------------------------------
    PRINTERR Status = NPTPrintDialog (&printDlgStruct);

    if ( Status != PRINTERR_OK && Status != PRINTERR_CANCEL )
        {
        LoadString (hInstance, IDS_NAVDBF,     szTitle, sizeof(szTitle) );
        LoadString (hInstance, IDS_PRINTNOTOK, szFile,  sizeof(szFile) );

        MessageBox ( hDlg, szFile, szTitle, MB_OK |
                     MB_ICONINFORMATION );
        }
}

// --------------------------------------------------------------------------
// This function is extracted from LUIGI\NAVXUTIL\winmisc.cpp
// --------------------------------------------------------------------------

VOID LOCAL GetStartDir (HINSTANCE hInstance, LPSTR lpPath, UINT uSize)
{
#ifndef SYM_DOS
    auto    char    szPath [ SYM_MAX_PATH + 1 ];

    GetModuleFileName ( hInstance, szPath, sizeof(szPath) );
    NameStripFile ( szPath );

    STATUS ShortNameReturnValue = NameReturnShortName ( szPath, lpPath );

    if  (
        (ShortNameReturnValue != NOERR) &&
        (SystemGetWindowsType() == SYM_SYSTEM_WIN_NT)
        )
        {
                                        // If we are running NT, and it
                                        // can't convert the LFN to a
                                        // SFN, return the LFN.
        STRCPY ( lpPath , szPath );
        }
#else
    if (EOS != szOptionsDir[0])
        {
        STRCPY(lpPath, szOptionsDir);
        }
    else
        {
        DOSGetProgramName(lpPath);
        NameStripFile(lpPath);
        }
#endif
}

//************************************************************************
// GetTabStop()
//
// This routine returns the tab stop for the virus listbox. Note
// that there is a different tab stop for Japanese, Korean, Chinese, and
// English. This routine also accounts for Small / Large font modes.
//
// Returns
//   int           [out] position for the tab stop
//************************************************************************
// 10/24/96 JBRENNA - created.
// 11/01/96 JBRENNA - Use CalcTabStop() instead of hard coding the postions.
//************************************************************************
static int GetTabStop (const HWND hDlg)
{
    auto    HWND  hInfects = GetDlgItem (hDlg, IDI_VL_INFTXT);
    auto    int   nTabStop;

    CalcTabStops (GetDlgItem (hDlg, IDI_VL_LISTBOX), &hInfects, &nTabStop, 1);

    return nTabStop;
}

// --------------------------------------------------------------------------
// Fill the VIRUSTYPE_ALL listbox. Set the virus count for this listbox
// to be the "actual" count from the .DAT file, not the number of defs
// shown in the listbox. The rest of the lists will display the count
// of the defs shown in each listbox.
// --------------------------------------------------------------------------



VOID LOCAL DoInitDialog (               // Process WM_INITDIALOG message
    HWND    hDlg,                       // [in] from this window
    LPARAM  lParam )                    // [in] user param
{
    static    WORD      wTabStop;       // This MUST be static for NT 3.51!!!
    auto      UINT      uIndex;
	auto BOOL bRet ;
	auto  TCHAR  szTitle [ SYM_MAX_PATH+1 ];
	auto  TCHAR szFile  [ SYM_MAX_PATH+1 ];

	

	LPHELPSTRUCT  lpHelpStruct =(LPHELPSTRUCT)MemAllocPtr ( GHND,sizeof ( HELPSTRUCT ) ) ;                                   

	bRet = InitGlobals() ;
	if (!bRet )
	{
		LoadString (hInstance, IDS_NAV,     szTitle, sizeof(szTitle) );
        LoadString (hInstance, IDS_CANT_GET_NAV_DIR, szFile,  sizeof(szFile) );
        MessageBox ( hDlg, szFile, szTitle, MB_ICONSTOP );
    }		 

	NG_SetFontsInDialog (hDlg, 0);

#ifdef SYM_WIN16
    Ctl3dSubclassDlgEx ( hDlg, CTL3D_ALL );
#endif
    NG_CenterDialog ( hDlg, GetParent(hDlg) );

    NG_SetChildSmallFont ( hDlg, IDI_VL_COMBOX );
    NG_SetChildSmallFont ( hDlg, IDI_VL_LISTBOX );
    NG_SetChildSmallFont ( hDlg, IDI_VL_TEXT1 );
    NG_SetChildSmallFont ( hDlg, IDI_VL_TEXT2 );

	//Disable the FindNext button for the first time
	Button_Enable (GetDlgItem(hDlg, IDI_VL_SEARCHAGAIN), FALSE);

    LPVIRUSDBF lpVir = (LPVIRUSDBF)lParam;
    SYM_ASSERT ( lpVir );

    SetWindowLong ( hDlg, DWL_USER, (LONG)lpVir );

    if ( *lpVir->VirList.szOKButtonTitle != EOS )
        {
        SetWindowText ( GetDlgItem(hDlg, IDOK),
                        lpVir->VirList.szOKButtonTitle );
        }

    lpVir->lpDeleted       = new CStackObjects(sizeof(HVLVIRUSENTRY));
    lpVir->uActiveEntry    = VIRUSTYPE_ALL;

    lpVir->ListboxArray[VIRUSTYPE_ALL].hListbox =
                                        GetDlgItem(hDlg, IDI_VL_LISTBOX);

                                        // This must be initialize due to
                                        // Close Messages being posted in
                                        // this routine.
    for ( UINT nIndex = 0; nIndex < NUM_LISTBOX; nIndex++)
        lpVir->ListboxArray[nIndex].lpuListBoxIndex = NULL;

                                        // --------------------------------
                                        // Clear all handles to list boxes
                                        // --------------------------------

    for ( nIndex = 1; nIndex < NUM_LISTBOX; nIndex++ )
        lpVir->ListboxArray[nIndex].hListbox = NULL;

    wTabStop = GetTabStop(hDlg);
    VListBox_SetTabStops ( ACTIVELISTBOX(lpVir), 1, &wTabStop );

    NG_AddSpeedSearch ( ACTIVELISTBOX(lpVir), 0, NULL, 0 );

    HWND hComBox = GetDlgItem(hDlg, IDI_VL_COMBOX);
    SYM_ASSERT ( hComBox );
    TCHAR szMessage [ SYM_MAX_PATH ];
                                        // --------------------------------
                                        // Fill the combobox options.
                                        // --------------------------------
    for ( uIndex = 0; uCombo[ uIndex ]; uIndex++ )
        {
        if ( (uCombo[uIndex] == IDS_MACVIRUSES) && lpVir->VirList.bMacViruses == FALSE )
            {
            continue;
            }

        LoadString ( hInstance, uCombo[uIndex], szMessage, sizeof(szMessage) );
        ComboBox_AddString ( hComBox, szMessage );
        }

    ComboBox_SetCurSel ( hComBox, 0 );

    if ( GetVirusHandles ( lpVir ) == TRUE )
        {

        DisplayNumDefs (hDlg,
                        lpVir->uTotalDetected);

//        LoadString (hInstance, IDS_VIRUSDEF, szMessage, sizeof(szMessage) );
//        SPRINTF ( szWork, szMessage, ACTIVELISTBOXENTRIES(lpVir) );
//        Static_SetText (GetDlgItem(hDlg, IDI_VL_TEXT1), szWork);

        SetFocus ( ACTIVELISTBOX(lpVir) );

                                    // --------------------------------
                                    // Display definitions date.
                                    // --------------------------------
        DisplayDefinitionDate ( hDlg, lpVir );

                                    // --------------------------------
                                    // Fix buttons according to settings
                                    // --------------------------------
        FixButtons ( hDlg, lpVir );
                        // Initialize the list box
        SendMessage(ACTIVELISTBOX(lpVir), VLB_INITIALIZE, 0, 0L);

        VListBox_SetCurSel(ACTIVELISTBOX(lpVir), 0);

        SetFocus(ACTIVELISTBOX(lpVir));

		//Initialize the help structure with the help file name 
        wsprintf ( lpHelpStruct->helpFileName, _T("%s\\%s"),
                   g_sNAVDir,
                   _T("navw32.hlp") );

		lpHelpStruct->hinst= hInstance ;
		lpHelpStruct->hwnd=hDlg ;
        
		StdHelpInit(lpHelpStruct) ;
		StdHelpActivate(hDlg, lpHelpStruct) ;
	    
        }
    else
        EndDialog ( hDlg, 0 );
}

// --------------------------------------------------------------------------
// --------------------------------------------------------------------------

VOID WINAPI DoOnDestroy (               // Handle the WM_DESTROY message
    HWND    hDlg )                      // [in] from this window
{
    auto   WORD   wIndex;

    LPVIRUSDBF lpVir = (LPVIRUSDBF)GetWindowLong ( hDlg, DWL_USER );

    if ( lpVir )
        {
        if ( lpVir->lphVirusArray )
            {
            FreeVirusHandles ( lpVir );
            MemFreePtr ( lpVir->lphVirusArray );
            }

        for (wIndex = 0; wIndex < NUM_LISTBOX; wIndex++)
            if (lpVir->ListboxArray[wIndex].lpuListBoxIndex !=  NULL)
                {
                MemFreePtr (lpVir->ListboxArray[wIndex].lpuListBoxIndex);
                lpVir->ListboxArray[wIndex].lpuListBoxIndex = NULL;
                }


        delete ( lpVir->lpDeleted );
        UnloadVirusLibrary ( lpVir->hContext );
        }
}

// --------------------------------------------------------------------------
// --------------------------------------------------------------------------

VOID WINAPI DisplayDefinitionDate (     // Show the date of the definition
    HWND        hDlg,                   // [in] parent window handle
    LPVIRUSDBF  lpVir )                 // [in] virus db ptr
{
    auto    DWORD       dwYear, dwMonth, dwDay;
    auto    TCHAR       szDate [ SYM_MAX_PATH ];
    auto    TCHAR       szWork [ SYM_MAX_PATH ];
    auto    TCHAR       szMess [ SYM_MAX_PATH ];

    VLGetDBInfo ( lpVir->hContext, DB_MONTH_STAMP, szDate );
    ConvertStringToLong ( &dwMonth, szDate );

    VLGetDBInfo ( lpVir->hContext, DB_YEAR_STAMP, szDate );
    ConvertStringToLong ( &dwYear, szDate );

    VLGetDBInfo ( lpVir->hContext, DB_DAY_STAMP, szDate );
    ConvertStringToLong ( &dwDay, szDate );

#ifdef SYM_WIN32
    SYSTEMTIME st;
    ZeroMemory( &st, sizeof(SYSTEMTIME) );
    st.wYear = (WORD) dwYear;
    st.wMonth = (WORD) dwMonth;
    st.wDay = (WORD) dwDay;

    GetDateFormat( LOCALE_SYSTEM_DEFAULT,
                   DATE_SHORTDATE,
                   &st,
                   NULL,
                   szDate,
                   SYM_MAX_PATH );
#else
    NSetIntlDate ( szDate, (UINT)dwMonth, (UINT)dwDay, (UINT)dwYear );
#endif


    LoadString ( hInstance, IDS_NEWESTDEF, szWork, sizeof(szWork) );
    SPRINTF (szMess, szWork, (LPTSTR)szDate);

    Static_SetText(GetDlgItem(hDlg, IDI_VL_TEXT2), szMess );
}

// --------------------------------------------------------------------------
// --------------------------------------------------------------------------

VOID LOCAL OnCommand (                  // Process the WM_COMMAND messages
    HWND    hDlg,                       // [in] from this dialog box
    int     nID,                        // [in] and this control ID
    HWND    hwndCtl,                    // [in] control window handle
    UINT    codeNotify )                // [in] control notification code
{
    LPVIRUSDBF lpVir = (LPVIRUSDBF)GetWindowLong ( hDlg, DWL_USER );
    SYM_ASSERT ( lpVir );

    switch ( nID )
        {
        case IDI_VL_COMBOX:
            {
            if ( codeNotify == CBN_SELENDOK )
                {
                HWND hComBox = GetDlgItem(hDlg, IDI_VL_COMBOX);
                int  nIndex  = ComboBox_GetCurSel(hComBox);

                if ( nIndex != CB_ERR && (UINT)nIndex != lpVir->uActiveEntry )
                    {
                    lpVir->uActiveEntry = nIndex;

                                        // Initialize the list box
                    SendMessage(ACTIVELISTBOX(lpVir), VLB_INITIALIZE, 0, 0L);
                    VListBox_SetCurSel ( ACTIVELISTBOX(lpVir), 0 );
                    DisplayNumDefs ( hDlg,
                                     (lpVir->uActiveEntry == VIRUSTYPE_ALL) ?
                                      lpVir->uTotalDetected :
                                     ACTIVELISTBOXENTRIES(lpVir) );
                    }
                }
            }
            break;

        case IDI_VL_INFO:
            {
            int nIndex = VListBox_GetCurSel(ACTIVELISTBOX(lpVir));

            if ( nIndex != LB_ERR )
                {
                VIRINFO Vir = {0};

                Vir.hParent             = hDlg;
                Vir.hListBox            = ACTIVELISTBOX(lpVir);
                Vir.uHelpID             = lpVir->VirList.uHelpVirusInfo;
                Vir.uHelpPrint          = lpVir->VirList.uHelpPrint;
                Vir.uHelpPrintOverwrite = lpVir->VirList.uHelpPrintOverwrite;
                Vir.uHelpPrintBrowse    = lpVir->VirList.uHelpPrintBrowse;

                Vir.uHelpPrintRightClickPrintFileButton =
                        lpVir->VirList.uHelpPrintRightClickPrintFileButton;

                Vir.uHelpPrintRightClickPrintPrinterButton =
                        lpVir->VirList.uHelpPrintRightClickPrintPrinterButton;

                Vir.uHelpPrintRightClickSendUserButton =
                    lpVir->VirList.uHelpPrintRightClickSendUserButton;

                Vir.uHelpPrintRightClickPrintFileTunnel =
                    lpVir->VirList.uHelpPrintRightClickPrintFileTunnel;

                Vir.uHelpPrintRightClickPrintPrinterTunnel =
                    lpVir->VirList.uHelpPrintRightClickPrintPrinterTunnel;

                    Vir.lpdwHelpHowToArray = NULL;

                    Vir.lpdwHelpHowToPrintArray = NULL;

                VirusInfo ( &Vir );
                }
            }
            break;

        case IDOK:
            {
            BOOL bUpdated = FALSE;

            if ( lpVir->lphVirusArray )
                {
                if ( lpVir->lpDeleted->GetCount() )
                    {
                    HCURSOR     hCursor ;
                    HVLVIRUSENTRY hEntry = 0;

                    bUpdated = TRUE;

                    hCursor = SetCursor(LoadCursor(NULL, IDC_WAIT));

                    while ( lpVir->lpDeleted->Pop((LPTSTR)&hEntry) == TRUE )
                        {
                        SYM_ASSERT ( hEntry );

                       // VSTATUS Status = VLDeleteEntry ( hEntry );

                       // SYM_ASSERT ( Status == VS_OK );
                        }
                    if ( hCursor )
                        SetCursor ( hCursor );
                    }
                }

            if ( *lpVir->VirList.szOKButtonTitle != EOS )
                {
                EndDialog ( hDlg,
                            bUpdated ? VIR_OK_CHANGED : VIR_OK_UNCHANGED );
                }
            else
                {
                EndDialog ( hDlg, bUpdated );
                }
            }
            break;

        case IDCANCEL:
            {
            EndDialog(hDlg, 0);
            }
            break;


        case IDI_VL_PRINT:
			{
				auto TCHAR szLogFile [SYM_MAX_PATH+1] ;
	           	auto BOOL bRetVal ;

                wsprintf ( szLogFile, _T("%s\\%s"),
                           g_sNAVDir, _T("activity.log") );

                hinstNavalog = LoadLibrary ( _T("NAVActLg.dll") );
				

				if ( NULL == hinstNavalog )
				{
					
					goto bailout;
				}

				 lpfnTempActivityLog = (LPFN_TEMPACTIVITYLOG)(
				GetProcAddress ( hinstNavalog, _T("_TempActivityLog@8")) );

				if ( NULL == lpfnTempActivityLog )
				{
				goto bailout;
				 }

    // TempActivityLog() returns 0 on success.
    bRetVal = ( 0 == lpfnTempActivityLog ( hDlg, szLogFile ) ) ;

bailout:
    
    if ( NULL != hinstNavalog )
        FreeLibrary ( hinstNavalog );

    
				DoPrint ( hDlg, lpVir );
			}  
            break;

        // Displays  virus search dialog 
        case IDI_VL_SEARCH:
			{
					int nIndex = VListBox_GetCurSel(ACTIVELISTBOX(lpVir));

					if ( nIndex != LB_ERR )
					{
						VIRSEARCH Vir = {0};

						Vir.hParent             = hDlg;
						Vir.hListBox            = ACTIVELISTBOX(lpVir);
						VirusSearch( &Vir ) ;
                    }
			}
			break ;		
        
        // does the virus search again 
        case IDI_VL_SEARCHAGAIN:
		{
                BOOL bRes ;
				auto    TCHAR   szMsg [ SYM_MAX_PATH ];
				auto    TCHAR   szTitle[ SYM_MAX_PATH ] ;
				HCURSOR hCursor  ;
				hCursor = SetCursor(LoadCursor(NULL, IDC_WAIT)) ;
				bRes = DoSearchAgain( hDlg, lpVir ) ;
				if( ! bRes ) 
				{
					if( hCursor ) 
						SetCursor(hCursor) ;
					LoadString ( hInstance, IDS_TITLE1, szTitle, sizeof(szTitle) );
					LoadString ( hInstance, IDS_MESSAGE, szMsg, sizeof(szMsg) );  
					MessageBox( hDlg, szMsg,szTitle,MB_OK|MB_ICONINFORMATION ) ;  
				}
				
			}  
			break;


        case IDI_VL_DELETE:
            DoDelete ( hDlg, lpVir );
            break;

        case IDI_VL_CUSTOM:
            {
            if ( codeNotify == BN_CLICKED )
                {
                                        // ----------------------------------
                                        // May the Lord be with us
                                        // ----------------------------------
                SYM_ASSERT ( lpVir->VirList.lpfnCustom );
                BOOL bDisable = (*lpVir->VirList.lpfnCustom)(
                                                    hDlg,
                                                    lpVir->VirList.dwUserData
                                                    );
                if ( bDisable == TRUE )
                    Button_Enable ( hwndCtl, FALSE );
                }
            }
            break;

        case IDHELP:
				
			StdHelpFromDialog (hDlg, lpVir->VirList.uHelpVirusList );
                   
            break;

        default:
            {
            if ( nID >= IDI_VL_LISTBOX && nID <= (IDI_VL_LISTBOX + NUM_LISTBOX) )
                {
                if ( codeNotify == LBN_DBLCLK &&
                     lpVir->VirList.bAllowInfo == TRUE )
                    {
                    FORWARD_WM_COMMAND ( hDlg, IDI_VL_INFO,
                                         GetDlgItem ( hDlg, IDI_VL_INFO ),
                                         BN_CLICKED, PostMessage );
                    }
                }
            }
            break;
        }
}

// --------------------------------------------------------------------------
// --------------------------------------------------------------------------

BOOL LOCAL ValidVirusType (             // Validate this virus entry
    HVLVIRUSENTRY hVirus,                 // [in] virus entry
    WORD        wTypeRequested,         // [in] type requested
    BOOL        bMacViruses )           // [in] TRUE to consider Mac viruses
{
    auto    BOOL    bValid = FALSE;
    auto    TCHAR   szWork [ SYM_MAX_PATH ];

    if ( wTypeRequested == VIRUSTYPE_ALL )
        bValid = TRUE;
    else if ( wTypeRequested == VIRUSTYPE_MULTIPARTY )
        bValid = VLDefInfo ( hVirus, VI_BOOL_MULTIPART, szWork );
    else if ( wTypeRequested == VIRUSTYPE_POLY )
        bValid = VLDefInfo ( hVirus, VI_BOOL_POLYMORPHIC, szWork );
    else if ( wTypeRequested == VIRUSTYPE_MACRO )
        bValid = VLDefInfo ( hVirus, VI_BOOL_MACRO_VIRUS, szWork );
    else if ( wTypeRequested == VIRUSTYPE_WINDOWS )
        bValid = VLDefInfo ( hVirus, VI_BOOL_WINDOWS_VIRUS, szWork );
    else if ( wTypeRequested == VIRUSTYPE_AGENT )
        bValid = VLDefInfo ( hVirus, VI_BOOL_AGENT_VIRUS, szWork );
    else if ( wTypeRequested == VIRUSTYPE_STEALTH )
        {
        if ( VLDefInfo ( hVirus, VI_BOOL_SIZE_STEALTH, szWork ) ||
             VLDefInfo ( hVirus, VI_BOOL_FULL_STEALTH, szWork ) )
            {
            bValid = TRUE;
            }
        }
    else if ( wTypeRequested == VIRUSTYPE_FILE )
        {
        if ( VLDefInfo ( hVirus, VI_BOOL_INFECT_FILES, szWork ) &&
             !VLDefInfo ( hVirus, VI_BOOL_MACRO_VIRUS, szWork )   )
            {
            bValid = TRUE;
            }
        }
    else if ( wTypeRequested == VIRUSTYPE_BOOT )
        {
        if ( VLDefInfo ( hVirus, VI_BOOL_INFECT_MBR, szWork ) ||
             VLDefInfo ( hVirus, VI_BOOL_INFECT_FLOPPY_BOOT, szWork ) ||
             VLDefInfo ( hVirus, VI_BOOL_INFECT_HD_BOOT, szWork ) )
            {
            bValid = TRUE;
            }
        }
    else if ( wTypeRequested == VIRUSTYPE_COMMON )
        {
        bValid = VLDefInfo ( hVirus, VI_BOOL_INFECT_WILD, szWork );
        }
    else if ( wTypeRequested == VIRUSTYPE_MAC )
        {
        bValid = VLDefInfo ( hVirus, VI_BOOL_MAC_VIRUS, szWork );
        }
                                        // ----------------------------------
                                        // Check if mac viruses should be
                                        // displayed.
                                        // ----------------------------------
    if ( bValid == TRUE && wTypeRequested != VIRUSTYPE_MAC &&
         VLDefInfo ( hVirus, VI_BOOL_MAC_VIRUS, szWork ) )
        {
        bValid = bMacViruses;
        }

    return ( bValid );
}

// --------------------------------------------------------------------------
// --------------------------------------------------------------------------

WORD LOCAL GetVirusInfects (            // Validate this virus entry
    HVLVIRUSENTRY hVirus )                // [in] virus entry
{
    auto    WORD    wInfects = 0;
    auto    TCHAR   szWork [ SYM_MAX_PATH ];

    if ( VLDefInfo ( hVirus, VI_BOOL_INFECT_FILES, szWork ) )
        {
        wInfects = INFECTS_FILE;
        }

    if ( VLDefInfo ( hVirus, VI_BOOL_INFECT_MBR, szWork ) ||
         VLDefInfo ( hVirus, VI_BOOL_INFECT_FLOPPY_BOOT, szWork ) ||
         VLDefInfo ( hVirus, VI_BOOL_INFECT_HD_BOOT, szWork ) )
        {
        if ( wInfects == INFECTS_FILE )
            wInfects = INFECTS_FILEANDBOOT;
        else
            wInfects = INFECTS_BOOT;
        }

    SYM_ASSERT ( wInfects );

    return ( wInfects );
}

// --------------------------------------------------------------------------
// --------------------------------------------------------------------------

BOOL WINAPI GetVirusHandles (           // Get all the virus handles
    LPVIRUSDBF  lpVir )                 // [in] pointer to virus list
{
    auto    BOOL    bSuccess = FALSE;
    auto    BOOL    bIndexAllocError;
    auto    WORD    wIndex;
    auto    DWORD   dwIndexSize;
    const   int     nSafety  = 2;
    auto    TCHAR   szTemp [ SYM_MAX_PATH ];
    auto    DWORD   dwTemp;             // Takes the converted return value

    SYM_ASSERT ( lpVir );
    SYM_ASSERT ( lpVir->hContext );

    UINT uNumEntries = VLNumVirusEntries ( lpVir->hContext ) + nSafety;
    SYM_ASSERT ( uNumEntries );

                                        // Use AVAPI to get total number of
                                        // viruses detected.
    VLGetDBInfo( lpVir->hContext, DB_TOTAL_VIRUS_COUNT, szTemp);
    _ConvertStringToLong( &dwTemp, szTemp, 10, FALSE);
    lpVir->uTotalDetected = (UINT) dwTemp;

    lpVir->lphVirusArray = (LPHVLVIRUSENTRY)MemAllocPtr (
                                            GHND,
                                            sizeof ( HVLVIRUSENTRY ) * uNumEntries
                                            );

    if ( lpVir-> lphVirusArray )
        {
        UINT uEntriesLoaded = VLFillListTable (
                                            lpVir->hContext,
                                            lpVir->lphVirusArray
                                            );

                                        // ----------------------------------
                                        // The number of entries read in mem
                                        // MUST be the same number as what
                                        // reported by VLNumVirusEntries()
                                        // ----------------------------------

        if ( uEntriesLoaded )
            {


            dwIndexSize = (DWORD)sizeof(UINT) * uEntriesLoaded;

            bIndexAllocError = FALSE;
            for (wIndex = 0; wIndex < NUM_LISTBOX; wIndex++)   // Allocate all the lists
                if ( (lpVir->ListboxArray[wIndex].lpuListBoxIndex = (LPUINT) MemAllocPtr(GHND, dwIndexSize)) == NULL)
                    bIndexAllocError = TRUE;

            if (bIndexAllocError)               // If any fail, free the rest.
                {
                for (wIndex = 0; wIndex < NUM_LISTBOX; wIndex++)
                    if (lpVir->ListboxArray[wIndex].lpuListBoxIndex !=  NULL)
                        {
                        MemFreePtr (lpVir->ListboxArray[wIndex].lpuListBoxIndex);
                        lpVir->ListboxArray[wIndex].lpuListBoxIndex = NULL;
                        }
                bSuccess = FALSE;               // And fail the load.

                if ( lpVir->lphVirusArray != NULL)
                    {
                    MemFreePtr ( lpVir->lphVirusArray ); // Cleaning house.
                    lpVir->lphVirusArray = NULL;
                    }
                }
            else
                {
                SortVirusList(lpVir->lphVirusArray, uEntriesLoaded);
                SetupIndexLists ( lpVir,
                                  lpVir->lphVirusArray,
                                  lpVir->VirList.bMacViruses );

                bSuccess = TRUE;
                }
            }
        else
            {
            FreeVirusHandles ( lpVir );
            MemFreePtr ( lpVir->lphVirusArray );
            lpVir->lphVirusArray = NULL;
            }
        }

    return ( bSuccess );
}

// --------------------------------------------------------------------------
// --------------------------------------------------------------------------

VOID LOCAL FreeVirusHandles (           // Free all the virus handles
    LPVIRUSDBF  lpVir )                 // [in] pointer to virus list
{
    SYM_ASSERT ( lpVir );

    if ( lpVir->lphVirusArray )
        {
        for ( UINT i = 0; lpVir->lphVirusArray[i]; i++ )
            {
            VLReleaseEntry( lpVir->lphVirusArray[i] );
            }
        }
}

// --------------------------------------------------------------------------
// --------------------------------------------------------------------------

VOID WINAPI DisplayNumDefs (             // Display the number of defs in view
    HWND    hDlg,                       // [in] main dlg box handle
    UINT    uNumDefs )                  // [in] number ot display
{
    auto    TCHAR   szTemp [ SYM_MAX_PATH ];
    auto    TCHAR   szWork [ SYM_MAX_PATH ];

    LoadString ( hInstance, IDS_VIRUSDEF, szTemp, sizeof(szTemp) );

    SPRINTF ( szWork, szTemp, uNumDefs );

    Static_SetText ( GetDlgItem ( hDlg, IDI_VL_TEXT1 ), szWork );
}

// --------------------------------------------------------------------------
// --------------------------------------------------------------------------

HGLOBAL SYM_EXPORT WINAPI BuildVirusListForPrinting (
    LPPRINTDLGSTRUCT lpPrintDlgStruct)
{
    auto    HGLOBAL hBuffer = NULL;

    LPVIRUSDBF lpVir = (LPVIRUSDBF)GetWindowLong ( lpPrintDlgStruct->hParent,
                                                   DWL_USER );
    SYM_ASSERT ( lpVir );

    hBuffer = BuildVirusListInfo(
                        lpPrintDlgStruct->hParent,
                        ACTIVELISTBOX(lpVir)
                        );

    return ( hBuffer );
}

// --------------------------------------------------------------------------
// --------------------------------------------------------------------------

VOID LOCAL FixButtons (                 // Hide required buttons
    HWND        hDlg,                   // [in] window handle
    LPVIRUSDBF  lpVir )                 // [in] pointer
{
    struct tagBUTTONINFO
        {
        UINT    uID;
        RECT    Rect;
        } ButtonInfo [] =
            {
                {
                IDI_VL_INFO,
                {0}
                },

                {
                IDI_VL_PRINT,
                {0}
                },

                {
                IDI_VL_DELETE,
                {0}
                },

                /*{
                IDHELP,
                {0}
                },*/

                {
                0,
                {0}
                }
           };

    for ( UINT uIndex = 0; ButtonInfo[uIndex].uID; uIndex ++ )
        {
        GetWindowRect (GetDlgItem(hDlg, ButtonInfo[uIndex].uID),
                       &ButtonInfo[uIndex].Rect);
        }

    if ( !lpVir->VirList.bAllowInfo )
        {
        Button_Enable (GetDlgItem(hDlg, IDI_VL_INFO), FALSE);
        ShowWindow    (GetDlgItem(hDlg, IDI_VL_INFO), SW_HIDE);

        for ( uIndex = 0; ButtonInfo[uIndex + 1].uID; uIndex ++ )
            {
            POINT ptUpperLeft;
            POINT ptLowerRight;

            ptUpperLeft.x  = ButtonInfo[uIndex].Rect.left;
            ptUpperLeft.y  = ButtonInfo[uIndex].Rect.top;
            ptLowerRight.x = ButtonInfo[uIndex].Rect.right;
            ptLowerRight.y = ButtonInfo[uIndex].Rect.bottom;

            ScreenToClient (hDlg, &ptUpperLeft );
            ScreenToClient (hDlg, &ptLowerRight);

            MoveWindow ( GetDlgItem(hDlg, ButtonInfo[uIndex + 1].uID),
                         ptUpperLeft.x, ptUpperLeft.y,
                         ptLowerRight.x - ptUpperLeft.x,
                         ptLowerRight.y - ptUpperLeft.y,
                         TRUE);
            }

        for ( uIndex = 0; ButtonInfo[uIndex + 1].uID; uIndex ++ )
            {
            ButtonInfo[uIndex].uID = ButtonInfo[uIndex+1].uID;
            }

        ButtonInfo[uIndex].uID = 0;
        }

    if ( lpVir->VirList.bAllowDelete == FALSE )
        {
        Button_Enable (GetDlgItem(hDlg, IDI_VL_DELETE), FALSE);
        ShowWindow    (GetDlgItem(hDlg, IDI_VL_DELETE), SW_HIDE);

        for ( uIndex = 0; ButtonInfo[uIndex].uID &&
                          ButtonInfo[uIndex].uID != IDI_VL_DELETE; uIndex ++ )
            ;

        for ( ; ButtonInfo[uIndex + 1].uID; uIndex ++ )
            {
            POINT ptUpperLeft;
            POINT ptLowerRight;

            ptUpperLeft.x  = ButtonInfo[uIndex].Rect.left;
            ptUpperLeft.y  = ButtonInfo[uIndex].Rect.top;
            ptLowerRight.x = ButtonInfo[uIndex].Rect.right;
            ptLowerRight.y = ButtonInfo[uIndex].Rect.bottom;

            ScreenToClient (hDlg, &ptUpperLeft );
            ScreenToClient (hDlg, &ptLowerRight);

            MoveWindow ( GetDlgItem(hDlg, ButtonInfo[uIndex + 1].uID),
                         ptUpperLeft.x, ptUpperLeft.y,
                         ptLowerRight.x - ptUpperLeft.x,
                         ptLowerRight.y - ptUpperLeft.y,
                         TRUE);
            }

        for ( uIndex = 0; ButtonInfo[uIndex].uID &&
                          ButtonInfo[uIndex].uID != IDI_VL_DELETE; uIndex ++ )
            ;

        for ( ; ButtonInfo[uIndex + 1].uID; uIndex ++ )
            {
            ButtonInfo[uIndex].uID = ButtonInfo[uIndex+1].uID;
            }

        ButtonInfo[uIndex].uID = 0;
        }
                                        // -----------------------------------
                                        // Note that we're only displaying the
                                        // custom button if one of the other
                                        // buttons had to be deleted.
                                        // -----------------------------------

    if ( ( lpVir->VirList.bAllowInfo == FALSE ||
           lpVir->VirList.bAllowDelete == FALSE ) &&
           lpVir->VirList.lpfnCustom &&
           lpVir->VirList.szCustomButton[0] != EOS )
        {
        for ( uIndex = 0; ButtonInfo[uIndex].uID &&
                          ButtonInfo[uIndex].uID != IDHELP; uIndex ++ )
            ;

        if ( ButtonInfo[uIndex].uID == IDHELP )
            {
            POINT ptUpperLeft;
            POINT ptLowerRight;

            ptUpperLeft.x  = ButtonInfo[uIndex + 1].Rect.left;
            ptUpperLeft.y  = ButtonInfo[uIndex + 1].Rect.top;
            ptLowerRight.x = ButtonInfo[uIndex + 1].Rect.right;
            ptLowerRight.y = ButtonInfo[uIndex + 1].Rect.bottom;

            ScreenToClient (hDlg, &ptUpperLeft );
            ScreenToClient (hDlg, &ptLowerRight);

            MoveWindow ( GetDlgItem(hDlg, ButtonInfo[uIndex].uID),
                         ptUpperLeft.x, ptUpperLeft.y,
                         ptLowerRight.x - ptUpperLeft.x,
                         ptLowerRight.y - ptUpperLeft.y,
                         TRUE);

            ptUpperLeft.x  = ButtonInfo[uIndex].Rect.left;
            ptUpperLeft.y  = ButtonInfo[uIndex].Rect.top;
            ptLowerRight.x = ButtonInfo[uIndex].Rect.right;
            ptLowerRight.y = ButtonInfo[uIndex].Rect.bottom;

            ScreenToClient (hDlg, &ptUpperLeft );
            ScreenToClient (hDlg, &ptLowerRight);

            HWND hChild = CreateWindow ("BUTTON",
                     lpVir->VirList.szCustomButton,
                     WS_CHILD | WS_VISIBLE | WS_TABSTOP |
                     BS_PUSHBUTTON,
                     ptUpperLeft.x,   ptUpperLeft.y,
                     ptLowerRight.x - ptUpperLeft.x + 1,
                     ptLowerRight.y - ptUpperLeft.y + 1,
                     hDlg,
                     (HMENU)(IDI_VL_CUSTOM),
                     hInstance, NULL);

            if ( hChild )
                {
#ifdef SYM_WIN16
                Ctl3dSubclassCtl ( hChild );
#endif

                UINT uInsertAfter;

                if ( lpVir->VirList.bAllowDelete == FALSE )
                    uInsertAfter = IDI_VL_PRINT;
                else
                    uInsertAfter = IDI_VL_DELETE;

                HFONT hFont = GetWindowFont ( GetDlgItem ( hDlg, uInsertAfter ) );
                SetWindowFont ( hChild, hFont, FALSE );
                                        // ----------------------------------
                                        // Make sure that the Z-order is OK
                                        // ----------------------------------

                SetWindowPos ( hChild, GetDlgItem ( hDlg, uInsertAfter ),
                               0, 0, 0, 0,
                               SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW );
                }
            }
        }
}


// ************************************************************************ //
// ****        (C) Copyright Symantec Corporation 19xx - 1993          **** //
// ************************************************************************ //
// ****                                                                **** //
// ****  Procedure: SetupIndexLists                                    **** //
// ****                                                                **** //
// ****  Author: Dan Sackinger               Date: September 22, 1995  **** //
// ****                                                                **** //
// ****  Functionality: Sets up the indexes needed for our VList.      **** //
// ****                                                                **** //
// ****                                                                **** //
// ****  Input:                                                        **** //
// ****                                                                **** //
// ****                                                                **** //
// ****  Output:                                                       **** //
// ****                                                                **** //
// ****                                                                **** //
// ****  Update History:                                               **** //
// ****                                                                **** //
// ****                                                                **** //
// ************************************************************************ //
//************************************************************************

VOID LOCAL SetupIndexLists (LPVIRUSDBF  lpVir,
                            LPHVLVIRUSENTRY lphVirusArray,
                            BOOL bShowMacViruses)
{
    auto    UINT        uCurrRecord;
    auto    WORD        wIndex;

                                        // Count them here
    for (wIndex = 0; wIndex < NUM_LISTBOX; wIndex++)
        lpVir->ListboxArray[wIndex].uNumEntries = 0;

    uCurrRecord = 0;
    while ( *lphVirusArray )
        {

        for (wIndex = 0; wIndex < NUM_LISTBOX; wIndex++)
            if ( ValidVirusType ( *lphVirusArray,
                                   wIndex,
                                   bShowMacViruses ) )
                {
                lpVir->ListboxArray[wIndex].lpuListBoxIndex[lpVir->ListboxArray[wIndex].uNumEntries] = uCurrRecord;
                lpVir->ListboxArray[wIndex].uNumEntries++;
                }

        lphVirusArray++;
        uCurrRecord++;
        } // End of while()
}


// ************************************************************
// ** Routines for sorting the virus list. (Taken from QAK2) **
// ************************************************************

int WINAPI VirusCompare (HPVOID hpOne, HPVOID hpTwo, UINT uDummy,
                                  DWORD dwDummy)
{
    auto HVLVIRUSENTRY hpOneEntry;
    auto HVLVIRUSENTRY hpTwoEntry;

    hpOneEntry = *((LPHVLVIRUSENTRY) hpOne);
    hpTwoEntry = *((LPHVLVIRUSENTRY) hpTwo);

    return ( STRCMP(hpOneEntry->stVirusInfoBlock.lpVName,
                    hpTwoEntry->stVirusInfoBlock.lpVName));
}

void WINAPI VirusSwap (HPVOID hpOne, HPVOID hpTwo)
{
    auto    HVLVIRUSENTRY SwapArea;

    SwapArea = *((LPHVLVIRUSENTRY) hpOne);
    *((LPHVLVIRUSENTRY) hpOne) = *((LPHVLVIRUSENTRY) hpTwo);
    *((LPHVLVIRUSENTRY) hpTwo) = SwapArea;
}


VOID WINAPI SortVirusList (LPHVLVIRUSENTRY lphVirusArray,
                           UINT          uEntriesLoaded)
{
    QuickSort ((HPBYTE)lphVirusArray, uEntriesLoaded, sizeof(HVLVIRUSENTRY),
               &VirusCompare, 0, NULL,
               &VirusSwap, sizeof(HVLVIRUSENTRY));
}


#ifdef SYM_WIN32

/*@API:*************************************************************************
@Declaration:
VOID LOCAL OnContextMenu ( HWND hWnd, LPARAM lParam )

@Description:
This function processes all right mouse button click help message requests.

@Parameters:
$hWnd$ handle to window that requests help
$lParam$ mouse coordinates where click occured.

@Returns:
Nothing.

@Compatibility: Win16 and Win32
*******************************************************************************/

VOID LOCAL OnContextMenu (              // Help on right mouse button click
    HWND    hWnd,                       // [in] window that needs help
	UINT	msg,
	WPARAM  wParam,
    LPARAM  lParam )                    // [in] mouse coordinates
{
#if 0
    static struct tagWHATSTHIS
        {
        DWORD   dwControlID;
        DWORD   dwHelpID;
        } aWhatsThis[7] = { 0 };

    LPVIRUSDBF lpVir = (LPVIRUSDBF)GetWindowLong ( hWnd, DWL_USER );
    SYM_ASSERT ( lpVir );

    if ( aWhatsThis[0].dwControlID == 0 )
        {
        int nIndex = 0;

        if ( lpVir->VirList.uHelpRightClickCombo )
            {
            aWhatsThis[nIndex].dwControlID = IDI_VL_COMBOX;
            aWhatsThis[nIndex].dwHelpID    = lpVir->VirList.uHelpRightClickCombo;
            nIndex++;
            }

        if ( lpVir->VirList.uHelpRightClickPrint )
            {
            aWhatsThis[nIndex].dwControlID = IDI_VL_PRINT;
            aWhatsThis[nIndex].dwHelpID    = lpVir->VirList.uHelpRightClickPrint;
            nIndex++;
            }

        if ( lpVir->VirList.uHelpRightClickCustom )
            {
            aWhatsThis[nIndex].dwControlID = IDI_VL_CUSTOM;
            aWhatsThis[nIndex].dwHelpID    = lpVir->VirList.uHelpRightClickCustom;
            nIndex++;
            }

        if ( lpVir->VirList.uHelpRightClickDelete )
            {
            aWhatsThis[nIndex].dwControlID = IDI_VL_DELETE;
            aWhatsThis[nIndex].dwHelpID    = lpVir->VirList.uHelpRightClickDelete;
            }

        if ( lpVir->VirList.uHelpRightClickInfo )
            {
            aWhatsThis[nIndex].dwControlID = IDI_VL_INFO;
            aWhatsThis[nIndex].dwHelpID    = lpVir->VirList.uHelpRightClickInfo;
            }
        }

    DWORD dwHelpID = 0;
#else
    static struct tagWHATSTHIS
        {
        DWORD   dwControlID;
        DWORD   dwHelpID;
        } aWhatsThis[] = {
                IDI_VL_COMBOX, IDH_NAVW_WHATHIS_VIRUS_LIST_DISPLAY_LSTBX,
                IDI_VL_PRINT, IDH_NAVW_WHATHIS_VIRUS_LIST_PRINT_BTN,
                IDI_VL_CUSTOM, IDH_NAVW_WHATHIS_VIRUS_LIST_BOX,
                IDI_VL_LISTBOX, IDH_NAVW_WHATHIS_VIRUS_LIST_BOX,
                IDI_VL_INFO, IDH_NAVW_WHATHIS_VIRUS_LIST_INFO_BTN,
				IDOK,   IDH_VIRUS_LIST_WHATHIS_CLOSE_BTN,
				IDI_VL_SEARCH,IDH_VIRUS_LIST_WHATHIS_FIND_BTN,
				IDI_VL_SEARCHAGAIN,IDH_VIRUS_LIST_WHATHIS_FIND_NEXT_BTN,
				0, 0
                         };
    LPVIRUSDBF lpVir = (LPVIRUSDBF)GetWindowLong ( hWnd, DWL_USER );
    SYM_ASSERT ( lpVir );

    DWORD dwHelpID = 0;
#endif

    if ( ClickOnListbox ( hWnd, lParam ) == TRUE )
        {
        dwHelpID = lpVir->VirList.uHelpRightClickList;
        }
    else
    {
        BOOL bFound = FALSE;
        short nIndex;

        for (nIndex = 0;
                !bFound && nIndex < sizeof (aWhatsThis) / sizeof (aWhatsThis[0]);
                nIndex++)
        {
                if (aWhatsThis[nIndex].dwControlID ==
                    GetDlgCtrlID (::WindowFromPoint ((*(LPPOINT)&lParam))))
                {
                    bFound = TRUE;
                    dwHelpID = aWhatsThis[nIndex].dwHelpID;
                }
        }
    }

#if 0
    if ( dwHelpID )
        {
        StdPopupHelpFromDialog (
                           hWnd,
                           lParam,
                           SPHFD_SINGLE_WHATS_THIS,
                           &dwHelpID,
                           lpVir->VirList.lpdwHelpHowToArray
                           );
        }
    else
        {
        StdPopupHelpFromDialog (
                           hWnd,
                           lParam,
                           SPHFD_MAPPED_WHATS_THIS,
                           (LPDWORD)&aWhatsThis[0],
                           lpVir->VirList.lpdwHelpHowToArray
                           );
        }
#endif
    StdHelpExOnContextHelp (hWnd,
    						msg,
    						wParam,
    						lParam,
    						(LPDWORD)aWhatsThis,
                    		NULL, 
                    		NULL,
                    		NULL );
}

// --------------------------------------------------------------------------
// --------------------------------------------------------------------------

BOOL LOCAL ClickOnListbox (             // Returns TRUE if the user clicked
    HWND    hWnd,                       // on a listbox within this dialog box.
    LPARAM  lParam )                    // [in] point where the user clicked.
{
    auto    BOOL    bClickedOnListbox = FALSE;
    auto    POINT   pointClickedClient = { 0 };

    pointClickedClient.x = LOWORD(lParam);
    pointClickedClient.y = HIWORD(lParam);
    ScreenToClient(hWnd, &pointClickedClient);

    HWND hWndClicked = ChildWindowFromPoint ( hWnd, pointClickedClient );

    if ( hWndClicked )
        {
        int nID = GetDlgCtrlID ( hWndClicked );

        if ( nID >= IDI_VL_LISTBOX && nID <= (IDI_VL_LISTBOX + NUM_LISTBOX) )
            bClickedOnListbox = TRUE;
        }

    return ( bClickedOnListbox );
}



/// Virus Search  Functionality


VOID WINAPI VirusSearch( LPVIRSEARCH   lpVirSearch )                 // Displays  virus search dialog 
{
    
	auto    SEARCH    Search = {0};

    SYM_ASSERT ( lpVirSearch);
    Search.lpVirSearch = lpVirSearch;


#ifndef SYM_WIN32
    auto        DLGPROC        lpfn;

    lpfn = (DLGPROC) MakeProcInstance((FARPROC)VirusSearchProc, hInstance);

    int nStatus = DialogBoxParam (
                             hInstance,
                             MAKEINTRESOURCE(IDD_VIRSEARCH),
                             lpVirSearch->hParent,
                             lpfn,
                             (LPARAM)&Search 
                             );

    FreeProcInstance((FARPROC) lpfn);
#else
    int nStatus = DialogBoxParam (
                             hInstance,
                             MAKEINTRESOURCE(IDD_VIRSEARCH),
                             lpVirSearch->hParent,
                             VirusSearchProc,
                             (LPARAM)&Search
                             );
#endif
    SYM_ASSERT ( nStatus != -1 );
}




// ************************************************************************ //
// ****        (C) Copyright Symantec Corporation 19xx - 1993          **** //
// ************************************************************************ //
// ****                                                                **** //
// ****  Procedure:                                                    **** //
// ****                                                                **** //
// ****  Author: TODO          Date:								   **** //
// ****                                                                **** //
// ****  Functionality:                                                **** //
// ****                                                                **** //
// ****                                                                **** //
// ****  Input:                                                        **** //
// ****                                                                **** //
// ****                                                                **** //
// ****  Output:                                                       **** //
// ****                                                                **** //
// ****                                                                **** //
// ****  Update History:                                               **** //
// ****                                                                **** //
// ****                                                                **** //
// ************************************************************************ //

BOOL SYM_EXPORT WINAPI VirusSearchProc (  // Handle message to info window
    HWND    hDlg,                       // [in] info window handle
    UINT    uMessage,                   // [in] message to process
    WPARAM  wParam,                     // [in] wparam
    LPARAM  lParam )                    // [in] lparam
{
    auto    BOOL    bReturn = FALSE;
	auto    int   nResult = 0; 
	/*auto    char  lpString[256];		 TODO: how big?*/
	auto    TCHAR   szMsg [ SYM_MAX_PATH ];
	auto    TCHAR   szTitle[ SYM_MAX_PATH ] ; 
	 
  

    switch ( uMessage )
     {
           case WM_INITDIALOG:
			{
			    NG_SetFontsInDialog ( hDlg, 0 );

				LPSEARCH lpSearch = (LPSEARCH)lParam;
				SYM_ASSERT ( lpSearch );
		        SetWindowLong ( hDlg, DWL_USER, (LONG)lpSearch );
				if( glpSearchstr )
				{
					Static_SetText(GetDlgItem(hDlg, IDI_VL_SEARCHTEXT), glpSearchstr ) ;
				} 

                bReturn = TRUE;
            }
            break;

			case WM_COMMAND:
				switch ( wParam ) 
				{
			    
								
					case IDOK:
					{
						BOOL bRes ;
						HCURSOR hCursor = NULL ;

                        hCursor = SetCursor(LoadCursor(NULL, IDC_WAIT)) ;
						LPSEARCH lpSearch = (LPSEARCH)GetWindowLong ( hDlg, DWL_USER );
						
						SYM_ASSERT ( lpSearch );
						GetDlgItemText(hDlg, IDI_VL_SEARCHTEXT, glpSearchstr, 255 ) ; //TODO: how big?
											
						bRes = DoSearch(hDlg,lpSearch, glpSearchstr ) ; //lpString
						
					    if( bRes )
						{
						    if( hCursor) 
								SetCursor( hCursor ) ;
							EndDialog ( hDlg, nResult );
						} 
					    else
						{
					        if( hCursor) 
								SetCursor( hCursor ) ;
							LoadString ( hInstance, IDS_TITLE1, szTitle, sizeof(szTitle) );
							LoadString ( hInstance, IDS_MESSAGE, szMsg, sizeof(szMsg) );  
							MessageBox( hDlg, szMsg,szTitle,MB_OK|MB_OK|MB_ICONINFORMATION ) ; 
						    EndDialog ( hDlg, nResult );
						}

                    }
					break ;

				case IDCANCEL:
					EndDialog ( hDlg, nResult );
					break;
			}
           
        }

   return ( FALSE );
}


BOOL DoSearch( HWND hDlg, LPSEARCH     lpSearch,  LPTSTR lpStr ) 
{
	
	char *pDest; 
	const   UINT    uBufferSize = ( SYM_MAX_PATH * 2 );
    auto    LPSTR   lpWork;
	int nCount ; 
	    

    lpWork  = ( LPSTR )MemAllocPtr ( GHND, uBufferSize );
    
	if ( glpSearchstr )
		Button_Enable (GetDlgItem(lpSearch->lpVirSearch->hParent, IDI_VL_SEARCHAGAIN), TRUE);

	if( ( strcmp(glpSearchstr, "" ) == 0 ) )
		Button_Enable (GetDlgItem(lpSearch->lpVirSearch->hParent , IDI_VL_SEARCHAGAIN), FALSE);
	
						

	nCount = VListBox_GetCount( lpSearch->lpVirSearch->hListBox ) - 1;
	
    if ( lpWork )
    {
       
        for (int i = 0; i <= nCount ; i++ )
		{
			
			
		    
			lpSearch->hEntry = (HVLVIRUSENTRY)ListBox_GetItemData(
                                                lpSearch->lpVirSearch->hListBox,
                                                i
                                                );
			SYM_ASSERT ( lpSearch->hEntry );
	      	BOOL bResult = VLDefInfo( lpSearch->hEntry, VI_VIRUS_NAME, lpWork );
			SYM_ASSERT ( bResult );
			SYM_ASSERT ( *lpWork );
    			            
			pDest= _tcsstr ( _tcslwr(lpWork), _tcslwr(lpStr) ) ;
		
            if( pDest != NULL  ) 
			{
		          SendMessage(GetDlgItem( GetParent( hDlg), IDI_VL_LISTBOX),LB_SETCURSEL,i,0);
                  return TRUE ;
			 }
		}//for
	}//if 
	
	return  FALSE ;
}


BOOL DoSearchAgain( HWND hDlg, LPVIRUSDBF lpVir ) 
{

	char *pDest;
	int  nCount ;
	const   UINT    uBufferSize = ( SYM_MAX_PATH * 2 );
    auto LPSTR   lpWork;
	
	HVLVIRUSENTRY hEntry ;
	SYM_ASSERT ( hDlg );
    SYM_ASSERT ( lpVir );
		
    lpWork      = ( LPSTR )MemAllocPtr ( GHND, uBufferSize );
	int nIndex = VListBox_GetCurSel(ACTIVELISTBOX(lpVir)); 
   	nCount = VListBox_GetCount( ACTIVELISTBOX(lpVir) ) - 1 ;
    if ( nIndex != LB_ERR )
	{
		for( int i = nIndex+1;i<= nCount;i++ ) 
		{
		   hEntry = (HVLVIRUSENTRY)ListBox_GetItemData(
                                                ACTIVELISTBOX(lpVir),
                                                i
                                                );
			SYM_ASSERT ( hEntry );
	      	BOOL bResult = VLDefInfo( hEntry, VI_VIRUS_NAME, lpWork );
			SYM_ASSERT ( bResult );
			SYM_ASSERT ( *lpWork );
			pDest= _tcsstr( _tcslwr(lpWork), _tcslwr(glpSearchstr) ) ;
			
		    if( pDest != NULL  ) 
			{
				
                 
				VListBox_SetCurSel(ACTIVELISTBOX(lpVir), i );
				return TRUE ;
            }
        }
	}
	return FALSE ;

}


BOOL InitGlobals()
{
BOOL  bRet = FALSE;
TCHAR szWork [SYM_MAX_PATH+1];
HKEY  hkey;
DWORD dwType, dwBuffSize = sizeof(szWork);

    g_bOSIsNT = ( GetVersion() & 0x80000000UL ) ? FALSE : TRUE;

    
    // Read the NAV install directory.    
    if ( ERROR_SUCCESS ==
         RegOpenKeyEx ( HKEY_LOCAL_MACHINE,
                        _T("software\\symantec\\InstalledApps"),
                        0, KEY_QUERY_VALUE, &hkey ) )
        {
        if ( ERROR_SUCCESS == 
             RegQueryValueEx ( hkey, _T("NAV"), NULL, &dwType,
                               (LPBYTE) szWork, &dwBuffSize ) )
            {
            if ( REG_SZ == dwType )
                {
				   bRet = TRUE;
                   lstrcpy( g_sNAVDir, szWork ) ;
                
				 }

			RegCloseKey ( hkey );
			}

       
	}
    return bRet;
}

#endif // 32 stuff
