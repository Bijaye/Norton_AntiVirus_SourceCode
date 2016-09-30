//************************************************************************
//
// $Header:   S:/NAVEX/VCS/storobj.h_v   1.10   27 May 1997 14:49:36   DCHI  $
//
// Description:
//      Contains OLE 2 Storage Object interface prototypes.
//
//************************************************************************
// $Log:   S:/NAVEX/VCS/storobj.h_v  $
// 
//    Rev 1.10   27 May 1997 14:49:36   DCHI
// Corrected #define of PACDIF_MAX_DIF from 255 to 254.
// 
//    Rev 1.9   14 Mar 1997 16:36:08   DCHI
// Added support for Office 97 repair.
// 
//    Rev 1.8   13 Feb 1997 13:29:22   DCHI
// Modifications to support VBA 5 scanning.
// 
//    Rev 1.7   17 Jan 1997 11:01:56   DCHI
// Modifications supporting new macro engine.
// 
//    Rev 1.6   21 Nov 1996 17:15:24   AOONWAL
// No change.
// 
//    Rev 1.5   29 Oct 1996 12:59:58   AOONWAL
// No change.
// 
//    Rev 1.4   13 Sep 1996 18:33:04   DCHI
// Added include of navexshr.h to take care of warnings.
// 
//    Rev 1.3   06 Sep 1996 14:53:46   DCHI
// Added SYM_VXD to pack changes.
// 
//    Rev 1.2   06 Sep 1996 14:20:28   DCHI
// pack changes
// 
//    Rev 1.1   28 Aug 1996 16:42:08   DCHI
// Added BAT cache structures and constant definitions.
// 
//    Rev 1.0   03 Jan 1996 17:12:10   DCHI
// Initial revision.
// 
//************************************************************************

#ifndef _STOROBJ_H_

#define _STOROBJ_H_

#include "platform.h"
#include "callback.h"

#include "navexshr.h"

#define OLE_HDR_NUM_BAT512			0x2C
#define OLE_HDR_DIR_BLK				0x30
#define OLE_HDR_BAT64_BLK			0x3C
#define OLE_HDR_MORE_BAT512_BLK		0x44
#define OLE_HDR_NUM_MORE_BAT_BLK    0x48
#define OLE_HDR_BAT512_BLK			0x4C

#define BATUNKNOWN		0
#define BAT64			64
#define BAT512			512

#define BLK_SIZE		512
#define BLK64_SIZE		64

#define BLKS_PER_BAT	(512/4)

#define OLE_OK				0
#define OLE_ERR_SEEK		0xFFFF
#define OLE_ERR_READ		0xFFFE
#define OLE_ERR_NO_ENTRY    0xFFFD
#define OLE_ERR_WRITE		0xFFFC

#define LINK_UNUSED			(-1)
#define LINK_LAST_BLK		(-2)
#define LINK_BAT512_BLK		(-3)

//////////////////////////////////////////////////////////////////////
//
// BAT Cache Constants
//
//////////////////////////////////////////////////////////////////////

#define MAX_PACKETS         84
#define MAX_SEC_PER_PAC		0xFFFFu

#define MAX_SEC_PER_SUBPAC_ABS		0xFFFFu
#define MAX_SEC_PER_SUBPAC_DIF		0xFFu

#define MAX_SUBPACS			255

#define CACHE_SIZE			512

#define MAX_SUBPAC_SIZE		8

#define PACTYPE_ABS			255
#define PACTYPE_FIRST_DIF	0

#define PACDIF_MAX_DIF      254

#if defined(SYM_WIN32) || defined(SYM_NTK) || defined(SYM_VXD)
#include "pshpack1.h"
#else
#pragma PACK1
#endif

typedef struct
{
	DWORD		dwStartSector;
	WORD		wCount;
} SUBPAC_ABS_T, FAR *LPSUBPAC_ABS_T;

typedef struct
{
	BYTE			byFlags;
	BYTE			byCountSubpacs;

    // Pad to next DWORD boundary

    BYTE            byPadding0;
    BYTE            byPadding1;
} PACKET_HDR_T, FAR *LPPACKET_HDR_T;

typedef struct
{
	SUBPAC_ABS_T	subpacs[MAX_SUBPACS];
} PACKET_ABS_T, FAR *LPPACKET_ABS_T;

typedef struct
{
	DWORD			dwStartSector;
	BYTE			byCounts[MAX_SUBPACS];
} PACKET_DIF_T, FAR *LPPACKET_DIF_T;

typedef struct
{
	DWORD		dwCountBlocksCached;
	WORD		wCountPackets;
	WORD		wLastPacketOffset;
} LAT_CACHE_HDR_T, FAR *LPLAT_CACHE_HDR_T;

#if defined(SYM_WIN32) || defined(SYM_NTK) || defined(SYM_VXD)
#include "poppack.h"
#else
#pragma PACK
#endif

typedef struct
{
	DWORD	dwStartBlk;
	DWORD	dwStreamLen;

    // Cache seek forward info for stream

	DWORD	dwStreamBlk;
	DWORD	dwLink;
	DWORD	dwBATOffset;

    // BAT cache

    LPBYTE  lpbyCache;
} STREAM_INFO_T;

typedef struct
{
	HFILE			hFile;
    DWORD           dwEntryNum;         // Entry number in directory
	DWORD			dwNumBAT512;
	DWORD			dwMoreBAT512Blk;
	DWORD			dwNumMoreBATBlk;
	DWORD			dwMaxPosLink;		// Maximum possible link value
	DWORD   		dwStartBAT64Blk;

	// Currently open stream

	DWORD			dwBATType;

    // Cache seek forward info
	//
	// For 512 byte blocks, this will cache the info for the stream.
	// For 64 byte subblocks, this will cache the info for the blocks
	// containing 64-byte subblocks

	STREAM_INFO_T   siStreamInfo;

	// The stream of BAT64 blocks

	STREAM_INFO_T	siBAT64Stream;

	// The stream of 64-byte subblocks

	STREAM_INFO_T	siBlk64Stream;

	// Cache the last used BAT512

	DWORD			dwBAT512Offset;
	DWORD			dwMinLink;
	DWORD			dwMaxLink;
    DWORD			dwBAT512[BLKS_PER_BAT];
} OLE_FILE_T, FAR *LPOLE_FILE;

#if defined(SYM_WIN32) || defined(SYM_NTK) || defined(SYM_VXD)
#include "pshpack1.h"
#else
#pragma PACK1
#endif

typedef struct tagVBA5_LINE_ENTRY
{
    DWORD   dwFlags;
    WORD    wSrcSize;
    WORD    wCompSize;
    DWORD   dwOffset;
} VBA5_LINE_ENTRY_T, FAR *LPVBA5_LINE_ENTRY;

#define MAX_OLE_NAME_LEN    32

#define STGTY_STORAGE       1
#define STGTY_STREAM        2

typedef struct
{
    /* Offset 0x00: Zero-terminated Unicode string */

    WORD    uszName[MAX_OLE_NAME_LEN];

    WORD    wPadding;

    BYTE    byMSE;          // Offset 0x42: STGTY_...
    BYTE    byBFlags;       // Offset 0x43

    DWORD   dwSIDLeftSib;   // Offset 0x44
    DWORD   dwSIDRightSib;  // Offset 0x48

    DWORD   dwSIDChild;     // Offset 0x4C
    DWORD   dwCLSID[4];     // Offset 0x50
    DWORD   dwUserFlags;    // Offset 0x60
    DWORD   dwTimes[4];     // Offset 0x64

    DWORD   dwStartBlk;     /* Offset 0x74:
                                Starting block of stream or
                                If Root Entry, then starting block
                                of 64 byte subblocks */

    DWORD   dwStreamLen;    /* Offset 0x78: Byte length of stream */

	DWORD	dwUnknown2;		/* Offset 0x7C: */

} OLE_DIR_ENTRY_T, FAR *LPOLE_DIR_ENTRY;

#if defined(SYM_WIN32) || defined(SYM_NTK) || defined(SYM_VXD)
#include "poppack.h"
#else
#pragma PACK
#endif

typedef OLE_FILE_T FAR *    LPOLE_FILE_T;
typedef STREAM_INFO_T FAR *	LPSTREAM_INFO_T;

DWORD OLESeekStreamBlk512
(
	LPCALLBACKREV1 	lpCallBack,				// File op callbacks
	LPOLE_FILE_T	pOLEFile,				// OLE file info
	LPSTREAM_INFO_T	psiStreamInfo,			// Stream info
	DWORD 			dwStreamBlk,			// Block to seek to
	LPDWORD			pdwBlkFileOffset		// For storing blk offset
);

DWORD OLESeekStreamBlk64
(
	LPCALLBACKREV1 	lpCallBack,				// File op callbacks
	LPOLE_FILE_T	pOLEFile,				// OLE file info
	LPSTREAM_INFO_T	psiStreamInfo,			// Stream info
	DWORD 			dwStreamBlk,			// Block to seek to
	LPDWORD			pdwBlkStreamOffset		// For storing blk offset
);

WORD OLESeekRead
(
	LPCALLBACKREV1 	lpCallBack,				// File op callbacks
	LPOLE_FILE_T	pOLEFile,				// OLE file info
	DWORD 			dwOffset,				// Read offset in stream
	LPBYTE 			lpbBuffer,				// Read buffer
	WORD 			wBytesToRead			// Number of bytes to read
);

WORD OLESeekWrite
(
	LPCALLBACKREV1 	lpCallBack,				// File op callbacks
	LPOLE_FILE_T	pOLEFile,				// OLE file info
	DWORD 			dwOffset,				// Write offset in stream
	LPBYTE 			lpbBuffer,				// Write buffer
	WORD 			wBytesToWrite			// Number of bytes to write
);

BOOL OLEWriteZeroes
(
    LPCALLBACKREV1  lpstCallBack,           // File op callbacks
    LPOLE_FILE      lpstOLEFile,            // OLE file info
    DWORD           dwStartOffset,          // Start offset to write
    DWORD           dwNumBytes,             // Num bytes to zero
    LPBYTE          lpbyWorkBuffer          // >= 512 bytes
);

BOOL OLECopyBytes
(
    LPCALLBACKREV1  lpstCallBack,           // File op callbacks
    LPOLE_FILE      lpstOLEFile,            // OLE file info
    DWORD           dwSrcOffset,            // Offset of source bytes
    DWORD           dwDstOffset,            // Offset of destination
    DWORD           dwNumBytes,             // Num bytes to copy
    LPBYTE          lpbyWorkBuffer          // >= 512 bytes
);

DWORD OLEStreamLen
(
    LPOLE_FILE_T    lpstOLEFile             // OLE file info
);

DWORD OLEStreamEntryNum
(
    LPOLE_FILE_T    lpstOLEFile             // OLE file info
);

WORD WStrNCmp
(
    LPWORD  puszStr0,   // First WORD array
    LPWORD  puszStr1,   // Second WORD array
    WORD    wN          // Compare up to wN WORDs
);

WORD OLEOpenStream
(
	LPCALLBACKREV1 	lpCallBack,				// File op callbacks
	HFILE 			hFile,					// Handle to OLE file
	LPOLE_FILE_T	pOLEFile,				// OLE file info
	LPWORD			puszStreamName,			// Stream to open
    LPBYTE          lpbyStreamBATCache,     // Non NULL if cache, 512 bytes
    LPWORD          lpwDirEntryNum          // Starting entry to search
);

WORD OLEOpenStreamEntryNum
(
	LPCALLBACKREV1 	lpstCallBack,			// File op callbacks
	HFILE 			hFile,					// Handle to OLE file
	LPOLE_FILE 	    lpstOLEFile,			// OLE file info
    WORD            wEntryNum,              // Entry to open
    LPBYTE          lpbyStreamBATCache      // Non NULL if cache, 512 bytes
);

#define OLE_OPEN_CB_STATUS_RETURN       0
#define OLE_OPEN_CB_STATUS_CONTINUE     1

typedef int (*LPFNOLE_OPEN_CB)
    (
        LPOLE_DIR_ENTRY     lpstEntry,  // Ptr to the entry
        DWORD               dwIndex,    // The entry's index in the directory
        LPVOID              lpvCookie
    );

WORD OLEOpenStreamCB
(
    LPCALLBACKREV1  lpstCallBack,           // File op callbacks
	HFILE 			hFile,					// Handle to OLE file
    LPOLE_FILE      lpstOLEFile,            // OLE file info
    LPFNOLE_OPEN_CB lpfnOpenCB,             // The callback function
    LPVOID          lpvCookie,              // Cookie
    LPBYTE          lpbyStreamBATCache,     // Non NULL if cache, 512 bytes
    LPWORD          lpwDirEntryNum          // Starting entry to search
);

typedef struct tagOLE_OPEN_SIB
{
    int             nMaxWaitingSibs;
    int             nNumWaitingSibs;
    LPWORD          lpawSibs;
} OLE_OPEN_SIB_T, FAR *LPOLE_OPEN_SIB;

WORD OLEOpenSiblingCB
(
    LPCALLBACKREV1  lpstCallBack,           // File op callbacks
	HFILE 			hFile,					// Handle to OLE file
    LPOLE_FILE      lpstOLEFile,            // OLE file info
    LPFNOLE_OPEN_CB lpfnOpenCB,             // The callback function
    LPVOID          lpvCookie,              // Cookie
    LPBYTE          lpbyStreamBATCache,     // Non NULL if cache, 512 bytes
    LPOLE_OPEN_SIB  lpstOLEOpenSib          // OLE open sibling structure
);

WORD OLEFindNextStorage
(
    LPCALLBACKREV1  lpstCallBack,           // File op callbacks
	HFILE 			hFile,					// Handle to OLE file
    LPOLE_FILE      lpstOLEFile,            // OLE file info
    LPWORD          lpwStorageChild,        // Child of found storage
    LPWORD          lpwDirEntryNum          // Starting entry to search
);

BOOL OLEGetChildOfStorage
(
    LPCALLBACKREV1  lpstCallBack,           // File op callbacks
	HFILE 			hFile,					// Handle to OLE file
    LPOLE_FILE      lpstOLEFile,            // OLE file info
    WORD            wStorageEntryNum,       // The storage of the child
    LPWORD          lpwStorageChild         // Child of found storage
);

BOOL OLEUnlinkEntry
(
    LPCALLBACKREV1  lpstCallBack,           // File op callbacks
    LPOLE_FILE      lpstOLEFile,            // OLE file structure
    WORD            wParentStorageEntryNum, // Parent storage of stream
    WORD            wUnlinkEntryNum         // Entry to unlink
);

BOOL OLESetStreamLen
(
    LPCALLBACKREV1  lpstCallBack,           // File op callbacks
    LPOLE_FILE      lpstOLEFile,            // OLE file structure
    DWORD           dwEntryNum,             // Entry to change
    DWORD           dwNewLen                // New stream length
);

#endif

