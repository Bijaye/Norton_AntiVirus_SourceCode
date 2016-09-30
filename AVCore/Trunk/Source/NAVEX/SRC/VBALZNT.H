//************************************************************************
//
// $Header:   S:/NAVEX/VCS/vbalznt.h_v   1.1   14 Mar 1997 16:33:16   DCHI  $
//
// Description:
//  Header file for LZNT.CPP containing structure and function
//  prototypes.
//
//************************************************************************
// $Log:   S:/NAVEX/VCS/vbalznt.h_v  $
// 
//    Rev 1.1   14 Mar 1997 16:33:16   DCHI
// Added support for Office 97 repair.
// 
//    Rev 1.0   13 Feb 1997 13:24:20   DCHI
// Initial revision.
// 
//************************************************************************

#ifndef _VBALZNT_H_

#define _VBALZNT_H_

#define LZNT_MAX_COPY_DISPLACEMENT  4096
#define LZNT_BUF_SIZE               64

typedef struct tagLZNT_BUF
{
    LPCALLBACKREV1  lpstCallBack;           // File op callbacks
    LPOLE_FILE      lpstOLEFile;            // Module stream

    BYTE            abyBuf[LZNT_BUF_SIZE];
    int             nNumBytesInBuf;
    int             nNextByteI;

    DWORD           dwNextReadOffset;
    DWORD           dwEndReadOffsetPlus1;
} LZNT_BUF_T, FAR *LPLZNT_BUF;

typedef struct tagLZNT
{
    DWORD           dwFirstChunkOffset;

    DWORD           dwSize;

    BOOL            bCompressed;
    BOOL            bChunkIsCompressed;

    BYTE            abyUncompressed[LZNT_MAX_COPY_DISPLACEMENT];

    // stLZNTBuf buffers the compressed data from the stream

    LZNT_BUF_T      stLZNTBuf;

    // i is the index of the byte in the uncompressed buffer
    //  with the smallest offset in the stream

    int             i;

    // dwOffsetAtI is the offset in the stream of the byte
    //  at index i

    DWORD           dwOffsetAtI;

    // lpbyNextDst points to the next location in abyUncompressed
    //  into which to continue reading bytes from the stream

    LPBYTE          lpbyNextDst;

    // nCopyLen will be non-zero if the last copy operation
    //  was partially completed and nCopyDisplacement will hold the
    //  corresponding copy displacement

    int             nCopyLen;
    int             nCopyDisplacement;

    // byFlag is the value of the last flag byte read

    BYTE            byFlag;

    // The current bit of the flags variable

    int             nFlagBit;

    // nFormat is the current assumed format of the copy descriptor

    int             nFormat;

    // nChunkBytesLeft holds the number of bytes left in the
    //  current chunk

    int             nChunkBytesLeft;

    // dwNumUncompressedChunkBytes holds the number of bytes
    //  decompressed so far from the current chunk

    DWORD           dwNumUncompressedChunkBytes;

} LZNT_T, FAR *LPLZNT;

// Function prototypes

int LZNTGetBytes
(
    LPLZNT          lpstLZNT,
    DWORD           dwOffset,
    int             nNumBytes,
    LPBYTE          lpbyDst
);

BOOL LZNTCopyCompress
(
    LPLZNT          lpstLZNT,   // Stream to compress to
    HFILE           hFile,      // Handle to file containing bytes
    DWORD           dwOffset,   // Offset in file of bytes to compress
    DWORD           dwNumBytes, // Number of bytes to copy/compress
    LPDWORD         lpdwLen     // Ptr to DWORD for compressed stream len
);

BOOL VBA5LZNTInit
(
    LPCALLBACKREV1  lpstCallBack,           // File op callbacks
    LPOLE_FILE      lpstOLEFile,            // Module stream
    LPLZNT          lpstLZNT
);

BOOL VBA5LZNTModuleInit
(
    LPCALLBACKREV1  lpstCallBack,           // File op callbacks
    LPOLE_FILE      lpstOLEFile,            // Module stream
    LPLZNT          lpstLZNT
);

#endif // #ifndef _VBALZNT_H_

