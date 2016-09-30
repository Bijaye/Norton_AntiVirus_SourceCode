// Copyright 1994 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/NAVEX/VCS/trojscan.cpv   1.20   15 Oct 1998 15:38:46   MKEATIN  $
//
// Description:
//
// Contains: Trojan scanning code.
//
// See Also:
//************************************************************************
// $Log:   S:/NAVEX/VCS/trojscan.cpv  $
// 
//    Rev 1.20   15 Oct 1998 15:38:46   MKEATIN
// #ifdef-ed the ThreadSwitch statements for SYM_NLM.
// 
//    Rev 1.18   14 Oct 1998 12:10:20   MKEATIN
// Carey's ThreadSwitch() statements for NLM.
// 
//    Rev 1.17   14 May 1998 19:51:36   CNACHEN
// Fixed header read bug.
// 
// 
//    Rev 1.16   14 May 1998 18:44:02   CNACHEN
// 
//    Rev 1.15   14 May 1998 18:42:44   CNACHEN
// Fixed NAVEX1.0 problem with CRC of first 64 bytes of files that are < 64
// bytes in length.
// 
// 
//    Rev 1.14   13 May 1998 12:59:14   CNACHEN
// changed to use new trojan+data file scanner
// 
//    Rev 1.13   24 Nov 1997 14:40:04   CNACHEN
// Check proper stWinData structure for filesize
// 
//    Rev 1.12   24 Nov 1997 12:04:20   CNACHEN
// Fixed problem with failing to compute CRC of segment table for PE files
// after all other checks have been completed.
// 
//    Rev 1.11   24 Nov 1997 10:42:34   CNACHEN
// Fixed two bugs:
// 
//         1. Where we check for file size; there was a problem with PE files
//         2. Where we check for MZ or ZM; there was a precedence probem
// 
//    Rev 1.10   16 Oct 1997 13:25:20   DCHI
// Changed to use CRC32.LIB and CRC32.H due to linking conflicts.
// 
//    Rev 1.9   14 Oct 1997 20:43:44   JWILBER
// Added FAR "keyword" to EXEHEADER declarations/casts in
// ObtainInformation() to avoid compiler warnings.
//
//    Rev 1.8   13 Oct 1997 14:16:38   CNACHEN
// Fixed win16 typecast problems.
//
//    Rev 1.7   13 Oct 1997 12:24:52   CNACHEN
// Further optimized trojan scanner.
//
//    Rev 1.6   13 Oct 1997 12:19:58   CNACHEN
// Improved efficiency of trojan scanner.  Only checks full file crc after
// verifying matching windows header.
//
//    Rev 1.5   10 Oct 1997 15:29:38   CNACHEN
// Added support for rebased/winaligned PE files.
//
//    Rev 1.4   09 Oct 1997 17:10:48   CNACHEN
// #include NAVEXSHR.H for SEEK_SET definition.
//
//    Rev 1.3   09 Oct 1997 16:59:40   CNACHEN
// Fixed HRESULT -> BOOL for DX.
//
//    Rev 1.2   09 Oct 1997 16:33:42   CNACHEN
//
//    Rev 1.1   09 Oct 1997 16:30:56   CNACHEN
//
//************************************************************************

#include <string.h>

#include "platform.h"

#include "avtypes.h"
#include "callback.h"
#include "ctsn.h"
#include "navex.h"
#include "trojscan.h"
#include "crc32.h"
#include "callfake.h"

#include "trojsigs.h"
#include "avendian.h"
#include "navexshr.h"
#include "winconst.h"
#include "datadir.h"

#if defined(NAVEX15)

//************************************************************************
// FreeTrojanData()
//
// Purpose
//
//  Frees all memory allocated for trojan signatures.
//
// Parameters:
//
//  lpstCallBacks           Callbacks for memory free
//
// Returns:
//
//  EXTSTATUS_OK            Always
//
//************************************************************************
// 9/11/97 Carey created.
//************************************************************************

EXTSTATUS FreeTrojanData
(
    LPCALLBACKREV1                          lpstCallBack
)
{
    DWORD                                   i;

    for (i=0;i<(DWORD)gstTrojanInfo.nNumPools;i++)
    {
        if (gstTrojanInfo.alpstPool[i] != NULL)
        {
            lpstCallBack->PermMemoryFree(gstTrojanInfo.alpstPool[i]);
            gstTrojanInfo.alpstPool[i] = NULL;
        }
    }

    gstTrojanInfo.dwSigsPerPool = 0;
    gstTrojanInfo.dwNumSigs = 0;
    gstTrojanInfo.nNumPools = 0;

    return(EXTSTATUS_OK);
}

//************************************************************************
// LoadTrojanData()
//
// Purpose
//
//  Loads all trojan sigs from the proper data file/section, allocates
//  appropriate memory.  On failure, frees all allocated memory.
//
// Parameters:
//
//  lpstCallBacks           Callbacks for file io and memory alloc/free
//  lpszNAVEXDataDir        Data directory to find data files.
//
// Returns:
//
//  EXTSTATUS_OK            On success; memory allocated
//  EXTSTATUS_FILE_ERROR    On file error, file not found, etc.
//  EXTSTATUS_MEM_ERROR     On memory allocation error
//
//************************************************************************
// 9/11/97 Carey created.
//************************************************************************

EXTSTATUS LoadTrojanData
(
    LPCALLBACKREV1                          lpstCallBack,
    LPTSTR                                  lpszNAVEXDataDir
)
{
    TCHAR                                   szDatFileName[SYM_MAX_PATH];
    HFILE                                   hFile;
    DWORD                                   dwOffset, dwLength, dwPoolMem,
                                            dwSigsInPool, dwSigsLeft, i, j;
    TROJAN_HEADER_T                         stHeader;
    LPTROJAN_DATA                           lpstCurSig;

    BOOL VDFLookUp
    (
        LPCALLBACKREV1      lpstCallBack,
        HFILE               hFile,
        DWORD               dwIdent,
        LPDWORD             lpdwOffset,
        LPDWORD             lpdwLength
    );

    // zero all!

    gstTrojanInfo.dwSigsPerPool = 0;
    gstTrojanInfo.dwNumSigs = 0;
    gstTrojanInfo.nNumPools = 0;

    // obtain the proper filename; assume data dir has trailing backslash

#if defined(SYM_UNIX)
    DataDirAppendName(lpszNAVEXDataDir,_T("virscan7.dat"),szDatFileName);
#else
    DataDirAppendName(lpszNAVEXDataDir,_T("VIRSCAN7.DAT"),szDatFileName);
#endif

    hFile = lpstCallBack->FileOpen(szDatFileName,0);

    if (hFile == (HFILE)-1)
        return(EXTSTATUS_FILE_ERROR);

    // look up our section!

    if (VDFLookUp(lpstCallBack,
                  hFile,
                  TROJAN_SECTION_ID,
                  &dwOffset,
                  &dwLength) != TRUE)
    {
        lpstCallBack->FileClose(hFile);
        return(EXTSTATUS_FILE_ERROR);
    }

    // seek to it and read it!

    if (lpstCallBack->FileSeek(hFile,dwOffset,SEEK_SET) != dwOffset)
    {
        lpstCallBack->FileClose(hFile);
        return(EXTSTATUS_FILE_ERROR);
    }

    if (lpstCallBack->FileRead(hFile,
                               &stHeader,
                               sizeof(stHeader)) != sizeof(stHeader))
    {
        lpstCallBack->FileClose(hFile);
        return(EXTSTATUS_FILE_ERROR);
    }

    // endianize!

    stHeader.dwVersion = DWENDIAN(stHeader.dwVersion);
    stHeader.dwNumSignatures = DWENDIAN(stHeader.dwNumSignatures);
    stHeader.dwNumPools = DWENDIAN(stHeader.dwNumPools);
    stHeader.dwPoolSize = DWENDIAN(stHeader.dwPoolSize);

    // check for wrong version or too many pools!

    if (stHeader.dwVersion != 1 || stHeader.dwNumPools > MAX_TROJAN_POOLS)
    {
        lpstCallBack->FileClose(hFile);
        return(EXTSTATUS_FILE_ERROR);
    }

    // reset any existing data!

    for (i=0;i<MAX_TROJAN_POOLS;i++)
        gstTrojanInfo.alpstPool[i] = NULL;

    gstTrojanInfo.dwSigsPerPool = stHeader.dwPoolSize;
    gstTrojanInfo.dwNumSigs = stHeader.dwNumSignatures;
    gstTrojanInfo.nNumPools = (int)stHeader.dwNumPools;

    // OK! now read in one pool at a time and endianize!

    dwSigsLeft = stHeader.dwNumSignatures;

    for (i=0;i<stHeader.dwNumPools;i++)
    {
        dwSigsInPool = (dwSigsLeft > stHeader.dwPoolSize) ?
                       stHeader.dwPoolSize : dwSigsLeft;

        dwPoolMem = dwSigsInPool * sizeof(TROJAN_DATA_T);

        gstTrojanInfo.alpstPool[i] =
            (LPTROJAN_DATA)lpstCallBack->PermMemoryAlloc(dwPoolMem);

        if (gstTrojanInfo.alpstPool[i] == NULL)
        {
            FreeTrojanData(lpstCallBack);

            lpstCallBack->FileClose(hFile);
            return(EXTSTATUS_MEM_ERROR);
        }

        // read in this pool of trojan sigs!

        if (lpstCallBack->FileRead(hFile,
                                   gstTrojanInfo.alpstPool[i],
                                   (UINT)dwPoolMem) != dwPoolMem)
        {
            FreeTrojanData(lpstCallBack);

            lpstCallBack->FileClose(hFile);
            return(EXTSTATUS_FILE_ERROR);
        }

        // endianize!

        lpstCurSig = (LPTROJAN_DATA)gstTrojanInfo.alpstPool[i];

        for (j=0;j<dwSigsInPool;j++)
        {
            lpstCurSig->dwDOSHeaderCRC = DWENDIAN(lpstCurSig->dwDOSHeaderCRC);
            lpstCurSig->dwWinHeaderCRC = DWENDIAN(lpstCurSig->dwWinHeaderCRC);
            lpstCurSig->dwFileSize = DWENDIAN(lpstCurSig->dwFileSize);
            lpstCurSig->dwFileCRC = DWENDIAN(lpstCurSig->dwFileCRC);
            lpstCurSig->wVirusID = WENDIAN(lpstCurSig->wVirusID);

            lpstCurSig++;
        }

        dwSigsLeft -= stHeader.dwPoolSize;

#ifdef SYM_NLM
        ThreadSwitch();
#endif
    }

    lpstCallBack->FileClose(hFile);
    return(EXTSTATUS_OK);
}

#endif //#if defined(NAVEX15)



//************************************************************************
// ComputeFileCRC()
//
// Purpose
//
//  Computes the CRC of the specified file (handle) from the TOF
//
// Parameters:
//
//  hStream                 Handle to file
//  lpdwCRC                 *lpdwCRC gets the result CRC
//
// Returns:
//
//  NOERROR                 If the operation is successful
//  E_FAIL                  Otherwise
//
//************************************************************************
// 9/11/97 Carey created.
//************************************************************************

BOOL ComputeFileCRC
(
    LPCALLBACKREV1          lpCallBack,
    HFILE                   hStream,
    LPDWORD                 lpdwCRC
)
{
    DWORD                   dwBytesRead,dwToRead,dwCRC,dwFileSize;
    BYTE                    byBuffer[512];

    // reset our seed

    dwCRC = CRC32_SEED;

    // seek to TOF

    if (lpCallBack->FileSeek(hStream,0,SEEK_SET) != 0)
        return(FALSE);

    // how many bytes to CRC?

    dwFileSize = lpCallBack->FileSize(hStream);

    // do it!

    while (dwFileSize > 0 && dwFileSize != 0xFFFFFFFFUL)
    {
        dwToRead = dwFileSize > 512 ? 512 : dwFileSize;

#ifdef SYM_NLM
        ThreadSwitch();
#endif

        if (dwToRead > 0)
        {
            dwBytesRead = lpCallBack->FileRead(hStream,
                                               byBuffer,
                                               (WORD)dwToRead);

            if (dwBytesRead != dwToRead)
            {
                return(FALSE);
            }

            dwCRC = CRC32Compute(dwToRead,
                                 byBuffer,
                                 dwCRC);     // seed

        }
        dwFileSize -= dwBytesRead;
    }

    *lpdwCRC = dwCRC;

    return(TRUE);
}


//************************************************************************
// GetPESectionEntry()
//
// Purpose
//
//  Obtains the ith section entry from after the PE header. Zeros out all
//  fileds that are not invariant after application of MS rebase/winalign.
//
// Parameters:
//
//  lpCallBack              Callback pointer
//  hFile                   File handle to the file to scan
//  dwWinHeaderOffset       Offset in file to PE header
//  dwSecNum                Section number to retrieve.
//  lpstSection             Place section record here...
//
// Returns:
//
//  EXTSTATUS_OK            Success
//  EXTSTATUS_FILE_ERROR    Otherwise
//
//************************************************************************
// 9/11/97 Carey created.
//************************************************************************


EXTSTATUS GetPESectionEntry
(
    LPCALLBACKREV1                  lpCallBack,
    HFILE                           hFile,
    DWORD                           dwWinHeaderOffset,
    DWORD                           dwSecNum,
    NAVEX_IMAGE_SECTION_HEADER FAR *lpstSection
)
{
    DWORD                               dwOffset;
    NAVEX_IMAGE_SECTION_HEADER          stSection;

    dwOffset = dwWinHeaderOffset +
                sizeof(NAVEX_IMAGE_NT_HEADERS) +
                dwSecNum * sizeof(NAVEX_IMAGE_SECTION_HEADER);

    if (dwOffset & 0x80000000UL)
        return EXTSTATUS_FILE_ERROR;

    if (lpCallBack->FileSeek(hFile,dwOffset,SEEK_SET) != dwOffset)
        return EXTSTATUS_FILE_ERROR;

    if (lpCallBack->FileRead(hFile,&stSection,
        sizeof(NAVEX_IMAGE_SECTION_HEADER)) !=
        sizeof(NAVEX_IMAGE_SECTION_HEADER))
        return EXTSTATUS_FILE_ERROR;

    // got the record

    *lpstSection = stSection;

    // zero out any fields that are likely to change because of winbase...

//    lpstSection->VirtualAddress = 0;      // not affected by rebase or winalign
    lpstSection->PointerToRawData = 0;
    lpstSection->PointerToRelocations = 0;
    lpstSection->PointerToLinenumbers = 0;
    lpstSection->Misc.PhysicalAddress = 0;

    return(EXTSTATUS_OK);
}

//************************************************************************
// ObtainInformationPE()
//
// Purpose
//
//  Obtains CRC/size information from the specified 32-bit windows PE file:
//  1. CRC of invariant fields in the PE header (invariant if rebase.exe or
//     winalign are applied to the file).  This is used as the windows header
//     CRC.
//  2. CRC of the invariant data contained in the section table; this is
//     used as the full-file CRC.
//
//  This function is used to compute the windows header crc and full file
//  crc instead of the standard ObtainInformation function, since these
//  files may be modified by MS tools and not have matching header CRCs or
//  file CRCs.
//
// Parameters:
//
//  lpCallBack              Callback pointer
//  hFile                   File handle to the file to scan
//  dwWinHeaderOffset       Offset in file to PE header
//  dwFlags                 Flags specify what info. should be obtained:
//                              BIT_FILL_DOS_HEADER     CRC DOS Header
//                              BIT_FILL_WIN_HEADER     CRC Windows Header
//                              BIT_FILL_FILE_SIZE      Get file size (always done)
//                              BIT_FILL_FILE_CRC       Get whole file CRC
//  lpbyWorkBuffer          Buffer containing 512 bytes of PE header
//  lpstFillMe              Information for the file is filled in here.
//
// Returns:
//
//  TRUE                    Success
//  FALSE                   Otherwise
//
//************************************************************************
// 9/11/97 Carey created.
//************************************************************************

BOOL ObtainInformationPE
(
    LPCALLBACKREV1              lpCallBack,
    HFILE                       hFile,
    DWORD                       dwWinHeaderOffset,
    DWORD                       dwFlags,
    LPBYTE                      lpbyWorkBuffer,
    LPTROJAN_DATA               lpstFillMe
)
{
    NAVEX_IMAGE_NT_HEADERS      stInvariant = {0};
    LPNAVEX_IMAGE_NT_HEADERS    lpstImage;
    NAVEX_IMAGE_SECTION_HEADER  stSection;
    int                         i;
    DWORD                       dwSectionCRC;

    // we know we're dealing with a PE file...
    // lpbyWorkBuffer contains our PE header...

    lpstImage = (LPNAVEX_IMAGE_NT_HEADERS)lpbyWorkBuffer;

    if (dwFlags & BIT_FILL_WIN_HEADER)
    {
        // copy over invariant fields

        stInvariant.Signature = lpstImage->Signature;
        stInvariant.FileHeader.Machine = lpstImage->FileHeader.Machine;
        stInvariant.FileHeader.NumberOfSections =
            lpstImage->FileHeader.NumberOfSections;
        stInvariant.FileHeader.NumberOfSymbols =
            lpstImage->FileHeader.NumberOfSymbols;
        stInvariant.FileHeader.SizeOfOptionalHeader =
            lpstImage->FileHeader.SizeOfOptionalHeader;
        stInvariant.FileHeader.Characteristics =
            lpstImage->FileHeader.Characteristics;

        stInvariant.OptionalHeader.Magic =
            lpstImage->OptionalHeader.Magic;
        stInvariant.OptionalHeader.MajorLinkerVersion =
            lpstImage->OptionalHeader.MajorLinkerVersion;
        stInvariant.OptionalHeader.MinorLinkerVersion =
            lpstImage->OptionalHeader.MinorLinkerVersion;
        stInvariant.OptionalHeader.MajorImageVersion =
            lpstImage->OptionalHeader.MajorImageVersion;
        stInvariant.OptionalHeader.MinorImageVersion =
            lpstImage->OptionalHeader.MinorImageVersion;
        stInvariant.OptionalHeader.MajorSubsystemVersion =
            lpstImage->OptionalHeader.MajorSubsystemVersion;
        stInvariant.OptionalHeader.MinorSubsystemVersion =
            lpstImage->OptionalHeader.MinorSubsystemVersion;
        stInvariant.OptionalHeader.SizeOfInitializedData=
            lpstImage->OptionalHeader.SizeOfInitializedData;
        stInvariant.OptionalHeader.SizeOfUninitializedData =
            lpstImage->OptionalHeader.SizeOfUninitializedData;
        stInvariant.OptionalHeader.SizeOfCode =
            lpstImage->OptionalHeader.SizeOfCode;

        stInvariant.OptionalHeader.SizeOfHeaders =
            lpstImage->OptionalHeader.SizeOfHeaders;
        stInvariant.OptionalHeader.Subsystem =
            lpstImage->OptionalHeader.Subsystem;
        stInvariant.OptionalHeader.SizeOfStackReserve =
            lpstImage->OptionalHeader.SizeOfStackReserve;
        stInvariant.OptionalHeader.SizeOfStackCommit =
            lpstImage->OptionalHeader.SizeOfStackCommit;
        stInvariant.OptionalHeader.SizeOfHeapReserve =
            lpstImage->OptionalHeader.SizeOfHeapReserve;
        stInvariant.OptionalHeader.SizeOfHeapCommit =
            lpstImage->OptionalHeader.SizeOfHeapCommit;

        lpstFillMe->dwWinHeaderCRC = CRC32Compute(sizeof(stInvariant),
                                                  (LPBYTE)&stInvariant,
                                                  CRC32_SEED);
    }

    // now compute the CRC of the sections (store it in the file crc)

    if (dwFlags & BIT_FILL_FILE_CRC)
    {
        dwSectionCRC = CRC32_SEED;

        for (i=0;i<WENDIAN(lpstImage->FileHeader.NumberOfSections);i++)
        {
#ifdef SYM_NLM
            ThreadSwitch();
#endif

            if (GetPESectionEntry(lpCallBack,
                                  hFile,
                                  dwWinHeaderOffset,
                                  i,
                                  (NAVEX_IMAGE_SECTION_HEADER FAR *)&stSection) != EXTSTATUS_OK)
                return(FALSE);

                dwSectionCRC = CRC32Compute(sizeof(stSection),
                                            (LPBYTE)&stSection,
                                            dwSectionCRC);
        }

        lpstFillMe->dwFileCRC = dwSectionCRC;
    }

    // reset file size; this is no longer an attribute we can use...

    lpstFillMe->dwFileSize = 0;

    return(TRUE);
}


//************************************************************************
// ObtainInformation()
//
// Purpose
//
//  Obtains CRC/size information from the specified file.
//
// Parameters:
//
//  lpCallBack              Callback pointer
//  hFile                   File handle to the file to scan
//  dwFlags                 Flags specify what info. should be obtained:
//                              BIT_FILL_DOS_HEADER     CRC DOS Header
//                              BIT_FILL_WIN_HEADER     CRC Windows Header
//                              BIT_FILL_FILE_SIZE      Get file size (always done)
//                              BIT_FILL_FILE_CRC       Get whole file CRC
//  lpbyHeader              Pointer to a buffer containing EXE header/TOF
//                          (64 bytes worth)
//  lpbyWorkBuffer          Minimum 2K work buffer
//  lpstFillMe              Information for the file is filled in here.
//
// Returns:
//
//  TRUE                    Success
//  FALSE                   Otherwise
//
//************************************************************************
// 9/11/97 Carey created.
//************************************************************************

BOOL ObtainInformation
(
    LPCALLBACKREV1              lpCallBack,
    HFILE                       hFile,
    DWORD                       dwFlags,
    LPBYTE                      lpbyHeader,
    LPBYTE                      lpbyWorkBuffer,
    LPTROJAN_DATA               lpstFillMe
)
{
    EXEHEADER FAR *             lpstEXEHeader;
    DWORD                       dwWinHeaderOffset,
                                dwBytesToRead;
    LPBYTE                      lpbyPtr;
    // zero fields out

    lpstFillMe->dwDOSHeaderCRC = 0;
    lpstFillMe->dwWinHeaderCRC = 0;
    lpstFillMe->dwFileSize = 0;
    lpstFillMe->dwFileCRC = 0;

    // first calculate the CRC of the EXE header

    if (dwFlags & BIT_FILL_DOS_HEADER)
    {
        BYTE                    byZeroedHeader[DOS_HEADER_MATCH_LEN] = {0};

        // seek to tof and read first 64 bytes of header in with NAVEX
        // 1.0 products, since these do not necessarily have the entire
        // buffer zeroed out if the file is < 64 bytes...

#ifdef NAVEX15
        int                     i;

        for (i=0;i<DOS_HEADER_MATCH_LEN;i++)
            byZeroedHeader[i] = lpbyHeader[i];

#else

        if (lpCallBack->FileSeek(hFile,0,SEEK_SET) != 0)
            return(FALSE);

        if (lpCallBack->FileRead(hFile,
                                 byZeroedHeader,
                                 DOS_HEADER_MATCH_LEN) == (UINT) -1)
        {
            return(FALSE);
        }
#endif

        byZeroedHeader[CHECKSUM_OFFSET_IN_HEADER1] = 0;
        byZeroedHeader[CHECKSUM_OFFSET_IN_HEADER2] = 0;

        lpstFillMe->dwDOSHeaderCRC =
            CRC32Compute(DOS_HEADER_MATCH_LEN,
                         byZeroedHeader,
                         CRC32_SEED);
    }

    // next (always) calculate the file size

    lpstFillMe->dwFileSize = lpCallBack->FileSize(hFile);
    if (lpstFillMe->dwFileSize == 0xFFFFFFFFUL)
        return(FALSE);
 
    lpstEXEHeader = (EXEHEADER FAR *)lpbyHeader;

    if ((dwFlags & (BIT_FILL_WIN_HEADER | BIT_FILL_FILE_CRC)) &&
        (lpstEXEHeader->exSignature == 0x4D5a ||
         lpstEXEHeader->exSignature == 0x5a4d))
    {

        // determine where to read the possible windows header...

        lpbyPtr = (LPBYTE)&lpstEXEHeader->exExtendOffset;

        dwWinHeaderOffset = lpbyPtr[0] |
                            ((DWORD)lpbyPtr[1] << 8) |
                            ((DWORD)lpbyPtr[2] << 16) |
                            ((DWORD)lpbyPtr[3] << 24);

        // don't check for error because we may just have a pklited
        // file that looks like a windows program but has no
        // extended header...

        lpCallBack->FileSeek(hFile,dwWinHeaderOffset,SEEK_SET);

        // determine how many bytes to read...

        if (lpstFillMe->dwFileSize > dwWinHeaderOffset + WIN_HEADER_MATCH_LEN)
        {
            dwBytesToRead = WIN_HEADER_MATCH_LEN;
        }
        else if (lpstFillMe->dwFileSize > dwWinHeaderOffset)
        {
            dwBytesToRead = lpstFillMe->dwFileSize - dwWinHeaderOffset;
        }
        else
        {
            dwBytesToRead = 0;
            lpstFillMe->dwWinHeaderCRC = 0;
        }

        // read the bytes from the windows header

        if (dwBytesToRead != 0)
        {
            if (lpCallBack->FileRead(hFile,
                                    lpbyWorkBuffer,
                                    (WORD)dwBytesToRead) != dwBytesToRead)
                return(FALSE);

            // check if the header is a "PE" header; in this case, we
            // want to use a different signature set to deal with
            // rebasing and winaligning
            // if we're successful in getting all the PE information then
            // use it for the CRC... otherwise, use the traditional CRC
            // information and file size and assume we have a corrupted
            // file...

            if (lpbyWorkBuffer[0] == 'P' &&
                lpbyWorkBuffer[1] == 'E' &&
                lpbyWorkBuffer[2] == 0 &&
                lpbyWorkBuffer[3] == 0)
            {
                if (ObtainInformationPE(lpCallBack,
                                        hFile,
                                        dwWinHeaderOffset,
                                        dwFlags,
                                        lpbyWorkBuffer,
                                        lpstFillMe) == TRUE)
                    return(TRUE);
            }

            // do a CRC on them... (Fall thru for error on PE failure)

            lpstFillMe->dwWinHeaderCRC =
                CRC32Compute(dwBytesToRead,
                             lpbyWorkBuffer,
                             CRC32_SEED);
        }
    }

    if (dwFlags & BIT_FILL_FILE_CRC)
    {
        if (ComputeFileCRC(lpCallBack,
                           hFile,
                           &lpstFillMe->dwFileCRC) != TRUE)
            return(FALSE);
    }

    return(TRUE);
}


//************************************************************************
// GetTrojanPointer()
//
// Purpose
//
//  Obtains a pointer to the dwEntryNum'th trojan signature.
//
// Parameters:
//
//  dwEntryNum      A number between 0 and gstTrojanInfo.dwNumSigs
//
// Returns:
//
//  NULL                    If the entry number is out of range
//  pointer to entry        Otherwise
//
//************************************************************************
// 9/11/97 Carey created.
//************************************************************************

LPTROJAN_DATA GetTrojanPointer
(
    DWORD           dwEntryNum
)
{
    DWORD           dwPoolIndex, dwPoolNum;

    if (dwEntryNum >= gstTrojanInfo.dwNumSigs)
        return(NULL);

    dwPoolNum = dwEntryNum / gstTrojanInfo.dwSigsPerPool;
    dwPoolIndex = dwEntryNum % gstTrojanInfo.dwSigsPerPool;

    return (&gstTrojanInfo.alpstPool[dwPoolNum][dwPoolIndex]);
}


//************************************************************************
// TrojanBinarySearch()
//
// Purpose
//
//  Performs a binary search on all the trojan signatures to locate any
//  signatures that have matching DOS/Win headers.  If there is more than
//  one such record, the index returned will point to the first record
//  in the list.
//
// Parameters:
//
//  lpstFileInfo    Information on the file being scanned
//
// Returns:
//
//  0xFFFFFFFFUL if the record cannot be found.
//  Record #     otherwise
//
//************************************************************************
// 9/11/97 Carey created.
//************************************************************************

DWORD TrojanBinarySearch
(
    LPTROJAN_DATA           lpstFileInfo
)
{
    long            imin, imax, i;
    int             result;
    LPTROJAN_DATA   lpstCurRec, lpstPrevRec;

    imin = 0;
    imax = gstTrojanInfo.dwNumSigs-1;

    i = (imin + imax) >> 1;             /* divide by 2 */

    while (imin <= imax)
    {
#ifdef SYM_NLM
        ThreadSwitch();
#endif

        lpstCurRec = GetTrojanPointer(i);

        if (lpstCurRec == NULL)
            return(0xFFFFFFFFUL);

        if (lpstFileInfo->dwDOSHeaderCRC > lpstCurRec->dwDOSHeaderCRC)
            result = 1;
        else if (lpstFileInfo->dwDOSHeaderCRC < lpstCurRec->dwDOSHeaderCRC)
            result = -1;
        else if (lpstFileInfo->dwWinHeaderCRC > lpstCurRec->dwWinHeaderCRC)
            result = 1;
        else if (lpstFileInfo->dwWinHeaderCRC < lpstCurRec->dwWinHeaderCRC)
            result = -1;
        else if (lpstFileInfo->dwFileSize > lpstCurRec->dwFileSize)
            result = 1;
        else if (lpstFileInfo->dwFileSize < lpstCurRec->dwFileSize)
            result = -1;
        else
        {
            // got a match! find the first matching item!

            do
            {
#ifdef SYM_NLM
                ThreadSwitch();
#endif

                lpstPrevRec = GetTrojanPointer(i-1);

                if (lpstPrevRec != NULL &&
                    lpstPrevRec->dwDOSHeaderCRC ==
                        lpstFileInfo->dwDOSHeaderCRC &&
                    lpstPrevRec->dwWinHeaderCRC ==
                        lpstFileInfo->dwWinHeaderCRC &&
                    lpstPrevRec->dwFileSize ==
                        lpstFileInfo->dwFileSize)
                {
                    i--;
                }
                else
                    break;
            }
            while (lpstPrevRec != NULL);

            return(i);
        }

        if (result < 0)
            imax = i - 1;
        else
            imin = i + 1;

        i = (imin + imax) >> 1;
    }

    return(0xFFFFFFFFUL);
}


//************************************************************************
// ScanForTrojans()
//
// Purpose
//
//  Obtains CRC/size information from the specified file.
//
// Parameters:
//
//  lpCallBack              Callback pointer
//  hFile                   File handle to the file to scan
//  lpbyInfectionBuffer     Pointer to the certlib/avenge fstart buffers
//  lpbyWorkBuffer          Minimum 2K work buffer
//  lpwVID                  Word containing the virus ID found, if any
//
// Returns:
//
//  EXTSTATUS_OK            No virus, success
//  EXTSTATUS_FILE_ERROR    Some error occured
//  EXTSTATUS_VIRUS_FOUND   Virus was found, *lpwVID is valid virus id
//
//************************************************************************
// 9/11/97 Carey created.
//************************************************************************

EXTSTATUS ScanForTrojans
(
    LPCALLBACKREV1              lpCallBack,
    HFILE                       hFile,
    LPBYTE                      lpbyInfectionBuffer,
    LPBYTE                      lpbyWorkBuffer,
    LPWORD                      lpwVID
)
{
    DWORD                       i, dwStartIndex;
    TROJAN_DATA_T               stData, stFullData;
    LPTROJAN_DATA               lpstCurRec;
    BOOL                        bWinHeaderSet, bFullCRCSet;

    bWinHeaderSet = bFullCRCSet = FALSE;

    // get the info for this file.

    if (ObtainInformation(lpCallBack,
                          hFile,
                          BIT_FILL_DOS_HEADER | BIT_FILL_WIN_HEADER,
                          lpbyInfectionBuffer + HEADER_OFFSET_IN_NAVEX_BUFFER,
                          lpbyWorkBuffer,
                          &stData) != TRUE)
    {
        return(EXTSTATUS_FILE_ERROR);
    }

    // search our database...

    dwStartIndex = TrojanBinarySearch(&stData);

    if (dwStartIndex == 0xFFFFFFFFUL)
    {
        // no matching items found!

        return(EXTSTATUS_OK);
    }

    // now iterate through all possbile hits.

    for (i=dwStartIndex;i<gstTrojanInfo.dwNumSigs;i++)
    {
        lpstCurRec = GetTrojanPointer(i);

#ifdef SYM_NLM
        ThreadSwitch();
#endif

        if (stData.dwDOSHeaderCRC != lpstCurRec->dwDOSHeaderCRC ||
            stData.dwWinHeaderCRC != lpstCurRec->dwWinHeaderCRC ||
            stData.dwFileSize != lpstCurRec->dwFileSize)
            break;

        // we're cool so far! all we have to do is verify the file CRC!

        if (bFullCRCSet == FALSE &&
            ObtainInformation(lpCallBack,
                              hFile,
                              BIT_FILL_FILE_CRC,
                              lpbyInfectionBuffer +
                                HEADER_OFFSET_IN_NAVEX_BUFFER,
                              lpbyWorkBuffer,
                              &stFullData) != TRUE)
        {
            return(EXTSTATUS_FILE_ERROR);
        }

        bFullCRCSet = TRUE;

        if (stFullData.dwFileCRC == lpstCurRec->dwFileCRC)
        {
            *lpwVID = lpstCurRec->wVirusID;
            return(EXTSTATUS_VIRUS_FOUND);
        }
    }

    return(EXTSTATUS_OK);
}
