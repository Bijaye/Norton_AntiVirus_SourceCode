//************************************************************************
//
// $Header:   S:/OLESSAPI/VCS/o97wdapi.cpv   1.11   08 Dec 1998 12:41:30   DCHI  $
//
// Description:
//  Core Office 97 WordDocument access implementation.
//
//************************************************************************
// $Log:   S:/OLESSAPI/VCS/o97wdapi.cpv  $
// 
//    Rev 1.11   08 Dec 1998 12:41:30   DCHI
// - New TDT parsing code.
// - Added gabyLZNTBlankThisDocumentSrc[] and modified O97WDDeleteModule()
//   to replace "ThisDocument" with blank source.
// 
//    Rev 1.10   12 Oct 1998 13:12:32   DCHI
// Change in O97WDCheckMacrosDeletable(), so that it returns TRUE if no
// WordDocument stream is found.
// 
//    Rev 1.9   11 Aug 1998 09:44:32   DCHI
// Changed a few fatal returns to nonfatal returns in O97WDTDTCheckModuleString().
// 
//    Rev 1.8   24 Nov 1997 17:32:26   DCHI
// Modified O97WDOpenTDT() to use FIB[11] to determine whether to use
// 1Table or 0Table.
// 
//    Rev 1.7   04 Nov 1997 10:30:08   DCHI
// Corrected comments in functions that parse record 0x07.
// 
//    Rev 1.6   31 Oct 1997 17:56:40   DCHI
// Fixed incorrect declaration of wLen as a BYTE
// in O97WDUpdateRecord0x07AfterMove().  Correct type is a WORD.
// 
//    Rev 1.5   14 Oct 1997 17:34:54   DCHI
// Added proper comparison of non-English module names to
// O97WDTDTCheckModuleString().
// 
//    Rev 1.4   06 Oct 1997 14:13:58   DCHI
// Modified O97WDOpenTDT() to verify that last record of TDT is 0x40
// before accepting as true TDT.
// 
//    Rev 1.3   13 Aug 1997 16:59:40   DCHI
// Fixed memory leak in certain case in CheckMacrosDeleteable function.
// 
//    Rev 1.2   15 May 1997 12:04:38   DCHI
// Modified so that CheckMacrosDeletable function does not need the
// presence of ThisDocument to succeed.
// 
//    Rev 1.1   08 May 1997 12:55:50   DCHI
// Added HasAutoText() and ChangeToDocument() functions.
// 
//************************************************************************

#include "o97api.h"
#include "olestrnm.h"

//********************************************************************
//
// Function:
//  int O97WDFindAssociatedStreamsCB()
//
// Description:
//  Searches for the streams WordDocument, 1Table, and the storage
//  Macros and sets the appropriate entry field in the
//  O97WD_FIND_STREAMS_T structure if found.
//
// Returns:
//  SS_ENUM_CB_STATUS_CONTINUE      Always
//
//********************************************************************

typedef struct tagO97WD_FIND_STREAMS
{
    DWORD       dwWordDocumentEntry;
    DWORD       dw1TableEntry;
    DWORD       dw0TableEntry;
    DWORD       dwMacrosEntry;
} O97WD_FIND_STREAMS_T, FAR *LPO97WD_FIND_STREAMS;

int O97WDFindAssociatedStreamsCB
(
    LPSS_DIR_ENTRY      lpstEntry,  // Ptr to the entry
    DWORD               dwIndex,    // The entry's index in the directory
    LPVOID              lpvCookie
)
{
    if (lpstEntry->byMSE == STGTY_STREAM)
    {
        // Is it the WordDocument stream?

        if (SSWStrNCmp(lpstEntry->uszName,
                       (LPWORD)gabywszWordDocument,
                       SS_MAX_NAME_LEN) == 0)
        {
            ((LPO97WD_FIND_STREAMS)lpvCookie)->dwWordDocumentEntry =
                dwIndex;
        }
        else

        // Is it the 1Table stream?

        if (SSWStrNCmp(lpstEntry->uszName,
                       (LPWORD)gabywsz1Table,
                       SS_MAX_NAME_LEN) == 0)
        {
            ((LPO97WD_FIND_STREAMS)lpvCookie)->dw1TableEntry =
                dwIndex;
        }
        else

        // Is it the 0Table stream?

        if (SSWStrNCmp(lpstEntry->uszName,
                       (LPWORD)gabywsz0Table,
                       SS_MAX_NAME_LEN) == 0)
        {
            ((LPO97WD_FIND_STREAMS)lpvCookie)->dw0TableEntry =
                dwIndex;
        }
    }
    else
    if (lpstEntry->byMSE == STGTY_STORAGE)
    {
        // Is it the <Macros> storage?

        if (SSWStrNCmp(lpstEntry->uszName,
                       (LPWORD)gabywszMacros,
                       SS_MAX_NAME_LEN) == 0)
        {
            ((LPO97WD_FIND_STREAMS)lpvCookie)->dwMacrosEntry =
                dwIndex;
        }
    }

    return(SS_ENUM_CB_STATUS_CONTINUE);
}


//********************************************************************
//
// Function:
//  BOOL O97WDFindAssociatedStreams()
//
// Description:
//  The following fields of lpstO97Scan should be valid before
//  entering this function:
//
//          dwMacrosEntry
//          dwMacrosChildEntry
//          dwVBAEntry
//          dwVBAChildEntry
//
//  The function will fill in the following fields:
//
//          dwParentStorageEntry
//          dwWordDocumentEntry
//          dw1TableEntry
//          dw0TableEntry
//          dwPROJECTEntry
//          dwPROJECTwmEntry
//          dw_VBA_PROJECTEntry
//          dwdirEntry
//
//  The following algorithm is used:
//      1. Search for the WordDocument and Table streams:
//          For each substorage do the following:
//              Iterate through each sibling:
//                  Take note of the WordDocument stream index
//                  Take note of the Table stream index
//                  Take note of the Macros substorage index
//              If all three are present and the index of the
//              Macros substorage is the same as the one passed
//              in, then we are done.
//          If the associated WordDocument stream or Table stream
//              can not be found, then return failure.
//      2. Search for the PROJECT, PROJECTwm, _VBA_PROJECT, and
//          dir streams by calling the shared function to do so.
//
// Returns:
//  TRUE        If all of the associated streams were found
//  FALSE       If any of the streams could not be found
//
//********************************************************************

BOOL O97WDFindAssociatedStreams
(
    LPSS_ROOT               lpstRoot,
    LPO97_SCAN              lpstO97Scan
)
{
    BOOL                    bFound;
    O97WD_FIND_STREAMS_T    stFindStreams;
    DWORD                   dwStorageChildID;
    DWORD                   dwSearchID;
    LPSS_ENUM_SIBS          lpstSibs;

    // Allocate a sibling enumeration structure

    if (SSAllocEnumSibsStruct(lpstRoot,
                              &lpstSibs,
                              SS_DEF_MAX_WAITING_SIBS) != SS_STATUS_OK)
    {
        return(FALSE);
    }

    /////////////////////////////////////////////////////////////
    // Find Word application specific streams
    /////////////////////////////////////////////////////////////

    // Go through all storages

    bFound = FALSE;
    dwSearchID = 0;
    while (SSEnumDirEntriesCB(lpstRoot,
                              O97EnumStorageEntriesCB,
                              &dwStorageChildID,
                              &dwSearchID,
                              NULL) == SS_STATUS_OK)
    {
        // Found a storage, so go through children looking
        //  for what we need

        SSInitEnumSibsStruct(lpstSibs,
                             dwStorageChildID);

        stFindStreams.dwWordDocumentEntry = 0xFFFFFFFF;
        stFindStreams.dw1TableEntry = 0xFFFFFFFF;
        stFindStreams.dw0TableEntry = 0xFFFFFFFF;
        stFindStreams.dwMacrosEntry = 0xFFFFFFFF;
        SSEnumSiblingEntriesCB(lpstRoot,
                               O97WDFindAssociatedStreamsCB,
                               &stFindStreams,
                               lpstSibs,
                               NULL);

        if (stFindStreams.dwMacrosEntry ==
                lpstO97Scan->u.stWD8.dwMacrosEntry &&
            stFindStreams.dwWordDocumentEntry != 0xFFFFFFFF &&
            stFindStreams.dw1TableEntry != 0xFFFFFFFF)
        {
            // Also store the parent storage's entry number

            lpstO97Scan->u.stWD8.dwParentStorageEntry =
                dwSearchID - 1;

            bFound = TRUE;
            break;
        }
    }

    if (bFound == FALSE)
    {
        // Failed finding associated Word application streams

        SSFreeEnumSibsStruct(lpstRoot,lpstSibs);
        return(FALSE);
    }

    // Set the found entries in the VBA 5 scan structure

    lpstO97Scan->u.stWD8.dwWordDocumentEntry =
        stFindStreams.dwWordDocumentEntry;

    lpstO97Scan->u.stWD8.dw1TableEntry = stFindStreams.dw1TableEntry;
    lpstO97Scan->u.stWD8.dw0TableEntry = stFindStreams.dw0TableEntry;


    /////////////////////////////////////////////////////////////
    // Find VBA 5 specific streams
    /////////////////////////////////////////////////////////////

    SSInitEnumSibsStruct(lpstSibs,
                         lpstO97Scan->u.stWD8.dwMacrosChildEntry);

    if (O97FindVBAStreams(lpstRoot,
                          lpstSibs,
                          lpstO97Scan) == FALSE)
    {
        // Failed finding associated VBA streams

        SSFreeEnumSibsStruct(lpstRoot,lpstSibs);
        return(FALSE);
    }

    // Successfully got the indices of all important streams

    SSFreeEnumSibsStruct(lpstRoot,lpstSibs);
    return(TRUE);
}


//********************************************************************
//
// Function:
//  int O97WDCheckMacrosDeletableCB()
//
// Description:
//  The callback checks the name and does the following:
//      1. If the name is "", "_*", or "dir", the function just
//         returns CONTINUE.
//      2. If the name is not "ThisDocument", the function
//         returns OPEN.
//
// Returns:
//  SS_ENUM_CB_STATUS_OPEN          If the entry is a candidate
//  SS_ENUM_CB_STATUS_CONTINUE      If the entry is not
//
//********************************************************************

int O97WDCheckMacrosDeletableCB
(
    LPSS_DIR_ENTRY      lpstEntry,  // Ptr to the entry
    DWORD               dwIndex,    // The entry's index in the directory
    LPVOID              lpvCookie
)
{
    (void)dwIndex;
    (void)lpvCookie;

    if (lpstEntry->byMSE == STGTY_STREAM)
    {
        // Is it any of "", "_*", or "dir"

        if (WENDIAN(lpstEntry->uszName[0]) == '_' ||
            lpstEntry->uszName[0] == 0 ||
            SSWStrNCmp(lpstEntry->uszName,
                       (LPWORD)gabywszdir,
                       SS_MAX_NAME_LEN) == 0)
        {
            return(SS_ENUM_CB_STATUS_CONTINUE);
        }

        // It is some other module name besides "ThisDocument"

        if (SSWStrNCmp(lpstEntry->uszName,
                       (LPWORD)gabywszThisDocument,
                       SS_MAX_NAME_LEN) != 0)
        {
            return(SS_ENUM_CB_STATUS_OPEN);
        }
    }

    return(SS_ENUM_CB_STATUS_CONTINUE);
}


//********************************************************************
//
// Function:
//  int O97WDOpenThisDocumentCB()
//
// Description:
//  Callback for opening "ThisDocument" stream.
//
// Returns:
//  SS_ENUM_CB_STATUS_OPEN      If the entry is a candidate
//  SS_ENUM_CB_STATUS_CONTINUE  If the entry is not
//
//********************************************************************

int O97WDOpenThisDocumentCB
(
    LPSS_DIR_ENTRY      lpstEntry,  // Ptr to the entry
    DWORD               dwIndex,    // The entry's index in the directory
    LPVOID              lpvCookie
)
{
    (void)dwIndex;
    (void)lpvCookie;

    if (lpstEntry->byMSE == STGTY_STREAM)
    {
        if (SSWStrNCmp(lpstEntry->uszName,
                       (LPWORD)gabywszThisDocument,
                       SS_MAX_NAME_LEN) == 0)
        {
            return(SS_ENUM_CB_STATUS_OPEN);
        }
    }

    return(SS_ENUM_CB_STATUS_CONTINUE);
}


//********************************************************************
//
// Function:
//  BOOL O97WDRec0x07ParseString()
//
// Parameters:
//  lpstStream          Ptr to Table stream structure
//  lpdwOffset          Offset of string
//
// Description:
//  The function parses through the string at the given offset.
//  The string is assumed to have the following structure:
//
//      byStrLen
//      awStr[byStrLen]
//
// Returns:
//  TRUE        If successful
//  FALSE       If unsuccessful
//
//********************************************************************

BOOL O97WDRec0x07ParseString
(
    LPSS_STREAM     lpstStream,
    LPDWORD         lpdwOffset
)
{
    DWORD           dwCount;
    BYTE            byStrLen;

    // byStrLen
    // awStr[byStrLen]

    if (SSSeekRead(lpstStream,
                   *lpdwOffset,
                   &byStrLen,
                   sizeof(BYTE),
                   &dwCount) != SS_STATUS_OK ||
        dwCount != sizeof(BYTE))
        return(FALSE);

    *lpdwOffset += sizeof(BYTE) + byStrLen * (DWORD)2;
    return(TRUE);
}


//********************************************************************
//
// Function:
//  BOOL O97WDRec0x07ParseButton()
//
// Parameters:
//  lpstStream          Ptr to Table stream structure
//  lpdwOffset          Offset of control subrecord
//
// Description:
//  The function parses through the button control subrecord
//  at the given offset.  The offset following the last
//  byte of the subrecord is stored at *lpdwOffset.
//
//  The control is assumed to have the following structure:
//
//      bybFlags
//      if (bybFlags & 0x08)
//      {
//          dwcbDIB0
//          abyDIB0[dwcbDIB0 - 10]
//          dwcbDIB1
//          abyDIB1[dwcbDIB1 - 10]
//      }
//      if (bybFlags & 0x10)
//          abyUnknown[2]
//      if (bybFlags & 0x04)
//      {
//          byAcceleratorLen
//          awAccelerator[byAcceleratorLen]
//      }
//
// Returns:
//  TRUE        If successful
//  FALSE       If unsuccessful
//
//********************************************************************

BOOL O97WDRec0x07ParseButton
(
    LPSS_STREAM     lpstStream,
    LPDWORD         lpdwOffset
)
{
    DWORD           dwOffset;
    DWORD           dwCount;
    BYTE            bybFlags;

    dwOffset = *lpdwOffset;

    // Read bFlags

    if (SSSeekRead(lpstStream,
                   dwOffset,
                   &bybFlags,
                   sizeof(BYTE),
                   &dwCount) != SS_STATUS_OK ||
        dwCount != sizeof(BYTE))
        return(FALSE);

    dwOffset += sizeof(BYTE);

    if (bybFlags & 0x08)
    {
        DWORD   dwcbDib;

        // dwcbDIB0
        // abyDIB0[dwcbDIB0 - 10]

        if (SSSeekRead(lpstStream,
                       dwOffset,
                       &dwcbDib,
                       sizeof(DWORD),
                       &dwCount) != SS_STATUS_OK ||
            dwCount != sizeof(DWORD))
            return(FALSE);

        dwcbDib = DWENDIAN(dwcbDib);
        dwOffset += sizeof(DWORD) + dwcbDib - 10;

        // dwcbDIB1
        // abyDIB1[dwcbDIB1 - 10]

        if (SSSeekRead(lpstStream,
                       dwOffset,
                       &dwcbDib,
                       sizeof(DWORD),
                       &dwCount) != SS_STATUS_OK ||
            dwCount != sizeof(DWORD))
            return(FALSE);

        dwcbDib = DWENDIAN(dwcbDib);
        dwOffset += sizeof(DWORD) + dwcbDib - 10;
    }

    if (bybFlags & 0x10)
    {
        // abyUnknown[2]

        dwOffset += 2;
    }

    if (bybFlags & 0x04)
    {
        // byAcceleratorLen
        // awAccelerator[byAcceleratorLen]

        if (O97WDRec0x07ParseString(lpstStream,&dwOffset) == FALSE)
            return(FALSE);
    }

    *lpdwOffset = dwOffset;
    return(TRUE);
}


//********************************************************************
//
// Function:
//  BOOL O97WDRec0x07ParseDropdown()
//
// Parameters:
//  lpstStream          Ptr to Table stream structure
//  lpdwOffset          Offset of control subrecord
//
// Description:
//  The function parses through the dropdown control subrecord
//  at the given offset.  The offset following the last
//  byte of the subrecord is stored at *lpdwOffset.
//
//  The control is assumed to have the following structure:
//
//      if (tcid == 1)
//      {
//          wcwtz
//          repeat wcwtz times
//          {
//              byItemLen
//              awItem[byItemLen]
//          }
//          abyUnknown[8]
//          byStrLen
//          awStr[byStrLen]
//      }
//
// Returns:
//  TRUE        If successful
//  FALSE       If unsuccessful
//
//********************************************************************

BOOL O97WDRec0x07ParseDropdown
(
    LPSS_STREAM     lpstStream,
    LPDWORD         lpdwOffset,
    WORD            wtcid
)
{
    DWORD           dwOffset;
    DWORD           dwCount;
    WORD            w;

    dwOffset = *lpdwOffset;

    if (wtcid == 1)
    {
        WORD        wcwtz;

        if (SSSeekRead(lpstStream,
                       dwOffset,
                       &wcwtz,
                       sizeof(WORD),
                       &dwCount) != SS_STATUS_OK ||
            dwCount != sizeof(WORD))
            return(FALSE);

        wcwtz = WENDIAN(wcwtz);
        dwOffset += sizeof(WORD);

        // repeat wcwtz times
        //     byItemLen
        //     awItem[byItemLen]

        for (w=0;w<wcwtz;w++)
        {
            if (O97WDRec0x07ParseString(lpstStream,&dwOffset) == FALSE)
                return(FALSE);
        }

        // abyUnknown[8]

        dwOffset += 8;

        // byStrLen
        // awStr[byStrLen]

        if (O97WDRec0x07ParseString(lpstStream,&dwOffset) == FALSE)
            return(FALSE);
    }

    *lpdwOffset = dwOffset;
    return(TRUE);
}


//********************************************************************
//
// Function:
//  BOOL O97WDRec0x07ParseMenu()
//
// Parameters:
//  lpstStream          Ptr to Table stream structure
//  lpdwOffset          Offset of control subrecord
//
// Description:
//  The function parses through the menu control subrecord
//  at the given offset.  The offset following the last
//  byte of the subrecord is stored at *lpdwOffset.
//
//  The control is assumed to have the following structure:
//
//      dwtbid
//      if (dwtbid == 1)
//      {
//          byMenuNameLen
//          awMenuName[byMenuNameLen]
//      }
//
// Returns:
//  TRUE        If successful
//  FALSE       If unsuccessful
//
//********************************************************************

BOOL O97WDRec0x07ParseMenu
(
    LPSS_STREAM     lpstStream,
    LPDWORD         lpdwOffset
)
{
    DWORD           dwOffset;
    DWORD           dwCount;
    DWORD           dwtbid;

    dwOffset = *lpdwOffset;

    if (SSSeekRead(lpstStream,
                   dwOffset,
                   &dwtbid,
                   sizeof(DWORD),
                   &dwCount) != SS_STATUS_OK ||
        dwCount != sizeof(DWORD))
        return(FALSE);

    dwtbid = DWENDIAN(dwtbid);
    dwOffset += sizeof(DWORD);

    if (dwtbid == 1)
    {
        // byMenuNameLen
        // awMenuName[byMenuNameLen]

        if (O97WDRec0x07ParseString(lpstStream,&dwOffset) == FALSE)
            return(FALSE);
    }

    *lpdwOffset = dwOffset;
    return(TRUE);
}


//********************************************************************
//
// Function:
//  BOOL O97WDRec0x07ParseControl()
//
// Parameters:
//  lpstStream          Ptr to Table stream structure
//  lpdwOffset          Offset of control subrecord
//
// Description:
//  The function parses through the control subrecord
//  at the given offset.  The offset following the last
//  byte of the subrecord is stored at *lpdwOffset.
//
//  The control is assumed to have the following structure:
//
//      abyHdr0[2]
//      byFlags
//      bytct
//      wtcid
//      abyHdr1[5]
//      if (byFlags & 0x10)
//          abyHdr2[4]
//      if (tcid != 1)
//          abyHdr3[4]
//      bybFlags
//      if (bybFlags & 0x01)
//      {
//          byLabelLen
//          awLabel[byLabelLen]
//      }
//      if (bybFlags & 0x02)
//      {
//          byStatusLen
//          awStatus[byStatusLen]
//          byTooltipLen
//          awTooltip[byTooltipLen]
//      }
//      if (bybFlags & 0x04)
//      {
//          byHelpfileLen
//          awHelpfile[byHelpfileLen]
//          abyExtra0[4]
//          byTagLen
//          awTag[byTagLen]
//          byMacroNameLen
//          awMacroName[byMacroNameLen]
//          byParameterLen
//          awParameter[byParameterLen]
//          abyExtra1[2]
//      }
//      switch (bytct)
//      {
//          case 1, 16, 17:
//              ParseButton()
//          case 2, 3, 4, 5, 6, 7, 8, 9, 20:
//              ParseDropdown()
//          case 10, 11, 12, 13, 14:
//              ParseMenu()
//          case 18, 19:
//              // nothing to do here
//          default
//              // error
//      }
//
// Returns:
//  TRUE        If successful
//  FALSE       If unsuccessful
//
//********************************************************************

BOOL O97WDRec0x07ParseControl
(
    LPSS_STREAM     lpstStream,
    LPDWORD         lpdwOffset
)
{
    DWORD           dwOffset;
    DWORD           dwCount;
    BYTE            byFlags;
    BYTE            bytct;
    WORD            wtcid;
    BYTE            bybFlags;

    dwOffset = *lpdwOffset;

    // abyHdr0[2]

    dwOffset += 2;

    // byFlags

    if (SSSeekRead(lpstStream,
                   dwOffset,
                   &byFlags,
                   sizeof(BYTE),
                   &dwCount) != SS_STATUS_OK ||
        dwCount != sizeof(BYTE))
        return(FALSE);

    dwOffset += sizeof(BYTE);

    // bytct: Control type

    if (SSSeekRead(lpstStream,
                   dwOffset,
                   &bytct,
                   sizeof(BYTE),
                   &dwCount) != SS_STATUS_OK ||
        dwCount != sizeof(BYTE))
        return(FALSE);

    dwOffset += sizeof(BYTE);

    // Read tcid: Control ID

    if (SSSeekRead(lpstStream,
                   dwOffset,
                   &wtcid,
                   sizeof(WORD),
                   &dwCount) != SS_STATUS_OK ||
        dwCount != sizeof(WORD))
        return(FALSE);

    wtcid = WENDIAN(wtcid);
    dwOffset += sizeof(WORD);

    // abyHdr1[5]

    dwOffset += 5;

    if (byFlags & 0x10)
    {
        // abyHdr2[4]

        dwOffset += 4;
    }

    if (wtcid != 1)
    {
        // abyHdr3[4]

        dwOffset += 4;
    }

    // Read bFlags

    if (SSSeekRead(lpstStream,
                   dwOffset,
                   &bybFlags,
                   sizeof(BYTE),
                   &dwCount) != SS_STATUS_OK ||
        dwCount != sizeof(BYTE))
        return(FALSE);

    dwOffset += sizeof(BYTE);

    if (bybFlags & 0x01)
    {
        // byLabelLen
        // awLabel[byLabelLen]

        if (O97WDRec0x07ParseString(lpstStream,&dwOffset) == FALSE)
            return(FALSE);
    }

    if (bybFlags & 0x02)
    {
        // byStatusLen
        // awStatus[byStatusLen]

        if (O97WDRec0x07ParseString(lpstStream,&dwOffset) == FALSE)
            return(FALSE);

        // byTooltipLen
        // awTooltip[byTooltipLen]

        if (O97WDRec0x07ParseString(lpstStream,&dwOffset) == FALSE)
            return(FALSE);
    }

    if (bybFlags & 0x04)
    {
        // byHelpfileLen
        // awHelpfile[byHelpfileLen]

        if (O97WDRec0x07ParseString(lpstStream,&dwOffset) == FALSE)
            return(FALSE);

        // abyExtra0[4]

        dwOffset += 4;

        // byTagLen
        // awTag[byTagLen]

        if (O97WDRec0x07ParseString(lpstStream,&dwOffset) == FALSE)
            return(FALSE);

        // byMacroNameLen
        // awMacroName[byMacroNameLen]

        if (O97WDRec0x07ParseString(lpstStream,&dwOffset) == FALSE)
            return(FALSE);

        // byParameterLen
        // awParameter[byParameterLen]

        if (O97WDRec0x07ParseString(lpstStream,&dwOffset) == FALSE)
            return(FALSE);

        // abyExtra1[2]

        dwOffset += 2;
    }

    // Switch on control type

    switch (bytct)
    {
        case 1:
        case 16:
        case 17:
            if (O97WDRec0x07ParseButton(lpstStream,
                                        &dwOffset) == FALSE)
                return(FALSE);

            break;

        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
        case 7:
        case 8:
        case 9:
        case 20:
            if (O97WDRec0x07ParseDropdown(lpstStream,
                                          &dwOffset,
                                          wtcid) == FALSE)
                return(FALSE);

            break;

        case 10:
        case 11:
        case 12:
        case 13:
        case 14:
            if (O97WDRec0x07ParseMenu(lpstStream,
                                      &dwOffset) == FALSE)
                return(FALSE);

            break;

        case 18:
        case 19:
            break;

        default:
            return(FALSE);
    }

    *lpdwOffset = dwOffset;
    return(TRUE);
}


//********************************************************************
//
// Function:
//  BOOL O97WDRec0x07ParseCommandBar()
//
// Parameters:
//  lpstStream          Ptr to Table stream structure
//  lpdwOffset          Offset of 0x07 record command bar subrecord
//
// Description:
//  The function parses through the 0x07 record command bar
//  subrecord at the given offset.  The offset following the last
//  byte of the subrecord is stored at *lpdwOffset.
//
//  The command bar is assumed to have the following structure:
//
//      wStrLen
//      awString[wStrLen]
//      abyUnknown[0x14]
//      byStrLen
//      awString[byStrLen]
//      abyUnknown[0x6C]
//      dwCount
//      repeat dwCount times
//          ParseControl()
//
// Returns:
//  TRUE        If successful
//  FALSE       If unsuccessful
//
//********************************************************************

BOOL O97WDRec0x07ParseCommandBar
(
    LPSS_STREAM     lpstStream,
    LPDWORD         lpdwOffset
)
{
    DWORD           dwOffset;
    DWORD           dwCount;
    WORD            wStrLen;
    DWORD           dwcCtls;
    DWORD           dwCtl;

    dwOffset = *lpdwOffset;

    // wStrLen
    // wString[wStrLen]
    // abyUnknown[0x14]

    if (SSSeekRead(lpstStream,
                   dwOffset,
                   &wStrLen,
                   sizeof(WORD),
                   &dwCount) != SS_STATUS_OK ||
        dwCount != sizeof(WORD))
        return(FALSE);

    wStrLen = WENDIAN(wStrLen);
    dwOffset += sizeof(WORD) + wStrLen * (DWORD)2 + 0x14;

    // byStrLen
    // wString[byStrLen]

    if (O97WDRec0x07ParseString(lpstStream,&dwOffset) == FALSE)
        return(FALSE);

    // abyUnknown[0x6C]

    dwOffset += 0x6C;

    // Read dwcCtls: Count of controls

    if (SSSeekRead(lpstStream,
                   dwOffset,
                   &dwcCtls,
                   sizeof(DWORD),
                   &dwCount) != SS_STATUS_OK ||
        dwCount != sizeof(DWORD))
        return(FALSE);

    dwcCtls = DWENDIAN(dwcCtls);
    dwOffset += sizeof(DWORD);

    for (dwCtl=0;dwCtl<dwcCtls;dwCtl++)
    {
        if (O97WDRec0x07ParseControl(lpstStream,
                                     &dwOffset) == FALSE)
            return(FALSE);
    }

    *lpdwOffset = dwOffset;
    return(TRUE);
}


//********************************************************************
//
// Function:
//  BOOL O97WDRec0x07Parse()
//
// Parameters:
//  lpstStream          Ptr to Table stream structure
//  dwOffset            Offset of beginning of 0x07 record
//  dwShiftOffset       Offset to shift position values
//  lpdwNewOffset       Offset following 0x07 record
//
// Description:
//  The function parses through the 0x07 record at the given
//  offset.  The offset following the last byte of the record
//  is stored at *lpdwNewOffset.
//
//  The record is assumed to have the following format:
//
//      wcbTdsh
//      wcbBts
//      wcbBtd
//      wctb
//      dwcbControls
//      abyControls[dwcbControls]
//      repeat wctb times
//          dwtbid
//          wcbut
//          abyExtra[wcbTdsh - 4]
//          if (dwtbid == 0)
//              ParseCommandBar()
//          else
//              abyButtons[wcbut][wcbBtd]
//
//  If dwShiftOffset is non-zero, then the position values in the
//  record are shifted by subtracting the given shift offset from
//  each of the current offsets.
//
// Returns:
//  TRUE        If successful
//  FALSE       If unsuccessful
//
//********************************************************************

BOOL O97WDRec0x07Parse
(
    LPSS_STREAM         lpstStream,
    DWORD               dwOffset,
    DWORD               dwShiftOffset,
    LPDWORD             lpdwNewOffset
)
{
    DWORD               dwCount;
    WORD                wcbTdsh;
    WORD                wcbBtd;
    DWORD               dwTemp;
    WORD                w;
    WORD                wctb;

    // cbTdsh: Toolbar descriptor header size

    if (SSSeekRead(lpstStream,
                   dwOffset,
                   &wcbTdsh,
                   sizeof(WORD),
                   &dwCount) != SS_STATUS_OK ||
        dwCount != sizeof(WORD))
        return(FALSE);

    wcbTdsh = WENDIAN(wcbTdsh);
    dwOffset += sizeof(WORD);

    // Skip cbBts

    dwOffset += sizeof(WORD);

    // cbBtd: Button descriptor size

    if (SSSeekRead(lpstStream,
                   dwOffset,
                   &wcbBtd,
                   sizeof(WORD),
                   &dwCount) != SS_STATUS_OK ||
        dwCount != sizeof(WORD))
        return(FALSE);

    wcbBtd = WENDIAN(wcbBtd);
    dwOffset += sizeof(WORD);

    // ctb: Toolbar count

    if (SSSeekRead(lpstStream,
                   dwOffset,
                   &wctb,
                   sizeof(WORD),
                   &dwCount) != SS_STATUS_OK ||
        dwCount != sizeof(WORD))
        return(FALSE);

    wctb = WENDIAN(wctb);
    dwOffset += sizeof(WORD);

    // cbControls: Control byte count

    if (SSSeekRead(lpstStream,
                   dwOffset,
                   &dwTemp,
                   sizeof(DWORD),
                   &dwCount) != SS_STATUS_OK ||
        dwCount != sizeof(DWORD))
        return(FALSE);

    dwTemp = DWENDIAN(dwTemp);
    dwOffset += sizeof(DWORD) + dwTemp;

    for (w=0;w<wctb;w++)
    {
        WORD            wcbut;

        // tbid: Toolbar ID

        if (SSSeekRead(lpstStream,
                       dwOffset,
                       &dwTemp,
                       sizeof(DWORD),
                       &dwCount) != SS_STATUS_OK ||
            dwCount != sizeof(DWORD))
            return(FALSE);

        dwTemp = DWENDIAN(dwTemp);
        dwOffset += wcbTdsh;

        // cbut: Count of buttons

        if (SSSeekRead(lpstStream,
                       dwOffset,
                       &wcbut,
                       sizeof(WORD),
                       &dwCount) != SS_STATUS_OK ||
            dwCount != sizeof(WORD))
            return(FALSE);

        wcbut = WENDIAN(wcbut);
        dwOffset += sizeof(WORD);

        if (dwTemp == 0)
        {
            // User-defined toolbar or menu

            if (O97WDRec0x07ParseCommandBar(lpstStream,
                                            &dwOffset) == FALSE)
                return(FALSE);
        }
        else
        {
            // If we are not shifting, just skip the section

            if (dwShiftOffset == 0)
            {
                // Count of buttons * size of button descriptor

                dwOffset += (DWORD)wcbut * (DWORD)wcbBtd;
            }
            else
            {
                WORD        wBut;

                for (wBut=0;wBut<wcbut;wBut++)
                {
                    // Read the control offset

                    if (SSSeekRead(lpstStream,
                                   dwOffset + 0x0A,
                                   &dwTemp,
                                   sizeof(DWORD),
                                   &dwCount) != SS_STATUS_OK ||
                        dwCount != sizeof(DWORD))
                        return(FALSE);

                    // Subtract the shift offset if non-zero

                    if (dwTemp != 0)
                    {
                        dwTemp = DWENDIAN(dwTemp);
                        dwTemp -= dwShiftOffset;
                        dwTemp = DWENDIAN(dwTemp);

                        // Write the new control offset

                        if (SSSeekWrite(lpstStream,
                                        dwOffset + 0x0A,
                                        &dwTemp,
                                        sizeof(DWORD),
                                        &dwCount) != SS_STATUS_OK ||
                            dwCount != sizeof(DWORD))
                            return(FALSE);
                    }

                    dwOffset += wcbBtd;
                }
            }
        }
    }

    *lpdwNewOffset = dwOffset;
    return(TRUE);
}


//********************************************************************
//
// Function:
//  BOOL O97WDGetTDTRecordInfo()
//
// Parameters:
//  lpstStream          Ptr to Table stream structure
//  lpstTDTInfo         Ptr to TDT info structure
//  lpbyWorkBuffer      Ptr to work buffer >= 256 bytes
//
// Description:
//  Parse through the template data table to obtain
//             the location, size, and number of subrecords
//             of record types 0x01, 0x03, 0x10, and 0x11.
//
//  The function returns failure if either of record types
//  0x01, 0x03, 0x10, or 0x11 have more than one record, if any
//  record does not conform to the assumed format, or if
//  an unknown record type is encountered.
//
// Returns:
//  TRUE        If successful
//  FALSE       If unsuccessful
//
//********************************************************************

typedef struct tagO97WD_TDT_INFO
{
    DWORD           dwTDTOffset;
    DWORD           dwTDTSize;

    DWORD           dwRecord0x01Offset;
    DWORD           dwRecord0x01Size;
    WORD            wNum0x01Subrecords;

    DWORD           dwRecord0x03Offset;
    DWORD           dwRecord0x03Size;

    DWORD           dwRecord0x07Offset;
    DWORD           dwRecord0x07Size;

    DWORD           dwRecord0x10Offset;
    DWORD           dwRecord0x10Size;
    WORD            wNum0x10Subrecords;
    DWORD           dwRecord0x11Offset;
    DWORD           dwRecord0x11Size;
    WORD            wNum0x11Subrecords;
} O97WD_TDT_INFO_T, FAR *LPO97WD_TDT_INFO;

BOOL O97WDGetTDTRecordInfo
(
    LPSS_STREAM         lpstStream,
    LPO97WD_TDT_INFO    lpstTDTInfo,
    LPBYTE              lpbyWorkBuffer
)
{
    DWORD               dwOffset;
    DWORD               dwEndOffset;
    DWORD               dwCount;
    WORD                wCount;
    WORD                wLen;
    BYTE                byTemp;
    BYTE                byRecType;
    DWORD               dwByteCount;

    // Verify that the first byte is 0xFF

    dwOffset = lpstTDTInfo->dwTDTOffset;
    dwEndOffset = dwOffset + lpstTDTInfo->dwTDTSize;
    if (SSSeekRead(lpstStream,
                   dwOffset++,
                   &byTemp,
                   sizeof(BYTE),
                   &dwByteCount) != SS_STATUS_OK ||
        dwByteCount != sizeof(BYTE) ||
        byTemp != 0xFF)
    {
        return(FALSE);
    }

    // Initialize fields to uninitialized state

    lpstTDTInfo->dwRecord0x01Offset = 0xFFFFFFFF;
    lpstTDTInfo->dwRecord0x03Offset = 0xFFFFFFFF;
    lpstTDTInfo->dwRecord0x07Offset = 0xFFFFFFFF;
    lpstTDTInfo->dwRecord0x10Offset = 0xFFFFFFFF;
    lpstTDTInfo->dwRecord0x11Offset = 0xFFFFFFFF;

    // Now begin parsing

    byRecType = 0;
    while (dwOffset < dwEndOffset && byRecType != 0x40)
    {
        if (SSSeekRead(lpstStream,
                       dwOffset++,
                       &byRecType,
                       sizeof(BYTE),
                       &dwByteCount) != SS_STATUS_OK ||
            dwByteCount != sizeof(BYTE))
        {
            return(FALSE);
        }

        switch (byRecType)
        {
            case 0x01:
                // Macro record format:
                //  byRecType
                //  wMacroCount
                //  wZero
                //  repeat wMacroCount times
                //  {
                //      abyUnknown[2]
                //      wRecord0x11ID
                //      wRecord0x10Index
                //      abyUnknown[18]
                //  }

                if (lpstTDTInfo->dwRecord0x01Offset != 0xFFFFFFFF)
                {
                    // Double 0x01 record!

                    return(FALSE);
                }
                lpstTDTInfo->dwRecord0x01Offset = dwOffset - 1;

                // Read the number of records

                if (SSSeekRead(lpstStream,
                               dwOffset,
                               (LPBYTE)&lpstTDTInfo->wNum0x01Subrecords,
                               sizeof(WORD),
                               &dwByteCount) != SS_STATUS_OK ||
                    dwByteCount != sizeof(WORD))
                {
                    return(FALSE);
                }

                // Skip past the count and zero word

                dwOffset += sizeof(WORD) + sizeof(WORD);

                lpstTDTInfo->wNum0x01Subrecords =
                    WENDIAN(lpstTDTInfo->wNum0x01Subrecords);

                dwOffset += lpstTDTInfo->wNum0x01Subrecords * (DWORD)24;

                lpstTDTInfo->dwRecord0x01Size = dwOffset -
                    lpstTDTInfo->dwRecord0x01Offset;

                break;

            case 0x02:
                // Font? Format:
                //  byRecType
                //  dwCount
                //  repeat dwCount times
                //  {
                //      abyUnknown[4]
                //  }

                if (SSSeekRead(lpstStream,
                               dwOffset,
                               (LPBYTE)&dwCount,
                               sizeof(DWORD),
                               &dwByteCount) != SS_STATUS_OK ||
                    dwByteCount != sizeof(DWORD))
                {
                    return(FALSE);
                }

                dwCount = DWENDIAN(dwCount);

                dwOffset += sizeof(DWORD) + dwCount * 4;

                break;

            case 0x03:
                // Keyboard command format:
                //  byRecType
                //  dwCount
                //  repeat dwCount times
                //  {
                //      abyUnknown[4] = 0x00 0x00 0x00 0x00
                //      byASCIIKey
                //      byModKey:   0x01 = Shift
                //                  0x02 = Ctrl
                //                  0x04 = Alt
                //      byASCIIKey2 = 0xFF
                //      byModKey2 = 0x00
                //      abyUnknown[4] = 0x00 0x00 0x02 0x00
                //      wRecord0x11ID
                //  }

                if (lpstTDTInfo->dwRecord0x03Offset != 0xFFFFFFFF)
                {
                    // Double 0x03 record!

                    return(FALSE);
                }
                lpstTDTInfo->dwRecord0x03Offset = dwOffset - 1;

                if (SSSeekRead(lpstStream,
                               dwOffset,
                               (LPBYTE)&dwCount,
                               sizeof(DWORD),
                               &dwByteCount) != SS_STATUS_OK ||
                    dwByteCount != sizeof(DWORD))
                {
                    return(FALSE);
                }

                dwCount = DWENDIAN(dwCount);

                dwOffset += sizeof(DWORD) + dwCount * 14;

                lpstTDTInfo->dwRecord0x03Size = dwOffset -
                    lpstTDTInfo->dwRecord0x03Offset;

                break;

            case 0x07:
                if (lpstTDTInfo->dwRecord0x07Offset != 0xFFFFFFFF)
                {
                    // Double 0x07 record!

                    return(FALSE);
                }
                lpstTDTInfo->dwRecord0x07Offset = dwOffset - 1;

                // Parse it

                if (O97WDRec0x07Parse(lpstStream,
                                      dwOffset,
                                      0,
                                      &dwOffset) == FALSE)
                {
                    // Error parsing

                    return(FALSE);
                }

                lpstTDTInfo->dwRecord0x07Size = dwOffset -
                    lpstTDTInfo->dwRecord0x07Offset;

                break;

            case 0x10:
                // Mixed case macro string format:
                //  byRecType
                //  w0xFFFF
                //  wSubrecordCount
                //  w0x0002
                //  repeat wSubrecordCount times
                //  {
                //      wStrLen
                //      wString[wStrLen]
                //      wUnknown
                //  }

                if (lpstTDTInfo->dwRecord0x10Offset != 0xFFFFFFFF)
                {
                    // Double 0x10 record!

                    return(FALSE);
                }
                lpstTDTInfo->dwRecord0x10Offset = dwOffset - 1;

                // Read the next six bytes

                if (SSSeekRead(lpstStream,
                               dwOffset,
                               lpbyWorkBuffer,
                               6,
                               &dwByteCount) != SS_STATUS_OK ||
                    dwByteCount != 6)
                {
                    return(FALSE);
                }

                dwOffset += 6;

                // Verify that the first word is 0xFFFF and
                //  the second is 0x0002

                if (lpbyWorkBuffer[0] != 0xFF ||
                    lpbyWorkBuffer[1] != 0xFF ||
                    lpbyWorkBuffer[4] != 0x02 ||
                    lpbyWorkBuffer[5] != 0x00)
                {
                    return(FALSE);
                }

                // Get the count

                lpstTDTInfo->wNum0x10Subrecords =
                    (lpbyWorkBuffer[3] << 8) | lpbyWorkBuffer[2];

                // Now parse through the strings

                wCount = lpstTDTInfo->wNum0x10Subrecords;
                while (wCount-- != 0)
                {
                    // Read the length

                    if (SSSeekRead(lpstStream,
                                   dwOffset,
                                   (LPBYTE)&wLen,
                                   sizeof(WORD),
                                   &dwByteCount) != SS_STATUS_OK ||
                        dwByteCount != sizeof(WORD))
                    {
                        return(FALSE);
                    }

                    wLen = WENDIAN(wLen);

                    // Skip past this subrecord

                    dwOffset += sizeof(WORD) + wLen * 2 + 2;
                }

                lpstTDTInfo->dwRecord0x10Size = dwOffset -
                    lpstTDTInfo->dwRecord0x10Offset;

                break;

            case 0x11:
                // Uppercase macro string format:
                //  byRecType
                //  wSubrecordCount
                //  repeat wSubrecordCount times
                //  {
                //      wID
                //      wStrLen
                //      wString[wStrLen]
                //      wUnknown
                //  }

                if (lpstTDTInfo->dwRecord0x11Offset != 0xFFFFFFFF)
                {
                    // Double 0x11 record!

                    return(FALSE);
                }
                lpstTDTInfo->dwRecord0x11Offset = dwOffset - 1;

                // Read the subrecord count

                if (SSSeekRead(lpstStream,
                               dwOffset,
                               (LPBYTE)&lpstTDTInfo->wNum0x11Subrecords,
                               sizeof(WORD),
                               &dwByteCount) != SS_STATUS_OK ||
                    dwByteCount != sizeof(WORD))
                {
                    return(FALSE);
                }

                dwOffset += sizeof(WORD);

                lpstTDTInfo->wNum0x11Subrecords =
                    WENDIAN(lpstTDTInfo->wNum0x11Subrecords);

                // Now parse through the strings

                wCount = lpstTDTInfo->wNum0x11Subrecords;
                while (wCount-- != 0)
                {
                    // Skip past the ID

                    dwOffset += sizeof(WORD);

                    // Read the length

                    if (SSSeekRead(lpstStream,
                                   dwOffset,
                                   (LPBYTE)&wLen,
                                   sizeof(WORD),
                                   &dwByteCount) != SS_STATUS_OK ||
                        dwByteCount != sizeof(WORD))
                    {
                        return(FALSE);
                    }

                    wLen = WENDIAN(wLen);

                    // Skip past this subrecord

                    dwOffset += sizeof(WORD) + wLen * 2 + 2;
                }

                lpstTDTInfo->dwRecord0x11Size = dwOffset -
                    lpstTDTInfo->dwRecord0x11Offset;

                break;

            case 0x12:
                // Format:
                //  byRecType
                //  wUnknown

                dwOffset += sizeof(WORD);
                break;

            case 0x40:
                break;

            default:
                // Unknown record type

                return(FALSE);
        }
    }

    return(TRUE);
}


//********************************************************************
//
// Function:
//  BOOL O97WDTDTCheckModuleString()
//
// Description:
//  The function reads the string at the given offset, searches
//  for the first period, then performs an uppercase match
//  against the given module name.  The given module name is
//  assumed to be all uppercase.  If there is a match, the
//  next character must be a period.
//
//  The function assumes that the string at the given offset
//  is made of double-byte characters.
//
//  If there is a match, the function sets *lpbMatch to TRUE.
//  Otherwise, *lpbMatch is set to FALSE.
//
// Returns:
//  TRUE        If successful
//  FALSE       If unsuccessful
//
//********************************************************************

BOOL O97WDTDTCheckModuleString
(
    LPSS_STREAM         lpstStream,
    LPBYTE              abyModuleName,      // Zero terminated module name
    DWORD               dwStrOffset,        // Offset of string to check
    WORD                wLen,               // Length of the string
    LPBOOL              lpbMatch,           // Ptr to BOOL for match status
    LPBYTE              lpbyWorkBuffer      // Work buffer >= 512 bytes
)
{
    WORD                w;
    int                 i;
    DWORD               dwByteCount;

    *lpbMatch = FALSE;

    // For now, if the length is greater than 256, just return
    //  failure

    if (wLen > 256)
        return(FALSE);

    // Multiply by 2 to account for double-byte characters

    wLen *= 2;

    // Read in the string

    if (SSSeekRead(lpstStream,
                   dwStrOffset,
                   lpbyWorkBuffer,
                   wLen,
                   &dwByteCount) != SS_STATUS_OK ||
        dwByteCount != wLen)
    {
        return(FALSE);
    }

    // Now search for the first period

    for (w=0;w<wLen;w+=2)
    {
        if (lpbyWorkBuffer[w] == '.')
        {
            w += 2;
            break;
        }
    }

    if (w >= wLen)
    {
        // No period!?

        return(TRUE);
    }

    // Now do a comparison

    for (i=0;abyModuleName[i];i++)
    {
        if (w >= wLen)
            return(TRUE);

        // Compare both characters as lower case

        if (SSToLower(lpbyWorkBuffer[w]) != SSToLower(abyModuleName[i]))
            return(TRUE);

        w += 2;
    }

    // At this point, the module name must have matched,
    //  so make sure that the next item is a period

    if (w >= wLen)
        return(TRUE);

    if (lpbyWorkBuffer[w] != '.')
        return(TRUE);

    // It was a match

    *lpbMatch = TRUE;

    return(TRUE);
}


//********************************************************************
//
// Function:
//  BOOL O97WDTDTDelete0x11Entry()
//
// Description:
//  The function parses through record 0x11 and deletes the record
//  with the given ID if the module name matches.
//
//  The function returns failure if no records are deleted.
//
// Returns:
//  TRUE        If successful
//  FALSE       If unsuccessful
//
//********************************************************************

BOOL O97WDTDTDelete0x11Entry
(
    LPSS_STREAM         lpstStream,
    LPBYTE              abyModuleName,      // Zero terminated module name
    WORD                wID,                // ID of subrecord to delete
    DWORD               dwRecordOffset,     // Offset of record 0x11
    WORD                wNumSubrecords,     // Number of subrecords
    LPDWORD             lpdwRecordSize,     // Ptr to DWORD of record size
    LPBOOL              lpbDeleted,         // Ptr to BOOL for delete status
    LPBYTE              lpbyWorkBuffer      // Work buffer >= 512 bytes
)
{
    WORD                wCandID;
    WORD                wLen;
    WORD                w;
    DWORD               dwOffset;
    DWORD               dwDelRecordOffset;
    DWORD               dwDelRecordEndOffset;
    BOOL                bMatch;
    DWORD               dwByteCount;

    *lpbDeleted = FALSE;

    // Skip past the record type field and subrecord count field

    dwOffset = dwRecordOffset + sizeof(BYTE) + sizeof(WORD);

    // Iterate through subrecords looking for given ID

    for (w=0;w<wNumSubrecords;w++)
    {
        // Read the ID

        if (SSSeekRead(lpstStream,
                       dwOffset,
                       (LPBYTE)&wCandID,
                       sizeof(WORD),
                       &dwByteCount) != SS_STATUS_OK ||
            dwByteCount != sizeof(WORD))
        {
            return(FALSE);
        }

        wCandID = WENDIAN(wCandID);
        dwOffset += sizeof(WORD);

        // Read the length

        if (SSSeekRead(lpstStream,
                       dwOffset,
                       (LPBYTE)&wLen,
                       sizeof(WORD),
                       &dwByteCount) != SS_STATUS_OK ||
            dwByteCount != sizeof(WORD))
        {
            return(FALSE);
        }

        wLen = WENDIAN(wLen);
        dwOffset += sizeof(WORD);

        if (wCandID == wID)
        {
            // Found it, so check to see whether it is one we
            //  want to delete

            if (O97WDTDTCheckModuleString(lpstStream,
                                          abyModuleName,
                                          dwOffset,
                                          wLen,
                                          &bMatch,
                                          lpbyWorkBuffer) == FALSE)
            {
                return(FALSE);
            }

            if (bMatch == TRUE)
            {
                dwDelRecordOffset = dwOffset - sizeof(WORD) - sizeof(WORD);
                dwDelRecordEndOffset = dwOffset + wLen * 2 + 2;
                break;
            }
            else
                return(TRUE);
        }

        // Skip past this subrecord

        dwOffset += wLen * 2 + 2;
    }

    // Was there a matching record?

    if (w < wNumSubrecords)
    {
        // Copy over the record to delete

        if (SSCopyBytes(lpstStream,
                        dwDelRecordEndOffset,
                        dwDelRecordOffset,
                        dwRecordOffset + *lpdwRecordSize -
                            dwDelRecordEndOffset) != SS_STATUS_OK)
        {
            return(FALSE);
        }

        // Update the size of the record

        *lpdwRecordSize -= (dwDelRecordEndOffset - dwDelRecordOffset);

        // Success

        *lpbDeleted = TRUE;
    }

    // Didn't find a matching record

    return(TRUE);
}


//********************************************************************
//
// Function:
//  BOOL O97WDTDTDelete0x10Entry()
//
// Description:
//  The function parses through record 0x10 and deletes the record
//  at the given index if the module name matches.
//
//  The function returns failure if no records are deleted.
//
// Returns:
//  TRUE        If successful
//  FALSE       If unsuccessful
//
//********************************************************************

BOOL O97WDTDTDelete0x10Entry
(
    LPSS_STREAM         lpstStream,
    LPBYTE              abyModuleName,      // Zero terminated module name
    WORD                wIndex,             // Index of subrecord to delete
    DWORD               dwRecordOffset,     // Offset of record 0x10
    WORD                wNumSubrecords,     // Number of subrecords
    LPDWORD             lpdwRecordSize,     // Ptr to DWORD of record size
    LPBOOL              lpbDeleted,         // Ptr to BOOL for delete status
    LPBYTE              lpbyWorkBuffer      // Work buffer >= 512 bytes
)
{
    WORD                wLen;
    WORD                w;
    DWORD               dwOffset;
    DWORD               dwDelRecordOffset;
    DWORD               dwDelRecordEndOffset;
    BOOL                bMatch;
    DWORD               dwByteCount;

    *lpbDeleted = FALSE;

    // Skip past the record type field, 0xFFFF, subrecord count field,
    //  and 0x0002

    dwOffset = dwRecordOffset + sizeof(BYTE) +
        sizeof(WORD) + sizeof(WORD) + sizeof(WORD);

    // Iterate through subrecords looking for given index

    for (w=0;w<wNumSubrecords;w++)
    {
        // Read the length

        if (SSSeekRead(lpstStream,
                       dwOffset,
                       (LPBYTE)&wLen,
                       sizeof(WORD),
                       &dwByteCount) != SS_STATUS_OK ||
            dwByteCount != sizeof(WORD))
        {
            return(FALSE);
        }

        wLen = WENDIAN(wLen);
        dwOffset += sizeof(WORD);

        if (w == wIndex)
        {
            // Found it

            if (O97WDTDTCheckModuleString(lpstStream,
                                          abyModuleName,
                                          dwOffset,
                                          wLen,
                                          &bMatch,
                                          lpbyWorkBuffer) == FALSE)
            {
                return(FALSE);
            }

            if (bMatch == TRUE)
            {
                dwDelRecordOffset = dwOffset - sizeof(WORD);
                dwDelRecordEndOffset = dwOffset + wLen * 2 + 2;
                break;
            }
            else
                return(TRUE);
        }

        // Skip past this subrecord

        dwOffset += wLen * 2 + 2;
    }

    // Was there a matching record?

    if (w < wNumSubrecords)
    {
        // Copy over the record to delete

        if (SSCopyBytes(lpstStream,
                        dwDelRecordEndOffset,
                        dwDelRecordOffset,
                        dwRecordOffset + *lpdwRecordSize -
                            dwDelRecordEndOffset) != SS_STATUS_OK)
        {
            return(FALSE);
        }

        // Update the size of the record

        *lpdwRecordSize -= (dwDelRecordEndOffset - dwDelRecordOffset);

        // Success

        *lpbDeleted = TRUE;
    }

    // Didn't find a matching record

    return(TRUE);
}


//********************************************************************
//
// Function:
//  BOOL O97WDTDTDelete0x03Entries()
//
// Description:
//  The function parses through record 0x03 and deletes any of the
//  key assignment records referencing the given ID.
//
// Returns:
//  TRUE        If successful
//  FALSE       If unsuccessful
//
//********************************************************************

BOOL O97WDTDTDelete0x03Entries
(
    LPSS_STREAM         lpstStream,
    WORD                wID,                // ID
    DWORD               dwRecordOffset,     // Offset of record 0x10
    LPDWORD             lpdwRecordSize,     // Ptr to DWORD of record size
    LPBYTE              lpbyWorkBuffer      // Work buffer >= 512 bytes
)
{
    DWORD               dwCount;
    DWORD               dwNumLeft;
    DWORD               dwOffset;
    DWORD               dwDstOffset;
    WORD                wCandID;
    BOOL                bDeleted;
    DWORD               dwByteCount;

    if (dwRecordOffset == 0xFFFFFFFF || *lpdwRecordSize == 0)
    {
        // No keyboard assignments

        return(TRUE);
    }

    // Keyboard command format:
    //  byRecType
    //  dwCount
    //  repeat dwCount times
    //  {
    //      abyUnknown[4] = 0x00 0x00 0x00 0x00
    //      byASCIIKey
    //      byModKey:   0x01 = Shift
    //                  0x02 = Ctrl
    //                  0x04 = Alt
    //      byASCIIKey2 = 0xFF
    //      byModKey2 = 0x00
    //      abyUnknown[4] = 0x00 0x00 0x02 0x00
    //      wRecord0x11ID
    //  }

    // Read the count

    dwOffset = dwRecordOffset + sizeof(BYTE);
    if (SSSeekRead(lpstStream,
                   dwOffset,
                   (LPBYTE)&dwCount,
                   sizeof(DWORD),
                   &dwByteCount) != SS_STATUS_OK ||
        dwByteCount != sizeof(DWORD))
    {
        return(FALSE);
    }

    dwOffset += sizeof(DWORD);

    dwCount = DWENDIAN(dwCount);

    // Iterate through subrecords

    dwNumLeft = dwCount;
    dwDstOffset = dwOffset;
    bDeleted = FALSE;
    while (dwNumLeft-- != 0)
    {
        // Read the ID of the next record

        if (SSSeekRead(lpstStream,
                       dwOffset+12,
                       (LPBYTE)&wCandID,
                       sizeof(WORD),
                       &dwByteCount) != SS_STATUS_OK ||
            dwByteCount != sizeof(WORD))
        {
            return(FALSE);
        }

        wCandID = WENDIAN(wCandID);
        if (wCandID != wID)
        {
            // Copy only if we have deleted something so far

            if (dwDstOffset != dwOffset)
            {
                // Read record

                if (SSSeekRead(lpstStream,
                               dwOffset,
                               lpbyWorkBuffer,
                               14,
                               &dwByteCount) != SS_STATUS_OK ||
                    dwByteCount != 14)
                {
                    return(FALSE);
                }

                // Write record

                if (SSSeekWrite(lpstStream,
                                dwDstOffset,
                                lpbyWorkBuffer,
                                14,
                                &dwByteCount) != SS_STATUS_OK ||
                    dwByteCount != 14)
                {
                    return(FALSE);
                }
            }

            dwDstOffset += 14;
        }
        else
        {
            // This is one to delete

            --dwCount;
            bDeleted = TRUE;
        }

        dwOffset += 14;
    }

    // Was at least one record deleted?

    if (bDeleted == TRUE)
    {
        // Update the size of the record

        if (dwCount == 0)
            *lpdwRecordSize = 0;
        else
        {
            *lpdwRecordSize = sizeof(BYTE) + sizeof(DWORD) + dwCount * 14;

            // Write the new count

            dwCount = DWENDIAN(dwCount);
            if (SSSeekWrite(lpstStream,
                            dwRecordOffset + sizeof(BYTE),
                            (LPBYTE)&dwCount,
                            sizeof(DWORD),
                            &dwByteCount) != SS_STATUS_OK ||
                dwByteCount != sizeof(DWORD))
            {
                return(FALSE);
            }
        }
    }

    return(TRUE);
}


//********************************************************************
//
// Function:
//  BOOL O97WDUpdateRecord0x07AfterMove()
//
// Description:
//  The function parses through record 0x07 and shifts the offsets
//  in the records that contain offsets to the top-level section.
//  The offsets are shifted by the given amount.
//
// Returns:
//  TRUE        If successful
//  FALSE       If unsuccessful
//
//********************************************************************

BOOL O97WDUpdateRecord0x07AfterMove
(
    LPSS_STREAM         lpstStream,
    DWORD               dwRecordOffset,     // Old offset of the record
    DWORD               dwRecordSize,       // Size of the record
    DWORD               dwShiftOffset,      // Amount shifted
    LPBYTE              lpbyWorkBuffer      // Work buffer >= 1K bytes
)
{
    DWORD               dwOffset;

    (void)dwRecordSize;
    (void)lpbyWorkBuffer;

    if (dwRecordOffset == 0xFFFFFFFF || dwShiftOffset == 0)
    {
        // No record 0x07

        return(TRUE);
    }

    dwOffset = dwRecordOffset - dwShiftOffset + 1;

    if (O97WDRec0x07Parse(lpstStream,
                          dwOffset,
                          dwShiftOffset,
                          &dwOffset) == FALSE)
        return(FALSE);

    return(TRUE);
}


//********************************************************************
//
// Function:
//  BOOL O97WDTDTDeleteModule()
//
// Description:
//  The function parses through record 0x01 and deletes all
//  subrecords belonging to the given module.
//
//  The function returns with *lpbDeleted == FALSE if no records
//  are deleted.  If a record was deleted, then the function
//  returns with *lpbDeleted = TRUE.
//
//  On success, the function updates the subrecord counts
//  for record types 0x01, 0x10, and 0x11.  On return,
//  lpstTDTInfo->dwTDTSize is updated to reflect the new
//  size of the template data table after compaction.
//
// Returns:
//  TRUE        If successful
//  FALSE       If unsuccessful
//
//********************************************************************

BOOL O97WDTDTDeleteModule
(
    LPSS_STREAM         lpstStream,
    LPO97WD_TDT_INFO    lpstTDTInfo,        // Ptr to TDT info structure
    LPBYTE              abyModuleName,      // Zero terminated module name
    LPBOOL              lpbDeleted,         // Ptr to BOOL for delete status
    LPBYTE              lpbyWorkBuffer      // Work buffer >= 512 bytes
)
{
    DWORD               dwRecord0x01Size;
    WORD                wNum0x01Subrecords;
    DWORD               dwRecord0x03Size;
    DWORD               dwRecord0x10Size;
    WORD                wNum0x10Subrecords;
    DWORD               dwRecord0x11Size;
    WORD                wNum0x11Subrecords;

    DWORD               dwRecord0x07ShiftOffset;

    WORD                w;
    WORD                wTemp;
    DWORD               dwTemp;

    WORD                wDeleteIndex;
    WORD                w0x11ID;
    WORD                w0x10Index;

    BOOL                bAtLeastOneDeleted;

    BOOL                b0x11Deleted;
    BOOL                b0x10Deleted;

    DWORD               dwOffset;
    DWORD               dwDstOffset;

#define NUM_REGIONS 4

    DWORD               dwRegionOffset[NUM_REGIONS];
    DWORD               dwRegionNewSize[NUM_REGIONS];
    DWORD               dwRegionOrigSize[NUM_REGIONS];
    int                 nNumRegions;

    int                 i, j;

    DWORD               dwByteCount;

    // There is no macro info if the record offset of the
    //  0x01 record is 0xFFFFFFFF

    if (lpstTDTInfo->dwRecord0x01Offset == 0xFFFFFFFF)
    {
        *lpbDeleted = FALSE;
        return(TRUE);
    }

    dwRecord0x01Size = lpstTDTInfo->dwRecord0x01Size;
    wNum0x01Subrecords = lpstTDTInfo->wNum0x01Subrecords;
    dwRecord0x03Size = lpstTDTInfo->dwRecord0x03Size;
    dwRecord0x10Size = lpstTDTInfo->dwRecord0x10Size;
    wNum0x10Subrecords = lpstTDTInfo->wNum0x10Subrecords;
    dwRecord0x11Size = lpstTDTInfo->dwRecord0x11Size;
    wNum0x11Subrecords = lpstTDTInfo->wNum0x11Subrecords;

    // Iterate through the subrecords of record type 0x01

    bAtLeastOneDeleted = FALSE;
    while (wNum0x01Subrecords != 0)
    {
        dwOffset = lpstTDTInfo->dwRecord0x01Offset + 7;
        for (w=0;w<wNum0x01Subrecords;w++)
        {
            // Get the 0x11 record ID and 0x10 record index

            if (SSSeekRead(lpstStream,
                           dwOffset,
                           (LPBYTE)&w0x11ID,
                           sizeof(WORD),
                           &dwByteCount) != SS_STATUS_OK ||
                dwByteCount != sizeof(WORD) ||
                SSSeekRead(lpstStream,
                           dwOffset + 2,
                           (LPBYTE)&w0x10Index,
                           sizeof(WORD),
                           &dwByteCount) != SS_STATUS_OK ||
                dwByteCount != sizeof(WORD))
            {
                return(FALSE);
            }

            // Endianize

            w0x11ID = WENDIAN(w0x11ID);
            w0x10Index = WENDIAN(w0x10Index);

            // Delete 0x11 entry

            if (O97WDTDTDelete0x11Entry(lpstStream,
                                        abyModuleName,
                                        w0x11ID,
                                        lpstTDTInfo->dwRecord0x11Offset,
                                        wNum0x11Subrecords,
                                        &dwRecord0x11Size,
                                        &b0x11Deleted,
                                        lpbyWorkBuffer) == FALSE)
            {
                return(FALSE);
            }

            // Delete 0x10 entry

            if (O97WDTDTDelete0x10Entry(lpstStream,
                                        abyModuleName,
                                        w0x10Index,
                                        lpstTDTInfo->dwRecord0x10Offset,
                                        wNum0x10Subrecords,
                                        &dwRecord0x10Size,
                                        &b0x10Deleted,
                                        lpbyWorkBuffer) == FALSE)
            {
                return(FALSE);
            }

            if (b0x11Deleted == TRUE && b0x10Deleted == TRUE)
            {
                --wNum0x11Subrecords;
                --wNum0x10Subrecords;
                wDeleteIndex = w;
                break;
            }

            if (b0x11Deleted == TRUE || b0x10Deleted == TRUE)
            {
                // Only one deleted is an error condition

                return(FALSE);
            }

            dwOffset += 24;
        }

        if (w == wNum0x01Subrecords)
        {
            // No records were deleted, so that's it

            break;
        }

        // Delete the keyboard associations if any

        if (O97WDTDTDelete0x03Entries(lpstStream,
                                      w0x11ID,
                                      lpstTDTInfo->dwRecord0x03Offset,
                                      &dwRecord0x03Size,
                                      lpbyWorkBuffer) == FALSE)
        {
            return(FALSE);
        }

        // Update the record count

        --wNum0x01Subrecords;
        w = WENDIAN(wNum0x01Subrecords);
        if (SSSeekWrite(lpstStream,
                        lpstTDTInfo->dwRecord0x01Offset + 1,
                        (LPBYTE)&w,
                        sizeof(WORD),
                        &dwByteCount) != SS_STATUS_OK ||
            dwByteCount != sizeof(WORD))
        {
            return(FALSE);
        }

        // Go through the records and update the 0x10 index
        //  of those that are greater than the deleted index,
        //  also rewriting the records so that the entry
        //  to delete is not written

        dwOffset = lpstTDTInfo->dwRecord0x01Offset + 5;
        dwDstOffset = dwOffset;
        for (w=0;w<=wNum0x01Subrecords;w++)
        {
            if (w != wDeleteIndex)
            {
                // Get the whole record

                if (SSSeekRead(lpstStream,
                               dwOffset,
                               lpbyWorkBuffer,
                               0x18,
                               &dwByteCount) != SS_STATUS_OK ||
                    dwByteCount != 0x18)
                {
                    return(FALSE);
                }

                // Check the index

                wTemp = *(LPWORD)(lpbyWorkBuffer + 4);
                wTemp = WENDIAN(wTemp);
                if (wTemp > w0x10Index)
                {
                    --wTemp;
                    wTemp = WENDIAN(wTemp);
                    *(LPWORD)(lpbyWorkBuffer+4) = wTemp;
                }

                // Write the whole record

                if (SSSeekWrite(lpstStream,
                                dwDstOffset,
                                lpbyWorkBuffer,
                                0x18,
                                &dwByteCount) != SS_STATUS_OK ||
                    dwByteCount != 0x18)
                {
                    return(FALSE);
                }

                dwDstOffset += 24;
            }

            dwOffset += 24;
        }

        dwRecord0x01Size -= 24;
        bAtLeastOneDeleted = TRUE;
    }

    if (bAtLeastOneDeleted == FALSE)
    {
        // Nothing was deleted

        *lpbDeleted = FALSE;

        return(TRUE);
    }

    // Something was deleted

    *lpbDeleted = TRUE;


    /////////////////////////////////////////////////////////////
    // Update subrecord counts for record types 0x01, 0x10, 0x11
    /////////////////////////////////////////////////////////////

    // Update subrecord count of record type 0x01

    wTemp = WENDIAN(wNum0x01Subrecords);
    if (SSSeekWrite(lpstStream,
                    lpstTDTInfo->dwRecord0x01Offset + 1,
                    (LPBYTE)&wTemp,
                    sizeof(WORD),
                    &dwByteCount) != SS_STATUS_OK ||
        dwByteCount != sizeof(WORD))
    {
        return(FALSE);
    }

    // Update subrecord count of record type 0x10

    wTemp = WENDIAN(wNum0x10Subrecords);
    if (SSSeekWrite(lpstStream,
                    lpstTDTInfo->dwRecord0x10Offset + 3,
                    (LPBYTE)&wTemp,
                    sizeof(WORD),
                    &dwByteCount) != SS_STATUS_OK ||
        dwByteCount != sizeof(WORD))
    {
        return(FALSE);
    }

    // Update subrecord count of record type 0x11

    wTemp = WENDIAN(wNum0x11Subrecords);
    if (SSSeekWrite(lpstStream,
                    lpstTDTInfo->dwRecord0x11Offset + 1,
                    (LPBYTE)&wTemp,
                    sizeof(WORD),
                    &dwByteCount) != SS_STATUS_OK ||
        dwByteCount != sizeof(WORD))
    {
        return(FALSE);
    }

    /////////////////////////////////////////////////////////////
    // Compact everything
    /////////////////////////////////////////////////////////////

    if (wNum0x01Subrecords == 0)
        dwRecord0x01Size = 0;

    if (wNum0x10Subrecords == 0)
        dwRecord0x10Size = 0;

    if (wNum0x11Subrecords == 0)
        dwRecord0x11Size = 0;

    // Get regions

    dwRegionOffset[0] = lpstTDTInfo->dwRecord0x01Offset;
    dwRegionNewSize[0] = dwRecord0x01Size;
    dwRegionOrigSize[0] = lpstTDTInfo->dwRecord0x01Size;
    dwRegionOffset[1] = lpstTDTInfo->dwRecord0x10Offset;
    dwRegionNewSize[1] = dwRecord0x10Size;
    dwRegionOrigSize[1] = lpstTDTInfo->dwRecord0x10Size;
    dwRegionOffset[2] = lpstTDTInfo->dwRecord0x11Offset;
    dwRegionNewSize[2] = dwRecord0x11Size;
    dwRegionOrigSize[2] = lpstTDTInfo->dwRecord0x11Size;

    if (lpstTDTInfo->dwRecord0x03Offset != 0xFFFFFFFF)
    {
        nNumRegions = 4;

        // Get 0x03 region

        dwRegionOffset[3] = lpstTDTInfo->dwRecord0x03Offset;
        dwRegionNewSize[3] = dwRecord0x03Size;
        dwRegionOrigSize[3] = lpstTDTInfo->dwRecord0x03Size;
    }
    else
        nNumRegions = 3;


    // Sort regions by offset using bubble sort

    for (i=0;i<nNumRegions;i++)
    {
        for (j=i+1;j<nNumRegions;j++)
        {
            if (dwRegionOffset[j] < dwRegionOffset[i])
            {
                // Do a swap

                dwTemp = dwRegionOffset[j];
                dwRegionOffset[j] = dwRegionOffset[i];
                dwRegionOffset[i] = dwTemp;

                dwTemp = dwRegionNewSize[j];
                dwRegionNewSize[j] = dwRegionNewSize[i];
                dwRegionNewSize[i] = dwTemp;

                dwTemp = dwRegionOrigSize[j];
                dwRegionOrigSize[j] = dwRegionOrigSize[i];
                dwRegionOrigSize[i] = dwTemp;
            }
        }
    }

    // Do compaction

    dwDstOffset = dwRegionOffset[0];
    for (i=0;i<nNumRegions;i++)
    {
        // Copy the region

        if (SSCopyBytes(lpstStream,
                        dwRegionOffset[i],
                        dwDstOffset,
                        dwRegionNewSize[i]) != SS_STATUS_OK)
        {
            return(FALSE);
        }

        dwDstOffset += dwRegionNewSize[i];

        // Copy the bytes after the region

        if (i < (nNumRegions - 1))
        {
            // The bytes consist of the bytes after the region
            //  up to the byte immediately before the next region.

            dwTemp = dwRegionOffset[i + 1] -
                (dwRegionOffset[i] + dwRegionOrigSize[i]);
        }
        else
        {
            // The bytes consist of the bytes after the region
            //  up to the end of the template data table

            dwTemp = (lpstTDTInfo->dwTDTOffset + lpstTDTInfo->dwTDTSize) -
                (dwRegionOffset[i] + dwRegionOrigSize[i]);
        }

        // Check to see if record 0x07 is within this region

        if (lpstTDTInfo->dwRecord0x07Offset != 0xFFFFFFFF)
        {
            if (dwRegionOffset[i] + dwRegionOrigSize[i] <=
                lpstTDTInfo->dwRecord0x07Offset &&
                lpstTDTInfo->dwRecord0x07Offset <
                dwRegionOffset[i] + dwRegionOrigSize[i] + dwTemp)
            {
                // The new offset of record 0x07 is lessened
                //  by the distance moved

                dwRecord0x07ShiftOffset =
                    dwRegionOffset[i] + dwRegionOrigSize[i] - dwDstOffset;
            }
        }

        if (SSCopyBytes(lpstStream,
                        dwRegionOffset[i] + dwRegionOrigSize[i],
                        dwDstOffset,
                        dwTemp) != SS_STATUS_OK)
        {
            return(FALSE);
        }

        dwDstOffset += dwTemp;
    }

    // Zero out the left over bytes

    if (SSWriteZeroes(lpstStream,
                      dwDstOffset,
                      lpstTDTInfo->dwTDTOffset + lpstTDTInfo->dwTDTSize -
                          dwDstOffset) != SS_STATUS_OK)
    {
        return(FALSE);
    }

    // Update the records in record 0x07

    if (O97WDUpdateRecord0x07AfterMove(lpstStream,
                                       lpstTDTInfo->dwRecord0x07Offset,
                                       lpstTDTInfo->dwRecord0x07Size,
                                       dwRecord0x07ShiftOffset,
                                       lpbyWorkBuffer) == FALSE)
    {
        return(FALSE);
    }

    // Update the template data table size

    lpstTDTInfo->dwTDTSize = dwDstOffset - lpstTDTInfo->dwTDTOffset;

    return(TRUE);
}


//********************************************************************
//
// Function:
//  BOOL O97WDOpenTDT()
//
// Description:
//  Opens the WordDocument stream to get the TDT offset and size.
//  The function reads byte 11 of the FIB and checks the second
//  LSB to determines whether to get the TDT info from the 1Table
//  stream or the 0Table stream.
//
// Returns:
//  TRUE        If successful
//  FALSE       If unsuccessful
//
//********************************************************************

BOOL O97WDOpenTDT
(
    LPSS_STREAM         lpstStream,
    LPO97_SCAN          lpstO97Scan,        // VBA5 project stream info
    LPO97WD_TDT_INFO    lpstTDTInfo,        // Ptr to TDT structure
    LPBYTE              lpbyWorkBuffer      // Work buffer >= 256 bytes
)
{
    DWORD               dwByteCount;
    BYTE                byFlags;

    // Open the WordDocument stream to get the template data
    //  table location and size

    if (SSOpenStreamAtIndex(lpstStream,
                            lpstO97Scan->u.stWD8.dwWordDocumentEntry) !=
        SS_STATUS_OK)
    {
        // Failed to open WordDocument stream

        return(FALSE);
    }

    // Get the template data table offset

    if (SSSeekRead(lpstStream,
                   346,
                   (LPBYTE)&lpstTDTInfo->dwTDTOffset,
                   sizeof(DWORD),
                   &dwByteCount) != SS_STATUS_OK ||
        dwByteCount != sizeof(DWORD))
    {
        return(FALSE);
    }

    lpstTDTInfo->dwTDTOffset = DWENDIAN(lpstTDTInfo->dwTDTOffset);

    // Get the template data table size

    if (SSSeekRead(lpstStream,
                   350,
                   (LPBYTE)&lpstTDTInfo->dwTDTSize,
                   sizeof(DWORD),
                   &dwByteCount) != SS_STATUS_OK ||
        dwByteCount != sizeof(DWORD))
    {
        return(FALSE);
    }

    lpstTDTInfo->dwTDTSize = DWENDIAN(lpstTDTInfo->dwTDTSize);

    // Get the 11th byte of the FIB

    if (SSSeekRead(lpstStream,
                   11,
                   &byFlags,
                   sizeof(BYTE),
                   &dwByteCount) != SS_STATUS_OK ||
        dwByteCount != sizeof(BYTE))
    {
        return(FALSE);
    }

    if (byFlags & 0x02)
    {
        // Open the 1Table stream to get the template data
        //  table location and size

        if (SSOpenStreamAtIndex(lpstStream,
                                lpstO97Scan->u.stWD8.dw1TableEntry) !=
            SS_STATUS_OK)
        {
            // Failed to open 1Table stream

            return(FALSE);
        }
    }
    else
    {
        // Open the 0Table stream to get the template data
        //  table location and size

        if (SSOpenStreamAtIndex(lpstStream,
                                lpstO97Scan->u.stWD8.dw0TableEntry) !=
            SS_STATUS_OK)
        {
            // Failed to open 0Table stream

            return(FALSE);
        }
    }

    // Get the info for record types 0x01, 0x10, and 0x11

    if (O97WDGetTDTRecordInfo(lpstStream,
                              lpstTDTInfo,
                              lpbyWorkBuffer) == FALSE)
        return(FALSE);

    // Success

    return(TRUE);
}


//********************************************************************
//
// Function:
//  BOOL O97WDDeleteModule()
//
// Description:
//  Deletes a module from a Word 8.0 document using the following
//  steps:
//
//      1. Delete the entry from the VBA streams
//      2. Open the WordDocument stream to get the template
//         data table location and size.
//      3. Open the 1Table stream to do the following:
//          a. Parse through the template data table to obtain
//             the location, size, and number of subrecords
//             of record types 0x01, 0x10, and 0x11.
//          b. Iterate through the subrecords of record type 0x01
//             to do the following:
//              i. Get the subrecord number for record type 0x10
//                 and 0x11 containing the information.
//              ii. If both records contain PROJECT.MODULENAME.,
//                  then delete all three subrecords.
//              iii. Update subrecord type 0x10 pointers of record
//                   type 0x01 to account for deleted subrecord 0x10.
//              iv. Update record size and number of subrecords.
//          c. If subrecords were eliminated, fill in the gaps
//             between the records.
//      4. If subrecords were eliminated, update the template data
//         table size field in the WordDocument stream.
//
// Returns:
//  TRUE        If the delete was successful
//  FALSE       If the delete was unsuccessful
//
//********************************************************************

#ifdef SYM_WIN16
#pragma data_seg("FAR_DATA_0")
#endif

BYTE FAR gabyszThisDocument[13] = "ThisDocument";

BYTE FAR gabyLZNTBlankThisDocumentSrc[0x8E] =
{
    0x01, 0x8A, 0xB0, 0x00, 0x41, 0x74, 0x74, 0x72,
    0x69, 0x62, 0x75, 0x74, 0x00, 0x65, 0x20, 0x56,
    0x42, 0x5F, 0x4E, 0x61, 0x6D, 0x00, 0x65, 0x20,
    0x3D, 0x20, 0x22, 0x54, 0x68, 0x69, 0x00, 0x73,
    0x44, 0x6F, 0x63, 0x75, 0x6D, 0x65, 0x6E, 0x10,
    0x74, 0x22, 0x0D, 0x0A, 0x0A, 0x8C, 0x42, 0x61,
    0x73, 0x01, 0x02, 0x8C, 0x31, 0x4E, 0x6F, 0x72,
    0x6D, 0x61, 0x6C, 0x02, 0x2E, 0x19, 0x56, 0x43,
    0x72, 0x65, 0x61, 0x74, 0x61, 0x04, 0x62, 0x6C,
    0x01, 0x60, 0x46, 0x61, 0x6C, 0x73, 0x65, 0x01,
    0x0C, 0x96, 0x50, 0x72, 0x65, 0x64, 0x65, 0x63,
    0x6C, 0x12, 0x61, 0x00, 0x06, 0x49, 0x64, 0x00,
    0x78, 0x54, 0x72, 0x75, 0x81, 0x0D, 0x22, 0x45,
    0x78, 0x70, 0x6F, 0x73, 0x65, 0x14, 0x1C, 0x00,
    0x54, 0x65, 0x6D, 0x70, 0x6C, 0x61, 0x74, 0x65,
    0x20, 0x44, 0x65, 0x72, 0x69, 0x76, 0x15, 0x24,
    0x43, 0x75, 0xC0, 0x73, 0x74, 0x6F, 0x6D, 0x69,
    0x7A, 0x04, 0x87, 0x03, 0x63, 0x00
};

#ifdef SYM_WIN16
#pragma data_seg()
#endif

BOOL O97WDDeleteModule
(
    LPSS_ROOT           lpstRoot,
    LPSS_STREAM         lpstStream,
    LPLZNT              lpstLZNT,
    LPBYTE              abyModuleName,      // Zero terminated module name
    LPO97_SCAN          lpstO97Scan,        // VBA5 project stream info
    LPBYTE              lpbyWorkBuffer      // Work buffer >= 0.75K bytes
)
{
    BOOL                bDeleted;
    O97WD_TDT_INFO_T    stTDTInfo;
    DWORD               dwByteCount;

    if (O97MemICmp(abyModuleName,
                   (LPBYTE)gabyszThisDocument,
                   sizeof(gabyszThisDocument)) != FALSE)
    {
        BOOL            bTooSmall;

        if (VBA5LZNTModuleInit(lpstStream,
                               lpstLZNT) == FALSE)
            return(FALSE);

        // If ThisDocument is already empty, just leave it

        if (O97LZNTStreamAttributeOnly(lpstLZNT) != FALSE)
            return(TRUE);

        // Replace the source with a blank ThisDocument source

        if (O97ModReplaceVBASrc(lpstStream,
                                lpstLZNT,
                                gabyLZNTBlankThisDocumentSrc,
                                sizeof(gabyLZNTBlankThisDocumentSrc),
                                &bTooSmall) == FALSE ||
            bTooSmall != FALSE)
            return(FALSE);
    }
    else
    {
        if (O97DeleteModule(lpstRoot,
                            lpstStream,
                            lpstLZNT,
                            abyModuleName,
                            lpstO97Scan,
                            lpbyWorkBuffer) == FALSE)
            return(FALSE);
    }

    if (O97WDOpenTDT(lpstStream,
                     lpstO97Scan,
                     &stTDTInfo,
                     lpbyWorkBuffer) == FALSE)
    {
        // Failed to open TDT

        return(FALSE);
    }

    // Delete the desired module entries

    if (O97WDTDTDeleteModule(lpstStream,
                             &stTDTInfo,
                             abyModuleName,
                             &bDeleted,
                             lpbyWorkBuffer) == FALSE)
    {
        return(FALSE);
    }

    if (bDeleted == FALSE)
    {
        // Nothing deleted

        return(TRUE);
    }

    // Update the template data table size in the WordDocument stream

    if (SSOpenStreamAtIndex(lpstStream,
                            lpstO97Scan->u.stWD8.dwWordDocumentEntry) !=
        SS_STATUS_OK)
    {
        // Failed to open WordDocument stream

        return(FALSE);
    }

    // Update the template data table size

    stTDTInfo.dwTDTSize = DWENDIAN(stTDTInfo.dwTDTSize);
    if (SSSeekWrite(lpstStream,
                    350,
                    (LPBYTE)&stTDTInfo.dwTDTSize,
                    sizeof(DWORD),
                    &dwByteCount) != SS_STATUS_OK ||
        dwByteCount != sizeof(DWORD))
    {
        return(FALSE);
    }

    return(TRUE);
}


//********************************************************************
//
// Function:
//  BOOL O97WDCheckMacrosDeletable()
//
// Description:
//  Checks to see whether there are any modules left in the
//  VBA storage and zeroes out the Macros storage name if that
//  is the case.
//
//  The function determines that there are no more modules
//  if all of the following conditions are met:
//      1. The names of streams in the VBA storage only
//         consist of the following:
//          - ""
//          - "_*"
//          - "dir"
//          - "ThisDocument"
//          - None of the above and the stream length is zero
//      2. The "ThisDocument" stream consists only of lines
//         that begin with "Attribute ", case insensitive.
//      3. The TDT does not contain any 0x01, 0x10, nor 0x11
//         records.
//
// Returns:
//  TRUE        If the ThisDocument stream was successfully
//              analyzed as empty
//  FALSE       Otherwise, including error
//
//********************************************************************

BOOL O97WDCheckMacrosDeletable
(
    LPSS_ROOT           lpstRoot,
    LPSS_STREAM         lpstStream,
    LPLZNT              lpstLZNT,
    LPO97_SCAN          lpstO97Scan,        // VBA5 project stream info
    LPBYTE              lpbyWorkBuffer      // Work buffer >= 0.5K bytes
)
{
    O97WD_TDT_INFO_T    stTDTInfo;
    LPSS_ENUM_SIBS      lpstSibs;

    // Allocate a sibling enumeration structure

    if (SSAllocEnumSibsStruct(lpstRoot,
                              &lpstSibs,
                              SS_DEF_MAX_WAITING_SIBS) != SS_STATUS_OK)
    {
        return(FALSE);
    }

    // Determine whether there is any other module stream
    //  besides "ThisDocument"

    SSInitEnumSibsStruct(lpstSibs,
                         lpstO97Scan->dwVBAChildEntry);

    if (SSEnumSiblingEntriesCB(lpstRoot,
                               O97WDCheckMacrosDeletableCB,
                               NULL,
                               lpstSibs,
                               lpstStream) == SS_STATUS_OK)
    {
        SSFreeEnumSibsStruct(lpstRoot,lpstSibs);
        return(FALSE);
    }

    // Open the ThisDocument stream

    SSInitEnumSibsStruct(lpstSibs,
                         lpstO97Scan->dwVBAChildEntry);

    if (SSEnumSiblingEntriesCB(lpstRoot,
                               O97WDOpenThisDocumentCB,
                               NULL,
                               lpstSibs,
                               lpstStream) == SS_STATUS_OK)
    {
        SSFreeEnumSibsStruct(lpstRoot,lpstSibs);

        // Determine whether the ThisDocument stream consists of only
        //  Attribute lines

        if (VBA5LZNTModuleInit(lpstStream,
                               lpstLZNT) == FALSE)
        {
            // Failed to initialize for reading source text string

            return(FALSE);
        }

        if (O97LZNTStreamAttributeOnly(lpstLZNT) == FALSE)
        {
            // The ThisDocument stream contains non-attribute lines

            return(FALSE);
        }
    }
    else
    {
        SSFreeEnumSibsStruct(lpstRoot,lpstSibs);
        return(TRUE);
    }

    // Check the TDT

    if (O97WDOpenTDT(lpstStream,
                     lpstO97Scan,
                     &stTDTInfo,
                     lpbyWorkBuffer) == FALSE)
    {
        // Failed to open TDT

        return(FALSE);
    }

    if (stTDTInfo.dwRecord0x01Offset != 0xFFFFFFFF ||
        stTDTInfo.dwRecord0x10Offset != 0xFFFFFFFF ||
        stTDTInfo.dwRecord0x11Offset != 0xFFFFFFFF)
    {
        // TDT contains at least one of record 0x01, 0x10, or 0x11

        return(FALSE);
    }

    return(TRUE);
}


//********************************************************************
//
// Function:
//  BOOL O97WDHasAutoText()
//
// Parameters:
//  lpstStream              Ptr to open WordDocument stream
//  lpbHasAutoText          Ptr to BOOL to store status
//
// Description:
//  Checks the value at offset 0xE6.  This value stores the
//  size of the glossary string table.  If the value is non-zero,
//  the function sets *lpbHasAutoText to TRUE.
//
//  Otherwise, the function checks the second header.  The offset
//  of the second header is 0x200 multiplied by the WORD value at
//  offset 0x08.
//
//  If the value at offset 0xE6 relative to the second header is
//  non-zero, the function sets *lpbHasAutoText to TRUE.
//
//  Otherwise the function sets *lpbHasAutoText to FALSE.
//
// Returns:
//  TRUE        On success
//  FALSE       On failure
//
//********************************************************************

BOOL O97WDHasAutoText
(
    LPSS_STREAM         lpstStream,
    LPBOOL              lpbHasAutoText
)
{
    DWORD               dwValue;
    WORD                w2ndHdrBlk;
    DWORD               dwByteCount;

    // Read DWORD at offset 0xE6

    if (SSSeekRead(lpstStream,
                   0xE6,
                   &dwValue,
                   sizeof(DWORD),
                   &dwByteCount) != SS_STATUS_OK ||
        dwByteCount != sizeof(DWORD))
    {
        // Failed to read DWORD at offset 0xE6

        return(FALSE);
    }

    dwValue = DWENDIAN(dwValue);
    if (dwValue != 0)
    {
        *lpbHasAutoText = TRUE;
        return(TRUE);
    }

    // Get the block number of the second header

    if (SSSeekRead(lpstStream,
                   0x08,
                   &w2ndHdrBlk,
                   sizeof(WORD),
                   &dwByteCount) != SS_STATUS_OK ||
        dwByteCount != sizeof(WORD))
    {
        // Failed to read WORD at offset 0x08

        return(FALSE);
    }

    w2ndHdrBlk = WENDIAN(w2ndHdrBlk);

    // Read DWORD at offset 0xE6 relative to the second header

    if (SSSeekRead(lpstStream,
                   (DWORD)w2ndHdrBlk * (DWORD)0x200 + (DWORD)0xE6,
                   &dwValue,
                   sizeof(DWORD),
                   &dwByteCount) != SS_STATUS_OK ||
        dwByteCount != sizeof(DWORD))
    {
        // Failed to read DWORD at offset 0xE6

        return(FALSE);
    }

    dwValue = DWENDIAN(dwValue);
    if (dwValue != 0)
    {
        *lpbHasAutoText = TRUE;
        return(TRUE);
    }

    *lpbHasAutoText = FALSE;
    return(TRUE);
}


//********************************************************************
//
// Function:
//  BOOL O97WDChangeToDocument()
//
// Parameters:
//  lpstStream              Ptr to open WordDocument stream
//
// Description:
//  Resets the zeroth bit of the byte at offset ten to zero.
//  This ensures that Word 97 interprets the document as
//  a document and not a template.
//
// Returns:
//  TRUE        On success
//  FALSE       On failure
//
//********************************************************************

BOOL O97WDChangeToDocument
(
    LPSS_STREAM         lpstStream
)
{
    BYTE                byFlags;
    DWORD               dwByteCount;

    // Read Word Document flag byte

    if (SSSeekRead(lpstStream,
                   10,
                   &byFlags,
                   sizeof(BYTE),
                   &dwByteCount) != SS_STATUS_OK ||
        dwByteCount != sizeof(BYTE))
    {
        // Failed to read flag byte

        return(FALSE);
    }

    // Make it a document

    byFlags &= 0xFE;

    // Write Word Document flag byte

    if (SSSeekWrite(lpstStream,
                    10,
                    &byFlags,
                    sizeof(BYTE),
                    &dwByteCount) != SS_STATUS_OK ||
        dwByteCount != sizeof(BYTE))
    {
        // Failed to write flag byte

        return(FALSE);
    }

    return(TRUE);
}


