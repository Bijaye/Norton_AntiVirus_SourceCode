// Copyright 1998 Symantec, SABU
//************************************************************************
//
// $Header:   S:/NAVEX/VCS/msxl2.cpv   1.0   08 Dec 1998 12:53:10   DCHI  $
//
// Description:
//  Contains the following functions:
//
//      int MSXL2CB()
//      int MSXL2_VBA_PROJECT()
//      int MSXL2VBA()
//      int MSXL2Filter()
//
//************************************************************************
// $Log:   S:/NAVEX/VCS/msxl2.cpv  $
// 
//    Rev 1.0   08 Dec 1998 12:53:10   DCHI
// Initial revision.
// 
//************************************************************************

#include "olessapi.h"
#include "olestrnm.h"
#include "crc32.h"
#include "wd7api.h"
#include "xl5api.h"
#include "o97api.h"

#include "msxi.h"

typedef struct tagMSXL2INFO
{
    int             nType;
    DWORD           dwID;
    DWORD           dwChildID;
} MSXL2INFO_T, FAR *LPMSXL2INFO;

//********************************************************************
//
// Function:
//  int MSXL2CB()
//
// Parameters:
//  lpstEntry           Ptr to the entry
//  dwIndex             The entry's index in the directory
//  lpvCookie           Cookie ptr
//
// Description:
//  If the entry is that of a storage named one of the following,
//  then the function returns the return result:
//
//      1. _VBA_PROJECT
//      2. VBA
//
//  Otherwise, the function returns the CONTINUE result.
//
// Returns:
//  SS_ENUM_CB_STATUS_RETURN        If an interesting storage is found
//  SS_ENUM_CB_STATUS_CONTINUE      Otherwise
//
//********************************************************************

int MSXL2CB
(
    LPSS_DIR_ENTRY      lpstEntry,
    DWORD               dwIndex,
    LPVOID              lpvCookie
)
{
    if (lpstEntry->byMSE == STGTY_STORAGE)
    {
        LPMSXL2INFO     lpstInfo;

        lpstInfo = (LPMSXL2INFO)lpvCookie;

        // Check for "_VBA_PROJECT"

        if (SSWStrNCmp(lpstEntry->uszName,
                       (LPWORD)gabywsz_VBA_PROJECT,
                       SS_MAX_NAME_LEN) == 0)
        {
            lpstInfo->nType = MSX_L2__VBA_PROJECT;
            lpstInfo->dwID = dwIndex;
            lpstInfo->dwChildID =
                DWENDIAN(lpstEntry->dwSIDChild);
            return(SS_ENUM_CB_STATUS_RETURN);
        }

        // Check for "VBA"

        if (SSWStrNCmp(lpstEntry->uszName,
                       (LPWORD)gabywszVBA,
                       SS_MAX_NAME_LEN) == 0)
        {
            lpstInfo->nType = MSX_L2_VBA;
            lpstInfo->dwID = dwIndex;
            lpstInfo->dwChildID =
                DWENDIAN(lpstEntry->dwSIDChild);
            return(SS_ENUM_CB_STATUS_RETURN);
        }
    }

    return(SS_ENUM_CB_STATUS_CONTINUE);
}


//********************************************************************
//
// Function:
//  int MSXL2_VBA_PROJECT()
//
// Parameters:
//  lpstMSX             Ptr to MSX_T structure
//  lpstRoot            Ptr to root structure
//  lpstStream          Ptr to stream structure
//  dwStorageID         ID of _VBA_PROJECT storage
//  dwChildID           ID of child of storage
//  lpstSibs            Ptr to sibling enumeration structure to use
//
// Description:
//  The function does the following:
//
// Returns:
//  MSX_NORMAL_SCAN     If the CRC did not match
//  MSX_ALL_CLEAN       If the CRC is in the database
//
//********************************************************************

int MSXL2_VBA_PROJECT
(
    LPMSX               lpstMSX,
    LPSS_ROOT           lpstRoot,
    LPSS_STREAM         lpstStream,
    DWORD               dwStorageID,
    DWORD               dwChildID,
    LPSS_ENUM_SIBS      lpstSibs
)
{
    DWORD               dwCRC;
    DWORD               dwCRCSize;

    // Compute the CRC on the streams

    if (MSXL2_VBA_PROJECTCRC(lpstRoot,
                             lpstStream,
                             dwChildID,
                             lpstSibs,
                             lpstMSX->lpabyWorkBuf,
                             lpstMSX->nWorkBufSize,
                             &dwCRC,
                             &dwCRCSize) == FALSE)
        return(MSX_NORMAL_SCAN);

    // Check the database

    if (MSXDBL2Match(lpstMSX,
                     dwCRC,
                     dwCRCSize) == FALSE)
        return(MSX_NORMAL_SCAN);

    return(MSX_ALL_CLEAN);
}


//********************************************************************
//
// Function:
//  int MSXL2VBA()
//
// Parameters:
//  lpstMSX             Ptr to MSX_T structure
//  lpstRoot            Ptr to root structure
//  lpstStream          Ptr to stream structure
//  dwStorageID         ID of VBA storage
//  dwChildID           ID of child of storage
//  lpstSibs            Ptr to sibling enumeration structure to use
//
// Description:
//  The function does the following:
//
// Returns:
//  MSX_NORMAL_SCAN     If the CRC did not match
//  MSX_ALL_CLEAN       If the CRC is in the database
//
//********************************************************************

int MSXL2VBA
(
    LPMSX               lpstMSX,
    LPSS_ROOT           lpstRoot,
    LPSS_STREAM         lpstStream,
    DWORD               dwStorageID,
    DWORD               dwChildID,
    LPSS_ENUM_SIBS      lpstSibs
)
{
    DWORD               dwCRC;
    DWORD               dwCRCSize;

    // Compute the CRC on the streams

    if (MSXL2VBACRC(lpstRoot,
                    lpstStream,
                    dwChildID,
                    lpstSibs,
                    lpstMSX->lpabyWorkBuf,
                    lpstMSX->nWorkBufSize,
                    &dwCRC,
                    &dwCRCSize) == FALSE)
        return(MSX_NORMAL_SCAN);

    // Check the database

    if (MSXDBL2Match(lpstMSX,
                     dwCRC,
                     dwCRCSize) == FALSE)
        return(MSX_NORMAL_SCAN);

    return(MSX_ALL_CLEAN);
}


//********************************************************************
//
// Function:
//  int MSXL2Filter()
//
// Parameters:
//  lpstRoot            Ptr to root structure
//  lpstStream          Ptr to stream structure to use
//
// Description:
//  The function looks for storages with the following names:
//
//      1. _VBA_PROJECT
//      2. VBA
//
//  If all streams are clean and there were no CRC matches,
//  assume the file is clean.
//
//  If all streams are clean and there all CRCs are matches,
//  assume perform second-level filter.
//
//  Otherwise, perform normal scan.
//
// Returns:
//  MSX_ALL_CLEAN       If all streams are clean
//  MSX_NORMAL_SCAN     If there was a CRC that did not match
//
//********************************************************************

int MSXL2Filter
(
    LPMSX               lpstMSX,
    LPSS_ROOT           lpstRoot,
    LPSS_STREAM         lpstStream
)
{
    int                 i;
    int                 nResult;
    DWORD               dwDirID;
    MSXL2INFO_T         stL2Info;
    LPSS_ENUM_SIBS      lpstSibs;

    // Allocate a sibling enumeration structure

    if (SSAllocEnumSibsStruct(lpstRoot,
                              &lpstSibs,
                              SS_DEF_MAX_WAITING_SIBS) != SS_STATUS_OK)
        return(MSX_NORMAL_SCAN);

    nResult = MSX_ALL_CLEAN;
    dwDirID = 0;
    for (i=0;i<16384;i++)
    {
        if (SSEnumDirEntriesCB(lpstRoot,
                               MSXL2CB,
                               &stL2Info,
                               &dwDirID,
                               lpstStream) != SS_STATUS_OK)
            break;

        switch (stL2Info.nType)
        {
            case MSX_L2__VBA_PROJECT:
                nResult = MSXL2_VBA_PROJECT(lpstMSX,
                                            lpstRoot,
                                            lpstStream,
                                            stL2Info.dwID,
                                            stL2Info.dwChildID,
                                            lpstSibs);
                break;

            case MSX_L2_VBA:
                nResult = MSXL2VBA(lpstMSX,
                                   lpstRoot,
                                   lpstStream,
                                   stL2Info.dwID,
                                   stL2Info.dwChildID,
                                   lpstSibs);
                break;

            default:
                nResult = MSX_NORMAL_SCAN;
                break;
        }

        // Return immediately if the result is MSX_NORMAL_SCAN.

        if (nResult == MSX_NORMAL_SCAN)
            break;
    }

    // Result should be either MSX_ALL_CLEAN or MSX_NORMAL_SCAN

    SSFreeEnumSibsStruct(lpstRoot,lpstSibs);
    return(nResult);
}   


