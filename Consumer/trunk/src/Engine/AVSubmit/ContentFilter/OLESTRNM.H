////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

//************************************************************************
//
// $Header:   S:/INCLUDE/VCS/olestrnm.h_v   1.5   31 Mar 1998 16:59:06   DCHI  $
//
// Description:
//  Header file for stream names
//
//************************************************************************
// $Log:   S:/INCLUDE/VCS/olestrnm.h_v  $
// 
//    Rev 1.5   31 Mar 1998 16:59:06   DCHI
// Added gabywsz1CompObj.
// 
//    Rev 1.4   26 Mar 1998 18:35:08   DCHI
// Added gabywsz5SummaryInformation and gabywsz5DocumentSummaryInformation.
// 
//    Rev 1.3   21 Nov 1997 15:35:52   DCHI
// Added "Current User".
// 
//    Rev 1.2   21 Nov 1997 11:26:08   DCHI
// Added "PowerPoint Document".
// 
//    Rev 1.1   30 May 1997 11:48:06   DCHI
// Added Book.
// 
//    Rev 1.0   05 May 1997 14:21:42   DCHI
// Initial revision.
// 
//************************************************************************

#ifndef _OLESTRNM_H_

#define _OLESTRNM_H_

#include "ssbstype.h"

extern BYTE FAR gabywszWordDocument[];          // WordDocument
extern BYTE FAR gabywszBook[];                  // Book
extern BYTE FAR gabywszVBA[];                   // VBA
extern BYTE FAR gabywszdir[];                   // dir
extern BYTE FAR gabywsz_VBA_PROJECT[];          // _VBA_PROJECT
extern BYTE FAR gabywszPROJECT[];               // PROJECT
extern BYTE FAR gabywszPROJECTwm[];             // PROJECTwm
extern BYTE FAR gabywsz1Table[];                // 1Table
extern BYTE FAR gabywsz0Table[];                // 0Table
extern BYTE FAR gabywszMacros[];                // Macros
extern BYTE FAR gabywszThisDocument[];          // ThisDocument
extern BYTE FAR gabywszWorkbook[];              // Workbook
extern BYTE FAR gabywsz_VBA_PROJECT_CUR[];      // _VBA_PROJECT_CUR
extern BYTE FAR gabywszThisWorkbook[];          // ThisWorkbook
extern BYTE FAR gabywsz__SRP_[];                // __SRP_
extern BYTE FAR gabywsz__SRP_0[];               // __SRP_0
extern BYTE FAR gabywszPowerPointDocument[];    // PowerPoint Document
extern BYTE FAR gabywszCurrentUser[];           // Current User
extern BYTE FAR gabywsz5SummaryInformation[];   // [5]SummaryInformation
extern BYTE FAR gabywsz5DocumentSummaryInformation[];   //[5]DocumentSummaryInformation
extern BYTE FAR gabywsz1CompObj[];              // [1]CompObj
extern BYTE FAR gabywszVBAProject[];            // VBAProject
extern BYTE FAR gabywszScripts[];               // Scripts
extern BYTE FAR gabywsz3DirData[];              // [3]DirData
extern BYTE FAR gabywszBlob[];                  // Blob
extern BYTE FAR gabywszPMLA[];                  // PROJECT_MACRO_LIBRARY_AREA
extern BYTE FAR gabywszBaseStorage[];           // BaseStorage
extern BYTE FAR gabywszMain[];                  // Main
extern BYTE FAR gabywszSRCAREA[];               // SRCAREA
extern BYTE FAR gabywszm[];                     // m
extern BYTE FAR gabywszz[];                     // z
extern BYTE FAR gabywszCVba[];                  // CVba
extern BYTE FAR gabywsz1Ole10Native[];          // [1]Ole10Native
extern BYTE FAR gabywszVisioDocument[];         // VisioDocument

#endif // _OLESTRNM_H_

