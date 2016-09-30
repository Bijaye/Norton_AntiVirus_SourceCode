//************************************************************************
//
// $Header:   S:/OLESSAPI/VCS/olestrnm.cpv   1.6   09 Nov 1998 13:42:26   DCHI  $
//
// Description:
//  File for stream name definitions.
//
//************************************************************************
// $Log:   S:/OLESSAPI/VCS/olestrnm.cpv  $
// 
//    Rev 1.6   09 Nov 1998 13:42:26   DCHI
// Added #pragma data_seg("FAR_DATA_0") for global FAR data.
// 
//    Rev 1.5   31 Mar 1998 16:59:38   DCHI
// Added gabywsz1CompObj.
// 
//    Rev 1.4   26 Mar 1998 18:35:42   DCHI
// Added gabywsz5SummaryInformation and gabywsz5DocumentSummaryInformation.
// 
//    Rev 1.3   21 Nov 1997 15:35:36   DCHI
// Added "Current User".
// 
//    Rev 1.2   21 Nov 1997 11:25:50   DCHI
// Added "PowerPoint Document".
// 
//    Rev 1.1   30 May 1997 11:47:58   DCHI
// Added Book.
// 
//    Rev 1.0   05 May 1997 14:21:54   DCHI
// Initial revision.
// 
//************************************************************************

#include "olestrnm.h"

#ifdef SYM_WIN16
#pragma data_seg("FAR_DATA_0")
#endif

// WordDocument

BYTE FAR gabywszWordDocument[] =
{
    'W', 0, 'o', 0, 'r', 0, 'd', 0,
    'D', 0, 'o', 0, 'c', 0, 'u', 0, 'm', 0, 'e', 0, 'n', 0, 't', 0,
    0, 0
};

// Book

BYTE FAR gabywszBook[] =
{
    'B', 0, 'o', 0, 'o', 0, 'k', 0, 0, 0
};

// VBA

BYTE FAR gabywszVBA[] = { 'V', 0, 'B', 0, 'A', 0, 0, 0 };

// dir

BYTE FAR gabywszdir[] = { 'd', 0, 'i', 0, 'r', 0, 0, 0 };

// _VBA_PROJECT

BYTE FAR gabywsz_VBA_PROJECT[] =
{
    '_', 0, 'V', 0, 'B', 0, 'A', 0,
    '_', 0, 'P', 0, 'R', 0, 'O', 0, 'J', 0, 'E', 0, 'C', 0, 'T', 0, 0, 0
};

// PROJECT

BYTE FAR gabywszPROJECT[] =
{
    'P', 0, 'R', 0, 'O', 0, 'J', 0, 'E', 0, 'C', 0, 'T', 0, 0, 0
};

// PROJECTwm

BYTE FAR gabywszPROJECTwm[] =
{
    'P', 0, 'R', 0, 'O', 0, 'J', 0, 'E', 0, 'C', 0, 'T', 0, 'w', 0, 'm', 0, 0, 0
};

// 1Table

BYTE FAR gabywsz1Table[] =
{
    '1', 0, 'T', 0, 'a', 0, 'b', 0, 'l', 0, 'e', 0, 0, 0
};

// 0Table

BYTE FAR gabywsz0Table[] =
{
    '0', 0, 'T', 0, 'a', 0, 'b', 0, 'l', 0, 'e', 0, 0, 0
};

// Macros

BYTE FAR gabywszMacros[] =
{
    'M', 0, 'a', 0, 'c', 0, 'r', 0, 'o', 0, 's', 0, 0, 0
};

// ThisDocument

BYTE FAR gabywszThisDocument[] =
{
    'T', 0, 'h', 0, 'i', 0, 's', 0,
    'D', 0, 'o', 0, 'c', 0, 'u', 0, 'm', 0, 'e', 0, 'n', 0, 't', 0,
    0, 0
};

// Workbook

BYTE FAR gabywszWorkbook[] =
{
    'W', 0, 'o', 0, 'r', 0, 'k', 0, 'b', 0, 'o', 0, 'o', 0, 'k', 0, 0, 0
};

// _VBA_PROJECT_CUR

BYTE FAR gabywsz_VBA_PROJECT_CUR[] =
{
    '_', 0, 'V', 0, 'B', 0, 'A', 0,
    '_', 0, 'P', 0, 'R', 0, 'O', 0, 'J', 0, 'E', 0, 'C', 0, 'T', 0,
    '_', 0, 'C', 0, 'U', 0, 'R', 0, 0, 0
};

// ThisWorkbook

BYTE FAR gabywszThisWorkbook[] =
{
    'T', 0, 'h', 0, 'i', 0, 's', 0,
    'W', 0, 'o', 0, 'r', 0, 'k', 0, 'b', 0, 'o', 0, 'o', 0, 'k', 0,
    0, 0
};

// __SRP_

BYTE FAR gabywsz__SRP_[] =
{
    '_', 0, '_', 0, 'S', 0, 'R', 0, 'P', 0, '_', 0, 0, 0
};

// __SRP_0

BYTE FAR gabywsz__SRP_0[] =
{
    '_', 0, '_', 0, 'S', 0, 'R', 0, 'P', 0, '_', 0, '0', 0, 0, 0
};

// PowerPoint Document

BYTE FAR gabywszPowerPointDocument[] =
{
    'P', 0, 'o', 0, 'w', 0, 'e', 0, 'r', 0,
    'P', 0, 'o', 0, 'i', 0, 'n', 0, 't', 0, ' ', 0,
    'D', 0, 'o', 0, 'c', 0, 'u', 0, 'm', 0, 'e', 0, 'n', 0, 't', 0, 0, 0
};

// Current User

BYTE FAR gabywszCurrentUser[] =
{
    'C', 0, 'u', 0, 'r', 0, 'r', 0, 'e', 0, 'n', 0, 't', 0, ' ', 0,
    'U', 0, 's', 0, 'e', 0, 'r', 0, 0, 0
};

// [5]SummaryInformation

BYTE FAR gabywsz5SummaryInformation[] =
{
    0x05, 0,
    'S', 0, 'u', 0, 'm', 0, 'm', 0, 'a', 0, 'r', 0, 'y', 0,
    'I', 0, 'n', 0, 'f', 0, 'o', 0, 'r', 0, 'm', 0, 'a', 0, 't', 0, 'i', 0, 'o', 0, 'n', 0,
    0, 0
};

// [5]DocumentSummaryInformation

BYTE FAR gabywsz5DocumentSummaryInformation[] =
{
    0x05, 0,
    'D', 0, 'o', 0, 'c', 0, 'u', 0, 'm', 0, 'e', 0, 'n', 0, 't', 0,
    'S', 0, 'u', 0, 'm', 0, 'm', 0, 'a', 0, 'r', 0, 'y', 0,
    'I', 0, 'n', 0, 'f', 0, 'o', 0, 'r', 0, 'm', 0, 'a', 0, 't', 0, 'i', 0, 'o', 0, 'n', 0,
    0, 0
};

// [1]CompObj

BYTE FAR gabywsz1CompObj[] =
{
    0x01, 0, 'C', 0, 'o', 0, 'm', 0, 'p', 0, 'O', 0, 'b', 0, 'j', 0, 0, 0
};

#ifdef SYM_WIN16
#pragma data_seg()
#endif

