//************************************************************************
//
// $Header:   S:/OLESSAPI/VCS/o97mod.cpv   1.12   06 Jan 1999 14:23:06   DCHI  $
//
// Description:
//  Core Office 97 VBA module binary access implementation.
//
//************************************************************************
// $Log:   S:/OLESSAPI/VCS/o97mod.cpv  $
// 
//    Rev 1.12   06 Jan 1999 14:23:06   DCHI
// Fixed O97ModReplaceVBASrc() for case where both orig and new are < 4K.
// 
//    Rev 1.11   08 Dec 1998 12:39:00   DCHI
// Added implementation of O97ModReplaceVBASrc().
// 
//    Rev 1.10   09 Nov 1998 13:41:36   DCHI
// Added #pragma data_seg("FAR_DATA_0") for global FAR data.
// 
//    Rev 1.9   04 Aug 1998 13:46:30   DCHI
// Modified O97GetIdentifierTableOffset() to handle big-endian streams as well.
// 
//    Rev 1.8   23 Jul 1998 12:42:08   DCHI
// Corrected incorrect operand count incrementation for VAR_DEFN in CRCBuf().
// 
//    Rev 1.7   15 Jul 1998 12:00:38   DCHI
// Modified ModAccGetDataPtr() to check offset within range first.
// 
//    Rev 1.6   13 Jul 1998 11:08:40   DCHI
// Added support for Office 9 modules.
// 
//    Rev 1.5   08 Jul 1998 15:58:12   DCHI
// Added sanity checks in CRCBuf() and CRCFast().
// 
//    Rev 1.4   24 Jun 1998 11:00:26   DCHI
// - Added implementations for:
//     O97Get_VBA_PROJECTEndian()
//     O97GetOpSkip()
//     O97CRCBuf()
//     O97ModuleCRCFast()
//     O97ModAccInit()
//     O97ModAccGetLineInfo()
//     O97ModAccGetDataPtr()
// - Modified O97GetIdentifierTableOffset() to correctly endianize.
// - Added opcode operand table gabyO97OpSkip[].
// 
//    Rev 1.3   16 Oct 1997 14:53:14   DCHI
// Added more efficient O97GetLineBinaryOffsets() in sync with MS docs.
// 
//    Rev 1.2   28 Aug 1997 15:08:14   DCHI
// Added PowerPoint identifier table search support.
// 
//    Rev 1.1   11 Aug 1997 15:06:18   DCHI
// Removed unreferenced variables.
// 
//    Rev 1.0   11 Aug 1997 14:33:04   DCHI
// Initial revision.
// 
//************************************************************************

#include "o97api.h"
#include "crc32.h"

//********************************************************************
//
// Function:
//  BOOL O97Get_VBA_PROJECTEndian()
//
// Parameters:
//  lpstStream      Ptr to open _VBA_PROJECT stream
//  lpnVersion      Ptr to int for version
//  lpbLitEnd       Ptr to BOOL for endian state
//
// Description:
//  The function reads the first six bytes of the module stream
//  to determine the version and endian state.
//
//  If the byte at offset 2 is 0x6A or less, it is assumed that it
//  is either Office 97 or Office 98 for the Mac.  Otherwise,
//  it is assumed that it is Office 98 for the PC.
//
//  If the byte at offset 4 is 0x01, it is assumed that the stream
//  is in little endian format, otherwise it is assumed to be
//  in big endian format.
//
//  Either of lpnVersion or lpbLitEnd may be NULL if the value
//  is not desired.
//
// Returns:
//  TRUE        On success
//  FALSE       On failure
//
//********************************************************************

BOOL O97Get_VBA_PROJECTEndian
(
    LPSS_STREAM         lpstStream,
    LPINT               lpnVersion,
    LPBOOL              lpbLitEnd
)
{
    BYTE                abyBuf[6];
    DWORD               dwBytesRead;

    // Read the first six bytes of the stream

    if (SSSeekRead(lpstStream,
                   0,
                   abyBuf,
                   6,
                   &dwBytesRead) != SS_STATUS_OK ||
        dwBytesRead != 6)
        return(FALSE);

    // Get the version

    if (lpnVersion != NULL)
    {
        if (abyBuf[2] <= 0x6A)
            *lpnVersion = O97_VERSION_O97;
        else
            *lpnVersion = O97_VERSION_O98;
    }

    if (lpbLitEnd != NULL)
    {
        if (abyBuf[5] == 0x01)
            *lpbLitEnd = TRUE;
        else
            *lpbLitEnd = FALSE;
    }

    return(TRUE);
}


//********************************************************************
//
// Function:
//  BOOL O97GetModuleVersionEndian()
//
// Parameters:
//  lpstStream      Ptr to open module stream
//  lpnVersion      Ptr to int for version
//  lpbLitEnd       Ptr to BOOL for endian state
//
// Description:
//  The function reads the first six bytes of the module stream
//  to determine the version and endian state.
//
//  If the byte at offset 5 is 0xB6, it is assumed that it
//  is either Office 97 or Office 98 for the Mac.  Otherwise,
//  it is assumed that it is Office 98 for the PC.
//
//  If the byte at offset 2 is 0x01, it is assumed that the stream
//  is in little endian format, otherwise it is assumed to be
//  in big endian format.
//
//  Either of lpnVersion or lpbLitEnd may be NULL if the value
//  is not desired.
//
// Returns:
//  TRUE        On success
//  FALSE       On failure
//
//********************************************************************

BOOL O97GetModuleVersionEndian
(
    LPSS_STREAM         lpstStream,
    LPINT               lpnVersion,
    LPBOOL              lpbLitEnd
)
{
    BYTE                abyBuf[6];
    DWORD               dwBytesRead;

    // Read the first six bytes of the module stream

    if (SSSeekRead(lpstStream,
                   0,
                   abyBuf,
                   6,
                   &dwBytesRead) != SS_STATUS_OK ||
        dwBytesRead != 6)
        return(FALSE);

    // Get the version

    if (lpnVersion != NULL)
    {
        if (abyBuf[5] == 0xB6)
            *lpnVersion = O97_VERSION_O97;
        else
            *lpnVersion = O97_VERSION_O98;
    }

    if (lpbLitEnd != NULL)
    {
        if (abyBuf[2] == 0x01)
            *lpbLitEnd = TRUE;
        else
            *lpbLitEnd = FALSE;
    }

    return(TRUE);
}


//********************************************************************
//
// BOOL O97GetIdentifierTableOffset()
//
// Parameters:
//  lpstStream      Ptr to open _VBA_PROJECT stream
//  lpdwOffset      Ptr to DWORD variable for table offset
//  lpwNumIdents    Ptr to WORD for number of identifiers
//
// Description:
//  The function performs up to two passes to find the beginning
//  of the identifier table.  During the first pass, the function
//  scans starting from offset 0x800 up until the end of the stream.
//  The second passes occurs only if the first pass fails to find
//  the table.  It scans starting from the beginning of the stream
//  and ends at offset 0x700.
//
//  Each pass searches for the byte sequence 0x80 0x00 0x00 0x00.
//  For each such found sequence, a search is done for either the byte
//  sequence 0x04 0x?? 'W' 'o' 'r' 'd' or
//           0x05 0x?? 'E' 'x' 'c' 'e' 'l' or
//           0x0A 0x?? 'P' 'o' 'w' 'e' 'r' 'P' 'o' 'i' 'n' 't'.
//  The function assumes that finding either of these second sequences
//  signals the beginning of the identifier table as starting at the
//  beginning of the found second sequence.
//
//  Let the 0x80 of the sequence 0x80 0x00 0x00 0x00 be at offset
//  zero for calculating the number of identifiers.  The number of
//  identifiers is the WORD at offset 6 minus the WORD at offset
//  10.
//
// Returns:
//  TRUE        On success
//  FALSE       On failure
//
//********************************************************************

BOOL O97GetIdentifierTableOffset
(
    LPSS_STREAM         lpstStream,
    LPDWORD             lpdwOffset,
    LPDWORD             lpdwNumIdents
)
{
    int             i, j, k, l, m;
    DWORD           dwOffset;
    DWORD           dwLimit, dwMaxLimit;
    DWORD           dwBytesToRead;
    DWORD           dwBytesRead;
    BYTE            abyWorkBuffer[512];
    int             nVersion;
    BOOL            bLittleEndian;

    if (O97Get_VBA_PROJECTEndian(lpstStream,
                                 &nVersion,
                                 &bLittleEndian) == FALSE)
        return(FALSE);

    dwOffset = 0x700;
    dwLimit = dwMaxLimit = SSStreamLen(lpstStream) - 10;
    for (i=0;i<2;i++)
    {
        while (dwOffset < dwLimit)
        {
            if (dwLimit - dwOffset < sizeof(abyWorkBuffer))
                dwBytesToRead = dwLimit - dwOffset;
            else
                dwBytesToRead = sizeof(abyWorkBuffer);

            if (dwBytesToRead < 12)
                break;

            if (SSSeekRead(lpstStream,
                           dwOffset,
                           abyWorkBuffer,
                           dwBytesToRead,
                           &dwBytesRead) != SS_STATUS_OK ||
                dwBytesRead != dwBytesToRead)
                return(FALSE);

            dwBytesToRead -= 11;
            for (j=0;j<dwBytesToRead;j++)
            {
                if (((abyWorkBuffer[j] == 0x80 &&
                      abyWorkBuffer[j+1] == 0x00 &&
                      bLittleEndian != FALSE) ||
                     (abyWorkBuffer[j] == 0x00 &&
                      abyWorkBuffer[j+1] == 0x80 &&
                      bLittleEndian == FALSE)) &&
                    abyWorkBuffer[j+2] == 0x00 &&
                    abyWorkBuffer[j+3] == 0x00)
                {
                    static BYTE abySeq0[] =
                        { 'W', 'o', 'r', 'd' };
                    static BYTE abySeq1[] =
                        { 'E', 'x', 'c', 'e', 'l' };
                    static BYTE abySeq2[] =
                        { 'P', 'o', 'w', 'e', 'r', 'P', 'o', 'i', 'n', 't' };

                    int         nLimit;
                    DWORD       dwCandOffset;
                    int         nCandSize;
                    LPBYTE      lpabyCand;
                    BOOL        bFullFlags;


                    // Potentially found it, note the number of IDs
                    //  WORD at offset 6 minus WORD at offset 10

                    *lpdwNumIdents = SSEndianWORD(abyWorkBuffer+j+6,
                                                  bLittleEndian);

                    if (*lpdwNumIdents >= SSEndianWORD(abyWorkBuffer+j+10,
                                                       bLittleEndian))
                    {
                        *lpdwNumIdents -= SSEndianWORD(abyWorkBuffer+j+10,
                                                       bLittleEndian);
                    }

                    dwCandOffset = dwOffset + j + 15;
                    while (dwCandOffset < dwMaxLimit)
                    {
                        if (dwMaxLimit - dwCandOffset < sizeof(abyWorkBuffer))
                            dwBytesToRead = dwMaxLimit - dwCandOffset;
                        else
                            dwBytesToRead = sizeof(abyWorkBuffer);

                        if (dwBytesToRead < 32)
                            break;

                        if (SSSeekRead(lpstStream,
                                       dwCandOffset,
                                       abyWorkBuffer,
                                       dwBytesToRead,
                                       &dwBytesRead) != SS_STATUS_OK ||
                            dwBytesRead != dwBytesToRead)
                            return(FALSE);

                        // Make sure we also have the type byte

                        nLimit = (int)dwBytesToRead - 2;
                        nCandSize = 0;
                        bFullFlags = FALSE;
                        for (k=0;k<nLimit;k++)
                        {
                            if (bLittleEndian)
                            {
                                if (abyWorkBuffer[k] == sizeof(abySeq0) ||
                                    abyWorkBuffer[k] == sizeof(abySeq1) ||
                                    abyWorkBuffer[k] == sizeof(abySeq2))
                                {
                                    nCandSize = abyWorkBuffer[k];
                                    if (abyWorkBuffer[k+1] & 0x80)
                                        bFullFlags = TRUE;
                                }
                            }
                            else
                            {
                                if (abyWorkBuffer[k+1] == sizeof(abySeq0) ||
                                    abyWorkBuffer[k+1] == sizeof(abySeq1) ||
                                    abyWorkBuffer[k+1] == sizeof(abySeq2))
                                {
                                    nCandSize = abyWorkBuffer[k+1];
                                    if (abyWorkBuffer[k] & 0x80)
                                        bFullFlags = TRUE;
                                }
                            }

                            if (nCandSize != 0)
                            {
                                if (bFullFlags)
                                {
                                    if (k + 6 + nCandSize > nLimit)
                                        break;

                                    m = 8;
                                }
                                else
                                {
                                    if (k + nCandSize > nLimit)
                                        break;

                                    m = 2;
                                }

                                if (nCandSize == sizeof(abySeq0))
                                    lpabyCand = abySeq0;
                                else
                                if (nCandSize == sizeof(abySeq1))
                                    lpabyCand = abySeq1;
                                else
                                if (nCandSize == sizeof(abySeq2))
                                    lpabyCand = abySeq2;

                                for (l=0;l<nCandSize;l++)
                                    if (abyWorkBuffer[k+l+m] != lpabyCand[l])
                                        break;

                                if (l == nCandSize)
                                {
                                    // Found it

                                    *lpdwOffset = dwCandOffset + k;
                                    return(TRUE);
                                }

                                // Reset flags

                                nCandSize = 0;
                                bFullFlags = FALSE;
                            }
                        }

                        dwCandOffset += k;
                    }

                    dwBytesToRead = j + 1;
                }
            }

            dwOffset += dwBytesToRead;
        }

        dwOffset = 0;
        if (dwLimit >= 0x700)
            dwLimit = 0x700;

        if (dwMaxLimit >= 0x700 + 0x400)
            dwMaxLimit = 0x700 + 0x400;
    }

    return(FALSE);
}


//********************************************************************
//
// BOOL O97GetTypeTableOffset()
//
// Parameters:
//  lpstStream      Ptr to open module stream
//  lpdwOffset      Ptr to DWORD variable for table offset
//  lpdwSize        Ptr to DWORD variable for size of table
//
// Description:
//  The function searches for the string
//      0xFF 0xFF 0xFF 0x78 0x00 0x00 0x00
//  starting from offset 0x70 and only searching forward up
//  to 0x200 bytes.  If found, the function assumes that at that
//  location plus 15 is a DWORD offset that plus 10 is the offset
//  of the DWORD that specifies the size of the type table and the
//  type table immediately follows the size DWORD.
//
// Returns:
//  TRUE        On success
//  FALSE       On failure
//
//********************************************************************

BOOL O97GetTypeTableOffset
(
    LPSS_STREAM         lpstStream,
    LPDWORD             lpdwOffset,
    LPDWORD             lpdwSize
)
{
    DWORD               dwOffset;
    DWORD               dwBytesRead;
    BYTE                abyWorkBuffer[512];
    int                 i;
    int                 nVersion;
    BOOL                bLittleEndian;

    // Get the version and endian state

    if (O97GetModuleVersionEndian(lpstStream,
                                  &nVersion,
                                  &bLittleEndian) == FALSE)
        return(FALSE);

    if (nVersion == O97_VERSION_O97)
    {
        // Assume Office 97/Mac O98

        // Read 512 bytes from offset 0x70

        if (SSSeekRead(lpstStream,
                       0x70,
                       abyWorkBuffer,
                       512,
                       &dwBytesRead) != SS_STATUS_OK ||
            dwBytesRead != 512)
            return(FALSE);

        // Search for 0xFF 0xFF 0xFF 0x78 0x00 0x00 0x00

        for (i=0;i<(512-19+1);i++)
        {
            if (abyWorkBuffer[i] == 0xFF &&
                abyWorkBuffer[i+1] == 0xFF &&
                abyWorkBuffer[i+2] == 0xFF &&
                ((abyWorkBuffer[i+3] == 0x78 &&
                  abyWorkBuffer[i+4] == 0x00) ||
                 (abyWorkBuffer[i+3] == 0x00 &&
                  abyWorkBuffer[i+4] == 0x78)) &&
                abyWorkBuffer[i+5] == 0x00 &&
                abyWorkBuffer[i+6] == 0x00)
            {
                dwOffset = SSEndianDWORD(abyWorkBuffer+i+15,
                                         bLittleEndian);
                break;
            }
        }
    }
    else
    {
        // Assume Office 9

        // Get the offset of the type table

        if (SSSeekRead(lpstStream,
                       0x11,
                       (LPBYTE)&dwOffset,
                       sizeof(DWORD),
                       &dwBytesRead) != SS_STATUS_OK ||
            dwBytesRead != sizeof(DWORD))
            return(FALSE);

        dwOffset = DWENDIAN(dwOffset);
    }

    // The size is located at the DWORD + 10

    dwOffset += 10;

    if (SSSeekRead(lpstStream,
                   dwOffset,
                   lpdwSize,
                   sizeof(DWORD),
                   &dwBytesRead) != SS_STATUS_OK ||
        dwBytesRead != sizeof(DWORD))
        return(FALSE);

    *lpdwSize = SSEndianDWORD((LPBYTE)lpdwSize,bLittleEndian);
    *lpdwOffset = dwOffset + 4;
    return(TRUE);
}


//********************************************************************
//
// BOOL O97GetLineBinaryOffsets()
//
// Parameters:
//  lpstStream          Ptr to open module stream
//  lpdwLineTableOffset Ptr to DWORD variable for line table offset
//  lpdwNumLines        Ptr to DWORD for number of lines
//  lpdwBinaryOffset    Ptr to DWORD variable for binary offset
//  lpbLittleEndian     Ptr to boolean for endian state
//
// Description:
//  The function assumes the file is in the following format:
//      abyUnknown0[11]
//      dwExtra0
//      abyExtra0[dwExtra0]
//      abyUnknown1[64]
//      wExtra1
//      abyExtra1[wExtra1 * 16]
//      dwExtra2
//      abyExtra2[dwExtra2]
//      abyUnknown2[6]
//      dwExtra3
//      abyExtra3[dwExtra3]
//      abyUnknown3[77]
//      dwTypeSrcOffset
//
//  At dwTypeSrcOffset + 60:
//      FE CA ?? ??
//      wLineRecordCount
//      abyLineRecords[wLineRecordCount][12]
//
//  The binary offset is 10 bytes after the last line entry.
//
// Returns:
//  TRUE        On success
//  FALSE       On failure
//
//********************************************************************

BOOL O97GetLineBinaryOffsets
(
    LPSS_STREAM         lpstStream,
    LPDWORD             lpdwLineTableOffset,
    LPDWORD             lpdwNumLines,
    LPDWORD             lpdwBinaryOffset
)
{
    DWORD               dwOffset;
    DWORD               dwTemp;
    WORD                wTemp;
    BYTE                abyBuf[6];
    DWORD               dwBytesRead;
    WORD                wNumLines;
    int                 nVersion;
    BOOL                bLittleEndian;

    // Get the version and endian state

    if (O97GetModuleVersionEndian(lpstStream,
                                  &nVersion,
                                  &bLittleEndian) == FALSE)
        return(FALSE);

    if (nVersion == O97_VERSION_O97)
    {
        // Assume Office 97/Mac O98

        dwOffset = 11;

        // dwExtra0

        if (SSSeekRead(lpstStream,
                       dwOffset,
                       &dwTemp,
                       sizeof(DWORD),
                       &dwBytesRead) != SS_STATUS_OK ||
            dwBytesRead != sizeof(DWORD))
            return(FALSE);

        dwTemp = SSEndianDWORD((LPBYTE)&dwTemp,bLittleEndian);

        // Skip over dwExtra0, abyExtra0[dwExtra0], and abyUnknown1[64]

        dwOffset += sizeof(DWORD) + dwTemp + 64;

        // wExtra1

        if (SSSeekRead(lpstStream,
                       dwOffset,
                       &wTemp,
                       sizeof(WORD),
                       &dwBytesRead) != SS_STATUS_OK ||
            dwBytesRead != sizeof(WORD))
            return(FALSE);

        wTemp = SSEndianWORD((LPBYTE)&wTemp,bLittleEndian);

        // Skip over wExtra1 and abyExtra1[wExtra1 * 16]

        dwOffset += sizeof(WORD) + wTemp * (DWORD)16;

        // dwExtra2

        if (SSSeekRead(lpstStream,
                       dwOffset,
                       &dwTemp,
                       sizeof(DWORD),
                       &dwBytesRead) != SS_STATUS_OK ||
            dwBytesRead != sizeof(DWORD))
            return(FALSE);

        dwTemp = SSEndianDWORD((LPBYTE)&dwTemp,bLittleEndian);

        // Skip over dwExtra2, abyExtra2[dwExtra2], and abyUnknown2[6]

        dwOffset += sizeof(DWORD) + dwTemp + 6;

        // dwExtra3

        if (SSSeekRead(lpstStream,
                       dwOffset,
                       &dwTemp,
                       sizeof(DWORD),
                       &dwBytesRead) != SS_STATUS_OK ||
            dwBytesRead != sizeof(DWORD))
            return(FALSE);

        dwTemp = SSEndianDWORD((LPBYTE)&dwTemp,bLittleEndian);

        // Skip over dwExtra3, abyExtra3[dwExtra3], and abyUnknown3[77]

        dwOffset += sizeof(DWORD) + dwTemp + 77;

        // Get dwTypeSrcOffset
    }
    else
    {
        // Assume Office 9

        // Get the offset of the line table

        dwOffset = 0x19;
    }

    if (SSSeekRead(lpstStream,
                   dwOffset,
                   &dwTemp,
                   sizeof(DWORD),
                   &dwBytesRead) != SS_STATUS_OK ||
        dwBytesRead != sizeof(DWORD))
        return(FALSE);

    dwTemp = SSEndianDWORD((LPBYTE)&dwTemp,bLittleEndian);

    // Read FE CA

    dwOffset = dwTemp + 60;

    if (SSSeekRead(lpstStream,
                   dwOffset,
                   abyBuf,
                   2 * sizeof(BYTE),
                   &dwBytesRead) != SS_STATUS_OK ||
        dwBytesRead != 2 * sizeof(BYTE))
        return(FALSE);

    if (!((abyBuf[0] == 0xFE && abyBuf[1] == 0xCA) ||
        (abyBuf[0] == 0xCA && abyBuf[1] == 0xFE)))
        return(FALSE);

    // Skip over FE CA ?? ??

    dwOffset += 4;

    // Get the number of lines

    if (SSSeekRead(lpstStream,
                   dwOffset,
                   &wNumLines,
                   sizeof(WORD),
                   &dwBytesRead) != SS_STATUS_OK ||
        dwBytesRead != sizeof(WORD))
        return(FALSE);

    // Skip over wLineRecordCount

    dwOffset += sizeof(WORD);

    *lpdwLineTableOffset = dwOffset;
    wNumLines = SSEndianWORD((LPBYTE)&wNumLines,bLittleEndian);
    *lpdwNumLines = wNumLines;
    *lpdwBinaryOffset = dwOffset + 10 +
        wNumLines * (DWORD)sizeof(O97_LINE_TABLE_LINE_T);

    return(TRUE);
}


//********************************************************************
//
// BOOL O97CreateModuleAccessStruct()
//
// Parameters:
//  lpstStream      Ptr to open module stream
//  lplpstModule    Ptr to var to store ptr to created module
//                  access structure
//
// Description:
//  Allocates memory for a module access structure.
//
// Returns:
//  TRUE        On success
//  FALSE       On failure
//
//********************************************************************

BOOL O97CreateModuleAccessStruct
(
    LPVOID              lpvRootCookie,
    LPLPO97_MODULE      lplpstModule
)
{
    LPO97_MODULE        lpstModule;

    if (SSMemoryAlloc(lpvRootCookie,
                      sizeof(O97_MODULE_T),
                      (LPLPVOID)&lpstModule) != SS_STATUS_OK)
        return(FALSE);

    lpstModule->stLineTable.dwNumBlocks = O97_LINE_TABLE_NUM_BLOCKS;
    lpstModule->stLineTable.dwBlockSize = O97_LINE_TABLE_BLOCK_SIZE;

    lpstModule->stBinary.dwNumBlocks = O97_MOD_BIN_NUM_BLOCKS;
    lpstModule->stBinary.dwBlockSize = O97_MOD_BIN_BLOCK_SIZE;

    *lplpstModule = lpstModule;

    return(TRUE);
}


//********************************************************************
//
// BOOL O97DestroyModuleAccessStruct()
//
// Parameters:
//  lpstStream      Ptr to open module stream
//  lpstModule      Ptr to created module access structure
//
// Description:
//  Deallocates memory allocated for a module access structure.
//
// Returns:
//  TRUE        On success
//  FALSE       On failure
//
//********************************************************************

BOOL O97DestroyModuleAccessStruct
(
    LPVOID              lpvRootCookie,
    LPO97_MODULE        lpstModule
)
{
    if (SSMemoryFree(lpvRootCookie,
                     lpstModule) != SS_STATUS_OK)
        return(FALSE);

    return(TRUE);
}


//********************************************************************
//
// BOOL O97InitModuleAccess()
//
// Parameters:
//  lpstStream      Ptr to open module stream
//  lpstModule      Ptr to created module access structure
//
// Description:
//  Initializes the fields of the structure for module access.
//
// Returns:
//  TRUE        On success
//  FALSE       On failure
//
//********************************************************************

BOOL O97InitModuleAccess
(
    LPSS_STREAM         lpstStream,
    LPO97_MODULE        lpstModule
)
{
    int                 i;

    lpstModule->lpstStream = lpstStream;

    // Get the version and endian state

    if (O97GetModuleVersionEndian(lpstStream,
                                  &lpstModule->nVersion,
                                  &lpstModule->bLittleEndian) == FALSE)
        return(FALSE);

    // Initialize type table info

    if (O97GetTypeTableOffset(lpstStream,
                              &lpstModule->stTypeTable.dwOffset,
                              &lpstModule->stTypeTable.dwSize) == FALSE)
        return(FALSE);

    // Initialize line table cache

    for (i=0;i<lpstModule->stLineTable.dwNumBlocks;i++)
        lpstModule->stLineTable.adwBlockNum[i] = 0xFFFFFFFF;

    // Initialize binary cache

    for (i=0;i<lpstModule->stBinary.dwNumBlocks;i++)
        lpstModule->stBinary.adwBlockNum[i] = 0xFFFFFFFF;

    if (O97GetLineBinaryOffsets(lpstStream,
                                &lpstModule->stLineTable.dwOffset,
                                &lpstModule->stLineTable.dwNumLines,
                                &lpstModule->stBinary.dwOffset) == FALSE)
        return(FALSE);

    return(TRUE);
}


//********************************************************************
//
// BOOL O97GetLineEntry()
//
// Parameters:
//  lpstModule      Ptr to initialized module access structure
//  dwLine          Line entry to get
//  lpstEntry       Ptr to line entry structure to fill
//
// Description:
//  Fills a line entry structure with the information from the
//  given line.
//
// Returns:
//  TRUE        On success
//  FALSE       On failure
//
//********************************************************************

BOOL O97GetLineEntry
(
    LPO97_MODULE        lpstModule,
    DWORD               dwLine,
    LPO97_LINE_ENTRY    lpstEntry
)
{
    DWORD               dwBlockNum;
    DWORD               dwCacheLineNum;

    if (dwLine >= lpstModule->stLineTable.dwNumLines)
        return(FALSE);

    dwBlockNum = dwLine & O97_LINE_TABLE_BLOCK_NUM_MASK;
    dwCacheLineNum = (dwLine & O97_LINE_TABLE_CLN_MASK) >>
        O97_LINE_TABLE_CLN_SHIFT;

    // First check to see if the block containing the wanted line
    //  is in the cache.  If not, then the block must be loaded.

    if (lpstModule->stLineTable.adwBlockNum[dwCacheLineNum] !=
        dwBlockNum)
    {
        O97_LINE_TABLE_LINE_T   stLine;
        DWORD                   dwCurLine;
        DWORD                   dwLimitLine;
        DWORD                   dwOffset;
        DWORD                   dwBytesRead;
        LPO97_LINE_ENTRY        lpstCurEntry;

        // Determine the number of lines to read

        dwLimitLine = dwBlockNum + O97_LINE_TABLE_BLOCK_SIZE;
        if (dwLimitLine > lpstModule->stLineTable.dwNumLines)
            dwLimitLine = lpstModule->stLineTable.dwNumLines;

        // Load the block

        dwCurLine = dwBlockNum;
        dwOffset = lpstModule->stLineTable.dwOffset +
            dwCurLine * sizeof(O97_LINE_TABLE_LINE_T);
        lpstCurEntry = lpstModule->stLineTable.abyBlocks[dwCacheLineNum];

        while (dwCurLine < dwLimitLine)
        {
            // Read the line entry

            if (SSSeekRead(lpstModule->lpstStream,
                           dwOffset,
                           &stLine,
                           sizeof(O97_LINE_TABLE_LINE_T),
                           &dwBytesRead) != SS_STATUS_OK ||
                dwBytesRead != sizeof(O97_LINE_TABLE_LINE_T))
                return(FALSE);

            // Set the flags

            lpstCurEntry->wType = 0;
            if (lpstModule->bLittleEndian == FALSE)
            {
                BYTE    byTemp;

                // Swap the first and last bytes

                byTemp = stLine.abyFlags[0];
                stLine.abyFlags[0] = stLine.abyFlags[3];
                stLine.abyFlags[3] = byTemp;

                // Swap the second and third bytes

                byTemp = stLine.abyFlags[1];
                stLine.abyFlags[1] = stLine.abyFlags[2];
                stLine.abyFlags[2] = byTemp;
            }

            if ((stLine.abyFlags[0] & 0x42) == 0x42)
                lpstCurEntry->wType |= O97_LINE_FLAG_FUNCTION;

            if ((stLine.abyFlags[0] & 0x22) == 0x22)
                lpstCurEntry->wType |= O97_LINE_FLAG_SUB;

            if ((stLine.abyFlags[0] & 0x04) == 0x04)
                lpstCurEntry->wType |= O97_LINE_FLAG_END_FUNC_SUB;

            if ((stLine.abyFlags[1] & 0x40) == 0x40)
                lpstCurEntry->wType |= O97_LINE_FLAG_CONST;

            if ((stLine.abyFlags[1] & 0x81) == 0x81)
                lpstCurEntry->wType |= O97_LINE_FLAG_STATEMENT;
            else
            if ((stLine.abyFlags[1] & 0x81) == 0x80)
            {
                if ((stLine.abyFlags[2] & 0x09) == 0x09)
                    lpstCurEntry->wType |= O97_LINE_FLAG_COMMENT;
                else
                if ((stLine.abyFlags[2] & 0x08) == 0x08)
                    lpstCurEntry->wType |= O97_LINE_FLAG_DECLARE;
            }

            // Set the size and offset

            lpstCurEntry->wSize = SSEndianWORD((LPBYTE)&stLine.wSize,
                                               lpstModule->bLittleEndian);
            lpstCurEntry->dwOffset = SSEndianDWORD((LPBYTE)&stLine.dwOffset,
                                                   lpstModule->bLittleEndian);

            ++dwCurLine;
            dwOffset += sizeof(O97_LINE_TABLE_LINE_T);
            ++lpstCurEntry;
        }

        lpstModule->stLineTable.adwBlockNum[dwCacheLineNum] =
            dwBlockNum;
    }

    // Return the desired entry

    *lpstEntry = lpstModule->stLineTable.
        abyBlocks[dwCacheLineNum][dwLine & O97_LINE_TABLE_LIB_MASK];

    return(TRUE);
}


//********************************************************************
//
// BOOL O97GetModuleBinary()
//
// Parameters:
//  lpstModule      Ptr to initialized module access structure
//  dwOffset        Offset in binary stream of bytes to get
//  lpbyBuf         Destination for bytes to get
//  dwNumBytes      Number of bytes to get
//
// Description:
//  Fills a buffer with the binary bytes of the module from the
//  given offset.
//
// Returns:
//  TRUE        On success
//  FALSE       On failure
//
//********************************************************************

BOOL O97GetModuleBinary
(
    LPO97_MODULE        lpstModule,
    DWORD               dwOffset,
    LPBYTE              lpbyBuf,
    DWORD               dwNumBytes
)
{
    DWORD               dwBlockNum;
    DWORD               dwCacheLineNum;
    DWORD               dwBlockOffset;
    DWORD               dwBytesToRead;
    DWORD               dwBytesRead;
    LPBYTE              lpbySrc;

    while (dwNumBytes != 0)
    {
        dwBlockNum = dwOffset & O97_MOD_BIN_BLOCK_NUM_MASK;
        dwCacheLineNum = (dwOffset & O97_MOD_BIN_CLN_MASK) >>
            O97_MOD_BIN_CLN_SHIFT;

        // Is the desired block in the cache?

        if (lpstModule->stBinary.adwBlockNum[dwCacheLineNum] !=
            dwBlockNum)
        {
            // Calculate the number of bytes to read and make
            //  sure the block read stays within the stream

            dwBlockOffset = lpstModule->stBinary.dwOffset +
                dwBlockNum;

            dwBytesToRead = lpstModule->stBinary.dwBlockSize;

            if (dwBlockOffset + dwBytesToRead >
                SSStreamLen(lpstModule->lpstStream))
            {
                if (dwBlockOffset >= SSStreamLen(lpstModule->lpstStream))
                    return(FALSE);

                dwBytesToRead = SSStreamLen(lpstModule->lpstStream) -
                    dwBlockOffset;
            }

            // Read the block

            if (SSSeekRead(lpstModule->lpstStream,
                           dwBlockOffset,
                           lpstModule->stBinary.abyBlocks[dwCacheLineNum],
                           dwBytesToRead,
                           &dwBytesRead) != SS_STATUS_OK ||
                dwBytesRead != dwBytesToRead)
                return(FALSE);

            lpstModule->stBinary.adwBlockNum[dwCacheLineNum] =
                dwBlockNum;
        }

        // Calculate source and number of bytes in block to read

        lpbySrc = lpstModule->stBinary.abyBlocks[dwCacheLineNum] +
            (dwOffset & O97_MOD_BIN_BIB_MASK);

        dwBytesToRead = lpstModule->stBinary.dwBlockSize -
            (dwOffset & O97_MOD_BIN_BIB_MASK);

        if (dwBytesToRead > dwNumBytes)
            dwBytesToRead = dwNumBytes;

        // Copy bytes from block

        for (dwBytesRead=0;dwBytesRead<dwBytesToRead;dwBytesRead++)
            *lpbyBuf++ = *lpbySrc++;

        // Update count of number of bytes read and offset

        dwNumBytes -= dwBytesToRead;
        dwOffset += dwBytesToRead;
    }

    return(TRUE);
}


//********************************************************************
//
// BOOL O97GetModuleWORD()
//
// Parameters:
//  lpstModule      Ptr to initialized module access structure
//  dwOffset        Offset in binary stream of WORD to get
//  lpwValue        Destination of WORD
//
// Description:
//  Retrieves a WORD value from the module binary stream.  The value
//  is endianized appropriately.
//
// Returns:
//  TRUE        On success
//  FALSE       On failure
//
//********************************************************************

BOOL O97GetModuleWORD
(
    LPO97_MODULE        lpstModule,
    DWORD               dwOffset,
    LPWORD              lpwValue
)
{
    if (O97GetModuleBinary(lpstModule,
                           dwOffset,
                           (LPBYTE)lpwValue,
                           sizeof(WORD)) == FALSE)
        return(FALSE);

    // Endianize it

    *lpwValue = SSEndianWORD((LPBYTE)lpwValue,
                             lpstModule->bLittleEndian);
    return(TRUE);
}


//********************************************************************
//
// BOOL O97GetModuleDWORD()
//
// Parameters:
//  lpstModule      Ptr to initialized module access structure
//  dwOffset        Offset in binary stream of DWORD to get
//  lpdwValue       Destination of DWORD
//
// Description:
//  Retrieves a DWORD value from the module binary stream.  The value
//  is endianized appropriately.
//
// Returns:
//  TRUE        On success
//  FALSE       On failure
//
//********************************************************************

BOOL O97GetModuleDWORD
(
    LPO97_MODULE        lpstModule,
    DWORD               dwOffset,
    LPDWORD             lpdwValue
)
{
    if (O97GetModuleBinary(lpstModule,
                           dwOffset,
                           (LPBYTE)lpdwValue,
                           sizeof(DWORD)) == FALSE)
        return(FALSE);

    // Endianize it

    *lpdwValue = SSEndianDWORD((LPBYTE)lpdwValue,
                               lpstModule->bLittleEndian);
    return(TRUE);
}


//********************************************************************
//
// BOOL O97TypeTableGetRecordIdent()
//
// Parameters:
//  lpstModule      Ptr to initialized module access structure
//  dwOffset        Offset in type table of record
//  lpwIdent        Ptr to WORD for identifier WORD
//
// Description:
//  Retrieves the ID of the identifier at the given record.  It is
//  assumed to be at offset two from the beginning of the record.
//
// Returns:
//  TRUE        On success
//  FALSE       On failure
//
//********************************************************************

BOOL O97TypeTableGetRecordIdent
(
    LPO97_MODULE        lpstModule,
    DWORD               dwOffset,
    LPWORD              lpwIdent
)
{
    DWORD               dwBytesRead;

    if (SSSeekRead(lpstModule->lpstStream,
                   lpstModule->stTypeTable.dwOffset + dwOffset + 2,
                   lpwIdent,
                   sizeof(WORD),
                   &dwBytesRead) != SS_STATUS_OK)
        return(FALSE);

    *lpwIdent = SSEndianWORD((LPBYTE)lpwIdent,
                             lpstModule->bLittleEndian);

    return(TRUE);
}


//********************************************************************
//
// BOOL O97TypeTableGetRoutineInfo()
//
// Parameters:
//  lpstModule           Ptr to initialized module access structure
//  dwOffset             Offset in type table of record
//  lpabyFlagBytes       First two bytes of record
//  lpwIdent             Ptr to WORD for identifier WORD
//  lpbyNumParams        Ptr to BYTE for number of parameters
//  lpdwFirstParamOffset Ptr to DWORD for next param record
//
// Description:
//  Retrieves the ID of the identifier and the first parameter offset
//  for the record at the given offset.  It is assumed that the ID
//  is at offset two from the beginning of the record and the first
//  parameter offset is at offset 0x24 from the beginning of the
//  record.
//
// Returns:
//  TRUE        On success
//  FALSE       On failure
//
//********************************************************************

BOOL O97TypeTableGetRoutineInfo
(
    LPO97_MODULE        lpstModule,
    DWORD               dwOffset,
    LPBYTE              lpabyFlagBytes,
    LPWORD              lpwIdent,
    LPBYTE              lpbyNumParams,
    LPDWORD             lpdwFirstParamOffset
)
{
    DWORD               dwBytesRead;

    if (lpabyFlagBytes != NULL)
    {
        if (SSSeekRead(lpstModule->lpstStream,
                       lpstModule->stTypeTable.dwOffset + dwOffset,
                       lpabyFlagBytes,
                       2 * sizeof(BYTE),
                       &dwBytesRead) != SS_STATUS_OK)
            return(FALSE);
    }

    if (lpwIdent != NULL)
    {
        if (SSSeekRead(lpstModule->lpstStream,
                       lpstModule->stTypeTable.dwOffset + dwOffset + 2,
                       lpwIdent,
                       sizeof(WORD),
                       &dwBytesRead) != SS_STATUS_OK)
            return(FALSE);

        *lpwIdent = SSEndianWORD((LPBYTE)lpwIdent,
                                 lpstModule->bLittleEndian);
    }

    if (lpbyNumParams != NULL)
    {
        if (SSSeekRead(lpstModule->lpstStream,
                       lpstModule->stTypeTable.dwOffset + dwOffset + 0x37,
                       lpbyNumParams,
                       sizeof(BYTE),
                       &dwBytesRead) != SS_STATUS_OK)
            return(FALSE);
    }

    if (lpdwFirstParamOffset != NULL)
    {
        if (SSSeekRead(lpstModule->lpstStream,
                       lpstModule->stTypeTable.dwOffset + dwOffset + 0x24,
                       lpdwFirstParamOffset,
                       sizeof(DWORD),
                       &dwBytesRead) != SS_STATUS_OK)
            return(FALSE);

        *lpdwFirstParamOffset = SSEndianDWORD((LPBYTE)lpdwFirstParamOffset,
                                              lpstModule->bLittleEndian);
    }

    return(TRUE);
}


//********************************************************************
//
// BOOL O97TypeTableGetParamInfo()
//
// Parameters:
//  lpstModule          Ptr to initialized module access structure
//  dwOffset            Offset in type table of record
//  lpwIdent            Ptr to WORD for identifier WORD
//  lpdwNextParamOffset Ptr to DWORD for next param record
//
// Description:
//  Retrieves the ID of the identifier and the next parameter offset
//  for the record at the given offset.  It is assumed that the ID
//  is at offset two from the beginning of the record and the next
//  parameter offset is at offset 0x14 from the beginning of the
//  record.
//
// Returns:
//  TRUE        On success
//  FALSE       On failure
//
//********************************************************************

BOOL O97TypeTableGetParamInfo
(
    LPO97_MODULE        lpstModule,
    DWORD               dwOffset,
    LPWORD              lpwIdent,
    LPDWORD             lpdwNextParamOffset
)
{
    DWORD               dwBytesRead;

    if (lpwIdent != NULL)
    {
        if (SSSeekRead(lpstModule->lpstStream,
                       lpstModule->stTypeTable.dwOffset + dwOffset + 2,
                       lpwIdent,
                       sizeof(WORD),
                       &dwBytesRead) != SS_STATUS_OK)
            return(FALSE);

        *lpwIdent = SSEndianWORD((LPBYTE)lpwIdent,
                                 lpstModule->bLittleEndian);
    }

    if (lpdwNextParamOffset != NULL)
    {
        if (SSSeekRead(lpstModule->lpstStream,
                       lpstModule->stTypeTable.dwOffset + dwOffset + 0x14,
                       lpdwNextParamOffset,
                       sizeof(DWORD),
                       &dwBytesRead) != SS_STATUS_OK)
            return(FALSE);

        *lpdwNextParamOffset = SSEndianDWORD((LPBYTE)lpdwNextParamOffset,
                                             lpstModule->bLittleEndian);
    }

    return(TRUE);
}


//********************************************************************
//
// Function:
//  int O97GetOpSkip()
//
// Parameters:
//  lpabyOp             Ptr to array of op bytes
//  nNumOpBytes         Number of bytes in array
//
// Description:
//  Calculates the number of operand bytes for the opcode at the
//  given pointer.  If the number of bytes needed to determine the
//  number of operand bytes for the opcode are not within nNumOpBytes,
//  the function returns -1.
//
// Returns:
//  >= 0            On success
//  < 0             On error
//
//********************************************************************

// The following table contains entries of the form:
//
//      A * 0x80 + B
//
// If A is 0, then B gives the constant number of operand bytes.
// If A is non-zero, then the opcode has a variable number of operands.
//  Except for opcode 0xD7 (quoted comment), the number of operand
//  bytes is given by the WORD value following the opcode WORD.
//  For opcode 0xD7, the number of additional operand bytes is given
//  in the second WORD following the opcode WORD.

#ifdef SYM_WIN16
#pragma data_seg("FAR_DATA_0")
#endif

BYTE FAR gabyO97OpSkip[O97_OP_MAX+1] =
{
    0 * 0x80 + 0,   // 0x00 // Imp
    0 * 0x80 + 0,   // 0x01 // Eqv
    0 * 0x80 + 0,   // 0x02 // Xor
    0 * 0x80 + 0,   // 0x03 // Or
    0 * 0x80 + 0,   // 0x04 // And
    0 * 0x80 + 0,   // 0x05 // Eq
    0 * 0x80 + 0,   // 0x06 // Ne
    0 * 0x80 + 0,   // 0x07 // Le
    0 * 0x80 + 0,   // 0x08 // Ge
    0 * 0x80 + 0,   // 0x09 // Lt
    0 * 0x80 + 0,   // 0x0A // Gt
    0 * 0x80 + 0,   // 0x0B // Add
    0 * 0x80 + 0,   // 0x0C // Sub
    0 * 0x80 + 0,   // 0x0D // Mod
    0 * 0x80 + 0,   // 0x0E // IDv
    0 * 0x80 + 0,   // 0x0F // Mul
    0 * 0x80 + 0,   // 0x10 // Div
    0 * 0x80 + 0,   // 0x11 // Concat
    0 * 0x80 + 0,   // 0x12 // Like
    0 * 0x80 + 0,   // 0x13 // Pwr
    0 * 0x80 + 0,   // 0x14 // Is
    0 * 0x80 + 0,   // 0x15 // Not
    0 * 0x80 + 0,   // 0x16 // UMi
    0 * 0x80 + 0,   // 0x17 // FnAbs
    0 * 0x80 + 0,   // 0x18 // FnFix
    0 * 0x80 + 0,   // 0x19 // FnInt
    0 * 0x80 + 0,   // 0x1A // FnSgn
    0 * 0x80 + 0,   // 0x1B // FnLen
    0 * 0x80 + 0,   // 0x1C // FnLenB
    0 * 0x80 + 0,   // 0x1D // Paren
    0 * 0x80 + 0,   // 0x1E // Sharp
    0 * 0x80 + 2,   // 0x1F // LdLHS
    0 * 0x80 + 2,   // 0x20 // Ld
    0 * 0x80 + 2,   // 0x21 // MemLd
    0 * 0x80 + 2,   // 0x22 // DictLd
    0 * 0x80 + 2,   // 0x23 // IndexLd
    0 * 0x80 + 4,   // 0x24 // ArgsLd
    0 * 0x80 + 4,   // 0x25 // ArgsMemLd
    0 * 0x80 + 4,   // 0x26 // ArgsDictLd
    0 * 0x80 + 2,   // 0x27 // St
    0 * 0x80 + 2,   // 0x28 // MemSt
    0 * 0x80 + 2,   // 0x29 // DictSt
    0 * 0x80 + 2,   // 0x2A // IndexSt
    0 * 0x80 + 4,   // 0x2B // ArgsSt
    0 * 0x80 + 4,   // 0x2C // ArgsMemSt
    0 * 0x80 + 4,   // 0x2D // ArgsDictSt
    0 * 0x80 + 2,   // 0x2E // Set
    0 * 0x80 + 2,   // 0x2F // MemSet
    0 * 0x80 + 2,   // 0x30 // DictSet
    0 * 0x80 + 2,   // 0x31 // IndexSet
    0 * 0x80 + 4,   // 0x32 // ArgsSet
    0 * 0x80 + 4,   // 0x33 // ArgsMemSet
    0 * 0x80 + 4,   // 0x34 // ArgsDictSet
    0 * 0x80 + 2,   // 0x35 // MemLdWith
    0 * 0x80 + 2,   // 0x36 // DictLdWith
    0 * 0x80 + 4,   // 0x37 // ArgsMemLdWith
    0 * 0x80 + 4,   // 0x38 // ArgsDictLdWith
    0 * 0x80 + 2,   // 0x39 // MemStWith
    0 * 0x80 + 2,   // 0x3A // DictStWith
    0 * 0x80 + 4,   // 0x3B // ArgsMemStWith
    0 * 0x80 + 4,   // 0x3C // ArgsDictStWith
    0 * 0x80 + 2,   // 0x3D // MemSetWith
    0 * 0x80 + 2,   // 0x3E // DictSetWith
    0 * 0x80 + 4,   // 0x3F // ArgsMemSetWith
    0 * 0x80 + 4,   // 0x40 // ArgsDictSetWith
    0 * 0x80 + 4,   // 0x41 // ArgsCall
    0 * 0x80 + 4,   // 0x42 // ArgsMemCall
    0 * 0x80 + 4,   // 0x43 // ArgsMemCallWith
    0 * 0x80 + 4,   // 0x44 // ArgsArray
    0 * 0x80 + 2,   // 0x45 // Bos
    0 * 0x80 + 0,   // 0x46 // BosImplicit
    0 * 0x80 + 0,   // 0x47 // Bol
    0 * 0x80 + 0,   // 0x48 // Case
    0 * 0x80 + 0,   // 0x49 // CaseTo
    0 * 0x80 + 0,   // 0x4A // CaseGt
    0 * 0x80 + 0,   // 0x4B // CaseLt
    0 * 0x80 + 0,   // 0x4C // CaseGe
    0 * 0x80 + 0,   // 0x4D // CaseLe
    0 * 0x80 + 0,   // 0x4E // CaseNe
    0 * 0x80 + 0,   // 0x4F // CaseEq
    0 * 0x80 + 0,   // 0x50 // CaseElse
    0 * 0x80 + 0,   // 0x51 // CaseDone
    0 * 0x80 + 2,   // 0x52 // Circle
    0 * 0x80 + 2,   // 0x53 // Close
    0 * 0x80 + 0,   // 0x54 // CloseAll
    0 * 0x80 + 0,   // 0x55 // Coerce
    0 * 0x80 + 0,   // 0x56 // CoerceVar
    0 * 0x80 + 4,   // 0x57 // Context
    0 * 0x80 + 0,   // 0x58 // Debug
    0 * 0x80 + 4,   // 0x59 // Deftype
    0 * 0x80 + 0,   // 0x5A // Dim
    0 * 0x80 + 0,   // 0x5B // DimImplicit
    0 * 0x80 + 0,   // 0x5C // Do
    0 * 0x80 + 0,   // 0x5D // DoEvents
    0 * 0x80 + 0,   // 0x5E // DoUntil
    0 * 0x80 + 0,   // 0x5F // DoWhile
    0 * 0x80 + 0,   // 0x60 // Else
    0 * 0x80 + 0,   // 0x61 // ElseBlock
    0 * 0x80 + 0,   // 0x62 // ElseIfBlock
    0 * 0x80 + 2,   // 0x63 // ElseIfTypeBlock
    0 * 0x80 + 0,   // 0x64 // End
    0 * 0x80 + 0,   // 0x65 // EndContext
    0 * 0x80 + 0,   // 0x66 // EndFunc
    0 * 0x80 + 0,   // 0x67 // EndIf
    0 * 0x80 + 0,   // 0x68 // EndIfBlock
    0 * 0x80 + 0,   // 0x69 // EndImmediate
    0 * 0x80 + 0,   // 0x6A // EndProp
    0 * 0x80 + 0,   // 0x6B // EndSelect
    0 * 0x80 + 0,   // 0x6C // EndSub
    0 * 0x80 + 0,   // 0x6D // EndType
    0 * 0x80 + 0,   // 0x6E // EndWith
    0 * 0x80 + 2,   // 0x6F // Erase
    0 * 0x80 + 0,   // 0x70 // Error
    0 * 0x80 + 0,   // 0x71 // ExitDo
    0 * 0x80 + 0,   // 0x72 // ExitFor
    0 * 0x80 + 0,   // 0x73 // ExitFunc
    0 * 0x80 + 0,   // 0x74 // ExitProp
    0 * 0x80 + 0,   // 0x75 // ExitSub
    0 * 0x80 + 0,   // 0x76 // FnCurDir
    0 * 0x80 + 0,   // 0x77 // FnDir
    0 * 0x80 + 0,   // 0x78 // Empty0
    0 * 0x80 + 0,   // 0x79 // Empty1
    0 * 0x80 + 0,   // 0x7A // FnError
    0 * 0x80 + 0,   // 0x7B // FnFormat
    0 * 0x80 + 0,   // 0x7C // FnFreeFile
    0 * 0x80 + 0,   // 0x7D // FnInStr
    0 * 0x80 + 0,   // 0x7E // FnInStr3
    0 * 0x80 + 0,   // 0x7F // FnInStr4
    0 * 0x80 + 0,   // 0x80 // FnInStrB
    0 * 0x80 + 0,   // 0x81 // FnInStrB3
    0 * 0x80 + 0,   // 0x82 // FnInStrB4
    0 * 0x80 + 2,   // 0x83 // FnLBound
    0 * 0x80 + 0,   // 0x84 // FnMid
    0 * 0x80 + 0,   // 0x85 // FnMidB
    0 * 0x80 + 0,   // 0x86 // FnStrComp
    0 * 0x80 + 0,   // 0x87 // FnStrComp3
    0 * 0x80 + 0,   // 0x88 // FnStringVar
    0 * 0x80 + 0,   // 0x89 // FnStringStr
    0 * 0x80 + 2,   // 0x8A // FnUBound
    0 * 0x80 + 0,   // 0x8B // For
    0 * 0x80 + 0,   // 0x8C // ForEach
    0 * 0x80 + 2,   // 0x8D // ForEachAs
    0 * 0x80 + 0,   // 0x8E // ForStep
    0 * 0x80 + 4,   // 0x8F // FuncDefn
    0 * 0x80 + 4,   // 0x90 // FuncDefnSave
    0 * 0x80 + 0,   // 0x91 // GetRec
    0 * 0x80 + 2,   // 0x92 // Gosub
    0 * 0x80 + 2,   // 0x93 // Goto
    0 * 0x80 + 0,   // 0x94 // If
    0 * 0x80 + 0,   // 0x95 // IfBlock
    0 * 0x80 + 2,   // 0x96 // TypeOf
    0 * 0x80 + 2,   // 0x97 // IfTypeBlock
    0 * 0x80 + 0,   // 0x98 // Input
    0 * 0x80 + 0,   // 0x99 // InputDone
    0 * 0x80 + 0,   // 0x9A // InputItem
    0 * 0x80 + 2,   // 0x9B // Label
    0 * 0x80 + 0,   // 0x9C // Let
    0 * 0x80 + 2,   // 0x9D // Line
    1 * 0x80 + 0,   // 0x9E // LineCont
    0 * 0x80 + 0,   // 0x9F // LineInput
    0 * 0x80 + 2,   // 0xA0 // LineNum
    0 * 0x80 + 8,   // 0xA1 // LitCy
    0 * 0x80 + 8,   // 0xA2 // LitDate
    0 * 0x80 + 0,   // 0xA3 // LitDefault
    0 * 0x80 + 2,   // 0xA4 // LitDI2
    0 * 0x80 + 4,   // 0xA5 // LitDI4
    0 * 0x80 + 2,   // 0xA6 // LitHI2
    0 * 0x80 + 4,   // 0xA7 // LitHI4
    0 * 0x80 + 0,   // 0xA8 // LitNothing
    0 * 0x80 + 2,   // 0xA9 // LitOI2
    0 * 0x80 + 4,   // 0xAA // LitOI4
    0 * 0x80 + 4,   // 0xAB // LitR4
    0 * 0x80 + 8,   // 0xAC // LitR8
    0 * 0x80 + 0,   // 0xAD // LitSmallI2
    1 * 0x80 + 0,   // 0xAE // LitStr
    0 * 0x80 + 0,   // 0xAF // LitVarSpecial
    0 * 0x80 + 0,   // 0xB0 // Lock
    0 * 0x80 + 0,   // 0xB1 // Loop
    0 * 0x80 + 0,   // 0xB2 // LoopUntil
    0 * 0x80 + 0,   // 0xB3 // LoopWhile
    0 * 0x80 + 0,   // 0xB4 // Lset
    0 * 0x80 + 0,   // 0xB5 // Me
    0 * 0x80 + 0,   // 0xB6 // MeImplicit
    0 * 0x80 + 8,   // 0xB7 // MemRedim
    0 * 0x80 + 8,   // 0xB8 // MemRedimWith
    0 * 0x80 + 8,   // 0xB9 // MemRedimAs
    0 * 0x80 + 8,   // 0xBA // MemRedimAsWith
    0 * 0x80 + 0,   // 0xBB // Mid
    0 * 0x80 + 0,   // 0xBC // MidB
    0 * 0x80 + 0,   // 0xBD // Name
    0 * 0x80 + 2,   // 0xBE // New
    0 * 0x80 + 0,   // 0xBF // Next
    0 * 0x80 + 0,   // 0xC0 // NextVar
    0 * 0x80 + 2,   // 0xC1 // OnError
    1 * 0x80 + 0,   // 0xC2 // OnGosub
    1 * 0x80 + 0,   // 0xC3 // OnGoto
    0 * 0x80 + 2,   // 0xC4 // Open
    0 * 0x80 + 0,   // 0xC5 // Option
    0 * 0x80 + 0,   // 0xC6 // OptionBase
    0 * 0x80 + 0,   // 0xC7 // ParamByval
    0 * 0x80 + 0,   // 0xC8 // ParamOmitted
    0 * 0x80 + 2,   // 0xC9 // ParamNamed
    0 * 0x80 + 0,   // 0xCA // PrintChan
    0 * 0x80 + 0,   // 0xCB // PrintComma
    0 * 0x80 + 0,   // 0xCC // PrintEos
    0 * 0x80 + 0,   // 0xCD // PrintItemComma
    0 * 0x80 + 0,   // 0xCE // PrintItemNL
    0 * 0x80 + 0,   // 0xCF // PrintItemSemi
    0 * 0x80 + 0,   // 0xD0 // PrintNL
    0 * 0x80 + 0,   // 0xD1 // PrintObj
    0 * 0x80 + 0,   // 0xD2 // PrintSemi
    0 * 0x80 + 0,   // 0xD3 // PrintSpc
    0 * 0x80 + 0,   // 0xD4 // PrintTab
    0 * 0x80 + 0,   // 0xD5 // PrintTabComma
    0 * 0x80 + 2,   // 0xD6 // Pset
    0 * 0x80 + 0,   // 0xD7 // PutRec
    1 * 0x80 + 0,   // 0xD8 // QuoteRem
    0 * 0x80 + 8,   // 0xD9 // Redim
    0 * 0x80 + 8,   // 0xDA // RedimAs
    1 * 0x80 + 0,   // 0xDB // Reparse
    1 * 0x80 + 0,   // 0xDC // Rem
    0 * 0x80 + 2,   // 0xDD // Resume
    0 * 0x80 + 0,   // 0xDE // Return
    0 * 0x80 + 0,   // 0xDF // Rset
    0 * 0x80 + 2,   // 0xE0 // Scale
    0 * 0x80 + 0,   // 0xE1 // Seek
    0 * 0x80 + 0,   // 0xE2 // SelectCase
    0 * 0x80 + 2,   // 0xE3 // SelectIs
    0 * 0x80 + 0,   // 0xE4 // SelectType
    0 * 0x80 + 0,   // 0xE5 // SetStmt
    0 * 0x80 + 6,   // 0xE6 // Stack
    0 * 0x80 + 0,   // 0xE7 // Stop
    0 * 0x80 + 4,   // 0xE8 // Type
    0 * 0x80 + 0,   // 0xE9 // UnLock
    1 * 0x80 + 0,   // 0xEA // VarDefn
    0 * 0x80 + 0,   // 0xEB // Wend
    0 * 0x80 + 0,   // 0xEC // While
    0 * 0x80 + 0,   // 0xED // With
    0 * 0x80 + 0,   // 0xEE // WriteChan
    0 * 0x80 + 0,   // 0xEF // ConstFuncExpr
    0 * 0x80 + 2,   // 0xF0 // LbConst
    0 * 0x80 + 0,   // 0xF1 // LbIf
    0 * 0x80 + 0,   // 0xF2 // LbElse
    0 * 0x80 + 0,   // 0xF3 // LbElseif
    0 * 0x80 + 0,   // 0xF4 // LbEndif
    0 * 0x80 + 0,   // 0xF5 // LbMark
    0 * 0x80 + 0,   // 0xF6 // EndForVariable
    0 * 0x80 + 0,   // 0xF7 // StartForVariable
    0 * 0x80 + 0,   // 0xF8 // NewRedim
    0 * 0x80 + 0,   // 0xF9 // StartWithExpr
    0 * 0x80 + 2,   // 0xFA // SetOrSt
    0 * 0x80 + 0,   // 0xFB // EndEnum
    0 * 0x80 + 0,   // 0xFC // Illegal
};

#ifdef SYM_WIN16
#pragma data_seg()
#endif

int O97GetOpSkip
(
    BOOL        bLittleEndian,
    LPBYTE      lpabyOp,
    int         nNumOpBytes
)
{
    int         nOpcode;
    WORD        wTemp;

    if (nNumOpBytes < 2)
        return(-1);

    if (bLittleEndian)
        nOpcode = lpabyOp[0];
    else
        nOpcode = lpabyOp[1];

    if (nOpcode > O97_OP_MAX)
        nOpcode = O97_OP_MAX;

    if (gabyO97OpSkip[nOpcode] < 0x80)
        return(gabyO97OpSkip[nOpcode]);

    if (nOpcode == O97_OP_VAR_DEFN)
    {
        if ((bLittleEndian && (lpabyOp[1] & 0x40)) ||
            (bLittleEndian == FALSE && (lpabyOp[1] & 0x40) == 0))
            return(6);
        else
            return(4);
    }

    if (nOpcode == O97_OP_QUOTED_COMMENT)
    {
        if (nNumOpBytes < 6)
            return(-1);

        wTemp = SSEndianWORD(lpabyOp+4,bLittleEndian);

        if (wTemp & 1)
            ++wTemp;

        return(4 + (int)wTemp);
    }
    else
    {
        if (nNumOpBytes < 4)
            return(-1);

        wTemp = SSEndianWORD(lpabyOp+2,bLittleEndian);

        if (wTemp & 1)
            ++wTemp;

        return(2 + (int)wTemp);
    }
}


//********************************************************************
//
// Function:
//  int O98GetOpSkip()
//
// Parameters:
//  lpabyOp             Ptr to array of op bytes
//  nNumOpBytes         Number of bytes in array
//
// Description:
//  Calculates the number of operand bytes for the opcode at the
//  given pointer.  If the number of bytes needed to determine the
//  number of operand bytes for the opcode are not within nNumOpBytes,
//  the function returns -1.
//
// Returns:
//  >= 0            On success
//  < 0             On error
//
//********************************************************************

// The following table contains entries of the form:
//
//      A * 0x80 + B
//
// If A is 0, then B gives the constant number of operand bytes.
// If A is non-zero, then the opcode has a variable number of operands.
//  Except for opcode 0xD7 (quoted comment), the number of operand
//  bytes is given by the WORD value following the opcode WORD.
//  For opcode 0xD7, the number of additional operand bytes is given
//  in the second WORD following the opcode WORD.

#ifdef SYM_WIN16
#pragma data_seg("FAR_DATA_0")
#endif

BYTE FAR gabyO98OpSkip[O98_OP_MAX+1] =
{
    0 * 0x80 + 0,   // 0x00 // Imp
    0 * 0x80 + 0,   // 0x01 // Eqv
    0 * 0x80 + 0,   // 0x02 // Xor
    0 * 0x80 + 0,   // 0x03 // Or
    0 * 0x80 + 0,   // 0x04 // And
    0 * 0x80 + 0,   // 0x05 // Eq
    0 * 0x80 + 0,   // 0x06 // Ne
    0 * 0x80 + 0,   // 0x07 // Le
    0 * 0x80 + 0,   // 0x08 // Ge
    0 * 0x80 + 0,   // 0x09 // Lt
    0 * 0x80 + 0,   // 0x0A // Gt
    0 * 0x80 + 0,   // 0x0B // Add
    0 * 0x80 + 0,   // 0x0C // Sub
    0 * 0x80 + 0,   // 0x0D // Mod
    0 * 0x80 + 0,   // 0x0E // IDv
    0 * 0x80 + 0,   // 0x0F // Mul
    0 * 0x80 + 0,   // 0x10 // Div
    0 * 0x80 + 0,   // 0x11 // Concat
    0 * 0x80 + 0,   // 0x12 // Like
    0 * 0x80 + 0,   // 0x13 // Pwr
    0 * 0x80 + 0,   // 0x14 // Is
    0 * 0x80 + 0,   // 0x15 // Not
    0 * 0x80 + 0,   // 0x16 // UMi
    0 * 0x80 + 0,   // 0x17 // FnAbs
    0 * 0x80 + 0,   // 0x18 // FnFix
    0 * 0x80 + 0,   // 0x19 // FnInt
    0 * 0x80 + 0,   // 0x1A // FnSgn
    0 * 0x80 + 0,   // 0x1B // FnLen
    0 * 0x80 + 0,   // 0x1C // FnLenB
    0 * 0x80 + 0,   // 0x1D // Paren
    0 * 0x80 + 0,   // 0x1E // Sharp
    0 * 0x80 + 2,   // 0x1F // LdLHS
    0 * 0x80 + 2,   // 0x20 // Ld
    0 * 0x80 + 2,   // 0x21 // MemLd
    0 * 0x80 + 2,   // 0x22 // DictLd
    0 * 0x80 + 2,   // 0x23 // IndexLd
    0 * 0x80 + 4,   // 0x24 // ArgsLd
    0 * 0x80 + 4,   // 0x25 // ArgsMemLd
    0 * 0x80 + 4,   // 0x26 // ArgsDictLd
    0 * 0x80 + 2,   // 0x27 // St
    0 * 0x80 + 2,   // 0x28 // MemSt
    0 * 0x80 + 2,   // 0x29 // DictSt
    0 * 0x80 + 2,   // 0x2A // IndexSt
    0 * 0x80 + 4,   // 0x2B // ArgsSt
    0 * 0x80 + 4,   // 0x2C // ArgsMemSt
    0 * 0x80 + 4,   // 0x2D // ArgsDictSt
    0 * 0x80 + 2,   // 0x2E // Set
    0 * 0x80 + 2,   // 0x2F // MemSet
    0 * 0x80 + 2,   // 0x30 // DictSet
    0 * 0x80 + 2,   // 0x31 // IndexSet
    0 * 0x80 + 4,   // 0x32 // ArgsSet
    0 * 0x80 + 4,   // 0x33 // ArgsMemSet
    0 * 0x80 + 4,   // 0x34 // ArgsDictSet
    0 * 0x80 + 2,   // 0x35 // MemLdWith
    0 * 0x80 + 2,   // 0x36 // DictLdWith
    0 * 0x80 + 4,   // 0x37 // ArgsMemLdWith
    0 * 0x80 + 4,   // 0x38 // ArgsDictLdWith
    0 * 0x80 + 2,   // 0x39 // MemStWith
    0 * 0x80 + 2,   // 0x3A // DictStWith
    0 * 0x80 + 4,   // 0x3B // ArgsMemStWith
    0 * 0x80 + 4,   // 0x3C // ArgsDictStWith
    0 * 0x80 + 2,   // 0x3D // MemSetWith
    0 * 0x80 + 2,   // 0x3E // DictSetWith
    0 * 0x80 + 4,   // 0x3F // ArgsMemSetWith
    0 * 0x80 + 4,   // 0x40 // ArgsDictSetWith
    0 * 0x80 + 4,   // 0x41 // ArgsCall
    0 * 0x80 + 4,   // 0x42 // ArgsMemCall
    0 * 0x80 + 4,   // 0x43 // ArgsMemCallWith
    0 * 0x80 + 4,   // 0x44 // ArgsArray
    0 * 0x80 + 0,   // 0x45 // Assert
    0 * 0x80 + 2,   // 0x46 // Bos
    0 * 0x80 + 0,   // 0x47 // BosImplicit
    0 * 0x80 + 0,   // 0x48 // Bol
    0 * 0x80 + 2,   // 0x49 // LdAddressOf
    0 * 0x80 + 2,   // 0x4A // MemAddressOf
    0 * 0x80 + 0,   // 0x4B // Case
    0 * 0x80 + 0,   // 0x4C // CaseTo
    0 * 0x80 + 0,   // 0x4D // CaseGt
    0 * 0x80 + 0,   // 0x4E // CaseLt
    0 * 0x80 + 0,   // 0x4F // CaseGe
    0 * 0x80 + 0,   // 0x50 // CaseLe
    0 * 0x80 + 0,   // 0x51 // CaseNe
    0 * 0x80 + 0,   // 0x52 // CaseEq
    0 * 0x80 + 0,   // 0x53 // CaseElse
    0 * 0x80 + 0,   // 0x54 // CaseDone
    0 * 0x80 + 2,   // 0x55 // Circle
    0 * 0x80 + 2,   // 0x56 // Close
    0 * 0x80 + 0,   // 0x57 // CloseAll
    0 * 0x80 + 0,   // 0x58 // Coerce
    0 * 0x80 + 0,   // 0x59 // CoerceVar
    0 * 0x80 + 4,   // 0x5A // Context
    0 * 0x80 + 0,   // 0x5B // Debug
    0 * 0x80 + 4,   // 0x5C // Deftype
    0 * 0x80 + 0,   // 0x5D // Dim
    0 * 0x80 + 0,   // 0x5E // DimImplicit
    0 * 0x80 + 0,   // 0x5F // Do
    0 * 0x80 + 0,   // 0x60 // DoEvents
    0 * 0x80 + 0,   // 0x61 // DoUntil
    0 * 0x80 + 0,   // 0x62 // DoWhile
    0 * 0x80 + 0,   // 0x63 // Else
    0 * 0x80 + 0,   // 0x64 // ElseBlock
    0 * 0x80 + 0,   // 0x65 // ElseIfBlock
    0 * 0x80 + 2,   // 0x66 // ElseIfTypeBlock
    0 * 0x80 + 0,   // 0x67 // End
    0 * 0x80 + 0,   // 0x68 // EndContext
    0 * 0x80 + 0,   // 0x69 // EndFunc
    0 * 0x80 + 0,   // 0x6A // EndIf
    0 * 0x80 + 0,   // 0x6B // EndIfBlock
    0 * 0x80 + 0,   // 0x6C // EndImmediate
    0 * 0x80 + 0,   // 0x6D // EndProp
    0 * 0x80 + 0,   // 0x6E // EndSelect
    0 * 0x80 + 0,   // 0x6F // EndSub
    0 * 0x80 + 0,   // 0x70 // EndType
    0 * 0x80 + 0,   // 0x71 // EndWith
    0 * 0x80 + 2,   // 0x72 // Erase
    0 * 0x80 + 0,   // 0x73 // Error
    0 * 0x80 + 4,   // 0x74 // EventDecl
    0 * 0x80 + 4,   // 0x75 // RaiseEvent
    0 * 0x80 + 4,   // 0x76 // ArgsMemRaiseEvent
    0 * 0x80 + 4,   // 0x77 // ArgsMemRaiseEventWith
    0 * 0x80 + 0,   // 0x78 // ExitDo
    0 * 0x80 + 0,   // 0x79 // ExitFor
    0 * 0x80 + 0,   // 0x7A // ExitFunc
    0 * 0x80 + 0,   // 0x7B // ExitProp
    0 * 0x80 + 0,   // 0x7C // ExitSub
    0 * 0x80 + 0,   // 0x7D // FnCurDir
    0 * 0x80 + 0,   // 0x7E // FnDir
    0 * 0x80 + 0,   // 0x7F // Empty0
    0 * 0x80 + 0,   // 0x80 // Empty1
    0 * 0x80 + 0,   // 0x81 // FnError
    0 * 0x80 + 0,   // 0x82 // FnFormat
    0 * 0x80 + 0,   // 0x83 // FnFreeFile
    0 * 0x80 + 0,   // 0x84 // FnInStr
    0 * 0x80 + 0,   // 0x85 // FnInStr3
    0 * 0x80 + 0,   // 0x86 // FnInStr4
    0 * 0x80 + 0,   // 0x87 // FnInStrB
    0 * 0x80 + 0,   // 0x88 // FnInStrB3
    0 * 0x80 + 0,   // 0x89 // FnInStrB4
    0 * 0x80 + 2,   // 0x8A // FnLBound
    0 * 0x80 + 0,   // 0x8B // FnMid
    0 * 0x80 + 0,   // 0x8C // FnMidB
    0 * 0x80 + 0,   // 0x8D // FnStrComp
    0 * 0x80 + 0,   // 0x8E // FnStrComp3
    0 * 0x80 + 0,   // 0x8F // FnStringVar
    0 * 0x80 + 0,   // 0x90 // FnStringStr
    0 * 0x80 + 2,   // 0x91 // FnUBound
    0 * 0x80 + 0,   // 0x92 // For
    0 * 0x80 + 0,   // 0x93 // ForEach
    0 * 0x80 + 2,   // 0x94 // ForEachAs
    0 * 0x80 + 0,   // 0x95 // ForStep
    0 * 0x80 + 4,   // 0x96 // FuncDefn
    0 * 0x80 + 4,   // 0x97 // FuncDefnSave
    0 * 0x80 + 0,   // 0x98 // GetRec
    0 * 0x80 + 2,   // 0x99 // Gosub
    0 * 0x80 + 2,   // 0x9A // Goto
    0 * 0x80 + 0,   // 0x9B // If
    0 * 0x80 + 0,   // 0x9C // IfBlock
    0 * 0x80 + 2,   // 0x9D // TypeOf
    0 * 0x80 + 2,   // 0x9E // IfTypeBlock
    0 * 0x80 + 4,   // 0x9F // Implements
    0 * 0x80 + 0,   // 0xA0 // Input
    0 * 0x80 + 0,   // 0xA1 // InputDone
    0 * 0x80 + 0,   // 0xA2 // InputItem
    0 * 0x80 + 2,   // 0xA3 // Label
    0 * 0x80 + 0,   // 0xA4 // Let
    0 * 0x80 + 2,   // 0xA5 // Line
    1 * 0x80 + 0,   // 0xA6 // LineCont
    0 * 0x80 + 0,   // 0xA7 // LineInput
    0 * 0x80 + 2,   // 0xA8 // LineNum
    0 * 0x80 + 8,   // 0xA9 // LitCy
    0 * 0x80 + 8,   // 0xAA // LitDate
    0 * 0x80 + 0,   // 0xAB // LitDefault
    0 * 0x80 + 2,   // 0xAC // LitDI2
    0 * 0x80 + 4,   // 0xAD // LitDI4
    0 * 0x80 + 2,   // 0xAE // LitHI2
    0 * 0x80 + 4,   // 0xAF // LitHI4
    0 * 0x80 + 0,   // 0xB0 // LitNothing
    0 * 0x80 + 2,   // 0xB1 // LitOI2
    0 * 0x80 + 4,   // 0xB2 // LitOI4
    0 * 0x80 + 4,   // 0xB3 // LitR4
    0 * 0x80 + 8,   // 0xB4 // LitR8
    0 * 0x80 + 0,   // 0xB5 // LitSmallI2
    1 * 0x80 + 0,   // 0xB6 // LitStr
    0 * 0x80 + 0,   // 0xB7 // LitVarSpecial
    0 * 0x80 + 0,   // 0xB8 // Lock
    0 * 0x80 + 0,   // 0xB9 // Loop
    0 * 0x80 + 0,   // 0xBA // LoopUntil
    0 * 0x80 + 0,   // 0xBB // LoopWhile
    0 * 0x80 + 0,   // 0xBC // Lset
    0 * 0x80 + 0,   // 0xBD // Me
    0 * 0x80 + 0,   // 0xBE // MeImplicit
    0 * 0x80 + 8,   // 0xBF // MemRedim
    0 * 0x80 + 8,   // 0xC0 // MemRedimWith
    0 * 0x80 + 8,   // 0xC1 // MemRedimAs
    0 * 0x80 + 8,   // 0xC2 // MemRedimAsWith
    0 * 0x80 + 0,   // 0xC3 // Mid
    0 * 0x80 + 0,   // 0xC4 // MidB
    0 * 0x80 + 0,   // 0xC5 // Name
    0 * 0x80 + 2,   // 0xC6 // New
    0 * 0x80 + 0,   // 0xC7 // Next
    0 * 0x80 + 0,   // 0xC8 // NextVar
    0 * 0x80 + 2,   // 0xC9 // OnError
    1 * 0x80 + 0,   // 0xCA // OnGosub
    1 * 0x80 + 0,   // 0xCB // OnGoto
    0 * 0x80 + 2,   // 0xCC // Open
    0 * 0x80 + 0,   // 0xCD // Option
    0 * 0x80 + 0,   // 0xCE // OptionBase
    0 * 0x80 + 0,   // 0xCF // ParamByval
    0 * 0x80 + 0,   // 0xD0 // ParamOmitted
    0 * 0x80 + 2,   // 0xD1 // ParamNamed
    0 * 0x80 + 0,   // 0xD2 // PrintChan
    0 * 0x80 + 0,   // 0xD3 // PrintComma
    0 * 0x80 + 0,   // 0xD4 // PrintEos
    0 * 0x80 + 0,   // 0xD5 // PrintItemComma
    0 * 0x80 + 0,   // 0xD6 // PrintItemNL
    0 * 0x80 + 0,   // 0xD7 // PrintItemSemi
    0 * 0x80 + 0,   // 0xD8 // PrintNL
    0 * 0x80 + 0,   // 0xD9 // PrintObj
    0 * 0x80 + 0,   // 0xDA // PrintSemi
    0 * 0x80 + 0,   // 0xDB // PrintSpc
    0 * 0x80 + 0,   // 0xDC // PrintTab
    0 * 0x80 + 0,   // 0xDD // PrintTabComma
    0 * 0x80 + 2,   // 0xDE // Pset
    0 * 0x80 + 0,   // 0xDF // PutRec
    1 * 0x80 + 0,   // 0xE0 // QuoteRem
    0 * 0x80 + 8,   // 0xE1 // Redim
    0 * 0x80 + 8,   // 0xE2 // RedimAs
    1 * 0x80 + 0,   // 0xE3 // Reparse
    1 * 0x80 + 0,   // 0xE4 // Rem
    0 * 0x80 + 2,   // 0xE5 // Resume
    0 * 0x80 + 0,   // 0xE6 // Return
    0 * 0x80 + 0,   // 0xE7 // Rset
    0 * 0x80 + 2,   // 0xE8 // Scale
    0 * 0x80 + 0,   // 0xE9 // Seek
    0 * 0x80 + 0,   // 0xEA // SelectCase
    0 * 0x80 + 2,   // 0xEB // SelectIs
    0 * 0x80 + 0,   // 0xEC // SelectType
    0 * 0x80 + 0,   // 0xED // SetStmt
    0 * 0x80 + 6,   // 0xEE // Stack
    0 * 0x80 + 0,   // 0xEF // Stop
    0 * 0x80 + 4,   // 0xF0 // Type
    0 * 0x80 + 0,   // 0xF1 // UnLock
    1 * 0x80 + 0,   // 0xF2 // VarDefn
    0 * 0x80 + 0,   // 0xF3 // Wend
    0 * 0x80 + 0,   // 0xF4 // While
    0 * 0x80 + 0,   // 0xF5 // With
    0 * 0x80 + 0,   // 0xF6 // WriteChan
    0 * 0x80 + 0,   // 0xF7 // ConstFuncExpr
    0 * 0x80 + 2,   // 0xF8 // LbConst
    0 * 0x80 + 0,   // 0xF9 // LbIf
    0 * 0x80 + 0,   // 0xFA // LbElse
    0 * 0x80 + 0,   // 0xFB // LbElseif
    0 * 0x80 + 0,   // 0xFC // LbEndif
    0 * 0x80 + 0,   // 0xFD // LbMark
    0 * 0x80 + 0,   // 0xFE // EndForVariable
    0 * 0x80 + 0,   // 0xFF // StartForVariable
    0 * 0x80 + 0,   // 0x100 // NewRedim
    0 * 0x80 + 0,   // 0x101 // StartWithExpr
    0 * 0x80 + 2,   // 0x102 // SetOrSt
    0 * 0x80 + 0,   // 0x103 // EndEnum
    0 * 0x80 + 0    // 0x104 // Illegal
};

#ifdef SYM_WIN16
#pragma data_seg()
#endif

int O98GetOpSkip
(
    BOOL        bLittleEndian,
    LPBYTE      lpabyOp,
    int         nNumOpBytes
)
{
    int         nOpcode;
    WORD        wTemp;

    if (nNumOpBytes < 2)
        return(-1);

    if (bLittleEndian)
        nOpcode = lpabyOp[0] | ((lpabyOp[1] & 0x01) << 8);
    else
        nOpcode = lpabyOp[1] | ((lpabyOp[0] & 0x01) << 8);

    if (nOpcode > O98_OP_MAX)
        nOpcode = O98_OP_MAX;

    if (gabyO98OpSkip[nOpcode] < 0x80)
        return(gabyO98OpSkip[nOpcode]);

    if (nOpcode == O98_OP_VAR_DEFN)
    {
        if ((bLittleEndian && (lpabyOp[1] & 0x40)) ||
            (bLittleEndian == FALSE && (lpabyOp[1] & 0x40) == 0))
            return(6);
        else
            return(4);
    }

    if (nOpcode == O98_OP_QUOTED_COMMENT)
    {
        if (nNumOpBytes < 6)
            return(-1);

        wTemp = SSEndianWORD(lpabyOp+4,bLittleEndian);

        if (wTemp & 1)
            ++wTemp;

        return(4 + (int)wTemp);
    }
    else
    {
        if (nNumOpBytes < 4)
            return(-1);

        wTemp = SSEndianWORD(lpabyOp+2,bLittleEndian);

        if (wTemp & 1)
            ++wTemp;

        return(2 + (int)wTemp);
    }
}


//********************************************************************
//
// Function:
//  BOOL O97ModuleCRCFast()
//
// Parameters:
//  lpstStream          Ptr to allocated stream to CRC
//  dwTableOffset       Offset of line table
//  dwTableSize         Number of lines
//  dwModuleOffset      Offset of module binary
//  bLittleEndian       TRUE if module is little endian
//  lpabyBuf            Buffer to use
//  nBufSize            Size of buffer
//  lpfnCB              Callback function
//  lpvCBCookie         Callback cookie
//  lpdwCRC             Ptr to DWORD to store CRC of module
//  lpdwCRCByteCount    Ptr to DWORD for CRC byte count
//
// Description:
//  Given a module stream, the function calculates a
//  32-bit CRC on the token values and literal string and numeric
//  token values.
//
//  The function attempts to optimize the calculation by grouping
//  contiguous lines and continuing the CRC on the batch of lines.
//
//  The provided buffer allows the function to optimize the
//  grouping operations.  Thus a larger buffer requires fewer
//  disk accesses.
//
//  The function assumes that the size of each line is a multiple
//  of two.  The function also assumes that each opcode has a value
//  less than 256.
//
//  Either or both of lpdwCRC or lpdwCRCByteCount may be NULL if
//  the corresponding value is not required.
//
//  If lpfnCB is not NULL then the function calls it each time
//  before continuing the CRC on the buffer.  If the callback
//  function returns FALSE, the function returns FALSE.
//
//  The function computes the CRC for both O97 and O98,
//  for Macintosh and Windows.
//
// Returns:
//  TRUE            On success
//  FALSE           On error
//
//********************************************************************

typedef struct tagO97B_CRC_STATE
{
    DWORD               dwCRC;
    DWORD               dwCRCByteCount;
    EO97B_CRC_STATE_T   eState;
    BYTE                abyOp[2];
    WORD                wOpLen;
    int                 nOpcode;
    DWORD               dwCRCN;
    DWORD               dwCRCSkipN;
    DWORD               dwN;
    BYTE                abyRevCRC[8];
} O97B_CRC_STATE_T, FAR *LPO97B_CRC_STATE;

//********************************************************************
//
// Function:
//  int O97CRCBuf()
//
// Parameters:
//  lpstCRC             Ptr to CRC state structure
//  bLittleEndian       TRUE if the module is in little endian format
//  lpabyBuf            Buffer containing bytes to CRC
//  nCount              Number of bytes in buffer
//
// Description:
//  The function continues the Office 97 module CRC on the
//  given buffer.
//
// Returns:
//  int             Number of bytes traversed
//
//********************************************************************

int O97CRCBuf
(
    LPO97B_CRC_STATE    lpstCRC,
    BOOL                bLittleEndian,
    LPBYTE              lpabyBuf,
    int                 nCount
)
{
    int                 i, j;
    O97B_CRC_STATE_T    stCRC;

    stCRC = *lpstCRC;

    i = 0;
    j = 0;
    while (i < nCount && j++ < nCount)
    {
        if (i < 0)
            return(nCount);

        switch (stCRC.eState)
        {
            case eO97B_CRC_STATE_OPCODE_LO:
                if (bLittleEndian)
                    stCRC.abyOp[0] = lpabyBuf[i++];
                else
                    stCRC.abyOp[1] = lpabyBuf[i++];

                stCRC.eState = eO97B_CRC_STATE_OPCODE_HI;
                break;

            case eO97B_CRC_STATE_OPCODE_HI:
                if (bLittleEndian)
                    stCRC.abyOp[1] = lpabyBuf[i++];
                else
                    stCRC.abyOp[0] = lpabyBuf[i++];

                stCRC.nOpcode = stCRC.abyOp[0];

                if (stCRC.nOpcode > O97_OP_MAX)
                    stCRC.nOpcode = O97_OP_MAX;

                if (gabyO97OpSkip[stCRC.nOpcode] < 0x80)
                {
                    // CRC the opcode

                    CRC32Continue(stCRC.dwCRC,stCRC.abyOp[0]);
                    CRC32Continue(stCRC.dwCRC,stCRC.abyOp[1]);
                    stCRC.dwCRCByteCount += 2;

                    switch (stCRC.nOpcode)
                    {
                        case O97_OP_CURRENCY:
                        case O97_OP_DATE:
                        case O97_OP_WORD_VALUE:
                        case O97_OP_DWORD_VALUE:
                        case O97_OP_HEX_WORD_VALUE:
                        case O97_OP_HEX_DWORD_VALUE:
                        case O97_OP_OCT_WORD_VALUE:
                        case O97_OP_OCT_DWORD_VALUE:
                        case O97_OP_SINGLE_FLOAT_VALUE:
                        case O97_OP_DOUBLE_FLOAT_VALUE:
                            stCRC.dwCRCN = gabyO97OpSkip[stCRC.nOpcode];
                            if (bLittleEndian)
                            {
                                stCRC.dwCRCSkipN = 0;
                                stCRC.eState = eO97B_CRC_STATE_CRC_N;
                            }
                            else
                            {
                                stCRC.dwN = stCRC.dwCRCN;
                                stCRC.eState = eO97B_CRC_STATE_REV_CRC_N;
                            }
                            break;

                        default:
                            // Skip over any operands

                            i += gabyO97OpSkip[stCRC.nOpcode];
                            stCRC.eState = eO97B_CRC_STATE_OPCODE_LO;
                            break;
                    }
                }
                else
                if (stCRC.nOpcode == O97_OP_VAR_DEFN)
                {
                    // Skip variable definitions

                    if (stCRC.abyOp[1] & 0x40)
                    {
                        // Add space for optional As column offset

                        i += 6;
                    }
                    else
                        i += 4;

                    stCRC.eState = eO97B_CRC_STATE_OPCODE_LO;
                }
                else
                {
                    // There are a variable number of arguments

                    if (stCRC.nOpcode == O97_OP_QUOTED_COMMENT)
                    {
                        // Skip the next two bytes

                        i += 2;
                    }
                    else
                    {
                        switch (stCRC.nOpcode)
                        {
                            case O97_OP_LINE_CONTINUATION:
                            case O97_OP_REM_COMMENT:
                                break;

                            default:
                                // CRC the opcode

                                CRC32Continue(stCRC.dwCRC,stCRC.abyOp[0]);
                                CRC32Continue(stCRC.dwCRC,stCRC.abyOp[1]);
                                stCRC.dwCRCByteCount += 2;
                                break;
                        }
                    }

                    // Then get the operand length

                    stCRC.eState = eO97B_CRC_STATE_OP_LEN_LO;
                }
                break;

            case eO97B_CRC_STATE_OP_LEN_LO:
                stCRC.wOpLen = lpabyBuf[i++];
                stCRC.eState = eO97B_CRC_STATE_OP_LEN_HI;
                break;

            case eO97B_CRC_STATE_OP_LEN_HI:
                if (bLittleEndian)
                    stCRC.wOpLen |= (WORD)lpabyBuf[i++] << 8;
                else
                    stCRC.wOpLen = (stCRC.wOpLen << 8) |
                        ((WORD)lpabyBuf[i++]);

                // Round up to next even number

                stCRC.dwCRCN = stCRC.wOpLen;
                if ((stCRC.wOpLen & 1) == 0)
                    stCRC.dwCRCSkipN = 0;
                else
                    stCRC.dwCRCSkipN = 1;

                switch (stCRC.nOpcode)
                {
                    case O97_OP_LIT_STR:
                        if (stCRC.dwCRCN == 0)
                            stCRC.eState = eO97B_CRC_STATE_OPCODE_LO;
                        else
                            stCRC.eState = eO97B_CRC_STATE_CRC_N;
                        break;

                    default:
                        i += (int)(stCRC.dwCRCN + stCRC.dwCRCSkipN);
                        stCRC.eState = eO97B_CRC_STATE_OPCODE_LO;
                        break;
                }

                break;

            case eO97B_CRC_STATE_CRC_N:
                if (nCount - i >= stCRC.dwCRCN)
                    stCRC.dwN = stCRC.dwCRCN;
                else
                    stCRC.dwN = nCount - i;

                stCRC.dwCRC = CRC32Compute(stCRC.dwN,
                                           lpabyBuf+i,
                                           stCRC.dwCRC);
                stCRC.dwCRCByteCount += stCRC.dwN;
                i += (int)stCRC.dwN;
                stCRC.dwCRCN -= stCRC.dwN;

                if (stCRC.dwCRCN == 0)
                {
                    i += (int)stCRC.dwCRCSkipN;
                    stCRC.eState = eO97B_CRC_STATE_OPCODE_LO;
                }

                break;

            case eO97B_CRC_STATE_REV_CRC_N:
                while (i < nCount)
                {
                    stCRC.abyRevCRC[--stCRC.dwN] = lpabyBuf[i++];
                    if (stCRC.dwN == 0)
                        break;
                }

                if (stCRC.dwN == 0)
                {
                    stCRC.dwCRC = CRC32Compute(stCRC.dwCRCN,
                                               stCRC.abyRevCRC,
                                               stCRC.dwCRC);

                    stCRC.dwCRCByteCount += stCRC.dwCRCN;

                    stCRC.eState = eO97B_CRC_STATE_OPCODE_LO;
                }
                break;

            default:
                // This is an error condition
                return(nCount);
        }
    }

    *lpstCRC = stCRC;

    return(i);
}

//********************************************************************
//
// Function:
//  int O98CRCBuf()
//
// Parameters:
//  lpstCRC             Ptr to CRC state structure
//  bLittleEndian       TRUE if the module is in little endian format
//  lpabyBuf            Buffer containing bytes to CRC
//  nCount              Number of bytes in buffer
//
// Description:
//  The function continues the Office 98 module CRC on the
//  given buffer.
//
// Returns:
//  int             Number of bytes traversed
//
//********************************************************************

int O98CRCBuf
(
    LPO97B_CRC_STATE    lpstCRC,
    BOOL                bLittleEndian,
    LPBYTE              lpabyBuf,
    int                 nCount
)
{
    int                 i, j;
    O97B_CRC_STATE_T    stCRC;

    stCRC = *lpstCRC;

    i = 0;
    j = 0;
    while (i < nCount && j++ < nCount)
    {
        if (i < 0)
            return(nCount);

        switch (stCRC.eState)
        {
            case eO97B_CRC_STATE_OPCODE_LO:
                if (bLittleEndian)
                    stCRC.abyOp[0] = lpabyBuf[i++];
                else
                    stCRC.abyOp[1] = lpabyBuf[i++];

                stCRC.eState = eO97B_CRC_STATE_OPCODE_HI;
                break;

            case eO97B_CRC_STATE_OPCODE_HI:
                if (bLittleEndian)
                    stCRC.abyOp[1] = lpabyBuf[i++];
                else
                    stCRC.abyOp[0] = lpabyBuf[i++];

                stCRC.nOpcode = stCRC.abyOp[0] | ((stCRC.abyOp[1] & 0x01) << 8);

                if (stCRC.nOpcode > O98_OP_MAX)
                    stCRC.nOpcode = O98_OP_MAX;

                if (gabyO98OpSkip[stCRC.nOpcode] < 0x80)
                {
                    // CRC the opcode

                    CRC32Continue(stCRC.dwCRC,stCRC.abyOp[0]);
                    CRC32Continue(stCRC.dwCRC,stCRC.abyOp[1]);
                    stCRC.dwCRCByteCount += 2;

                    switch (stCRC.nOpcode)
                    {
                        case O98_OP_CURRENCY:
                        case O98_OP_DATE:
                        case O98_OP_WORD_VALUE:
                        case O98_OP_DWORD_VALUE:
                        case O98_OP_HEX_WORD_VALUE:
                        case O98_OP_HEX_DWORD_VALUE:
                        case O98_OP_OCT_WORD_VALUE:
                        case O98_OP_OCT_DWORD_VALUE:
                        case O98_OP_SINGLE_FLOAT_VALUE:
                        case O98_OP_DOUBLE_FLOAT_VALUE:
                            stCRC.dwCRCN = gabyO98OpSkip[stCRC.nOpcode];
                            if (bLittleEndian)
                            {
                                stCRC.dwCRCSkipN = 0;
                                stCRC.eState = eO97B_CRC_STATE_CRC_N;
                            }
                            else
                            {
                                stCRC.dwN = stCRC.dwCRCN;
                                stCRC.eState = eO97B_CRC_STATE_REV_CRC_N;
                            }
                            break;

                        default:
                            // Skip over any operands

                            i += gabyO98OpSkip[stCRC.nOpcode];
                            stCRC.eState = eO97B_CRC_STATE_OPCODE_LO;
                            break;
                    }
                }
                else
                if (stCRC.nOpcode == O98_OP_VAR_DEFN)
                {
                    // Skip variable definitions

                    if (stCRC.abyOp[1] & 0x40)
                    {
                        // Add space for optional As column offset

                        i += 6;
                    }
                    else
                        i += 4;

                    stCRC.eState = eO97B_CRC_STATE_OPCODE_LO;
                }
                else
                {
                    // There are a variable number of arguments

                    if (stCRC.nOpcode == O98_OP_QUOTED_COMMENT)
                    {
                        // Skip the next two bytes

                        i += 2;
                    }
                    else
                    {
                        switch (stCRC.nOpcode)
                        {
                            case O98_OP_LINE_CONTINUATION:
                            case O98_OP_REM_COMMENT:
                                break;

                            default:
                                // CRC the opcode

                                CRC32Continue(stCRC.dwCRC,stCRC.abyOp[0]);
                                CRC32Continue(stCRC.dwCRC,stCRC.abyOp[1]);
                                stCRC.dwCRCByteCount += 2;
                                break;
                        }
                    }

                    // Then get the operand length

                    stCRC.eState = eO97B_CRC_STATE_OP_LEN_LO;
                }
                break;

            case eO97B_CRC_STATE_OP_LEN_LO:
                stCRC.wOpLen = lpabyBuf[i++];
                stCRC.eState = eO97B_CRC_STATE_OP_LEN_HI;
                break;

            case eO97B_CRC_STATE_OP_LEN_HI:
                if (bLittleEndian)
                    stCRC.wOpLen |= (WORD)lpabyBuf[i++] << 8;
                else
                    stCRC.wOpLen = (stCRC.wOpLen << 8) |
                        ((WORD)lpabyBuf[i++]);

                // Round up to next even number

                stCRC.dwCRCN = stCRC.wOpLen;
                if ((stCRC.wOpLen & 1) == 0)
                    stCRC.dwCRCSkipN = 0;
                else
                    stCRC.dwCRCSkipN = 1;

                switch (stCRC.nOpcode)
                {
                    case O98_OP_LIT_STR:
                        if (stCRC.dwCRCN == 0)
                            stCRC.eState = eO97B_CRC_STATE_OPCODE_LO;
                        else
                            stCRC.eState = eO97B_CRC_STATE_CRC_N;
                        break;

                    default:
                        i += (int)(stCRC.dwCRCN + stCRC.dwCRCSkipN);
                        stCRC.eState = eO97B_CRC_STATE_OPCODE_LO;
                        break;
                }

                break;

            case eO97B_CRC_STATE_CRC_N:
                if (nCount - i >= stCRC.dwCRCN)
                    stCRC.dwN = stCRC.dwCRCN;
                else
                    stCRC.dwN = nCount - i;

                stCRC.dwCRC = CRC32Compute(stCRC.dwN,
                                           lpabyBuf+i,
                                           stCRC.dwCRC);
                stCRC.dwCRCByteCount += stCRC.dwN;
                i += (int)stCRC.dwN;
                stCRC.dwCRCN -= stCRC.dwN;

                if (stCRC.dwCRCN == 0)
                {
                    i += (int)stCRC.dwCRCSkipN;
                    stCRC.eState = eO97B_CRC_STATE_OPCODE_LO;
                }

                break;

            case eO97B_CRC_STATE_REV_CRC_N:
                while (i < nCount)
                {
                    stCRC.abyRevCRC[--stCRC.dwN] = lpabyBuf[i++];
                    if (stCRC.dwN == 0)
                        break;
                }

                if (stCRC.dwN == 0)
                {
                    stCRC.dwCRC = CRC32Compute(stCRC.dwCRCN,
                                               stCRC.abyRevCRC,
                                               stCRC.dwCRC);

                    stCRC.dwCRCByteCount += stCRC.dwCRCN;

                    stCRC.eState = eO97B_CRC_STATE_OPCODE_LO;
                }
                break;

            default:
                // This is an error condition
                return(nCount);
        }
    }

    *lpstCRC = stCRC;

    return(i);
}

#define O97_CRC_MAX_GROUP_COUNT     128

BOOL O97ModuleCRCFast
(
    LPSS_STREAM             lpstStream,
    DWORD                   dwTableOffset,
    DWORD                   dwTableSize,
    DWORD                   dwModuleOffset,
    LPBYTE                  lpabyBuf,
    int                     nBufSize,
    LPFNO97_CRC_FAST_CB     lpfnCB,
    LPVOID                  lpvCBCookie,
    LPDWORD                 lpdwCRC,
    LPDWORD                 lpdwCRCByteCount
)
{
    LPO97_LINE_TABLE_LINE   lpstLine;
    int                     nMaxBufLines;
    WORD                    wSize;

    int                     i;
    int                     nNumGroups;
    DWORD                   adwOffsets[O97_CRC_MAX_GROUP_COUNT];
    DWORD                   adwSizes[O97_CRC_MAX_GROUP_COUNT];

    DWORD                   dwLine;
    DWORD                   dwCount;
    DWORD                   dwOffset;
    DWORD                   dwStartOffset;
    DWORD                   dwEndOffset;
    int                     nStartI;

    DWORD                   dwGroupOffset;
    DWORD                   dwGroupSize;

    DWORD                   dwBytesRead;

    O97B_CRC_STATE_T        stCRC;
    EO97B_CRC_STATE_T       eState;

    int                     nVersion;
    BOOL                    bLittleEndian;

    // Get the version and endian state

    if (O97GetModuleVersionEndian(lpstStream,
                                  &nVersion,
                                  &bLittleEndian) == FALSE)
        return(FALSE);

    nMaxBufLines = nBufSize / sizeof(O97_LINE_TABLE_LINE_T);
    if (nMaxBufLines == 0 || lpabyBuf == NULL)
        return(FALSE);

    CRC32Init(stCRC.dwCRC);
    stCRC.dwCRCByteCount = 0;
    dwLine = 0;
    while (dwLine < dwTableSize)
    {
        /////////////////////////////////////////////////////////
        // Get up to 128 groups of contiguous lines

        nNumGroups = 0;
        dwEndOffset = 0;
        dwCount = 0;
        while (dwLine < dwTableSize)
        {
            // Need to rebuffer?

            if (dwCount == 0)
            {
                // Read as many line entries as possible

                if (dwLine + nMaxBufLines > dwTableSize)
                    dwCount = dwTableSize - dwLine;
                else
                    dwCount = nMaxBufLines;

                // Read the line entries

                if (SSSeekRead(lpstStream,
                               dwTableOffset +
                                   dwLine * sizeof(O97_LINE_TABLE_LINE_T),
                               lpabyBuf,
                               dwCount * sizeof(O97_LINE_TABLE_LINE_T),
                               &dwBytesRead) != SS_STATUS_OK ||
                    dwBytesRead != dwCount * sizeof(O97_LINE_TABLE_LINE_T))
                    return(FALSE);

                lpstLine = (LPO97_LINE_TABLE_LINE)lpabyBuf;
            }

            wSize = SSEndianWORD((LPBYTE)&lpstLine->wSize,
                                 bLittleEndian);

            // Ignore blank lines

            if (wSize != 0)
            {
                dwOffset = SSEndianDWORD((LPBYTE)&lpstLine->dwOffset,
                                         bLittleEndian);
                if (dwEndOffset == 0)
                {
                    // This is the first line in the group

                    dwStartOffset = dwOffset;
                    dwEndOffset = dwOffset + wSize;
                }
                else
                if (dwEndOffset == dwOffset)
                {
                    // The line is contiguous with the previous line

                    dwEndOffset += wSize;
                }
                else
                {
                    // This line is not contiguous with the previous line

                    // Store the offset and size of the group

                    adwOffsets[nNumGroups] = dwStartOffset;
                    adwSizes[nNumGroups] = dwEndOffset - dwStartOffset;

                    dwEndOffset = 0;

                    // Have the maximum number of groups been reached

                    if (++nNumGroups == O97_CRC_MAX_GROUP_COUNT)
                        break;

                    // Start a new group

                    dwStartOffset = dwOffset;
                    dwEndOffset = dwOffset + wSize;
                }
            }

            // Move to the next line

            ++dwLine;
            --dwCount;
            ++lpstLine;
        }

        // Store the hanging group

        if (dwEndOffset != 0)
        {
            adwOffsets[nNumGroups] = dwStartOffset;
            adwSizes[nNumGroups] = dwEndOffset - dwStartOffset;
            ++nNumGroups;
        }

        /////////////////////////////////////////////////////////
        // CRC the groups of contiguous lines

        i = 0;
        while (i < nNumGroups)
        {
            // Get a set of groups that fit within the buffer

            nStartI = i;
            dwOffset = adwOffsets[i];
            dwEndOffset = dwOffset + adwSizes[i];
            dwCount = adwSizes[i];
            ++i;
            while (i < nNumGroups)
            {
                dwGroupOffset = adwOffsets[i];
                dwGroupSize = adwSizes[i];
                if (dwGroupOffset < dwOffset)
                {
                    if (dwGroupOffset + dwGroupSize > dwEndOffset)
                    {
                        // This is weird

                        break;
                    }

                    // Will the buffer limit be exceeded?

                    if ((int)(dwEndOffset - dwGroupOffset) > nBufSize)
                        break;

                    // Set the start offset to this line

                    dwOffset = dwGroupOffset;
                }
                else
                if (dwGroupOffset + dwGroupSize > dwEndOffset)
                {
                    // Will adding this group exceed the buffer size?

                    if (dwGroupOffset + dwGroupSize - dwOffset > nBufSize)
                        break;

                    dwEndOffset = dwGroupOffset + dwGroupSize;
                }

                ++i;
            }

            if (nStartI + 1 == i)
            {
                // There is only one group

                stCRC.eState = eState = eO97B_CRC_STATE_OPCODE_LO;
                dwOffset += dwModuleOffset;
                dwEndOffset += dwModuleOffset;
                dwCount = nBufSize;
                while (dwOffset < dwEndOffset)
                {
                    if (dwEndOffset - dwOffset < dwCount)
                        dwCount = dwEndOffset - dwOffset;

                    // Read a chunk

                    if (SSSeekRead(lpstStream,
                                   dwOffset,
                                   lpabyBuf,
                                   dwCount,
                                   &dwBytesRead) != SS_STATUS_OK ||
                        dwBytesRead != dwCount)
                        return(FALSE);

                    // Call the callback if necessary

                    if (lpfnCB != NULL)
                    {
                        if (lpfnCB(lpvCBCookie,
                                   &eState,
                                   nVersion,
                                   bLittleEndian,
                                   lpabyBuf,
                                   (int)dwCount) == FALSE)
                            return(FALSE);
                    }

                    // CRC the chunk and increment the offset

                    if (nVersion == O97_VERSION_O97)
                    {
                        dwOffset += O97CRCBuf(&stCRC,
                                              bLittleEndian,
                                              lpabyBuf,
                                              (int)dwCount);
                    }
                    else
                    {
                        dwOffset += O98CRCBuf(&stCRC,
                                              bLittleEndian,
                                              lpabyBuf,
                                              (int)dwCount);
                    }
                }
            }
            else
            {
                // There are multiple groups.  All groups
                //  fit within the buffer

                dwCount = dwEndOffset - dwOffset;

                // Make sure we are legal

                if (dwCount > nBufSize)
                    return(FALSE);

                // Read a chunk

                if (SSSeekRead(lpstStream,
                               dwModuleOffset + dwOffset,
                               lpabyBuf,
                               dwCount,
                               &dwBytesRead) != SS_STATUS_OK ||
                    dwBytesRead != dwCount)
                    return(FALSE);

                // CRC each group

                while (nStartI < i)
                {
                    stCRC.eState = eState = eO97B_CRC_STATE_OPCODE_LO;

                    // Verify that the chunk is within the buffer

                    if (adwOffsets[nStartI] - dwOffset >= dwCount)
                        return(FALSE);

                    if ((dwCount - (adwOffsets[nStartI] - dwOffset)) <
                        adwSizes[nStartI])
                        return(FALSE);

                    // Call the callback if necessary

                    if (lpfnCB != NULL)
                    {
                        if (lpfnCB(lpvCBCookie,
                                   &eState,
                                   nVersion,
                                   bLittleEndian,
                                   lpabyBuf + adwOffsets[nStartI] - dwOffset,
                                   (int)adwSizes[nStartI]) == FALSE)
                            return(FALSE);
                    }

                    // CRC the group

                    if (nVersion == O97_VERSION_O97)
                    {
                        O97CRCBuf(&stCRC,
                                  bLittleEndian,
                                  lpabyBuf + adwOffsets[nStartI] - dwOffset,
                                  (int)adwSizes[nStartI]);
                    }
                    else
                    {
                        O98CRCBuf(&stCRC,
                                  bLittleEndian,
                                  lpabyBuf + adwOffsets[nStartI] - dwOffset,
                                  (int)adwSizes[nStartI]);
                    }

                    ++nStartI;
                }
            }
        }
    }

    if (lpdwCRC != NULL)
        *lpdwCRC = stCRC.dwCRC;

    if (lpdwCRCByteCount != NULL)
        *lpdwCRCByteCount = stCRC.dwCRCByteCount;

    return(TRUE);
}


//********************************************************************
//
// Function:
//  BOOL O97ModAccInit()
//
// Parameters:
//  lpstMod                 Ptr to O97_MOD_ACC_T structure
//  lpstStream              Ptr to the module stream
//  lpabyBuf                Ptr to buffering buffer to use
//  dwBufSize               Size of buffering buffer
//
// Description:
//  The function initializes the O97_MOD_ACC_T structure
//  for buffered access to the line index and data.
//
// Returns:
//  TRUE                    On success
//  FALSE                   On error
//
//********************************************************************

BOOL O97ModAccInit
(
    LPO97_MOD_ACC           lpstMod,
    LPSS_STREAM             lpstStream,
    LPBYTE                  lpabyBuf,
    DWORD                   dwBufSize
)
{
    lpstMod->lpstStream = lpstStream;

    if (O97GetModuleVersionEndian(lpstStream,
                                  &lpstMod->nVersion,
                                  &lpstMod->bLittleEndian) == FALSE)
        return(FALSE);

    if (O97GetLineBinaryOffsets(lpstStream,
                                &lpstMod->dwTableOffset,
                                &lpstMod->dwTableSize,
                                &lpstMod->dwDataOffset) == FALSE)
        return(FALSE);

    lpstMod->dwDataSize = SSStreamLen(lpstStream) - lpstMod->dwDataOffset;

    // Assign line buffer

    lpstMod->dwMaxLineEntries = (dwBufSize / 2) /
        sizeof(O97_LINE_TABLE_LINE_T);
    lpstMod->lpastLines = (LPO97_LINE_TABLE_LINE)lpabyBuf;

    lpstMod->dwLineStart = 0;
    lpstMod->dwLineEnd = 0;
    lpstMod->dwLineCenter = 0;

    // Assign data buffer

    lpstMod->dwMaxDataSize = dwBufSize -
        lpstMod->dwMaxLineEntries * sizeof(O97_LINE_TABLE_LINE_T);
    lpstMod->lpabyData = (LPBYTE)
        (lpstMod->lpastLines + lpstMod->dwMaxLineEntries);

    lpstMod->dwDataStart = 0;
    lpstMod->dwDataEnd = 0;
    lpstMod->dwDataCenter = 0;

    return(TRUE);
}


//********************************************************************
//
// Function:
//  BOOL O97ModAccGetLineInfo()
//
// Parameters:
//  lpstMod                 Ptr to O97_MOD_ACC_T structure
//  dwLine                  Line to get
//  lpdwOffset              Ptr to DWORD for line offset
//  lpwSize                 Ptr to WORD for size offset
//
// Description:
//  The function returns in *lpdwOffset and *lpwSize the offset
//  and size of the given line.
//
//  If the line is outside of the buffered lines, then the buffer
//  is refilled starting with the given line.  If dwLineCenter
//  of lpstMod is non-zero, then dwLineCenter becomes the center
//  line, unless the desired line lies outside, in which case
//  the desired line becomes the starting line.  In this last
//  case, centering is left on for the next read.
//
// Returns:
//  TRUE                    On success
//  FALSE                   On error
//
//********************************************************************

BOOL O97ModAccGetLineInfo
(
    LPO97_MOD_ACC           lpstMod,
    DWORD                   dwLine,
    LPDWORD                 lpdwOffset,
    LPWORD                  lpwSize
)
{
    DWORD                   dwCount;
    DWORD                   dwBytesRead;
    LPO97_LINE_TABLE_LINE   lpstLine;

    if (dwLine < lpstMod->dwLineStart || lpstMod->dwLineEnd <= dwLine)
    {
        if (dwLine >= lpstMod->dwTableSize)
            return(FALSE);

        // Determine the starting and ending lines to read

        if (lpstMod->dwLineCenter == 0)
            lpstMod->dwLineStart = dwLine;
        else
        {
            if (lpstMod->dwLineCenter <= lpstMod->dwMaxLineEntries / 2)
                lpstMod->dwLineStart = 0;
            else
                lpstMod->dwLineStart = lpstMod->dwLineCenter -
                    lpstMod->dwMaxLineEntries / 2;

            if (dwLine < lpstMod->dwLineStart ||
                lpstMod->dwLineStart + lpstMod->dwMaxLineEntries <= dwLine)
            {
                // Just read from the starting line

                lpstMod->dwLineStart = dwLine;
            }
            else
            {
                // Turn off centering for the next read

                lpstMod->dwLineCenter = 0;
            }
        }

        lpstMod->dwLineEnd = lpstMod->dwLineStart + lpstMod->dwMaxLineEntries;
        if (lpstMod->dwLineEnd > lpstMod->dwTableSize)
            lpstMod->dwLineEnd = lpstMod->dwTableSize;

        // Read the lines

        dwCount = (lpstMod->dwLineEnd - lpstMod->dwLineStart) *
            sizeof(O97_LINE_TABLE_LINE_T);

        if (SSSeekRead(lpstMod->lpstStream,
                       lpstMod->dwTableOffset +
                           lpstMod->dwLineStart * sizeof(O97_LINE_TABLE_LINE_T),
                       lpstMod->lpastLines,
                       dwCount,
                       &dwBytesRead) != SS_STATUS_OK ||
            dwBytesRead != dwCount)
            return(FALSE);
    }

    lpstLine = lpstMod->lpastLines + (dwLine - lpstMod->dwLineStart);
    *lpwSize = SSEndianWORD((LPBYTE)&lpstLine->wSize,
                            lpstMod->bLittleEndian);
    *lpdwOffset = SSEndianDWORD((LPBYTE)&lpstLine->dwOffset,
                                lpstMod->bLittleEndian);
    return(TRUE);
}


//********************************************************************
//
// Function:
//  LPBYTE O97ModAccGetDataPtr()
//
// Parameters:
//  lpstMod                 Ptr to O97_MOD_ACC_T structure
//  wOffset                 Offset of data to get
//  wSize                   Size of data needed
//  lpwSize                 Ptr to WORD for size offset
//
// Description:
//  The function returns in *lpwSize the size of the data at
//  the returned ptr.
//
//  If the data is outside of the buffered data, then the buffer
//  is refilled starting with the given data.  If dwDataCenter
//  of lpstMod is non-zero, then dwDataCenter becomes the center
//  data, unless the desired data lies outside, in which case
//  the desired data becomes the starting data.  In this last
//  case, centering is left on for the next read.
//
// Returns:
//  LPBYTE          Ptr to buffer containing data
//  NULL            On error
//
//********************************************************************

LPBYTE O97ModAccGetDataPtr
(
    LPO97_MOD_ACC           lpstMod,
    DWORD                   dwOffset,
    WORD                    wSize,
    LPWORD                  lpwPtrSize
)
{
    DWORD                   dwCount;
    DWORD                   dwBytesRead;

    // Verify that the offset is within bounds

    if (dwOffset >= lpstMod->dwDataSize)
        return(NULL);

    if (dwOffset < lpstMod->dwDataStart ||
        lpstMod->dwDataEnd < ((DWORD)dwOffset + wSize))
    {
        // Determine the starting and ending offsets

        if (lpstMod->dwDataCenter == 0)
            lpstMod->dwDataStart = dwOffset;
        else
        {
            if (lpstMod->dwDataCenter <= lpstMod->dwMaxDataSize / 2)
                lpstMod->dwDataStart = 0;
            else
                lpstMod->dwDataStart = lpstMod->dwDataCenter -
                    lpstMod->dwMaxDataSize / 2;

            if (dwOffset < lpstMod->dwDataStart ||
                lpstMod->dwDataStart + lpstMod->dwMaxDataSize <
                ((DWORD)dwOffset + wSize))
            {
                // Just read from the starting offset

                lpstMod->dwDataStart = dwOffset;
            }
            else
            {
                // Turn off centering for the next read

                lpstMod->dwDataCenter = 0;
            }
        }

        lpstMod->dwDataEnd = lpstMod->dwDataStart + lpstMod->dwMaxDataSize;
        if (lpstMod->dwDataEnd > lpstMod->dwDataSize)
            lpstMod->dwDataEnd = lpstMod->dwDataSize;

        // Read the data

        dwCount = lpstMod->dwDataEnd - lpstMod->dwDataStart;

        if (SSSeekRead(lpstMod->lpstStream,
                       lpstMod->dwDataOffset + lpstMod->dwDataStart,
                       lpstMod->lpabyData,
                       dwCount,
                       &dwBytesRead) != SS_STATUS_OK ||
            dwBytesRead != dwCount)
            return(NULL);
    }

    if (lpstMod->dwDataStart + wSize <= lpstMod->dwDataEnd)
        *lpwPtrSize = wSize;
    else
        *lpwPtrSize = (WORD)(lpstMod->dwDataEnd - lpstMod->dwDataStart);

    // Return the pointer to the data

    return(lpstMod->lpabyData + (dwOffset - lpstMod->dwDataStart));
}


//********************************************************************
//
// Function:
//  BOOL O97ModReplaceVBASrc()
//
// Parameters:
//  lpstStream          Ptr to stream to replace
//  lpstLZNT            Ptr to LZNT structure
//  lpabySrc            Ptr to LZNT source
//  dwSrcSize           Size of source
//  lpbNoRoom           Ptr to BOOL for no room result
//
// Description:
//  Replaces the compressed source of the given stream with
//  the given source.  The remainder of the stream is zeroed out.
//  The stack-based binary is also zeroed out.
//
//  If the compressed source of the given stream is smaller than
//  the given source, the function returns TRUE and the value
//  TRUE in *lpbNoRoom.
//
// Returns:
//  TRUE                If replacement was successful
//  FALSE               On error
//
//********************************************************************

BOOL O97ModReplaceVBASrc
(
    LPSS_STREAM         lpstStream,
    LPLZNT              lpstLZNT,
    LPBYTE              lpabySrc,
    DWORD               dwSrcSize,
    LPBOOL              lpbNoRoom
)
{
    int                 nVersion;
    BOOL                bLitEnd;

    DWORD               dwLZNTOffset;
    DWORD               dwStreamLen;
    DWORD               dwZeroesToWrite;
    DWORD               dwBytesToWrite;
    DWORD               dwCount;

    DWORD               dwLineTableOffset;
    DWORD               dwNumLines;
    DWORD               dwBinaryOffset;

    // Determine module type

    if (O97GetModuleVersionEndian(lpstStream,
                                  &nVersion,
                                  &bLitEnd) == FALSE)
        return(FALSE);

    if (VBA5LZNTModuleInit(lpstStream,
                           lpstLZNT) == FALSE)
        return(FALSE);

    dwLZNTOffset = lpstLZNT->dwFirstChunkOffset - 1;

    dwStreamLen = SSStreamLen(lpstStream);
    if (dwLZNTOffset >= dwStreamLen)
        return(FALSE);

    // Determine whether there is enough room for the new source

    dwBytesToWrite = dwStreamLen - dwLZNTOffset;
    if (dwBytesToWrite < dwSrcSize)
    {
        // Report too small

        *lpbNoRoom = TRUE;
        return(TRUE);
    }

    // Not too small

    *lpbNoRoom = FALSE;

    dwZeroesToWrite = dwBytesToWrite - dwSrcSize;

    /////////////////////////////////////////////////////////////
    // Replace the module source

    if (SSSeekWrite(lpstStream,
                    dwLZNTOffset,
                    lpabySrc,
                    dwSrcSize,
                    &dwCount) != SS_STATUS_OK ||
        dwCount != dwSrcSize)
    {
        // Wrote less than the desired number of bytes

        return(FALSE);
    }

    // Zero out the remainder of the stream

    if (SSWriteZeroes(lpstStream,
                      dwLZNTOffset + dwSrcSize,
                      dwZeroesToWrite) != SS_STATUS_OK)
    {
        // Failed to zero out rest of stream

        return(FALSE);
    }

    /////////////////////////////////////////////////////////////
    // Zero out the stack-based code

    if (O97GetLineBinaryOffsets(lpstStream,
                                &dwLineTableOffset,
                                &dwNumLines,
                                &dwBinaryOffset) == FALSE)
        return(FALSE);

    // Validate the binary offset

    if (dwBinaryOffset > dwLZNTOffset || dwBinaryOffset < 10)
        return(FALSE);

    // Get the size of the stack based code

    if (SSSeekRead(lpstStream,
                   dwBinaryOffset - 4,
                   &dwZeroesToWrite,
                   sizeof(DWORD),
                   &dwCount) != SS_STATUS_OK ||
        dwCount != sizeof(DWORD))
        return(FALSE);

    dwZeroesToWrite = SSEndianDWORD((LPBYTE)&dwZeroesToWrite,bLitEnd);

    // Validate the size

    if (dwZeroesToWrite >= dwStreamLen ||
        dwBinaryOffset + dwZeroesToWrite > dwLZNTOffset)
        return(FALSE);

    // Zero out the stack-based code

    if (SSWriteZeroes(lpstStream,
                      dwBinaryOffset,
                      dwZeroesToWrite) != SS_STATUS_OK)
    {
        // Failed to zero out rest of stream

        return(FALSE);
    }

    // Calculate what the new size should be

    if (dwLZNTOffset + dwSrcSize < 4096)
    {
        if (dwStreamLen >= 4096)
            dwStreamLen = 4096;
        else
            dwStreamLen = dwLZNTOffset + dwSrcSize;
    }
    else
        dwStreamLen = dwLZNTOffset + dwSrcSize;

    // Truncate the stream

    if (SSSetStreamLen(lpstStream->lpstRoot,
                       SSStreamID(lpstStream),
                       dwStreamLen) != SS_STATUS_OK)
        return(FALSE);

    return(TRUE);
}

