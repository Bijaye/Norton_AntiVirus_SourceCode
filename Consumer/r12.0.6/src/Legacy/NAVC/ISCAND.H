// Copyright 1994-1996 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/navc/VCS/iscand.h_v   1.0   06 Feb 1997 20:56:36   RFULLER  $
//
// Description:
//      This include file contains the prototypes, constants and typedefs
//      for the NAV DOS scanner.
//************************************************************************
// $Log:   S:/navc/VCS/iscand.h_v  $
// 
//    Rev 1.0   06 Feb 1997 20:56:36   RFULLER
// Initial revision
// 
//    Rev 1.0   31 Dec 1996 15:25:36   MKEATIN
// Initial revision.
// 
//    Rev 1.0   02 Oct 1996 13:00:58   JBELDEN
// Initial revision.
//************************************************************************

#ifndef __NEWSCAND_H
#define __NEWSCAND_H

//************************************************************************
// DEFINES
//************************************************************************


//************************************************************************
// STRUCTS
//************************************************************************

                                        // Cookie passed to the NAVSCAN
                                        // scanning functions, holds DOS UI
                                        // specific info.
typedef struct tagDOSSCANINFO
    {
    DIALOG_RECORD *     lpScanFilesDialog;
    DWORD               dwLastDialogUpdate;

    char                szCurrentItem [SYM_MAX_PATH];
    } DOSSCANINFO, NEAR *NPDOSSCANINFO, *PDOSSCANINFO, FAR *LPDOSSCANINFO;


//************************************************************************
// PROTOTYPES FOR SCAND PROJECT FUNCTIONS
//************************************************************************

// Scanning DOS dialog functions.

VOID PASCAL OpenScanDialog (LPDOSSCANINFO lpDosInfo);
VOID PASCAL CloseScanDialog (LPDOSSCANINFO lpDosInfo);
VOID PASCAL UpdateScanDialog (LPDOSSCANINFO lpDosInfo,
                              LPNVSCAN      lpScan,
                              LPSCANITEM    lpScanItem);
VOID PASCAL ForceUpdateScanDialog (LPDOSSCANINFO lpDosInfo,
                                   LPNVSCAN      lpScan,
                                   LPSCANITEM    lpScanItem);

#endif  //  __NEWSCAND_H
