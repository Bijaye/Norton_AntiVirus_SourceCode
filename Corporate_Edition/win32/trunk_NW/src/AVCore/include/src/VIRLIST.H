// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright 1995 - 2003, 2005 Symantec Corporation. All rights reserved.
//***************************************************************************
//
// $Header:   S:/INCLUDE/VCS/virlist.h_v   1.16   25 Jun 1996 16:04:10   DSACKIN  $
//
// Description:
//
// See Also:
//
//***************************************************************************
// $Log:   S:/INCLUDE/VCS/virlist.h_v  $
// 
//    Rev 1.16   25 Jun 1996 16:04:10   DSACKIN
// the #endif at the end of the file had text after it and the RC compiler
// was complaining about it.
//
//    Rev 1.15   20 Jun 1996 08:38:40   TIVANOV
// remove a ctrl-z at the end of the file
//
//    Rev 1.14   14 Nov 1995 13:49:38   GDZIECI
// Merged Keith's changes from QUAKE 6.
// Added standard file header.
//***************************************************************************

#ifndef _VIRLIST_H_
#define _VIRLIST_H_

#include "npt.h"
#include "avapi.h"

#define MAX_CUSTOM_BUTTON 20
typedef BOOL ( WINAPI FAR * LPFNCUSTOM ) ( HWND, DWORD );

typedef struct tagVIRLIST
{
    DWORD       dwSizeStruct;           // Structure size
    HWND        hDlg;                   // Mommy window
    LPFNCUSTOM  lpfnCustom;             // custom callback
    DWORD       dwUserData;
    char        szOKButtonTitle[ MAX_CUSTOM_BUTTON ];
    char        szCustomButton [ MAX_CUSTOM_BUTTON ];
    char        szDatFile [ SYM_MAX_PATH ]; // VIRSCAN.DAT path
    char        szInfFile [ SYM_MAX_PATH ]; // VIRSCAN.INF path
    BOOL        bMacViruses;            // TRUE to show mac viruses
    BOOL        bAllowInfo;             // TRUE to show info button
    BOOL        bAllowDelete;           // TRUE to show delete button

    DWORD       dwPassword;             // Password if any
    UINT        uMaxChars;              // max num pw chars
    BOOL        bCaseSensitive;         // be case sensitive

    UINT        uHelpVirusList;         // Help ID for Virus List
    UINT        uHelpRightClickThisDlg; // Help ID for WhatsThis Dlg itself

    UINT        uHelpVirdefDelete;      // Help ID for Delete Virus

    UINT        uHelpVirusInfo;         // Help IDsfor Virus Info
    UINT        uHelpVirusInfoRightClickThisDlg;
    UINT        uHelpVirusInfoRightClickComments;

    UINT        uHelpPrint;             // Help ID for Print
    UINT        uHelpPrintRightClickThisDlg;

    UINT        uHelpPrintOverwrite;    // Help ID for Print Overwrite
    UINT        uHelpPrintBrowse;       // Help ID for Print Browse


    UINT        uHelpRightClickCombo;
    UINT        uHelpRightClickList;
    UINT        uHelpRightClickPrint;
    UINT        uHelpRightClickCustom;
    UINT        uHelpRightClickDelete;
    UINT        uHelpRightClickInfo;

    UINT        uHelpPrintRightClickPrintFileButton;
    UINT        uHelpPrintRightClickPrintPrinterButton;
    UINT        uHelpPrintRightClickSendUserButton;
    UINT        uHelpPrintRightClickPrintFileTunnel;
    UINT        uHelpPrintRightClickPrintPrinterTunnel;

    LPDWORD     lpdwHelpHowToArray;      // Array of DWORDS for How To help.
    LPDWORD     lpdwHelpHowToDelVirDefArray;
    LPDWORD     lpdwHelpHowToInfoVirDefArray;
    LPDWORD     lpdwHelpHowToPrintArray; // Array of DWORDS for How To help.

                                        // Added members for other Help IDs:
    UINT        uHelpVirusInfoRightClickCloseBtn;
    UINT        uHelpVirusInfoRightClickPrintBtn;
    UINT        uHelpVirusInfoRightClickCharacteristics;
    UINT        uHelpVirusInfoRightClickUnused1;
    UINT        uHelpVirusInfoRightClickUnused2;
    UINT        uHelpVirusInfoRightClickUnused3;

} VIRLIST, FAR * LPVIRLIST;

// --------------------------------------------------------------------------
//              EXPORTED FUNCTIONS ACCESSIBLE TO APPLICATIONS
// --------------------------------------------------------------------------

#define VIR_OK_CHANGED   2
#define VIR_OK_UNCHANGED 1
#define VIR_OK_CANCELED  0  // keep this 0 to match bUpdated (FALSE)

#ifdef __cplusplus
extern "C" {
#endif

// v_info.c //

int SYM_EXPORT WINAPI VirusList (       // Display the virus list
    LPVIRLIST   lpVir );                // [in] initialization structure

int SYM_EXPORT WINAPI VirusListOnly (   // Display the virus list only
    LPVIRLIST   lpVir );                // [in] initialization structure

HVCONTEXT SYM_EXPORT WINAPI InitVirusLibrary ( // Initialize virus library
    LPSTR   lpDatFile,                  // [in] path to DAT file
    LPSTR   lpInfFile );                // [in] path to INF file

HVCONTEXT SYM_EXPORT WINAPI InitVirusList ( // Initialize virus list - no scan
    LPSTR   lpDatFile,                  // [in] path to DAT file
    LPSTR   lpInfFile );                // [in] path to INF file

VOID SYM_EXPORT WINAPI UnloadVirusLibrary ( // Unload virus library
    HVCONTEXT hContext );                   // [in] our nav pointer

BOOL SYM_EXPORT WINAPI ResetVirusInfoContents ( // Reset contents of virus info
    HWND    hVirusInfo );                       // handle to dialog box.

HGLOBAL SYM_EXPORT WINAPI BuildVirusListForPrinting (
    LPPRINTDLGSTRUCT lpPrintDlgStruct);

#ifdef __cplusplus
}
#endif


#endif // _VIRLIST_H_
