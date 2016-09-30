//************************************************************************
//
// $Header:   S:/OLESSAPI/VCS/ppapi.cpv   1.4   10 Dec 1998 21:03:46   DCHI  $
//
// Description:
//  Core PowerPoint 97 access implementation.
//
//************************************************************************
// $Log:   S:/OLESSAPI/VCS/ppapi.cpv  $
// 
//    Rev 1.4   10 Dec 1998 21:03:46   DCHI
// Added offset checks in PPFindRecord() loop.
// 
//    Rev 1.3   10 Dec 1998 20:51:04   DCHI
// Added more offset checks in PPGetReferenceOffset() while loop.
// 
//    Rev 1.2   03 Feb 1998 14:22:18   DCHI
// Added embedded object support functions.
// 
//    Rev 1.1   26 Nov 1997 15:05:38   DCHI
// Added PPGetVBAInfoAtomOffset() and added dwUserEditOffset parameter
// to PPGetOLEStgOffset().
// 
//    Rev 1.0   24 Nov 1997 17:37:02   DCHI
// Initial revision.
//
//************************************************************************

#include "olestrnm.h"
#include "ppapi.h"

//********************************************************************
//
// Function:
//  BOOL PPFindRecord()
//
// Parameters:
//  lpstStream          Ptr to stream to search
//  wType               Record type to search for
//  dwStartOffset       Starting offset in stream to search
//  dwMaxDist           Maximum distance to search
//  lpdwOffset          Ptr to DWORD for record body offset
//  lpdwLength          Ptr to DWORD for record body length
//
// Description:
//  Searches the top-level records beginning from a given offset
//  and up to a given maximum distance from the beginning offset
//  for a record of the given type.  If a record with the given
//  type is found, the offset and size of the record body
//  are returned.
//
// Returns:
//  TRUE                On success
//  FALSE               On error or record not found
//
//********************************************************************

BOOL PPFindRecord
(
    LPSS_STREAM         lpstStream,
    WORD                wType,
    DWORD               dwStartOffset,
    DWORD               dwMaxDist,
    LPDWORD             lpdwOffset,
    LPDWORD             lpdwLength
)
{
    DWORD               dwEndOffset;
    DWORD               dwBytesRead;
    PP_REC_HDR_T        stRecHdr;

    dwEndOffset = dwStartOffset + dwMaxDist;
    while (dwStartOffset < dwEndOffset)
    {
        if (SSSeekRead(lpstStream,
                       dwStartOffset,
                       &stRecHdr,
                       sizeof(PP_REC_HDR_T),
                       &dwBytesRead) != SS_STATUS_OK ||
            dwBytesRead != sizeof(PP_REC_HDR_T))
        {
            // Error reading record header

            return(FALSE);
        }

        if (dwEndOffset - dwStartOffset <= sizeof(PP_REC_HDR_T))
            return(FALSE);

        // Advance past the record header

        dwStartOffset += sizeof(PP_REC_HDR_T);

        stRecHdr.dwLen = DWENDIAN(stRecHdr.dwLen);

        if (wType == WENDIAN(stRecHdr.wType))
        {
            // Found it

            if (lpdwOffset != NULL)
                *lpdwOffset = dwStartOffset;

            if (lpdwLength != NULL)
                *lpdwLength = stRecHdr.dwLen;

            return(TRUE);
        }

        if (dwEndOffset - dwStartOffset < stRecHdr.dwLen)
            return(FALSE);

        // Skip the record body

        dwStartOffset += stRecHdr.dwLen;
    }

    // Couldn't find such a record

    return(FALSE);
}


//********************************************************************
//
// Function:
//  BOOL PPGetUserEditAtomOffset()
//
// Parameters:
//  lpstStream          Ptr to PowerPoint Document stream
//  lpdwOffset          Ptr to DWORD for UserEditAtom offset
//
// Description:
//  Returns the offset of the last UserEditAtom record.
//  The function finds the offset in the CurrentUserAtom record
//  of the "Current User" stream associated with the given
//  "PowerPoint Document" stream.
//
// Returns:
//  TRUE                On success
//  FALSE               On error
//
//********************************************************************

BOOL PPGetUserEditAtomOffset
(
    LPSS_STREAM         lpstStream,
    LPDWORD             lpdwOffset
)
{
    BOOL                bResult = TRUE;
    LPSS_STREAM         lpstCurrentUserStream;
    DWORD               dwParentID;
    DWORD               dwChildID;
    LPSS_ENUM_SIBS      lpstSibs;
    DWORD               dwCurrentUserAtomOffset;
    DWORD               dwCurrentUserAtomLength;

    /////////////////////////////////////////////////////////////
    // Find the associated "Current User" stream

    // Get the parent storage of the PowerPoint Document stream

    if (SSGetParentStorage(lpstStream->lpstRoot,
                           SSStreamID(lpstStream),
                           &dwParentID,
                           &dwChildID) != SS_STATUS_OK)
        return(FALSE);

    // Allocate a sibling enumeration structure

    if (SSAllocEnumSibsStruct(lpstStream->lpstRoot,
                              &lpstSibs,
                              SS_DEF_MAX_WAITING_SIBS) != SS_STATUS_OK)
        return(FALSE);

    // Allocate a stream structure for accessing the Current User stream

    if (SSAllocStreamStruct(lpstStream->lpstRoot,
                            &lpstCurrentUserStream,
                            SS_STREAM_FLAG_DEF_BAT_CACHE) != SS_STATUS_OK)
    {
        SSFreeEnumSibsStruct(lpstStream->lpstRoot,lpstSibs);
        return(FALSE);
    }

    // Now get the sibling "Current User" stream

    SSInitEnumSibsStruct(lpstSibs,dwChildID);

    if (SSEnumSiblingEntriesCB(lpstStream->lpstRoot,
                               SSOpenStreamByNameCB,
                               gabywszCurrentUser,
                               lpstSibs,
                               lpstCurrentUserStream) != SS_STATUS_OK)
    {
        // Couldn't find it

        bResult = FALSE;
    }

    /////////////////////////////////////////////////////////////
    // Get the offset of the last UserEditAtom

    // Get the CurrentUserAtom record offset

    if (bResult == TRUE)
    {
        if (PPFindRecord(lpstCurrentUserStream,
                         ePPREC_CurrentUserAtom,
                         0,
                         SSStreamLen(lpstCurrentUserStream),
                         &dwCurrentUserAtomOffset,
                         &dwCurrentUserAtomLength) == FALSE)
        {
            // Couldn't find it

            bResult = FALSE;
        }
    }

    // Get the offset of the last UserEditAtom

    if (bResult == TRUE)
    {
        PP_REC_CurrentUserAtom_T    stCurrentUserAtom;
        DWORD                       dwBytesRead;

        if (SSSeekRead(lpstCurrentUserStream,
                       dwCurrentUserAtomOffset,
                       &stCurrentUserAtom,
                       sizeof(PP_REC_CurrentUserAtom_T),
                       &dwBytesRead) != SS_STATUS_OK ||
            dwBytesRead != sizeof(PP_REC_CurrentUserAtom_T))
        {
            // Error reading

            bResult = FALSE;
        }
        else
        {
            *lpdwOffset =
                DWENDIAN(stCurrentUserAtom.dwCurrentEditOffset);
        }
    }

    if (SSFreeStreamStruct(lpstCurrentUserStream) != SS_STATUS_OK)
        bResult = FALSE;

    if (SSFreeEnumSibsStruct(lpstStream->lpstRoot,
                             lpstSibs) != SS_STATUS_OK)
        bResult = FALSE;

    return(bResult);
}


//********************************************************************
//
// Function:
//  BOOL PPGetReferenceOffset()
//
// Parameters:
//  lpstStream          Ptr to PowerPoint Document stream
//  dwRefNum            Reference number
//  dwUserEditOffset    Offset of last UserEditAtom record
//  lpdwOffset          Ptr to DWORD for reference offset
//
// Description:
//  Given a reference number, the function returns the offset
//  of the record with the given reference number.  The function
//  accomplishes this by searching through the chain of
//  UserEditAtom records for the most recent reference.
//
//  The function will limit itself to following a maximum of 4096
//  UserEditAtom records in the chain.
//
// Returns:
//  TRUE                On success
//  FALSE               On error
//
//********************************************************************

BOOL PPGetReferenceOffset
(
    LPSS_STREAM             lpstStream,
    DWORD                   dwRefNum,
    DWORD                   dwUserEditOffset,
    LPDWORD                 lpdwOffset
)
{
    int                     i;
    PP_REC_UserEditAtom_T   stUserEdit;
    DWORD                   dwBytesRead;
    PP_REC_HDR_T            stRecHdr;
    DWORD                   dwOffset;
    DWORD                   dwMaxOffset;
    DWORD                   dwBaseRef;
    DWORD                   dwCount;
    DWORD                   dwStreamLen;

    dwStreamLen = SSStreamLen(lpstStream);
    for (i=0;i<4096;i++)
    {
        // Zero means the end of the UserEditAtoms chain

        if (dwUserEditOffset == 0)
            return(FALSE);

        /////////////////////////////////////////////////////////
        // Get the next UserEditAtom

        // Read the UserEditAtom record header

        if (SSSeekRead(lpstStream,
                       dwUserEditOffset,
                       &stRecHdr,
                       sizeof(PP_REC_HDR_T),
                       &dwBytesRead) != SS_STATUS_OK ||
            dwBytesRead != sizeof(PP_REC_HDR_T))
            return(FALSE);

        // Verify that it is a UserEditAtom

        if (WENDIAN(stRecHdr.wType) != ePPREC_UserEditAtom)
            return(FALSE);

        // Read the next UserEditAtom record

        if (SSSeekRead(lpstStream,
                       dwUserEditOffset + sizeof(PP_REC_HDR_T),
                       &stUserEdit,
                       sizeof(PP_REC_UserEditAtom_T),
                       &dwBytesRead) != SS_STATUS_OK ||
            dwBytesRead != sizeof(PP_REC_UserEditAtom_T))
            return(FALSE);

        /////////////////////////////////////////////////////////
        // Get the next UserEditAtom's persistent directory

        // Read the header of the persistent directory

        dwOffset = DWENDIAN(stUserEdit.dwPersistDirOffset);
        if (SSSeekRead(lpstStream,
                       dwOffset,
                       &stRecHdr,
                       sizeof(PP_REC_HDR_T),
                       &dwBytesRead) != SS_STATUS_OK ||
            dwBytesRead != sizeof(PP_REC_HDR_T))
            return(FALSE);

        // Verify that it is a PersistPtrIncrementalBlock

        if (WENDIAN(stRecHdr.wType) != ePPREC_PersistPtrIncrementalBlock)
            return(FALSE);

        if (dwStreamLen - dwOffset < sizeof(PP_REC_HDR_T))
            return(FALSE);

        dwOffset += sizeof(PP_REC_HDR_T);

        /////////////////////////////////////////////////////////
        // Now iterate through the references to find the
        //  one for which we are searching

        stRecHdr.dwLen = DWENDIAN(stRecHdr.dwLen);
        if (dwStreamLen - dwOffset < stRecHdr.dwLen)
            dwMaxOffset = dwStreamLen;
        else
            dwMaxOffset = dwOffset + stRecHdr.dwLen;

        while (dwOffset < dwMaxOffset)
        {
            // Read the header of the set

            if (SSSeekRead(lpstStream,
                           dwOffset,
                           &dwBaseRef,
                           sizeof(DWORD),
                           &dwBytesRead) != SS_STATUS_OK ||
                dwBytesRead != sizeof(DWORD))
                return(FALSE);

            if (dwMaxOffset - dwOffset < sizeof(DWORD))
                break;

            dwOffset += sizeof(DWORD);

            dwBaseRef = DWENDIAN(dwBaseRef);

            dwCount = dwBaseRef >> 20;
            dwBaseRef &= ((DWORD)1 << 20) - 1;
            if (dwBaseRef <= dwRefNum &&
                dwRefNum < (dwBaseRef + dwCount))
            {
                // It is in this set

                if (SSSeekRead(lpstStream,
                               dwOffset +
                                   (dwRefNum - dwBaseRef) * sizeof(DWORD),
                               lpdwOffset,
                               sizeof(DWORD),
                               &dwBytesRead) != SS_STATUS_OK ||
                    dwBytesRead != sizeof(DWORD))
                    return(FALSE);

                *lpdwOffset = DWENDIAN(*lpdwOffset);
                return(TRUE);
            }

            if ((dwMaxOffset - dwOffset) / sizeof(DWORD) < dwCount)
                break;

            dwOffset += dwCount * sizeof(DWORD);
        }

        dwUserEditOffset = DWENDIAN(stUserEdit.dwLastEditOffset);
    }

    // Could not find the reference

    return(FALSE);
}


//********************************************************************
//
// Function:
//  BOOL PPGetVBAInfoAtomOffset()
//
// Parameters:
//  lpstStream          Ptr to PowerPoint Document stream
//  dwUserEditOffset    Offset of current UserEditAtom
//  lpdwOffset          Ptr to DWORD for storage offset
//  lpdwLength          Ptr to DWORD for storage length
//
// Description:
//  Given a "PowerPoint Document" stream, the function returns
//  the offset and length of the current VBAInfoAtom record.
//
//  The function accomplishes this by looking up the
//  VBAInfoAtom record in the VBAInfo record of the List record
//  of the Document record.  The VBAInfoAtom contains in the first
//  DWORD, the reference number of the ExOleObjStg record containing
//  the compressed OLE storage object.
//
//  If dwUserEditOffset is 0, then the function will obtain it
//  itself.
//
//  Either or both of lpdwOffset and lpdwLength may be NULL if
//  the corresponding value is not desired.
//
// Returns:
//  TRUE                On success
//  FALSE               On error or no VBAInfoAtom
//
//********************************************************************

BOOL PPGetVBAInfoAtomOffset
(
    LPSS_STREAM             lpstStream,
    DWORD                   dwUserEditOffset,
    LPDWORD                 lpdwOffset,
    LPDWORD                 lpdwLength
)
{
    PP_REC_UserEditAtom_T   stUserEdit;
    DWORD                   dwBytesRead;
    PP_REC_HDR_T            stRecHdr;
    DWORD                   dwDocumentOffset;
    DWORD                   dwOffset;
    DWORD                   dwLength;

    if (dwUserEditOffset == 0)
    {
        // Get the offset of the last UserEditAtom

        if (PPGetUserEditAtomOffset(lpstStream,
                                    &dwUserEditOffset) == FALSE)
            return(FALSE);
    }

    // Read the current UserEditAtom record header

    if (SSSeekRead(lpstStream,
                   dwUserEditOffset,
                   &stRecHdr,
                   sizeof(PP_REC_HDR_T),
                   &dwBytesRead) != SS_STATUS_OK ||
            dwBytesRead != sizeof(PP_REC_HDR_T))
        return(FALSE);

    // Verify that it is a UserEditAtom

    if (WENDIAN(stRecHdr.wType) != ePPREC_UserEditAtom)
        return(FALSE);

    // Read the UserEditAtom record

    if (SSSeekRead(lpstStream,
                   dwUserEditOffset + sizeof(PP_REC_HDR_T),
                   &stUserEdit,
                   sizeof(PP_REC_UserEditAtom_T),
                   &dwBytesRead) != SS_STATUS_OK ||
        dwBytesRead != sizeof(PP_REC_UserEditAtom_T))
        return(FALSE);

    // Get the Document record offset

    if (PPGetReferenceOffset(lpstStream,
                             DWENDIAN(stUserEdit.dwDocumentRef),
                             dwUserEditOffset,
                             &dwDocumentOffset) == FALSE)
        return(FALSE);

    // Get the Document record header

    if (SSSeekRead(lpstStream,
                   dwDocumentOffset,
                   &stRecHdr,
                   sizeof(PP_REC_HDR_T),
                   &dwBytesRead) != SS_STATUS_OK ||
            dwBytesRead != sizeof(PP_REC_HDR_T))
        return(FALSE);

    // Verify that it is a Document record

    if (WENDIAN(stRecHdr.wType) != ePPREC_Document)
        return(FALSE);

    // Search for the List record within the Document record

    if (PPFindRecord(lpstStream,
                     ePPREC_List,
                     dwDocumentOffset + sizeof(PP_REC_HDR_T),
                     DWENDIAN(stRecHdr.dwLen),
                     &dwOffset,
                     &dwLength) == FALSE)
        return(FALSE);

    // Search for the VBAInfo record within the List record

    if (PPFindRecord(lpstStream,
                     ePPREC_VBAInfo,
                     dwOffset,
                     dwLength,
                     &dwOffset,
                     &dwLength) == FALSE)
        return(FALSE);

    // Search for the VBAInfoAtom record within the VBAInfo record

    if (PPFindRecord(lpstStream,
                     ePPREC_VBAInfoAtom,
                     dwOffset,
                     dwLength,
                     &dwOffset,
                     &dwLength) == FALSE)
        return(FALSE);

    if (lpdwOffset != NULL)
        *lpdwOffset = dwOffset;

    if (lpdwLength != NULL)
        *lpdwLength = dwLength;

    return(TRUE);
}


//********************************************************************
//
// Function:
//  BOOL PPGetOLEStgOffset()
//
// Parameters:
//  lpstStream          Ptr to PowerPoint Document stream
//  dwUserEditOffset    Offset of current UserEditAtom
//  lpdwOffset          Ptr to DWORD for storage offset
//  lpdwLength          Ptr to DWORD for storage length
//
// Description:
//  Given a "PowerPoint Document" stream, the function returns
//  the offset of the compressed OLE storage object and its length.
//  The function accomplishes this by first looking up the
//  VBAInfoAtom record in the VBAInfo record of the List record
//  of the Document record.  The VBAInfoAtom contains in the first
//  DWORD, the reference number of the ExOleObjStg record containing
//  the compressed OLE storage object. If the second DWORD is zero,
//  then the function assumes that the OLE storage is empty.
//
//  If dwUserEditOffset is 0, then the function will obtain it
//  itself.
//
//  Either or both of lpdwOffset and lpdwLength may be NULL if
//  the corresponding value is not desired.
//
// Returns:
//  TRUE                On success
//  FALSE               On error or no OLE storage
//
//********************************************************************

BOOL PPGetOLEStgOffset
(
    LPSS_STREAM             lpstStream,
    DWORD                   dwUserEditOffset,
    LPDWORD                 lpdwOffset,
    LPDWORD                 lpdwLength
)
{
    DWORD                   dwBytesRead;
    PP_REC_HDR_T            stRecHdr;
    DWORD                   dwOffset;
    DWORD                   dwLength;
    PP_REC_VBAInfoAtom_T    stVBAInfoAtom;

    if (dwUserEditOffset == 0)
    {
        // Get the offset of the last UserEditAtom

        if (PPGetUserEditAtomOffset(lpstStream,
                                    &dwUserEditOffset) == FALSE)
            return(FALSE);
    }

    if (PPGetVBAInfoAtomOffset(lpstStream,
                               dwUserEditOffset,
                               &dwOffset,
                               &dwLength) == FALSE)
        return(FALSE);

    // Read the VBAInfoAtom record

    if (SSSeekRead(lpstStream,
                   dwOffset,
                   &stVBAInfoAtom,
                   sizeof(PP_REC_VBAInfoAtom_T),
                   &dwBytesRead) != SS_STATUS_OK ||
        dwBytesRead != sizeof(PP_REC_VBAInfoAtom_T))
        return(FALSE);

    // Is there an active VBA storage?

    if (stVBAInfoAtom.dwFlags == 0)
        return(FALSE);

    // Look up the storage using the reference number

    if (PPGetReferenceOffset(lpstStream,
                             DWENDIAN(stVBAInfoAtom.dwExOleObjStgRef),
                             dwUserEditOffset,
                             &dwOffset) == FALSE)
        return(FALSE);

    // Get the ExOleObjStg record header

    if (SSSeekRead(lpstStream,
                   dwOffset,
                   &stRecHdr,
                   sizeof(PP_REC_HDR_T),
                   &dwBytesRead) != SS_STATUS_OK ||
            dwBytesRead != sizeof(PP_REC_HDR_T))
        return(FALSE);

    // Verify that it is a ExOleObjStg record

    if (WENDIAN(stRecHdr.wType) != ePPREC_ExOleObjStg)
        return(FALSE);

    // Verify that the length is at least 10 (6 + arbitrary 4)

    stRecHdr.dwLen = DWENDIAN(stRecHdr.dwLen);
    if (stRecHdr.dwLen < 10)
        return(FALSE);

    // Return the offset and length of the storage object

    if (lpdwOffset != NULL)
        *lpdwOffset = dwOffset + sizeof(PP_REC_HDR_T) + 6;

    if (lpdwLength != NULL)
        *lpdwLength = stRecHdr.dwLen - 6;

    return(TRUE);
}


//********************************************************************
//
// Function:
//  BOOL PPGetFirstEmbeddedObject()
//
// Parameters:
//  lpstStream          Ptr to PowerPoint Document stream
//  dwUserEditOffset    Offset of current UserEditAtom
//  lpstEmbed           Ptr to the embed state structure
//  lpdwOffset          Ptr to DWORD for storage offset
//  lpdwLength          Ptr to DWORD for storage length
//
// Description:
//  Given a "PowerPoint Document" stream, the function returns
//  the offset of the first embedded compressed OLE storage object
//  and its length.
//
//  The function accomplishes this by first looking up the
//  ExObjList record of the current Document record.
//
//  After setting the search limits, the function calls
//  PPGetNextEmbeddedObject() to get the first embedded
//  object.
//
//  If there are embedded objects then upon return lpstEmbed->bNoMore
//  will be TRUE.  Otherwise, it will be FALSE.
//
// Returns:
//  TRUE                On success
//  FALSE               On error or no OLE storage
//
//********************************************************************

BOOL PPGetFirstEmbeddedObject
(
    LPSS_STREAM             lpstStream,
    DWORD                   dwUserEditOffset,
    LPPP_EMBED              lpstEmbed,
    LPDWORD                 lpdwOffset,
    LPDWORD                 lpdwLength
)
{
    PP_REC_UserEditAtom_T   stUserEdit;
    DWORD                   dwBytesRead;
    PP_REC_HDR_T            stRecHdr;
    DWORD                   dwOffset;
    DWORD                   dwLength;

    // Initialize no more to FALSE

    lpstEmbed->bNoMore = FALSE;

    if (dwUserEditOffset == 0)
    {
        // Get the offset of the last UserEditAtom

        if (PPGetUserEditAtomOffset(lpstStream,
                                    &dwUserEditOffset) == FALSE)
            return(FALSE);
    }

    lpstEmbed->dwUserEditOffset = dwUserEditOffset;

    // Read the current UserEditAtom record header

    if (SSSeekRead(lpstStream,
                   dwUserEditOffset,
                   &stRecHdr,
                   sizeof(PP_REC_HDR_T),
                   &dwBytesRead) != SS_STATUS_OK ||
            dwBytesRead != sizeof(PP_REC_HDR_T))
        return(FALSE);

    // Verify that it is a UserEditAtom

    if (WENDIAN(stRecHdr.wType) != ePPREC_UserEditAtom)
        return(FALSE);

    // Read the UserEditAtom record

    if (SSSeekRead(lpstStream,
                   dwUserEditOffset + sizeof(PP_REC_HDR_T),
                   &stUserEdit,
                   sizeof(PP_REC_UserEditAtom_T),
                   &dwBytesRead) != SS_STATUS_OK ||
        dwBytesRead != sizeof(PP_REC_UserEditAtom_T))
        return(FALSE);

    // Get the Document record offset

    if (PPGetReferenceOffset(lpstStream,
                             DWENDIAN(stUserEdit.dwDocumentRef),
                             dwUserEditOffset,
                             &dwOffset) == FALSE)
        return(FALSE);

    // Get the Document record header

    if (SSSeekRead(lpstStream,
                   dwOffset,
                   &stRecHdr,
                   sizeof(PP_REC_HDR_T),
                   &dwBytesRead) != SS_STATUS_OK ||
        dwBytesRead != sizeof(PP_REC_HDR_T))
        return(FALSE);

    // Verify that it is a Document record

    if (WENDIAN(stRecHdr.wType) != ePPREC_Document)
        return(FALSE);

    // Search for the ExObjList record within the Document record

    if (PPFindRecord(lpstStream,
                     ePPREC_ExObjList,
                     dwOffset + sizeof(PP_REC_HDR_T),
                     DWENDIAN(stRecHdr.dwLen),
                     &dwOffset,
                     &dwLength) == FALSE)
    {
        // Assume that there are none

        lpstEmbed->bNoMore = TRUE;
        return(TRUE);
    }

    // Set the maximum offset

    lpstEmbed->dwMaxEmbedOffset = dwOffset + dwLength;

    lpstEmbed->dwNextSearchOffset = dwOffset;

    return PPGetNextEmbeddedObject(lpstStream,
                                   lpstEmbed,
                                   lpdwOffset,
                                   lpdwLength);
}


//********************************************************************
//
// Function:
//  BOOL PPGetNextEmbeddedObject()
//
// Parameters:
//  lpstStream          Ptr to PowerPoint Document stream
//  lpstEmbed           Ptr to the embed state structure
//  lpdwOffset          Ptr to DWORD for storage offset
//  lpdwLength          Ptr to DWORD for storage length
//
// Description:
//  Given a "PowerPoint Document" stream, the function returns
//  the offset of the next embedded compressed OLE storage object
//  and its length.
//
//  The search begins at lpstEmbed->dwNextSearchOffset.
//
//  The function searches for the next ExEmbed record with an
//  ExOleObjAtom that has its dwType field set to zero.  The
//  dwObjStgDataRef contains the reference number of the ExOleObjStg
//  record containing the embedded object.
//
//  If there an embedded object is not found, then upon return
//  lpstEmbed->bNoMore will be TRUE.  Otherwise, it will be FALSE.
//
// Returns:
//  TRUE                On success
//  FALSE               On error or no OLE storage
//
//********************************************************************

BOOL PPGetNextEmbeddedObject
(
    LPSS_STREAM             lpstStream,
    LPPP_EMBED              lpstEmbed,
    LPDWORD                 lpdwOffset,
    LPDWORD                 lpdwLength
)
{
    DWORD                   dwBytesRead;
    PP_REC_HDR_T            stRecHdr;
    DWORD                   dwOffset;
    DWORD                   dwLength;
    PP_REC_ExOleObjAtom_T   stExOleObjAtom;

    // Search for the next ExEmbed record within the ExObjList record

    while (lpstEmbed->dwNextSearchOffset < lpstEmbed->dwMaxEmbedOffset)
    {
        if (PPFindRecord(lpstStream,
                         ePPREC_ExEmbed,
                         lpstEmbed->dwNextSearchOffset,
                         lpstEmbed->dwMaxEmbedOffset -
                             lpstEmbed->dwNextSearchOffset,
                         &dwOffset,
                         &dwLength) == FALSE)
        {
            // Assume that there are none

            lpstEmbed->bNoMore = TRUE;
            return(TRUE);
        }

        // Set the next search offset

        lpstEmbed->dwNextSearchOffset = dwOffset + dwLength;

        // Now search for the ExOleObjAtom record within the ExEmbed record

        if (PPFindRecord(lpstStream,
                         ePPREC_ExOleObjAtom,
                         dwOffset,
                         dwLength,
                         &dwOffset,
                         &dwLength) == FALSE)
        {
            // Could not find an ExOleObjAtom, so assume it is not
            //  an embedded object

            continue;
        }

        // Read the ExOleObjAtom record

        if (SSSeekRead(lpstStream,
                       dwOffset,
                       &stExOleObjAtom,
                       sizeof(PP_REC_ExOleObjAtom_T),
                       &dwBytesRead) != SS_STATUS_OK ||
            dwBytesRead != sizeof(PP_REC_ExOleObjAtom_T))
            return(FALSE);

        // Is the object embedded?

        stExOleObjAtom.dwType = DWENDIAN(stExOleObjAtom.dwType);
        if (stExOleObjAtom.dwType == 0)
        {
            // It is embedded

            stExOleObjAtom.dwObjStgDataRef =
                DWENDIAN(stExOleObjAtom.dwObjStgDataRef);

            // Look up the storage using the reference number

            if (PPGetReferenceOffset(lpstStream,
                                     stExOleObjAtom.dwObjStgDataRef,
                                     lpstEmbed->dwUserEditOffset,
                                     &dwOffset) == FALSE)
                return(FALSE);

            // Set the type in the embed structure

            lpstEmbed->dwType = DWENDIAN(stExOleObjAtom.dwSubType);

            // Get the ExOleObjStg record header

            if (SSSeekRead(lpstStream,
                           dwOffset,
                           &stRecHdr,
                           sizeof(PP_REC_HDR_T),
                           &dwBytesRead) != SS_STATUS_OK ||
                dwBytesRead != sizeof(PP_REC_HDR_T))
                return(FALSE);

            // Verify that it is a ExOleObjStg record

            if (WENDIAN(stRecHdr.wType) != ePPREC_ExOleObjStg)
                return(FALSE);

            // Return the offset and length of the storage object

            if (lpdwOffset != NULL)
                *lpdwOffset = dwOffset + sizeof(PP_REC_HDR_T) + 6;

            if (lpdwLength != NULL)
                *lpdwLength = stRecHdr.dwLen - 6;

            return(TRUE);
        }
    }

    lpstEmbed->bNoMore = TRUE;
    return(TRUE);
}



