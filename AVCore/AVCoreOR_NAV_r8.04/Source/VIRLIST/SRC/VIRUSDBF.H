#ifndef _VIRUSDBF
#define _VIRUSDBF

#include "ngui.h"                       // Password stuff
#include "avapi.h"                      // AV lib API's
#include "virlist.h"                    // structures visible to applications
#include "virusstb.h"                   // our string resource ID's
#include "virusdlg.h"                   // our dialog box resource ID's
#include "stack.hpp"                    // for stack objects

#define SIZE_COPYRIGHT  128

// *********************************************** //
// Different type of viruses that can be displayed //
// *********************************************** //

enum {
     VIRUSTYPE_ALL = 0,
     VIRUSTYPE_COMMON,
     VIRUSTYPE_FILE,
     VIRUSTYPE_BOOT,
     VIRUSTYPE_STEALTH,
     VIRUSTYPE_POLY,
     VIRUSTYPE_MULTIPARTY,
     VIRUSTYPE_MACRO,
     VIRUSTYPE_WINDOWS,
     VIRUSTYPE_AGENT,
     VIRUSTYPE_MAC
     };

typedef WORD VISTATUS;

#define VERR_OK             ((VISTATUS)0)
#define VERR_MEMALLOC       ((VISTATUS)1)
#define VERR_FILEREAD       ((VISTATUS)2)
#define VERR_FILEWRITE      ((VISTATUS)3)
#define VERR_LISTBOXCREATE  ((VISTATUS)4)
#define VERR_DATAFILES      ((VISTATUS)5)   // VIRSCAN.DAT and/or VIRSCAN.INF
                                            // doesn't exist or invalid format
#define VERR_WRONGPARENT    ((VISTATUS)6)   // Bad parent window handle
#define VERR_INTERNAL       ((VISTATUS)7)

typedef struct tagVIRINFO
{
    HWND        hParent;                // Parent handle
    HWND        hListBox;               // Listbox handle

    UINT        uHelpID;                // Help ID to use
    UINT        uHelpRightClickThisDlg; // Help ID for WhatsThis Dlg itself
    UINT        uHelpRightClickComments;

    UINT        uHelpPrint;             // Help ID for Print
    UINT        uHelpPrintRightClickThisDlg;
    UINT        uHelpPrintOverwrite;    // Help ID for Print Overwrite
    UINT        uHelpPrintBrowse;       // Help ID for Print Browse

    UINT        uHelpPrintRightClickPrintFileButton;
    UINT        uHelpPrintRightClickPrintPrinterButton;
    UINT        uHelpPrintRightClickSendUserButton;
    UINT        uHelpPrintRightClickPrintFileTunnel;
    UINT        uHelpPrintRightClickPrintPrinterTunnel;

    LPDWORD     lpdwHelpHowToArray;      // Array of DWORDS for How To help.
    LPDWORD     lpdwHelpHowToPrintArray; // Array of DWORDS for How To help.

                                         // Added members for other Help IDs:
                                         // (Added after additions to VIRLIST
                                         // in virlist.h)
    UINT        uHelpVirusInfoRightClickCloseBtn;     
    UINT        uHelpVirusInfoRightClickPrintBtn;  
    UINT        uHelpVirusInfoRightClickCharacteristics;

} VIRINFO, FAR *LPVIRINFO;


typedef struct tagVIRSEARCH
{
    HWND        hParent;                // Parent handle
    HWND        hListBox;               // Listbox handle
}
VIRSEARCH, FAR *LPVIRSEARCH;

typedef struct tagERASESTRUCT
{
    HWND        hDlg;
    LPSTR       lpFileName;
    UINT        uHelpID;

} ERASE, FAR * LPERASE;


// ********************************** //
// Variables used throughout this dll //
// ********************************** //

//#define NUM_LISTBOX 8
#define NUM_LISTBOX 11

typedef HVLVIRUSENTRY FAR * LPHVLVIRUSENTRY;

typedef struct tagLBOX
    {
    HWND    hListbox;
    UINT    uNumEntries;
                                        // Add support for VLIST.
    LPUINT      lpuListBoxIndex;        // Index to entry number in lpN30Notes
    } LBOX, FAR * LPLBOX;

typedef struct tagVIRUSDBF
{
    VIRLIST         VirList;            // Information from application.

    UINT            uActiveEntry;       // ListboxArray active entry index
    LPHVLVIRUSENTRY   lphVirusArray;
    HVCONTEXT       hContext;

    LPSTACKOBJECT   lpDeleted;          // Deleted items;

    LBOX        ListboxArray [ NUM_LISTBOX + 1 ];

    UINT            uTotalDetected;     // Total number detected (from header)

} VIRUSDBF, FAR *LPVIRUSDBF;

                                        // No more multiple list boxes!
#define ACTIVELISTBOX(lpV)\
  (((LPVIRUSDBF)lpV)->ListboxArray[VIRUSTYPE_ALL].hListbox)

#define ACTIVELISTBOXENTRIES(lpV)\
  (((LPVIRUSDBF)lpV)->ListboxArray[((LPVIRUSDBF)lpV)->uActiveEntry].uNumEntries)

// --------------------------------------------------------------------------
//              FUNCTION PROTOTYPES USED INTERNALLY
// --------------------------------------------------------------------------

VOID WINAPI ErrorHandler (              // Error handler that display a message
    HWND    hDlg,                       // [in] parent window
    VISTATUS );                         // [in] error status

VOID WINAPI VirusInfo (                 // Display virus information
    LPVIRINFO   lpVirInfo );            // [in] initialization structure

// modified by Anjana
/*VOID WINAPI VirusSearch (HWND hDlg, 
	LPVIRUSDBF  lpVir ) ;

BOOL  OnSearch( LPTSTR lpStr ) ;

BOOL OnSearchAgain() ;*/


VOID WINAPI DisplayNumDefs (            // Display the number of defs in view
    HWND    hDlg,                       // [in] main dlg box handle
    UINT    uNumDefs );                 // [in] number ot display

VOID WINAPI DoDelete (                  // Do delete ritual
    HWND        hDlg,                   // [in] window handle
    LPVIRUSDBF  lpVir );                // [in] virus database pointer

VOID WINAPI DoOnDestroy (               // Handle the WM_DESTROY message
    HWND    hDlg );                     // [in] from this window

VOID WINAPI DisplayDefinitionDate (     // Show the date of the definition
    HWND        hDlg,                   // [in] parent window handle
    LPVIRUSDBF  lpVir );                // [in] virus db ptr

BOOL WINAPI GetVirusHandles (           // Get all the virus handles
    LPVIRUSDBF  lpVir );                // [in] pointer to virus list

BOOL WINAPI FillVirusListbox (          // Fill the virus listbox
    HWND        hListbox,               // [in] handle to listbox
    LPUINT      lpuEntries,             // [out] entries filled
    LPHVLVIRUSENTRY lphVirusArray,        // [in] array of virus entries
    WORD        wVirusTypes,            // [in] virus types to fill
    BOOL        bMacViruses );          // [in] also mac viruses

HGLOBAL SYM_EXPORT WINAPI BuildVirusListInfo (// Print the virus list
    HWND        hDlg,                   // [in] main dlg box handle
    HWND        hListBox                // [in] listbox handle
    );

HGLOBAL WINAPI BuildVirusInfo (         // Build virus information
    HWND        hDlg,                   // [in] dialog box handle that has info
    BOOL        bMacType                // [in] include mac type of virus info
    );

LPHVLVIRUSENTRY WINAPI GetVirusInfoPtr (  // Get Item Data from our listbox.
    HWND        hListBox,               // [in] Handle to listbox
    WORD        wIndex                  // [in] Index of data to retrieve
    );

BOOL WINAPI GetVirusText (              // Retrieve a string from our list box
    HWND        hListBox,               // [in] Handle to our list box
    WORD        wIndex,                 // [in] Index of string to create
    LPBYTE      lpWorkMem               // [out] string in this buffer.
    );

BOOL WINAPI GetListBoxString (
    LPVIRUSDBF    lpVir,                // [in] virus database pointer
    UINT          uIndex,               // [in] index of string to get
    TCHAR        *szWork                // [out] string in buffer
    );

#endif // _VIRUSDBF
