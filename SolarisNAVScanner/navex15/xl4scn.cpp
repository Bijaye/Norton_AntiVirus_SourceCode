//************************************************************************
//
// $Header:   S:/NAVEX/VCS/XL4SCN.CPv   1.4   09 Dec 1998 17:45:36   DCHI  $
//
// Description:
//  Contains Excel 4.0 macro scan function code.
//
//************************************************************************
// $Log:   S:/NAVEX/VCS/XL4SCN.CPv  $
// 
//    Rev 1.4   09 Dec 1998 17:45:36   DCHI
// Added #include "gdefines.h"
// 
//    Rev 1.3   08 Dec 1998 12:57:52   DCHI
// - Changes for MSX.
// - Use XL4GetBoundsheetOffset() to get first BOUNDSHEET record.
// 
//    Rev 1.2   09 Nov 1998 13:58:46   DCHI
// Added #pragma data_seg()'s for global FAR data for SYM_WIN16.
// 
//    Rev 1.1   11 May 1998 17:49:46   DCHI
// Changes for Access 2.0 scanning and Access datafile conversion.
// 
//    Rev 1.0   15 Apr 1998 16:46:18   DCHI
// Initial revision.
// 
//************************************************************************

#include "gdefines.h"

#include "platform.h"

#ifdef WIN16        // VIRSCAN QUAKE 2
#include "cts4.h"
#else
#include "ctsn.h"
#include "callback.h"
#endif

#include "navex.h"

#include "wd7api.h"
#include "o97api.h"
#include "xl5api.h"
#include "acsapi.h"
#include "xlrec.h"
#include "olestrnm.h"

#include "wdscan15.h"
#include "offcsc15.h"

#include "xl4scn.h"

#include "mcrsgutl.h"

#include "crc32.h"

//********************************************************************
//
// Function:
//  BOOL XL4PreScanCleanFilter()
//
// Parameters:
//  lpstOffcScan            Ptr to office scan structure
//
// Description:
//  Performs a pre-filter for clean files installed in the
//  Excel library directory of default office installations.
//
// Returns:
//  TRUE                    If no chance of infection
//  FALSE                   If possible chance of Paix infection
//
//********************************************************************

typedef struct tagXL_CLEAN_FILTER
{
    DWORD       dwStreamSize;
    DWORD       dwInfoOffset;
    DWORD       dwInfoSize;
    DWORD       dwInfoCRC;
} XL_CLEAN_FILTER_T, FAR *LPXL_CLEAN_FILTER;

#ifdef SYM_WIN16
#pragma data_seg("FAR_DATA_0")
#endif

XL_CLEAN_FILTER_T FAR gastXLCleanFilter[] =
{
    // 95: TMPLTNUM.XLA
    { 0x0000082A, 0x000004E2, 0x0000002F, 0x9AD1C8E5 },
    // 95: UPDTLINK.XLA
    // 42: UPDTLINK.XLA
    // 43: UPDTLINK.XLA
    { 0x00000D5A, 0x00000899, 0x0000002D, 0xBBDDB684 },
    // 50: UPDTLINK.XLA
    { 0x00000D99, 0x00000899, 0x00000027, 0xB0B27783 },
    // 97: UPDTLINK.XLA
    { 0x00000E45, 0x00000951, 0x00000027, 0x1FA97358 },
    // 97: TMPLTNUM.XLA
    { 0x00000E6D, 0x00000661, 0x00000031, 0xC45BE114 },
    // 95: COMMON.XLS
    { 0x00001000, 0x000004AC, 0x00000042, 0x63998CB6 },
    // 95: CRTDB.XLS
    // 95: EXPDB.XLS
    // 95: INVDB.XLS
    // 95: PODB.XLS
    // 95: SQDB.XLS
    // 95: TIMEDB.XLS
    { 0x00001000, 0x000004A8, 0x00000015, 0x15FE284D },
    // 97: EXPDB.XLS
    { 0x00001000, 0x00000674, 0x00000016, 0xD0465084 },
    // 97: INVDB.XLS
    { 0x00001000, 0x00000674, 0x00000016, 0x492569CE },
    // 97: PODB.XLS
    { 0x00001000, 0x00000674, 0x00000016, 0x41BC177F },
    // 97: COMMON.XLS
    { 0x000011C7, 0x00000694, 0x00000044, 0x63DB287F },
    // 50: SLIDES.XLT
    // 42: SLIDES.XLT
    // 43: SLIDES.XLT
    { 0x00001911, 0x00000D6B, 0x00000019, 0x1FCE70C9 },
    // 97: BSHXL.XLA
    { 0x000025A7, 0x0000071D, 0x000000AD, 0xA7DD402F },
    // 42: ADDINFNS.XLA
    // 43: ADDINFNS.XLA
    { 0x00002C4C, 0x00000EC3, 0x00000025, 0xC91DCA86 },
    // 50: ADDINFNS.XLA
    { 0x00002D3F, 0x00000EDD, 0x00000025, 0xF80C0D7C },
    // 97: XLQUERY.XLA
    { 0x00002DB1, 0x000007E4, 0x0000002C, 0x745D39F8 },
    // 95: ACCLINK.XLA
    { 0x00002F93, 0x00000DAF, 0x00000093, 0xD49AFEB4 },
    // 97: FILECONV.XLA
    { 0x00003B9C, 0x00000AF5, 0x00000041, 0x2B8D0842 },
    // 97: ACCLINK.XLA
    { 0x000052D7, 0x00000CE9, 0x0000009F, 0x68231235 },
    // 97: LOOKUP.XLA
    { 0x00005B00, 0x00000C96, 0x0000003F, 0x0AECEEF1 },
    // 97: SUMIF.XLA
    { 0x00005E6F, 0x00000CAC, 0x0000003E, 0x2DEB99FF },
    // 43: XLODBC.XLA
    { 0x000063FC, 0x00001BCD, 0x00000041, 0xDF405FD8 },
    // 50: XLODBC.XLA
    // 95: XLODBC.XLA
    // 97: XLODBC.XLA
    { 0x000070B9, 0x00001B62, 0x00000041, 0x76AF14BB },
    // 97: HTML.XLA
    { 0x0000727B, 0x00000BD6, 0x00000052, 0x69CE3580 },
    // 42: ATPVBAEN.XLA
    // 43: ATPVBAEN.XLA
    { 0x00008310, 0x00002E84, 0x00000033, 0xCC6B854E },
    // 97: ATPVBAEN.XLA
    { 0x00008835, 0x000009DA, 0x0000004A, 0x2C48EDB6 },
    // 42: ANALYSIS.XLA
    { 0x00008757, 0x00001B16, 0x00000025, 0x3BCE8C2A },
    // 43: ANALYSIS.XLA
    { 0x00008757, 0x00001B16, 0x00000025, 0x3BCE8C2A },
    // 50: ANALYSIS.XLA
    { 0x0000889A, 0x00001B40, 0x00000025, 0x5CA447D4 },
    // 50: AUTOSAVE.XLA
    // 42: AUTOSAVE.XLA
    // 43: AUTOSAVE.XLA
    { 0x00008993, 0x00001ED3, 0x0000002B, 0xC2F4BCCD },
    // 50: ATPVBAEN.XLA
    // 95: ATPVBAEN.XLA
    { 0x00008A5D, 0x0000327E, 0x00000047, 0x12BB3B11 },
    // 95: AUTOSAVE.XLA
    { 0x00009A53, 0x00001DED, 0x00000041, 0x0EEF1823 },
    // 97: AUTOSAVE.XLA
    { 0x00009A6F, 0x00000DF4, 0x00000044, 0x522DC57E },
    // 97: WEBFORM.XLA
    { 0x0000CD01, 0x00000CFA, 0x00000086, 0x53355BE6 },
    // 97: FUNCRES.XLA
    { 0x0000F22D, 0x000009FC, 0x00000013, 0xBE5CC185 },
    // 42: FUNCRES.XLA
    // 43: FUNCRES.XLA
    { 0x0000FE37, 0x00000B97, 0x00000012, 0x436BFFBF },
    // 50: FUNCRES.XLA
    { 0x0000FFB9, 0x00000D17, 0x00000012, 0x60FDCF80 },
    // 95: FUNCRES.XLA
    { 0x00010111, 0x00000E6F, 0x00000012, 0xA0792A42 },
    // 97: SOLVER.XLA
    { 0x000111D6, 0x000010FC, 0x00000143, 0x770CB3C3 },
    // 42: PROCDB.XLA
    // 43: PROCDB.XLA
    { 0x00015330, 0x00007B21, 0x00000021, 0x577BD732 },
    // 97: PROCDB.XLA
    { 0x000157A4, 0x000009CE, 0x00000023, 0xD60AE2C3 },
    // 50: PROCDB.XLA
    // 95: PROCDB.XLA
    { 0x00015D28, 0x00007D0D, 0x00000021, 0x4D7417B4 },
    // 50: VIEWS.XLA
    // 42: VIEWS.XLA
    // 43: VIEWS.XLA
    { 0x00018B67, 0x0000624E, 0x0000002C, 0xBCC28882 },
    // 42: ANALYSF.XLA
    // 43: ANALYSF.XLA
    { 0x00018F24, 0x00003A13, 0x00000024, 0xCEAEA93F },
    // 50: ANALYSF.XLA
    { 0x000191C1, 0x00003B94, 0x00000024, 0x3D793272 },
    // 95: VIEWS.XLA
    { 0x00019984, 0x000068C8, 0x0000004B, 0x48DE23F8 },
    // 50: REPORTS.XLA
    // 42: REPORTS.XLA
    // 43: REPORTS.XLA
    { 0x0001CC2D, 0x00005D71, 0x0000004E, 0x84B7E494 },
    // 95: REPORTS.XLA
    { 0x0001DA17, 0x0000649F, 0x0000006D, 0xDB1CB567 },
    // 97: REPORTS.XLA
    { 0x0001DEB5, 0x000015B9, 0x00000071, 0x858EB541 },
    // Mac 98: Report Manager
    { 0x0001E372, 0x000016AE, 0x00000071, 0xD579A56D },
    // 50: SLIDES.XLA
    // 42: SLIDES.XLA
    // 43: SLIDES.XLA
    { 0x0002273B, 0x00005368, 0x00000015, 0x7143AC42 },
    // 50: QE.XLA
    // 43: QE.XLA
    { 0x0002399A, 0x00003DA5, 0x00000011, 0xE079E3FE },
    // 43: XLQUERY.XLA
    { 0x000295B8, 0x00003AAD, 0x0000005F, 0x5246C57D },
    // 95: WZTEMPLT.XLA
    { 0x000295C9, 0x00000D58, 0x00000232, 0x4FDC85F8 },
    // 50: XLQUERY.XLA
    { 0x0002976A, 0x00003EFB, 0x0000005F, 0x86C399F2 },
    // 95: XLQUERY.XLA
    { 0x0002A48B, 0x00003F93, 0x0000005F, 0xCB662C38 },
    // 97: WZTEMPLT.XLA
    { 0x00034AEA, 0x00000DD3, 0x00000268, 0xD9FC38E5 },
    // 42: SOLVER.XLA
    // 43: SOLVER.XLA
    { 0x0003A91B, 0x0000A0CB, 0x0000006A, 0x79D45899 },
    // 50: SOLVER.XLA
    { 0x0003CB1E, 0x0000A5C2, 0x00000089, 0xAC7BAF26 },
    // 95: SOLVER.XLA
    { 0x0003CFEF, 0x0000A8FA, 0x00000089, 0xF93DE98E },
    // KPMG: C1040MAC.XLA
    { 0x000A870B, 0x0000CB9B, 0x0000002D, 0xC94C0C78 },
};

#ifdef SYM_WIN16
#pragma data_seg()
#endif

#define NUM_XL_CLEAN_FILTERS    \
    (sizeof(gastXLCleanFilter)/sizeof(XL_CLEAN_FILTER_T))

BOOL XL4PreScanCleanFilter
(
    LPMSOFFICE_SCAN         lpstOffcScan
)
{
    LPSS_STREAM             lpstStream;
    DWORD                   dwOffset;
    DWORD                   dwMaxOffset;
    DWORD                   dwSize;
    DWORD                   dwCandSize;
    DWORD                   dwByteCount;
    int                     nLow, nMid, nHigh;
    LPBYTE                  lpabyBuf;
    DWORD                   dwCRC;

    lpstStream = lpstOffcScan->lpstStream;
    dwSize = SSStreamLen(lpstStream);

    // Binary search for the size

    nLow = 0;
    nHigh = NUM_XL_CLEAN_FILTERS - 1;
    while (nLow <= nHigh)
    {
        nMid = (nLow + nHigh) >> 1;
        dwCandSize = gastXLCleanFilter[nMid].dwStreamSize;
        if (dwSize < dwCandSize)
            nHigh = nMid - 1;   // In lower half
        else
        if (dwSize > dwCandSize)
            nLow = nMid + 1;    // In upper half
        else
            break;              // Found a match
    }

    // Doesn't match size of a clean stream?

    if (nLow > nHigh)
        return(FALSE);

    // Find lower bound

    nLow = nMid;
    while (nLow != 0)
    {
        if (gastXLCleanFilter[nLow - 1].dwStreamSize != dwCandSize)
            break;

        --nLow;
    }

    // Find upper bound

    nHigh = nMid + 1;
    while (nHigh < NUM_XL_CLEAN_FILTERS)
    {
        if (gastXLCleanFilter[nHigh].dwStreamSize != dwCandSize)
            break;

        ++nHigh;
    }

    // Try CRC for all size matches

    for (nMid=nLow;nMid<nHigh;nMid++)
    {
        CRC32Init(dwCRC);

        dwSize = lpstOffcScan->lpstScan->nRunBufSize;
        lpabyBuf = lpstOffcScan->lpstScan->abyRunBuf;

        dwOffset = gastXLCleanFilter[nMid].dwInfoOffset;
        dwMaxOffset = dwOffset + gastXLCleanFilter[nMid].dwInfoSize;
        while (dwOffset < dwMaxOffset)
        {
            if (dwMaxOffset - dwOffset < dwSize)
                dwSize = dwMaxOffset - dwOffset;

            // Read a chunk

            if (SSSeekRead(lpstStream,
                           dwOffset,
                           lpabyBuf,
                           dwSize,
                           &dwByteCount) != SS_STATUS_OK ||
                dwByteCount != dwSize)
                return(FALSE);

            // CRC the chunk

            dwCRC = CRC32Compute(dwSize,lpabyBuf,dwCRC);

            // Move to next chunk

            dwOffset += dwSize;
        }

        // Does the CRC match.  If it does, assume clean.

        if (dwCRC == gastXLCleanFilter[nMid].dwInfoCRC)
            return(TRUE);
    }

    return(FALSE);
}


//********************************************************************
//
// Function:
//  BOOL XL4ScanSheet()
//
// Parameters:
//  wType                   Type of Book stream
//  lpstScan                Ptr to scan structure
//  dwOffset                Offset of BOUNDSHEET structure
//  stRec                   BOUNDSHEET record header
//  lpstBoundSheet          Ptr to BOUNDSHEET record
//
// Description:
//  Given a BOUNDSHEET record, the function scans the BOUNDSHEET
//  for the presence of Excel 4.0 macro viruses.
//
// Returns:
//  TRUE                    On successful scan
//  FALSE                   On error
//
//********************************************************************

BOOL XL4ScanSheet
(
    WORD                    wType,
    LPWD_SCAN               lpstScan,
    DWORD                   dwOffset,
    XL_REC_HDR_T            stRec,
    LPXL_REC_BOUNDSHEET     lpstBoundSheet
)
{
    int             i;
    int             nNameLen;

    // First, get the name

    if (lpstBoundSheet->byNameLen > XL_REC_BOUNDSHEET_MAX_NAME_LEN)
        nNameLen = XL_REC_BOUNDSHEET_MAX_NAME_LEN;
    else
        nNameLen = lpstBoundSheet->byNameLen;

    if (wType == OLE_SCAN_XL4_95)
    {
        if (XL5EncryptedRead(lpstScan->lpstStream,
                             dwOffset,
                             stRec,
                             lpstScan->uScan.stXL4.lpstKey,
                             sizeof(XL_REC_BOUNDSHEET_T),
                             lpstScan->abyName,
                             nNameLen) == FALSE)
            return(FALSE);
    }
    else
    {
        if (XL97ReadUnicodeStr(lpstScan->lpstStream,
                               dwOffset + sizeof(XL_REC_HDR_T) +
                                   sizeof(XL_REC_BOUNDSHEET_T),
                               TRUE,
                               lpstScan->abyName,
                               (WORD)nNameLen,
                               NULL) == FALSE)
            return(FALSE);
    }

    // Zero-terminate the name and uppercase it

    lpstScan->abyName[nNameLen] = 0;
    for (i=0;i<nNameLen;i++)
        lpstScan->abyName[i] = SSToUpper(lpstScan->abyName[i]);

    // Set the offset of the sheet's BOF record

    lpstScan->uScan.stXL4.dwBOFOffset =
        DWENDIAN(lpstBoundSheet->dwOffset);

    if (WDScanXL4Macro(&gstXL4SigSet,
                       lpstScan) != WD_STATUS_OK)
        return(FALSE);

    return(TRUE);
}


//********************************************************************
//
// Function:
//  BOOL XL4PaixScanFilter()
//
// Parameters:
//  lpstOffcScan            Ptr to office scan structure
//
// Description:
//  Performs a pre-filter for Paix.
//
// Returns:
//  TRUE                    If no chance of Paix infection
//  FALSE                   If possible chance of Paix infection
//
//********************************************************************

BOOL XL4PaixScanFilter
(
    LPMSOFFICE_SCAN         lpstOffcScan
)
{
    LPSS_STREAM             lpstStream;
    DWORD                   dwOffset;
    DWORD                   dwMaxOffset;
    DWORD                   dwBytesRead;
    int                     nBufSize;
    int                     nReadSize;
    LPBYTE                  lpabyBuf;
    int                     i;
    WORD                    wSize;

    lpstStream = lpstOffcScan->lpstStream;
    dwOffset = 0;
    dwMaxOffset = SSStreamLen(lpstStream);
    nBufSize = lpstOffcScan->lpstScan->nRunBufSize;
    lpabyBuf = lpstOffcScan->lpstScan->abyRunBuf;
    nReadSize = nBufSize;
    while (dwOffset < dwMaxOffset)
    {
        // Read a chunk

        if (dwMaxOffset - dwOffset < (DWORD)nReadSize)
            nReadSize = (int)(dwMaxOffset - dwOffset);

        if (nReadSize < 1000)
            return(TRUE);

        if (SSSeekRead(lpstStream,
                       dwOffset,
                       lpabyBuf,
                       nReadSize,
                       &dwBytesRead) != SS_STATUS_OK ||
            dwBytesRead != (DWORD)nReadSize)
        {
            // Error reading

            return(FALSE);
        }

        // Iterate through the buffer

        i = 0;
        wSize = 0;
        while (1)
        {
            // Make sure we don't overflow

            if ((DWORD)(i + 14) + wSize >= (DWORD)nReadSize)
            {
                dwOffset += i + (DWORD)wSize;
                break;
            }

            i += wSize;

            // Check for EOF

            if (lpabyBuf[i] == 0x0A && lpabyBuf[i+1] == 0x00)
                return(TRUE);

            // Check for BOUNDSHEET with '!' beginning name

            if (lpabyBuf[i] == 0x85 && lpabyBuf[i+1] == 0x00 &&
                (lpabyBuf[i+9] & XL_REC_BOUNDSHEET_40_MACRO) != 0 &&
                lpabyBuf[i+10] == 5 &&
                (lpabyBuf[i+11] == '!' || lpabyBuf[i+12] == '!'))
                return(FALSE);

            wSize = lpabyBuf[i+2] | ((WORD)lpabyBuf[i+3] << 8);
            i += 4;
        }
    }

    return(TRUE);
}


//********************************************************************
//
// Function:
//  BOOL XL4GetBoundsheetOffset()
//
// Parameters:
//  lpstOffcScan            Ptr to office scan structure
//  lpdwBoundsheetOffset    Ptr to DWORD for boundsheet offset
//
// Description:
//  Finds the offset of the first boundsheet record.  If the
//  document is an Excel 97 document and is password protected,
//  the function will return FALSE.
//
// Returns:
//  TRUE                    On success
//  FALSE                   On error or no boundsheet record found
//
//********************************************************************

BOOL XL4GetBoundsheetOffset
(
    LPMSOFFICE_SCAN         lpstOffcScan,
    LPDWORD                 lpdwBoundsheetOffset
)
{
    LPSS_STREAM             lpstStream;
    DWORD                   dwOffset;
    DWORD                   dwMaxOffset;
    DWORD                   dwBytesRead;
    int                     nBufSize;
    int                     nReadSize;
    LPBYTE                  lpabyBuf;
    int                     i;
    WORD                    wSize;

    lpstStream = lpstOffcScan->lpstStream;
    dwOffset = 0;
    dwMaxOffset = SSStreamLen(lpstStream);
    nBufSize = lpstOffcScan->lpstScan->nRunBufSize;
    lpabyBuf = lpstOffcScan->lpstScan->abyRunBuf;
    nReadSize = nBufSize;
    while (dwOffset < dwMaxOffset)
    {
        // Read a chunk

        if (dwMaxOffset - dwOffset < (DWORD)nReadSize)
            nReadSize = (int)(dwMaxOffset - dwOffset);

        if (nReadSize < 1000)
            return(FALSE);

        if (SSSeekRead(lpstStream,
                       dwOffset,
                       lpabyBuf,
                       nReadSize,
                       &dwBytesRead) != SS_STATUS_OK ||
            dwBytesRead != (DWORD)nReadSize)
        {
            // Error reading

            return(FALSE);
        }

        // Iterate through the buffer

        i = 0;
        wSize = 0;
        while (1)
        {
            // Make sure we have enough bytes for the type
            //  and length fields of the record header plus the
            //  size of the previous record's data

            if ((DWORD)(i + 4) + wSize > (DWORD)nReadSize)
            {
                dwOffset += i + (DWORD)wSize;
                break;
            }

            // Increment past the previous record's data

            i += wSize;

            // Check for EOF

            if (lpabyBuf[i+1] == 0x00)
            {
                switch (lpabyBuf[i])
                {
                    case eXLREC_EOF:
                        // Couldn't find a BOUNDSHEET record

                        return(FALSE);

                    case eXLREC_BOUNDSHEET:
                        // Found a BOUNDSHEET

                        *lpdwBoundsheetOffset = dwOffset + i;
                        return(TRUE);

                    case eXLREC_FILEPASS:
                        // Return FALSE if it is an Excel 97 document
                        //  since we can't scan the BOUNDSHEETS
                        //  anyway

                        if (lpstOffcScan->wType != OLE_SCAN_XL4_95)
                            return(FALSE);

                        break;

                    default:
                        break;
                }
            }

            // Get the size of the record's data

            wSize = lpabyBuf[i+2] | ((WORD)lpabyBuf[i+3] << 8);

            // Increment past the record's header

            i += 4;
        }
    }

    return(FALSE);
}


//********************************************************************
//
// Function:
//  EXTSTATUS XL4Scan()
//
// Description:
//  Scans a file for the presence of Excel 4.0 viruses.
//
// Returns:
//	EXTSTATUS_OK			If no virus was found
//	EXTSTATUS_VIRUS_FOUND   If a virus was found
//
//********************************************************************

EXTSTATUS XL4Scan
(
    LPMSOFFICE_SCAN         lpstOffcScan,
    LPLPWD_VIRUS_SIG_INFO   lplpstVirusSigInfo
)
{
    LPWD_SCAN               lpstScan;
    DWORD                   dwOffset;
    DWORD                   dwMaxOffset;
    XL5ENCKEY_T             stKey;
    XL_REC_BOUNDSHEET_T     stBoundSheet;
    LPSS_STREAM             lpstStream;
    XL_REC_HDR_T            stRec;
    DWORD                   dwByteCount;

    // No Excel 4 signatures

    if (gstXL4SigSet.lNumVirusSigs == 0)
    {
        return(EXTSTATUS_OK);
    }

    lpstScan = lpstOffcScan->lpstScan;
    lpstStream = lpstOffcScan->lpstStream;

    // Assign hit memory

    if (AssignMacroSigHitMem(lpstOffcScan,
                             lpstScan) == FALSE)
    {
        return(EXTSTATUS_MEM_ERROR);
    }

    // Initialize hit memory

    WDInitHitBitArrays(&gstXL4SigSet,lpstScan);

    // Initialize other scan structure fields

    lpstScan->lpstStream = lpstStream;

    // Initialize macro count

    lpstScan->wMacroCount = 0;

    // Disable full set

    lpstScan->wFlags = 0;

    // Get the key for the stream

    if (lpstOffcScan->wType == OLE_SCAN_XL4_95)
    {
        lpstScan->wScanType = WD_SCAN_TYPE_XL4_95;
        if (XL5FindKey(lpstStream,
                       &stKey) == FALSE)
            return(EXTSTATUS_MEM_ERROR);

        lpstScan->uScan.stXL4.lpstKey = &stKey;
    }
    else
    {
        lpstScan->wScanType = WD_SCAN_TYPE_XL4_97;
        stKey.bEncrypted = FALSE;
        lpstScan->uScan.stXL4.lpstKey = NULL;
    }

    // Do a quick filter scan if not encrypted

    if (stKey.bEncrypted == FALSE)
    {
        // Check against clean files

        if (XL4PreScanCleanFilter(lpstOffcScan) == TRUE)
            return(EXTSTATUS_OK);

        // Check for paix, assuming that it is the only one

        if (gstXL4SigSet.lNumVirusSigs == 1)
        {
            if (XL4PaixScanFilter(lpstOffcScan) == TRUE)
                return(EXTSTATUS_OK);
        }
    }

    // Get the offset of the first BOUNDSHEET record

    if (XL4GetBoundsheetOffset(lpstOffcScan,
                               &dwOffset) == FALSE)
        return(EXTSTATUS_OK);

    // Iterate through the global workbook records, looking
    //  for boundsheet records that are Excel 4.0 sheets

    dwMaxOffset = SSStreamLen(lpstStream);
    while (dwOffset < dwMaxOffset)
    {
        if (SSSeekRead(lpstStream,
                       dwOffset,
                       &stRec,
                       sizeof(XL_REC_HDR_T),
                       &dwByteCount) != SS_STATUS_OK ||
            dwByteCount != sizeof(XL_REC_HDR_T))
            return(WD_STATUS_ERROR);

        stRec.wType = WENDIAN(stRec.wType);
        stRec.wLen = WENDIAN(stRec.wLen);

        // If it is an Excel 97 document and we see a FILEPASS
        //  record, we know it is encrypted, so just return

        if (stRec.wType == eXLREC_FILEPASS &&
            lpstScan->wScanType == WD_SCAN_TYPE_XL4_97)
            return(EXTSTATUS_OK);

        if (stRec.wType == eXLREC_EOF)
            break;

        if (stRec.wType == eXLREC_BOUNDSHEET)
        {
            // Read the BOUNDSHEET record

            if (XL5EncryptedRead(lpstStream,
                                 dwOffset,
                                 stRec,
                                 &stKey,
                                 0,
                                 (LPBYTE)&stBoundSheet,
                                 sizeof(XL_REC_BOUNDSHEET_T)) != FALSE)
            {
                // Is it an Excel 4.0 sheet?

                if ((stBoundSheet.bySheetType &
                     XL_REC_BOUNDSHEET_TYPE_MASK) ==
                    XL_REC_BOUNDSHEET_40_MACRO)
                {
                    // Increment macro count

                    lpstScan->wMacroCount++;

                    if (XL4ScanSheet(lpstOffcScan->wType,
                                     lpstScan,
                                     dwOffset,
                                     stRec,
                                     &stBoundSheet) == FALSE)
                    {
                        // Just continue
                    }
                }
            }
        }

        dwOffset += (DWORD)sizeof(XL_REC_HDR_T) + stRec.wLen;
    }

    // Update the long scan count

    lpstOffcScan->dwLongScanCount += 3 * lpstScan->wMacroCount;
    lpstOffcScan->dwLongScanCount += dwOffset / 16384;


    /////////////////////////////////////////////////////////////
    // Do virus check
    /////////////////////////////////////////////////////////////

    // Apply virus signatures

    if (WDApplyVirusSignatures(&gstXL4SigSet,
                               lpstScan,
                               lplpstVirusSigInfo) == WD_STATUS_VIRUS_FOUND)
    {
        return(EXTSTATUS_VIRUS_FOUND);
    }

    // No virus

    return (EXTSTATUS_OK);
}



