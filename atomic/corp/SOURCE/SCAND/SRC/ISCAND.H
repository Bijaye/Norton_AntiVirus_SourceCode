// Copyright 1994 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/scand/VCS/iscand.h_v   1.0   06 Feb 1997 21:08:46   RFULLER  $
//
// Description:
//      This include file contains the prototypes, constants and typedefs
//      for the NAV DOS scanner.
//************************************************************************
// $Log:   S:/scand/VCS/iscand.h_v  $
// 
//    Rev 1.0   06 Feb 1997 21:08:46   RFULLER
// Initial revision
// 
//    Rev 1.1   08 Aug 1996 13:28:34   JBRENNA
// Ported from DOSNAV environment
// 
//    Rev 1.0   25 Aug 1994 14:48:34   DALLEE
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
