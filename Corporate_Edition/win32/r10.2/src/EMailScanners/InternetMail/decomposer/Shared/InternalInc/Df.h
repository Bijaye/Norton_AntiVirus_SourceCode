// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
//************************************************************************
//
// $Header:   S:/INCLUDE/VCS/df.h_v   1.4   23 Dec 1998 14:05:34   DCHI  $
//
// Description:
//  Header file for DF (deflation/inflation) library.
//
//************************************************************************
// $Log:   S:/INCLUDE/VCS/df.h_v  $
// 
//    Rev 1.4   23 Dec 1998 14:05:34   DCHI
// Increased size of tree arrays in IF_T for fixed Huffman trees.
// 
//    Rev 1.3   12 Nov 1998 18:09:08   DCHI
// Added endianization macros.
// 
//    Rev 1.2   29 Jul 1998 10:12:00   DCHI
// Added DFZeroBuf() and DFCopyBuf() prototypes.
// 
//    Rev 1.1   26 Nov 1997 15:17:26   DCHI
// Added DFInit().
// 
//    Rev 1.0   11 Nov 1997 16:20:56   DCHI
// Initial revision.
// 
//************************************************************************

#ifndef _DF_H_

#define _DF_H_

//////////////////////////////////////////////////////////////////////
// Endianization definitions
//////////////////////////////////////////////////////////////////////

#if !defined(WENDIAN) || !defined(DWENDIAN)

	#ifdef	BIG_ENDIAN

		#define WENDIAN(w)	( (WORD)((WORD)(w) >> 8) | (WORD)((WORD)(w) << 8) )
		#define DWENDIAN(dw) ( (DWORD)((DWORD)(dw) >> 24) | \
							  (DWORD)((DWORD)dw << 24)	| \
							  (DWORD)(((DWORD)(dw) >> 8) & 0xFF00U) | \
							  (DWORD)(((DWORD)(dw) << 8) & 0xFF0000UL) )

	#else	// if LITTLE_ENDIAN (default)

		#define WENDIAN(w)	(w)
		#define DWENDIAN(dw) (dw)

	#endif

#endif // #if !defined(WENDIAN) || !defined(DWENDIAN)


///////////////////////////////////////////////////////////////////////////
//
// D e f l a t e
//
///////////////////////////////////////////////////////////////////////////

#define DF_NUM_LL_CODES				286
#define DF_NUM_LITERAL_CODES		257
#define DF_NUM_LEN_CODES			29
#define DF_NUM_DIST_CODES			30
#define DF_MAX_BITS					15

#define DF_NUM_CODE_LEN_CODES		19
#define DF_MAX_CODE_LEN_CODE_BITS	7

// Minimum and maximum length of repeated string

#define DF_MIN_LEN					3
#define DF_MAX_LEN					258

// Maximum distance of repeated string

#define DF_MAX_DIST					32768

// Each uncompressed block will have a size of DF_HIST_SIZE bytes

#define DF_HIST_SIZE				16384

// Hash parameters

#define DF_HASH_SIZE				8192
#define DF_PAS_CHAIN_LEN			32
#define DF_ACT_CHAIN_LEN			64

#define DF_HASH(lpby)		(((lpby[0] << 6) ^ (lpby[1] << 3) ^ (lpby[2])) & (DF_HASH_SIZE-1))

// Buffer DF_OUT_BUF_SIZE before outputting

#define DF_OUT_BUF_SIZE				512

typedef struct tagDFITEM
{
	WORD		wFreq;
	WORD		wDepth;

	union
	{
		int		nParent;
		int		nLen;
	} uPL;
} DFITEM_T, FAR *LPDFITEM;

typedef struct tagDF_HASH
{
	BOOL		bActive0;

	LPWORD		lpawFirst0;
	LPWORD		lpawPrev0;

	LPWORD		lpawFirst1;
	LPWORD		lpawPrev1;
} DF_HASH_T, FAR *LPDF_HASH;

typedef struct tagDF
{
	LPVOID			lpvRootCookie;

	int				nNumCodes;

	int				nMaxCode;	// Maximum encountered code

	int				nNumItems;
	DFITEM_T		astItems[2 * DF_NUM_LL_CODES + 1];
	int				an[2 * DF_NUM_LL_CODES + 1];

	int				anBLCount[DF_MAX_BITS+1];

	// Index of the root item of the Huffman tree

	int				nRootIndex;

	// The compressed length

	DWORD			dwCompressedLen;

	// awLL[] and awDist[] are also used upon entrance to
	//  store the frequencies

	int				nMaxLLCode;
	BYTE			abyLLLen[DF_NUM_LL_CODES];
	WORD			awLL[DF_NUM_LL_CODES];

	int				nMaxDistCode;
	BYTE			abyDistLen[DF_NUM_DIST_CODES];
	WORD			awDist[DF_NUM_DIST_CODES];

	// awCodeLen[] is used initially to store the frequency

	int				nMaxCodeLenCodeIndex;
	BYTE			abyCodeLenLen[DF_NUM_CODE_LEN_CODES];
	WORD			awCodeLen[DF_NUM_CODE_LEN_CODES];

	// The first half of abyStrm[] is the previous block.
	//  The second half is the block we are currently looking at.

	BYTE			abyStrm[(unsigned)2 * DF_HIST_SIZE];    // 32K

	// wBlkSize is the size of the block

	WORD			wBlkSize;

	DF_HASH_T		stHash;

	// Halve time

	WORD			wNumLitLenDistPairs;
	LPBYTE			lpabyLitLen;
	LPWORD			lpawDist;

	// Fields used to manage the output

	LPVOID			lpvFile;

	BOOL			bFinalBlock;	// TRUE if last block
	int				nCurBit;		// Current bit in current byte
	int				nCurByte;		// Current byte in buffer
	BYTE			abyOutBuf[DF_OUT_BUF_SIZE]; // Output buffer

	// State of item

	DWORD			dwHdrOffset;
	DWORD			dwUncompressedSize;
	DWORD			dwCRC;
	DWORD			dwCompressedSize;
} DF_T, FAR *LPDF;

void DFZeroBuf
(
	LPBYTE		lpabyBuf,
	int			nSize
);

void DFCopyBuf
(
	LPBYTE		lpabyDst,
	LPBYTE		lpabySrc,
	int			nSize
);

void DFBuildTrees
(
	LPDF		lpstDF
);

BOOL DFOutputFlush
(
	LPDF		lpstDF
);

BOOL DFOutputNoCompressHdr
(
	LPDF		lpstDF,
	WORD		wLen
);

BOOL DFOutputDeflateHdr
(
	LPDF		lpstDF
);

BOOL DFOutputLitLenDist
(
	LPDF		lpstDF,
	WORD		wLitLen,
	WORD		wDist
);

void DFUpdateLenDistFreq
(
	LPDF		lpstDF,
	WORD		wLen,
	WORD		wDist
);

BOOL DFInit
(
	LPDF		lpstDF,
	LPVOID		lpvDstFile,
	DWORD		dwSize
);

BOOL DFDeflate
(
	LPDF		lpstDF,
	LPVOID		lpvFile
);

LPDF DFAllocDF
(
	LPVOID		lpvRootCookie
);

BOOL DFFreeDF
(
	LPDF		lpstDF
);


///////////////////////////////////////////////////////////////////////////
//
// I n f l a t e
//
///////////////////////////////////////////////////////////////////////////

#define IF_HISTORY_SIZE		32768
#define IF_CLT_SIZE			18
#define IF_LLT_SIZE			285
#define IF_DT_SIZE			29

#define IF_READ_AHEAD_SIZE	512

typedef struct tagIF
{
	// LLT: Literal/length tree

	WORD		awLLT0[IF_LLT_SIZE+2];
	WORD		awLLT1[IF_LLT_SIZE+2];

	// DT: Distance tree

	WORD		awDT0[IF_DT_SIZE+2];
	WORD		awDT1[IF_DT_SIZE+2];

	BYTE		abyHistory[IF_HISTORY_SIZE];

	// Bit access

	LPVOID		lpvRootCookie;
	LPVOID		lpvSrcFile;
	int			nReadAheadI;
	int			nReadAheadByteCount;
	DWORD		dwNextI;
	BYTE		abyReadAhead[IF_READ_AHEAD_SIZE];
	DWORD		dwNextReadOffset;
	DWORD		dwEndReadOffset;
	BYTE		byCurByte;
	int			nBitMask;

	DWORD		dwCRC;
} IF_T, FAR *LPIF;

BOOL IFInit
(
	LPIF		lpstIF,
	LPVOID		lpvSrcFile,
	DWORD		dwStartOffset,
	DWORD		dwSize
);

BOOL IFInflate
(
	LPIF		lpstIF,
	LPVOID		lpvDstFile
);

LPIF IFAllocIF();

BOOL IFFreeIF
(
	LPIF		lpstIF
);


/////////////////////////////////////////////////////////////////
// Callbacks

// Default return type

typedef int DF_STATUS;

#define DF_STATUS_OK		0
#define DF_STATUS_ERROR		1

DF_STATUS DFMemoryAlloc
(
	LPVOID		lpvRootCookie,
	DWORD		dwNumBytes,
	LPLPVOID	lplpvBuffer
);

DF_STATUS DFMemoryFree
(
	LPVOID		lpvRootCookie,
	LPVOID		lpvBuffer
);

#define DF_SEEK_SET		0
#define DF_SEEK_CUR		1
#define DF_SEEK_END		2

DF_STATUS DFFileSeek
(
	LPVOID		lpvRootCookie,
	LPVOID		lpvFile,
	long		lOffset,
	int			nWhence,
	long		*lplNewOffset
);

DF_STATUS DFFileRead
(
	LPVOID		lpvRootCookie,
	LPVOID		lpvFile,
	LPVOID		lpvBuffer,
	DWORD		dwNumBytes,
	LPDWORD		lpdwBytesRead
);

DF_STATUS DFFileWrite
(
	LPVOID		lpvRootCookie,
	LPVOID		lpvFile,
	LPVOID		lpvBuffer,
	DWORD		dwNumBytes,
	LPDWORD		lpdwBytesWritten
);

DF_STATUS DFFileTruncate
(
	LPVOID		lpvRootCookie,
	LPVOID		lpvFile
);

DF_STATUS DFProgress
(
	LPVOID		lpvRootCookie,
	DWORD		dwValue
);

#endif // #ifndef _DF_H_

