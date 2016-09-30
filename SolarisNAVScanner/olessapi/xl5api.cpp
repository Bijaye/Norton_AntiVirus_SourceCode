//************************************************************************
//
// $Header:   S:/OLESSAPI/VCS/xl5api.cpv   1.27   08 Dec 1998 12:43:08   DCHI  $
//
// Description:
//  Source file for Excel 5.0/7.0/95 document access API
//
//************************************************************************
// $Log:   S:/OLESSAPI/VCS/xl5api.cpv  $
// 
//    Rev 1.27   08 Dec 1998 12:43:08   DCHI
// Fixed XL5GetIdentInfo() to correctly parse file references.
// 
//    Rev 1.26   09 Nov 1998 13:42:28   DCHI
// Added #pragma data_seg("FAR_DATA_0") for global FAR data.
// 
//    Rev 1.25   22 Sep 1998 13:47:52   DCHI
// Added XL5DecryptData() implementation.
// 
//    Rev 1.24   04 Aug 1998 13:44:28   DCHI
// Corrected operand byte count for opcode VAR_DEFN in XL5CRCBuf().
// 
//    Rev 1.23   15 Jul 1998 12:01:24   DCHI
// Modified ModAccGetDataPtr() to check offset within range first.
// 
//    Rev 1.22   10 Jul 1998 17:46:16   DCHI
// Added legal count verification in CRCFast().
// 
//    Rev 1.21   08 Jul 1998 15:59:40   DCHI
// Added sanity checks in CRCBuf() and CRCFast().
// 
//    Rev 1.20   08 Jul 1998 12:35:02   DCHI
// Minor mods.
// 
//    Rev 1.19   24 Jun 1998 11:05:34   DCHI
// - Added additional opcode definitions.
// - Added implementations for:
//     XL5GetOpSkip()
//     XL5ModAccInit()
//     XL5ModAccGetLineInfo()
//     XL5ModAccGetDataPtr()
// - Corrected clogomeration bug in XL5ModuleCRCFast()
// 
//    Rev 1.18   11 Jun 1998 19:24:02   DCHI
// Corrected buffer and size parameters in CRCFast() callback call.
// 
//    Rev 1.17   09 Jun 1998 11:30:46   DCHI
// Added extra callback params to XL5ModuleCRCFast().
// 
//    Rev 1.16   02 Jun 1998 13:23:56   DCHI
// Added XL5ModuleCRCFast().
// 
//    Rev 1.15   04 May 1998 11:53:40   DCHI
// Added XL5WriteBlankSheet().
// 
//    Rev 1.14   09 Apr 1998 13:24:10   DCHI
// For sheet record iterator functions, added BOF/EOF depth checking.
// 
//    Rev 1.13   07 Apr 1998 15:30:02   DCHI
// Added module type table and identifier retrieval functions.
// 
//    Rev 1.12   03 Apr 1998 18:15:20   DCHI
// Modified XL5GetMacroLineInfo() to correctly check NULL lpdwMacroSize param.
// 
//    Rev 1.11   03 Apr 1998 10:32:34   DCHI
// Modified XL5ModuleCRC() to also CRC high byte of opcode as well as
// additional values: currency, date, hex WORD and DWORD,
// octal WORD and DWORD, and singles.
// 
//    Rev 1.10   30 Mar 1998 18:50:16   DCHI
// Minor correction to XL5AssignRndSheetName().
// 
//    Rev 1.9   30 Mar 1998 14:53:38   DCHI
// Modified XLBlankAllCellValues() to leave STRING records as is, except
// for zeroing out the contents.
// 
//    Rev 1.8   19 Mar 1998 10:52:02   DCHI
// Modified XL5FindKey() to assume FILEPASS comes before WRITEACCESS.
// 
//    Rev 1.7   12 Mar 1998 10:42:54   DCHI
// Various additional functionality for new Excel engine.
// 
//    Rev 1.6   11 Feb 1998 16:18:06   DCHI
// Added XL5GetMacroLineInfo(), XL5GetIdentInfo(), XL5CRC32Expr(),
// and XL5ModuleCRC().  Modified XL5DirInfoInit() to use XL5GetIdentInfo().
// 
//    Rev 1.5   26 Jan 1998 17:25:04   DCHI
// Endianzed wLineRecordCount in XL5GetMacroOffsetSize().
// 
//    Rev 1.4   05 Dec 1997 13:12:30   DCHI
// Added XL5FindKey() and XL5CalculatePassword().
// 
//    Rev 1.3   12 Nov 1997 18:18:46   DCHI
// Added module/stream name association functions.
// 
//    Rev 1.2   16 Oct 1997 14:16:54   DCHI
// Added FE CA verification.
// 
//    Rev 1.1   16 Oct 1997 12:31:12   DCHI
// Change to use more efficient means for getting macro binary beginning.
// 
//    Rev 1.0   02 Oct 1997 17:30:34   DCHI
// Initial revision.
// 
//************************************************************************

#include "xl5api.h"
#include "olestrnm.h"
#include "crc32.h"

//********************************************************************
//
// Function:
//  BOOL XL5GetMacroOffsetSize()
//
// Parameters:
//  lpstStream          Ptr to Excel 5.0/7.0/95 VB module stream
//  lpdwMacroOffset     Ptr to DWORD for offset of macro binary
//  lpdwMacroSize       Ptr to DWORD for size of macro binary
//
// Description:
//  The stream is assumed to be in the following format:
//      Offset 0:
//          abyUnknown0[10]
//          dwExtra0
//          abyExtra0[dwExtra0]
//          abyUnknown1[14]
//          dwExtra1
//          abyExtra1[dwExtra1]
//          abyUnknown2[28]
//          dwLineRecOffset;
//
//      Offset dwLineRecOffset + 60:
//          0xFE 0xCA 0x00 0x00
//          wLineRecordCount
//          abyLineRecords[wLineRecCount][12]
//          abyUnknown3[4]
//          dwMacroSize
//          abyMacroBinary[dwMacroSize]
//
// Returns:
//  TRUE                On success
//  FALSE               On error
//
//********************************************************************

BOOL XL5GetMacroOffsetSize
(
    LPSS_STREAM     lpstStream,
    LPDWORD         lpdwMacroOffset,
    LPDWORD         lpdwMacroSize
)
{
    DWORD           dwBytesRead;
    DWORD           dwTemp;
    BYTE            abyBuf[2];
    DWORD           dwOffset;
    WORD            wLineRecordCount;

    dwOffset = 10;

    // Get dwExtra0

    if (SSSeekRead(lpstStream,
                   dwOffset,
                   &dwTemp,
                   sizeof(DWORD),
                   &dwBytesRead) != SS_STATUS_OK ||
        dwBytesRead != sizeof(DWORD))
        return(FALSE);

    dwTemp = DWENDIAN(dwTemp);

    dwOffset += sizeof(DWORD) + dwTemp + 14;

    // Get dwExtra1

    if (SSSeekRead(lpstStream,
                   dwOffset,
                   &dwTemp,
                   sizeof(DWORD),
                   &dwBytesRead) != SS_STATUS_OK ||
        dwBytesRead != sizeof(DWORD))
        return(FALSE);

    dwTemp = DWENDIAN(dwTemp);

    // Skip over
    //  abyExtra1[dwExtra1]
    //  abyUnknown2[28]

    dwOffset += sizeof(DWORD) + dwTemp + 28;

    // Get dwLineRecOffset

    if (SSSeekRead(lpstStream,
                   dwOffset,
                   &dwOffset,
                   sizeof(DWORD),
                   &dwBytesRead) != SS_STATUS_OK ||
        dwBytesRead != sizeof(DWORD))
        return(FALSE);

    dwOffset = DWENDIAN(dwOffset);

    // Go forward 60

    dwOffset += 60;

    // Verify 0xFE and 0xCA

    if (SSSeekRead(lpstStream,
                   dwOffset,
                   abyBuf,
                   2 * sizeof(BYTE),
                   &dwBytesRead) != SS_STATUS_OK ||
        dwBytesRead != 2 * sizeof(BYTE))
        return(FALSE);

    if (abyBuf[0] != 0xFE || abyBuf[1] != 0xCA)
        return(FALSE);

    // Skip over 0xFE 0xCA 0x00 0x00

    dwOffset += 4;

    // Get line record count

    if (SSSeekRead(lpstStream,
                   dwOffset,
                   &wLineRecordCount,
                   sizeof(WORD),
                   &dwBytesRead) != SS_STATUS_OK ||
        dwBytesRead != sizeof(WORD))
        return(FALSE);

    wLineRecordCount = WENDIAN(wLineRecordCount);

    // Skip over line record count, line records, and 0xFF 0xFF 0x01 0x01

    dwOffset += sizeof(WORD) + wLineRecordCount * (DWORD)12 + 4;

    // This is the macro offset

    *lpdwMacroOffset = dwOffset + sizeof(DWORD);

    // Get the size

    if (SSSeekRead(lpstStream,
                   dwOffset,
                   lpdwMacroSize,
                   sizeof(DWORD),
                   &dwBytesRead) != SS_STATUS_OK ||
        dwBytesRead != sizeof(DWORD))
        return(FALSE);

    *lpdwMacroSize = DWENDIAN(*lpdwMacroSize);

    // The macro offset is after the size

    *lpdwMacroOffset = dwOffset + sizeof(DWORD);

    return(TRUE);
}


//********************************************************************
//
// Function:
//  BOOL XL5GetMacroLineInfo()
//
// Parameters:
//  lpstStream          Ptr to Excel 5.0/7.0/95 VB module stream
//  lpdwTableOffset     Ptr to DWORD for offset of macro line table
//  lpdwTableSize       Ptr to DWORD for number of line entries
//  lpdwMacroOffset     Ptr to DWORD for offset of macro binary
//  lpdwMacroSize       Ptr to DWORD for size of macro binary
//
// Description:
//  The function parses through the macro module stream and
//  retrieves the line table offset and size as well as the binary
//  offset and size.
//
//  The stream is assumed to be in the following format:
//      Offset 0:
//          abyUnknown0[10]
//          dwExtra0
//          abyExtra0[dwExtra0]
//          abyUnknown1[14]
//          dwExtra1
//          abyExtra1[dwExtra1]
//          abyUnknown2[28]
//          dwLineRecOffset;
//
//      Offset dwLineRecOffset + 60:
//          0xFE 0xCA 0x00 0x00
//          wLineRecordCount
//          abyLineRecords[wLineRecCount][12]
//          abyUnknown3[4]
//          dwMacroSize
//          abyMacroBinary[dwMacroSize]
//
// Returns:
//  TRUE                On success
//  FALSE               On error
//
//********************************************************************

BOOL XL5GetMacroLineInfo
(
    LPSS_STREAM     lpstStream,
    LPDWORD         lpdwTableOffset,
    LPDWORD         lpdwTableSize,
    LPDWORD         lpdwMacroOffset,
    LPDWORD         lpdwMacroSize
)
{
    DWORD           dwBytesRead;
    DWORD           dwTemp;
    BYTE            abyBuf[2];
    DWORD           dwOffset;
    WORD            wLineRecordCount;

    dwOffset = 10;

    // Get dwExtra0

    if (SSSeekRead(lpstStream,
                   dwOffset,
                   &dwTemp,
                   sizeof(DWORD),
                   &dwBytesRead) != SS_STATUS_OK ||
        dwBytesRead != sizeof(DWORD))
        return(FALSE);

    dwTemp = DWENDIAN(dwTemp);

    dwOffset += sizeof(DWORD) + dwTemp + 14;

    // Get dwExtra1

    if (SSSeekRead(lpstStream,
                   dwOffset,
                   &dwTemp,
                   sizeof(DWORD),
                   &dwBytesRead) != SS_STATUS_OK ||
        dwBytesRead != sizeof(DWORD))
        return(FALSE);

    dwTemp = DWENDIAN(dwTemp);

    // Skip over
    //  abyExtra1[dwExtra1]
    //  abyUnknown2[28]

    dwOffset += sizeof(DWORD) + dwTemp + 28;

    // Get dwLineRecOffset

    if (SSSeekRead(lpstStream,
                   dwOffset,
                   &dwOffset,
                   sizeof(DWORD),
                   &dwBytesRead) != SS_STATUS_OK ||
        dwBytesRead != sizeof(DWORD))
        return(FALSE);

    dwOffset = DWENDIAN(dwOffset);

    // Go forward 60

    dwOffset += 60;

    // Verify 0xFE and 0xCA

    if (SSSeekRead(lpstStream,
                   dwOffset,
                   abyBuf,
                   2 * sizeof(BYTE),
                   &dwBytesRead) != SS_STATUS_OK ||
        dwBytesRead != 2 * sizeof(BYTE))
        return(FALSE);

    if (abyBuf[0] != 0xFE || abyBuf[1] != 0xCA)
        return(FALSE);

    // Skip over 0xFE 0xCA 0x00 0x00

    dwOffset += 4;

    // Get line record count

    if (SSSeekRead(lpstStream,
                   dwOffset,
                   &wLineRecordCount,
                   sizeof(WORD),
                   &dwBytesRead) != SS_STATUS_OK ||
        dwBytesRead != sizeof(WORD))
        return(FALSE);

    wLineRecordCount = WENDIAN(wLineRecordCount);

    if (lpdwTableSize != NULL)
        *lpdwTableSize = wLineRecordCount;

    if (lpdwTableOffset != NULL)
        *lpdwTableOffset = dwOffset + sizeof(WORD);

    // Skip over line record count, line records, and 0xFF 0xFF 0x01 0x01

    dwOffset += sizeof(WORD) + wLineRecordCount * (DWORD)12 + 4;

    if (lpdwMacroOffset != NULL)
        *lpdwMacroOffset = dwOffset + sizeof(DWORD);

    // Get the size

    if (lpdwMacroSize != NULL)
    {
        if (SSSeekRead(lpstStream,
                       dwOffset,
                       lpdwMacroSize,
                       sizeof(DWORD),
                       &dwBytesRead) != SS_STATUS_OK ||
            dwBytesRead != sizeof(DWORD))
            return(FALSE);

        *lpdwMacroSize = DWENDIAN(*lpdwMacroSize);
    }

    // The macro offset is after the size

    return(TRUE);
}


//********************************************************************
//
// Function:
//  BOOL XL5GetTypeTableInfo()
//
// Parameters:
//  lpstStream              Ptr to Excel 5.0/7.0/95 VB module stream
//  lpdwDeclareTableOffset  Ptr to DWORD for declare table offset
//  lpdwDeclareTableSize    Ptr to DWORD for declare table size
//  lpdwTypeTableOffset     Ptr to DWORD for offset of type table
//  lpdwTypeTableSize       Ptr to DWORD for sizeof type table
//
// Description:
//  The function parses through the macro module stream and
//  retrieves the declaration table offset and size and the
//  type table offset and size.
//
//  The stream is assumed to be in the following format:
//      Offset 0:
//          abyUnknown0[10]
//          dwDeclareTableSize
//          abyDeclareTable[dwDeclareTableSize]
//          abyUnknown1[14]
//          dwExtra1
//          abyExtra1[dwExtra1]
//          abyUnknown2[8]
//          dwTypeTableOffsetM6;
//
//      Offset dwTypeTableOffsetM6 + 6:
//          dwTypeTableSize
//          abyTypeTable[dwTypeTableSize]
//
// Returns:
//  TRUE                On success
//  FALSE               On error
//
//********************************************************************

BOOL XL5GetTypeTableInfo
(
    LPSS_STREAM     lpstStream,
    LPDWORD         lpdwDeclareTableOffset,
    LPDWORD         lpdwDeclareTableSize,
    LPDWORD         lpdwTypeTableOffset,
    LPDWORD         lpdwTypeTableSize
)
{
    DWORD           dwBytesRead;
    DWORD           dwTemp;
    DWORD           dwOffset;

    dwOffset = 10;

    // Get dwExtra0

    if (SSSeekRead(lpstStream,
                   dwOffset,
                   &dwTemp,
                   sizeof(DWORD),
                   &dwBytesRead) != SS_STATUS_OK ||
        dwBytesRead != sizeof(DWORD))
        return(FALSE);

    dwTemp = DWENDIAN(dwTemp);

    dwOffset += sizeof(DWORD);

    if (lpdwDeclareTableSize != NULL)
        *lpdwDeclareTableSize = dwTemp;

    if (lpdwDeclareTableOffset != NULL)
        *lpdwDeclareTableOffset = dwOffset;

    dwOffset += dwTemp + 14;

    // Get dwExtra1

    if (SSSeekRead(lpstStream,
                   dwOffset,
                   &dwTemp,
                   sizeof(DWORD),
                   &dwBytesRead) != SS_STATUS_OK ||
        dwBytesRead != sizeof(DWORD))
        return(FALSE);

    dwTemp = DWENDIAN(dwTemp);

    // Skip over
    //  abyExtra1[dwExtra1]
    //  abyUnknown2[8]

    dwOffset += sizeof(DWORD) + dwTemp + 8;

    // Get dwTypeTableOffsetM6

    if (SSSeekRead(lpstStream,
                   dwOffset,
                   &dwOffset,
                   sizeof(DWORD),
                   &dwBytesRead) != SS_STATUS_OK ||
        dwBytesRead != sizeof(DWORD))
        return(FALSE);

    dwOffset = DWENDIAN(dwOffset);

    // Go forward 6

    dwOffset += 6;

    // Get the size

    if (lpdwTypeTableSize != NULL)
    {
        if (SSSeekRead(lpstStream,
                       dwOffset,
                       lpdwTypeTableSize,
                       sizeof(DWORD),
                       &dwBytesRead) != SS_STATUS_OK ||
            dwBytesRead != sizeof(DWORD))
            return(FALSE);

        *lpdwTypeTableSize = DWENDIAN(*lpdwTypeTableSize);
    }

    // The macro offset is after the size

    if (lpdwTypeTableOffset != NULL)
        *lpdwTypeTableOffset = dwOffset + sizeof(DWORD);

    return(TRUE);
}


//********************************************************************
//
// Function:
//  BOOL XL5GetIdentInfo()
//
// Parameters:
//  lpstStream              Ptr to dir stream
//  lpwNumModules           Ptr to WORD for number of modules
//  lpdwModuleIndexOffset   Ptr to DWORD for module index offset
//  lpdwTableSize           Ptr to DWORD for identifier table size
//  lpdwTableOffset         Ptr to DWORD for identifier table offset
//  lpdwDataSize            Ptr to DWORD for identifier array size
//  lpdwDataOffset          Ptr to DWORD for identifier array offset
//
// Description:
//  The function fills in the given values for the identifier table
//  offset and size and the identifier array offset and size.
//
//  The dir stream is assumed to be in the following format:
//
//      abyUnknown0[10]
//      wCount0
//      wExtra0
//      {
//          wExtra1
//          abyExtra1[wExtra1]
//          if (abyExtra1[0..2] == '*\C')
//          {
//              wExtra1A
//              abyExtra1A[wExtra1A]
//          }
//          abyExtra1B[6]
//      }[wCount0]
//      wExtra2
//      abyExtra2[wExtra2 * 4]
//      abyUnknown1[10]
//      wExtra3
//      if (wExtra3 != 0xFFFF)
//          abyExtra3[wExtra3]
//      abyUnknown2[84];
//      FF FF
//      wCount1
//      {
//          wNameUsedLen
//          abyNameUsed[wName0Len]      // Stream name of module
//          wNameUnusedLen
//          abyNameUnused[wName1Len]
//          abyUnknown3[5] = {03 00 2A 44 01}
//          wIdentifierRecordOffset
//          abyUnknown4[10] = {FF FF FF FF 00 00 00 00 FF FF}
//      }[wCount1]
//      abyUnknown5[4] = {FF FF 01 01}
//      wExtra4
//      abyExtra4[wExtra4]
//      abyUnknown6[8] = {FF FF 00 00 FF FF 01 00}
//      dwIdentRecordLen
//      abyIdentRecords[dwIdentRecordLen]
//      abyUnknown7[4] = {FF FF 01 00}
//      dwIdentifierLen
//      abyIdentifiers[dwIdentifierLen]
//
// Returns:
//  TRUE            On success
//  FALSE           On error
//
//********************************************************************

BOOL XL5GetIdentInfo
(
    LPSS_STREAM     lpstStream,
    LPWORD          lpwNumModules,
    LPDWORD         lpdwModuleIndexOffset,
    LPDWORD         lpdwTableSize,
    LPDWORD         lpdwTableOffset,
    LPDWORD         lpdwDataSize,
    LPDWORD         lpdwDataOffset
)
{
    DWORD           dwOffset;
    DWORD           dwTemp;
    WORD            wCount;
    WORD            wTemp;
    WORD            wNumModules;
    WORD            w;
    DWORD           dwBytesRead;
    BYTE            byTemp;

    // abyUnknown0[10]

    dwOffset = 10;

    // wCount0

    if (SSSeekRead(lpstStream,
                   dwOffset,
                   &wCount,
                   sizeof(WORD),
                   &dwBytesRead) != SS_STATUS_OK ||
        dwBytesRead != sizeof(WORD))
        return(FALSE);

    // Skip over wCount0 and wExtra0

    dwOffset += sizeof(WORD) + 2;

    wCount = WENDIAN(wCount);
    while (wCount-- != 0)
    {
        // wExtra1

        if (SSSeekRead(lpstStream,
                       dwOffset,
                       &wTemp,
                       sizeof(WORD),
                       &dwBytesRead) != SS_STATUS_OK ||
            dwBytesRead != sizeof(WORD))
            return(FALSE);

        dwOffset += sizeof(WORD);

        wTemp = WENDIAN(wTemp);

        // Read abyExtra1[2]

        if (SSSeekRead(lpstStream,
                       dwOffset + 2,
                       &byTemp,
                       sizeof(BYTE),
                       &dwBytesRead) != SS_STATUS_OK ||
            dwBytesRead != sizeof(BYTE))
            return(FALSE);

        // Skip over abyExtra1[wExtra1]

        dwOffset += wTemp;

        if (byTemp == 'C')
        {
            // Read wExtra1A

            if (SSSeekRead(lpstStream,
                           dwOffset,
                           &wTemp,
                           sizeof(WORD),
                           &dwBytesRead) != SS_STATUS_OK ||
                dwBytesRead != sizeof(WORD))
                return(FALSE);

            wTemp = WENDIAN(wTemp);

            // Skip over wExtra1A and abyExtra1A[wExtra1A]

            dwOffset += sizeof(WORD) + wTemp;
        }

        // Skip over abyExtra1B[6]

        dwOffset += 6;
    }

    // wExtra2

    if (SSSeekRead(lpstStream,
                   dwOffset,
                   &wTemp,
                   sizeof(WORD),
                   &dwBytesRead) != SS_STATUS_OK ||
        dwBytesRead != sizeof(WORD))
        return(FALSE);

    wTemp = WENDIAN(wTemp);

    // Skip over wExtra2, abyExtra2[wExtra2 * 4]

    dwOffset += sizeof(WORD) + wTemp * (DWORD)4;

    // abyUnknown1[10]

    dwOffset += 10;

    // wExtra3

    if (SSSeekRead(lpstStream,
                   dwOffset,
                   &wTemp,
                   sizeof(WORD),
                   &dwBytesRead) != SS_STATUS_OK ||
        dwBytesRead != sizeof(WORD))
        return(FALSE);

    wTemp = WENDIAN(wTemp);

    if (wTemp == 0xFFFF)
        wTemp = 0;

    // Skip over wExtra3, abyExtra3[wExtra3], abyUnknown2[84],
    //  0xFF, 0xFF

    dwOffset += sizeof(WORD) + wTemp + 84 + 1 + 1;

    // wCount

    if (SSSeekRead(lpstStream,
                   dwOffset,
                   &wTemp,
                   sizeof(WORD),
                   &dwBytesRead) != SS_STATUS_OK ||
        dwBytesRead != sizeof(WORD))
        return(FALSE);

    wNumModules = WENDIAN(wTemp);
    if (lpwNumModules != NULL)
        *lpwNumModules = wNumModules;

    dwOffset += sizeof(WORD);

    if (lpdwModuleIndexOffset != NULL)
        *lpdwModuleIndexOffset = dwOffset;

    // Skip over module index records

    for (w=0;w<wNumModules;w++)
    {
        // wNameUsedLen
        // abyNameUsed[wName0Len]      // Stream name of module

        if (SSSeekRead(lpstStream,
                       dwOffset,
                       &wTemp,
                       sizeof(WORD),
                       &dwBytesRead) != SS_STATUS_OK ||
            dwBytesRead != sizeof(WORD))
            return(FALSE);

        dwOffset += sizeof(WORD) + WENDIAN(wTemp);

        // wNameUnusedLen
        // abyNameUnused[wName1Len]

        if (SSSeekRead(lpstStream,
                       dwOffset,
                       &wTemp,
                       sizeof(WORD),
                       &dwBytesRead) != SS_STATUS_OK ||
            dwBytesRead != sizeof(WORD))
            return(FALSE);

        dwOffset += sizeof(WORD) + WENDIAN(wTemp);

        // abyUnknown3[5] = {03 00 2A 44 01}
        // wIdentifierRecordOffset
        // abyUnknown4[10] = {FF FF FF FF 00 00 00 00 FF FF}

        dwOffset += 5 + sizeof(WORD) + 10;
    }

    // abyUnknown5[4] = {FF FF 01 01}

    dwOffset += 4;

    // wExtra4

    if (SSSeekRead(lpstStream,
                   dwOffset,
                   &wTemp,
                   sizeof(WORD),
                   &dwBytesRead) != SS_STATUS_OK ||
        dwBytesRead != sizeof(WORD))
        return(FALSE);

    wTemp = WENDIAN(wTemp);

    // Skip over wExtra4, abyExtra4[wExtra4], and abyUnknown6[8]

    dwOffset += sizeof(WORD) + wTemp + 8;

    // dwIdentRecordLen

    if (SSSeekRead(lpstStream,
                   dwOffset,
                   &dwTemp,
                   sizeof(DWORD),
                   &dwBytesRead) != SS_STATUS_OK ||
        dwBytesRead != sizeof(DWORD))
        return(FALSE);

    dwTemp = DWENDIAN(dwTemp);
    if (lpdwTableSize != NULL)
        *lpdwTableSize = dwTemp;

    // Skip over dwIdentRecordLen

    dwOffset += sizeof(DWORD);

    // Remember offset of identifier records

    if (lpdwTableOffset != NULL)
        *lpdwTableOffset = dwOffset;

    // Skip over abyIdentRecords[dwIdentRecordLen]
    //  and abyUnknown7[4] = {FF FF 01 00}

    dwOffset += dwTemp + 4;

    // dwIdentifierLen

    if (SSSeekRead(lpstStream,
                   dwOffset,
                   &dwTemp,
                   sizeof(DWORD),
                   &dwBytesRead) != SS_STATUS_OK ||
        dwBytesRead != sizeof(DWORD))
        return(FALSE);

    dwTemp = DWENDIAN(dwTemp);
    if (lpdwDataSize != NULL)
        *lpdwDataSize = dwTemp;

    if (lpdwDataOffset != NULL)
        *lpdwDataOffset = dwOffset + sizeof(DWORD);

    return(TRUE);
}


//********************************************************************
//
// Function:
//  BOOL XL5DirInfoInit()
//
// Parameters:
//  lpstStream      Ptr to dir stream
//  lpstDir         Ptr to dir info structure
//
// Description:
//  The function initializes the dir stream structure for access
//  to the stream/module name pairs.
//
// Returns:
//  TRUE            On success
//  FALSE           On error
//
//********************************************************************

BOOL XL5DirInfoInit
(
    LPSS_STREAM     lpstStream,
    LPXL5_DIR_INFO  lpstDir
)
{
    lpstDir->lpstStream = lpstStream;

    if (XL5GetIdentInfo(lpstStream,
                        &lpstDir->wNumModules,
                        &lpstDir->dwFirstModuleIndexOffset,
                        &lpstDir->dwStringRecordLen,
                        &lpstDir->dwStringRecordOffset,
                        &lpstDir->dwStringLen,
                        &lpstDir->dwStringOffset) == FALSE)
        return(FALSE);

    lpstDir->wFirstCachedModuleName = 0;
    lpstDir->wNumCachedModuleNames = 0;
    lpstDir->dwNextModuleIndexOffset = lpstDir->dwFirstModuleIndexOffset;

    return(TRUE);
}


//********************************************************************
//
// Function:
//  BOOL XL5DirInfoLoad()
//
// Parameters:
//  lpstDir         Ptr to dir info structure
//
// Description:
//  Caches the next set of stream/module name pairs starting
//  at lpstDir->dwNextModuleIndexOffset.  The function caches
//  as many as will fit in abyModuleNameBuf[].
//
// Returns:
//  TRUE            On success
//  FALSE           On error
//
//********************************************************************

BOOL XL5DirInfoLoad
(
    LPXL5_DIR_INFO  lpstDir
)
{
    WORD            wIndex;
    int             nBytesUsed;
    DWORD           dwOffset;
    BYTE            byTemp;
    WORD            wTemp;
    DWORD           dwStrOffset;
    DWORD           dwBytesRead;
    LPBYTE          lpbyCur;

    wIndex = lpstDir->wFirstCachedModuleName +
             lpstDir->wNumCachedModuleNames;

    if (wIndex == lpstDir->wNumModules)
    {
        lpstDir->wFirstCachedModuleName = 0;
        dwOffset = lpstDir->dwFirstModuleIndexOffset;
        wIndex = 0;
    }
    else
    {
        lpstDir->wFirstCachedModuleName = wIndex;
        dwOffset = lpstDir->dwNextModuleIndexOffset;
    }

    nBytesUsed = 0;
    while (wIndex < lpstDir->wNumModules &&
           nBytesUsed < XL5_DIR_NAME_BUF_SIZE)
    {
        lpstDir->dwNextModuleIndexOffset = dwOffset;

        /////////////////////////////////////////////////////////
        // Get the module record

        // Get used stream name length

        if (SSSeekRead(lpstDir->lpstStream,
                       dwOffset,
                       &wTemp,
                       sizeof(WORD),
                       &dwBytesRead) != SS_STATUS_OK ||
            dwBytesRead != sizeof(WORD))
            return(FALSE);

        wTemp = WENDIAN(wTemp);

        dwOffset += sizeof(WORD);

        if (wTemp > SS_MAX_NAME_LEN)
        {
            // Kind of weird, don't store the name

            lpstDir->abyModuleNameBuf[nBytesUsed++] = 0;
        }
        else
        {
            lpstDir->abyModuleNameBuf[nBytesUsed++] = (BYTE)wTemp;

            // Is there enough room for the name?

            if (nBytesUsed + wTemp > XL5_DIR_NAME_BUF_SIZE)
                break;

            // Read the stream name

            if (SSSeekRead(lpstDir->lpstStream,
                           dwOffset,
                           lpstDir->abyModuleNameBuf + nBytesUsed,
                           wTemp,
                           &dwBytesRead) != SS_STATUS_OK ||
                dwBytesRead != wTemp)
                return(FALSE);

            nBytesUsed += wTemp;
        }

        dwOffset += wTemp;

        // Skip over unused name

        // Get unused stream name length

        if (SSSeekRead(lpstDir->lpstStream,
                       dwOffset,
                       &wTemp,
                       sizeof(WORD),
                       &dwBytesRead) != SS_STATUS_OK ||
            dwBytesRead != sizeof(WORD))
            return(FALSE);

        wTemp = WENDIAN(wTemp);

        dwOffset += sizeof(WORD) + wTemp + 5;

        // Get identifier record offset

        if (SSSeekRead(lpstDir->lpstStream,
                       dwOffset,
                       &wTemp,
                       sizeof(WORD),
                       &dwBytesRead) != SS_STATUS_OK ||
            dwBytesRead != sizeof(WORD))
            return(FALSE);

        wTemp = WENDIAN(wTemp);

        dwOffset += sizeof(WORD) + 10;

        /////////////////////////////////////////////////////////
        // Get the string offset from the string record

        if (SSSeekRead(lpstDir->lpstStream,
                       lpstDir->dwStringRecordOffset + wTemp + 6,
                       &wTemp,
                       sizeof(WORD),
                       &dwBytesRead) != SS_STATUS_OK ||
            dwBytesRead != sizeof(WORD))
            return(FALSE);

        wTemp = WENDIAN(wTemp);

        /////////////////////////////////////////////////////////
        // Get the string

        dwStrOffset = lpstDir->dwStringOffset + wTemp;

        if (nBytesUsed + 1 > XL5_DIR_NAME_BUF_SIZE)
            break;

        // Get a pointer to the length byte

        lpbyCur = lpstDir->abyModuleNameBuf + nBytesUsed++;

        for (wTemp=1;wTemp<=255;wTemp++)
        {
            if (SSSeekRead(lpstDir->lpstStream,
                           dwStrOffset++,
                           &byTemp,
                           sizeof(BYTE),
                           &dwBytesRead) != SS_STATUS_OK ||
                dwBytesRead != sizeof(BYTE))
                return(FALSE);

            if (byTemp == 0)
            {
                // Set the length

                *lpbyCur = (BYTE)(wTemp - 1);
                break;
            }

            if (++nBytesUsed > XL5_DIR_NAME_BUF_SIZE)
            {
                // Not enough room

                wTemp = 0;
                break;
            }

            lpbyCur[wTemp] = byTemp;
        }

        if (wTemp == 0)
        {
            // There wasn't enough room

            break;
        }

        ++wIndex;
    }

    if (wIndex == lpstDir->wFirstCachedModuleName)
    {
        // Nothing was cached

        return(FALSE);
    }

    lpstDir->wNumCachedModuleNames =
        wIndex - lpstDir->wFirstCachedModuleName;

    return(TRUE);
}


//********************************************************************
//
// Function:
//  BOOL XL5DirInfoModuleName()
//
// Parameters:
//  lpstDir         Ptr to dir info structure
//  lppsStreamName  Ptr to pascal string of stream name
//  lppsModuleName  Ptr to buffer for pascal string of module name
//
// Description:
//  Gets the module name given the stream name.
//
// Returns:
//  TRUE            On success
//  FALSE           On error
//
//********************************************************************

BOOL XL5DirInfoModuleName
(
    LPXL5_DIR_INFO  lpstDir,
    LPBYTE          lppsStreamName,
    LPBYTE          lppsModuleName
)
{
    WORD            wNumModulesLeft;
    WORD            wCurIndex;
    LPBYTE          lpbyCurName;
    int             i;

    wNumModulesLeft = lpstDir->wNumModules;
    wCurIndex = lpstDir->wFirstCachedModuleName;
    lpbyCurName = lpstDir->abyModuleNameBuf;
    while (wNumModulesLeft-- != 0)
    {
        if (wCurIndex < lpstDir->wFirstCachedModuleName ||
            wCurIndex >= lpstDir->wFirstCachedModuleName +
                         lpstDir->wNumCachedModuleNames)
        {
            // Load next set

            if (XL5DirInfoLoad(lpstDir) == FALSE)
                return(FALSE);

            lpbyCurName = lpstDir->abyModuleNameBuf;

            wCurIndex = lpstDir->wFirstCachedModuleName;
        }

        // Compare

        if (lpbyCurName[0] == lppsStreamName[0])
        {
            // Compare the stream names

            for (i=0;i<lpbyCurName[0];i++)
            {
                if (lpbyCurName[i + 1] != lppsStreamName[i + 1])
                    break;
            }

            if (i == lpbyCurName[0])
            {
                // Found a match, copy the module name

                lpbyCurName += lpbyCurName[0] + 1;
                for (i=0;i<=lpbyCurName[0];i++)
                    lppsModuleName[i] = lpbyCurName[i];

                return(TRUE);
            }
        }

        // No match, skip over stream and module name

        lpbyCurName += lpbyCurName[0] + 1;
        lpbyCurName += lpbyCurName[0] + 1;

        ++wCurIndex;
    }

    // Could not find it

    return(FALSE);
}


//********************************************************************
//
// Function:
//  BOOL XL5DirIDInit()
//
// Parameters:
//  lpstStream          Ptr to dir stream
//  lpstDir             Ptr to XL5_DIR_ID_T structure
//
// Description:
//  Initializes the XL5_DIR_ID_T structure for ID access.
//
// Returns:
//  TRUE                On success
//  FALSE               On error
//
//********************************************************************

BOOL XL5DirIDInit
(
    LPSS_STREAM         lpstStream,
    LPXL5_DIR_ID        lpstDir
)
{
    lpstDir->lpstStream = lpstStream;

    return XL5GetIdentInfo(lpstStream,
                           NULL,
                           NULL,
                           &lpstDir->dwTableSize,
                           &lpstDir->dwTableOffset,
                           &lpstDir->dwDataSize,
                           &lpstDir->dwDataOffset);
}


//********************************************************************
//
// Function:
//  BOOL XL5GetIdentifier()
//
// Parameters:
//  lpstDir             Ptr to XL5_DIR_ID_T structure
//  wID                 ID of identifier
//  lpszID              256-byte buffer for C-string ID
//
// Description:
//  Returns in lpszID the identifier with the given ID.
//
// Returns:
//  TRUE                On success
//  FALSE               On error
//
//********************************************************************

BOOL XL5GetIdentifier
(
    LPXL5_DIR_ID        lpstDir,
    WORD                wID,
    LPSTR               lpszID
)
{
    WORD                wIdentOffset;
    DWORD               dwBytesRead;
    DWORD               dwNumBytesToRead;

    // Read the offset of the identifier

    if (SSSeekRead(lpstDir->lpstStream,
                   lpstDir->dwTableOffset + wID + 6,
                   &wIdentOffset,
                   sizeof(WORD),
                   &dwBytesRead) != SS_STATUS_OK ||
        dwBytesRead != sizeof(WORD))
        return(FALSE);

    wIdentOffset = WENDIAN(wIdentOffset);

    // Out of bounds?

    if (wIdentOffset >= lpstDir->dwDataSize)
        return(FALSE);

    // Read at most 255 bytes of the identifier

    if (wIdentOffset + 256 > lpstDir->dwDataSize)
        dwNumBytesToRead = lpstDir->dwDataSize - wIdentOffset;
    else
        dwNumBytesToRead = 256;

    if (SSSeekRead(lpstDir->lpstStream,
                   lpstDir->dwDataOffset + wIdentOffset,
                   lpszID,
                   dwNumBytesToRead,
                   &dwBytesRead) != SS_STATUS_OK ||
        dwBytesRead != dwNumBytesToRead)
        return(FALSE);

    // Make sure it is zero-terminated

    lpszID[dwNumBytesToRead - 1] = 0;

    return(TRUE);
}


//*************************************************************************
//
// Function:
//  BOOL XL5FindKey()
//
// Parameters:
//  lpstStream          Ptr to Book stream
//  lpstKey             Ptr to XL5 key structure to store key if found
//
// Description:
//  Determines whether the document is encrypted or not and returns
//  in lpstKey the 16-byte key if it is.
//
//  The function determines the encryption state by searching
//  through the workbook global records for a FILEPASS record.
//  The presence of a FILEPASS record indicates that the file is
//  encrypted.
//
//  If the document is encrypted, the WRITEACCESS record is used
//  to determine the 16-byte key.
//
//  The function assumes that if the file is encrypted, that the
//  FILEPASS record will always precede the WRITEACCESS record.
//
//  In order to be able to determine the 16-byte key, at least
//  20 consecutive BYTEs within the WRITEACCESS record must be
//  spaces (0x20).  The function assumes that if it sees 20
//  bytes such that the last four are the same as the first four,
//  that underneath the last 16 is the 16-byte key.  The
//  success probability in this case is assumed to be around 25%.
//
//  If there are 32 bytes such that the first 16 are the same as
//  the last 16, then the success probability is assumed to be
//  around 100%.
//
//  The nSuccessProbability field is set to four for 25% and to
//  16 for 100%.
//
//  If the function returns TRUE with a success probability of zero,
//  then it just obtains the key using the last 16-bytes.
//
// Returns:
//  TRUE                On success
//  FALSE               On error
//
//*************************************************************************

BOOL XL5FindKey
(
    LPSS_STREAM         lpstStream,
    LPXL5ENCKEY         lpstKey
)
{
    DWORD               dwOffset;
    DWORD               dwLen;
    DWORD               dwWriteAccessRecOffset;
    DWORD               dwWriteAccessRecLength;
    DWORD               dwNumBytesToRead;
    DWORD               dwBytesRead;
    XL_REC_HDR_T        stHdr;
    BYTE                abyBuf[32];
    BYTE                abyKeyTemp[16];
    int                 i, j;

    dwLen = SSStreamLen(lpstStream);

    // Initialize to not encrypted

    lpstKey->bEncrypted = FALSE;

    /////////////////////////////////////////////////////////////
    // Look for the FILEPASS and WRITEACCESS records in the
    //  workbook global records

    dwOffset = 0;
    dwWriteAccessRecOffset = 0;
    while (dwOffset < dwLen)
    {
        // Get the record header

        if (SSSeekRead(lpstStream,
                       dwOffset,
                       &stHdr,
                       sizeof(XL_REC_HDR_T),
                       &dwBytesRead) != SS_STATUS_OK ||
            dwBytesRead != sizeof(XL_REC_HDR_T))
            return(FALSE);

        stHdr.wType = WENDIAN(stHdr.wType);
        stHdr.wLen = WENDIAN(stHdr.wLen);

        if (stHdr.wType == eXLREC_FILEPASS)
        {
            // Must be >= length 2

            if (stHdr.wLen < 2)
                return(FALSE);

            // Store the offset

            lpstKey->dwFilePassOffset = dwOffset;

            // It is encrypted

            lpstKey->bEncrypted = TRUE;
        }
        else
        if (stHdr.wType == eXLREC_WRITEACCESS)
        {
            dwWriteAccessRecOffset = dwOffset;
            dwWriteAccessRecLength = stHdr.wLen;
            break;
        }
        else
        if (stHdr.wType == eXLREC_EOF)
            break;

        dwOffset += sizeof(XL_REC_HDR_T) + stHdr.wLen;
    }

    // If it is not encrypted, just return

    if (lpstKey->bEncrypted == FALSE)
        return(TRUE);


    /////////////////////////////////////////////////////////////
    // Get info from WRITEACCESS record

    if (dwWriteAccessRecOffset == 0)
        return(FALSE);

    dwLen = dwWriteAccessRecLength;
    while (1)
    {
        lpstKey->nSuccessProbability = 0;

        if (dwLen < 32)
            dwNumBytesToRead = (int)dwLen;
        else
            dwNumBytesToRead = 32;

        if (dwNumBytesToRead < 16)
        {
            if (dwWriteAccessRecLength < 20)
            {
                // Impossible to determine key using present methods

                return(FALSE);
            }

            // Use last 16 bytes before the last four to determine key

            // Read the last 16 bytes of the record before the last four

            if (SSSeekRead(lpstStream,
                           dwWriteAccessRecOffset + sizeof(XL_REC_HDR_T) +
                               dwWriteAccessRecLength - 20,
                           abyBuf,
                           16,
                           &dwBytesRead) != SS_STATUS_OK ||
                dwBytesRead != 16)
            {
                // Failed to read write access record data

                return(FALSE);
            }

            dwLen = dwWriteAccessRecLength - 4;
            dwNumBytesToRead = 16;
            break;
        }

        // Read up to the last 32 bytes of the record

        if (SSSeekRead(lpstStream,
                       dwWriteAccessRecOffset + sizeof(XL_REC_HDR_T) +
                           dwLen - dwNumBytesToRead,
                       abyBuf,
                       dwNumBytesToRead,
                       &dwBytesRead) != SS_STATUS_OK ||
            dwBytesRead != dwNumBytesToRead)
        {

            // Failed to read write access record data

            return(FALSE);
        }

        // Check probability of success

        i = (int)(dwNumBytesToRead - 16);
        while (i != 0)
        {
            --i;
            if (abyBuf[i] != abyBuf[i+16])
                break;

            // Increment success probability count

            lpstKey->nSuccessProbability++;
        }

        if (lpstKey->nSuccessProbability >= 4)
            break;

        --dwLen;
    }

    /////////////////////////////////////////////////////////////
    // Calculate key

    // Determine key using last 16 bytes

    j = (int)(dwNumBytesToRead - 16);
    for (i=0;i<16;i++)
        abyKeyTemp[i] = abyBuf[j+i] ^
            ((0x20 >> 3) | ((0x20 << 5) & 0xFF));

    // Normalize key

    j = (int)
        ((((dwWriteAccessRecOffset + dwWriteAccessRecLength) & 0xF) +
          ((dwLen -  16) & 0xF)) & 0xF);

    j = (16 - j) & 0xF;

    for (i=0;i<16;i++)
    {
        if (j == 16)
            j = 0;

        lpstKey->abyKey[i] = abyKeyTemp[j++];
    }

    return(TRUE);
}


//*************************************************************************
//
// Function:
//  BOOL XL5CalculatePassword()
//
// Parameters:
//  lpstStream          Ptr to Book stream
//  lpstKey             Ptr to XL5 key structure containing key
//  lpabyPassword       16-byte buffer for password
//
// Description:
//  Given the 16-byte key in lpstKey, calculates the password
//  and stores it as a Pascal string at lpabyPassword.
//
// Returns:
//  TRUE                On success
//  FALSE               On error
//
//*************************************************************************

BOOL XL5CalculatePassword
(
    LPSS_STREAM         lpstStream,
    LPXL5ENCKEY         lpstKey,
    LPBYTE              lpabyPassword
)
{
    BYTE                byTemp;
    int                 i, j;
    BYTE                abyFilePass[2];
    DWORD               dwBytesRead;

    // Read the first two bytes of the FILEPASS record

    if (SSSeekRead(lpstStream,
                   lpstKey->dwFilePassOffset + sizeof(XL_REC_HDR_T),
                   abyFilePass,
                   2 * sizeof(BYTE),
                   &dwBytesRead) != SS_STATUS_OK ||
        dwBytesRead != 2 * sizeof(BYTE))
        return(FALSE);

    // Calculate the password

    i = 0;
    j = 12;
    while (i < 16)
    {
        byTemp = (lpstKey->abyKey[j] << 1) & 0xFE;
        if (lpstKey->abyKey[j] & 0x80)
            byTemp |= 0x01;

        lpabyPassword[i++] = byTemp ^ abyFilePass[0];

        j = (j + 1) & 0xF;

        byTemp = (lpstKey->abyKey[j] << 1) & 0xFE;
        if (lpstKey->abyKey[j] & 0x80)
            byTemp |= 0x01;

        lpabyPassword[i++] = byTemp ^ abyFilePass[1];

        j = (j + 1) & 0xF;
    }

    // Search for the last 0xBB

    for (i=15;i>=0;i--)
        if (lpabyPassword[i] == 0xBB)
            break;

    // i is the length

    for (j=i;j>0;j--)
        lpabyPassword[j] = lpabyPassword[j-1];

    // Store the length as the first BYTE

    lpabyPassword[0] = (BYTE)i;

    return(TRUE);
}


//*************************************************************************
//
// Function:
//  BOOL XL5EncryptedRead()
//
// Parameters:
//  lpstStream          Ptr to Book stream
//  dwRecOffset         Offset of record header of record data to read
//  stRecHdr            Record header
//  lpstKey             Decryption key
//  wDataOffset         Offset in record of data to read
//  lpbyData            Ptr to buffer to store read data
//  wNumBytes           Number of bytes to read from the given offset
//
// Description:
//  Reads the encrypted data from the record and then decrypts
//  using the key.  The first four bytes of a BOUNDSHEET record are
//  assumed to not be encrypted.
//
// Returns:
//  TRUE    On success
//  FALSE   On failure
//
//*************************************************************************

BOOL XL5EncryptedRead
(
    LPSS_STREAM         lpstStream,
    DWORD               dwRecOffset,
    XL_REC_HDR_T        stRecHdr,
    LPXL5ENCKEY         lpstKey,
    WORD                wDataOffset,
    LPBYTE              lpbyData,
    WORD                wNumBytes
)
{
    DWORD               dwBytesRead;
    int                 i;

    // Read the encrypted data

    if (SSSeekRead(lpstStream,
                   dwRecOffset + sizeof(XL_REC_HDR_T) + wDataOffset,
                   lpbyData,
                   wNumBytes,
                   &dwBytesRead) != SS_STATUS_OK ||
        dwBytesRead != wNumBytes)
        return(FALSE);

    // If the data is not encrypted, just return success

    if (lpstKey == NULL || lpstKey->bEncrypted == FALSE)
        return(TRUE);

    // BOF records are not encrypted

    if (stRecHdr.wType == eXLREC_BOF)
        return(TRUE);

    /////////////////////////////////////////////////////////////
    // Decrypt the data

    // BOUNDSHEET starts decryption at offset four

    if (stRecHdr.wType == eXLREC_BOUNDSHEET && wDataOffset < 4)
    {
        // Was only the non-encrypted portion read?

        if (wNumBytes <= (4 - wDataOffset))
            return(TRUE);

        lpbyData += 4 - wDataOffset;
        wNumBytes -= 4 - wDataOffset;
        wDataOffset = 4;
    }

    // Determine the first encryption byte

    i = (int)((dwRecOffset + stRecHdr.wLen + wDataOffset) & 0x0F);

    // Decrypt the data

    while (wNumBytes-- != 0)
    {
        *lpbyData = *lpbyData ^ lpstKey->abyKey[i];
        *lpbyData = ((*lpbyData << 3) & 0xF8) | ((*lpbyData >> 5) & 0x07);
        ++lpbyData;
        if (++i == 16)
            i = 0;
    }

    return(TRUE);
}


//*************************************************************************
//
// Function:
//  BOOL XL5EncryptedWrite()
//
// Parameters:
//  lpstStream          Ptr to Book stream
//  dwRecOffset         Offset of record header of record to write to
//  stRecHdr            Record header
//  lpstKey             Decryption key
//  wDataOffset         Offset in record to write data
//  lpbyData            Ptr to buffer with data to write
//  wNumBytes           Number of bytes to write at the given offset
//
// Description:
//  Encrypts the data and then writes the data to the record.
//  The first four bytes of a BOUNDSHEET record are assumed
//  to not be encrypted.
//
// Returns:
//  TRUE    On success
//  FALSE   On failure
//
//*************************************************************************

BOOL XL5EncryptedWrite
(
    LPSS_STREAM         lpstStream,
    DWORD               dwRecOffset,
    XL_REC_HDR_T        stRecHdr,
    LPXL5ENCKEY         lpstKey,
    WORD                wDataOffset,
    LPBYTE              lpbyData,
    WORD                wNumBytes
)
{
    DWORD               dwBytesWritten;
    int                 i;

    if (lpstKey != NULL &&
        lpstKey->bEncrypted != FALSE &&
        stRecHdr.wType != eXLREC_BOF)
    {
        WORD            wEncDataOffset = wDataOffset;
        WORD            wEncNumBytes = wNumBytes;
        LPBYTE          lpbyEncData = lpbyData;

        // Encrypt the data

        // BOUNDSHEET starts decryption at offset four

        if (stRecHdr.wType == eXLREC_BOUNDSHEET && wEncDataOffset < 4)
        {
            // Was only the non-encrypted portion read?

            if (wNumBytes <= (4 - wDataOffset))
                wEncNumBytes = 0;
            else
            {
                lpbyEncData += 4 - wEncDataOffset;
                wEncNumBytes -= 4 - wEncDataOffset;
                wEncDataOffset = 4;
            }
        }

        // Determine the first encryption byte

        i = (int)((dwRecOffset + stRecHdr.wLen + wEncDataOffset) & 0x0F);

        // Encrypt the data

        while (wEncNumBytes-- != 0)
        {
            *lpbyEncData = ((*lpbyEncData >> 3) & 0x1F) |
                ((*lpbyEncData << 5) & 0xE0);
            *lpbyEncData = *lpbyEncData ^ lpstKey->abyKey[i];
            ++lpbyEncData;
            if (++i == 16)
                i = 0;
        }
    }

    // Write the encrypted data

    if (SSSeekWrite(lpstStream,
                    dwRecOffset + sizeof(XL_REC_HDR_T) + wDataOffset,
                    lpbyData,
                    wNumBytes,
                    &dwBytesWritten) != SS_STATUS_OK ||
        dwBytesWritten != wNumBytes)
        return(FALSE);

    return(TRUE);
}


//*************************************************************************
//
// Function:
//  BOOL XL5EncryptedWriteRepeat()
//
// Parameters:
//  lpstStream          Ptr to Book stream
//  dwRecOffset         Offset of record header of record to write to
//  stRecHdr            Record header
//  lpstKey             Decryption key
//  wDataOffset         Offset in record to write data
//  byValue             Byte value to repeat
//  wNumBytes           Number of bytes to write at the given offset
//
// Description:
//  Repeats a write of the given byte value at the given offset.
//  The bytes are encrypted appropriately with calls to
//  XL5EncryptedWrite().
//
// Returns:
//  TRUE    On success
//  FALSE   On failure
//
//*************************************************************************

BOOL XL5EncryptedWriteRepeat
(
    LPSS_STREAM         lpstStream,
    DWORD               dwRecOffset,
    XL_REC_HDR_T        stRecHdr,
    LPXL5ENCKEY         lpstKey,
    WORD                wDataOffset,
    BYTE                byValue,
    WORD                wNumBytes
)
{
    BYTE                abyChunk[16];
    WORD                wBytesToWrite;
    int                 i;

    for (i=0;i<16;i++)
        abyChunk[i] = byValue;

    wBytesToWrite = 16;
    while (wNumBytes > 0)
    {
        if (wNumBytes < 16)
            wBytesToWrite = wNumBytes;

        if (XL5EncryptedWrite(lpstStream,
                              dwRecOffset,
                              stRecHdr,
                              lpstKey,
                              wDataOffset,
                              abyChunk,
                              wBytesToWrite) == FALSE)
            return(FALSE);

        wNumBytes -= wBytesToWrite;
        wDataOffset += wBytesToWrite;
    }

    return(TRUE);
}


//*************************************************************************
//
// Function:
//  BOOL XL5EncryptWriteRecords()
//
// Parameters:
//  lpstStream          Ptr to Book stream
//  lpstKey             Encryption key
//  dwOffset            Offset of beginning of write
//  lpabyRecordData     Ptr to record data to write
//  lRecordDataSize     Size of record data
//
// Description:
//  The function assumes that the record data is an array of bytes
//  in raw format as it would appear unencrypted in the actual stream.
//  The array of bytes is assumed to be one or more records.
//
//  The function writes the records in encrypted form if the stream
//  is encrypted.
//
// Returns:
//  TRUE                On success
//  FALSE               One error
//
//*************************************************************************

BOOL XL5EncryptWriteRecords
(
    LPSS_STREAM             lpstStream,
    LPXL5ENCKEY             lpstKey,
    DWORD                   dwOffset,
    LPBYTE                  lpabyRecordData,
    long                    lRecordDataSize
)
{
    DWORD                   dwByteCount;
    LPXL_REC_HDR            lpstHdr;
    XL_REC_HDR_T            stHdr;

    while (lRecordDataSize > 0)
    {
        lpstHdr = (LPXL_REC_HDR)lpabyRecordData;
        stHdr = *lpstHdr;
        stHdr.wType = WENDIAN(stHdr.wType);
        stHdr.wLen = WENDIAN(stHdr.wLen);

        // Write the record header

        if (SSSeekWrite(lpstStream,
                        dwOffset,
                        lpstHdr,
                        sizeof(XL_REC_HDR_T),
                        &dwByteCount) != SS_STATUS_OK ||
            dwByteCount != sizeof(XL_REC_HDR_T))
            return(FALSE);

        // Write the record data

        if (stHdr.wLen != 0)
        {
            if (XL5EncryptedWrite(lpstStream,
                                  dwOffset,
                                  stHdr,
                                  lpstKey,
                                  0,
                                  lpabyRecordData + sizeof(XL_REC_HDR_T),
                                  stHdr.wLen) == FALSE)
                return(FALSE);
        }

        dwOffset += sizeof(XL_REC_HDR_T) + stHdr.wLen;
        lRecordDataSize -= sizeof(XL_REC_HDR_T) + stHdr.wLen;
        lpabyRecordData += sizeof(XL_REC_HDR_T) + stHdr.wLen;
    }

    return(TRUE);
}


//********************************************************************
//
// Function:
//  void XL5DecryptData()
//
// Parameters:
//  dwRecOffset         Offset of the record header
//  stRecHdr            The record header (endianized)
//  lpstKey             Ptr to the key
//  wDataOffset         Offset of data wrt to header
//  lpabyData           Ptr to the data
//  wNumBytes           Number of bytes to decrypt
//
// Description:
//  The function assumes that lpstKey points to a valid key
//  and that the data is actually encrypted.
//
//  The function decrypts the given data using the given key.
//
// Returns:
//  Nothing
//
//********************************************************************

void XL5DecryptData
(
    DWORD               dwRecOffset,
    XL_REC_HDR_T        stRecHdr,
    LPXL5ENCKEY         lpstKey,
    WORD                wDataOffset,
    LPBYTE              lpabyData,
    WORD                wNumBytes
)
{
    int                 i;

    // BOUNDSHEET starts decryption at offset four

    if (stRecHdr.wType == eXLREC_BOUNDSHEET && wDataOffset < 4)
    {
        // Was only the non-encrypted portion read?

        if (wNumBytes <= (4 - wDataOffset))
            return;

        lpabyData += 4 - wDataOffset;
        wNumBytes -= 4 - wDataOffset;
        wDataOffset = 4;
    }

    // Determine the first encryption byte

    i = (int)((dwRecOffset + stRecHdr.wLen + wDataOffset) & 0x0F);

    // Decrypt the data

    while (wNumBytes-- != 0)
    {
        *lpabyData = *lpabyData ^ lpstKey->abyKey[i];
        *lpabyData = ((*lpabyData << 3) & 0xF8) | ((*lpabyData >> 5) & 0x07);
        ++lpabyData;
        if (++i == 16)
            i = 0;
    }
}


//*************************************************************************
//
// Function:
//  DWORD XL5CRC32Expr()
//
// Parameters:
//  lpabyExpr           Ptr to the expression buffer
//  dwExprSize          Number of bytes in the expression buffer
//  dwCRC               Starting CRC value
//  lpdwCRCByteCount    Ptr to DWORD containing beginning count
//
// Description:
//  Continues a CRC32 on an expression.  The expression does not
//  need to be a complete expression.
//
//  The expression is assumed to be in the form of an Excel 4.0
//  macro formula parsed expression.
//
//  The number of bytes used to continue the CRC is added
//  to *lpdwCRCByteCount.  lpdwCRCByteCount may be NULL if the
//  count is not desired.
//
// Returns:
//  DWORD               The continued CRC value
//
//*************************************************************************

// Number of bytes, including the token byte

#ifdef SYM_WIN16
#pragma data_seg("FAR_DATA_0")
#endif

BYTE FAR gabyXL5FormulaTokenSkip[] =
{
    1,  // Unknown
    5,  // eXLFM_EXP        0x01 // Array formula or shared formula
    5,  // eXLFM_TBL        0x02 // Data table
    1,  // eXLFM_ADD        0x03 // Addition
    1,  // eXLFM_SUB        0x04 // Subtraction
    1,  // eXLFM_MUL        0x05 // Multiplication
    1,  // eXLFM_DIV        0x06 // Division
    1,  // eXLFM_POWER      0x07 // Exponentiation
    1,  // eXLFM_CONCAT     0x08 // Concatenation
    1,  // eXLFM_LT         0x09 // Less than
    1,  // eXLFM_LE         0x0A // Less than or equal
    1,  // eXLFM_EQ         0x0B // Equal
    1,  // eXLFM_GE         0x0C // Greater than or equal
    1,  // eXLFM_GT         0x0D // Greater than
    1,  // eXLFM_NE         0x0E // Not equal
    1,  // eXLFM_ISECT      0x0F // Intersection
    1,  // eXLFM_UNION      0x10 // Union
    1,  // eXLFM_RANGE      0x11 // Range
    1,  // eXLFM_UPLUS      0x12 // Unary plus
    1,  // eXLFM_UMINUS     0x13 // Unary minus
    1,  // eXLFM_PERCENT    0x14 // Percent sign
    1,  // eXLFM_PAREN      0x15 // Parenthesis
    1,  // eXLFM_MISSARG    0x16 // Missing argument
    1,  // eXLFM_STR        0x17 // String constant
    1,  // eXLFM_EXTEND     0x18 // Extended ptg
    1,  // eXLFM_ATTR       0x19 // Special attribute
    1,  // eXLFM_SHEET      0x1A // Deleted
    1,  // eXLFM_ENDSHEET   0x1B // Deleted
    2,  // eXLFM_ERR        0x1C // Error value
    2,  // eXLFM_BOOL       0x1D // Boolean
    3,  // eXLFM_INT        0x1E // Integer
    9,  // eXLFM_NUM        0x1F // Number
};

BYTE FAR gabyXL5FormulaOpTokenSkip[] =
{
    8,  // eXLFM_ARRAY          0x20 // Array constant
    3,  // eXLFM_FUNC           0x21 // Function, fixed number of arguments
    4,  // eXLFM_FUNCVAR        0x22 // Function, variable number of arguments
    15, // eXLFM_NAME           0x23 // Name
    4,  // eXLFM_REF            0x24 // Cell reference
    7,  // eXLFM_AREA           0x25 // Area reference
    7,  // eXLFM_MEMAREA        0x26 // Constant reference subexpression
    7,  // eXLFM_MEMERR         0x27 // Erroneous constant reference subexpression
    7,  // eXLFM_MEMNOMEM       0x28 // Incomplete constant reference subexpression
    3,  // eXLFM_MEMFUNC        0x29 // Variable reference subexpression
    4,  // eXLFM_REFERR         0x2A // Deleted cell reference
    7,  // eXLFM_AREAERR        0x2B // Deleted area reference
    4,  // eXLFM_REFN           0x2C // Cell reference within a shared formula
    7,  // eXLFM_AREAN          0x2D // Area reference within a shared formula
    3,  // eXLFM_MEMAREAN       0x2E // Reference subexpression within a name
    3,  // eXLFM_MEMNOMEMN      0x2F // Incomplete reference subexpression within a name
    1,  // Unknown              0x30
    1,  // Unknown              0x31
    1,  // Unknown              0x32
    1,  // Unknown              0x33
    1,  // Unknown              0x34
    1,  // Unknown              0x35
    1,  // Unknown              0x36
    1,  // Unknown              0x37
    1,  // eXLFM_FUNCCE         0x38 // FuncCE
    25, // eXLFM_NAMEX          0x39 // Name or external name
    18, // eXLFM_REF3D          0x3A // 3-D cell reference
    21, // eXLFM_AREA3D         0x3B // 3-D area reference
    18, // eXLFM_REFERR3D       0x3C // Deleted 3-D cell reference
    21, // eXLFM_AREAERR3D      0x3D // Deleted 3-D area reference
    1,  // Unknown              0x3E
    1,  // Unknown              0x3F
};

BYTE FAR gabyXL5FormulaOpTokenChange[] =
{
    0x00,  // eXLFM_ARRAY
    0x01,  // eXLFM_FUNC
    0x02,  // eXLFM_FUNCVAR
    0x03,  // eXLFM_NAME
    0x04,  // eXLFM_REF
    0x05,  // eXLFM_AREA
    0x06,  // eXLFM_MEMAREA
    0x06,  // eXLFM_MEMERR      -> eXLFM_MEMAREA
    0x06,  // eXLFM_MEMNOMEM    -> eXLFM_MEMAREA
    0x09,  // eXLFM_MEMFUNC
    0x04,  // eXLFM_REFERR      -> eXLFM_REF
    0x05,  // eXLFM_AREAERR     -> eXLFM_AREA
    0x0C,  // eXLFM_REFN
    0x0D,  // eXLFM_AREAN
    0x0E,  // eXLFM_MEMAREAN
    0x0E,  // eXLFM_MEMNOMEMN   -> eXLFM_MEMAREAN
    0x10,  // Unknown
    0x11,  // Unknown
    0x12,  // Unknown
    0x13,  // Unknown
    0x14,  // Unknown
    0x15,  // Unknown
    0x16,  // Unknown
    0x17,  // Unknown
    0x18,  // eXLFM_FUNCCE
    0x19,  // eXLFM_NAMEX
    0x1A,  // eXLFM_REF3D
    0x1B,  // eXLFM_AREA3D
    0x1A,  // eXLFM_REFERR3D    -> eXLFM_REF3D
    0x1B,  // eXLFM_AREAERR3D   -> eXLFM_AREA3D
    0x1E,  // Unknown
    0x1F,  // Unknown
};

#ifdef SYM_WIN16
#pragma data_seg()
#endif

DWORD XL5CRC32Expr
(
    LPBYTE              lpabyExpr,
    DWORD               dwExprSize,
    DWORD               dwCRC,
    LPDWORD             lpdwCRCByteCount
)
{
    BYTE                byToken;
    BYTE                byTemp;
    DWORD               dwTemp;
    DWORD               dwCRCByteCount;

    dwCRCByteCount = 0;
    while (dwExprSize != 0)
    {
        byToken = *lpabyExpr;
        if (byToken < 0x20)
        {
            if (byToken == eXLFM_STR)
            {
                // CRC the token byte

                CRC32Continue(dwCRC,byToken);
                ++dwCRCByteCount;
                ++lpabyExpr;
                if (--dwExprSize != 0)
                {
                    // Get the length byte and CRC it

                    byTemp = *lpabyExpr++;
                    CRC32Continue(dwCRC,byTemp);
                    ++dwCRCByteCount;

                    // CRC the string

                    if (--dwExprSize < byTemp)
                        byTemp = (BYTE)dwExprSize;

                    dwCRC = CRC32Compute(byTemp,
                                         lpabyExpr,
                                         dwCRC);

                    dwCRCByteCount += byTemp;
                    dwExprSize -= byTemp;
                    lpabyExpr += byTemp;
                }
            }
            else
            if (byToken == eXLFM_ATTR)
            {
                // Determine what the special attribute is

                ++lpabyExpr;
                if (--dwExprSize != 0)
                {
                    byTemp = *lpabyExpr++;
                    --dwExprSize;
                    if (byTemp == 0x04)
                    {
                        // bitFAttrChoose

                        // Just CRC the attribute byte

                        CRC32Continue(dwCRC,byTemp);
                        ++dwCRCByteCount;
                        if (dwExprSize != 0)
                        {
                            // CRC the first byte of wCase

                            byTemp = *lpabyExpr++;
                            CRC32Continue(dwCRC,byTemp);
                            ++dwCRCByteCount;
                            if (--dwExprSize != 0)
                            {
                                // CRC the second byte of wCase

                                dwTemp = byTemp;
                                byTemp = *lpabyExpr++;
                                CRC32Continue(dwCRC,byTemp);
                                ++dwCRCByteCount;
                                --dwExprSize;

                                // Form the number of cases

                                dwTemp |= (((DWORD)byTemp) << 8) & 0xFF00;

                                // Skip over the choose offsets

                                dwTemp *= sizeof(WORD);
                                if (dwExprSize < dwTemp)
                                    dwExprSize = 0;
                                else
                                {
                                    dwExprSize -= dwTemp;
                                    lpabyExpr += dwTemp;
                                }
                            }
                        }
                    }
                    else
                    {
                        if (byTemp == 0x02 || byTemp == 0x10)
                        {
                            // bitFAttrIf
                            // bitFAttrSum

                            // Just CRC the attribute byte

                            CRC32Continue(dwCRC,byTemp);
                            ++dwCRCByteCount;
                        }

                        // Skip the other fields

                        if (dwExprSize <= 2)
                            dwExprSize = 0;
                        else
                        {
                            dwExprSize -= 2;
                            lpabyExpr += 2;
                        }
                    }
                }
            }
            else
            {
                // Use the token byte skip table

                byTemp = gabyXL5FormulaTokenSkip[byToken];
                if (dwExprSize < byTemp)
                    byTemp = (BYTE)dwExprSize;

                dwCRC = CRC32Compute(byTemp,
                                     lpabyExpr,
                                     dwCRC);

                dwCRCByteCount += byTemp;
                dwExprSize -= byTemp;
                lpabyExpr += byTemp;
            }
        }
        else
        if (byToken < 0x80)
        {
            // Normalize

            byTemp = byToken & 0x1F;

            // Change the token to a non-error token

            byToken = (byToken & 0x60) | gabyXL5FormulaOpTokenChange[byTemp];

            // CRC the token

            CRC32Continue(dwCRC,byToken);
            ++dwCRCByteCount;

            if (byTemp == 0x01)
            {
                if (dwExprSize > 1)
                {
                    if (dwExprSize == 2)
                        dwTemp = 1;
                    else
                        dwTemp = 2;

                    dwCRC = CRC32Compute(dwTemp,
                                         lpabyExpr + 1,
                                         dwCRC);

                    dwCRCByteCount += dwTemp;
                }
            }
            else
            if (byTemp == 0x02)
            {
                if (dwExprSize > 1)
                {
                    if (dwExprSize == 2)
                        dwTemp = 1;
                    else
                    if (dwExprSize == 3)
                        dwTemp = 2;
                    else
                        dwTemp = 3;

                    dwCRC = CRC32Compute(dwTemp,
                                         lpabyExpr + 1,
                                         dwCRC);

                    dwCRCByteCount += dwTemp;
                }
            }

            if (dwExprSize < gabyXL5FormulaOpTokenSkip[byTemp])
                dwExprSize = 0;
            else
            {
                // Skip the token and its value

                dwExprSize -= gabyXL5FormulaOpTokenSkip[byTemp];
                lpabyExpr += gabyXL5FormulaOpTokenSkip[byTemp];
            }
        }
        else
        {
            // Skip it

            --dwExprSize;
            ++lpabyExpr;
        }
    }

    if (lpdwCRCByteCount != NULL)
        *lpdwCRCByteCount += dwCRCByteCount;

    return(dwCRC);
}


//********************************************************************
//
// Function:
//  int XL5GetOpSkip()
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

BYTE FAR gabyXL5OpSkip[XL5_OP_MAX+1] =
{
    0 * 0x80 + 0,   // 0x00 // A Imp B
    0 * 0x80 + 0,   // 0x01 // A Eqv B
    0 * 0x80 + 0,   // 0x02 // A Xor B
    0 * 0x80 + 0,   // 0x03 // A Or B
    0 * 0x80 + 0,   // 0x04 // A And B
    0 * 0x80 + 0,   // 0x05 // A = B
    0 * 0x80 + 0,   // 0x06 // A <> B
    0 * 0x80 + 0,   // 0x07 // A <= B
    0 * 0x80 + 0,   // 0x08 // A >= B
    0 * 0x80 + 0,   // 0x09 // A < B
    0 * 0x80 + 0,   // 0x0A // A > B
    0 * 0x80 + 0,   // 0x0B // A + B
    0 * 0x80 + 0,   // 0x0C // A - B
    0 * 0x80 + 0,   // 0x0D // A Mod B
    0 * 0x80 + 0,   // 0x0E // A \ B
    0 * 0x80 + 0,   // 0x0F // A * B
    0 * 0x80 + 0,   // 0x10 // A / B
    0 * 0x80 + 0,   // 0x11 // A & B
    0 * 0x80 + 0,   // 0x12 // A Like B
    0 * 0x80 + 0,   // 0x13 // A ^ B
    0 * 0x80 + 0,   // 0x14 // A Is B
    0 * 0x80 + 0,   // 0x15
    0 * 0x80 + 0,   // 0x16 // Negate
    0 * 0x80 + 0,   // 0x17
    0 * 0x80 + 0,   // 0x18
    0 * 0x80 + 0,   // 0x19
    0 * 0x80 + 0,   // 0x1A
    0 * 0x80 + 0,   // 0x1B
    0 * 0x80 + 0,   // 0x1C
    0 * 0x80 + 0,   // 0x1D
    0 * 0x80 + 0,   // 0x1E
    0 * 0x80 + 2,   // 0x1F
    0 * 0x80 + 2,   // 0x20 // Identifier
    0 * 0x80 + 2,   // 0x21 // x.Identifier
    0 * 0x80 + 2,   // 0x22 // x!Identifier
    0 * 0x80 + 2,   // 0x23 // x = y(1)(dimension)
    0 * 0x80 + 4,   // 0x24 // Identifier(dimension)
    0 * 0x80 + 4,   // 0x25 // x.Identifier(dimension)
    0 * 0x80 + 4,   // 0x26 // x!Identifier(dimension)
    0 * 0x80 + 2,   // 0x27 // Identifier = A
    0 * 0x80 + 2,   // 0x28 // x.Identifier = A
    0 * 0x80 + 2,   // 0x29 // x!Identifier = A
    0 * 0x80 + 2,   // 0x2A // x(1)(dimension) = A
    0 * 0x80 + 4,   // 0x2B // Identifier(dimension) = A
    0 * 0x80 + 4,   // 0x2C // x.Identifier(dimension) = A
    0 * 0x80 + 4,   // 0x2D // x!Identifier(dimension) = A
    0 * 0x80 + 2,   // 0x2E // Set Identifier = A
    0 * 0x80 + 2,   // 0x2F // Set x.Identifier = A
    0 * 0x80 + 2,   // 0x30 // Set x!Identifier = A
    0 * 0x80 + 2,   // 0x31 // Set x(1)(dimension) = A
    0 * 0x80 + 4,   // 0x32 // Set x(dimension) = A
    0 * 0x80 + 4,   // 0x33 // Set x.Identifier(dimension) = A
    0 * 0x80 + 4,   // 0x34 // Set x!Identifier(dimension) = A
    0 * 0x80 + 2,   // 0x35 // .Identifier
    0 * 0x80 + 2,   // 0x36 // !Identifier
    0 * 0x80 + 4,   // 0x37 // .Identifier(dimension)
    0 * 0x80 + 4,   // 0x38 // !Identifier(dimension)
    0 * 0x80 + 2,   // 0x39 // .Identifier = A
    0 * 0x80 + 2,   // 0x3A // !Identifier = A
    0 * 0x80 + 4,   // 0x3B // .Identifier(dimension) = A
    0 * 0x80 + 4,   // 0x3C // !Identifier(dimension) = A
    0 * 0x80 + 2,   // 0x3D // Set .Identifier = A
    0 * 0x80 + 2,   // 0x3E // Set !Identifier = A
    0 * 0x80 + 4,   // 0x3F // Set .Identifier(dimension) = A
    0 * 0x80 + 4,   // 0x40 // Set !Identifier(dimension) = A
    0 * 0x80 + 4,   // 0x41 // Call Identifier dimension
    0 * 0x80 + 4,   // 0x42 // Call x.Identifier dimension
    0 * 0x80 + 4,   // 0x43 // Call .Identifier dimension
    0 * 0x80 + 2,   // 0x44 // single line If ':' separator
    0 * 0x80 + 0,   // 0x45 // begin Then or Else of single line If
    0 * 0x80 + 0,   // 0x46
    0 * 0x80 + 0,   // 0x47 // Case byte 0
    0 * 0x80 + 0,   // 0x48 // A To B in Case statement
    0 * 0x80 + 0,   // 0x49
    0 * 0x80 + 0,   // 0x4A
    0 * 0x80 + 0,   // 0x4B
    0 * 0x80 + 0,   // 0x4C
    0 * 0x80 + 0,   // 0x4D
    0 * 0x80 + 0,   // 0x4E
    0 * 0x80 + 0,   // 0x4F // Case Else
    0 * 0x80 + 0,   // 0x50 // end of list in Case statement
    0 * 0x80 + 2,   // 0x51
    0 * 0x80 + 2,   // 0x52
    0 * 0x80 + 0,   // 0x53
    0 * 0x80 + 0,   // 0x54
    0 * 0x80 + 0,   // 0x55
    0 * 0x80 + 4,   // 0x56
    0 * 0x80 + 0,   // 0x57 // debug
    0 * 0x80 + 4,   // 0x58
    0 * 0x80 + 0,   // 0x59 // Dim
    0 * 0x80 + 0,   // 0x5A
    0 * 0x80 + 0,   // 0x5B // Do
    0 * 0x80 + 0,   // 0x5C
    0 * 0x80 + 0,   // 0x5D
    0 * 0x80 + 0,   // 0x5E
    0 * 0x80 + 0,   // 0x5F // Else of single line If
    0 * 0x80 + 0,   // 0x60 // Else
    0 * 0x80 + 0,   // 0x61
    0 * 0x80 + 2,   // 0x62
    0 * 0x80 + 0,   // 0x63
    0 * 0x80 + 0,   // 0x64
    0 * 0x80 + 0,   // 0x65 // End Function
    0 * 0x80 + 0,   // 0x66 // end single line If
    0 * 0x80 + 0,   // 0x67 // End If
    0 * 0x80 + 0,   // 0x68
    0 * 0x80 + 0,   // 0x69
    0 * 0x80 + 0,   // 0x6A // End Select
    0 * 0x80 + 0,   // 0x6B // End Sub
    0 * 0x80 + 0,   // 0x6C
    0 * 0x80 + 0,   // 0x6D // End With
    0 * 0x80 + 2,   // 0x6E
    0 * 0x80 + 0,   // 0x6F
    0 * 0x80 + 0,   // 0x70 // Exit Do
    0 * 0x80 + 0,   // 0x71 // Exit For
    0 * 0x80 + 0,   // 0x72 // Exit Function
    0 * 0x80 + 0,   // 0x73
    0 * 0x80 + 0,   // 0x74 // Exit Sub
    0 * 0x80 + 0,   // 0x75 // CurDir()
    0 * 0x80 + 0,   // 0x76
    0 * 0x80 + 0,   // 0x77
    0 * 0x80 + 0,   // 0x78
    0 * 0x80 + 0,   // 0x79
    0 * 0x80 + 0,   // 0x7A // Format(A,B)
    0 * 0x80 + 0,   // 0x7B
    0 * 0x80 + 0,   // 0x7C
    0 * 0x80 + 0,   // 0x7D
    0 * 0x80 + 0,   // 0x7E
    0 * 0x80 + 0,   // 0x7F
    0 * 0x80 + 0,   // 0x80
    0 * 0x80 + 0,   // 0x81
    0 * 0x80 + 2,   // 0x82
    0 * 0x80 + 0,   // 0x83
    0 * 0x80 + 0,   // 0x84
    0 * 0x80 + 0,   // 0x85
    0 * 0x80 + 0,   // 0x86
    0 * 0x80 + 0,   // 0x87
    0 * 0x80 + 0,   // 0x88
    0 * 0x80 + 2,   // 0x89
    0 * 0x80 + 0,   // 0x8A // For A = B To C
    0 * 0x80 + 0,   // 0x8B // For Each A In B
    0 * 0x80 + 2,   // 0x8C
    0 * 0x80 + 0,   // 0x8D // For A = B To C Step D
    0 * 0x80 + 2,   // 0x8E // function definition
    0 * 0x80 + 2,   // 0x8F
    0 * 0x80 + 0,   // 0x90
    0 * 0x80 + 2,   // 0x91 // GoSub Identifier
    0 * 0x80 + 2,   // 0x92 // GoTo Identifier
    0 * 0x80 + 0,   // 0x93 // If...Then...Else
    0 * 0x80 + 0,   // 0x94 // Then
    0 * 0x80 + 2,   // 0x95
    0 * 0x80 + 2,   // 0x96 unused references obsolete
    0 * 0x80 + 0,   // 0x97
    0 * 0x80 + 0,   // 0x98
    0 * 0x80 + 0,   // 0x99
    0 * 0x80 + 2,   // 0x9A // label Identifier:
    0 * 0x80 + 0,   // 0x9B
    0 * 0x80 + 2,   // 0x9C
    1 * 0x80 + 0,   // 0x9D // line continuation
    0 * 0x80 + 0,   // 0x9E
    0 * 0x80 + 2,   // 0x9F // label Identifier:
    0 * 0x80 + 8,   // 0xA0 // A@ currency
    0 * 0x80 + 8,   // 0xA1 // #A# date
    0 * 0x80 + 0,   // 0xA2 // Identifier()
    0 * 0x80 + 2,   // 0xA3 // WORD value
    0 * 0x80 + 4,   // 0xA4 // DWORD value
    0 * 0x80 + 2,   // 0xA5 // &hWORD value
    0 * 0x80 + 4,   // 0xA6 // &hDWORD value
    0 * 0x80 + 0,   // 0xA7 // Nothing
    0 * 0x80 + 2,   // 0xA8 // &oWORD value
    0 * 0x80 + 4,   // 0xA9 // &oDWORD value
    0 * 0x80 + 4,   // 0xAA // A! single precision floating point value
    0 * 0x80 + 8,   // 0xAB // double precision floating point value
    0 * 0x80 + 0,   // 0xAC
    1 * 0x80 + 0,   // 0xAD // "literal string"
    0 * 0x80 + 0,   // 0xAE // False / True if bit 10 sit
    0 * 0x80 + 0,   // 0xAF
    0 * 0x80 + 0,   // 0xB0 // Loop
    0 * 0x80 + 0,   // 0xB1
    0 * 0x80 + 0,   // 0xB2
    0 * 0x80 + 0,   // 0xB3
    0 * 0x80 + 0,   // 0xB4
    0 * 0x80 + 0,   // 0xB5
    0 * 0x80 + 6,   // 0xB6
    0 * 0x80 + 6,   // 0xB7
    0 * 0x80 + 6,   // 0xB8
    0 * 0x80 + 6,   // 0xB9
    0 * 0x80 + 0,   // 0xBA
    0 * 0x80 + 0,   // 0xBB
    0 * 0x80 + 0,   // 0xBC
    0 * 0x80 + 2,   // 0xBD
    0 * 0x80 + 0,   // 0xBE // Next
    0 * 0x80 + 0,   // 0xBF
    0 * 0x80 + 2,   // 0xC0 // On Error GoTo Identifier
    1 * 0x80 + 0,   // 0xC1
    1 * 0x80 + 0,   // 0xC2
    0 * 0x80 + 2,   // 0xC3
    0 * 0x80 + 0,   // 0xC4 // Option
    0 * 0x80 + 0,   // 0xC5 // precedes array bounds for ReDim
    0 * 0x80 + 0,   // 0xC6
    0 * 0x80 + 0,   // 0xC7
    0 * 0x80 + 2,   // 0xC8 // Assign to param by name
    0 * 0x80 + 0,   // 0xC9
    0 * 0x80 + 0,   // 0xCA
    0 * 0x80 + 0,   // 0xCB
    0 * 0x80 + 0,   // 0xCC
    0 * 0x80 + 0,   // 0xCD // print item nl
    0 * 0x80 + 0,   // 0xCE
    0 * 0x80 + 0,   // 0xCF
    0 * 0x80 + 0,   // 0xD0 // print obj
    0 * 0x80 + 0,   // 0xD1
    0 * 0x80 + 0,   // 0xD2
    0 * 0x80 + 0,   // 0xD3
    0 * 0x80 + 0,   // 0xD4
    0 * 0x80 + 2,   // 0xD5
    0 * 0x80 + 0,   // 0xD6
    1 * 0x80 + 0,   // 0xD7 // ' comment
    0 * 0x80 + 6,   // 0xD8
    0 * 0x80 + 6,   // 0xD9
    1 * 0x80 + 0,   // 0xDA
    1 * 0x80 + 0,   // 0xDB // REM comment
    0 * 0x80 + 2,   // 0xDC // Resume Identifier
    0 * 0x80 + 0,   // 0xDD
    0 * 0x80 + 0,   // 0xDE
    0 * 0x80 + 2,   // 0xDF
    0 * 0x80 + 0,   // 0xE0
    0 * 0x80 + 0,   // 0xE1 // Select Case
    0 * 0x80 + 2,   // 0xE2
    0 * 0x80 + 0,   // 0xE3
    0 * 0x80 + 0,   // 0xE4 // Set
    0 * 0x80 + 6,   // 0xE5
    0 * 0x80 + 0,   // 0xE6
    0 * 0x80 + 2,   // 0xE7
    0 * 0x80 + 0,   // 0xE8
    1 * 0x80 + 0,   // 0xE9 // variable declaration
    0 * 0x80 + 0,   // 0xEA // Wend
    0 * 0x80 + 0,   // 0xEB
    0 * 0x80 + 0,   // 0xEC // With
    0 * 0x80 + 0,   // 0xED
    0 * 0x80 + 0,   // 0xEE
    0 * 0x80 + 0,   // 0xEF
};

#ifdef SYM_WIN16
#pragma data_seg()
#endif

int XL5GetOpSkip
(
    LPBYTE      lpabyOp,
    int         nNumOpBytes
)
{
    int         nOpcode;
    WORD        wTemp;

    if (nNumOpBytes < 2)
        return(-1);

    nOpcode = lpabyOp[0];
    if (nOpcode > XL5_OP_MAX)
        nOpcode = XL5_OP_MAX;

    if (gabyXL5OpSkip[nOpcode] < 0x80)
        return(gabyXL5OpSkip[nOpcode]);

    if (nOpcode == XL5_OP_VAR_DEFN)
    {
        if (lpabyOp[1] & 0x40)
            return(4);
        else
            return(2);
    }

    if (nOpcode == XL5_OP_QUOTED_COMMENT)
    {
        if (nNumOpBytes < 6)
            return(-1);

        wTemp = ((WORD)lpabyOp[4]) | (((WORD)lpabyOp[5]) << 8);

        if (wTemp & 1)
            ++wTemp;

        return(4 + (int)wTemp);
    }
    else
    {
        if (nNumOpBytes < 4)
            return(-1);

        wTemp = ((WORD)lpabyOp[2]) | (((WORD)lpabyOp[3]) << 8);

        if (wTemp & 1)
            ++wTemp;

        return(2 + (int)wTemp);
    }
}


//********************************************************************
//
// Function:
//  BOOL XL5ModuleCRC()
//
// Parameters:
//  lpstStream          Ptr to allocated stream to CRC
//  dwTableOffset       Offset of line table
//  dwTableSize         Number of lines
//  dwModuleOffset      Offset of module binary
//  dwModuleSize        Size of module binary
//  lpdwCRC             Ptr to DWORD to store CRC of module
//  lpdwCRCByteCount    Ptr to DWORD for CRC byte count
//
// Description:
//  Given an Excel 5.0 module stream, the function calculates a
//  32-bit CRC on the token values and literal string and numeric
//  token values.
//
//  The function attempts to optimize the calculation by grouping
//  contiguous lines and continuing the CRC on the batch of lines.
//
//  The function assumes that the size of each line is a multiple
//  of two.  The function also assumes that each opcode has a value
//  less than 256.
//
//  Either or both of lpdwCRC or lpdwCRCByteCount may be NULL if
//  the corresponding value is not required.
//
// Returns:
//  TRUE            On success
//  FALSE           On error
//
//********************************************************************

typedef struct tagXL5_CRC_STATE
{
    DWORD               dwCRC;
    DWORD               dwCRCByteCount;
    EXL5_CRC_STATE_T    eState;
    BYTE                abyOp[2];
    WORD                wOpLen;
    int                 nOpcode;
    DWORD               dwCRCN;
    DWORD               dwCRCSkipN;
    DWORD               dwN;
} XL5_CRC_STATE_T, FAR *LPXL5_CRC_STATE;

//********************************************************************
//
// Function:
//  int XL5CRCBuf()
//
// Parameters:
//  lpstCRC             Ptr to CRC state structure
//  lpabyBuf            Buffer containing bytes to CRC
//  nCount              Number of bytes in buffer
//
// Description:
//  The function continues the Excel 5.0 CRC on the given buffer.
//
// Returns:
//  int             Number of bytes traversed
//
//********************************************************************

int XL5CRCBuf
(
    LPXL5_CRC_STATE     lpstCRC,
    LPBYTE              lpabyBuf,
    int                 nCount
)
{
    int                 i, j;
    XL5_CRC_STATE_T     stCRC;

    stCRC = *lpstCRC;

    i = 0;
    j = 0;
    while (i < nCount && j++ < nCount)
    {
        if (i < 0)
            return(nCount);

        switch (stCRC.eState)
        {
            case eXL5_CRC_STATE_OPCODE_LO:
                stCRC.abyOp[0] = lpabyBuf[i++];
                stCRC.eState = eXL5_CRC_STATE_OPCODE_HI;
                break;

            case eXL5_CRC_STATE_OPCODE_HI:
                stCRC.abyOp[1] = lpabyBuf[i++];
                stCRC.nOpcode = stCRC.abyOp[0];

                if (stCRC.nOpcode > XL5_OP_MAX)
                    stCRC.nOpcode = XL5_OP_MAX;

                if (gabyXL5OpSkip[stCRC.nOpcode] < 0x80)
                {
                    // CRC the opcode

                    CRC32Continue(stCRC.dwCRC,stCRC.abyOp[0]);
                    stCRC.dwCRCByteCount++;
                    CRC32Continue(stCRC.dwCRC,stCRC.abyOp[1]);
                    stCRC.dwCRCByteCount++;

                    switch (stCRC.nOpcode)
                    {
                        case XL5_OP_CURRENCY:
                        case XL5_OP_DATE:
                        case XL5_OP_WORD_VALUE:
                        case XL5_OP_DWORD_VALUE:
                        case XL5_OP_HEX_WORD_VALUE:
                        case XL5_OP_HEX_DWORD_VALUE:
                        case XL5_OP_OCT_WORD_VALUE:
                        case XL5_OP_OCT_DWORD_VALUE:
                        case XL5_OP_SINGLE_FLOAT_VALUE:
                        case XL5_OP_DOUBLE_FLOAT_VALUE:
                            stCRC.dwCRCN = gabyXL5OpSkip[stCRC.nOpcode];
                            stCRC.dwCRCSkipN = 0;
                            stCRC.eState = eXL5_CRC_STATE_CRC_N;
                            break;

                        default:
                            // Skip over any operands

                            i += gabyXL5OpSkip[stCRC.nOpcode];
                            stCRC.eState = eXL5_CRC_STATE_OPCODE_LO;
                            break;
                    }
                }
                else
                if (stCRC.nOpcode == XL5_OP_VAR_DEFN)
                {
                    // Skip variable definitions

                    if ((stCRC.abyOp[1] & 0x40) != 0)
                    {
                        // Add space for optional As column offset

                        i += 4;
                    }
                    else
                        i += 2;

                    stCRC.eState = eXL5_CRC_STATE_OPCODE_LO;
                }
                else
                {
                    // There are a variable number of arguments

                    if (stCRC.nOpcode == XL5_OP_QUOTED_COMMENT)
                    {
                        // Skip the next two bytes

                        i += 2;
                    }
                    else
                    {
                        switch (stCRC.nOpcode)
                        {
                            case XL5_OP_LINE_CONTINUATION:
                            case XL5_OP_REM_COMMENT:
                                break;

                            default:
                                // CRC the opcode

                                CRC32Continue(stCRC.dwCRC,
                                              stCRC.abyOp[0]);
                                stCRC.dwCRCByteCount++;
                                CRC32Continue(stCRC.dwCRC,
                                              stCRC.abyOp[1]);
                                stCRC.dwCRCByteCount++;
                                break;
                        }
                    }

                    // Then get the operand length

                    stCRC.eState = eXL5_CRC_STATE_OP_LEN_LO;
                }
                break;

            case eXL5_CRC_STATE_OP_LEN_LO:
                stCRC.wOpLen = lpabyBuf[i++];
                stCRC.eState = eXL5_CRC_STATE_OP_LEN_HI;
                break;

            case eXL5_CRC_STATE_OP_LEN_HI:
                stCRC.wOpLen |= (WORD)lpabyBuf[i++] << 8;

                // Round up to next even number

                stCRC.dwCRCN = stCRC.wOpLen;
                if ((stCRC.wOpLen & 1) == 0)
                    stCRC.dwCRCSkipN = 0;
                else
                    stCRC.dwCRCSkipN = 1;

                switch (stCRC.nOpcode)
                {
                    case XL5_OP_LIT_STR:
                        if (stCRC.dwCRCN == 0)
                            stCRC.eState = eXL5_CRC_STATE_OPCODE_LO;
                        else
                            stCRC.eState = eXL5_CRC_STATE_CRC_N;
                        break;

                    default:
                        i += (int)(stCRC.dwCRCN + stCRC.dwCRCSkipN);
                        stCRC.eState = eXL5_CRC_STATE_OPCODE_LO;
                        break;
                }

                break;

            case eXL5_CRC_STATE_CRC_N:
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
                    stCRC.eState = eXL5_CRC_STATE_OPCODE_LO;
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
//  BOOL XL5ModuleCRCFast()
//
// Parameters:
//  lpstStream          Ptr to allocated stream to CRC
//  dwTableOffset       Offset of line table
//  dwTableSize         Number of lines
//  dwModuleOffset      Offset of module binary
//  dwModuleSize        Size of module binary
//  lpabyBuf            Buffer to use
//  nBufSize            Size of buffer
//  lpfnCB              Callback function
//  lpvCBCookie         Callback cookie
//  lpdwCRC             Ptr to DWORD to store CRC of module
//  lpdwCRCByteCount    Ptr to DWORD for CRC byte count
//
// Description:
//  Given an Excel 5.0 module stream, the function calculates a
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
// Returns:
//  TRUE            On success
//  FALSE           On error
//
//********************************************************************

#define XL5_CRC_MAX_GROUP_COUNT     256

BOOL XL5ModuleCRCFast
(
    LPSS_STREAM         lpstStream,
    DWORD               dwTableOffset,
    DWORD               dwTableSize,
    DWORD               dwModuleOffset,
    DWORD               dwModuleSize,
    LPBYTE              lpabyBuf,
    int                 nBufSize,
    LPFNXL5_CRC_FAST_CB lpfnCB,
    LPVOID              lpvCBCookie,
    LPDWORD             lpdwCRC,
    LPDWORD             lpdwCRCByteCount
)
{
    LPXL5_LINE_ENTRY    lpstLine;
    int                 nMaxBufLines;
    WORD                wSize;
    WORD                wOffset;

    WORD                wStartOffset;
    WORD                wEndOffset;

    int                 i;
    int                 nNumGroups;
    WORD                awOffsets[XL5_CRC_MAX_GROUP_COUNT];
    WORD                awSizes[XL5_CRC_MAX_GROUP_COUNT];

    DWORD               dwLine;
    DWORD               dwCount;
    DWORD               dwOffset;
    DWORD               dwEndOffset;
    int                 nStartI;

    DWORD               dwBytesRead;

    XL5_CRC_STATE_T     stCRC;
    EXL5_CRC_STATE_T    eState;

    (void)dwModuleSize;

    nMaxBufLines = nBufSize / sizeof(XL5_LINE_ENTRY_T);
    if (nMaxBufLines == 0 || lpabyBuf == NULL)
        return(FALSE);

    CRC32Init(stCRC.dwCRC);
    stCRC.dwCRCByteCount = 0;
    dwLine = 0;
    while (dwLine < dwTableSize)
    {
        /////////////////////////////////////////////////////////
        // Get up to 256 groups of contiguous lines

        nNumGroups = 0;
        wEndOffset = 0;
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
                                   dwLine * sizeof(XL5_LINE_ENTRY_T),
                               lpabyBuf,
                               dwCount * sizeof(XL5_LINE_ENTRY_T),
                               &dwBytesRead) != SS_STATUS_OK ||
                    dwBytesRead != dwCount * sizeof(XL5_LINE_ENTRY_T))
                    return(FALSE);

                lpstLine = (LPXL5_LINE_ENTRY)lpabyBuf;
            }

            wSize = lpstLine->wSize;
            wSize = WENDIAN(wSize);

            // Ignore blank lines

            if (wSize != 0)
            {
                wOffset = lpstLine->wOffset;
                wOffset = WENDIAN(wOffset);
                if (wEndOffset == 0)
                {
                    // This is the first line in the group

                    wStartOffset = wOffset;
                    wEndOffset = wOffset + wSize;
                }
                else
                if (wEndOffset == wOffset)
                {
                    // The line is contiguous with the previous line

                    wEndOffset += wSize;
                }
                else
                {
                    // This line is not contiguous with the previous line

                    // Store the offset and size of the group

                    awOffsets[nNumGroups] = wStartOffset;
                    awSizes[nNumGroups] = wEndOffset - wStartOffset;

                    wEndOffset = 0;

                    // Have the maximum number of groups been reached

                    if (++nNumGroups == XL5_CRC_MAX_GROUP_COUNT)
                        break;

                    // Start a new group

                    wStartOffset = wOffset;
                    wEndOffset = wOffset + wSize;
                }
            }

            // Move to the next line

            ++dwLine;
            --dwCount;
            ++lpstLine;
        }

        // Store the hanging group

        if (wEndOffset != 0)
        {
            awOffsets[nNumGroups] = wStartOffset;
            awSizes[nNumGroups] = wEndOffset - wStartOffset;
            ++nNumGroups;
        }

        /////////////////////////////////////////////////////////
        // CRC the groups of contiguous lines

        i = 0;
        while (i < nNumGroups)
        {
            // Get a set of groups that fit within the buffer

            nStartI = i;
            dwOffset = awOffsets[i];
            dwEndOffset = dwOffset + awSizes[i];
            dwCount = awSizes[i];
            ++i;
            while (i < nNumGroups)
            {
                wOffset = awOffsets[i];
                wSize = awSizes[i];
                if (wOffset < dwOffset)
                {
                    if (wOffset + wSize > dwEndOffset)
                    {
                        // This is weird

                        break;
                    }

                    // Will the buffer limit be exceeded?

                    if ((int)(dwEndOffset - wOffset) > nBufSize)
                        break;

                    // Set the start offset to this line

                    dwOffset = wOffset;
                }
                else
                if (wOffset + wSize > dwEndOffset)
                {
                    // Will adding this group exceed the buffer size?

                    if (wOffset + wSize - dwOffset > nBufSize)
                        break;

                    dwEndOffset = wOffset + wSize;
                }

                ++i;
            }

            if (nStartI + 1 == i)
            {
                // There is only one group

                stCRC.eState = eState = eXL5_CRC_STATE_OPCODE_LO;
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
                                   lpabyBuf,
                                   (int)dwCount) == FALSE)
                            return(FALSE);
                    }

                    // CRC the chunk and increment the offset

                    dwOffset += XL5CRCBuf(&stCRC,
                                          lpabyBuf,
                                          (int)dwCount);
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
                    stCRC.eState = eState = eXL5_CRC_STATE_OPCODE_LO;

                    // Verify that the chunk is within the buffer

                    if (awOffsets[nStartI] - dwOffset >= dwCount)
                        return(FALSE);

                    if ((dwCount - (awOffsets[nStartI] - dwOffset)) <
                        awSizes[nStartI])
                        return(FALSE);


                    // Call the callback if necessary

                    if (lpfnCB != NULL)
                    {
                        if (lpfnCB(lpvCBCookie,
                                   &eState,
                                   lpabyBuf + awOffsets[nStartI] - dwOffset,
                                   (int)awSizes[nStartI]) == FALSE)
                            return(FALSE);
                    }

                    // CRC the group

                    XL5CRCBuf(&stCRC,
                              lpabyBuf + awOffsets[nStartI] - dwOffset,
                              awSizes[nStartI]);

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
//  BOOL XL5ModuleCRC()
//
// Parameters:
//  lpstStream          Ptr to allocated stream to CRC
//  dwTableOffset       Offset of line table
//  dwTableSize         Number of lines
//  dwModuleOffset      Offset of module binary
//  dwModuleSize        Size of module binary
//  lpdwCRC             Ptr to DWORD to store CRC of module
//  lpdwCRCByteCount    Ptr to DWORD for CRC byte count
//
// Description:
//  Given an Excel 5.0 module stream, the function calculates a
//  32-bit CRC on the token values and literal string and numeric
//  token values.
//
//  The function attempts to optimize the calculation by grouping
//  contiguous lines and continuing the CRC on the batch of lines.
//
//  The function assumes that the size of each line is a multiple
//  of two.  The function also assumes that each opcode has a value
//  less than 256.
//
//  Either or both of lpdwCRC or lpdwCRCByteCount may be NULL if
//  the corresponding value is not required.
//
// Returns:
//  TRUE            On success
//  FALSE           On error
//
//********************************************************************

BOOL XL5ModuleCRC
(
    LPSS_STREAM         lpstStream,
    DWORD               dwTableOffset,
    DWORD               dwTableSize,
    DWORD               dwModuleOffset,
    DWORD               dwModuleSize,
    LPDWORD             lpdwCRC,
    LPDWORD             lpdwCRCByteCount
)
{
    DWORD               dwLine;
    XL5_LINE_ENTRY_T    stLine;
    BYTE                abyBuf[512];
    DWORD               dwOffset;
    DWORD               dwEndOffset;
    DWORD               dwCount;
    DWORD               dwBytesRead;

    XL5_CRC_STATE_T     stCRC;

    (void)dwModuleSize;

    CRC32Init(stCRC.dwCRC);
    stCRC.dwCRCByteCount = 0;
    dwLine = 0;
    while (dwLine < dwTableSize)
    {
        /////////////////////////////////////////////////////////
        // Get a group of contiguous lines

        dwEndOffset = 0;
        while (dwLine < dwTableSize)
        {
            // Read a line entry

            if (SSSeekRead(lpstStream,
                           dwTableOffset,
                           &stLine,
                           sizeof(XL5_LINE_ENTRY_T),
                           &dwBytesRead) != SS_STATUS_OK ||
                dwBytesRead != sizeof(XL5_LINE_ENTRY_T))
                return(FALSE);

            stLine.wSize = WENDIAN(stLine.wSize);
            if (stLine.wSize != 0)
            {
                stLine.wOffset = WENDIAN(stLine.wOffset);
                if (dwEndOffset == 0)
                {
                    // This is the first line in the group

                    dwOffset = stLine.wOffset;
                    dwEndOffset = dwOffset + stLine.wSize;
                }
                else
                if (dwEndOffset != stLine.wOffset)
                {
                    // This line is not contiguous with the previous line

                    break;
                }
                else
                {
                    // The line is contiguous with the previous line

                    dwEndOffset += stLine.wSize;
                }
            }

            ++dwLine;
            dwTableOffset += sizeof(XL5_LINE_ENTRY_T);
        }

        /////////////////////////////////////////////////////////
        // CRC the group of contiguous lines

        stCRC.eState = eXL5_CRC_STATE_OPCODE_LO;
        dwOffset += dwModuleOffset;
        dwEndOffset += dwModuleOffset;
        dwCount = sizeof(abyBuf);
        while (dwOffset < dwEndOffset)
        {
            if (dwEndOffset - dwOffset < dwCount)
                dwCount = dwEndOffset - dwOffset;

            // Read a chunk

            if (SSSeekRead(lpstStream,
                           dwOffset,
                           abyBuf,
                           dwCount,
                           &dwBytesRead) != SS_STATUS_OK ||
                dwBytesRead != dwCount)
                return(FALSE);

            // CRC the chunk

            dwOffset += XL5CRCBuf(&stCRC,
                                  abyBuf,
                                  (int)dwCount);
        }
    }

    if (lpdwCRC != NULL)
        *lpdwCRC = stCRC.dwCRC;

    if (lpdwCRCByteCount != NULL)
        *lpdwCRCByteCount = stCRC.dwCRCByteCount;

    return(TRUE);
}


//*************************************************************************
//
// Function:
//  BOOL XLSetSheetTabSelectState()
//
// Parameters:
//  lpstStream              Ptr to Book stream
//  lpstKey                 Ptr to decryption key
//  dwOffset                Offset of sheet BOF record
//  bSelected               BOOL specifying selection state
//
// Description:
//  The function iterates through the records of the given sheet
//  looking for the Sheet Window Information record.  It sets the
//  records selection bit to the state given by bSelected.
//
//  The function will work for Excel 5.0, 95, and 97 documents.
//
// Returns:
//  TRUE                    If successful
//  FALSE                   On error
//
//*************************************************************************

BOOL XLSetSheetTabSelectState
(
    LPSS_STREAM             lpstStream,
    LPXL5ENCKEY             lpstKey,
    DWORD                   dwOffset,
    BOOL                    bSelected
)
{
    XL_REC_HDR_T            stRec;
    DWORD                   dwMaxOffset;
    DWORD                   dwBytesRead;
    XL_REC_WINDOW2_T        stWindowInfo;
    DWORD                   dwBOFDepth;

    dwBOFDepth = 0;
    dwMaxOffset = SSStreamLen(lpstStream);
    while (dwOffset < dwMaxOffset)
    {
        if (SSSeekRead(lpstStream,
                       dwOffset,
                       &stRec,
                       sizeof(XL_REC_HDR_T),
                       &dwBytesRead) != SS_STATUS_OK ||
            dwBytesRead != sizeof(XL_REC_HDR_T))
        {
            // Failed to read record

            return(FALSE);
        }

        stRec.wType = WENDIAN(stRec.wType);
        stRec.wLen = WENDIAN(stRec.wLen);

        if (stRec.wType == eXLREC_WINDOW2 ||
            stRec.wType == eXLREC_WINDOW22)
        {
            // Read the Sheet Window Information record

            if (XL5EncryptedRead(lpstStream,
                                 dwOffset,
                                 stRec,
                                 lpstKey,
                                 0,
                                 (LPBYTE)&stWindowInfo,
                                 sizeof(XL_REC_WINDOW2_T)) == FALSE)
                return(FALSE);

            // Set the selected state

            if (bSelected == FALSE)
                stWindowInfo.byFlags1 &= ~(XL_REC_WINDOW2_F1_SELECTED);
            else
                stWindowInfo.byFlags1 |= XL_REC_WINDOW2_F1_SELECTED;

            // Write out the new record

            if (XL5EncryptedWrite(lpstStream,
                                  dwOffset,
                                  stRec,
                                  lpstKey,
                                  0,
                                  (LPBYTE)&stWindowInfo,
                                  sizeof(XL_REC_WINDOW2_T)) == FALSE)
                return(FALSE);

            return(TRUE);
        }
        else
        if (stRec.wType == eXLREC_BOF)
            ++dwBOFDepth;
        else
        if (stRec.wType == eXLREC_EOF && dwBOFDepth-- <= 1)
            break;

        dwOffset += sizeof(XL_REC_HDR_T) + stRec.wLen;
    }

    // Couldn't find the Sheet Window Information record

    return(FALSE);
}


//*************************************************************************
//
// Function:
//  BOOL XLSetStandardSheetFlags()
//
// Parameters:
//  lpstStream              Ptr to Book stream
//  lpstKey                 Ptr to decryption key
//  dwOffset                Offset of sheet BOF record
//
// Description:
//  The function iterates through the records of the given sheet
//  looking for the Sheet Window Information record.  The option flags
//  are modified to that of a standard sheet.  The flags which are
//  set to true are the following:
//
//      XL_REC_WINDOW2_F0_DSP_GRID      0x02
//      XL_REC_WINDOW2_F0_DSP_RW_COL    0x04
//      XL_REC_WINDOW2_F0_DSP_ZEROES    0x10
//      XL_REC_WINDOW2_F0_DEFAULT_HDR   0x20
//      XL_REC_WINDOW2_F0_DSP_GUTS      0x80
//
//      XL_REC_WINDOW2_F1_PAGED         0x04
//
//  All other bits except for XL_REC_WINDOW2_F1_SELECTED are set
//  to false.  The XL_REC_WINDOW2_F1_SELECTED flag is left as is.
//
//  The remaining fields of the record are set to zero.
//
//  The function will work for Excel 5.0, 95, and 97 documents.
//
// Returns:
//  TRUE                    If successful
//  FALSE                   On error
//
//*************************************************************************

BOOL XLSetStandardSheetFlags
(
    LPSS_STREAM             lpstStream,
    LPXL5ENCKEY             lpstKey,
    DWORD                   dwOffset
)
{
    XL_REC_HDR_T            stRec;
    DWORD                   dwMaxOffset;
    DWORD                   dwBytesRead;
    XL_REC_WINDOW2_T        stWindowInfo;
    DWORD                   dwBOFDepth;

    dwBOFDepth = 0;
    dwMaxOffset = SSStreamLen(lpstStream);
    while (dwOffset < dwMaxOffset)
    {
        if (SSSeekRead(lpstStream,
                       dwOffset,
                       &stRec,
                       sizeof(XL_REC_HDR_T),
                       &dwBytesRead) != SS_STATUS_OK ||
            dwBytesRead != sizeof(XL_REC_HDR_T))
        {
            // Failed to read record

            return(FALSE);
        }

        stRec.wType = WENDIAN(stRec.wType);
        stRec.wLen = WENDIAN(stRec.wLen);

        if (stRec.wType == eXLREC_WINDOW2 ||
            stRec.wType == eXLREC_WINDOW22)
        {
            // Read the Sheet Window Information record

            if (XL5EncryptedRead(lpstStream,
                                 dwOffset,
                                 stRec,
                                 lpstKey,
                                 0,
                                 (LPBYTE)&stWindowInfo,
                                 sizeof(XL_REC_WINDOW2_T)) == FALSE)
                return(FALSE);

            // Set the first option flag byte

            stWindowInfo.byFlags0 =
                XL_REC_WINDOW2_F0_DSP_GRID |
                XL_REC_WINDOW2_F0_DSP_RW_COL |
                XL_REC_WINDOW2_F0_DSP_ZEROES |
                XL_REC_WINDOW2_F0_DEFAULT_HDR |
                XL_REC_WINDOW2_F0_DSP_GUTS;

            // Set the second option flag byte

            stWindowInfo.byFlags1 =
                (stWindowInfo.byFlags1 & XL_REC_WINDOW2_F1_SELECTED) |
                XL_REC_WINDOW2_F1_PAGED;

            // Zero out the remaining values of the record

            stWindowInfo.wTopRow = 0;
            stWindowInfo.wLeftCol = 0;
            stWindowInfo.dwRGBHdr = 0;

            // Write out the new record

            if (XL5EncryptedWrite(lpstStream,
                                  dwOffset,
                                  stRec,
                                  lpstKey,
                                  0,
                                  (LPBYTE)&stWindowInfo,
                                  sizeof(XL_REC_WINDOW2_T)) == FALSE)
                return(FALSE);

            return(TRUE);
        }
        else
        if (stRec.wType == eXLREC_BOF)
            ++dwBOFDepth;
        else
        if (stRec.wType == eXLREC_EOF && dwBOFDepth-- <= 1)
            break;

        dwOffset += sizeof(XL_REC_HDR_T) + stRec.wLen;
    }

    // Couldn't find the Sheet Window Information record

    return(FALSE);
}


//*************************************************************************
//
// Function:
//  BOOL XLMakeWindowInfoValid()
//
// Parameters:
//  lpstStream              Ptr to Book stream
//  lpstKey                 Ptr to decryption key
//
// Description:
//  The function puts the tab selection state and the first displayed
//  workbook tab in a valid state.  The state is valid if there is at
//  least one visible tab, the first displayed tab is visible, and
//  the first selected tab is visible.  The function does this using
//  the following steps:
//
//  1. Perform a scan through the workbook global records:
//      a. Read the Window Information record and note its offset
//      b. Count the number of Sheet Information records
//      c. Note the offset of the first Sheet Information record
//      d. Note the offset and index of the first Sheet Information
//          record with a hidden state of visible
//
//  2. If the Window Information record was not found or no
//      Sheet Information records were found, then return error.
//
//  3. If no sheets are visible, then make the first sheet visible.
//
//  4. Go through the Sheet Window Information records of each sheet
//      and set the selected state to false, except for the first
//      visible sheet, whose selected state is set to true.
//
//  5. In the Window Information record, set the index of the selected
//      workbook tab and the index of the first displayed workbook tab
//      to the index of the first visible sheet.  Set the number of
//      tabs selected to one.
//
//  The function will work for Excel 5.0, 95, and 97 documents.
//
// Returns:
//  TRUE                    If the workbook is now in a valid state
//  FALSE                   On error
//
//*************************************************************************

BOOL XLMakeWindowInfoValid
(
    LPSS_STREAM             lpstStream,
    LPXL5ENCKEY             lpstKey
)
{
    DWORD                   dwOffset;
    DWORD                   dwMaxOffset;
    XL_REC_HDR_T            stRec;
    XL_REC_BOUNDSHEET_T     stBoundSheet;
    DWORD                   dwWindowInfoOffset;
    XL_REC_WINDOW1          stWindowInfo;
    DWORD                   dwBytesRead;
    DWORD                   dwFirstBoundSheetOffset;
    DWORD                   dwFirstVisibleBoundSheetOffset;
    WORD                    wSheetIndex;
    WORD                    wFirstVisibleSheetIndex;

    dwMaxOffset = SSStreamLen(lpstStream);

    dwWindowInfoOffset = 0;

    dwFirstBoundSheetOffset = 0;
    dwFirstVisibleBoundSheetOffset = 0;

    wSheetIndex = 0;

    //////////////////////////////////////////////////////////////////
    // Perform a scan through the workbook global records

    dwOffset = 0;
    while (dwOffset < dwMaxOffset)
    {
        if (SSSeekRead(lpstStream,
                       dwOffset,
                       &stRec,
                       sizeof(XL_REC_HDR_T),
                       &dwBytesRead) != SS_STATUS_OK ||
            dwBytesRead != sizeof(XL_REC_HDR_T))
        {
            // Failed to read record

            return(FALSE);
        }

        stRec.wType = WENDIAN(stRec.wType);
        stRec.wLen = WENDIAN(stRec.wLen);

        if (stRec.wType == eXLREC_WINDOW1)
        {
            // Note the Window Information record offset

            dwWindowInfoOffset = dwOffset;
        }
        else
        if (stRec.wType == eXLREC_BOUNDSHEET)
        {
            // Note the offset of the first Sheet Information record

            if (dwFirstBoundSheetOffset == 0)
                dwFirstBoundSheetOffset = dwOffset;

            // Read the BOUNDSHEET structure

            if (XL5EncryptedRead(lpstStream,
                                 dwOffset,
                                 stRec,
                                 lpstKey,
                                 0,
                                 (LPBYTE)&stBoundSheet,
                                 sizeof(XL_REC_BOUNDSHEET_T)) == FALSE)
                return(FALSE);

            // Note the offset and index of the first Sheet Information
            //  record with a hidden state of visible

            if (dwFirstVisibleBoundSheetOffset == 0 &&
                (stBoundSheet.byHiddenState & XL_REC_BOUNDSHEET_STATE_MASK) ==
                XL_REC_BOUNDSHEET_VISIBLE)
            {
                dwFirstVisibleBoundSheetOffset = dwOffset;
                wFirstVisibleSheetIndex = wSheetIndex;
            }

            ++wSheetIndex;
        }
        else
        if (stRec.wType == eXLREC_EOF)
            break;

        dwOffset += sizeof(XL_REC_HDR_T) + stRec.wLen;
    }

    // If the Window Information record was not found or no
    //  Sheet Information records were found, then return error.

    if (dwWindowInfoOffset == 0 || wSheetIndex == 0)
        return(FALSE);


    //////////////////////////////////////////////////////////////////
    // If no sheets are visible, then make the first sheet visible.

    if (dwFirstVisibleBoundSheetOffset == 0)
    {
        // Read the record header of the first BOUNDSHEET

        if (SSSeekRead(lpstStream,
                       dwFirstBoundSheetOffset,
                       &stRec,
                       sizeof(XL_REC_HDR_T),
                       &dwBytesRead) != SS_STATUS_OK ||
            dwBytesRead != sizeof(XL_REC_HDR_T))
        {
            // Failed to read record

            return(FALSE);
        }

        stRec.wType = WENDIAN(stRec.wType);
        stRec.wLen = WENDIAN(stRec.wLen);

        // Read the BOUNDSHEET structure

        if (XL5EncryptedRead(lpstStream,
                             dwFirstBoundSheetOffset,
                             stRec,
                             lpstKey,
                             0,
                             (LPBYTE)&stBoundSheet,
                             sizeof(XL_REC_BOUNDSHEET_T)) == FALSE)
            return(FALSE);

        // Make the sheet visible

        stBoundSheet.byHiddenState =
            (stBoundSheet.byHiddenState & ~(XL_REC_BOUNDSHEET_STATE_MASK)) |
            XL_REC_BOUNDSHEET_VISIBLE;

        // Write the updated BOUNDSHEET structure

        if (XL5EncryptedWrite(lpstStream,
                              dwFirstBoundSheetOffset,
                              stRec,
                              lpstKey,
                              0,
                              (LPBYTE)&stBoundSheet,
                              sizeof(XL_REC_BOUNDSHEET_T)) == FALSE)
            return(FALSE);

        // Set the offset of the first visible bound sheet

        dwFirstVisibleBoundSheetOffset = dwFirstBoundSheetOffset;
        wFirstVisibleSheetIndex = 0;
    }


    //////////////////////////////////////////////////////////////////
    // Go through the Sheet Window Information records of each sheet
    //  and set the selected state to false, except for the first
    //  visible sheet, whose selected state is set to true.

    dwOffset = dwFirstBoundSheetOffset;
    while (dwOffset < dwMaxOffset)
    {
        if (SSSeekRead(lpstStream,
                       dwOffset,
                       &stRec,
                       sizeof(XL_REC_HDR_T),
                       &dwBytesRead) != SS_STATUS_OK ||
            dwBytesRead != sizeof(XL_REC_HDR_T))
        {
            // Failed to read record

            return(FALSE);
        }

        stRec.wType = WENDIAN(stRec.wType);
        stRec.wLen = WENDIAN(stRec.wLen);

        if (stRec.wType == eXLREC_BOUNDSHEET)
        {
            BOOL bSelected;

            // Note the offset of the first Sheet Information record

            if (dwFirstBoundSheetOffset == 0)
                dwFirstBoundSheetOffset = dwOffset;

            // Read the BOUNDSHEET structure

            if (XL5EncryptedRead(lpstStream,
                                 dwOffset,
                                 stRec,
                                 lpstKey,
                                 0,
                                 (LPBYTE)&stBoundSheet,
                                 sizeof(XL_REC_BOUNDSHEET_T)) == FALSE)
                return(FALSE);

            stBoundSheet.dwOffset = DWENDIAN(stBoundSheet.dwOffset);

            if (dwOffset == dwFirstVisibleBoundSheetOffset)
                bSelected = TRUE;
            else
                bSelected = FALSE;

            // Set the selection state

            if (XLSetSheetTabSelectState(lpstStream,
                                         lpstKey,
                                         stBoundSheet.dwOffset,
                                         bSelected) == FALSE)
                return(FALSE);
        }
        else
        if (stRec.wType == eXLREC_EOF)
            break;

        dwOffset += sizeof(XL_REC_HDR_T) + stRec.wLen;
    }


    //////////////////////////////////////////////////////////////////
    // In the Window Information record, set the index of the selected
    //  workbook tab and the index of the first displayed workbook tab
    //  to the index of the first visible sheet.  Set the number of
    //  tabs selected to one.

    if (SSSeekRead(lpstStream,
                   dwWindowInfoOffset,
                   &stRec,
                   sizeof(XL_REC_HDR_T),
                   &dwBytesRead) != SS_STATUS_OK ||
        dwBytesRead != sizeof(XL_REC_HDR_T))
    {
        // Failed to read record

        return(FALSE);
    }

    stRec.wType = WENDIAN(stRec.wType);
    stRec.wLen = WENDIAN(stRec.wLen);

    // Read the Window Information record

    if (XL5EncryptedRead(lpstStream,
                         dwWindowInfoOffset,
                         stRec,
                         lpstKey,
                         0,
                         (LPBYTE)&stWindowInfo,
                         sizeof(XL_REC_WINDOW1)) == FALSE)
        return(FALSE);

    // Set the visible, selection, and selection count fields

    stWindowInfo.wTabCur = WENDIAN(wFirstVisibleSheetIndex);
    stWindowInfo.wTabFirst = WENDIAN(wFirstVisibleSheetIndex);
    stWindowInfo.wCountTabSel = WENDIAN(1);

    // Write the Window Information record

    if (XL5EncryptedWrite(lpstStream,
                          dwWindowInfoOffset,
                          stRec,
                          lpstKey,
                          0,
                          (LPBYTE)&stWindowInfo,
                          sizeof(XL_REC_WINDOW1)) == FALSE)
        return(FALSE);

    // Success

    return(TRUE);
}


//*************************************************************************
//
// Function:
//  BOOL XLRemoveUnusedVBProj()
//
// Parameters:
//  lpstStream              Ptr to Book stream
//  lpstKey                 Ptr to decryption key
//
// Description:
//  The function removes the Visual Basic Project record
//  if it is present and there are no sheets marked as Visual Basic
//  modules.
//
//  The function sets the Visual Basic Project record to be
//  of type CONTINUE and zeroes out the contents.
//
//  The function will work for Excel 5.0, 95, and 97 documents.
//
// Returns:
//  TRUE                    On success
//  FALSE                   On error
//
//*************************************************************************

BOOL XLRemoveUnusedVBProj
(
    LPSS_STREAM             lpstStream,
    LPXL5ENCKEY             lpstKey
)
{
    DWORD                   dwOffset;
    DWORD                   dwMaxOffset;
    XL_REC_HDR_T            stRec;
    DWORD                   dwBytesRead;
    XL_REC_BOUNDSHEET_T     stBoundSheet;
    XL_REC_HDR_T            stVBProjRecHdr;
    DWORD                   dwVBProjOffset;
    DWORD                   dwVBModuleSheetCount;

    dwVBProjOffset = 0;
    dwVBModuleSheetCount = 0;

    //////////////////////////////////////////////////////////////////
    // Perform a scan through the workbook global records

    dwOffset = 0;
    dwMaxOffset = SSStreamLen(lpstStream);
    while (dwOffset < dwMaxOffset)
    {
        if (SSSeekRead(lpstStream,
                       dwOffset,
                       &stRec,
                       sizeof(XL_REC_HDR_T),
                       &dwBytesRead) != SS_STATUS_OK ||
            dwBytesRead != sizeof(XL_REC_HDR_T))
        {
            // Failed to read record

            return(FALSE);
        }

        stRec.wType = WENDIAN(stRec.wType);
        stRec.wLen = WENDIAN(stRec.wLen);

        if (stRec.wType == eXLREC_OBPROJ)
        {
            // Note the record offset

            dwVBProjOffset = dwOffset;

            stVBProjRecHdr = stRec;
        }
        else
        if (stRec.wType == eXLREC_BOUNDSHEET)
        {
            // Read the BOUNDSHEET structure

            if (XL5EncryptedRead(lpstStream,
                                 dwOffset,
                                 stRec,
                                 lpstKey,
                                 0,
                                 (LPBYTE)&stBoundSheet,
                                 sizeof(XL_REC_BOUNDSHEET_T)) == FALSE)
                return(FALSE);

            // Is it a Visual Basic module sheet?

            if ((stBoundSheet.bySheetType & XL_REC_BOUNDSHEET_TYPE_MASK) ==
                XL_REC_BOUNDSHEET_VB_MODULE)
                ++dwVBModuleSheetCount;
        }
        else
        if (stRec.wType == eXLREC_EOF)
            break;

        dwOffset += sizeof(XL_REC_HDR_T) + stRec.wLen;
    }


    //////////////////////////////////////////////////////////////////
    // Remove the Visual Basic Project record if applicable

    if (dwVBProjOffset != 0 && dwVBModuleSheetCount == 0)
    {
        WORD    wRecSize;

        // Set the Visual Basic Project record to a continue record

        wRecSize = stVBProjRecHdr.wLen;
        stVBProjRecHdr.wType = eXLREC_CONTINUE;
        stRec = stVBProjRecHdr;
        stVBProjRecHdr.wType = WENDIAN(stVBProjRecHdr.wType);
        stVBProjRecHdr.wLen = WENDIAN(stVBProjRecHdr.wLen);
        if (SSSeekWrite(lpstStream,
                        dwVBProjOffset,
                        &stVBProjRecHdr,
                        sizeof(XL_REC_HDR_T),
                        &dwBytesRead) != SS_STATUS_OK ||
            dwBytesRead != sizeof(XL_REC_HDR_T))
        {
            // Failed to write record header

            return(FALSE);
        }

        // Zero out the contents of the record

        if (XL5EncryptedWriteRepeat(lpstStream,
                                    dwVBProjOffset,
                                    stRec,
                                    lpstKey,
                                    0,
                                    0,
                                    wRecSize) == FALSE)
            return(FALSE);
    }

    return(TRUE);
}


//*************************************************************************
//
// Function:
//  BOOL XLEraseNameRecords()
//
// Parameters:
//  lpstStream              Ptr to Book stream
//  lpstKey                 Ptr to decryption key
//  wIndex                  Zero-based sheet index to use for search
//
// Description:
//  The function invalidates all NAME records referring to the
//  given index.  Although the NAME records use a one-based index,
//  the function will account for this.
//
//  The function accomplishes this by setting all fields of the NAME
//  record to zero.  The ixals and itab fields are left as is.
//
//  The function will work for Excel 5.0, 95, and 97 documents.
//
// Returns:
//  TRUE                    On success
//  FALSE                   On error
//
//*************************************************************************

BOOL XLEraseNameRecords
(
    LPSS_STREAM             lpstStream,
    LPXL5ENCKEY             lpstKey,
    WORD                    wIndex
)
{
    DWORD                   dwOffset;
    DWORD                   dwMaxOffset;
    XL_REC_HDR_T            stRec;
    DWORD                   dwBytesRead;
    XL_REC_NAME_T           stName;

    //////////////////////////////////////////////////////////////////
    // Perform a scan through the workbook global records
    //  for desired NAME records

    // The index is one based

    ++wIndex;

    // Endianize the index

    wIndex = WENDIAN(wIndex);

    dwOffset = 0;
    dwMaxOffset = SSStreamLen(lpstStream);
    while (dwOffset < dwMaxOffset)
    {
        if (SSSeekRead(lpstStream,
                       dwOffset,
                       &stRec,
                       sizeof(XL_REC_HDR_T),
                       &dwBytesRead) != SS_STATUS_OK ||
            dwBytesRead != sizeof(XL_REC_HDR_T))
        {
            // Failed to read record

            return(FALSE);
        }

        stRec.wType = WENDIAN(stRec.wType);
        stRec.wLen = WENDIAN(stRec.wLen);

        if (stRec.wType == eXLREC_NAME ||
            stRec.wType == eXLREC_NAME2)
        {
            // Read the NAME record

            if (XL5EncryptedRead(lpstStream,
                                 dwOffset,
                                 stRec,
                                 lpstKey,
                                 0,
                                 (LPBYTE)&stName,
                                 sizeof(XL_REC_NAME_T)) == FALSE)
                return(FALSE);

            if (stName.wSheetIndex2 == wIndex)
            {
                // Set all fields except the index fields to zero

                stName.wFlags = 0;
                stName.byKey = 0;
                stName.byNameLen = 0;
                stName.wDefLen = 0;
                stName.byMenuLen = 0;
                stName.byDescLen = 0;
                stName.byHelpLen = 0;
                stName.byStatusLen = 0;

                // Write it back out

                if (XL5EncryptedWrite(lpstStream,
                                      dwOffset,
                                      stRec,
                                      lpstKey,
                                      0,
                                      (LPBYTE)&stName,
                                      sizeof(XL_REC_NAME_T)) == FALSE)
                    return(FALSE);

                // Zero out the rest of the record

                if (stRec.wLen > sizeof(XL_REC_NAME_T))
                {
                    if (XL5EncryptedWriteRepeat(lpstStream,
                                                dwOffset,
                                                stRec,
                                                lpstKey,
                                                sizeof(XL_REC_NAME_T),
                                                0,
                                                stRec.wLen -
                                                    sizeof(XL_REC_NAME_T)) ==
                        FALSE)
                        return(FALSE);
                }
            }
        }
        else
        if (stRec.wType == eXLREC_EOF)
            break;

        dwOffset += sizeof(XL_REC_HDR_T) + stRec.wLen;
    }

    return(TRUE);
}


//*************************************************************************
//
// Function:
//  BOOL XLBlankAllCellValues()
//
// Parameters:
//  lpstStream              Ptr to Book stream
//  lpstKey                 Ptr to decryption key
//  dwOffset                Offset of BOF record for sheet
//
// Description:
//  The function replaces the following cell record types with the
//  BLANK type and the original cell contents zeroed out:
//
//          eXLREC_BOOLERR
//          eXLREC_BOOLERR2
//          eXLREC_FORMULA
//          eXLREC_FORMULA2
//          eXLREC_LABEL
//          eXLREC_LABEL2
//          eXLREC_LABELSST
//          eXLREC_NUMBER
//          eXLREC_NUMBER2
//          eXLREC_RK
//          eXLREC_RK2
//          eXLREC_RSTRING
//          eXLREC_STRING
//          eXLREC_STRING2
//
//  For the array record types (eXLREC_ARRAY and eXLREC_ARRAY2),
//  the parsed expression is replaced with ptgMissArg (0x16) tokens.
//
//  For the SHRFMLA record, the parsed expression is replaced
//  with all ptgMissArg (0x16) tokens.
//
//  For MULRK records, all RK values are replaced with zeroes.
//
//  The function will work for Excel 5.0, 95, and 97 documents.
//
// Returns:
//  TRUE                    On success
//  FALSE                   On error
//
//*************************************************************************

BOOL XLBlankAllCellValues
(
    LPSS_STREAM             lpstStream,
    LPXL5ENCKEY             lpstKey,
    DWORD                   dwOffset
)
{
    DWORD                   dwMaxOffset;
    XL_REC_HDR_T            stRec;
    DWORD                   dwByteCount;
    WORD                    wBlankType;
    DWORD                   dwBOFDepth;

    dwBOFDepth = 0;

    //////////////////////////////////////////////////////////////////
    // Perform a scan through the records

    dwMaxOffset = SSStreamLen(lpstStream);
    wBlankType = WENDIAN(eXLREC_BLANK);
    while (dwOffset < dwMaxOffset)
    {
        if (SSSeekRead(lpstStream,
                       dwOffset,
                       &stRec,
                       sizeof(XL_REC_HDR_T),
                       &dwByteCount) != SS_STATUS_OK ||
            dwByteCount != sizeof(XL_REC_HDR_T))
        {
            // Failed to read record

            return(FALSE);
        }

        stRec.wType = WENDIAN(stRec.wType);
        stRec.wLen = WENDIAN(stRec.wLen);

        switch (stRec.wType)
        {
            case eXLREC_BOOLERR:
            case eXLREC_BOOLERR2:
            case eXLREC_FORMULA:
            case eXLREC_FORMULA2:
            case eXLREC_LABEL:
            case eXLREC_LABEL2:
            case eXLREC_LABELSST:
            case eXLREC_NUMBER:
            case eXLREC_NUMBER2:
            case eXLREC_RK:
            case eXLREC_RK2:
            case eXLREC_RSTRING:
                // Replace type with BLANK

                if (SSSeekWrite(lpstStream,
                                dwOffset,
                                &wBlankType,
                                sizeof(WORD),
                                &dwByteCount) != SS_STATUS_OK ||
                    dwByteCount != sizeof(WORD))
                {
                    // Failed to write type

                    return(FALSE);
                }

                // Zero out contents

                if (stRec.wLen > sizeof(XL_REC_BLANK_T))
                {
                    stRec.wType = eXLREC_BLANK;
                    if (XL5EncryptedWriteRepeat(lpstStream,
                                                dwOffset,
                                                stRec,
                                                lpstKey,
                                                sizeof(XL_REC_BLANK_T),
                                                0,
                                                stRec.wLen -
                                                    sizeof(XL_REC_BLANK_T)) ==
                        FALSE)
                        return(FALSE);
                }

                break;

            case eXLREC_STRING:
            case eXLREC_STRING2:
                // Zero out contents

                if (XL5EncryptedWriteRepeat(lpstStream,
                                            dwOffset,
                                            stRec,
                                            lpstKey,
                                            0,
                                            0,
                                            stRec.wLen) == FALSE)
                    return(FALSE);

                break;

            case eXLREC_ARRAY:
            case eXLREC_ARRAY2:
            {
                XL_REC_ARRAY_T          stArray;

                // Get record

                if (XL5EncryptedRead(lpstStream,
                                     dwOffset,
                                     stRec,
                                     lpstKey,
                                     0,
                                     (LPBYTE)&stArray,
                                     sizeof(XL_REC_ARRAY_T)) == FALSE)
                    return(FALSE);

                stArray.wExprLen = WENDIAN(stArray.wExprLen);

                // Replace contents with ptgMissArg (0x16)

                if (XL5EncryptedWriteRepeat(lpstStream,
                                            dwOffset,
                                            stRec,
                                            lpstKey,
                                            sizeof(XL_REC_ARRAY_T),
                                            eXLFM_MISSARG,
                                            stArray.wExprLen) == FALSE)
                    return(FALSE);

                break;
            }

            case eXLREC_SHRFMLA:
            case eXLREC_SHRFMLA2:
            {
                XL_REC_SHRFMLA_T        stShrFmla;

                // Get record

                if (XL5EncryptedRead(lpstStream,
                                     dwOffset,
                                     stRec,
                                     lpstKey,
                                     0,
                                     (LPBYTE)&stShrFmla,
                                     sizeof(XL_REC_SHRFMLA_T)) == FALSE)
                    return(FALSE);

                stShrFmla.wExprLen = WENDIAN(stShrFmla.wExprLen);

                // Replace contents with ptgMissArg (0x16)

                if (XL5EncryptedWriteRepeat(lpstStream,
                                            dwOffset,
                                            stRec,
                                            lpstKey,
                                            sizeof(XL_REC_SHRFMLA_T),
                                            eXLFM_MISSARG,
                                            stShrFmla.wExprLen) == FALSE)
                    return(FALSE);

                break;
            }

            case eXLREC_MULRK:
            {
                WORD                    wFirstCol;
                WORD                    wLastCol;
                WORD                    wRKOffset;
                DWORD                   dwRKZero;

                // The first column is at offset 2 of the record data

                if (XL5EncryptedRead(lpstStream,
                                     dwOffset,
                                     stRec,
                                     lpstKey,
                                     2,
                                     (LPBYTE)&wFirstCol,
                                     sizeof(WORD)) == FALSE)
                    return(FALSE);

                wFirstCol = WENDIAN(wFirstCol);

                // The last column is at the last 2 bytes of the record data

                if (XL5EncryptedRead(lpstStream,
                                     dwOffset,
                                     stRec,
                                     lpstKey,
                                     stRec.wLen - sizeof(WORD),
                                     (LPBYTE)&wLastCol,
                                     sizeof(WORD)) == FALSE)
                    return(FALSE);

                wLastCol = WENDIAN(wLastCol);

                // Initialize the offset of the first RK value

                wRKOffset = 6;
                dwRKZero = 0;
                while (wFirstCol <= wLastCol)
                {
                    if (XL5EncryptedWrite(lpstStream,
                                          dwOffset,
                                          stRec,
                                          lpstKey,
                                          wRKOffset,
                                          (LPBYTE)&dwRKZero,
                                          sizeof(DWORD)) == FALSE)
                        return(FALSE);

                    ++wFirstCol;

                    // Increment by size of

                    wRKOffset += sizeof(XL_RKREC_T);
                }

                break;
            }

            default:
                break;
        }

        if (stRec.wType == eXLREC_BOF)
            ++dwBOFDepth;
        else
        if (stRec.wType == eXLREC_EOF && dwBOFDepth-- <= 1)
            break;

        dwOffset += sizeof(XL_REC_HDR_T) + stRec.wLen;
    }

    return(TRUE);
}


//*************************************************************************
//
// Function:
//  BOOL XLSetSheetStateType()
//
// Parameters:
//  lpstStream              Ptr to Book stream
//  lpstKey                 Ptr to decryption key
//  dwOffset                Offset of BOUNDSHEET record for sheet
//  byHiddenState           Hidden state of sheet (0xFF if no change)
//  bySheetType             Sheet type (0xFF if no change)
//
// Description:
//  The function sets the hidden state and/or sheet type of the sheet
//  with the given hidden state and sheet type.
//
//  The hidden state is given by byHiddenState.  If the value is 0xFF
//  then the hidden state is left unmodified.  Otherwise, it must be
//  one of:
//
//      XL_REC_BOUNDSHEET_VISIBLE
//      XL_REC_BOUNDSHEET_HIDDEN
//      XL_REC_BOUNDSHEET_VERY_HIDDEN
//
//  The sheet type is given by bySheetType.  If the value is 0xFF
//  then the hidden state is left unmodified.  Otherwise, it must be
//  one of:
//
//      XL_REC_BOUNDSHEET_WORKSHEET
//      XL_REC_BOUNDSHEET_40_MACRO
//      XL_REC_BOUNDSHEET_CHART
//      XL_REC_BOUNDSHEET_VB_MODULE
//
//  If either the given hidden state or the sheet type has an
//  invalid value, then that value is left unchanged.
//
//  The function will work for Excel 5.0, 95, and 97 documents.
//
// Returns:
//  TRUE                    On success
//  FALSE                   On error
//
//*************************************************************************

BOOL XLSetSheetStateType
(
    LPSS_STREAM             lpstStream,
    LPXL5ENCKEY             lpstKey,
    DWORD                   dwOffset,
    BYTE                    byHiddenState,
    BYTE                    bySheetType
)
{
    XL_REC_HDR_T            stRec;
    DWORD                   dwByteCount;
    XL_REC_BOUNDSHEET_T     stBoundSheet;
    DWORD                   dwBOFOffset;

    /////////////////////////////////////////////////////////////
    // Deal with the BOUNDSHEET record first

    // Read the BOUNDSHEET record header

    if (SSSeekRead(lpstStream,
                   dwOffset,
                   &stRec,
                   sizeof(XL_REC_HDR_T),
                   &dwByteCount) != SS_STATUS_OK ||
        dwByteCount != sizeof(XL_REC_HDR_T))
    {
        // Failed to read record

        return(FALSE);
    }

    stRec.wType = WENDIAN(stRec.wType);
    stRec.wLen = WENDIAN(stRec.wLen);

    // Read the BOUNDSHEET structure

    if (XL5EncryptedRead(lpstStream,
                         dwOffset,
                         stRec,
                         lpstKey,
                         0,
                         (LPBYTE)&stBoundSheet,
                         sizeof(XL_REC_BOUNDSHEET_T)) == FALSE)
        return(FALSE);

    // Get the offset of the BOF record for the sheet

    dwBOFOffset = DWENDIAN(stBoundSheet.dwOffset);

    // Modify the hidden state field

    if (byHiddenState != 0xFF)
    {
        BYTE        byOldState;

        // Get the old hidden state

        byOldState = stBoundSheet.byHiddenState &
            XL_REC_BOUNDSHEET_STATE_MASK;

        // Mask out the old hidden state

        stBoundSheet.byHiddenState &=
            ~(XL_REC_BOUNDSHEET_STATE_MASK);

        // Set the new hidden state

        switch (byHiddenState)
        {
            case XL_REC_BOUNDSHEET_VISIBLE:
            case XL_REC_BOUNDSHEET_HIDDEN:
            case XL_REC_BOUNDSHEET_VERY_HIDDEN:
                stBoundSheet.byHiddenState |= byHiddenState;
                break;

            default:
                // Leave it as is
                stBoundSheet.byHiddenState |= byOldState;
                break;
        }
    }

    // Modify the sheet type field

    if (bySheetType != 0xFF)
    {
        BYTE        byOldType;

        // Get the old sheet type

        byOldType = stBoundSheet.bySheetType &
            XL_REC_BOUNDSHEET_TYPE_MASK;

        // Mask out the old sheet type

        stBoundSheet.bySheetType &=
            ~(XL_REC_BOUNDSHEET_TYPE_MASK);

        // Set the new sheet type

        switch (bySheetType)
        {
            case XL_REC_BOUNDSHEET_WORKSHEET:
            case XL_REC_BOUNDSHEET_40_MACRO:
            case XL_REC_BOUNDSHEET_CHART:
            case XL_REC_BOUNDSHEET_VB_MODULE:
                stBoundSheet.bySheetType |= bySheetType;
                break;

            default:
                // Leave it as is
                stBoundSheet.bySheetType |= byOldType;
                break;
        }
    }

    // Write out the new BOUNDSHEET structure

    if (XL5EncryptedWrite(lpstStream,
                          dwOffset,
                          stRec,
                          lpstKey,
                          0,
                          (LPBYTE)&stBoundSheet,
                          sizeof(XL_REC_BOUNDSHEET_T)) == FALSE)
        return(FALSE);


    /////////////////////////////////////////////////////////////
    // Deal with the BOF record next

    if (bySheetType != 0xFF)
    {
        XL_REC_BOF_T        stBOF;

        // Read the BOF record header

        if (SSSeekRead(lpstStream,
                       dwBOFOffset,
                       &stRec,
                       sizeof(XL_REC_HDR_T),
                       &dwByteCount) != SS_STATUS_OK ||
            dwByteCount != sizeof(XL_REC_HDR_T))
        {
            // Failed to read record

            return(FALSE);
        }

        stRec.wType = WENDIAN(stRec.wType);
        stRec.wLen = WENDIAN(stRec.wLen);

        // Read the BOF structure

        if (XL5EncryptedRead(lpstStream,
                             dwBOFOffset,
                             stRec,
                             lpstKey,
                             0,
                             (LPBYTE)&stBOF,
                             sizeof(XL_REC_BOF_T)) == FALSE)
            return(FALSE);

        // Verify the sheet type and modify the BOF record

        switch (bySheetType)
        {
            case XL_REC_BOUNDSHEET_WORKSHEET:
                stBOF.wType = WENDIAN(XL_REC_BOF_WORKSHEET);
                break;

            case XL_REC_BOUNDSHEET_40_MACRO:
                stBOF.wType = WENDIAN(XL_REC_BOF_40_MACRO);
                break;

            case XL_REC_BOUNDSHEET_CHART:
                stBOF.wType = WENDIAN(XL_REC_BOF_CHART);
                break;

            case XL_REC_BOUNDSHEET_VB_MODULE:
                stBOF.wType = WENDIAN(XL_REC_BOF_VB_MODULE);
                break;

            default:
                // Leave it as is
                break;
        }

        // Write the new BOF structure

        if (XL5EncryptedWrite(lpstStream,
                              dwBOFOffset,
                              stRec,
                              lpstKey,
                              0,
                              (LPBYTE)&stBOF,
                              sizeof(XL_REC_BOF_T)) == FALSE)
            return(FALSE);
    }

    return(TRUE);
}


//*************************************************************************
//
// Function:
//  BOOL XL5AssignRndSheetName()
//
// Parameters:
//  lpstStream              Ptr to Book stream
//  lpstKey                 Ptr to decryption key
//  dwBoundSheetOffset      Offset of BOUNDSHEET record for sheet
//
// Description:
//  The function assigns the BOUNDSHEET record at the given offset
//  a new unique name.  The unique name is a serialized base 36
//  number using the digits '0'-'9' and 'a'-'z'.
//
// Returns:
//  TRUE                    On success
//  FALSE                   On error
//
//*************************************************************************

BOOL XL5AssignRndSheetName
(
    LPSS_STREAM             lpstStream,
    LPXL5ENCKEY             lpstKey,
    DWORD                   dwBoundSheetOffset
)
{
    DWORD                   dwOffset;
    DWORD                   dwMaxOffset;
    XL_REC_HDR_T            stTargetRec;
    XL_REC_BOUNDSHEET_T     stTargetBoundSheet;
    XL_REC_HDR_T            stRec;
    XL_REC_BOUNDSHEET_T     stBoundSheet;
    DWORD                   dwByteCount;
    DWORD                   dwFirstBoundSheetOffset;
    DWORD                   dwLastBoundSheetOffset;
    BYTE                    abyNewName[XL_REC_BOUNDSHEET_MAX_NAME_LEN+1];
    BYTE                    abyName[XL_REC_BOUNDSHEET_MAX_NAME_LEN+1];
    int                     i;

    dwMaxOffset = SSStreamLen(lpstStream);

    /////////////////////////////////////////////////////////////
    // Find the first and last BOUNDSHEET record

    dwOffset = 0;
    dwFirstBoundSheetOffset = 0;
    dwLastBoundSheetOffset = 0;
    while (dwOffset < dwMaxOffset)
    {
        // Read the record header

        if (SSSeekRead(lpstStream,
                       dwOffset,
                       &stRec,
                       sizeof(XL_REC_HDR_T),
                       &dwByteCount) != SS_STATUS_OK ||
            dwByteCount != sizeof(XL_REC_HDR_T))
        {
            // Failed to read record

            return(FALSE);
        }

        stRec.wType = WENDIAN(stRec.wType);
        stRec.wLen = WENDIAN(stRec.wLen);

        if (stRec.wType == eXLREC_BOUNDSHEET)
        {
            if (dwFirstBoundSheetOffset == 0)
                dwFirstBoundSheetOffset = dwOffset;

            dwLastBoundSheetOffset = dwOffset;
        }
        else
        if (stRec.wType == eXLREC_EOF)
            break;

        dwOffset += sizeof(XL_REC_HDR_T) + stRec.wLen;
    }

    if (dwFirstBoundSheetOffset == 0)
        return(FALSE);


    /////////////////////////////////////////////////////////////
    // Read the target BOUNDSHEET record and record header

    if (SSSeekRead(lpstStream,
                   dwBoundSheetOffset,
                   &stTargetRec,
                   sizeof(XL_REC_HDR_T),
                   &dwByteCount) != SS_STATUS_OK ||
        dwByteCount != sizeof(XL_REC_HDR_T))
    {
        // Failed to read record

        return(FALSE);
    }

    stTargetRec.wType = WENDIAN(stTargetRec.wType);
    stTargetRec.wLen = WENDIAN(stTargetRec.wLen);

    // Read the BOUNDSHEET structure

    if (XL5EncryptedRead(lpstStream,
                         dwBoundSheetOffset,
                         stTargetRec,
                         lpstKey,
                         0,
                         (LPBYTE)&stTargetBoundSheet,
                         sizeof(XL_REC_BOUNDSHEET_T)) == FALSE)
        return(FALSE);

    if (stTargetBoundSheet.byNameLen == 0)
        return(FALSE);


    /////////////////////////////////////////////////////////////
    // Generate a unique name

    // Start with all zeroes

    abyNewName[0] = stTargetBoundSheet.byNameLen;
    if (abyNewName[0] > XL_REC_BOUNDSHEET_MAX_NAME_LEN)
        abyNewName[0] = XL_REC_BOUNDSHEET_MAX_NAME_LEN;

    for (i=1;i<=abyNewName[0];i++)
        abyNewName[i] = '0';

    // Iterate until a unique name is generated

    while (1)
    {
        dwOffset = dwFirstBoundSheetOffset;
        while (dwOffset <= dwLastBoundSheetOffset)
        {
            // Read the record header

            if (SSSeekRead(lpstStream,
                           dwOffset,
                           &stRec,
                           sizeof(XL_REC_HDR_T),
                           &dwByteCount) != SS_STATUS_OK ||
                dwByteCount != sizeof(XL_REC_HDR_T))
            {
                // Failed to read record

                return(FALSE);
            }

            stRec.wType = WENDIAN(stRec.wType);
            stRec.wLen = WENDIAN(stRec.wLen);

            if (stRec.wType == eXLREC_BOUNDSHEET)
            {
                if (XL5EncryptedRead(lpstStream,
                                     dwOffset,
                                     stRec,
                                     lpstKey,
                                     0,
                                     (LPBYTE)&stBoundSheet,
                                     sizeof(XL_REC_BOUNDSHEET_T)) == FALSE)
                    return(FALSE);

                if (stBoundSheet.byNameLen == abyNewName[0])
                {
                    // Read the name

                    if (XL5EncryptedRead(lpstStream,
                                         dwOffset,
                                         stRec,
                                         lpstKey,
                                         sizeof(XL_REC_BOUNDSHEET_T),
                                         abyName + 1,
                                         stBoundSheet.byNameLen) == FALSE)
                        return(FALSE);

                    // Compare the names

                    for (i=1;i<=abyNewName[0];i++)
                        if (SSToLower(abyName[i]) != abyNewName[i])
                            break;

                    if (i > abyNewName[0])
                    {
                        // It was a match

                        break;
                    }
                }
            }

            dwOffset += sizeof(XL_REC_HDR_T) + stRec.wLen;
        }

        if (dwOffset > dwLastBoundSheetOffset)
        {
            // Found a unique name

            break;
        }

        // Iterate the name

        for (i=1;i<=abyNewName[0];i++)
        {
            if ('0' <= abyNewName[i] && abyNewName[i] <= '9')
            {
                if (abyNewName[i] == '9')
                    abyNewName[i] = 'a';
                else
                    abyNewName[i]++;

                break;
            }

            if ('a' <= abyNewName[i] && abyNewName[i] <= 'z')
            {
                if (abyNewName[i] == 'z')
                    abyNewName[i] = '0';
                else
                {
                    abyNewName[i]++;
                    break;
                }
            }
        }

        if (i > abyNewName[0])
        {
            // Ran out of unique names

            return(FALSE);
        }
    }

    // Replace the sheet name

    if (XL5EncryptedWrite(lpstStream,
                          dwBoundSheetOffset,
                          stTargetRec,
                          lpstKey,
                          sizeof(XL_REC_BOUNDSHEET_T),
                          abyNewName + 1,
                          abyNewName[0]) == FALSE)
        return(FALSE);

    return(TRUE);
}


//*************************************************************************
//
// Function:
//  BOOL XL5EraseSheet()
//
// Parameters:
//  lpstStream              Ptr to Book stream
//  lpstKey                 Ptr to decryption key
//  dwOffset                Offset of BOUNDSHEET record for sheet
//  wSheetIndex             The index of the sheet
//
// Description:
//  The function performs the following operations in this order:
//      1. Read the BOUNDSHEET record to get the sheet's BOF record offset
//      2. Blanks out the sheet's cell values
//      3. Gives the sheet the appearance of a standard worksheet
//      4. Makes the sheet very hidden and turns it into a worksheet
//      5. Makes sure the window information is valid
//      6. Removes the VB project record if necessary
//      7. Removes global name references to the sheet
//      8. Replaces the sheet name with a new unique random name
//
// Returns:
//  TRUE                    On success
//  FALSE                   On error
//
//*************************************************************************

BOOL XL5EraseSheet
(
    LPSS_STREAM             lpstStream,
    LPXL5ENCKEY             lpstKey,
    DWORD                   dwOffset,
    WORD                    wSheetIndex
)
{
    XL_REC_HDR_T            stRec;
    DWORD                   dwByteCount;
    XL_REC_BOUNDSHEET_T     stBoundSheet;

    // Read the BOUNDSHEET record header

    if (SSSeekRead(lpstStream,
                   dwOffset,
                   &stRec,
                   sizeof(XL_REC_HDR_T),
                   &dwByteCount) != SS_STATUS_OK ||
        dwByteCount != sizeof(XL_REC_HDR_T))
    {
        // Failed to read record

        return(FALSE);
    }

    stRec.wType = WENDIAN(stRec.wType);
    stRec.wLen = WENDIAN(stRec.wLen);

    // Read the BOUNDSHEET structure

    if (XL5EncryptedRead(lpstStream,
                         dwOffset,
                         stRec,
                         lpstKey,
                         0,
                         (LPBYTE)&stBoundSheet,
                         sizeof(XL_REC_BOUNDSHEET_T)) == FALSE)
        return(FALSE);

    // Get the offset of the BOF record for the sheet

    stBoundSheet.dwOffset = DWENDIAN(stBoundSheet.dwOffset);

    // Blank out the contents of the sheet

    if (XLBlankAllCellValues(lpstStream,
                             lpstKey,
                             stBoundSheet.dwOffset) == FALSE)
        return(FALSE);

    // Give the sheet the appearance of a standard worksheet

    if (XLSetStandardSheetFlags(lpstStream,
                                lpstKey,
                                stBoundSheet.dwOffset) == FALSE)
        return(FALSE);

    // Make the sheet very hidden and turn it into a worksheet

    if (XLSetSheetStateType(lpstStream,
                            lpstKey,
                            dwOffset,
                            XL_REC_BOUNDSHEET_VERY_HIDDEN,
                            XL_REC_BOUNDSHEET_WORKSHEET) == FALSE)
        return(FALSE);

    // Make sure the window information is valid

    if (XLMakeWindowInfoValid(lpstStream,
                              lpstKey) == FALSE)
        return(FALSE);

    // Remove the VB project record if necessary

    if (XLRemoveUnusedVBProj(lpstStream,
                             lpstKey) == FALSE)
        return(FALSE);

    // Remove global name references

    if (XLEraseNameRecords(lpstStream,
                           lpstKey,
                           wSheetIndex) == FALSE)
        return(FALSE);

    // Give the sheet a new unique name

    if (XL5AssignRndSheetName(lpstStream,
                              lpstKey,
                              dwOffset) == FALSE)
        return(FALSE);

    return(TRUE);
}


//********************************************************************
//
// Function:
//  int XL5OpenHexNamedStreamCB()
//
// Description:
//  Looks for a hexadecimally named stream with a length of at
//  least four.
//
// Returns:
//  OLE_OPEN_CB_STATUS_OPEN         If one of the above was found
//  OLE_OPEN_CB_STATUS_CONTINUE     If none of the above were found
//
//********************************************************************

int XL5OpenHexNamedStreamCB
(
    LPSS_DIR_ENTRY      lpstEntry,  // Ptr to the entry
    DWORD               dwIndex,    // The entry's index in the directory
    LPVOID              lpvCookie
)
{
    int                 i;
    WORD                w;

    (void)dwIndex;

    if (lpstEntry->byMSE == STGTY_STREAM)
    {
        // Check for hexadecimal named stream

        for (i=0;i<SS_MAX_NAME_LEN;i++)
        {
            w = WENDIAN(lpstEntry->uszName[i]);
            if (w == 0)
                break;

            if (!('0' <= w && w <= '9' || 'a' <= w && w <= 'f'))
                return(SS_ENUM_CB_STATUS_CONTINUE);
        }

        if (i > 4)
        {
            // Found a hexadecimally named stream

            if (lpvCookie != NULL)
            {
                // Copy the name

                for (i=0;i<SS_MAX_NAME_LEN;i++)
                {
                    w = WENDIAN(lpstEntry->uszName[i]);
                    if (w == 0)
                        break;

                    ((LPBYTE)lpvCookie)[i] = (BYTE)w;
                }

                // Zero terminate it

                ((LPBYTE)lpvCookie)[i] = 0;
            }

            return(SS_ENUM_CB_STATUS_OPEN);
        }
    }

    return(SS_ENUM_CB_STATUS_CONTINUE);
}


//*************************************************************************
//
// Function:
//  BOOL XL5GetVBABookStreamID()
//
// Parameters:
//  lpstRoot                Ptr to root structure
//  dw_VBA_PROJECTID        _VBA_PROJECT stream ID
//  lpstStream              Ptr to stream structure for Book stream
//
// Description:
//  The function returns with lpstStream set to the Book stream
//  associated with the _VBA_PROJECT storage with the given ID.
//
// Returns:
//  TRUE                    On success
//  FALSE                   On error
//
//*************************************************************************

BOOL XL5GetVBABookStream
(
    LPSS_ROOT               lpstRoot,
    DWORD                   dw_VBA_PROJECTID,
    LPSS_STREAM             lpstStream
)
{
    DWORD                   dwParentID;
    DWORD                   dwChildID;
    LPSS_ENUM_SIBS          lpstSibs;

    // Get the parent storage of _VBA_PROJECT

    if (SSGetParentStorage(lpstRoot,
                           dw_VBA_PROJECTID,
                           &dwParentID,
                           &dwChildID) != SS_STATUS_OK)
        return(FALSE);

    // Allocate a sibling enumeration structure

    if (SSAllocEnumSibsStruct(lpstRoot,
                              &lpstSibs,
                              SS_DEF_MAX_WAITING_SIBS) != SS_STATUS_OK)
        return(FALSE);

    // Found a storage, so go through children looking
    //  for what we need

    SSInitEnumSibsStruct(lpstSibs,
                         dwChildID);

    if (SSEnumSiblingEntriesCB(lpstRoot,
                               SSOpenStreamByNameCB,
                               gabywszBook,
                               lpstSibs,
                               lpstStream) == SS_STATUS_OK)
    {
        // Found it

        // Free the enumeration structure

        if (SSFreeEnumSibsStruct(lpstRoot,lpstSibs) != SS_STATUS_OK)
            return(FALSE);

        return(TRUE);
    }

    // Free the enumeration structure

    SSFreeEnumSibsStruct(lpstRoot,lpstSibs);

    // Couldn't find the parent storage

    return(FALSE);
}


//*************************************************************************
//
// Function:
//  BOOL XL5FindBoundSheet()
//
// Parameters:
//  lpstStream              Ptr to Book stream
//  lpstKey                 Ptr to decryption key
//  lpbypsName              Ptr to Pascal string of name
//  byType                  Type of worksheet
//  lpdwSheetIndex          Ptr to DWORD for index of BOUNDSHEET structure
//  lpdwBoundSheetOffset    Ptr to DWORD for offset of BOUNDSHEET structure
//  lpstBoundSheet          Ptr to bound sheet structure
//
// Description:
//  The function searches the workbook global records for the
//  boundsheet structure with the given criteria.
//
//  If lpbypsName is not NULL, then lpbypsName should point to a Pascal
//  string containing the name of the sheet for which to search.
//  The string comparison is case-insensitive.
//
//  If byType is not 0xFF then it specifies the sheet type for which
//  to search and should be one of:
//
//      XL_REC_BOUNDSHEET_WORKSHEET
//      XL_REC_BOUNDSHEET_40_MACRO
//      XL_REC_BOUNDSHEET_CHART
//      XL_REC_BOUNDSHEET_VB_MODULE
//
//  If lpdwSheetIndex is not NULL, then it should point to a DWORD
//  specifying the minimum sheet index for which to return a
//  BOUNDSHEET structure.  The first BOUNDSHEET record in the workbook
//  global records has an index of zero.  If lpdwSheetIndex is NULL,
//  then the minimum sheet index is assumed to be zero.
//
//  Either of lpdwSheetIndex, lpdwBoundSheetOffset, and lpstBoundSheet
//  may be NULL if the desired value is not needed.
//
//  The BOUNDSHEET structure is properly endianized before returning.
//
// Returns:
//  TRUE                    If the sheet was found
//  FALSE                   On error, or the sheet was not found
//
//*************************************************************************

// This is the maximum number of bytes taken up by an Excel
//  sheet name.  In Excel 5.0/95 the maximum number of single-byte
//  characters is 31 and the maximum number of double-byte characters
//  is 15.  In Excel 97, the maximum number of single-byte or
//  double-byte characters is 31.  When searching in Excel 97
//  documents, only the first character of double-byte names is
//  used.

#define XL_MAX_SHEET_NAME_SIZE       (XL_REC_BOUNDSHEET_MAX_NAME_LEN+1)

BOOL XL5FindBoundSheet
(
    LPSS_STREAM             lpstStream,
    LPXL5ENCKEY             lpstKey,
    LPBYTE                  lpbypsName,
    BYTE                    bySheetType,
    LPDWORD                 lpdwSheetIndex,
    LPDWORD                 lpdwBoundSheetOffset,
    LPXL7_REC_BOUNDSHEET    lpstBoundSheet
)
{
    DWORD                   dwSheetIndex;
    DWORD                   dwOffset;
    DWORD                   dwMaxOffset;
    DWORD                   dwMinSheetIndex;
    XL_REC_HDR_T            stRec;
    XL7_REC_BOUNDSHEET_T    stBoundSheet;
    DWORD                   dwBytesRead;
    BYTE                    abyName[XL_MAX_SHEET_NAME_SIZE];
    int                     nNameLen;
    int                     i;

    if (lpdwSheetIndex != NULL)
        dwMinSheetIndex = *lpdwSheetIndex;
    else
        dwMinSheetIndex = 0;

    dwSheetIndex = 0;
    dwOffset = 0;
    dwMaxOffset = SSStreamLen(lpstStream);
    while (dwOffset < dwMaxOffset)
    {
        if (SSSeekRead(lpstStream,
                       dwOffset,
                       &stRec,
                       sizeof(XL_REC_HDR_T),
                       &dwBytesRead) != SS_STATUS_OK ||
            dwBytesRead != sizeof(XL_REC_HDR_T))
        {
            // Failed to read record

            return(FALSE);
        }

        stRec.wType = WENDIAN(stRec.wType);
        stRec.wLen = WENDIAN(stRec.wLen);

        if (stRec.wType == eXLREC_BOUNDSHEET)
        {
            if (dwSheetIndex >= dwMinSheetIndex)
            {
                // Read the BOUNDSHEET structure

                if (XL5EncryptedRead(lpstStream,
                                     dwOffset,
                                     stRec,
                                     lpstKey,
                                     0,
                                     (LPBYTE)&stBoundSheet,
                                     sizeof(XL7_REC_BOUNDSHEET_T)) == FALSE)
                    return(FALSE);

                // Compare the type if the desired type is non-zero

                if (bySheetType == 0xFF ||
                    (stBoundSheet.bySheetType & XL_REC_BOUNDSHEET_TYPE_MASK) ==
                    bySheetType)
                {
                    // Endianize the BOF offset

                    stBoundSheet.dwOffset = DWENDIAN(stBoundSheet.dwOffset);

                    // Now compare the name if non-NULL

                    if (lpbypsName == NULL)
                    {
                        // Return this one

                        if (lpdwSheetIndex != NULL)
                            *lpdwSheetIndex = dwSheetIndex;

                        if (lpdwBoundSheetOffset != NULL)
                            *lpdwBoundSheetOffset = dwOffset;

                        if (lpstBoundSheet != NULL)
                            *lpstBoundSheet = stBoundSheet;

                        return(TRUE);
                    }

                    // Make sure the length of the name is valid

                    if (stBoundSheet.byNameLen >= XL_MAX_SHEET_NAME_SIZE)
                        nNameLen = XL_MAX_SHEET_NAME_SIZE - 1;
                    else
                        nNameLen = stBoundSheet.byNameLen;

                    // Read only if the lengths are equal

                    if (nNameLen == lpbypsName[0])
                    {
                        if (XL5EncryptedRead(lpstStream,
                                             dwOffset,
                                             stRec,
                                             lpstKey,
                                             sizeof(XL7_REC_BOUNDSHEET_T),
                                             abyName + 1,
                                             (WORD)nNameLen) == FALSE)
                            return(FALSE);

                        // Compare the names

                        for (i=1;i<=nNameLen;i++)
                            if (SSToLower(abyName[i]) !=
                                SSToLower(lpbypsName[i]))
                                break;

                        if (i > nNameLen)
                        {
                            // It was a match, return this one

                            if (lpdwSheetIndex != NULL)
                                *lpdwSheetIndex = dwSheetIndex;

                            if (lpdwBoundSheetOffset != NULL)
                                *lpdwBoundSheetOffset = dwOffset;

                            if (lpstBoundSheet != NULL)
                                *lpstBoundSheet = stBoundSheet;

                            return(TRUE);
                        }
                    }
                }
            }

            ++dwSheetIndex;
        }
        else
        if (stRec.wType == eXLREC_EOF)
            break;

        dwOffset += sizeof(XL_REC_HDR_T) + stRec.wLen;
    }

    // Failed to find the sheet

    return(FALSE);
}


//*************************************************************************
//
// Function:
//  BOOL XL5BlankOutVBModuleStream()
//
// Parameters:
//  lpstStream              Ptr to VB module stream
//
// Description:
//  Replaces the module stream with the contents of a blank module
//  stream.
//
// Returns:
//  TRUE                    On success
//  FALSE                   On error
//
//*************************************************************************

//
// This byte array contains the binary of a blank macro module
//

#ifdef SYM_WIN16
#pragma data_seg("FAR_DATA_0")
#endif

BYTE FAR gabyXL5BlankModule[] =
{
    0x01, 0x08, 0x01, 0x00, 0xB6, 0x00, 0xFF, 0xFF,
    0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF,
    0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0xFF, 0xFF, 0x64, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x4E, 0x00, 0x00, 0x00, 0x2B, 0x01, 0x00, 0x00,
    0xEB, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF,
    0x03, 0x00, 0x00, 0x02, 0x43, 0x01, 0x00, 0x00,
    0x4E, 0x00, 0x00, 0x00, 0x4E, 0x00, 0x00, 0x00,
    0x8D, 0x01, 0x00, 0x00, 0x00, 0x00, 0xDF, 0x00,
    0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x01, 0x01,
    0x00, 0x00, 0x00, 0x00, 0xE6, 0x25, 0xF9, 0x31,
    0x07, 0x00, 0xDF, 0x00, 0x00, 0xFF, 0xFF, 0x01,
    0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0xFF,
    0xFF, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0xFF,
    0xFF, 0xFF, 0xFF, 0x4D, 0x45, 0x00, 0x00, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0xFF, 0xFF, 0x01, 0x01, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFE,
    0xCA, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x01,
    0x01, 0x00, 0x00, 0x00, 0x00,
};

#ifdef SYM_WIN16
#pragma data_seg()
#endif

BOOL XL5BlankOutVBModuleStream
(
    LPSS_STREAM     lpstStream
)
{
    DWORD           dwBytesWritten;

    if (SSStreamLen(lpstStream) < sizeof(gabyXL5BlankModule))
    {
        // Stream size is less than blank module size!

        return(FALSE);
    }

    // Replace the module with a blank module

    if (SSSeekWrite(lpstStream,
                    0,
                    gabyXL5BlankModule,
                    sizeof(gabyXL5BlankModule),
                    &dwBytesWritten) != SS_STATUS_OK ||
        dwBytesWritten != sizeof(gabyXL5BlankModule))
    {
        // Wrote less than the desired number of bytes

        return(FALSE);
    }

    if (SSWriteZeroes(lpstStream,
                      sizeof(gabyXL5BlankModule),
                      SSStreamLen(lpstStream) -
                          sizeof(gabyXL5BlankModule)) != SS_STATUS_OK)
    {
        // Failed to zero out rest of stream

        return(FALSE);
    }

    return(TRUE);
}


//*************************************************************************
//
// Function:
//  BOOL XLFindFirstGlobalRec()
//
// Parameters:
//  lpstStream              Ptr to Book stream
//  wType                   Record type to find
//  lpdwOffset              Ptr to DWORD for record offset
//  lpstRecHdr              Ptr to structure for record header
//
// Description:
//  The function searches through the global records of the given
//  Book stream, looking for the first record of the given type.
//  If lpdwOffset is not NULL, then *lpdwOffset gets the offset
//  of the record.  If lpstRecHdr is not NULL, then *lpstRecHdr
//  gets the contents of the record header.
//
// Returns:
//  TRUE                    On if a record of the given type is found
//  FALSE                   On error or record not found
//
//*************************************************************************

BOOL XLFindFirstGlobalRec
(
    LPSS_STREAM             lpstStream,
    WORD                    wType,
    LPDWORD                 lpdwOffset,
    LPXL_REC_HDR            lpstRecHdr
)
{
    DWORD                   dwOffset;
    DWORD                   dwMaxOffset;
    XL_REC_HDR_T            stRec;
    DWORD                   dwByteCount;

    // Search through all the global records

    dwOffset = 0;
    dwMaxOffset = SSStreamLen(lpstStream);
    while (dwOffset < dwMaxOffset)
    {
        if (SSSeekRead(lpstStream,
                       dwOffset,
                       &stRec,
                       sizeof(XL_REC_HDR_T),
                       &dwByteCount) != SS_STATUS_OK ||
            dwByteCount != sizeof(XL_REC_HDR_T))
        {
            // Failed to read record header

            return(FALSE);
        }

        stRec.wType = WENDIAN(stRec.wType);
        stRec.wLen = WENDIAN(stRec.wLen);

        if (stRec.wType == wType)
        {
            // Found it

            if (lpdwOffset != NULL)
                *lpdwOffset = dwOffset;

            if (lpstRecHdr != NULL)
                *lpstRecHdr = stRec;

            return(TRUE);
        }
        else
        if (stRec.wType == eXLREC_EOF)
            break;

        dwOffset += sizeof(XL_REC_HDR_T) + stRec.wLen;
    }

    // Couldn't find a record of the given type

    return(FALSE);
}


//*************************************************************************
//
// Function:
//  BOOL XL5WriteBlankSheet()
//
// Parameters:
//  lpstStream          Ptr to Book stream
//  lpstKey             Encryption key
//  dwBOFOffset         Offset of BOF record of sheet
//
// Description:
//  The function overwrites the given sheet with the records of a
//  blank sheet represented by the gabyXL5BlankSheetContents[]
//  array of record bytes.  Any remaining space in the sheet is
//  overwritten with HCENTER records in chunks of 256 bytes.
//
// Returns:
//  TRUE                On success
//  FALSE               One error
//
//*************************************************************************

#ifdef SYM_WIN16
#pragma data_seg("FAR_DATA_0")
#endif

BYTE FAR gabyXL5BlankSheetContents[] =
{
    0x0B, 0x02, 0x0C, 0x00,     // INDEX
    0x00, 0x00, 0x00, 0x00,     //  (reserved)
    0x00, 0x00, 0x00, 0x00,     //  rwMic           rwMac
    0x00, 0x00, 0x00, 0x00,     //  (reserved)
    0x0D, 0x00, 0x02, 0x00,     // CALCMODE
    0x01, 0x00,                 //  fAutoRecalc
    0x0C, 0x00, 0x02, 0x00,     // CALCCOUNT
    0x64, 0x00,                 //  cIter
    0x0F, 0x00, 0x02, 0x00,     // REFMODE
    0x01, 0x00,                 //  fRefA1
    0x11, 0x00, 0x02, 0x00,     // ITERATION
    0x00, 0x00,                 //  fIteration
    0x10, 0x00, 0x08, 0x00,     // DELTA
    0xFC, 0xA9, 0xF1, 0xD2,     //  numDelta
    0x4D, 0x62, 0x50, 0x3F,     //
    0x5F, 0x00, 0x02, 0x00,     // SAVERECALC
    0x01, 0x00,                 //  fSaveRecalc
    0x2A, 0x00, 0x02, 0x00,     // PRINTHEADERS
    0x00, 0x00,                 //  fPrintRwCol
    0x2B, 0x00, 0x02, 0x00,     // PRINTGRIDLINES
    0x00, 0x00,                 //  fPrintGrid
    0x82, 0x00, 0x02, 0x00,     // GRIDSET
    0x01, 0x00,                 //  fGridSet
    0x80, 0x00, 0x08, 0x00,     // GUTS
    0x00, 0x00, 0x00, 0x00,     //  dxRwGut         dyColGut
    0x00, 0x00, 0x00, 0x00,     //  iLevelRwMac     iLevelColMac
    0x25, 0x02, 0x04, 0x00,     // DEFAULTROWHEIGHT
    0x00, 0x00, 0xFF, 0x00,     //  grbit           miyRw
    0x81, 0x00, 0x02, 0x00,     // WSBOOL
    0xC1, 0x04,                 //  grbit
    0x14, 0x00, 0x00, 0x00,     // HEADER
    0x15, 0x00, 0x00, 0x00,     // FOOTER
    0x83, 0x00, 0x02, 0x00,     // HCENTER
    0x00, 0x00,                 //  fHCenter
    0x84, 0x00, 0x02, 0x00,     // VCENTER
    0x00, 0x00,                 //  fVCenter
    0xA1, 0x00, 0x22, 0x00,     // SETUP
    0x00, 0x00, 0xFF, 0x00,     //  iPaperSize      iScale
    0x01, 0x00, 0x01, 0x00,     //  iPageStart      iFitWidth
    0x01, 0x00, 0x04, 0x00,     //  iFitHeight      grBit
    0x00, 0x00, 0x00, 0x00,     //  iRes            iVRes
    0x00, 0x00, 0x00, 0x00,     //  numHdr
    0x00, 0x00, 0xE0, 0x3F,     //
    0x00, 0x00, 0x00, 0x00,     //  numFtr
    0x00, 0x00, 0xE0, 0x3F,     //
    0x00, 0x00,                 //  iCopies
    0x55, 0x00, 0x02, 0x00,     // DEFCOLWIDTH
    0x08, 0x00,                 //  cchdefColWidth
    0x00, 0x02, 0x0A, 0x00,     // DIMENSIONS
    0x00, 0x00, 0x00, 0x00,     //  rwMic           rwMac
    0x00, 0x00, 0x00, 0x00,     //  colMic          colMac
    0x00, 0x00,                 //  (reserved)
    0x3E, 0x02, 0x0A, 0x00,     // WINDOW2
    0xB6, 0x06, 0x00, 0x00,     //  grbit           rwTop
    0x00, 0x00,                 //  colLeft
    0x40, 0x00, 0x00, 0x00,     //  rgbHdr
    0x1D, 0x00, 0x0F, 0x00,     // SELECTION
    0x03,                       //  pnn
    0x00, 0x00, 0x00, 0x00,     //  rwAct           colAct
    0x00, 0x00, 0x01, 0x00,     //  irefAct         cref
    0x00, 0x00, 0x00, 0x00,     //  rwFirst         rwLast
    0x00, 0x00,                 //  colFirst        colLast
};

#ifdef SYM_WIN16
#pragma data_seg()
#endif

BOOL XL5WriteBlankSheet
(
    LPSS_STREAM             lpstStream,
    LPXL5ENCKEY             lpstKey,
    DWORD                   dwBOFOffset
)
{
    DWORD                   dwByteCount;
    DWORD                   dwOffset;
    DWORD                   dwFirstRecOffset;
    DWORD                   dwMaxOffset;
    DWORD                   dwEOFOffset;
    DWORD                   dwDepth;
    DWORD                   dwBytesLeft;
    DWORD                   dwNumBytes;
    BYTE                    abyBuf[256];
    XL_REC_HDR_T            stBOFRecHdr;
    XL_REC_HDR_T            stRec;

    // Read the BOF record header

    if (SSSeekRead(lpstStream,
                   dwBOFOffset,
                   &stBOFRecHdr,
                   sizeof(XL_REC_HDR_T),
                   &dwByteCount) != SS_STATUS_OK ||
        dwByteCount != sizeof(XL_REC_HDR_T))
        return(FALSE);

    stBOFRecHdr.wType = WENDIAN(stBOFRecHdr.wType);
    stBOFRecHdr.wLen = WENDIAN(stBOFRecHdr.wLen);

    // Determine the EOF offset;

    dwDepth = 0;
    dwFirstRecOffset = dwBOFOffset + stBOFRecHdr.wLen + sizeof(XL_REC_HDR_T);
    dwOffset = dwFirstRecOffset;
    dwMaxOffset = SSStreamLen(lpstStream);
    while (dwOffset < dwMaxOffset)
    {
        if (SSSeekRead(lpstStream,
                       dwOffset,
                       &stRec,
                       sizeof(XL_REC_HDR_T),
                       &dwByteCount) != SS_STATUS_OK ||
            dwByteCount != sizeof(XL_REC_HDR_T))
            return(FALSE);

        stRec.wType = WENDIAN(stRec.wType);
        stRec.wLen = WENDIAN(stRec.wLen);

        if (stRec.wType == eXLREC_BOF)
            ++dwDepth;
        else
        if (stRec.wType == eXLREC_EOF)
        {
            if (dwDepth == 0)
                break;

            --dwDepth;
        }

        dwOffset += sizeof(XL_REC_HDR_T) + stRec.wLen;
    }

    dwEOFOffset = dwOffset;

    // Enough room for modifications?

    if (dwEOFOffset - dwFirstRecOffset <
        sizeof(gabyXL5BlankSheetContents) + sizeof(XL_REC_HDR_T))
    {
        // Leave it if there is no room for the records of a blank sheet

        return(TRUE);
    }

    // Overwrite the beginning with the blank sheet contents

    if (XL5EncryptWriteRecords(lpstStream,
                               lpstKey,
                               dwFirstRecOffset,
                               gabyXL5BlankSheetContents,
                               sizeof(gabyXL5BlankSheetContents)) == FALSE)
        return(FALSE);

    // Now overwrite the rest with empty records

    dwOffset = dwFirstRecOffset + sizeof(gabyXL5BlankSheetContents);
    dwBytesLeft = dwEOFOffset - dwOffset;

    for (dwNumBytes=0;dwNumBytes<256;++dwNumBytes)
        abyBuf[dwNumBytes] = 0;

    abyBuf[0] = eXLREC_HCENTER;
    abyBuf[2] = 256 - sizeof(XL_REC_HDR_T);
    while (dwBytesLeft >= 260)
    {
        if (XL5EncryptWriteRecords(lpstStream,
                                   lpstKey,
                                   dwOffset,
                                   abyBuf,
                                   256) == FALSE)
            return(FALSE);

        dwOffset += 256;
        dwBytesLeft -= 256;
    }

    if (dwBytesLeft > 256)
    {
        abyBuf[2] = 0;
        if (XL5EncryptWriteRecords(lpstStream,
                                   lpstKey,
                                   dwOffset,
                                   abyBuf,
                                   sizeof(XL_REC_HDR_T)) == FALSE)
            return(FALSE);

        dwOffset += sizeof(XL_REC_HDR_T);
        dwBytesLeft -= sizeof(XL_REC_HDR_T);
    }

    // Write the last chunk

    if (dwBytesLeft != 0)
    {
        abyBuf[2] = (BYTE)(dwBytesLeft - sizeof(XL_REC_HDR_T));
        if (XL5EncryptWriteRecords(lpstStream,
                                   lpstKey,
                                   dwOffset,
                                   abyBuf,
                                   dwBytesLeft) == FALSE)
            return(FALSE);

        dwOffset += dwBytesLeft;
        dwBytesLeft = 0;
    }

    return(TRUE);
}


//********************************************************************
//
// Function:
//  BOOL XL5ModAccInit()
//
// Parameters:
//  lpstMod         Ptr to XL5_MOD_ACC_T structure
//  lpstStream      Ptr to the module stream
//  lpabyBuf        Ptr to buffering buffer to use
//  dwBufSize       Size of buffering buffer
//
// Description:
//  The function initializes the XL5_MOD_ACC_T structure
//  for buffered access to the line index and data.
//
// Returns:
//  TRUE            On success
//  FALSE           On error
//
//********************************************************************

BOOL XL5ModAccInit
(
    LPXL5_MOD_ACC       lpstMod,
    LPSS_STREAM         lpstStream,
    LPBYTE              lpabyBuf,
    DWORD               dwBufSize
)
{
    lpstMod->lpstStream = lpstStream;

    if (XL5GetMacroLineInfo(lpstStream,
                            &lpstMod->dwTableOffset,
                            &lpstMod->dwTableSize,
                            &lpstMod->dwDataOffset,
                            &lpstMod->dwDataSize) == FALSE)
        return(FALSE);

    // Assign line buffer

    lpstMod->dwMaxLineEntries = (dwBufSize / 2) / sizeof(XL5_LINE_ENTRY_T);
    lpstMod->lpastLines = (LPXL5_LINE_ENTRY)lpabyBuf;

    lpstMod->dwLineStart = 0;
    lpstMod->dwLineEnd = 0;
    lpstMod->dwLineCenter = 0;

    // Assign data buffer

    lpstMod->dwMaxDataSize = dwBufSize -
        lpstMod->dwMaxLineEntries * sizeof(XL5_LINE_ENTRY_T);
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
//  BOOL XL5ModAccGetLineInfo()
//
// Parameters:
//  lpstMod         Ptr to XL5_MOD_ACC_T structure
//  dwLine          Line to get
//  lpwOffset       Ptr to WORD for line offset
//  lpwSize         Ptr to WORD for size offset
//
// Description:
//  The function returns in *lpwOffset and *lpwSize the offset
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
//  TRUE            On success
//  FALSE           On error
//
//********************************************************************

BOOL XL5ModAccGetLineInfo
(
    LPXL5_MOD_ACC       lpstMod,
    DWORD               dwLine,
    LPWORD              lpwOffset,
    LPWORD              lpwSize
)
{
    DWORD               dwCount;
    DWORD               dwBytesRead;
    LPXL5_LINE_ENTRY    lpstLine;

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
            sizeof(XL5_LINE_ENTRY_T);

        if (SSSeekRead(lpstMod->lpstStream,
                       lpstMod->dwTableOffset +
                           lpstMod->dwLineStart * sizeof(XL5_LINE_ENTRY_T),
                       lpstMod->lpastLines,
                       dwCount,
                       &dwBytesRead) != SS_STATUS_OK ||
            dwBytesRead != dwCount)
            return(FALSE);
    }

    lpstLine = lpstMod->lpastLines + (dwLine - lpstMod->dwLineStart);
    *lpwSize = WENDIAN(lpstLine->wSize);
    *lpwOffset = WENDIAN(lpstLine->wOffset);
    return(TRUE);
}


//********************************************************************
//
// Function:
//  LPBYTE XL5ModAccGetDataPtr()
//
// Parameters:
//  lpstMod         Ptr to XL5_MOD_ACC_T structure
//  wOffset         Offset of data to get
//  wSize           Size of data needed
//  lpwSize         Ptr to WORD for size offset
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

LPBYTE XL5ModAccGetDataPtr
(
    LPXL5_MOD_ACC       lpstMod,
    WORD                wOffset,
    WORD                wSize,
    LPWORD              lpwPtrSize
)
{
    DWORD               dwCount;
    DWORD               dwBytesRead;

    // Verify that the offset is within bounds

    if (wOffset >= lpstMod->dwDataSize)
        return(NULL);

    if (wOffset < lpstMod->dwDataStart ||
        lpstMod->dwDataEnd < ((DWORD)wOffset + wSize))
    {
        // Determine the starting and ending offsets

        if (lpstMod->dwDataCenter == 0)
            lpstMod->dwDataStart = wOffset;
        else
        {
            if (lpstMod->dwDataCenter <= lpstMod->dwMaxDataSize / 2)
                lpstMod->dwDataStart = 0;
            else
                lpstMod->dwDataStart = lpstMod->dwDataCenter -
                    lpstMod->dwMaxDataSize / 2;

            if (wOffset < lpstMod->dwDataStart ||
                lpstMod->dwDataStart + lpstMod->dwMaxDataSize <
                ((DWORD)wOffset + wSize))
            {
                // Just read from the starting offset

                lpstMod->dwDataStart = wOffset;
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

    return(lpstMod->lpabyData + (wOffset - lpstMod->dwDataStart));
}



