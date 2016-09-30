//************************************************************************
//
// $Header:   S:/NAVEX/VCS/xl4mrep.cpv   1.2   30 Jan 1998 19:03:36   DCHI  $
//
// Description:
//   Contains Excel 5.0/95/97 Excel 4.0 macro repair function code.
//
//************************************************************************
// $Log:   S:/NAVEX/VCS/xl4mrep.cpv  $
// 
//    Rev 1.2   30 Jan 1998 19:03:36   DCHI
// For 5.0/95 documents, go through and no-op NAME records referring
// to the sheet.
// 
//    Rev 1.1   30 Jan 1998 15:20:22   DCHI
// Removed unnecessary RepairFileClose() call.
// 
//    Rev 1.0   29 Jan 1998 19:05:24   DCHI
// Initial revision.
// 
//************************************************************************

#if defined(NAVEX15) || !defined(SYM_NLM)

#include "platform.h"

#ifdef WIN16        // VIRSCAN QUAKE 2
#include "cts4.h"
#else
#include "ctsn.h"
#include "callback.h"
#endif

#include "navex.h"

#include "navexshr.h"
#include "macrovid.h"

#include "wd7api.h"
#include "o97api.h"
#include "xlrec.h"

#include "wdscan15.h"
#include "offcsc15.h"
#include "xl5scn15.h"
#include "excelrep.h"

#include "xl4mrep.h"

#define EXCEL_BOF_TYPE_GLOBALS      0x0005
#define EXCEL_BOF_TYPE_VB_MODULE    0x0006
#define EXCEL_BOF_TYPE_WORKSHEET    0x0010
#define EXCEL_BOF_TYPE_CHART        0x0020
#define EXCEL_BOF_TYPE_40_MACRO     0x0040
#define EXCEL_BOF_TYPE_WORKSPACE    0x0100

// Parameters:
//  lpstStream      Ptr to book stream
//  lpabySheetName  Pascal sheet name

BOOL ExcelNOPAutoNames
(
    LPSS_STREAM     lpstStream,
    LPBYTE          lpabySheetName
)
{
    XL_REC_HDR_T    stRec;
    DWORD           dwByteCount;
    DWORD           dwBytesToWrite;
    DWORD           dwMaxOffset;
    WORD            w;
    DWORD           dwOffset;
    BYTE            abyBuf[256];
    WORD            wSheetIndex;
    WORD            wViralSheetIndex;

    dwOffset = 0;
    dwMaxOffset = SSStreamLen(lpstStream);

    // Go through the EXTERNSHEET records, looking for the index
    //  with the given sheet name

    wSheetIndex = 0;
    wViralSheetIndex = 0;
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

        if (stRec.wType == eXLREC_EXTERNSHEET)
        {
            ++wSheetIndex;

            // Read the current name length

            if (SSSeekRead(lpstStream,
                           dwOffset + sizeof(XL_REC_HDR_T),
                           abyBuf,
                           sizeof(BYTE),
                           &dwByteCount) != SS_STATUS_OK ||
                dwByteCount != sizeof(BYTE))
                return(FALSE);

            if (abyBuf[0] == lpabySheetName[0])
            {
                // Read the name

                if (SSSeekRead(lpstStream,
                               dwOffset + sizeof(XL_REC_HDR_T) + 2,
                               abyBuf+1,
                               abyBuf[0],
                               &dwByteCount) != SS_STATUS_OK ||
                    dwByteCount != abyBuf[0])
                    return(FALSE);

                // Compare the names

                for (w=1;w<=abyBuf[0];w++)
                    if (abyBuf[w] != lpabySheetName[w])
                        break;

                if (w > abyBuf[0])
                {
                    // This is it

                    wViralSheetIndex = wSheetIndex;
                    break;
                }
            }
        }
        else
        if (stRec.wType == eXLREC_EOF)
            break;

        dwOffset += sizeof(XL_REC_HDR_T) + stRec.wLen;
    }

    if (wViralSheetIndex == 0)
        return(FALSE);

    // Now iterate through and change all NAME records with the
    //  given index to NOP records

    dwOffset = 0;
    dwMaxOffset = SSStreamLen(lpstStream);

    // Create zero buffer

    for (w=0;w<256;w++)
        abyBuf[w] = 0;

    // Go through the NAME records, changing built-in names
    //  to be not built-in names

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

        if (stRec.wType == eXLREC_NAME ||
            stRec.wType == eXLREC_NAME2)
        {
            // Read the ixals field

            if (SSSeekRead(lpstStream,
                           dwOffset + sizeof(XL_REC_HDR_T) + 6,
                           &w,
                           sizeof(WORD),
                           &dwByteCount) != SS_STATUS_OK ||
                dwByteCount != sizeof(WORD))
                return(FALSE);

            w = WENDIAN(w);

            if (w == wViralSheetIndex)
            {
                w = WENDIAN(eXLREC_PROTECT);

                // Set the type to NOP

                if (SSSeekWrite(lpstStream,
                                dwOffset,
                                &w,
                                sizeof(WORD),
                                &dwByteCount) != SS_STATUS_OK ||
                    dwByteCount != sizeof(WORD))
                    return(FALSE);

                // Zero out the rest of the record

                w = 0;
                dwBytesToWrite = sizeof(abyBuf);
                while (w < stRec.wLen)
                {
                    if (dwBytesToWrite > (stRec.wLen - w))
                        dwBytesToWrite = stRec.wLen - w;

                    if (SSSeekWrite(lpstStream,
                                    dwOffset + sizeof(XL_REC_HDR_T) + w,
                                    abyBuf,
                                    dwBytesToWrite,
                                    &dwByteCount) != SS_STATUS_OK ||
                        dwByteCount != dwBytesToWrite)
                        return(FALSE);

                    w += (WORD)dwBytesToWrite;
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

// Parameters:
//  lpstStream      Ptr to book stream

BOOL ExcelDisableBuiltInNames
(
    LPSS_STREAM     lpstStream
)
{
    XL_REC_HDR_T    stRec;
    DWORD           dwByteCount;
    DWORD           dwMaxOffset;
    WORD            w;
    DWORD           dwOffset;

    dwOffset = 0;
    dwMaxOffset = SSStreamLen(lpstStream);

    // Go through the NAME records, changing built-in names
    //  to be not built-in names

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

        if (stRec.wType == eXLREC_NAME ||
            stRec.wType == eXLREC_NAME2)
        {
            // Read the current option flags

            if (SSSeekRead(lpstStream,
                           dwOffset + sizeof(XL_REC_HDR_T),
                           &w,
                           sizeof(WORD),
                           &dwByteCount) != SS_STATUS_OK ||
                dwByteCount != sizeof(WORD))
                return(FALSE);

            w = WENDIAN(w);

            if (w & 0x0020)
            {
                w &= ~(0x0020);
                w = WENDIAN(w);

                // Write the new option flags

                if (SSSeekWrite(lpstStream,
                                dwOffset + sizeof(XL_REC_HDR_T),
                                &w,
                                sizeof(WORD),
                                &dwByteCount) != SS_STATUS_OK ||
                    dwByteCount != sizeof(WORD))
                    return(FALSE);
            }
        }
        else
        if (stRec.wType == eXLREC_EOF)
            break;

        dwOffset += sizeof(XL_REC_HDR_T) + stRec.wLen;
    }

    return(TRUE);
}


// Parameters:
//  lpstStream      Ptr to book stream
//  dwOffset        Offset of sheet

BOOL ExcelSheetBlankVirusCells
(
    LPSS_STREAM     lpstStream,
    DWORD           dwOffset
)
{
    XL_REC_HDR_T    stRec;
    DWORD           dwByteCount;
    DWORD           dwBytesToWrite;
    DWORD           dwMaxOffset;
    WORD            w;
    BYTE            abyBuf[256];

    // The first record should be a BOF record

    if (SSSeekRead(lpstStream,
                   dwOffset,
                   &stRec,
                   sizeof(XL_REC_HDR_T),
                   &dwByteCount) != SS_STATUS_OK ||
        dwByteCount != sizeof(XL_REC_HDR_T))
        return(FALSE);

    stRec.wType = WENDIAN(stRec.wType);
    stRec.wLen = WENDIAN(stRec.wLen);

    if (stRec.wType != eXLREC_BOF)
        return(FALSE);

    // Set the substream type to worksheet

    w = WENDIAN(EXCEL_BOF_TYPE_WORKSHEET);

    if (SSSeekWrite(lpstStream,
                    dwOffset + 6,
                    &w,
                    sizeof(WORD),
                    &dwByteCount) != SS_STATUS_OK ||
        dwByteCount != sizeof(WORD))
        return(FALSE);

    for (w=0;w<256;w++)
        abyBuf[w] = 0;

    dwMaxOffset = SSStreamLen(lpstStream);

    // Go through the FORMULA and LABEL records, changing them
    //  to BLANK records

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

        if (stRec.wType == eXLREC_FORMULA ||
            stRec.wType == eXLREC_LABEL ||
            stRec.wType == eXLREC_LABEL2)
        {
            // Change record type to BLANK

            w = WENDIAN(eXLREC_BLANK);

            // Write back out the record header

            if (SSSeekWrite(lpstStream,
                            dwOffset,
                            &w,
                            sizeof(WORD),
                            &dwByteCount) != SS_STATUS_OK ||
                dwByteCount != sizeof(WORD))
                return(FALSE);

            // Zero out contents

            w = 6;
            dwBytesToWrite = sizeof(abyBuf);
            while (w < stRec.wLen)
            {
                if (dwBytesToWrite > (stRec.wLen - w))
                    dwBytesToWrite = stRec.wLen - w;

                if (SSSeekWrite(lpstStream,
                                dwOffset + sizeof(XL_REC_HDR_T) + w,
                                abyBuf,
                                dwBytesToWrite,
                                &dwByteCount) != SS_STATUS_OK ||
                    dwByteCount != dwBytesToWrite)
                    return(FALSE);

                w += (WORD)dwBytesToWrite;
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
//  WORD Excel4MacroVirusRepair()
//
// Description:
//  Repairs the first stream found that is infected with the Excel 4.0
//  macro virus identified by VID in Excel 5.0/95/97 documents.
//
// Returns:
//  EXTSTATUS_OK            If the repair was successful
//  EXTSTATUS_NO_REPAIR     If the repair was unsuccessful
//
//*************************************************************************

EXTSTATUS Excel4MacroVirusRepair
(
    LPMSOFFICE_SCAN lpstOffcScan,
    LPCALLBACKREV1  lpCallBack,             // File op callbacks
    WORD            wVID,                   // ID of virus to repair
    LPTSTR          lpszFileName,           // Infected file's name
    HFILE           hFile,                  // Handle to file to repair
    UINT            uAttr,
    LPBYTE          lpbyWorkBuffer          // Work buffer >= 1024 bytes
)
{
    DWORD           dwOffset;
    BYTE            abySheetName[257];
    DWORD           dwBytesRead;
    WORD            wResult;

    wResult = 1;

    // Read the sheet name

    if (SSSeekRead(lpstOffcScan->lpstStream,
                   lpstOffcScan->u.stXL4.dwBoundSheetRecOffset + 10,
                   abySheetName,
                   256,
                   &dwBytesRead) != SS_STATUS_OK ||
        dwBytesRead != 256)
    {
        // Error reading

        wResult = 0;
    }

    if (wResult != 0 && lpstOffcScan->wType == OLE_SCAN_XL4_97)
    {
        if (SSSeekRead(lpstOffcScan->lpstStream,
                       lpstOffcScan->u.stXL4.dwBoundSheetRecOffset + 12,
                       abySheetName + 1,
                       abySheetName[0],
                       &dwBytesRead) != SS_STATUS_OK ||
            dwBytesRead != abySheetName[0])
        {
            // Error reading

            wResult = 0;
        }
    }

    if (wResult != 0)
    {
        // Zero-terminate the sheet name

        abySheetName[abySheetName[0]+1] = 0;

        // Remove the module

        wResult = ExcelBooksRemoveModule(lpstOffcScan->lpstRoot,
                                         lpstOffcScan->lpstStream,
                                         lpbyWorkBuffer,
                                         abySheetName,
                                         TRUE);
    }

    if (wResult != 0)
    {
        // Get the offset of the sheet

        if (SSSeekRead(lpstOffcScan->lpstStream,
                       lpstOffcScan->u.stXL4.dwBoundSheetRecOffset + 4,
                       &dwOffset,
                       sizeof(DWORD),
                       &dwBytesRead) != SS_STATUS_OK ||
            dwBytesRead != sizeof(DWORD))
        {
            // Error reading

            wResult = 0;
        }
        else
            dwOffset = DWENDIAN(dwOffset);
    }

    if (wResult != 0)
    {
        if (ExcelSheetBlankVirusCells(lpstOffcScan->lpstStream,
                                      dwOffset) == FALSE)
            wResult = 0;
    }

    if (wResult != 0)
    {
        if (lpstOffcScan->wType == OLE_SCAN_XL4_97)
        {
            if (ExcelDisableBuiltInNames(lpstOffcScan->lpstStream) == FALSE)
                wResult = 0;
        }
        else
        {
            if (ExcelNOPAutoNames(lpstOffcScan->lpstStream,
                                  abySheetName) == FALSE)
                wResult = 0;
        }
    }

    if (wResult == 0)
    {
        return(EXTSTATUS_NO_REPAIR);
    }

    return(EXTSTATUS_OK);
}

#endif  // #ifndef SYM_NLM
