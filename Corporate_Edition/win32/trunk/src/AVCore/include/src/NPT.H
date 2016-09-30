// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
/* Copyright 1992 - 2003, 2005 Symantec Corporation. All rights reserved.     */
/************************************************************************/
/*                                                                      */
/* $Header:   S:/INCLUDE/VCS/NPT.H_v   1.0   26 Jan 1996 20:15:38   RJACKSO  $ */
/*                                                                      */
/* Description:                                                         */
/*    This is the main include file for the NPT (Symantec PrinT)        */
/*    component.  It contains the definitions and prototypes required   */
/*    to use the Symantec Printing Dynamic Link Library.                */
/*                                                                      */
/* See Also:                                                            */
/*    nptint.h                                                          */
/*                                                                      */
/************************************************************************/
/* $Log:   S:/INCLUDE/VCS/NPT.H_v  $ */
// 
//    Rev 1.0   26 Jan 1996 20:15:38   RJACKSO
// Initial revision.
// 
//    Rev 1.19   29 Nov 1995 15:09:54   RSTANEV
// Fixed compilation errors in NTK platform.
//
//    Rev 1.18   02 Mar 1995 10:40:22   BRIANF
// Added SymNPTAttach as well as making several changes necessary for custom quake builds.
//
//    Rev 1.17   25 Jan 1995 14:41:06   PATRICKM
// Added prototype for NptGetPlainPrinterDC().
//
//    Rev 1.16   08 Nov 1994 17:28:14   REESE
// Make some of Npt's error codes work.
//
//    Rev 1.15   30 Sep 1994 21:21:04   MFALLEN
// Changed title size back to 40 so I don't brake anything.
//
//    Rev 1.14   30 Sep 1994 20:46:54   MFALLEN
// Title size changed from 40 to 256
//
//    Rev 1.13   28 Sep 1994 08:52:48   MFALLEN
// How To help added.
//
//    Rev 1.12   22 Aug 1994 18:03:36   MFALLEN
// Added right click help.
//
//    Rev 1.11   05 Aug 1994 22:12:32   RSTANEV
// Fixed compilation errors in VxD platform.
//
//    Rev 1.10   20 May 1994 16:03:32   MFALLEN
// Prepended NPT to func name
//
//    Rev 1.9   20 May 1994 16:00:00   MFALLEN
// Added print dialog
//
//    Rev 1.8   26 Apr 1994 16:40:56   BRAD
// Removed PVCS Logs
/************************************************************************/


#ifndef _NPT_H
#define _NPT_H

#include "quakever.h"
#include "version.h"

#define NPT_USERCANCEL      0xBAD

#define NPT_VERSION_MAJOR   QUAKE_VERSION_MAJOR
#define NPT_VERSION_MINOR   QUAKE_VERSION_MINOR
#define NPT_BUILD_NUMBER    QUAKE_BUILD_NUMBER

/************************************************************************/
/* Preprocessor constants                        */
/************************************************************************/
#define NPT_INVAL_CUR_PRNTR        ERR_SEV_NUM (ERR_SEV_WARNING, \
                             NPT_INVALID_PRINTER)
#define NPT_CANT_CREATE_DC        ERR_SEV_NUM (ERR_SEV_WARNING, \
                             ERR_DC_CANT_CREATE)
#define NPT_CANT_STARTDOC        ERR_SEV_NUM (ERR_SEV_WARNING, \
                             NPT_STARTDOC)
#define NPT_CANT_DRAFTMODE        ERR_SEV_NUM (ERR_SEV_WARNING, \
                             NPT_DRAFTMODE)
#define NPT_CANT_GETPHYSPAGESIZE    ERR_SEV_NUM (ERR_SEV_WARNING, \
                             NPT_GETPHYSPAGESIZE)
#define NPT_CANT_GETPRINTINGOFFSET    ERR_SEV_NUM (ERR_SEV_WARNING, \
                             NPT_GETPRINTINGOFFSET)
#define NPT_CANT_SETABORTPROC        ERR_SEV_NUM (ERR_SEV_FATAL, \
                             NPT_SETABORTPROC)
#define NPT_CANT_SETCOPYCOUNT        ERR_SEV_NUM (ERR_SEV_WARNING, \
                             NPT_SETCOPYCOUNT)
#define NPT_CANT_GETTECHNOLOGY        ERR_SEV_NUM (ERR_SEV_WARNING, \
                             NPT_GETTECHNOLOGY)
#define NPT_NO_BITBLT             15776 /* ERR_NUM(  3,   8) */
#define NPT_CANT_BITBLT             ERR_SEV_NUM (ERR_SEV_WARNING, \
                             NPT_NO_BITBLT)
#define NPT_DOCNAME_OVERFLOW        ERR_SEV_NUM (ERR_SEV_FATAL, \
                             NPT_DOCNAME_TOO_LONG)
#define NPT_APPLNAME_OVERFLOW        ERR_SEV_NUM (ERR_SEV_FATAL, \
                             NPT_APPLNAME_TOO_LONG)
#define NPT_CANT_MPI_ABORTPROC        ERR_SEV_NUM (ERR_SEV_FATAL, \
                             ERR_MAKEPROCINSTANCE)
#define NPT_CANT_DESTROY_DC        ERR_SEV_NUM (ERR_SEV_FATAL, \
                             ERR_DC_CANT_DELETE)
#define NPT_CANT_NEWFRAME        ERR_SEV_NUM (ERR_SEV_WARNING, \
                             NPT_NEWFRAME)
#define NPT_EXCEEDED_PRINT_JOBS        ERR_SEV_NUM (ERR_SEV_FATAL, \
                             NPT_PRINT_JOBS)
#define NPT_JOB_LIST_CORRUPT        ERR_SEV_NUM (ERR_SEV_FATAL, \
                             NPT_LIST_CORRUPT)
#define NPT_MEM_CANT_ALLOC        ERR_SEV_NUM (ERR_SEV_FATAL, \
                             NERR_MEM_CANT_ALLOC)
#define NPT_MEM_CANT_LOCK        ERR_SEV_NUM (ERR_SEV_FATAL, \
                             ERR_MEM_CANT_LOCK)
#define NPT_MEM_CANT_UNLOCK        ERR_SEV_NUM (ERR_SEV_FATAL, \
                             ERR_MEM_CANT_UNLOCK)
#define NPT_MEM_CANT_FREE        ERR_SEV_NUM (ERR_SEV_FATAL, \
                             ERR_MEM_CANT_FREE)
#define NPT_ESC_GEN_ERROR        ERR_SEV_NUM (ERR_SEV_WARNING, \
                             NPT_ESCAPE_GEN_ERROR)
#define NPT_ESC_FROM_PROG        ERR_SEV_NUM (ERR_SEV_WARNING, \
                             NPT_ESCAPE_FROM_PROG)
#define NPT_ESC_FROM_SPOOLER        ERR_SEV_NUM (ERR_SEV_WARNING, \
                             NPT_ESCAPE_FROM_SPOOLER)
#define NPT_ESC_OUT_OF_DISK        ERR_SEV_NUM (ERR_SEV_WARNING, \
                             NPT_ESCAPE_FROM_DISK)
#define NPT_ESC_OUT_OF_MEM        ERR_SEV_NUM (ERR_SEV_WARNING, \
                             NPT_ESCAPE_OUT_OF_MEM)
#define NPT_INVALID_WINDOW        ERR_SEV_NUM (ERR_SEV_FATAL, \
                             ERR_WND_INVALID)
#define NPT_STRING_CANT_LOAD        ERR_SEV_NUM (ERR_SEV_FATAL, \
                             ERR_STRING_CANT_LOAD)
#define NPT_FILE_CANT_OPEN        ERR_SEV_NUM (ERR_SEV_WARNING, \
                             NPT_FILE_NOT_FOUND)
#define NPT_FILE_IO_ERR            ERR_SEV_NUM (ERR_SEV_WARNING, \
                             ERR_FILE_IO)
#define NPT_DIALOG_CANT_CREATE        ERR_SEV_NUM (ERR_SEV_FATAL, \
                             ERR_DIALOG_CANT_CREATE)

#define NPT_TOOMANYPRINTERS  ERR_SEV_NUM (ERR_SEV_WARNING, \
                             NPT_TOO_MANY_PRINTERS)

#define NPT_DRAFTMODE_ON            1
#define NPT_DRAFTMODE_OFF           0

#define NPT_TECHNOLOGY_SIZE         80
#define NPT_DOCUMENTNAME_SIZE       61
#define NPT_APPLNAME_SIZE           26
#define NPT_FILENAME_SIZE           129

#define NPT_LIST_TYPE_COMBO         1
#define NPT_LIST_TYPE_LIST          2

#define FAXDRIVER   "FAX"

#define NPT_INVALID                 0
#define NPT_PRINTER                 1
#define NPT_FAX                     2

#define NPT_PRINTINFOSIZE           128


/************************************************************************/
/* Type defs that SHOULD be in windows.h (those goons....)              */
/************************************************************************/
typedef HDC         *PHDC;
typedef HDC NEAR    *SPHDC;
typedef HDC FAR     *LPHDC;
typedef HWND FAR    *LPHWND;

typedef WORD (CALLBACK * NPTABORTEDPROC)(HWND, HDC);


/************************************************************************/
/* Type defs required by the Symantec Print Routines                    */
/************************************************************************/
typedef struct tagNPTINFO
   {
    BOOL    bBitBltSupport;             // BitBlt support of printer
    WORD    wActualCopies;              // Actual copies per page
    BOOL    bBanding;                   // Banding support of printer
    char    szTechnology[NPT_TECHNOLOGY_SIZE]; // Printer technology
    int     iHorzSize;                  // Horizontal Size (DEVCAPS)
    int     iVertSize;                  // Vertical Size (DEVCAPS)
    POINT   ptDimensions;               // Physical Page Dimensions
    POINT   ptOffset;                   // Physical U/L Page Offset
   } NPTINFO;
typedef NPTINFO         *PNPTINFO;
typedef NPTINFO NEAR    *SPNPTINFO;
typedef NPTINFO FAR     *LPNPTINFO;


//************************************************************************
// Component global variables
//************************************************************************



//************************************************************************/
// Component prototype declarations
//************************************************************************/

                                        /* LibMain() init routine for    */
                                        /* NWIN.DLL only!               */
WORD NptInit (void);

#if !defined(SYM_WIN32) && !defined(SYM_VXD) && !defined(SYM_NTK)
#    if _MSC_VER >= 700
        WORD __based(__segname("_CUSTCTRL")) NptInit (void);
#    else
#       pragma alloc_text(_CUSTCTRL, NptInit)
#    endif
#endif

#ifdef __cplusplus
extern "C" {
#endif

WORD SYM_EXPORT WINAPI NptStartDocumentEx (    /* Starting Document any printer*/
    HWND            hWnd,               /* Window handle of caller    */
    LPHDC           lphDC,              /* LP to buffer to return DC    */
    LPHWND          lphStatusDlg,       /* LP to to Status/Abort hDlg    */
    LPSTR           lpszApplName,       /* LP to application name    */
    LPSTR           lpszDocumentName,   /* LP to document name        */
    NPTABORTEDPROC  lpfnAbortNotify,    /* What to call if job aborted    */
    BOOL            bBitBlt,            /* TRUE if must have BitBlt    */
    WORD            wNumCopies,         /* Number of copies        */
    WORD            wDraftMode,         /* Draftmode            */
    LPNPTINFO       lpNptInfo,          /* Lp to Npt Info record    */
    LPSTR           lpszPrinterSpec);   /* Specifies printer to use     */

#ifdef  __cplusplus
inline WORD NptStartDocument(HWND A, LPHDC B, LPHWND C, LPSTR D,
                             LPSTR E, NPTABORTEDPROC F, BOOL G,
                             WORD H, WORD I, LPNPTINFO J)
        { return NptStartDocumentEx(A, B, C, D, E, F, G, H, I, J, NULL); }
#else
#define NptStartDocument(A,B,C,D,E,F,G,H,I,J) \
        NptStartDocumentEx(A,B,C,D,E,F,G,H,I,J,NULL)
#endif

WORD SYM_EXPORT WINAPI NptEndPage (        /* For starting Symantec Document    */
    HWND    hWnd,                          /* Window handle of caller    */
    LPHDC    lphDC);                       /* LP to buffer of DC to destroy*/

WORD SYM_EXPORT WINAPI NptEndDocument (    /* For starting Symantec Document    */
    HWND    hWnd,                          /* Window handle of caller    */
    LPHDC    lphDC,                        /* LP to buffer of DC to destroy*/
    BOOL    bAbnormal);                    /* Printing abnormally stopped?    */

WORD SYM_EXPORT WINAPI NptPrintBuffer (    /* Outputs buffer to printer    */
    HWND    hWnd,                          /* Handle of calling window    */
    LPSTR    lpszApplName,                 /* Name of app calling routine    */
    LPSTR    lpszDocumentName,             /* Name of buffer to be printed    */
    LPSTR    lpszBuffer,                   /* Buffer to output to printer    */
    BOOL    bWordWrap,                     /* Is output to be wordwrapped?    */
    WORD    wDraftMode);                   /* Draft mode            */

WORD SYM_EXPORT WINAPI NptPrintAsciiFileEx (    /* Outputs file to printer    */
    HWND    hWnd,                               /* Handle of calling window    */
    LPSTR    lpszApplName,                      /* Name of app calling routine    */
    LPSTR    lpszDocumentName,                  /* Name of buffer to be printed    */
    LPSTR    lpszFileSpec,                      /* Name of file to print    */
    BOOL    bWordWrap,                          /* Is output to be wordwrapped?    */
    WORD    wDraftMode,                         /* Draft mode            */
    LPSTR       lpszPrinterSpec);               /* Printer to use (NULL=default)*/

#ifdef  __cplusplus
inline WORD NptPrintAsciiFile (HWND A, LPSTR B, LPSTR C, LPSTR D, BOOL E, WORD F)
        { return NptPrintAsciiFileEx(A, B, C, D, E, F, NULL); }
#else
#define NptPrintAsciiFile(A,B,C,D,E,F) \
        NptPrintAsciiFileEx(A,B,C,D,E,F,NULL)
#endif

WORD SYM_EXPORT WINAPI NptPrinterSelect (      /* Selects current printer      */
    HWND        hWnd);                  /* Handle of calling window     */

WORD SYM_EXPORT WINAPI NptPrinterAgentSelect ( /* Selects printer agents       */
    HWND        hWnd,                   /* Handle of calling window     */
    HANDLE      hAgentList);            /* Handle list of agents        */

WORD SYM_EXPORT WINAPI NptPrinterSelListInit  (/* Initialize printer list      */
    HWND        hDlg,                   /* Handle to the dialog box     */
    WORD        wListID,                /* Id of list box control       */
    WORD        wListType);             /* Type of list control         */

WORD SYM_EXPORT WINAPI NptPrinterSelListDeInit ( /* Deinits printer list       */
    HWND        hDlg,                   /* Handle to the dialog box     */
    WORD        wListID,                /* ID of list box control       */
    WORD        wListType,              /* Type of list control         */
    BOOL        bSetDefault);           /* TRUE sets default printer    */

WORD SYM_EXPORT WINAPI NptPrinterSelSetDefault ( /* Set default printer        */
    HWND        hDlg,                   /* Handle to dialog box         */
    WORD        wListID,                /* ID for list box control      */
    WORD        wListType);             /* Type of list control         */

WORD SYM_EXPORT WINAPI NptPrinterSelSetup  (   /* Setup selected printer       */
    HWND        hDlg,                   /* Handle to the dialog box     */
    WORD        wListID,                /* Id of list box control       */
    WORD        wListType);             /* Type of list control         */

WORD SYM_EXPORT WINAPI NptPrinterGetType (     /* Fax, printer, or invalid?    */
    HWND        hDlg,                   /* Handle to the dialog box     */
    LPSTR       lpPrinter);             /* Printer name                 */

WORD SYM_EXPORT WINAPI NptPrinterSelListEntry ( /* Build printer list entry  */
    HWND        hDlg,                   /* Handle to dialog box         */
    LPSTR       lpszPrinter,            /* Name of printer              */
    LPSTR       lpszEntry,              /* Entry in list to build       */
    LPSTR       lpszFormat);            /* Format string to use         */

WORD SYM_EXPORT WINAPI NptGetPlainPrinterDC (  		/* Obtains a DC for a printer	*/
    HWND	 hWnd,			/* Caller's window handle	*/
    BOOL	 bBitBlt,		/* TRUE if must have BitBlt	*/
    LPBOOL	 lpbActualBitBlt,	/* LP to receive BltBlt support	*/
    LPHDC	 lphDC,			/* LP to buffer to return DC	*/
    LPBOOL	 lpbBanding,		/* LP to buff to receive band 	*/
    LPINT	 lpiHorzSize,		/* LP to receive horz size	*/
    LPINT	 lpiVertSize,		/* LP to receive vert size	*/
    LPSTR        lpszPrinterSpec);      /* Printer or NULL for defualt  */

BOOL WINAPI SymNPTAttach(HINSTANCE hModule,WORD wDataSeg,WORD cbHeapSize,
                   LPSTR lpszCmdLine);

//////////////////////////////////////////////////////////////////////////
// NPTCheckVersion
//
// A DLL which links to NPT calls this function inside of its own
// <DLL-Name>CheckVersion function, in order to ensure that the version
// of NPT it is linked to is late enough.
//
// Input:   lpExpected  expected version
//          CallBack    function to call in case of mismatch
//////////////////////////////////////////////////////////////////////////
void SYM_EXPORT WINAPI NPTCheckVersion( LPVERREC lpExpected,
    CHECKVERCALLBACK_TYP CallBack );


//-----------------------------------------------------------------------------
//  This section deals with displaying a dialog box to the user where it is
//  possible to select where the printing should take place. Information to
//  be printed can go to the printer, to an ASCII file and also be sent via
//  mail if the appropriate DLL's are present.   Author: Martin Fallenstedt
//-----------------------------------------------------------------------------

struct tagPRINTDLGSTRUCT;
typedef struct tagPRINTDLGSTRUCT FAR *LPPRINTDLGSTRUCT;

                                        // Prototype for the NPTPRINTDLGPROC
                                        // callback routine.
typedef HGLOBAL (CALLBACK *PRINTDLGPROC)(LPPRINTDLGSTRUCT);

typedef WORD PRINTERR;

#define PRINTERR_OK              ((WORD)1)  // No error occured.
#define PRINTERR_INTERNAL        ((WORD)2)  // Internal error occured.
#define PRINTERR_APPLICATION     ((WORD)3)  // Error caused by application.
#define PRINTERR_CANCEL          ((WORD)4)  // User cancelled printing.
#define PRINTERR_BADFILESELECTED ((WORD)5)  // Invalid file selected.

#define MAX_PRINT_TITLE 40

typedef struct tagPRINTDLGSTRUCT
{
    DWORD   dwSize;                     // Size of this structure
    HWND    hParent;                    // Parent window handle
    DWORD   dwUserData;                 // User information.
    WORD    wHelpID;                    // Help ID to use.
    WORD    wHelpIDOverwrite;           // Help ID to use for Overwrite dlg
    WORD    wHelpIDBrowse;              // Help ID to use for Print Browse dlg
    BOOL    bWordWrap;                  // TRUE if word wrap print out
    char    szTitle [MAX_PRINT_TITLE + 1]; // Title of Print Dlg
    char    szFile [SYM_MAX_PATH + 1];  // Default file to print to
    PRINTDLGPROC lpfnBuild;             // Function to build the print info

    UINT    uHelpRightClickPrintFileButton;
    UINT    uHelpRightClickPrintPrinterButton;
    UINT    uHelpRightClickSendUserButton;
    UINT    uHelpRightClickPrintFileTunnel;
    UINT    uHelpRightClickPrintPrinterTunnel;

    LPDWORD lpdwHelpHowToPrintArray;    // Array of DWORDS for How To help.

} PRINTDLGSTRUCT, FAR * LPPRINTDLGSTRUCT;

PRINTERR SYM_EXPORT WINAPI NPTPrintDialog ( // Dlg to print a file
    LPPRINTDLGSTRUCT lpPrintDlgStruct );    // [in] print struct pointer

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

#ifdef __cplusplus
}
#endif

#endif
