//************************************************************************
//
// $Header:   S:/OLESSAPI/VCS/SUMMARY.CPv   1.1   07 Jan 1999 16:04:02   DCHI  $
//
// Description:
//  Summary information access implementation.
//
//************************************************************************
// $Log:   S:/OLESSAPI/VCS/SUMMARY.CPv  $
// 
//    Rev 1.1   07 Jan 1999 16:04:02   DCHI
// Implemented SSDocSumInfoRemDigiSig() and SSGetDocSumInfoStream().
// 
//    Rev 1.0   26 Mar 1998 18:36:30   DCHI
// Initial revision.
// 
//************************************************************************

#include "olessapi.h"
#include "olestrnm.h"

typedef struct tagSSINFO_STREAMS
{
    DWORD       dwSummaryInfoStreamID;
    DWORD       dwDocSummaryInfoStreamID;
} SSINFO_STREAMS_T, FAR *LPSSINFO_STREAMS;

//********************************************************************
//
// Function:
//  int SSGetInfoStreamsCB()
//
// Parameters:
//  lpstEntry               Ptr to the entry
//  dwIndex                 The entry's index in the directory
//  lpvCookie               Ptr to the INFO_STREAMS_T structure
//
// Description:
//  The cookie is assumed to point to a SSINFO_STREAMS structure
//  for receiving the indexes of the SummaryInformation and
//  DocumentSummaryInformation streams.
//
// Returns:
//  SS_ENUM_CB_STATUS_RETURN        If both streams found
//  SS_ENUM_CB_STATUS_CONTINUE      If both streams are not found
//
//********************************************************************

int SSGetInfoStreamsCB
(
    LPSS_DIR_ENTRY      lpstEntry,
    DWORD               dwIndex,
    LPVOID              lpvCookie
)
{
    LPSSINFO_STREAMS    lpstInfo;

    if (lpstEntry->byMSE == STGTY_STREAM)
    {
        // Is it the desired stream?

        if (SSWStrNCmp(lpstEntry->uszName,
                       (LPWORD)gabywsz5SummaryInformation,
                       SS_MAX_NAME_LEN) == 0)
        {
            lpstInfo = (LPSSINFO_STREAMS)lpvCookie;

            lpstInfo->dwSummaryInfoStreamID = dwIndex;
            if (lpstInfo->dwDocSummaryInfoStreamID != 0)
                return(SS_ENUM_CB_STATUS_RETURN);
        }
        else
        if (SSWStrNCmp(lpstEntry->uszName,
                       (LPWORD)gabywsz5DocumentSummaryInformation,
                       SS_MAX_NAME_LEN) == 0)
        {
            lpstInfo = (LPSSINFO_STREAMS)lpvCookie;

            lpstInfo->dwDocSummaryInfoStreamID = dwIndex;
            if (lpstInfo->dwSummaryInfoStreamID != 0)
                return(SS_ENUM_CB_STATUS_RETURN);
        }
    }

    return(SS_ENUM_CB_STATUS_CONTINUE);
}


//********************************************************************
//
// Function:
//  BOOL SSGetInfoStreams()
//
// Parameters:
//  lpstRoot                Ptr to root structure for storage
//  dwDocStreamID           Doc stream ID of info streams to search
//  lpstInfoStreams         Ptr to SSINFO_STREAMS structure for IDs
//
// Description:
//  The function returns in the lpstInfoStreams structure the
//  stream IDs of the SummaryInformation and
//  DocumentSummaryInformation streams associated with the
//  given document stream.  If either or both are not found, the
//  corresponding field will have a stream ID of zero.
//
// Returns:
//  TRUE                    On success
//  FALSE                   On error
//
//********************************************************************

BOOL SSGetInfoStreams
(
    LPSS_ROOT           lpstRoot,
    DWORD               dwDocStreamID,
    LPSSINFO_STREAMS    lpstInfoStreams
)
{
    LPSS_ENUM_SIBS      lpstSibs;
    DWORD               dwParentID;
    DWORD               dwChildID;

    // Get the parent and child of the parent of the document stream

    if (SSGetParentStorage(lpstRoot,
                           dwDocStreamID,
                           &dwParentID,
                           &dwChildID) != SS_STATUS_OK)
        return(FALSE);

    // Allocate a sibling enumeration structure

    if (SSAllocEnumSibsStruct(lpstRoot,
                              &lpstSibs,
                              SS_DEF_MAX_WAITING_SIBS) != SS_STATUS_OK)
        return(FALSE);

    SSInitEnumSibsStruct(lpstSibs,dwChildID);

    // Initialize to not found

    lpstInfoStreams->dwSummaryInfoStreamID = 0;
    lpstInfoStreams->dwDocSummaryInfoStreamID = 0;

    // Find the information streams

    SSEnumSiblingEntriesCB(lpstRoot,
                           SSGetInfoStreamsCB,
                           lpstInfoStreams,
                           lpstSibs,
                           NULL);

    // Free the enumeration structure

    SSFreeEnumSibsStruct(lpstRoot,lpstSibs);

    return(TRUE);
}


//********************************************************************
//
// Function:
//  BOOL SSEraseSummaryInfoData()
//
// Parameters:
//  lpstStream              Ptr to SummaryInformation stream
//
// Description:
//  The function goes through the first property set of the
//  given SummaryInformation stream and erases the following:
//
//      SSPID_TITLE             Set to the empty string
//      SSPID_SUBJECT           Set to the empty string
//      SSPID_AUTHOR            Set to the empty string
//      SSPID_KEYWORDS          Set to the empty string
//      SSPID_COMMENTS          Set to the empty string
//      SSPID_LASTAUTHOR        Set to the empty string
//      SSPID_REVNUMBER         Set to the empty string
//
//      SSPID_EDITTIME          Set to 0
//      SSPID_LASTPRINTED       Set to 0
//      SSPID_CREATE_DTM        Set to 0
//      SSPID_LASTSAVE_DTM      Set to 0
//
//      SSPID_PAGECOUNT         Set to 1
//      SSPID_WORDCOUNT         Set to 0
//      SSPID_CHARCOUNT         Set to 0
//
// Returns:
//  TRUE                    On success
//  FALSE                   On error
//
//********************************************************************

#define SSPID_CODEPAGE      0x0001  // Code page
#define SSPID_TITLE         0x0002  // Title
#define SSPID_SUBJECT       0x0003  // Subject
#define SSPID_AUTHOR        0x0004  // Author
#define SSPID_KEYWORDS      0x0005  // Keywords
#define SSPID_COMMENTS      0x0006  // Comments
#define SSPID_TEMPLATE      0x0007  // Template
#define SSPID_LASTAUTHOR    0x0008  // Last saved by
#define SSPID_REVNUMBER     0x0009  // Revision number
#define SSPID_EDITTIME      0x000A  // Total editing time
#define SSPID_LASTPRINTED   0x000B  // Last printed
#define SSPID_CREATE_DTM    0x000C  // Created
#define SSPID_LASTSAVE_DTM  0x000D  // Last saved
#define SSPID_PAGECOUNT     0x000E  // Pages
#define SSPID_WORDCOUNT     0x000F  // Words
#define SSPID_CHARCOUNT     0x0010  // Characters
#define SSPID_THUMBNAIL     0x0011  // Thumbnail view
#define SSPID_APPNAME       0x0012  // Application name
#define SSPID_SECURITY      0x0013  // Security

BOOL SSEraseSummaryInfoData
(
    LPSS_STREAM         lpstStream
)
{
    DWORD               dwBaseOffset;
    DWORD               dwOffset;
    DWORD               dwPropOffset;
    DWORD               dwPropCount;
    DWORD               dwByteCount;
    DWORD               dwI;
    DWORD               dwProperty;
    DWORD               dwTemp;

    // Assume offset 0x2C contains the offset of the property set header

    if (SSSeekRead(lpstStream,
                   0x2C,
                   &dwOffset,
                   sizeof(DWORD),
                   &dwByteCount) != SS_STATUS_OK ||
        dwByteCount != sizeof(DWORD))
        return(FALSE);

    dwOffset = DWENDIAN(dwOffset);

    // Skip the property set size

    dwOffset += sizeof(DWORD);
    dwBaseOffset = dwOffset;

    // Get the property count

    if (SSSeekRead(lpstStream,
                   dwOffset,
                   &dwPropCount,
                   sizeof(DWORD),
                   &dwByteCount) != SS_STATUS_OK ||
        dwByteCount != sizeof(DWORD))
        return(FALSE);

    // Skip the property count

    dwOffset += sizeof(DWORD);

    for (dwI=0;dwI<dwPropCount;dwI++)
    {
        // Read the property

        if (SSSeekRead(lpstStream,
                       dwOffset,
                       &dwProperty,
                       sizeof(DWORD),
                       &dwByteCount) != SS_STATUS_OK ||
            dwByteCount != sizeof(DWORD))
            return(FALSE);

        dwOffset += sizeof(DWORD);

        dwProperty = DWENDIAN(dwProperty);

        // Read the property offset

        if (SSSeekRead(lpstStream,
                       dwOffset,
                       &dwPropOffset,
                       sizeof(DWORD),
                       &dwByteCount) != SS_STATUS_OK ||
            dwByteCount != sizeof(DWORD))
            return(FALSE);

        dwOffset += sizeof(DWORD);

        dwPropOffset = DWENDIAN(dwPropOffset);
        dwPropOffset += dwBaseOffset;

        // Erase depending on property

        switch (dwProperty)
        {
            case SSPID_TITLE:
            case SSPID_SUBJECT:
            case SSPID_AUTHOR:
            case SSPID_KEYWORDS:
            case SSPID_COMMENTS:
            case SSPID_LASTAUTHOR:
            case SSPID_REVNUMBER:
                // Read the string length

                if (SSSeekRead(lpstStream,
                               dwPropOffset,
                               &dwTemp,
                               sizeof(DWORD),
                               &dwByteCount) != SS_STATUS_OK ||
                    dwByteCount != sizeof(DWORD))
                    return(FALSE);

                dwTemp = DWENDIAN(dwTemp);

                // Zero out the string

                if (SSWriteZeroes(lpstStream,
                                  dwPropOffset + sizeof(DWORD),
                                  dwTemp) != SS_STATUS_OK)
                    return(FALSE);

                // Set the length to one

                dwTemp = 1;
                dwTemp = DWENDIAN(dwTemp);
                if (SSSeekWrite(lpstStream,
                                dwPropOffset,
                                &dwTemp,
                                sizeof(DWORD),
                                &dwByteCount) != SS_STATUS_OK ||
                    dwByteCount != sizeof(DWORD))
                    return(FALSE);

                break;

            case SSPID_EDITTIME:
            case SSPID_LASTPRINTED:
            case SSPID_CREATE_DTM:
            case SSPID_LASTSAVE_DTM:
                // Zero out the time

                if (SSWriteZeroes(lpstStream,
                                  dwPropOffset,
                                  8) != SS_STATUS_OK)
                    return(FALSE);

                break;

            case SSPID_PAGECOUNT:
                // Set the page count to one

                dwTemp = 1;
                dwTemp = DWENDIAN(dwTemp);

                if (SSSeekWrite(lpstStream,
                                dwPropOffset,
                                &dwTemp,
                                sizeof(DWORD),
                                &dwByteCount) != SS_STATUS_OK ||
                    dwByteCount != sizeof(DWORD))
                    return(FALSE);

                break;

            case SSPID_WORDCOUNT:
            case SSPID_CHARCOUNT:
                // Set the counts to zero

                if (SSWriteZeroes(lpstStream,
                                  dwPropOffset,
                                  4) != SS_STATUS_OK)
                    return(FALSE);

                break;

            default:
                break;
        }
    }

    return(TRUE);
}


//********************************************************************
//
// Function:
//  BOOL SSEraseDocSummaryInfoData()
//
// Parameters:
//  lpstStream              Ptr to DocumentSummaryInformation stream
//
// Description:
//  The function goes through the first property set of the
//  given DocumentSummaryInformation stream and erases the following:
//
//      SSPID_CATEGORY      Set to empty string
//      SSPID_MANAGER       Set to empty string
//      SSPID_COMPANY       Set to empty string
//
//      SSPID_DOCPARTS      All strings set to empty strings
//
// Returns:
//  TRUE                    On success
//  FALSE                   On error
//
//********************************************************************

#define SSPID_CATEGORY      0x0002  // Category
#define SSPID_PRESFORMAT    0x0003  // Presentation target
#define SSPID_BYTECOUNT     0x0004  // Bytes
#define SSPID_LINECOUNT     0x0005  // Lines
#define SSPID_PARCOUNT      0x0006  // Paragraphs
#define SSPID_SLIDECOUNT    0x0007  // Slides
#define SSPID_NOTECOUNT     0x0008  // Notes
#define SSPID_HIDDENCOUNT   0x0009  // Hidden slides
#define SSPID_MMCLIPCOUNT   0x000A  // MM clips
#define SSPID_SCALE         0x000B  // Scale crop
#define SSPID_HEADINGPAIR   0x000C  // Heading pairs
#define SSPID_DOCPARTS      0x000D  // Titles of parts
#define SSPID_MANAGER       0x000E  // Manager
#define SSPID_COMPANY       0x000F  // Company
#define SSPID_LINKSDIRTY    0x0010  // Links up to date

#define SSPID_DIGITAL_SIG   0x0018  // Digital signature

BOOL SSEraseDocSummaryInfoData
(
    LPSS_STREAM         lpstStream
)
{
    DWORD               dwBaseOffset;
    DWORD               dwOffset;
    DWORD               dwPropOffset;
    DWORD               dwPropCount;
    DWORD               dwByteCount;
    DWORD               dwI;
    DWORD               dwProperty;
    DWORD               dwTemp;

    // Assume offset 0x2C contains the offset of the property set header

    if (SSSeekRead(lpstStream,
                   0x2C,
                   &dwOffset,
                   sizeof(DWORD),
                   &dwByteCount) != SS_STATUS_OK ||
        dwByteCount != sizeof(DWORD))
        return(FALSE);

    dwOffset = DWENDIAN(dwOffset);

    // Skip the property set size

    dwOffset += sizeof(DWORD);
    dwBaseOffset = dwOffset;

    // Get the property count

    if (SSSeekRead(lpstStream,
                   dwOffset,
                   &dwPropCount,
                   sizeof(DWORD),
                   &dwByteCount) != SS_STATUS_OK ||
        dwByteCount != sizeof(DWORD))
        return(FALSE);

    // Skip the property count

    dwOffset += sizeof(DWORD);

    for (dwI=0;dwI<dwPropCount;dwI++)
    {
        // Read the property

        if (SSSeekRead(lpstStream,
                       dwOffset,
                       &dwProperty,
                       sizeof(DWORD),
                       &dwByteCount) != SS_STATUS_OK ||
            dwByteCount != sizeof(DWORD))
            return(FALSE);

        dwOffset += sizeof(DWORD);

        dwProperty = DWENDIAN(dwProperty);

        // Read the property offset

        if (SSSeekRead(lpstStream,
                       dwOffset,
                       &dwPropOffset,
                       sizeof(DWORD),
                       &dwByteCount) != SS_STATUS_OK ||
            dwByteCount != sizeof(DWORD))
            return(FALSE);

        dwOffset += sizeof(DWORD);

        dwPropOffset = DWENDIAN(dwPropOffset);
        dwPropOffset += dwBaseOffset;

        // Erase depending on property

        switch (dwProperty)
        {
            case SSPID_CATEGORY:
            case SSPID_MANAGER:
            case SSPID_COMPANY:
                // Read the string length

                if (SSSeekRead(lpstStream,
                               dwPropOffset,
                               &dwTemp,
                               sizeof(DWORD),
                               &dwByteCount) != SS_STATUS_OK ||
                    dwByteCount != sizeof(DWORD))
                    return(FALSE);

                dwTemp = DWENDIAN(dwTemp);

                // Zero out the string

                if (SSWriteZeroes(lpstStream,
                                  dwPropOffset + sizeof(DWORD),
                                  dwTemp) != SS_STATUS_OK)
                    return(FALSE);

                // Set the length to one

                dwTemp = 1;
                dwTemp = DWENDIAN(dwTemp);
                if (SSSeekWrite(lpstStream,
                                dwPropOffset,
                                &dwTemp,
                                sizeof(DWORD),
                                &dwByteCount) != SS_STATUS_OK ||
                    dwByteCount != sizeof(DWORD))
                    return(FALSE);

                break;

            case SSPID_DOCPARTS:
            {
                DWORD       dwStrOffset;
                DWORD       dwCount;

                // Read the string count

                if (SSSeekRead(lpstStream,
                               dwPropOffset,
                               &dwCount,
                               sizeof(DWORD),
                               &dwByteCount) != SS_STATUS_OK ||
                    dwByteCount != sizeof(DWORD))
                    return(FALSE);

                dwCount = DWENDIAN(dwCount);

                // Set all titles to empty strings

                dwStrOffset = dwPropOffset + sizeof(DWORD);
                while (dwCount-- != 0)
                {
                    // Read the string length

                    if (SSSeekRead(lpstStream,
                                   dwStrOffset,
                                   &dwTemp,
                                   sizeof(DWORD),
                                   &dwByteCount) != SS_STATUS_OK ||
                        dwByteCount != sizeof(DWORD))
                        return(FALSE);

                    dwTemp = DWENDIAN(dwTemp);

                    // Zero out the string

                    if (SSWriteZeroes(lpstStream,
                                      dwStrOffset,
                                      sizeof(DWORD) + dwTemp) != SS_STATUS_OK)
                        return(FALSE);

                    // Advance to the next string

                    dwStrOffset += sizeof(DWORD) + dwTemp;
                }

                // Read the string count

                if (SSSeekRead(lpstStream,
                               dwPropOffset,
                               &dwCount,
                               sizeof(DWORD),
                               &dwByteCount) != SS_STATUS_OK ||
                    dwByteCount != sizeof(DWORD))
                    return(FALSE);

                dwCount = DWENDIAN(dwCount);

                // Set all lengths to one

                dwTemp = 1;
                dwTemp = DWENDIAN(dwTemp);
                dwStrOffset = dwPropOffset + sizeof(DWORD);
                while (dwCount-- != 0)
                {
                    // Write the string length

                    if (SSSeekWrite(lpstStream,
                                    dwStrOffset,
                                    &dwTemp,
                                    sizeof(DWORD),
                                    &dwByteCount) != SS_STATUS_OK ||
                        dwByteCount != sizeof(DWORD))
                        return(FALSE);

                    dwStrOffset += sizeof(DWORD) + 1;
                }

                break;
            }

            default:
                break;
        }
    }

    return(TRUE);
}


//********************************************************************
//
// Function:
//  BOOL SSClearSummaryInfo()
//
// Parameters:
//  lpstStream              Ptr to document stream
//
// Description:
//  The function erases user information from the SummaryInformation
//  and DocumentSummaryInformation streams.
//
// Returns:
//  TRUE                    On success
//  FALSE                   On error
//
//********************************************************************

BOOL SSClearSummaryInfo
(
    LPSS_STREAM         lpstStream
)
{
    SSINFO_STREAMS_T    stInfo;
    LPSS_STREAM         lpstInfoStream;

    // Get the info streams

    if (SSGetInfoStreams(lpstStream->lpstRoot,
                         SSStreamID(lpstStream),
                         &stInfo) == FALSE)
        return(FALSE);

    // Allocate a stream structure

    if (SSAllocStreamStruct(lpstStream->lpstRoot,
                            &lpstInfoStream,
                            SS_STREAM_FLAG_DEF_BAT_CACHE) != SS_STATUS_OK)
        return(FALSE);

    if (stInfo.dwSummaryInfoStreamID != 0)
    {
        // Open the SummaryInformation stream

        if (SSOpenStreamAtIndex(lpstInfoStream,
                                stInfo.dwSummaryInfoStreamID) !=
            SS_STATUS_OK)
        {
            SSFreeStreamStruct(lpstInfoStream);
            return(FALSE);
        }

        // Erase SummaryInformation data

        if (SSEraseSummaryInfoData(lpstInfoStream) == FALSE)
        {
            SSFreeStreamStruct(lpstInfoStream);
            return(FALSE);
        }
    }

    if (stInfo.dwDocSummaryInfoStreamID != 0)
    {
        // Open the DocumentSummaryInformation stream

        if (SSOpenStreamAtIndex(lpstInfoStream,
                                stInfo.dwDocSummaryInfoStreamID) !=
            SS_STATUS_OK)
        {
            SSFreeStreamStruct(lpstInfoStream);
            return(FALSE);
        }

        // Erase SummaryInformation data

        if (SSEraseDocSummaryInfoData(lpstInfoStream) == FALSE)
        {
            SSFreeStreamStruct(lpstInfoStream);
            return(FALSE);
        }
    }

    SSFreeStreamStruct(lpstInfoStream);

    return(TRUE);
}


//********************************************************************
//
// Function:
//  BOOL SSDocSumInfoRemProp()
//
// Parameters:
//  lpstStream              Ptr to document summary info stream
//  dwBaseOffset            Base offset of property to remove
//  dwPropToRemove          Property to remove
//  lpbFound                Ptr to BOOL for property existence query
//
// Description:
//  The function erases the given property from the given
//  DocumentSummaryInformation stream.
//
//  Returns FALSE only on error during modification.
//
// Returns:
//  TRUE                    On success
//  FALSE                   On error
//
//********************************************************************

BOOL SSDocSumInfoRemProp
(
    LPSS_STREAM         lpstStream,
    DWORD               dwBaseOffset,
    DWORD               dwPropToRemove,
    LPBOOL              lpbFound
)
{
    DWORD               dwInfoOffset;
    DWORD               dwSetSize;
    DWORD               dwByteCount;
    DWORD               dwPropCount;
    DWORD               dwI;
    DWORD               dwPropID;

    dwInfoOffset = dwBaseOffset;

    if (lpbFound != NULL)
        *lpbFound = FALSE;

    // Read the property set size

    if (SSSeekRead(lpstStream,
                   dwInfoOffset,
                   &dwSetSize,
                   sizeof(DWORD),
                   &dwByteCount) != SS_STATUS_OK ||
        dwByteCount != sizeof(DWORD))
        return(FALSE);

    dwInfoOffset += sizeof(DWORD);
    dwSetSize = DWENDIAN(dwSetSize);

    // Get the property count

    if (SSSeekRead(lpstStream,
                   dwInfoOffset,
                   &dwPropCount,
                   sizeof(DWORD),
                   &dwByteCount) != SS_STATUS_OK ||
        dwByteCount != sizeof(DWORD))
        return(TRUE);

    dwInfoOffset += sizeof(DWORD);
    dwPropCount = DWENDIAN(dwPropCount);

    // Validate the set size and the property count

    if (dwBaseOffset + dwSetSize > SSStreamLen(lpstStream) ||
        dwPropCount == 0 ||
        dwPropCount > (SSStreamLen(lpstStream) - dwBaseOffset) / 8)
        return(TRUE);

    for (dwI=0;dwI<dwPropCount;dwI++)
    {
        // Read the property ID

        if (SSSeekRead(lpstStream,
                       dwInfoOffset,
                       &dwPropID,
                       sizeof(DWORD),
                       &dwByteCount) != SS_STATUS_OK ||
            dwByteCount != sizeof(DWORD))
            return(FALSE);

        dwPropID = DWENDIAN(dwPropID);

        if (dwPropID == dwPropToRemove)
            break;

        dwInfoOffset += sizeof(DWORD) + sizeof(DWORD);
    }

    // Return immediately if the property was not found

    if (dwI == dwPropCount)
        return(TRUE);

    // Indicate that the property was found

    if (lpbFound != NULL)
        *lpbFound = TRUE;

    // Shift the properties ahead back by one slot

    if (dwI + 1 != dwPropCount)
    {
        if (SSCopyBytes(lpstStream,
                        dwInfoOffset + sizeof(DWORD) + sizeof(DWORD),
                        dwInfoOffset,
                        (dwPropCount - dwI - 1) *
                            (sizeof(DWORD) + sizeof(DWORD))) != SS_STATUS_OK)
            return(FALSE);
    }

    // Decrement the property count and write it out

    --dwPropCount;
    dwPropCount = DWENDIAN(dwPropCount);
    if (SSSeekWrite(lpstStream,
                    dwBaseOffset + sizeof(DWORD),
                    &dwPropCount,
                    sizeof(DWORD),
                    &dwByteCount) != SS_STATUS_OK ||
        dwByteCount != sizeof(DWORD))
        return(FALSE);

    return(TRUE);
}


//********************************************************************
//
// Function:
//  BOOL SSDocSumInfoRemDigiSig()
//
// Parameters:
//  lpstStream              Ptr to document summary info stream
//  lpbFound                Ptr to BOOL for property existence query
//
// Description:
//  The function erases the digital property from the given
//  DocumentSummaryInformation stream.
//
//  Returns FALSE only on error during modification.
//
// Returns:
//  TRUE                    On success
//  FALSE                   On error
//
//********************************************************************

#ifdef SYM_WIN16
#pragma data_seg("FAR_DATA_0")
#endif

// FMTID for the Microsoft Office Summary Information property set

BYTE FAR gabyMOSIFMTIT[] =
{
    0x02, 0xD5, 0xCD, 0xD5,
    0x9C, 0x2E,
    0x1B, 0x10,
    0x93, 0x97,
    0x08, 0x00, 0x2B, 0x2C, 0xF9, 0xAE
};

#ifdef SYM_WIN16
#pragma data_seg()
#endif

BOOL SSDocSumInfoRemDigiSig
(
    LPSS_STREAM         lpstStream,
    LPBOOL              lpbFound
)
{
    BYTE                abyGUID[16];
    DWORD               dwByteCount;
    int                 i;
    DWORD               dwOffset;

    // Assume that the first section is the Microsoft
    //  Office Summary Information property set

    // Read the GUID

    if (SSSeekRead(lpstStream,
                   0x1C,
                   abyGUID,
                   16,
                   &dwByteCount) != SS_STATUS_OK ||
        dwByteCount != 16)
        return(FALSE);

    // Verify the GUID

    for (i=0;i<16;i++)
    {
        if (abyGUID[i] != gabyMOSIFMTIT[i])
            return(TRUE);
    }

    // Get the offset of the property set header

    if (SSSeekRead(lpstStream,
                   0x2C,
                   &dwOffset,
                   sizeof(DWORD),
                   &dwByteCount) != SS_STATUS_OK ||
        dwByteCount != sizeof(DWORD))
        return(FALSE);

    dwOffset = DWENDIAN(dwOffset);

    // Validate that the offset is within bounds

    if (dwOffset > SSStreamLen(lpstStream))
        return(TRUE);

    // Remove the digital signature if any

    if (SSDocSumInfoRemProp(lpstStream,
                            dwOffset,
                            SSPID_DIGITAL_SIG,
                            lpbFound) == FALSE)
        return(FALSE);

    return(TRUE);
}


//********************************************************************
//
// Function:
//  BOOL SSGetDocSumInfoStream()
//
// Parameters:
//  lpstRoot                Ptr to root structure for storage
//  dwDocStreamID           Doc stream ID of info streams to search
//  lpstStream              Ptr to stream structure to use
//
// Description:
//  The function opens the DocumentSummaryInformation stream
//  associated with the given stream ID.
//
// Returns:
//  TRUE                    On success
//  FALSE                   On error
//
//********************************************************************

BOOL SSGetDocSumInfoStream
(
    LPSS_ROOT           lpstRoot,
    DWORD               dwDocStreamID,
    LPSS_STREAM         lpstStream
)
{
    LPSS_ENUM_SIBS      lpstSibs;
    DWORD               dwParentID;
    DWORD               dwChildID;

    // Get the parent and child of the parent of the document stream

    if (SSGetParentStorage(lpstRoot,
                           dwDocStreamID,
                           &dwParentID,
                           &dwChildID) != SS_STATUS_OK)
        return(FALSE);

    // Allocate a sibling enumeration structure

    if (SSAllocEnumSibsStruct(lpstRoot,
                              &lpstSibs,
                              SS_DEF_MAX_WAITING_SIBS) != SS_STATUS_OK)
        return(FALSE);

    SSInitEnumSibsStruct(lpstSibs,dwChildID);

    // Find the information streams

    if (SSEnumSiblingEntriesCB(lpstRoot,
                               SSOpenStreamByNameCB,
                               gabywsz5DocumentSummaryInformation,
                               lpstSibs,
                               lpstStream) != SS_STATUS_OK)
    {
        // Free the enumeration structure

        SSFreeEnumSibsStruct(lpstRoot,lpstSibs);

        return(FALSE);
    }

    // Free the enumeration structure

    SSFreeEnumSibsStruct(lpstRoot,lpstSibs);

    return(TRUE);
}



