//************************************************************************
//
// $Header:   S:/NAVEX/VCS/xl4mscan.cpv   1.1   25 Mar 1998 15:02:00   DCHI  $
//
// Description:
//  Contains Excel 5.0/95/97 Excel 4.0 macro scan function code.
//
//************************************************************************
// $Log:   S:/NAVEX/VCS/xl4mscan.cpv  $
// 
//    Rev 1.1   25 Mar 1998 15:02:00   DCHI
// Added clean XL4 filter and no Paix filter.
// 
//    Rev 1.0   29 Jan 1998 19:05:26   DCHI
// Initial revision.
// 
//************************************************************************

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
#include "xlrec.h"

#include "wdscan15.h"
#include "offcsc15.h"

#include "xl4mscan.h"
#include "macrovid.h"
#include "crc32.h"

// path_xla="C:\WINDOWS"

BYTE FAR gabyXMPaix95Str0[] =
{
    26,
    0x17+1, 0x08+1, 0x70+1, 0x61+1, 0x74+1, 0x68+1, 0x5F+1, 0x78+1,
    0x6C+1, 0x61+1, 0x17+1, 0x0A+1, 0x43+1, 0x3A+1, 0x5C+1, 0x57+1,
    0x49+1, 0x4E+1, 0x44+1, 0x4F+1, 0x57+1, 0x53+1, 0x42+1, 0x02+1,
    0x58+1, 0x00+1
};

// VBA.MAKE.ADDIN(path_xla&"\xlsheet.xla")

BYTE FAR gabyXMPaix95Str1[] =
{
    19,
    0x17+1, 0x0C+1, 0x5C+1, 0x78+1, 0x6C+1, 0x73+1, 0x68+1, 0x65+1,
    0x65+1, 0x74+1, 0x2E+1, 0x78+1, 0x6C+1, 0x61+1, 0x08+1, 0x42+1,
    0x01+1, 0xDE+1, 0x81+1,
};

// APP.TITLE("Enfin la paix...")

BYTE FAR gabyXMPaix95Str2[] =
{
    23,
    0x17+1, 0x11+1, 0x45+1, 0x6E+1, 0x66+1, 0x69+1, 0x6E+1, 0x20+1,
    0x6C+1, 0x61+1, 0x20+1, 0x70+1, 0x61+1, 0x69+1, 0x78+1, 0x20+1,
    0x2E+1, 0x2E+1, 0x2E+1, 0x42+1, 0x01+1, 0x06+1, 0x01+1
};

LPBYTE FAR galpbyXMPaix95[] =
{
    gabyXMPaix95Str0,
    gabyXMPaix95Str1,
    gabyXMPaix95Str2
};

// path_xla="C:\WINDOWS"

BYTE FAR gabyXMPaix97Str0[] =
{
    28,
    0x17+1, 0x08+1, 0x00+1, 0x70+1, 0x61+1, 0x74+1, 0x68+1, 0x5F+1,
    0x78+1, 0x6C+1, 0x61+1, 0x17+1, 0x0A+1, 0x00+1, 0x43+1, 0x3A+1,
    0x5C+1, 0x57+1, 0x49+1, 0x4E+1, 0x44+1, 0x4F+1, 0x57+1, 0x53+1,
    0x42+1, 0x02+1, 0x58+1, 0x00+1
};

// VBA.MAKE.ADDIN(path_xla&"\xlsheet.xla")

BYTE FAR gabyXMPaix97Str1[] =
{
    20,
    0x17+1, 0x0C+1, 0x00+1, 0x5C+1, 0x78+1, 0x6C+1, 0x73+1, 0x68+1,
    0x65+1, 0x65+1, 0x74+1, 0x2E+1, 0x78+1, 0x6C+1, 0x61+1, 0x08+1,
    0x42+1, 0x01+1, 0xDE+1, 0x81+1,
};

// APP.TITLE("Enfin la paix...")

BYTE FAR gabyXMPaix97Str2[] =
{
    24,
    0x17+1, 0x11+1, 0x00+1, 0x45+1, 0x6E+1, 0x66+1, 0x69+1, 0x6E+1,
    0x20+1, 0x6C+1, 0x61+1, 0x20+1, 0x70+1, 0x61+1, 0x69+1, 0x78+1,
    0x20+1, 0x2E+1, 0x2E+1, 0x2E+1, 0x42+1, 0x01+1, 0x06+1, 0x01+1
};

LPBYTE FAR galpbyXMPaix97[] =
{
    gabyXMPaix97Str0,
    gabyXMPaix97Str1,
    gabyXMPaix97Str2
};

//********************************************************************
//
// Function:
//  WORD XL4ScanMacroSheet()
//
// Parameters:
//  lpstStream              Ptr to stream containing sheet
//  dwOffset                Sheet offset
//  lpalpabySig             The signature to use
//
// Description:
//  Scans a sheet for the presence of Excel 4.0 macro viruses.
//
//  If a virus was found, the function returns the VID.
//
// Returns:
//  0                        If no virus was found
//  wVID                    If a virus was found
//
//********************************************************************

WORD XL4ScanMacroSheet
(
    LPSS_STREAM             lpstStream,
    DWORD                   dwOffset,
    LPLPBYTE                lpalpabySig
)
{
    DWORD                   dwMaxOffset;
    XL_REC_HDR_T            stRec;
    DWORD                   dwBytesRead;
    WORD                    wLen;
    BYTE                    abyBuf[256];
    int                     i, j, k;
    BYTE                    bySigHit;
    LPBYTE                  lpabySig;

    bySigHit = 0;
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
            // Error reading

            return(EXTSTATUS_OK);
        }

        stRec.wType = WENDIAN(stRec.wType);
        stRec.wLen = WENDIAN(stRec.wLen);

        if (stRec.wType == eXLREC_FORMULA)
        {
            // Get the length of the parsed expression

            if (SSSeekRead(lpstStream,
                           dwOffset + 24,
                           &wLen,
                           sizeof(WORD),
                           &dwBytesRead) == SS_STATUS_OK &&
                dwBytesRead == sizeof(WORD))
            {
                wLen = WENDIAN(wLen);

                if (wLen >= 19)
                {
                    // Read up to 256 bytes of the formula

                    if (wLen > sizeof(abyBuf))
                        wLen = sizeof(abyBuf);

                    if (SSSeekRead(lpstStream,
                                   dwOffset + 26,
                                   abyBuf,
                                   wLen,
                                   &dwBytesRead) == SS_STATUS_OK &&
                        dwBytesRead == wLen)
                    {
                        // Scan the buffer

                        for (i=0;i<wLen;i++)
                        {
                            if (abyBuf[i] == 0x17)
                            {
                                if ((bySigHit & 0x01) == 0)
                                {
                                    // Try the first string

                                    lpabySig = lpalpabySig[0];
                                    for (j=2,k=i+1;j<=lpabySig[0] && k<wLen;j++,k++)
                                    {
                                        if ((BYTE)(lpabySig[j]-1) != abyBuf[k])
                                            break;
                                    }

                                    if (j > lpabySig[0])
                                    {
                                        // Match

                                        bySigHit |= 0x01;
                                        if (bySigHit == 7)
                                            return(VID_PAIX);

                                        break;
                                    }
                                }
                                if ((bySigHit & 0x02) == 0)
                                {
                                    // Try the first string

                                    lpabySig = lpalpabySig[1];
                                    for (j=2,k=i+1;j<=lpabySig[0] && k<wLen;j++,k++)
                                    {
                                        if ((BYTE)(lpabySig[j]-1) != abyBuf[k])
                                            break;
                                    }

                                    if (j > lpabySig[0])
                                    {
                                        // Match

                                        bySigHit |= 0x02;
                                        if (bySigHit == 7)
                                            return(VID_PAIX);

                                        break;
                                    }
                                }
                                if ((bySigHit & 0x04) == 0)
                                {
                                    // Try the first string

                                    lpabySig = lpalpabySig[2];
                                    for (j=2,k=i+1;j<=lpabySig[0] && k<wLen;j++,k++)
                                    {
                                        if ((BYTE)(lpabySig[j]-1) != abyBuf[k])
                                            break;
                                    }

                                    if (j > lpabySig[0])
                                    {
                                        // Match

                                        bySigHit |= 0x04;
                                        if (bySigHit == 7)
                                            return(VID_PAIX);

                                        break;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        else
        if (stRec.wType == eXLREC_EOF)
            break;

        dwOffset += sizeof(XL_REC_HDR_T) + stRec.wLen;
    }

    return(0);
}


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

XL_CLEAN_FILTER_T gastXLCleanFilter[] =
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
};

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

        if (dwMaxOffset - dwOffset < nReadSize)
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
//  EXTSTATUS XL4MacroScan()
//
// Parameters:
//  lpstOffcScan            Ptr to office scan structure
//
// Description:
//  Scans a file for the presence of Excel 4.0 macro viruses
//  in the document stream of Excel 5.0/95/97 documents.
//
//  If a virus was found, the function sets
//  lpstOffcScan->u.stXL4.wVID to the VID of the virus.
//
// Returns:
//	EXTSTATUS_OK			If no virus was found
//	EXTSTATUS_VIRUS_FOUND   If a virus was found
//
//********************************************************************

EXTSTATUS XL4MacroScan
(
    LPMSOFFICE_SCAN         lpstOffcScan
)
{
    LPSS_STREAM             lpstStream;
    DWORD                   dwOffset;
    DWORD                   dwMaxOffset;
    XL_REC_HDR_T            stRec;
    DWORD                   dwBytesRead;
    BYTE                    bySheetType;
    DWORD                   dwSheetOffset;

    if (XL4PreScanCleanFilter(lpstOffcScan) == TRUE)
    {
        // No chance of Paix infection

        return(EXTSTATUS_OK);
    }

    if (XL4PaixScanFilter(lpstOffcScan) == TRUE)
    {
        // No chance of Paix infection

        return(EXTSTATUS_OK);
    }

    // Search for all Excel 4.0 macro sheets

    lpstStream = lpstOffcScan->lpstStream;
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
            // Error reading

            return(EXTSTATUS_OK);
        }

        stRec.wType = WENDIAN(stRec.wType);
        stRec.wLen = WENDIAN(stRec.wLen);

        if (stRec.wType == eXLREC_BOUNDSHEET)
        {
            if (SSSeekRead(lpstStream,
                           dwOffset + 9,
                           &bySheetType,
                           sizeof(BYTE),
                           &dwBytesRead) == SS_STATUS_OK &&
                dwBytesRead == sizeof(BYTE))
            {
                if (bySheetType & 0x01)
                {
                    // It is an Excel 4.0 macro sheet

                    // Get the offset of the sheet

                    if (SSSeekRead(lpstStream,
                                   dwOffset + sizeof(XL_REC_HDR_T),
                                   &dwSheetOffset,
                                   sizeof(DWORD),
                                   &dwBytesRead) == SS_STATUS_OK &&
                        dwBytesRead == sizeof(DWORD))
                    {
                        dwSheetOffset = DWENDIAN(dwSheetOffset);

                        lpstOffcScan->u.stXL4.wVID =
                            XL4ScanMacroSheet(lpstStream,
                                              dwSheetOffset,
                                              (lpstOffcScan->wType == OLE_SCAN_XL4_95) ?
                                                  galpbyXMPaix95 :
                                                  galpbyXMPaix97);

                        if (lpstOffcScan->u.stXL4.wVID != 0)
                        {
                            lpstOffcScan->u.stXL4.dwBoundSheetRecOffset =
                                dwOffset;
                            return(EXTSTATUS_VIRUS_FOUND);
                        }
                    }
                }
            }
        }
        else
        if (stRec.wType == eXLREC_EOF)
            break;

        dwOffset += sizeof(XL_REC_HDR_T) + stRec.wLen;
    }

    // No virus

    return(EXTSTATUS_OK);
}



