//************************************************************************
//
// $Header:   S:/NAVEX/VCS/EXCELSHR.CPv   1.15   06 May 1997 13:35:42   DCHI  $
//
// Description:
//      Contains function for opening a hex-named OLE stream.
//
//************************************************************************
// $Log:   S:/NAVEX/VCS/EXCELSHR.CPv  $
// 
//    Rev 1.15   06 May 1997 13:35:42   DCHI
// Added #include "macrovid.h".
// 
//    Rev 1.14   06 May 1997 12:10:34   CFORMUL
// Added Detection for LAroux.C
// 
//    Rev 1.13   27 Mar 1997 14:57:40   CFORMUL
// Modified Laroux string to detect infections that are converted
// back to 95 from 97.  This has the side effect of also detecting
// Laroux.B samples.
// 
//    Rev 1.12   06 Mar 1997 12:57:14   CFORMUL
// Added XM.Robocop.A
// 
// 
//    Rev 1.11   05 Mar 1997 16:53:00   CFORMUL
// Added XM.Legend
// 
// 
//    Rev 1.10   10 Jan 1997 19:52:14   CFORMUL
// modified detection for Delta
// 
// 
//    Rev 1.9   09 Jan 1997 18:45:00   CFORMUL
// Added detection for XM.Delta
// 
// 
//    Rev 1.8   26 Dec 1996 15:21:52   AOONWAL
// No change.
// 
//    Rev 1.7   06 Dec 1996 15:21:30   CFORMUL
// Added search string for XM.Sofa
// 
// 
//    Rev 1.6   04 Dec 1996 20:22:50   CFORMUL
// Added Scan string for Laroux.B
// 
// 
//    Rev 1.5   02 Dec 1996 14:01:44   AOONWAL
// No change.
// 
//    Rev 1.4   29 Oct 1996 12:58:36   AOONWAL
// No change.
// 
//    Rev 1.3   10 Sep 1996 13:02:34   DCHI
// Added OLE signature check to OLEOpenStream(), for Mac compatibility.
// 
//    Rev 1.2   30 Aug 1996 15:06:10   DCHI
// BAT cache related addition.
// 
//    Rev 1.1   30 Aug 1996 14:12:12   DCHI
// Endianization and BAT cache changes.
// 
//    Rev 1.0   05 Aug 1996 10:39:58   DCHI
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

#include "storobj.h"

#include "navexshr.h"
#include "macrovid.h"

#include "endutils.h"

#include "excelshr.h"

WORD gpwExcelLaroux[] =
{
//    0xC8, 0x00, 0xEA, 0x01, 
    0xAD, 0x00, 0x06, 0x00,
    0x6C, 0x61, 0x72, 0x6F, 0x75, 0x78, 0xAD, 0x00,
    0x0C, 0x00, 0x50, 0x45, 0x52, 0x53, 0x4F, 0x4E,
    0x41, 0x4C, 0x2E, 0x58, 0x4C, 0x53, 0x24, 0x00
};

WORD gpwExcelLarouxB[] =
{
    0xC8, 0x00, 0xE0, 0x01, 0xAD, 0x00, 0x06, 0x00,
    0x6C, 0x61, 0x72, 0x6F, 0x75, 0x78, 0xAD, 0x00,
    0x0C, 0x00, 0x50, 0x45, 0x52, 0x53, 0x4F, 0x4E,
    0x41, 0x4C, 0x2E, 0x58, 0x4C, 0x53, 0x24, 0x00
};

WORD gpwExcelLarouxC[] =
{
    0xAD, 0x00, 0x06, 0x00, 0x6C, 0x61, 0x72, 0x6F, 
    0x75, 0x78, 0xAD, 0x00, 0x08, 0x00, 0x42, 0x49, 
    0x4E, 0x56, 0x2E, 0x58, 0x4C, 0x53, 0x24, 0x00
};

WORD gpwExcelSofa[] =
{
    0x73, 0x6f, 0x66, 0x61, 0x20, 0x45, 0x78, 0x63,
    0x65, 0x6c, 0x00, 0x20, 0x00, 0x40, 0x00, 0x28,
    0x00, 0x60, 0x00, 0x20, 0x00, 0x74, 0x00, 0x21,
    0x00, 0x80, 0x00, 0xa3, 0x00, 0x04, 0x00, 0x24,
    0x20, 0x6a, 0x00, 0x02, 0x00, 0xad, 0x00, 0x04,
    0x00, 0x42, 0x6f, 0x6f, 0x6b, 0x05, 0x00, 0x94 
};

WORD gpwExcelDelta[] =
{
    0x72, 0x79, 0x20, 0x2e, 0x2e, 0x21, 0x2c, 0x20,
    0x4d, 0x79, 0x20, 0x50, 0x72, 0x6f, 0x6a, 0x65,
    0x63, 0x74, 0x20, 0x4e, 0x61, 0x6d, 0x65, 0x20, 
    0x69, 0x73, 0x20, 0x44, 0x65, 0x6c, 0x74, 0x61
};

WORD gpwExcelLegend[] =
{
    0xad, 0x00, 0x06, 0x00, 0x4c, 0x65, 0x67, 0x65,
    0x6e, 0x64, 0xad, 0x00, 0x0c, 0x00, 0x50, 0x45,
    0x52, 0x53, 0x4f, 0x4e, 0x41, 0x4c, 0x2e, 0x58,
    0x4c, 0x53, 0x24, 0x00, 0xca, 0x00, 0x01, 0x00
};


WORD gpwExcelRobocop[] =
{
    0x52, 0x4f, 0x42, 0x4f, 0x20, 0x00, 0xea, 0x00,
    0x21, 0x00, 0xaa, 0x00, 0x20, 0x00, 0x74, 0x00,
    0x25, 0x00, 0x94, 0x00, 0x01, 0x00, 0x25, 0x00,
    0xc0, 0x00, 0x01, 0x00, 0x42, 0x40, 0x00, 0x01
};

EXCELSIG_T gExcelSigs[] =
{
    VID_LAROUX,                 // VID
    0x1c,                       // signature length
    gpwExcelLaroux,             // signature
    0x00AD,                     // first word of signature

    VID_LAROUXB,                // VID
    0x20,                       // signature length
    gpwExcelLarouxB,            // signature
    0x00C8,                     // first word of signature

    VID_LAROUXC,                // VID
    0x18,                       // signature length
    gpwExcelLarouxC,            // signature
    0x00AD,                     // first word of signature

    VID_SOFA,                   // VID
    0x30,                       // signature length
    gpwExcelSofa,               // signature
    0x6f73,                     // first word of signature

    VID_DELTA,                  // VID
    0x20,                       // signature length
    gpwExcelDelta,              // signature
    0x7972,                     // first word of signature

    VID_LEGEND,                 // VID
    0x20,                       // signature length
    gpwExcelLegend,             // signature
    0x00ad,                     // first word of signature

    VID_ROBOCOPA,               // VID
    0x20,                       // signature length
    gpwExcelRobocop,            // signature
    0x4f52                      // first word of signature

};

WORD gwNumExcelSigs = sizeof(gExcelSigs) / sizeof(EXCELSIG_T);



//********************************************************************
//
// Function:
//  WORD OLEIsHexNamed()
//
// Description:
//  Determines whether the Unicode string passed in consists
//  of only the ASCII characters '0' through '9' and 'a' through 'f'
//  and is of length 8.
//
// Returns:
//  1               If the name consists only of '0'-'9' and 'a'-'f'
//  0               If not
//
//********************************************************************

WORD OLEIsHexNamed
(
    LPWORD          lpuszStreamName,
    int				nMaxLen
)
{
    int             i;

    for (i=0;i<nMaxLen && lpuszStreamName[i] != 0;i++)
    {
        if (!('0' <= ConvertEndianShort(lpuszStreamName[i]) &&
                     ConvertEndianShort(lpuszStreamName[i]) <= '9' ||
              'a' <= ConvertEndianShort(lpuszStreamName[i]) &&
                     ConvertEndianShort(lpuszStreamName[i]) <= 'f'))
            return 0;
    }

    return 1;
}

//********************************************************************
//
// Function:
//  WORD OLEOpenHexNamedStream()
//
// Description:
//  Starts searching at the entry indicated by the magic number
//  for the next stream with a name consisting of only the characters
//  '0'-'9' and 'a'-'f'.
//
//  This function should always be called first with
//  *pwDirEntryNum = 0.
//
// Returns:
//  OLE_OK          on success
//  OLE_ERR_???     on error
//
//********************************************************************

WORD OLEOpenHexNamedStream
(
	LPCALLBACKREV1 	lpCallBack,				// File op callbacks
	HFILE 			hFile,					// Handle to OLE file
	LPOLE_FILE_T	pOLEFile,				// OLE file info
	LPWORD			pwDirEntryNum			// Starting entry to search
)
{
	DWORD dwDirBlk, dwCount;
	OLE_DIR_ENTRY_T oleDirEntries[4];
	DWORD dwCurBlk, dwResult;
    LPBYTE lpbySig;
    WORD i, wCurEntryNum;

	pOLEFile->hFile = hFile;

	// Seek to the beginning of the file

	if (lpCallBack->FileSeek(pOLEFile->hFile,
		0,SEEK_SET) != 0)
	{
	    // Seek failed

        return OLE_ERR_SEEK;
    }

	// Read in the Header up to the first (OLE_HDR_BAT512_BLK + 4) bytes
	// Use dwBAT512 as a buffer

	if (lpCallBack->FileRead(pOLEFile->hFile,
        (LPBYTE)(pOLEFile->dwBAT512),(OLE_HDR_BAT512_BLK + 4)) !=
		(OLE_HDR_BAT512_BLK + 4))
	{
	    // Read failed

        return OLE_ERR_READ;
    }

    // Verify that this is an OLE storage file
    //  Signature: 0xD? 0xCF 0x11 0xE0 0xA1 0xB1 0x1A 0xE1

    lpbySig = (LPBYTE)(pOLEFile->dwBAT512);
    if (((lpbySig[0] & 0xF0) != 0xD0) || (lpbySig[1] != 0xCF) ||
        (lpbySig[2] != 0x11) || (lpbySig[3] != 0xE0) ||
        (lpbySig[4] != 0xA1) || (lpbySig[5] != 0xB1) ||
        (lpbySig[6] != 0x1A) || (lpbySig[7] != 0xE1))
    {
        // Not an OLE storage file

        return OLE_ERR_READ;
    }

    // Get number of BAT512s

    pOLEFile->dwNumBAT512 = ConvertEndianLong(
        *((LPDWORD)((LPBYTE)(pOLEFile->dwBAT512) + OLE_HDR_NUM_BAT512)));

	// Get first block of directory

    dwDirBlk = ConvertEndianLong(
        *((LPDWORD)((LPBYTE)(pOLEFile->dwBAT512) + OLE_HDR_DIR_BLK)));

	// Get first block containing BAT64s

    pOLEFile->siBAT64Stream.dwStartBlk = ConvertEndianLong(
        *((LPDWORD)((LPBYTE)(pOLEFile->dwBAT512) + OLE_HDR_BAT64_BLK)));

	// Get block continuing BAT512 locations array

    pOLEFile->dwMoreBAT512Blk = ConvertEndianLong(
        *((LPDWORD)((LPBYTE)(pOLEFile->dwBAT512) + OLE_HDR_MORE_BAT512_BLK)));

	// Get number of additional blocks

    pOLEFile->dwNumMoreBATBlk = ConvertEndianLong(
		*((LPDWORD)((LPBYTE)(pOLEFile->dwBAT512) +
            OLE_HDR_NUM_MORE_BAT_BLK)));

	// Assuming an OLE file cannot be larger than 4G

	if (pOLEFile->dwNumMoreBATBlk >= 0x800000lu)
		return OLE_ERR_NO_ENTRY;

	// Note maximum possible link value

	pOLEFile->dwMaxPosLink = (DWORD)(109*128) - 1 +
		(DWORD)(127 * 128) * pOLEFile->dwNumMoreBATBlk;

	if ((pOLEFile->dwMoreBAT512Blk >= 0x800000lu ||
		pOLEFile->dwMoreBAT512Blk > pOLEFile->dwMaxPosLink) &&
		pOLEFile->dwMoreBAT512Blk != 0xFFFFFFFElu)
		return OLE_ERR_NO_ENTRY;

	// Read in the first BAT512

    pOLEFile->dwBAT512Offset = (DWORD)BLK_SIZE * (ConvertEndianLong(
        *((LPDWORD)((LPBYTE)(pOLEFile->dwBAT512) + OLE_HDR_BAT512_BLK))) + 1);

    if (lpCallBack->FileSeek(pOLEFile->hFile,
		pOLEFile->dwBAT512Offset,SEEK_SET) != pOLEFile->dwBAT512Offset)
	{
	    // Seek failed

        return OLE_ERR_SEEK;
    }

	if (lpCallBack->FileRead(pOLEFile->hFile,
		(LPBYTE)(pOLEFile->dwBAT512),sizeof(DWORD) * BLKS_PER_BAT) !=
		sizeof(DWORD) * BLKS_PER_BAT)
	{
	    // Read failed

        return OLE_ERR_READ;
    }

	pOLEFile->dwMinLink = 0;
	pOLEFile->dwMaxLink = BLKS_PER_BAT - 1;

	// Now find stream name in directory
    // Search up to 64K entries

	pOLEFile->dwBATType = BAT512;
	pOLEFile->siStreamInfo.dwStartBlk = dwDirBlk;
    pOLEFile->siStreamInfo.dwStreamLen = (DWORD)16384 * (DWORD)BLK_SIZE;
	pOLEFile->siStreamInfo.dwStreamBlk = (DWORD)(-1);

    pOLEFile->siStreamInfo.lpbyCache = NULL;    // Dir search/no cache

	wCurEntryNum = (*pwDirEntryNum / 4) * 4;
	for (dwCurBlk=wCurEntryNum/4;dwCurBlk<512;dwCurBlk++)
	{
		// Read the directory block

		dwResult = OLESeekRead(lpCallBack,
                               pOLEFile,
                               dwCurBlk * BLK_SIZE,
                               (LPBYTE)oleDirEntries,
                               BLK_SIZE);

		if (dwResult != BLK_SIZE)
			return (WORD)(dwResult);

		for (i=0;i<4;i++)
		{
			if (wCurEntryNum++ < *pwDirEntryNum)
				continue;

			if (i == 0 && dwCurBlk == 0)
			{
				// Get the starting block containing
				// 64-byte subblocks

				pOLEFile->siBlk64Stream.dwStartBlk =
                    ConvertEndianLong(oleDirEntries[i].dwStartBlk);

				pOLEFile->siBlk64Stream.dwStreamLen =
                    ConvertEndianLong(oleDirEntries[i].dwStreamLen);

				pOLEFile->siBlk64Stream.dwStreamBlk = (DWORD)(-1);

				continue;
			}

            // See if this directory block contains a Hex-Named entry
			// and verify that it is a stream object

            if (OLEIsHexNamed(oleDirEntries[i].uszName,32) == 1)
			{
				// Bingo!

				// If the stream length is less than 4096
				// then use BAT64, otherwise use BAT512 for >= 4096.

                if (ConvertEndianLong(oleDirEntries[i].dwStreamLen) >= 4096)
				{
					pOLEFile->dwBATType = BAT512;
				}
				else
				{
					pOLEFile->dwBATType = BAT64;

					// Calculate number of 64-byte subblocks

					dwCount = pOLEFile->siBlk64Stream.dwStreamLen /
						BLK64_SIZE;

					if (pOLEFile->siBlk64Stream.dwStreamLen %
						BLK64_SIZE != 0)
						++dwCount;

                    // Calculate how many BAT64s are required.
					// This is an approximation.

					dwCount = ((dwCount * 4) / BLK_SIZE + 1) * BLK_SIZE;

					pOLEFile->siBAT64Stream.dwStreamLen = dwCount;
					pOLEFile->siBAT64Stream.dwStreamBlk = (DWORD)(-1);

                    pOLEFile->siBlk64Stream.lpbyCache = NULL; // No Blk64S cache
                    pOLEFile->siBAT64Stream.lpbyCache = NULL; // No BAT64S cache
                }

				pOLEFile->siStreamInfo.dwStartBlk =
                    ConvertEndianLong(oleDirEntries[i].dwStartBlk);
				pOLEFile->siStreamInfo.dwStreamLen =
                    ConvertEndianLong(oleDirEntries[i].dwStreamLen);
				pOLEFile->siStreamInfo.dwStreamBlk = (DWORD)(-1);

                // No need for the stream BAT cache

                pOLEFile->siStreamInfo.lpbyCache = NULL;

                *pwDirEntryNum = wCurEntryNum;

				return OLE_OK;
			}
		}
	}

	return OLE_ERR_NO_ENTRY;
}


