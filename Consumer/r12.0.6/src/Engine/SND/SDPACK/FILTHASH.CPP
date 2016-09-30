// Copyright 1998 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/SDFILTER/VCS/filthash.cpv   1.0   06 Apr 1998 11:55:36   CNACHEN  $
//
// Description:
//
// Contains: Filter scanning code.
//
// See Also:
//************************************************************************
// $Log:   S:/SDFILTER/VCS/filthash.cpv  $
// 
//    Rev 1.0   06 Apr 1998 11:55:36   CNACHEN
// Initial revision.
// 
//    Rev 1.0   12 Mar 1998 18:02:28   CNACHEN
// Initial revision.
// 
//************************************************************************

#include "platform.h"
#include "callback.h"
#include "ctsn.h"
#include "navex.h"
#include "crc32.h"
#include "callfake.h"

#include "avendian.h"
#include "navexshr.h"
#include "winconst.h"

#include "filthash.h"

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
    LPFILTER_DATA               lpstFillMe
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
    LPFILTER_DATA               lpstFillMe
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
        BYTE                    byZeroedHeader[DOS_HEADER_MATCH_LEN];
        int                     i;

        for (i=0;i<DOS_HEADER_MATCH_LEN;i++)
            byZeroedHeader[i] = lpbyHeader[i];

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
