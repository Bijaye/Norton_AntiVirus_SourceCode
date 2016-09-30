//************************************************************************
//
// $Header:   S:/NAVEX/VCS/storobj.cpv   1.22   05 Jun 1997 13:14:44   DCHI  $
//
// Description:
//      Contains OLE 2 Storage Object interface code.
//
//************************************************************************
// $Log:   S:/NAVEX/VCS/storobj.cpv  $
// 
//    Rev 1.22   05 Jun 1997 13:14:44   DCHI
// #ifdef'd out unused symbols for SYM_NLM.
// 
//    Rev 1.21   27 May 1997 14:49:58   DCHI
// Corrected comparison usage of PACDIF_MAX_DIF due to change from 255 to 254.
// 
//    Rev 1.20   14 Mar 1997 16:34:20   DCHI
// Added support for Office 97 repair.
// 
//    Rev 1.19   13 Feb 1997 13:29:14   DCHI
// Modifications to support VBA 5 scanning.
// 
//    Rev 1.18   17 Jan 1997 11:01:58   DCHI
// Modifications supporting new macro engine.
// 
//    Rev 1.17   26 Dec 1996 15:21:46   AOONWAL
// No change.
// 
//    Rev 1.16   02 Dec 1996 14:01:10   AOONWAL
// No change.
// 
//    Rev 1.15   29 Oct 1996 12:59:00   AOONWAL
// No change.
// 
//    Rev 1.14   16 Sep 1996 18:03:04   DCHI
// Fixed a couple of cache implementation problems.
// 
//    Rev 1.13   10 Sep 1996 13:03:12   DCHI
// Added OLE signature check to OLEOpenStream(), for Mac compatibility.
// 
//    Rev 1.12   30 Aug 1996 15:07:14   DCHI
// BAT cache related addition.
// 
//    Rev 1.11   28 Aug 1996 16:41:26   DCHI
// Added BAT cache ifdef'd out for DOS_DOC_REPAIR.
// 
//    Rev 1.10   02 Aug 1996 10:58:04   DCHI
// Corrected boundary condition on read of more than 512 bytes on a read
// of a multiple of 512 bytes.
// 
//    Rev 1.9   26 Jul 1996 12:40:24   DCHI
// Correction of boundary condition in OLESeekRead64().
// 
//    Rev 1.8   10 May 1996 15:25:00   DCHI
// Corrected parenthesization after endianization change.
// 
//    Rev 1.7   09 May 1996 11:10:44   DCHI
// Endian corrections.
// 
//    Rev 1.6   03 May 1996 13:27:00   DCHI
// Endian-enabled the code for Mac compatibility.
// 
//    Rev 1.5   30 Jan 1996 15:43:36   DCHI
// Added #ifndef SYM_NLM and #endif pairs to ifdef out functions unused
// on NLM platform.
// 
//    Rev 1.4   12 Jan 1996 17:20:40   DCHI
// Removed string comparison of root entry check.
// 
//    Rev 1.3   05 Jan 1996 14:37:38   DCHI
// Added one more relinguish control call for NLM.
// 
//    Rev 1.2   05 Jan 1996 13:59:56   DCHI
// Added relinguish control calls for NLM to OLESeekStream512().
// 
//    Rev 1.1   05 Jan 1996 13:28:02   DCHI
// Increased max # of checked dir entries to 64K.
// 
//    Rev 1.0   03 Jan 1996 17:14:50   DCHI
// Initial revision.
// 
//************************************************************************

#include "storobj.h"

#include "navexshr.h"

#include "endutils.h"

//////////////////////////////////////////////////////////////////////
// Disable BAT cache for plain non-DX DOS macro repair              //
//////////////////////////////////////////////////////////////////////

#ifdef DOS_DOC_REPAIR
#define NO_BAT_CACHE
#endif // #ifdef DOS_DOC_REPAIR

//////////////////////////////////////////////////////////////////////

//********************************************************************
//
// Function:
//	WORD WStrNCmp()
//
// Description:
//	Compares up to wN characters from the two WORD arrays.
//
// Returns:
//	0	If both arrays are the same up to wN WORDS
//	-1	If the first array comes lexicographically before the second
//	1	If the first array comes lexicographically after the second
//
//********************************************************************

WORD WStrNCmp
(
    LPWORD  puszStr0,   // First WORD array
    LPWORD  puszStr1,   // Second WORD array
    WORD    wN          // Compare up to wN WORDs
)
{
    WORD    wA;
    WORD    wB;

    while (wN-- != 0)
    {
        wA = *puszStr0++;
        wB = *puszStr1++;

        wA = ConvertEndianShort(wA);
        wB = ConvertEndianShort(wB);

        if (wA == 0 && wB == 0)
            return ((WORD)(0));

        if (wA < wB)
            return ((WORD)(-1));

        if (wA > wB)
            return ((WORD)(1));
    }

    return 0;
}


//////////////////////////////////////////////////////////////////////
// Begin BAT cache functions
//////////////////////////////////////////////////////////////////////

#ifndef NO_BAT_CACHE

//********************************************************************
//
// Function:
//  WORD CreateNDIF()
//
// Description:
//  Creates an NDIF packet at the location pointed to by lpPacketHdr.
//
//  If byCount1 == 0, then the new packet is initialized with one
//  subpacket with count byCount0.
//
//  If byCount1 != 0, then the new packet is initialized with two
//  subpackets with counts byCount0 and byCount1.
//
// Returns:
//  0   If the CACHE has no more room for the packet
//  1   On successful creation of the packet
//
//********************************************************************

WORD CreateNDIF
(
    LPLAT_CACHE_HDR_T   lpCacheHdr,     // Pointer to cache header
    LPPACKET_HDR_T      lpPacketHdr,    // Pointer to packet dest
    BYTE                byN,            // N dif between subpackets
    DWORD               dwStartSector,  // Starting sector of packet
    BYTE                byCount0,       // Count for subpacket #0
    BYTE                byCount1        // Count for subpacket #1
)
{
    WORD            wLastPacketOffset;
    LPPACKET_DIF_T  lpPacketDif;

    wLastPacketOffset = (LPBYTE)lpPacketHdr - (LPBYTE)lpCacheHdr;

    lpPacketDif = (LPPACKET_DIF_T)
        ((LPBYTE)lpPacketHdr + sizeof(PACKET_HDR_T));

    if (byCount1 == 0)
    {
        if ((wLastPacketOffset + sizeof(PACKET_HDR_T) +
            sizeof(DWORD) + sizeof(BYTE)) > CACHE_SIZE)
            return 0;

        lpPacketHdr->byCountSubpacs = 1;
    }
    else
    {
        if ((wLastPacketOffset + sizeof(PACKET_HDR_T) +
            sizeof(DWORD) + 2 * sizeof(BYTE)) > CACHE_SIZE)
            return 0;

        lpPacketHdr->byCountSubpacs = 2;
        lpPacketDif->byCounts[1] = byCount1;
    }

    lpCacheHdr->wCountPackets++;

    lpCacheHdr->wLastPacketOffset = wLastPacketOffset;
    lpCacheHdr->dwCountBlocksCached++;;
    lpPacketHdr->byFlags = byN;

    lpPacketDif->dwStartSector = dwStartSector;
    lpPacketDif->byCounts[0] = byCount0;

    return 1;
}


//********************************************************************
//
// Function:
//  WORD InitMacroPacket()
//
// Description:
//  Initializes the a stream BAT cache by creating the cache header
//  and an NDIF packet with one subpacket containing the first sector
//  of the stream.
//
// Returns:
//  0   On error
//  1   On successful intiailization of the cache
//
//********************************************************************

WORD InitMacroPacket
(
    LPBYTE          lpbyCache,      // Pointer to cache memory
    DWORD           dwStartSector   // First sector of stream
)
{
    ((LPLAT_CACHE_HDR_T)lpbyCache)->dwCountBlocksCached = 0;
    ((LPLAT_CACHE_HDR_T)lpbyCache)->wCountPackets = 0;

    return CreateNDIF(
        (LPLAT_CACHE_HDR_T)lpbyCache,
        (LPPACKET_HDR_T)(lpbyCache + sizeof(LAT_CACHE_HDR_T)),
        PACTYPE_FIRST_DIF,
        dwStartSector,1,0);
}


//********************************************************************
//
// Function:
//  WORD AddSector()
//
// Description:
//  Adds the next consecutive sector of a stream to its BAT cache.
//
// Returns:
//  0   If the CACHE has no more room for the new block
//  1   On successful addition of the sector to the cache
//
//********************************************************************

WORD AddSector
(
    LPBYTE          lpbyCache,      // Pointer to the cache
    DWORD           dwSector        // The sector to add
)
{
	BYTE				byFlags;
	WORD				wCountSubpacs;
	LPBYTE				lpbyPtrCache;
	DWORD				dwDif, dwStartSector;
	LPLAT_CACHE_HDR_T	lpCacheHdr;
	LPPACKET_HDR_T		lpPacketHdr;
	LPSUBPAC_ABS_T 		lpSubpac;
    int                 nNumPadBytes;

	// Is there room on the last subpacket?

	lpCacheHdr = (LPLAT_CACHE_HDR_T)lpbyCache;

	lpPacketHdr = (LPPACKET_HDR_T)(lpbyCache +
		lpCacheHdr->wLastPacketOffset);
	byFlags = lpPacketHdr->byFlags;
    wCountSubpacs = lpPacketHdr->byCountSubpacs;

	lpbyPtrCache = (LPBYTE)lpPacketHdr + sizeof(PACKET_HDR_T);
	switch (byFlags)
	{
		case PACTYPE_ABS:
		{
			DWORD			dwContiguousSector;
			WORD			wCount;

			lpSubpac = ((LPPACKET_ABS_T)lpbyPtrCache)->subpacs +
				wCountSubpacs - 1;

			dwStartSector = lpSubpac->dwStartSector;
			wCount = lpSubpac->wCount;
			dwContiguousSector = dwStartSector + wCount;

            // Can the new sector be placed in this subpacket?
            //  It can if it immediately follows the last sector
            //  in this subpacket and there is still room in this
            //  subpacket for it.

			if (dwContiguousSector == dwSector &&
				wCount < MAX_SEC_PER_SUBPAC_ABS)
			{
				lpSubpac->wCount++;
				lpCacheHdr->dwCountBlocksCached++;
				return 1;
			}

            // Calculate the difference between the new sector
            //  and the last cached sector

			dwDif = dwSector - dwContiguousSector;

            // If the new sector follows the last cached sector
            //      and the difference is within that allowed by an NDIF
            //      and this ABS packet has at least two subpackets,
            //  then
            //      make the last subpacket and the new sector into an
            //      NDIF packet.

			if (dwContiguousSector < dwSector &&
                dwDif <= PACDIF_MAX_DIF &&
				wCountSubpacs >= 2 && wCount < MAX_SEC_PER_SUBPAC_DIF)
			{
                // Decrement count of subpackets for this ABS packet

                lpPacketHdr->byCountSubpacs--;

				// Create NDIF with two subpacs

				return CreateNDIF(
					lpCacheHdr,
					(LPPACKET_HDR_T)lpSubpac,
					(BYTE)dwDif,
					dwStartSector,
					(BYTE)wCount,
					1);
			}

            // At this point, the new sector is not contiguous
            //  with the last cached sector

            // If the number of subpackets for this ABS packet has been
            //  reached, just create a new NDIF packet

			if (wCountSubpacs == MAX_SUBPACS)
			{
				// Create FIRST_DIF

				return CreateNDIF(
					lpCacheHdr,
					(LPPACKET_HDR_T)(lpSubpac + 1),
					PACTYPE_FIRST_DIF,
					dwSector,1,0);
			}

            // At this point, either the difference between the new sector
            //  and the last cached sector is greater than the maximum
            //  allowed by an NDIF or the number of subpackets was one.

			// New ABS subpac

			if (lpCacheHdr->wLastPacketOffset + sizeof(PACKET_HDR_T) +
				(wCountSubpacs + 1) * sizeof(SUBPAC_ABS_T) >
				CACHE_SIZE)
				return 0;

			lpCacheHdr->dwCountBlocksCached++;
			lpPacketHdr->byCountSubpacs++;
			(++lpSubpac)->dwStartSector = dwSector;
			lpSubpac->wCount = 1;

			return 1;
		}

		default:
		{
			WORD	i;
			LPBYTE	lpbyCount;

			dwStartSector = ((LPPACKET_DIF_T)lpbyPtrCache)->dwStartSector;
			lpbyCount = ((LPPACKET_DIF_T)lpbyPtrCache)->byCounts;
			i = 0;
			while (1)
			{
				dwStartSector += *lpbyCount;
				if (++i >= wCountSubpacs)
					break;
				dwStartSector += byFlags;
				++lpbyCount;
			}

			dwDif = dwSector - dwStartSector;
			if (dwDif == 0)
			{
				if (*lpbyCount < MAX_SEC_PER_SUBPAC_DIF)
				{
					++*lpbyCount;
					lpCacheHdr->dwCountBlocksCached++;
					return 1;
				}

				// Convert to ABS

				lpPacketHdr->byCountSubpacs--;
				if (wCountSubpacs != 1)
				{
                    // Convert last subpac to an ABS subpac

                    // Align on a DWORD boundary

                    if (((lpbyCount - lpbyCache) & 0x03) == 0)
                        nNumPadBytes = 0;
                    else
                        nNumPadBytes = 4 - ((lpbyCount - lpbyCache) & 0x03);

                    if (lpbyCount - lpbyCache + nNumPadBytes +
                        sizeof(PACKET_HDR_T) + sizeof(SUBPAC_ABS_T) >
                        CACHE_SIZE)
						return 0;

					lpCacheHdr->wCountPackets++;

                    // Begin on DWORD boundary

                    lpPacketHdr = (LPPACKET_HDR_T)(lpbyCount + nNumPadBytes);

                    lpCacheHdr->wLastPacketOffset =
                        (LPBYTE)lpPacketHdr - (LPBYTE)lpCacheHdr;
                }

				lpCacheHdr->dwCountBlocksCached++;

				lpPacketHdr->byFlags = PACTYPE_ABS;
				lpPacketHdr->byCountSubpacs = 1;

                lpSubpac = (LPSUBPAC_ABS_T)(lpPacketHdr + 1);

                lpSubpac->dwStartSector = dwStartSector -
                    MAX_SEC_PER_SUBPAC_DIF;
				lpSubpac->wCount = MAX_SEC_PER_SUBPAC_DIF + 1;

				return 1;
			}

			if (dwDif <= PACDIF_MAX_DIF)
			{
				if (lpCacheHdr->wLastPacketOffset + sizeof(PACKET_HDR_T) +
					sizeof(DWORD) + wCountSubpacs * sizeof(BYTE) >
					CACHE_SIZE)
					return 0;

				if (byFlags == PACTYPE_FIRST_DIF ||
					byFlags == dwDif &&
					wCountSubpacs < MAX_SEC_PER_SUBPAC_DIF)
				{
					// Convert to NDIF and add new NDIF

					lpCacheHdr->dwCountBlocksCached++;
                    lpPacketHdr->byFlags = (BYTE)dwDif;
					lpPacketHdr->byCountSubpacs++;
					*++lpbyCount = 1;

					return 1;
				}
			}

			// Create FIRST_DIF

            // The 1 is to account for lpbyCount pointing
            //  to the last subpacket

            if ((wCountSubpacs & 0x03) == 0)
                nNumPadBytes = 1;
            else
                nNumPadBytes = 1 + 4 - (wCountSubpacs & 0x03);

            return CreateNDIF(
				lpCacheHdr,
                (LPPACKET_HDR_T)(lpbyCount + nNumPadBytes),
				PACTYPE_FIRST_DIF,
				dwSector,1,0);
		}
	}
}


//********************************************************************
//
// Function:
//  WORD GetSector()
//
// Description:
//  Returns in *lpdwSector the sector containing dwFindBlock.
//  In the case where the cache does not containg the mapping,
//  *lpdwSector is set to the last known link
//
// Returns:
//  0   If the block is not present in the cache
//  1   If the function found the mapping
//
//********************************************************************

WORD GetSector
(
	LPBYTE			lpbyCache,
	DWORD			dwFindBlock,
	LPDWORD			lpdwSector
)
{
	BYTE			byFlags;
	WORD			i, j, wCountSubpacs, wCountPackets;
	DWORD			dwCurBlock;

	wCountPackets = ((LPLAT_CACHE_HDR_T)lpbyCache)->wCountPackets;
	lpbyCache += sizeof(LAT_CACHE_HDR_T);
	dwCurBlock = 0;
	for (i=0;i<wCountPackets;i++)
	{
		byFlags = ((LPPACKET_HDR_T)lpbyCache)->byFlags;
		wCountSubpacs = ((LPPACKET_HDR_T)lpbyCache)->byCountSubpacs;

		lpbyCache += sizeof(PACKET_HDR_T);
		switch (byFlags)
		{
			case PACTYPE_ABS:
			{
				LPSUBPAC_ABS_T lpSubpac;

				lpSubpac = ((LPPACKET_ABS_T)lpbyCache)->subpacs;
				for (j=0;j<wCountSubpacs;j++)
				{
					if (dwCurBlock <= dwFindBlock &&
						dwFindBlock < dwCurBlock + lpSubpac->wCount)
					{
						*lpdwSector = lpSubpac->dwStartSector +
							(dwFindBlock - dwCurBlock);
						return 1;
					}

					dwCurBlock += lpSubpac->wCount;
					++lpSubpac;
				}
                *lpdwSector = (lpSubpac-1)->dwStartSector +
                    (lpSubpac-1)->wCount - 1;
				lpbyCache = (LPBYTE)lpSubpac;
				break;
			}
			default:
			{
				DWORD 	dwStartSector;
				LPBYTE	lpbyCount;

				dwStartSector = ((LPPACKET_DIF_T)lpbyCache)->dwStartSector;
				lpbyCount = ((LPPACKET_DIF_T)lpbyCache)->byCounts;
				for (j=0;j<wCountSubpacs;j++)
				{
					if (dwCurBlock <= dwFindBlock &&
						dwFindBlock < dwCurBlock + *lpbyCount)
					{
						*lpdwSector = dwStartSector +
							(dwFindBlock - dwCurBlock);
						return 1;
					}

					dwStartSector += *lpbyCount + byFlags;
					dwCurBlock += *lpbyCount;
					++lpbyCount;
				}
				*lpdwSector = dwStartSector - byFlags - 1;
				lpbyCache = lpbyCount;

                // Jump to the next DWORD boundary

                if ((wCountSubpacs & 0x03) != 0)
                    lpbyCache += 4 - (wCountSubpacs & 0x03);

				break;
			}
		}
	}

	// Return last known link

	return 0;
}

#endif // !NO_BAT_CACHE

//////////////////////////////////////////////////////////////////////
// End BAT cache functions
//////////////////////////////////////////////////////////////////////


//********************************************************************
//
// Function:
//	DWORD OLESeekStreamBlk512()
//
// Description:
//	Finds the offset of the dwStreamBlk block of the given stream
//	within the OLE file.
//
// Returns:
//	dwStreamBlk		on success
//	OLE_ERR_???		on error
//
//  *pdwBlkFileOffset is set to the offset of the block on success.
//
//********************************************************************

DWORD OLESeekStreamBlk512
(
	LPCALLBACKREV1 	lpCallBack,				// File op callbacks
	LPOLE_FILE_T	pOLEFile,				// OLE file info
	LPSTREAM_INFO_T	psiStreamInfo,			// Stream info
	DWORD 			dwStreamBlk,			// Block to seek to
	LPDWORD			pdwBlkFileOffset		// For storing blk offset
)
{
	DWORD dwCurBlk, dwMoreBATBlk;
	LPDWORD dwBAT512;
	DWORD dwBAT512Offset, dwOffset;
	DWORD dwMinLink, dwMaxLink, dwLink;

#ifdef SYM_NLM
    DWORD dwRelinguishCount = 0;

    lpCallBack->Progress(0,0);  // Used to relinguish control under NLM
#endif

#ifndef NO_BAT_CACHE
    // Checked for a cached link

    if (psiStreamInfo->lpbyCache)
	{
		if (GetSector(psiStreamInfo->lpbyCache,dwStreamBlk,
			&dwOffset) != 0)
		{
			*pdwBlkFileOffset = (dwOffset + 1) * BLK_SIZE;

			return dwStreamBlk;
		}

		dwCurBlk = ((LPLAT_CACHE_HDR_T)psiStreamInfo->lpbyCache)->
			dwCountBlocksCached - 1;
		dwLink = dwOffset;
	}
	else
	{
		dwCurBlk = 0;
		dwLink = psiStreamInfo->dwStartBlk;;
	}

#endif // !NO_BAT_CACHE

    // If the block to search for in the stream is less than the
	// last seeked block in the stream or the last seeked
	// block is -1, then start over by following the links
	// beginning with the first block of the stream

	dwBAT512 = pOLEFile->dwBAT512;
#ifdef NO_BAT_CACHE
    if (dwStreamBlk < psiStreamInfo->dwStreamBlk ||
		psiStreamInfo->dwStreamBlk == (DWORD)(-1))
#else // !NO_BAT_CACHE
    if (dwCurBlk != 0 ||
        dwStreamBlk < psiStreamInfo->dwStreamBlk ||
        psiStreamInfo->dwStreamBlk == (DWORD)(-1))
#endif // !NO_BAT_CACHE
    {
		// Calculate the offset of the BAT512 for the first block
		// of the stream:
		//	- pOLEFile->dwStartBlk is the block location in the
		//	  storage object of the first block of the stream.
		//  - The block locations of the BAT512s are stored
		//	  linearly beginning at offset OLE_HDR_BAT512_BLK
		//	  in the header of the storage object.
		//	- dwCurBlk is initialized to zero to indicate that
		//    we begin following the links in the BAT for this
		//	  stream with the first block of the stream

#ifdef NO_BAT_CACHE
		dwCurBlk = 0;
		dwLink = psiStreamInfo->dwStartBlk;
#endif

		// 13952 = (BLK_SIZE/4) * ((BLK_SIZE-OLE_HDR_BAT512_BLK)/4)

		if (dwLink >= pOLEFile->dwMinLink &&
			dwLink <= pOLEFile->dwMaxLink)
		{
			dwBAT512Offset = pOLEFile->dwBAT512Offset;
		}
		else
		{
			if (dwLink < (DWORD)(13952))
			{
				dwOffset = (DWORD)OLE_HDR_BAT512_BLK +
					(DWORD)4 * (dwLink / BLKS_PER_BAT);

				if (lpCallBack->FileSeek(pOLEFile->hFile,
					dwOffset,SEEK_SET) != dwOffset)
					return OLE_ERR_SEEK;
			}
			else
			if (pOLEFile->dwNumMoreBATBlk > 0 &&
				dwLink <= pOLEFile->dwMaxPosLink)
			{
				dwMinLink = dwLink - (DWORD)(13952);
				dwMoreBATBlk = pOLEFile->dwMoreBAT512Blk;
				while (dwMinLink >= BLKS_PER_BAT * (BLK_SIZE / 4 - 1))
				{
					// Find next More BAT Block

					dwOffset = (dwMoreBATBlk + 2) * BLK_SIZE - 4;
					if (lpCallBack->FileSeek(pOLEFile->hFile,
						dwOffset,SEEK_SET) != dwOffset)
						return OLE_ERR_SEEK;

					if (lpCallBack->FileRead(pOLEFile->hFile,
						(LPBYTE)&dwMoreBATBlk,sizeof(DWORD)) !=
						sizeof(DWORD))
						return OLE_ERR_READ;

                    dwMoreBATBlk = ConvertEndianLong(dwMoreBATBlk);

					dwMinLink -= BLKS_PER_BAT * (BLK_SIZE / 4 - 1);

#ifdef SYM_NLM
                    if ((dwRelinguishCount++ % 32) == 0)
                        lpCallBack->Progress(0,0);
#endif
                }

                dwOffset = (dwMoreBATBlk + 1) * BLK_SIZE +
					(dwMinLink / BLKS_PER_BAT) * 4;

				if (lpCallBack->FileSeek(pOLEFile->hFile,
					dwOffset,SEEK_SET) != dwOffset)
					return OLE_ERR_SEEK;
			}
			else
				return OLE_ERR_SEEK;

			if (lpCallBack->FileRead(pOLEFile->hFile,
				(LPBYTE)&dwBAT512Offset,sizeof(DWORD)) !=
				sizeof(DWORD))
				return OLE_ERR_READ;

            dwBAT512Offset = (ConvertEndianLong(dwBAT512Offset) + 1) *
                BLK_SIZE;
		}
	}
	else
	{
		// Use the cached info to continue searching forward
		//	- psiStreamInfo->dwStreamBlk is the last block that was
		//	  seeked for this stream
		//	- psiStreamInfo->dwLink is the link pointing to
		//	  psiStreamInfo->dwStreamBlk.
		//	- dwBAT512Offset is the byte location within the file
		//	  of the BAT for the psiStreamInfo->dwLink block

		dwCurBlk = psiStreamInfo->dwStreamBlk;
		dwLink = psiStreamInfo->dwLink;
		dwBAT512Offset = psiStreamInfo->dwBATOffset;
	}

	if (dwLink < pOLEFile->dwMinLink ||
		dwLink > pOLEFile->dwMaxLink)
	{
		// Read the BAT

		if (lpCallBack->FileSeek(pOLEFile->hFile,
			dwBAT512Offset,SEEK_SET) != dwBAT512Offset)
			return OLE_ERR_SEEK;

		if (lpCallBack->FileRead(pOLEFile->hFile,
			(LPBYTE)dwBAT512,sizeof(DWORD)*BLKS_PER_BAT) !=
			sizeof(DWORD) * BLKS_PER_BAT)
		{
			pOLEFile->dwMinLink = pOLEFile->dwMaxLink = (DWORD)(-1);

			return OLE_ERR_READ;
		}

		// Calculate min and max links in this BAT

		dwMinLink = dwLink - dwLink % BLKS_PER_BAT;
		dwMaxLink = dwMinLink + BLKS_PER_BAT - 1;
	}
	else
	{
		dwMinLink = pOLEFile->dwMinLink;
		dwMaxLink = pOLEFile->dwMaxLink;
	}

	// Now follow links until we get to the link pointing
	// to dwStreamBlk

	while (dwCurBlk < dwStreamBlk)
	{
        dwLink = ConvertEndianLong(dwBAT512[dwLink % BLKS_PER_BAT]);

		if (dwLink == (DWORD)LINK_LAST_BLK)
		{
			// Error, attempt to seek past the end, so reset

			psiStreamInfo->dwStreamBlk = (DWORD)(-1);
			return OLE_ERR_READ;
		}

#ifndef NO_BAT_CACHE
        if (psiStreamInfo->lpbyCache)
        {
            if (((LPLAT_CACHE_HDR_T)(psiStreamInfo->lpbyCache))->
                dwCountBlocksCached == dwCurBlk + 1)
                AddSector(psiStreamInfo->lpbyCache,dwLink);
        }
#endif // !NO_BAT_CACHE

        if (dwLink < dwMinLink || dwLink > dwMaxLink)
		{
#ifdef SYM_NLM
            if ((dwRelinguishCount++ % 32) == 0)
                lpCallBack->Progress(0,0);
#endif

            // Need to read a new BAT

			if (dwLink < (DWORD)(13952))
			{
				dwOffset = (DWORD)OLE_HDR_BAT512_BLK +
					(DWORD)4 * (dwLink / BLKS_PER_BAT);

				if (lpCallBack->FileSeek(pOLEFile->hFile,
					dwOffset,SEEK_SET) != dwOffset)
					return OLE_ERR_SEEK;
			}
			else
			if (pOLEFile->dwNumMoreBATBlk > 0 &&
				dwLink <= pOLEFile->dwMaxPosLink)
			{
				dwMinLink = dwLink - (DWORD)(13952);
				dwMoreBATBlk = pOLEFile->dwMoreBAT512Blk;
				while (dwMinLink >= BLKS_PER_BAT * (BLK_SIZE / 4 - 1))
				{
					// Find next More BAT Block

					dwOffset = (dwMoreBATBlk + 2) * BLK_SIZE - 4;
					if (lpCallBack->FileSeek(pOLEFile->hFile,
						dwOffset,SEEK_SET) != dwOffset)
						return OLE_ERR_SEEK;

					if (lpCallBack->FileRead(pOLEFile->hFile,
						(LPBYTE)&dwMoreBATBlk,sizeof(DWORD)) !=
						sizeof(DWORD))
						return OLE_ERR_READ;

                    dwMoreBATBlk = ConvertEndianLong(dwMoreBATBlk);

					dwMinLink -= BLKS_PER_BAT * (BLK_SIZE / 4 - 1);

#ifdef SYM_NLM
                    if ((dwRelinguishCount++ % 32) == 0)
                        lpCallBack->Progress(0,0);
#endif
                }

				dwOffset = (dwMoreBATBlk + 1) * BLK_SIZE +
					(dwMinLink / BLKS_PER_BAT) * 4;
				if (lpCallBack->FileSeek(pOLEFile->hFile,
					dwOffset,SEEK_SET) != dwOffset)
					return OLE_ERR_SEEK;
			}
			else
				return OLE_ERR_SEEK;

			if (lpCallBack->FileRead(pOLEFile->hFile,
				(LPBYTE)&dwBAT512Offset,sizeof(DWORD)) !=
				sizeof(DWORD))
				return OLE_ERR_READ;

            dwBAT512Offset = (ConvertEndianLong(dwBAT512Offset) + 1) *
                BLK_SIZE;

			// Read the BAT

			if (lpCallBack->FileSeek(pOLEFile->hFile,
				dwBAT512Offset,SEEK_SET) != dwBAT512Offset)
				return OLE_ERR_SEEK;

			if (lpCallBack->FileRead(pOLEFile->hFile,
				(LPBYTE)dwBAT512,sizeof(DWORD)*BLKS_PER_BAT) !=
				sizeof(DWORD)*BLKS_PER_BAT)
			{
				pOLEFile->dwMinLink = pOLEFile->dwMaxLink = (DWORD)(-1);

				return OLE_ERR_READ;
            }

			// Calculate min and max links in this BAT

			dwMinLink = dwLink - dwLink % BLKS_PER_BAT;
			dwMaxLink = dwMinLink + BLKS_PER_BAT - 1;
		}

		++dwCurBlk;
	}

	// Cache the info

	psiStreamInfo->dwStreamBlk = dwCurBlk;
	psiStreamInfo->dwLink = dwLink;
	psiStreamInfo->dwBATOffset = dwBAT512Offset;

	pOLEFile->dwBAT512Offset = dwBAT512Offset;
	pOLEFile->dwMinLink = dwMinLink;
	pOLEFile->dwMaxLink = dwMaxLink;

	*pdwBlkFileOffset = (dwLink + 1) * BLK_SIZE;

	return (dwCurBlk);
}


//********************************************************************
//
// Function:
//	WORD OLESeekRead512()
//
// Description:
//	Seeks to dwOffset within the given stream and reads wBytesToRead
//	bytes into the read buffer.
//
// Returns:
//	wBytesToRead	on success
//	OLE_ERR_???		on error
//
//********************************************************************

WORD OLESeekRead512
(
	LPCALLBACKREV1 	lpCallBack,             // File op callbacks
	LPOLE_FILE_T	pOLEFile,				// OLE file info
	LPSTREAM_INFO_T	psiStreamInfo,			// Stream info
	DWORD 			dwOffset,				// Read offset in stream
	LPBYTE 			lpbBuffer,				// Read buffer
	WORD 			wBytesToRead			// Number of bytes to read
)
{
	DWORD 	dwBegBlk, dwEndBlk, dwCurBlk;
	WORD	wBytesRead;
	DWORD	dwBlkFileOffset, dwSeekOffset;

	dwBegBlk = dwOffset / BLK_SIZE;
	dwEndBlk = (dwOffset + wBytesToRead - 1) / BLK_SIZE;

	wBytesRead = 0;

	// Read bytes in first block

	if (OLESeekStreamBlk512(lpCallBack,
		pOLEFile,psiStreamInfo,
		dwBegBlk,&dwBlkFileOffset) != dwBegBlk)
		return OLE_ERR_SEEK;

	wBytesRead = (WORD)(BLK_SIZE - (dwOffset % BLK_SIZE));

	if (wBytesRead > wBytesToRead)
		wBytesRead = wBytesToRead;

	dwSeekOffset = dwBlkFileOffset + dwOffset % BLK_SIZE;
	if (lpCallBack->FileSeek(pOLEFile->hFile,
		dwSeekOffset,SEEK_SET) != dwSeekOffset)
	{
		// Seek failed

		return OLE_ERR_SEEK;
	}

	if (lpCallBack->FileRead(pOLEFile->hFile,
		lpbBuffer,wBytesRead) != wBytesRead)
	{
		// Read failed

		return OLE_ERR_READ;
	}

	// Return immediately if there are no more bytes to read

	if (dwBegBlk == dwEndBlk)
		return wBytesToRead;

	lpbBuffer += wBytesRead;

	// Now read middle blocks

	for (dwCurBlk=dwBegBlk+1;dwCurBlk<dwEndBlk;dwCurBlk++)
	{
		if (OLESeekStreamBlk512(lpCallBack,
			pOLEFile,psiStreamInfo,
			dwCurBlk,&dwBlkFileOffset) != dwCurBlk)
			return OLE_ERR_SEEK;

		if (lpCallBack->FileSeek(pOLEFile->hFile,
			dwBlkFileOffset,SEEK_SET) != dwBlkFileOffset)
		{
			// Seek failed

			return OLE_ERR_SEEK;
		}

		if (lpCallBack->FileRead(pOLEFile->hFile,
			lpbBuffer,BLK_SIZE) != BLK_SIZE)
		{
			// Read failed

			return OLE_ERR_READ;
		}

		lpbBuffer += BLK_SIZE;
	}

	// Read last block

	if (OLESeekStreamBlk512(lpCallBack,
		pOLEFile,psiStreamInfo,
		dwEndBlk,&dwBlkFileOffset) != dwEndBlk)
		return OLE_ERR_SEEK;

	if (lpCallBack->FileSeek(pOLEFile->hFile,
		dwBlkFileOffset,SEEK_SET) != dwBlkFileOffset)
	{
		// Seek failed

		return OLE_ERR_SEEK;
	}

	wBytesRead = (WORD)((dwOffset + wBytesToRead) % BLK_SIZE);

    // Take care of boundary condition

    if (wBytesRead == 0)
        wBytesRead = BLK_SIZE;

	if (lpCallBack->FileRead(pOLEFile->hFile,
		lpbBuffer,wBytesRead) != wBytesRead)
	{
		// Read failed

		return OLE_ERR_READ;
	}

	return (wBytesToRead);
}


//********************************************************************
//
// Function:
//	DWORD OLESeekStreamBlk64()
//
// Description:
//	Finds the offset of the dwStreamBlk block of the given stream
//	within the OLE file.  This function is used for streams stored
//	within the Stream64 stream.
//
// Returns:
//	dwStreamBlk		on success
//	OLE_ERR_???		on error
//
//  *pdwBlkFileOffset is set to the offset of the block on success.
//
//********************************************************************

DWORD OLESeekStreamBlk64
(
	LPCALLBACKREV1 	lpCallBack,				// File op callbacks
	LPOLE_FILE_T	pOLEFile,				// OLE file info
	LPSTREAM_INFO_T	psiStreamInfo,			// Stream info
	DWORD 			dwStreamBlk,			// Block to seek to
	LPDWORD			pdwBlkStreamOffset		// For storing blk offset
)
{
	DWORD dwCurBlk;
	DWORD dwLink;

	// If the block to search for in the stream is less than the
	// last seeked block in the stream or the last seeked
	// block is -1, then start over by following the links
	// beginning with the first block of the stream

	if (dwStreamBlk < psiStreamInfo->dwStreamBlk ||
		psiStreamInfo->dwStreamBlk == (DWORD)(-1))
	{
		//	- pOLEFile->dwStartBlk is the first subblock of the
		//	  stream in the subblock stream of the storage object
		//	- dwCurBlk is initialized to zero to indicate that
		//    we begin following the links in the BAT for this
		//	  stream with the first subblock of the stream

    	dwCurBlk = 0;
		dwLink = psiStreamInfo->dwStartBlk;
	}
	else
	{
		// Use the cached info to continuing searching forward
		//	- psiStreamInfo->dwStreamBlk is the last block that was
		//	  seeked for this stream
		//	- psiStreamInfo->dwLink is the link pointing to
		//	  psiStreamInfo->dwStreamBlk.

		dwCurBlk = psiStreamInfo->dwStreamBlk;
		dwLink = psiStreamInfo->dwLink;
	}

	// Now follow links until we get to the link pointing
	// to dwStreamBlk

	while (dwCurBlk < dwStreamBlk)
	{
		// Get next link

		if (OLESeekRead512(lpCallBack,
			pOLEFile,&(pOLEFile->siBAT64Stream),
			dwLink*4,(LPBYTE)(&dwLink),4) != 4)
			return OLE_ERR_SEEK;

        dwLink = ConvertEndianLong(dwLink);

		if (dwLink == (DWORD)LINK_LAST_BLK)
		{
			// Error, attempt to seek past the end, so reset

        	psiStreamInfo->dwStreamBlk = (DWORD)(-1);
			return OLE_ERR_READ;
		}

		++dwCurBlk;
	}

	// Cache the info

	psiStreamInfo->dwStreamBlk = dwCurBlk;
	psiStreamInfo->dwLink = dwLink;

	*pdwBlkStreamOffset = dwLink * BLK64_SIZE;

	return (dwCurBlk);
}


//********************************************************************
//
// Function:
//	WORD OLESeekRead64()
//
// Description:
//	Seeks to dwOffset within the given stream and reads wBytesToRead
//	bytes into the read buffer.  This function is used for streams
//	stored within the Stream64 stream.
//
// Returns:
//	wBytesToRead	on success
//	OLE_ERR_???		on error
//
//********************************************************************

WORD OLESeekRead64
(
	LPCALLBACKREV1 	lpCallBack,				// File op callbacks
	LPOLE_FILE_T	pOLEFile,				// OLE file info
	LPSTREAM_INFO_T	psiStreamInfo,			// Stream info
	DWORD 			dwOffset,				// Read offset in stream
	LPBYTE 			lpbBuffer,				// Read buffer
	WORD 			wBytesToRead			// Number of bytes to read
)
{
	DWORD 	dwBegBlk, dwEndBlk, dwCurBlk;
	WORD	wBytesRead;
	DWORD	dwBlkStreamOffset;

	dwBegBlk = dwOffset / BLK64_SIZE;
	dwEndBlk = (dwOffset + wBytesToRead - 1) / BLK64_SIZE;

	wBytesRead = 0;

	// Read bytes in first subblock

	if (OLESeekStreamBlk64(lpCallBack,
		pOLEFile,psiStreamInfo,
		dwBegBlk,&dwBlkStreamOffset) != dwBegBlk)
		return OLE_ERR_SEEK;

	wBytesRead = (WORD)(BLK64_SIZE - (dwOffset % BLK64_SIZE));

	if (wBytesRead > wBytesToRead)
		wBytesRead = wBytesToRead;

	if (OLESeekRead512(lpCallBack,
		pOLEFile,&(pOLEFile->siBlk64Stream),
		dwBlkStreamOffset + dwOffset % BLK64_SIZE,
		lpbBuffer,wBytesRead) != wBytesRead)
		return OLE_ERR_READ;

	// Return immediately if there are no more bytes to read

	if (dwBegBlk == dwEndBlk)
		return wBytesToRead;

	lpbBuffer += wBytesRead;

	// Now read middle blocks

	for (dwCurBlk=dwBegBlk+1;dwCurBlk<dwEndBlk;dwCurBlk++)
	{
		if (OLESeekStreamBlk64(lpCallBack,
			pOLEFile,psiStreamInfo,
			dwCurBlk,&dwBlkStreamOffset) != dwCurBlk)
			return OLE_ERR_SEEK;

		if (OLESeekRead512(lpCallBack,
			pOLEFile,&(pOLEFile->siBlk64Stream),
			dwBlkStreamOffset,lpbBuffer,BLK64_SIZE) != BLK64_SIZE)
			return OLE_ERR_READ;

		lpbBuffer += BLK64_SIZE;
	}

	// Read last block

	if (OLESeekStreamBlk64(lpCallBack,
		pOLEFile,psiStreamInfo,
		dwEndBlk,&dwBlkStreamOffset) != dwEndBlk)
		return OLE_ERR_SEEK;

	wBytesRead = (WORD)((dwOffset + wBytesToRead) % BLK64_SIZE);

    // Take care of boundary condition

    if (wBytesRead == 0)
    	wBytesRead = BLK64_SIZE;

	if (OLESeekRead512(lpCallBack,
		pOLEFile,&(pOLEFile->siBlk64Stream),
		dwBlkStreamOffset,lpbBuffer,wBytesRead) != wBytesRead)
		return OLE_ERR_READ;

	return (wBytesToRead);
}


//********************************************************************
//
// Function:
//	WORD OLESeekRead()
//
// Description:
//	Seeks to dwOffset within the given stream and reads wBytesToRead
//	bytes into the read buffer.
//
// Returns:
//	wBytesToRead	on success
//	OLE_ERR_???		on error
//
//********************************************************************

WORD OLESeekRead
(
	LPCALLBACKREV1 	lpCallBack,				// File op callbacks
	LPOLE_FILE_T	pOLEFile,				// OLE file info
	DWORD 			dwOffset,				// Read offset in stream
	LPBYTE 			lpbBuffer,				// Read buffer
	WORD 			wBytesToRead			// Number of bytes to read
)
{
	if (pOLEFile->dwBATType == BAT512)
	{
		return OLESeekRead512(lpCallBack,
			pOLEFile,&(pOLEFile->siStreamInfo),
			dwOffset,lpbBuffer,wBytesToRead);
	}
	else
	if (pOLEFile->dwBATType == BAT64)
	{
		return OLESeekRead64(lpCallBack,
			pOLEFile,&(pOLEFile->siStreamInfo),
			dwOffset,lpbBuffer,wBytesToRead);
	}

	return OLE_ERR_SEEK;
}


//********************************************************************
//
// Function:
//  WORD OLEOpenPreprocess()
//
// Description:
//  Performs preprocessing for all OLE opens.
//
// Returns:
//  wBytesToRead    on success
//  OLE_ERR_???     on error
//
//********************************************************************

WORD OLEOpenPreprocess
(
    LPCALLBACKREV1  lpstCallBack,           // File op callbacks
    HFILE           hFile,                  // Handle to OLE file
    LPOLE_FILE      lpstOLEFile             // OLE file info
)
{
    DWORD           dwDirBlk;
    LPBYTE          lpbySig;

    lpstOLEFile->hFile = hFile;

    // Seek to the beginning of the file

    if (lpstCallBack->FileSeek(lpstOLEFile->hFile,
                               0,
                               SEEK_SET) != 0)
    {
        // Seek failed

        return OLE_ERR_SEEK;
    }

    // Read in the Header up to the first (OLE_HDR_BAT512_BLK + 4) bytes
    // Use dwBAT512 as a buffer

    if (lpstCallBack->FileRead(lpstOLEFile->hFile,
                               (LPBYTE)(lpstOLEFile->dwBAT512),
                               (OLE_HDR_BAT512_BLK + 4)) !=
        (OLE_HDR_BAT512_BLK + 4))
    {
        // Read failed

        return OLE_ERR_READ;
    }

    // Verify that this is an OLE storage file
    //  Signature: 0xD? 0xCF 0x11 0xE0 0xA1 0xB1 0x1A 0xE1

    lpbySig = (LPBYTE)(lpstOLEFile->dwBAT512);
    if (((lpbySig[0] & 0xF0) != 0xD0) || (lpbySig[1] != 0xCF) ||
        (lpbySig[2] != 0x11) || (lpbySig[3] != 0xE0) ||
        (lpbySig[4] != 0xA1) || (lpbySig[5] != 0xB1) ||
        (lpbySig[6] != 0x1A) || (lpbySig[7] != 0xE1))
    {
        // Not an OLE storage file

        return OLE_ERR_READ;
    }

    // Get number of BAT512s

    lpstOLEFile->dwNumBAT512 = ConvertEndianLong(
        *((LPDWORD)((LPBYTE)(lpstOLEFile->dwBAT512) + OLE_HDR_NUM_BAT512)));

    // Get first block of directory

    dwDirBlk = ConvertEndianLong(
        *((LPDWORD)((LPBYTE)(lpstOLEFile->dwBAT512) + OLE_HDR_DIR_BLK)));

    // Get first block containing BAT64s

    lpstOLEFile->siBAT64Stream.dwStartBlk = ConvertEndianLong(
        *((LPDWORD)((LPBYTE)(lpstOLEFile->dwBAT512) + OLE_HDR_BAT64_BLK)));

    // Get block continuing BAT512 locations array

    lpstOLEFile->dwMoreBAT512Blk = ConvertEndianLong(
        *((LPDWORD)((LPBYTE)(lpstOLEFile->dwBAT512) + OLE_HDR_MORE_BAT512_BLK)));

    // Get number of additional blocks

    lpstOLEFile->dwNumMoreBATBlk = ConvertEndianLong(
        *((LPDWORD)((LPBYTE)(lpstOLEFile->dwBAT512) +
            OLE_HDR_NUM_MORE_BAT_BLK)));

    // Assuming an OLE file cannot be larger than 4G

    if (lpstOLEFile->dwNumMoreBATBlk >= 0x800000lu)
        return OLE_ERR_NO_ENTRY;

    // Note maximum possible link value

    lpstOLEFile->dwMaxPosLink = (DWORD)(109*128) - 1 +
        (DWORD)(127 * 128) * lpstOLEFile->dwNumMoreBATBlk;

    if ((lpstOLEFile->dwMoreBAT512Blk >= 0x800000lu ||
        lpstOLEFile->dwMoreBAT512Blk > lpstOLEFile->dwMaxPosLink) &&
        lpstOLEFile->dwMoreBAT512Blk != 0xFFFFFFFElu)
        return OLE_ERR_NO_ENTRY;

    // Read in the first BAT512

    lpstOLEFile->dwBAT512Offset = (DWORD)BLK_SIZE * (ConvertEndianLong(
        *((LPDWORD)((LPBYTE)(lpstOLEFile->dwBAT512) + OLE_HDR_BAT512_BLK))) + 1);

    if (lpstCallBack->FileSeek(lpstOLEFile->hFile,
                               lpstOLEFile->dwBAT512Offset,
                               SEEK_SET) != lpstOLEFile->dwBAT512Offset)
    {
        // Seek failed

        return OLE_ERR_SEEK;
    }

    if (lpstCallBack->FileRead(lpstOLEFile->hFile,
                               (LPBYTE)(lpstOLEFile->dwBAT512),
                               sizeof(DWORD) * BLKS_PER_BAT) !=
        sizeof(DWORD) * BLKS_PER_BAT)
    {
        // Read failed

        return OLE_ERR_READ;
    }

    lpstOLEFile->dwMinLink = 0;
    lpstOLEFile->dwMaxLink = BLKS_PER_BAT - 1;

    // Now find stream name in directory
    // Search up to 64K entries

    lpstOLEFile->dwBATType = BAT512;
    lpstOLEFile->siStreamInfo.dwStartBlk = dwDirBlk;
    lpstOLEFile->siStreamInfo.dwStreamLen = (DWORD)16384 * (DWORD)BLK_SIZE;
    lpstOLEFile->siStreamInfo.dwStreamBlk = (DWORD)(-1);

    lpstOLEFile->siStreamInfo.lpbyCache = NULL;    // Dir search/no cache

    return(OLE_OK);
}


//********************************************************************
//
// Function:
//  void OLEInitOpenOLEFile()
//
// Description:
//  Performs initialization of OLE_FILE_T structure
//  for an entry.
//
// Returns:
//  wBytesToRead    on success
//  OLE_ERR_???     on error
//
//********************************************************************

void OLEInitOpenOLEFile
(
    LPOLE_FILE      lpstOLEFile,            // OLE file info
    LPOLE_DIR_ENTRY lpstEntry,              // The entry to open
    DWORD           dwEntryNum,             // The directory entry number
    LPBYTE          lpbyStreamBATCache      // non NULL if cache
)
{
    DWORD           dwCount;

    // Store the entry number

    lpstOLEFile->dwEntryNum = dwEntryNum;

    // If the stream length is less than 4096
    // then use BAT64, otherwise use BAT512 for >= 4096.

    if (ConvertEndianLong(lpstEntry->dwStreamLen) >= 4096)
    {
        lpstOLEFile->dwBATType = BAT512;
    }
    else
    {
        lpstOLEFile->dwBATType = BAT64;

        // Calculate number of 64-byte subblocks

        dwCount = lpstOLEFile->siBlk64Stream.dwStreamLen /
            BLK64_SIZE;

        if (lpstOLEFile->siBlk64Stream.dwStreamLen % BLK64_SIZE != 0)
            ++dwCount;

        // Calculate how many BAT64s are required.
        // This is an approximation.

        dwCount = ((dwCount * 4) / BLK_SIZE + 1) * BLK_SIZE;

        lpstOLEFile->siBAT64Stream.dwStreamLen = dwCount;
        lpstOLEFile->siBAT64Stream.dwStreamBlk = (DWORD)(-1);

        lpstOLEFile->siBlk64Stream.lpbyCache = NULL; // No Blk64S cache
        lpstOLEFile->siBAT64Stream.lpbyCache = NULL; // No BAT64S cache
    }

    lpstOLEFile->siStreamInfo.dwStartBlk =
        ConvertEndianLong(lpstEntry->dwStartBlk);
    lpstOLEFile->siStreamInfo.dwStreamLen =
        ConvertEndianLong(lpstEntry->dwStreamLen);
    lpstOLEFile->siStreamInfo.dwStreamBlk = (DWORD)(-1);

    // Initialize stream BAT cache

    lpstOLEFile->siStreamInfo.lpbyCache = lpbyStreamBATCache;

#ifndef NO_BAT_CACHE
    if (lpstOLEFile->siStreamInfo.lpbyCache != NULL)
    {
        // Initialize the BAT cache

        InitMacroPacket(lpstOLEFile->siStreamInfo.lpbyCache,
            lpstOLEFile->siStreamInfo.dwStartBlk);
    }
#endif // !NO_BAT_CACHE
}


//********************************************************************
//
// Function:
//	DWORD OLEStreamLen()
//
// Returns:
//	Returns the length of the currently open stream.
//
//********************************************************************

DWORD OLEStreamLen
(
    LPOLE_FILE_T    lpstOLEFile             // OLE file info
)
{
    return (lpstOLEFile->siStreamInfo.dwStreamLen);
}


#ifndef SYM_NLM

//********************************************************************
//
// Function:
//  DWORD OLEStreamEntryNum()
//
// Returns:
//  Returns the entry number of the currently open stream.
//
//********************************************************************

DWORD OLEStreamEntryNum
(
    LPOLE_FILE_T    lpstOLEFile             // OLE file info
)
{
    return (lpstOLEFile->dwEntryNum);
}

#endif

#ifndef SYM_NLM

//********************************************************************
//
// Function:
//	WORD OLEOpenStream()
//
// Description:
//	Opens the stream named by puszStreamName in the OLE file.
//	A given name may appear multiple times in the directory,
//  so *lpwDirEntryNum is used to continue searching beginning
//  with that index in the directory.
//
//	This function should always be called first with
//  *lpwDirEntryNum = 0.
//
// Returns:
//	OLE_OK			on success
//	OLE_ERR_???		on error
//
//********************************************************************

BYTE gpuszRootEntry[] =
{
    'R', 0, 'o', 0, 'o', 0, 't', 0, ' ', 0,
    'E', 0, 'n', 0, 't', 0, 'r', 0, 'y', 0, 0, 0
};

// First call should always be zero

WORD OLEOpenStream
(
	LPCALLBACKREV1 	lpstCallBack,			// File op callbacks
	HFILE 			hFile,					// Handle to OLE file
	LPOLE_FILE 	    lpstOLEFile,			// OLE file info
	LPWORD			puszStreamName,			// Stream to open
    LPBYTE          lpbyStreamBATCache,     // Non NULL if cache, 512 bytes
    LPWORD          lpwDirEntryNum          // Starting entry to search
)
{
	OLE_DIR_ENTRY_T oleDirEntries[4];
    DWORD           dwCurBlk, dwResult;
    WORD            i, wCurEntryNum;

    if (OLEOpenPreprocess(lpstCallBack,
                          hFile,
                          lpstOLEFile) != OLE_OK)
    {
        return(OLE_ERR_READ);
    }

    wCurEntryNum = (*lpwDirEntryNum / 4) * 4;
    for (dwCurBlk=wCurEntryNum/4;dwCurBlk<512;dwCurBlk++)
    {
        // Read the directory block

        dwResult = OLESeekRead512(lpstCallBack,
            lpstOLEFile,
            &(lpstOLEFile->siStreamInfo),
            dwCurBlk * BLK_SIZE,(LPBYTE)oleDirEntries,BLK_SIZE);

        if (dwResult != BLK_SIZE)
            return (WORD)(dwResult);

		for (i=0;i<4;i++)
		{
            if (wCurEntryNum++ < *lpwDirEntryNum)
				continue;

			if (i == 0 && dwCurBlk == 0)
			{
				// Get the starting block containing
				// 64-byte subblocks

                lpstOLEFile->siBlk64Stream.dwStartBlk =
                    ConvertEndianLong(oleDirEntries[i].dwStartBlk);

                lpstOLEFile->siBlk64Stream.dwStreamLen =
                    ConvertEndianLong(oleDirEntries[i].dwStreamLen);

                lpstOLEFile->siBlk64Stream.dwStreamBlk = (DWORD)(-1);

				continue;
			}

			// See if this directory block contains the entry
			// and verify that it is a stream object

			if (WStrNCmp(puszStreamName,
				oleDirEntries[i].uszName,32) == 0)
			{
				// Bingo!

                OLEInitOpenOLEFile(lpstOLEFile,
                                   oleDirEntries+i,
                                   wCurEntryNum - 1,
                                   lpbyStreamBATCache);

                *lpwDirEntryNum = wCurEntryNum;

				return OLE_OK;
			}
		}
	}

	return OLE_ERR_NO_ENTRY;
}


//********************************************************************
//
// Function:
//  WORD OLEOpenStreamEntryNum()
//
// Description:
//  Opens the stream at the given entry.
//
// Returns:
//	OLE_OK			on success
//	OLE_ERR_???		on error
//
//********************************************************************

WORD OLEOpenStreamEntryNum
(
	LPCALLBACKREV1 	lpstCallBack,			// File op callbacks
	HFILE 			hFile,					// Handle to OLE file
	LPOLE_FILE 	    lpstOLEFile,			// OLE file info
    WORD            wEntryNum,              // Entry to open
    LPBYTE          lpbyStreamBATCache      // Non NULL if cache, 512 bytes
)
{
    OLE_DIR_ENTRY_T stOLEDirEntry;
    DWORD           dwResult;

    if (OLEOpenPreprocess(lpstCallBack,
                          hFile,
                          lpstOLEFile) != OLE_OK)
    {
        return(OLE_ERR_READ);
    }

    dwResult = OLESeekRead512(lpstCallBack,
                              lpstOLEFile,
                              &(lpstOLEFile->siStreamInfo),
                              0,
                              (LPBYTE)&stOLEDirEntry,
                              sizeof(OLE_DIR_ENTRY_T));

    if (dwResult != sizeof(OLE_DIR_ENTRY_T))
        return (WORD)(dwResult);

    // Get the starting block containing
    // 64-byte subblocks

    lpstOLEFile->siBlk64Stream.dwStartBlk =
        ConvertEndianLong(stOLEDirEntry.dwStartBlk);

    lpstOLEFile->siBlk64Stream.dwStreamLen =
        ConvertEndianLong(stOLEDirEntry.dwStreamLen);

    lpstOLEFile->siBlk64Stream.dwStreamBlk = (DWORD)(-1);

    dwResult = OLESeekRead512(lpstCallBack,
                              lpstOLEFile,
                              &(lpstOLEFile->siStreamInfo),
                              wEntryNum * (DWORD)sizeof(OLE_DIR_ENTRY_T),
                              (LPBYTE)&stOLEDirEntry,
                              sizeof(OLE_DIR_ENTRY_T));

    if (dwResult != sizeof(OLE_DIR_ENTRY_T))
        return (WORD)(dwResult);

    if (stOLEDirEntry.byMSE == STGTY_STREAM)
    {
        // Bingo!

        OLEInitOpenOLEFile(lpstOLEFile,
                           &stOLEDirEntry,
                           wEntryNum,
                           lpbyStreamBATCache);

        return OLE_OK;
	}

	return OLE_ERR_NO_ENTRY;
}

#endif  // #ifndef SYM_NLM


//********************************************************************
//
// Function:
//  WORD OLEOpenStreamCB()
//
// Description:
//  Searches through the directory, calling the callback function
//  for each to determine whether the stream should be returned
//  as open.
//
//  A given name may appear multiple times in the directory,
//  so *lpwDirEntryNum is used to continue searching beginning
//  with that index in the directory.
//
//	This function should always be called first with
//  *lpwDirEntryNum = 0.
//
// Returns:
//	OLE_OK			on success
//	OLE_ERR_???		on error
//
//********************************************************************

WORD OLEOpenStreamCB
(
    LPCALLBACKREV1  lpstCallBack,           // File op callbacks
	HFILE 			hFile,					// Handle to OLE file
    LPOLE_FILE      lpstOLEFile,            // OLE file info
    LPFNOLE_OPEN_CB lpfnOpenCB,             // The callback function
    LPVOID          lpvCookie,              // Cookie
    LPBYTE          lpbyStreamBATCache,     // Non NULL if cache, 512 bytes
    LPWORD          lpwDirEntryNum          // Starting entry to search
)
{
	OLE_DIR_ENTRY_T oleDirEntries[4];
    DWORD           dwCurBlk, dwResult;
    WORD            i, wCurEntryNum;

    if (OLEOpenPreprocess(lpstCallBack,
                          hFile,
                          lpstOLEFile) != OLE_OK)
    {
        return(OLE_ERR_READ);
    }

    wCurEntryNum = (*lpwDirEntryNum / 4) * 4;
	for (dwCurBlk=wCurEntryNum/4;dwCurBlk<512;dwCurBlk++)
	{
		// Read the directory block

        dwResult = OLESeekRead512(lpstCallBack,
                                  lpstOLEFile,
                                  &(lpstOLEFile->siStreamInfo),
                                  dwCurBlk * BLK_SIZE,
                                  (LPBYTE)oleDirEntries,
                                  BLK_SIZE);

		if (dwResult != BLK_SIZE)
			return (WORD)(dwResult);

		for (i=0;i<4;i++)
		{
            if (wCurEntryNum++ < *lpwDirEntryNum)
				continue;

			if (i == 0 && dwCurBlk == 0)
			{
				// Get the starting block containing
				// 64-byte subblocks

                lpstOLEFile->siBlk64Stream.dwStartBlk =
                    ConvertEndianLong(oleDirEntries[i].dwStartBlk);

                lpstOLEFile->siBlk64Stream.dwStreamLen =
                    ConvertEndianLong(oleDirEntries[i].dwStreamLen);

                lpstOLEFile->siBlk64Stream.dwStreamBlk = (DWORD)(-1);

				continue;
			}

			// See if this directory block contains the entry
			// and verify that it is a stream object

            if (lpfnOpenCB(oleDirEntries+i,
                           wCurEntryNum - 1,
                           lpvCookie) == OLE_OPEN_CB_STATUS_RETURN)
			{
				// Bingo!

                OLEInitOpenOLEFile(lpstOLEFile,
                                   oleDirEntries+i,
                                   wCurEntryNum - 1,
                                   lpbyStreamBATCache);

                *lpwDirEntryNum = wCurEntryNum;

				return OLE_OK;
			}
		}
	}

	return OLE_ERR_NO_ENTRY;
}


//********************************************************************
//
// Function:
//  WORD OLEOpenSiblingCB()
//
// Description:
//  Uses the lpstOLEOpenSib structure for opening only siblings.
//
//  lpstOLEOpenSib should point to a properly initialized sibling
//  structure.
//
// Returns:
//	OLE_OK			on success
//	OLE_ERR_???		on error
//
//********************************************************************

WORD OLEOpenSiblingCB
(
    LPCALLBACKREV1  lpstCallBack,           // File op callbacks
	HFILE 			hFile,					// Handle to OLE file
    LPOLE_FILE      lpstOLEFile,            // OLE file info
    LPFNOLE_OPEN_CB lpfnOpenCB,             // The callback function
    LPVOID          lpvCookie,              // Cookie
    LPBYTE          lpbyStreamBATCache,     // Non NULL if cache, 512 bytes
    LPOLE_OPEN_SIB  lpstOLEOpenSib          // OLE open sibling structure
)
{
	OLE_DIR_ENTRY_T oleDirEntries[4];
    DWORD           dwCurBlk, dwResult;
    WORD            i, wCurEntryNum;

    if (OLEOpenPreprocess(lpstCallBack,
                          hFile,
                          lpstOLEFile) != OLE_OK)
    {
        return(OLE_ERR_READ);
    }

    dwCurBlk = 0xFFFFFFFF;
    while (lpstOLEOpenSib->nNumWaitingSibs > 0)
	{
        // Get a sibling from the stack

        wCurEntryNum = lpstOLEOpenSib->
            lpawSibs[--(lpstOLEOpenSib->nNumWaitingSibs)];

        // Does a new block have to be read?

        if ((wCurEntryNum >> 2) != dwCurBlk)
        {
            dwCurBlk = wCurEntryNum >> 2;

            // Read the directory block

            dwResult = OLESeekRead512(lpstCallBack,
                                      lpstOLEFile,
                                      &(lpstOLEFile->siStreamInfo),
                                      dwCurBlk * BLK_SIZE,
                                      (LPBYTE)oleDirEntries,
                                      BLK_SIZE);

            if (dwResult != BLK_SIZE)
                return (WORD)(dwResult);
        }

        i = wCurEntryNum & 3;

        // Store the siblings, pushing right, then left

        if (ConvertEndianLong(oleDirEntries[i].dwSIDRightSib) != 0xFFFFFFFF)
            lpstOLEOpenSib->lpawSibs[lpstOLEOpenSib->nNumWaitingSibs++] =
                (WORD)ConvertEndianLong(oleDirEntries[i].dwSIDRightSib);

        if (ConvertEndianLong(oleDirEntries[i].dwSIDLeftSib) != 0xFFFFFFFF)
        {
            // If not enough room, just silently ignore for now

            if (lpstOLEOpenSib->nNumWaitingSibs <
                lpstOLEOpenSib->nMaxWaitingSibs)
            {
                lpstOLEOpenSib->lpawSibs[lpstOLEOpenSib->nNumWaitingSibs++] =
                    (WORD)ConvertEndianLong(oleDirEntries[i].dwSIDLeftSib);
            }
        }

        if (lpfnOpenCB(oleDirEntries+i,
                       wCurEntryNum,
                       lpvCookie) == OLE_OPEN_CB_STATUS_RETURN)
        {
            OLEInitOpenOLEFile(lpstOLEFile,
                               oleDirEntries+i,
                               wCurEntryNum,
                               lpbyStreamBATCache);

            return OLE_OK;
		}
	}

	return OLE_ERR_NO_ENTRY;
}


#ifndef SYM_NLM

//********************************************************************
//
// Function:
//  WORD OLEFindNextStorage()
//
// Description:
//  Searches through the directory looking for the next storage
//  starting from the given entry.
//
//  The ROOT is considered a storage.
//
//  A given name may appear multiple times in the directory,
//  so *lpwDirEntryNum is used to continue searching beginning
//  with that index in the directory.
//
//	This function should always be called first with
//  *lpwDirEntryNum = 0.
//
//  If the return value is OLE_OK, *lpwDirEntryNum is set to the
//  index immediately after the found storage.
//
// Returns:
//	OLE_OK			on success
//	OLE_ERR_???		on error
//
//********************************************************************

WORD OLEFindNextStorage
(
    LPCALLBACKREV1  lpstCallBack,           // File op callbacks
	HFILE 			hFile,					// Handle to OLE file
    LPOLE_FILE      lpstOLEFile,            // OLE file info
    LPWORD          lpwStorageChild,        // Child of found storage
    LPWORD          lpwDirEntryNum          // Starting entry to search
)
{
	OLE_DIR_ENTRY_T oleDirEntries[4];
    DWORD           dwCurBlk, dwResult;
    WORD            i, wCurEntryNum;

    if (OLEOpenPreprocess(lpstCallBack,
                          hFile,
                          lpstOLEFile) != OLE_OK)
    {
        return(OLE_ERR_READ);
    }

    wCurEntryNum = (*lpwDirEntryNum / 4) * 4;
	for (dwCurBlk=wCurEntryNum/4;dwCurBlk<512;dwCurBlk++)
	{
		// Read the directory block

        dwResult = OLESeekRead512(lpstCallBack,
                                  lpstOLEFile,
                                  &(lpstOLEFile->siStreamInfo),
                                  dwCurBlk * BLK_SIZE,
                                  (LPBYTE)oleDirEntries,
                                  BLK_SIZE);

		if (dwResult != BLK_SIZE)
			return (WORD)(dwResult);

        for (i=0;i<4;i++,wCurEntryNum++)
		{
			if (i == 0 && dwCurBlk == 0)
			{
				// Get the starting block containing
				// 64-byte subblocks

                lpstOLEFile->siBlk64Stream.dwStartBlk =
                    ConvertEndianLong(oleDirEntries[i].dwStartBlk);

                lpstOLEFile->siBlk64Stream.dwStreamLen =
                    ConvertEndianLong(oleDirEntries[i].dwStreamLen);

                lpstOLEFile->siBlk64Stream.dwStreamBlk = (DWORD)(-1);
			}

            if (wCurEntryNum < *lpwDirEntryNum)
                continue;

            // See if the entry is a storage

            if (oleDirEntries[i].byMSE == STGTY_STORAGE ||
                wCurEntryNum == 0)
            {
                // Found a storage

                *lpwStorageChild =
                    (WORD)ConvertEndianLong(oleDirEntries[i].dwSIDChild);
                *lpwDirEntryNum = wCurEntryNum + 1;

                return OLE_OK;
            }
		}
	}

	return OLE_ERR_NO_ENTRY;
}


//********************************************************************
//
// Function:
//  BOOL OLEGetChildOfStorage()
//
// Description:
//  Gets the child of the storage at the given entry number.
//
//  If the return value is TRUE, *lpwChildEntryNum is set to the
//  entry number of the child.
//
// Returns:
//  TRUE            on success
//  FALSE           on error
//
//********************************************************************

BOOL OLEGetChildOfStorage
(
    LPCALLBACKREV1  lpstCallBack,           // File op callbacks
	HFILE 			hFile,					// Handle to OLE file
    LPOLE_FILE      lpstOLEFile,            // OLE file info
    WORD            wStorageEntryNum,       // The storage of the child
    LPWORD          lpwStorageChild         // Child of found storage
)
{
    OLE_DIR_ENTRY_T stStorageDirEntry;
    DWORD           dwResult;

    if (OLEOpenPreprocess(lpstCallBack,
                          hFile,
                          lpstOLEFile) != OLE_OK)
    {
        return(OLE_ERR_READ);
    }

    dwResult = OLESeekRead512(lpstCallBack,
                              lpstOLEFile,
                              &(lpstOLEFile->siStreamInfo),
                              wStorageEntryNum *
                                  (DWORD)sizeof(OLE_DIR_ENTRY_T),
                              (LPBYTE)&stStorageDirEntry,
                              sizeof(OLE_DIR_ENTRY_T));

    if (dwResult != sizeof(OLE_DIR_ENTRY_T))
        return(FALSE);

    *lpwStorageChild =
        (WORD)ConvertEndianLong(stStorageDirEntry.dwSIDChild);

    return (TRUE);
}


//********************************************************************
//
// Function:
//	WORD OLESeekWrite512()
//
// Description:
//	Seeks to dwOffset within the given stream and writes
//	wBytesToWrite bytes from the write buffer.
//
// Returns:
//	wBytesToWrite	on success
//	OLE_ERR_???		on error
//
//********************************************************************

WORD OLESeekWrite512
(
	LPCALLBACKREV1 	lpCallBack,				// File op callbacks
	LPOLE_FILE_T	pOLEFile,				// OLE file info
	LPSTREAM_INFO_T	psiStreamInfo,			// Stream info
	DWORD 			dwOffset,				// Write offset in stream
	LPBYTE 			lpbBuffer,				// Write buffer
	WORD 			wBytesToWrite			// Number of bytes to write
)
{
	DWORD 	dwBegBlk, dwEndBlk, dwCurBlk;
	WORD	wBytesWrite;
	DWORD	dwBlkFileOffset, dwSeekOffset;

	dwBegBlk = dwOffset / BLK_SIZE;
	dwEndBlk = (dwOffset + wBytesToWrite - 1) / BLK_SIZE;

	wBytesWrite = 0;

    // Write bytes in first block

	if (OLESeekStreamBlk512(lpCallBack,
		pOLEFile,psiStreamInfo,
		dwBegBlk,&dwBlkFileOffset) != dwBegBlk)
		return OLE_ERR_SEEK;

	wBytesWrite = (WORD)(BLK_SIZE - (dwOffset % BLK_SIZE));

	if (wBytesWrite > wBytesToWrite)
		wBytesWrite = wBytesToWrite;

	dwSeekOffset = dwBlkFileOffset + dwOffset % BLK_SIZE;
	if (lpCallBack->FileSeek(pOLEFile->hFile,
		dwSeekOffset,SEEK_SET) != dwSeekOffset)
	{
		// Seek failed

		return OLE_ERR_SEEK;
	}

    if (wBytesWrite > 0 &&
        lpCallBack->FileWrite(pOLEFile->hFile,
		lpbBuffer,wBytesWrite) != wBytesWrite)
	{
		// Write failed

		return OLE_ERR_WRITE;
	}

    // Return immediately if there are no more bytes to write

	if (dwBegBlk == dwEndBlk)
		return wBytesToWrite;

	lpbBuffer += wBytesWrite;

    // Now write middle blocks

	for (dwCurBlk=dwBegBlk+1;dwCurBlk<dwEndBlk;dwCurBlk++)
	{
		if (OLESeekStreamBlk512(lpCallBack,
			pOLEFile,psiStreamInfo,
			dwCurBlk,&dwBlkFileOffset) != dwCurBlk)
			return OLE_ERR_SEEK;

		if (lpCallBack->FileSeek(pOLEFile->hFile,
			dwBlkFileOffset,SEEK_SET) != dwBlkFileOffset)
		{
			// Seek failed

			return OLE_ERR_SEEK;
		}

		if (lpCallBack->FileWrite(pOLEFile->hFile,
			lpbBuffer,BLK_SIZE) != BLK_SIZE)
		{
			// Write failed

			return OLE_ERR_WRITE;
		}

		lpbBuffer += BLK_SIZE;
	}

	// Write last block

	if (OLESeekStreamBlk512(lpCallBack,
		pOLEFile,psiStreamInfo,
		dwEndBlk,&dwBlkFileOffset) != dwEndBlk)
		return OLE_ERR_SEEK;

	if (lpCallBack->FileSeek(pOLEFile->hFile,
		dwBlkFileOffset,SEEK_SET) != dwBlkFileOffset)
	{
		// Seek failed

		return OLE_ERR_SEEK;
	}

	wBytesWrite = (WORD)((dwOffset + wBytesToWrite) % BLK_SIZE);

    // Take care of boundary condition

    if (wBytesWrite == 0)
        wBytesWrite = BLK_SIZE;

    if (lpCallBack->FileWrite(pOLEFile->hFile,lpbBuffer,wBytesWrite) !=
        wBytesWrite)
    {
        // Write failed

        return OLE_ERR_WRITE;
    }

	return (wBytesToWrite);
}


//********************************************************************
//
// Function:
//	WORD OLESeekWrite64()
//
// Description:
//	Seeks to dwOffset within the given stream and writes
//	wBytesToWrites bytes from the write buffer.  This function is
//	used for streams stored within the Stream64 stream.
//
// Returns:
//	wBytesToRead	on success
//	OLE_ERR_???		on error
//
//********************************************************************

WORD OLESeekWrite64
(
	LPCALLBACKREV1 	lpCallBack,				// File op callbacks
	LPOLE_FILE_T	pOLEFile,				// OLE file info
	LPSTREAM_INFO_T	psiStreamInfo,			// Stream info
	DWORD 			dwOffset,				// Write offset in stream
	LPBYTE 			lpbBuffer,				// Write buffer
	WORD 			wBytesToWrite			// Number of bytes to write
)
{
	DWORD 	dwBegBlk, dwEndBlk, dwCurBlk;
	WORD	wBytesWrite;
	DWORD	dwBlkStreamOffset;

	dwBegBlk = dwOffset / BLK64_SIZE;
	dwEndBlk = (dwOffset + wBytesToWrite - 1) / BLK64_SIZE;

	wBytesWrite = 0;

	// Read bytes in first subblock

	if (OLESeekStreamBlk64(lpCallBack,
		pOLEFile,psiStreamInfo,
		dwBegBlk,&dwBlkStreamOffset) != dwBegBlk)
		return OLE_ERR_SEEK;

	wBytesWrite = (WORD)(BLK64_SIZE - (dwOffset % BLK64_SIZE));

	if (wBytesWrite > wBytesToWrite)
		wBytesWrite = wBytesToWrite;

	if (OLESeekWrite512(lpCallBack,
		pOLEFile,&(pOLEFile->siBlk64Stream),
		dwBlkStreamOffset + dwOffset % BLK64_SIZE,
		lpbBuffer,wBytesWrite) != wBytesWrite)
		return OLE_ERR_WRITE;

	// Return immediately if there are no more bytes to write

	if (dwBegBlk == dwEndBlk)
		return wBytesToWrite;

	lpbBuffer += wBytesWrite;

	// Now write middle blocks

	for (dwCurBlk=dwBegBlk+1;dwCurBlk<dwEndBlk;dwCurBlk++)
	{
		if (OLESeekStreamBlk64(lpCallBack,
			pOLEFile,psiStreamInfo,
			dwCurBlk,&dwBlkStreamOffset) != dwCurBlk)
			return OLE_ERR_SEEK;

		if (OLESeekWrite512(lpCallBack,
			pOLEFile,&(pOLEFile->siBlk64Stream),
			dwBlkStreamOffset,lpbBuffer,BLK64_SIZE) != BLK64_SIZE)
			return OLE_ERR_WRITE;

		lpbBuffer += BLK64_SIZE;
	}

	// Write last block

	if (OLESeekStreamBlk64(lpCallBack,
		pOLEFile,psiStreamInfo,
		dwEndBlk,&dwBlkStreamOffset) != dwEndBlk)
		return OLE_ERR_SEEK;

	wBytesWrite = (WORD)((dwOffset + wBytesToWrite) % BLK64_SIZE);

    // Take care of boundary condition

    if (wBytesWrite == 0)
        wBytesWrite = BLK64_SIZE;

	if (OLESeekWrite512(lpCallBack,
		pOLEFile,&(pOLEFile->siBlk64Stream),
		dwBlkStreamOffset,lpbBuffer,wBytesWrite) != wBytesWrite)
		return OLE_ERR_WRITE;

	return (wBytesToWrite);
}


//********************************************************************
//
// Function:
//	WORD OLESeekWrite()
//
// Description:
//	Seeks to dwOffset within the given stream and writes
//	wBytesToWrites bytes from the write buffer.
//
// Returns:
//	wBytesToRead	on success
//	OLE_ERR_???		on error
//
//********************************************************************

WORD OLESeekWrite
(
	LPCALLBACKREV1 	lpCallBack,				// File op callbacks
	LPOLE_FILE_T	pOLEFile,				// OLE file info
	DWORD 			dwOffset,				// Write offset in stream
	LPBYTE 			lpbBuffer,				// Write buffer
	WORD 			wBytesToWrite			// Number of bytes to write
)
{
	if (pOLEFile->dwBATType == BAT512)
	{
		return OLESeekWrite512(lpCallBack,
			pOLEFile,&(pOLEFile->siStreamInfo),
			dwOffset,lpbBuffer,wBytesToWrite);
	}
	else
	if (pOLEFile->dwBATType == BAT64)
	{
		return OLESeekWrite64(lpCallBack,
			pOLEFile,&(pOLEFile->siStreamInfo),
			dwOffset,lpbBuffer,wBytesToWrite);
	}

	return OLE_ERR_SEEK;
}


//********************************************************************
//
// Function:
//  BOOL OLEWriteZeroes()
//
// Description:
//  Writes a given number of zeroes to a stream beginning from
//  a given offset.
//
// Returns:
//  TRUE            On success
//  FALSE           On error
//
//********************************************************************

BOOL OLEWriteZeroes
(
    LPCALLBACKREV1  lpstCallBack,           // File op callbacks
    LPOLE_FILE      lpstOLEFile,            // OLE file info
    DWORD           dwStartOffset,          // Start offset to write
    DWORD           dwNumBytes,             // Num bytes to zero
    LPBYTE          lpbyWorkBuffer          // >= 512 bytes
)
{
    int             i;
    WORD            wNumBytesToWrite;

    // Create a buffer of zeroes

    for (i=0;i<512;i++)
        lpbyWorkBuffer[i] = 0;

    // Write the zeroes

    wNumBytesToWrite = 512;
    while (dwNumBytes != 0)
    {
        if (dwNumBytes < 512)
        {
            wNumBytesToWrite = (WORD)dwNumBytes;
        }

        if (OLESeekWrite(lpstCallBack,
                         lpstOLEFile,
                         dwStartOffset,
                         lpbyWorkBuffer,
                         wNumBytesToWrite) != wNumBytesToWrite)
        {
            // Failed to write buffer of zeroes

            return(FALSE);
        }

        dwStartOffset += wNumBytesToWrite;
        dwNumBytes -= wNumBytesToWrite;
    }

    return(TRUE);
}


//********************************************************************
//
// Function:
//  BOOL OLECopyBytes()
//
// Description:
//  Copies an array of bytes from one part of a stream to another
//  part.  The function copies in 512 byte chunks.
//
//  The function works correctly even if the arrays overlap.
//
// Returns:
//  TRUE            On success
//  FALSE           On error
//
//********************************************************************

BOOL OLECopyBytes
(
    LPCALLBACKREV1  lpstCallBack,           // File op callbacks
    LPOLE_FILE      lpstOLEFile,            // OLE file info
    DWORD           dwSrcOffset,            // Offset of source bytes
    DWORD           dwDstOffset,            // Offset of destination
    DWORD           dwNumBytes,             // Num bytes to copy
    LPBYTE          lpbyWorkBuffer          // >= 512 bytes
)
{
    BOOL            bForward;
    WORD            wNumBytesToCopy;

    if (dwSrcOffset < dwDstOffset)
    {
        // Forward copy, so copy starting from end

        bForward = TRUE;

        if (dwNumBytes > 512)
        {
            // Starting from the end

            dwSrcOffset = dwSrcOffset + dwNumBytes - 512;
            dwDstOffset = dwDstOffset + dwNumBytes - 512;
        }
    }
    else
    {
        // Backward copy, so copy starting from beginning

        bForward = FALSE;
    }

    wNumBytesToCopy = 512;
    while (dwNumBytes != 0)
    {
        if (dwNumBytes < 512)
        {
            wNumBytesToCopy = (WORD)dwNumBytes;
        }

        if (OLESeekRead(lpstCallBack,
                        lpstOLEFile,
                        dwSrcOffset,
                        lpbyWorkBuffer,
                        wNumBytesToCopy) != wNumBytesToCopy)
        {
            // Failed to read source bytes

            return(FALSE);
        }


        if (OLESeekWrite(lpstCallBack,
                         lpstOLEFile,
                         dwDstOffset,
                         lpbyWorkBuffer,
                         wNumBytesToCopy) != wNumBytesToCopy)
        {
            // Failed to write bytes to destination

            return(FALSE);
        }

        // Update source and destination offsets

        if (bForward == FALSE)
        {
            dwSrcOffset += wNumBytesToCopy;
            dwDstOffset += wNumBytesToCopy;
        }
        else
        {
            dwSrcOffset -= wNumBytesToCopy;
            dwDstOffset -= wNumBytesToCopy;
        }

        dwNumBytes -= wNumBytesToCopy;
    }

    return(TRUE);
}


#define MAX_UNLINK_DEPTH        64

#if 0

//********************************************************************
//
// Function:
//  BOOL OLEUnlinkEntry()
//
// Description:
//  Unlinks the entry by doing the following:
//      1. Replace the node with the deepest node of the subtree
//          starting from the entry to delete.
//      2. Zero out the name
//      3. Set the left and right siblings pointers to 0xFFFFFFFF
//      4. Set the child pointer to 0xFFFFFFFF
//
// Returns:
//  TRUE            On success
//  FALSE           On error
//
//********************************************************************

BOOL OLEUnlinkEntry
(
    LPCALLBACKREV1  lpstCallBack,           // File op callbacks
    LPOLE_FILE      lpstOLEFile,            // OLE file structure
    WORD            wParentStorageEntryNum, // Parent storage of stream
    WORD            wUnlinkEntryNum         // Entry to unlink
)
{
    OLE_DIR_ENTRY_T stParentStorageDirEntry;
    OLE_DIR_ENTRY_T stUnlinkDirEntry;
    WORD            wParentEntryNum;
    WORD            wDeepestLeafParentEntryNum;
    WORD            wDeepestLeafEntryNum;
    DWORD           dwResult;
    OLE_DIR_ENTRY_T stTempDirEntry;
    int             nDeepestDepth;
    int             nDepth;
    WORD            wCurParentEntryNum;
    WORD            wCurEntryNum;
    int             nNumWaitingSibs;
    WORD            awSibs[MAX_UNLINK_DEPTH];
    WORD            awParent[MAX_UNLINK_DEPTH];
    BYTE            abyDepth[MAX_UNLINK_DEPTH];
    DWORD           dwLeftSibling;
    DWORD           dwRightSibling;
    int             i;

    if (OLEOpenPreprocess(lpstCallBack,
                          lpstOLEFile->hFile,
                          lpstOLEFile) != OLE_OK)
    {
        return(FALSE);
    }

    // Get the entry of the parent storage

    dwResult = OLESeekRead512(lpstCallBack,
                              lpstOLEFile,
                              &(lpstOLEFile->siStreamInfo),
                              wParentStorageEntryNum *
                                  (DWORD)sizeof(OLE_DIR_ENTRY_T),
                              (LPBYTE)&stParentStorageDirEntry,
                              sizeof(OLE_DIR_ENTRY_T));

    if (dwResult != sizeof(OLE_DIR_ENTRY_T))
        return (FALSE);


    /////////////////////////////////////////////////////////////
    // Search through the siblings to find the following:
    //  - The parent of the entry to unlink
    /////////////////////////////////////////////////////////////

    wParentEntryNum = 0xFFFF;

    nNumWaitingSibs = 1;
    awSibs[0] = (WORD)ConvertEndianLong(stParentStorageDirEntry.dwSIDChild);
    while (nNumWaitingSibs > 0)
	{
        // Get a sibling from the stack

        wCurEntryNum = awSibs[--nNumWaitingSibs];
        if (wCurEntryNum == wUnlinkEntryNum)
        {
            // Must be the child of the storage

            break;
        }

        // Read the entry

        dwResult = OLESeekRead512(lpstCallBack,
                                  lpstOLEFile,
                                  &(lpstOLEFile->siStreamInfo),
                                  wCurEntryNum *
                                      (DWORD)sizeof(OLE_DIR_ENTRY_T),
                                  (LPBYTE)&stTempDirEntry,
                                  sizeof(OLE_DIR_ENTRY_T));

        if (dwResult != sizeof(OLE_DIR_ENTRY_T))
            return (FALSE);

        // Check the left sibling

        dwLeftSibling = ConvertEndianLong(stTempDirEntry.dwSIDLeftSib);

        if (dwLeftSibling != 0xFFFFFFFF)
        {
            // Is this the parent of the entry to unlink?

            if (dwLeftSibling == wUnlinkEntryNum)
            {
                // This is the parent of the entry to unlink

                wParentEntryNum = wCurEntryNum;
                break;
            }

            // No need to check if there is room for another
            //  sibling on the stack  because the parent was
            //  just removed.

            awSibs[nNumWaitingSibs++] = (WORD)dwLeftSibling;
        }

        // Check the right sibling

        dwRightSibling = ConvertEndianLong(stTempDirEntry.dwSIDRightSib);

        if (dwRightSibling != 0xFFFFFFFF)
        {
            // Is this the parent of the entry to unlink?

            if (dwRightSibling == wUnlinkEntryNum)
            {
                // This is the parent of the entry to unlink

                wParentEntryNum = wCurEntryNum;
                break;
            }

            if (nNumWaitingSibs >= MAX_UNLINK_DEPTH)
            {
                // Out of room to trace another sibling

                return(FALSE);
            }

            // Add the sibling to the list

            awSibs[nNumWaitingSibs++] = (WORD)dwRightSibling;
        }
    }

    /////////////////////////////////////////////////////////////
    // Search through the siblings to find the following:
    //  - The deepest leaf of the subtree with the entry
    //      to unlink as the root of the subtree
    //  - The parent of the deepest leaf
    /////////////////////////////////////////////////////////////

    nDeepestDepth = 0;
    wDeepestLeafEntryNum = wUnlinkEntryNum;
    wDeepestLeafParentEntryNum = wParentEntryNum;

    nNumWaitingSibs = 1;
    awSibs[0] = wUnlinkEntryNum;
    awParent[0] = wParentEntryNum;
    abyDepth[0] = 0;
    while (nNumWaitingSibs > 0)
	{
        // Get a sibling from the stack

        wCurEntryNum = awSibs[--nNumWaitingSibs];
        wCurParentEntryNum = awParent[nNumWaitingSibs];
        nDepth = abyDepth[nNumWaitingSibs];

        // Read the entry

        dwResult = OLESeekRead512(lpstCallBack,
                                  lpstOLEFile,
                                  &(lpstOLEFile->siStreamInfo),
                                  wCurEntryNum *
                                      (DWORD)sizeof(OLE_DIR_ENTRY_T),
                                  (LPBYTE)&stTempDirEntry,
                                  sizeof(OLE_DIR_ENTRY_T));

        if (dwResult != sizeof(OLE_DIR_ENTRY_T))
            return (FALSE);

        // Check the left sibling

        dwLeftSibling = ConvertEndianLong(stTempDirEntry.dwSIDLeftSib);

        if (dwLeftSibling != 0xFFFFFFFF)
        {
            // No need to check if there is room for another
            //  sibling on the stack  because the parent was
            //  just removed.

            awSibs[nNumWaitingSibs] = (WORD)dwLeftSibling;
            awParent[nNumWaitingSibs] = wCurEntryNum;
            abyDepth[nNumWaitingSibs++] = (BYTE)(nDepth + 1);
        }

        // Check the right sibling

        dwRightSibling = ConvertEndianLong(stTempDirEntry.dwSIDRightSib);

        if (dwRightSibling != 0xFFFFFFFF)
        {
            if (nNumWaitingSibs >= MAX_UNLINK_DEPTH)
            {
                // Out of room to trace another sibling

                return(FALSE);
            }

            // Add the sibling to the list

            awSibs[nNumWaitingSibs] = (WORD)dwRightSibling;
            awParent[nNumWaitingSibs] = wCurEntryNum;
            abyDepth[nNumWaitingSibs++] = (BYTE)(nDepth + 1);
        }

        // Is it a leaf node?  It is if it has no siblings.

        if (dwLeftSibling == 0xFFFFFFFF && dwRightSibling == 0xFFFFFFFF)
        {
            // Update the deepest node if either the depth
            //  is greater than the deepest depth so far
            //  or if the current entry is the entry to unlink
            //  and its depth is the same as the deepest depth
            //  so far.  This is so that if the entry to unlink
            //  is at least as deep as the deepest entry so
            //  far, we just unlink it without dealing with any
            //  other link

            if (nDepth > nDeepestDepth)
            {
                nDeepestDepth = nDepth;
                wDeepestLeafEntryNum = wCurEntryNum;
                wDeepestLeafParentEntryNum = wCurParentEntryNum;
            }
        }
    }

    /////////////////////////////////////////////////////////////
    // The general steps are:
    //  1. Unlink the deepest leaf from its parent
    //  2. Set the deepest leaf's sibling pointers to point
    //      to the siblings of the node to unlink
    //  3. Set the parent of the node to unlink to point to the
    //     deepest leaf
    /////////////////////////////////////////////////////////////

    if (wDeepestLeafParentEntryNum == 0xFFFF)
    {
        // No parent, meaning that the deepest leaf is the
        //  leaf to delete, so just unlink from the parent storage

        stParentStorageDirEntry.dwSIDChild = 0xFFFFFFFF;

        dwResult = OLESeekWrite512(lpstCallBack,
                                   lpstOLEFile,
                                   &(lpstOLEFile->siStreamInfo),
                                   wParentStorageEntryNum *
                                       (DWORD)sizeof(OLE_DIR_ENTRY_T),
                                   (LPBYTE)&stParentStorageDirEntry,
                                   sizeof(OLE_DIR_ENTRY_T));

        if (dwResult != sizeof(OLE_DIR_ENTRY_T))
            return (FALSE);
    }
    else
    {
        // Unlink the deepest leaf from its parent

        dwResult = OLESeekRead512(lpstCallBack,
                                  lpstOLEFile,
                                  &(lpstOLEFile->siStreamInfo),
                                  wDeepestLeafParentEntryNum *
                                      (DWORD)sizeof(OLE_DIR_ENTRY_T),
                                  (LPBYTE)&stTempDirEntry,
                                  sizeof(OLE_DIR_ENTRY_T));

        if (dwResult != sizeof(OLE_DIR_ENTRY_T))
            return (FALSE);

        if (ConvertEndianLong(stTempDirEntry.dwSIDLeftSib) ==
            wDeepestLeafEntryNum)
        {
            stTempDirEntry.dwSIDLeftSib = 0xFFFFFFFF;
        }
        else
        if (ConvertEndianLong(stTempDirEntry.dwSIDRightSib) ==
            wDeepestLeafEntryNum)
        {
            stTempDirEntry.dwSIDRightSib = 0xFFFFFFFF;
        }
        else
        {
            // Fatal error

            return(FALSE);
        }

        // Update the deepest leaf's parent

        dwResult = OLESeekWrite512(lpstCallBack,
                                   lpstOLEFile,
                                   &(lpstOLEFile->siStreamInfo),
                                   wDeepestLeafParentEntryNum *
                                       (DWORD)sizeof(OLE_DIR_ENTRY_T),
                                   (LPBYTE)&stTempDirEntry,
                                   sizeof(OLE_DIR_ENTRY_T));

        if (dwResult != sizeof(OLE_DIR_ENTRY_T))
            return (FALSE);

        // Check to see if the deepest leaf is the same as the
        //  node to unlink

        if (wDeepestLeafEntryNum != wUnlinkEntryNum)
        {
            // The two are not the same, so it makes sense
            //  to continue with the following

            // Set the deepest leaf's sibling pointers to the
            //  same values as the node to unlink's

            dwResult = OLESeekRead512(lpstCallBack,
                                      lpstOLEFile,
                                      &(lpstOLEFile->siStreamInfo),
                                      wDeepestLeafEntryNum *
                                          (DWORD)sizeof(OLE_DIR_ENTRY_T),
                                      (LPBYTE)&stTempDirEntry,
                                      sizeof(OLE_DIR_ENTRY_T));

            if (dwResult != sizeof(OLE_DIR_ENTRY_T))
                return (FALSE);

            // Get the entry of the node to unlink

            dwResult = OLESeekRead512(lpstCallBack,
                                      lpstOLEFile,
                                      &(lpstOLEFile->siStreamInfo),
                                      wUnlinkEntryNum *
                                          (DWORD)sizeof(OLE_DIR_ENTRY_T),
                                      (LPBYTE)&stUnlinkDirEntry,
                                      sizeof(OLE_DIR_ENTRY_T));

            if (dwResult != sizeof(OLE_DIR_ENTRY_T))
                return (FALSE);

            stTempDirEntry.dwSIDLeftSib = stUnlinkDirEntry.dwSIDLeftSib;
            stTempDirEntry.dwSIDRightSib = stUnlinkDirEntry.dwSIDRightSib;

            dwResult = OLESeekWrite512(lpstCallBack,
                                       lpstOLEFile,
                                       &(lpstOLEFile->siStreamInfo),
                                       wDeepestLeafEntryNum *
                                           (DWORD)sizeof(OLE_DIR_ENTRY_T),
                                       (LPBYTE)&stTempDirEntry,
                                       sizeof(OLE_DIR_ENTRY_T));

            if (dwResult != sizeof(OLE_DIR_ENTRY_T))
                return (FALSE);

            // Now set the parent of the node to unlink to point
            //  to this deepest leaf

            if (wParentEntryNum == 0xFFFF)
            {
                // No parent, meaning that the parent of the node
                //  to unlink is the parent storage

                stParentStorageDirEntry.dwSIDChild =
                    ConvertEndianLong(wDeepestLeafEntryNum);

                dwResult = OLESeekWrite512(lpstCallBack,
                                           lpstOLEFile,
                                           &(lpstOLEFile->siStreamInfo),
                                           wParentStorageEntryNum *
                                               (DWORD)sizeof(OLE_DIR_ENTRY_T),
                                           (LPBYTE)&stParentStorageDirEntry,
                                           sizeof(OLE_DIR_ENTRY_T));

                if (dwResult != sizeof(OLE_DIR_ENTRY_T))
                    return (FALSE);
            }
            else
            {
                // Determine whether it is the left or right sibling
                //  of the parent of the node to unlink that needs
                //  to change

                dwResult = OLESeekRead512(lpstCallBack,
                                          lpstOLEFile,
                                          &(lpstOLEFile->siStreamInfo),
                                          wParentEntryNum *
                                              (DWORD)sizeof(OLE_DIR_ENTRY_T),
                                          (LPBYTE)&stTempDirEntry,
                                          sizeof(OLE_DIR_ENTRY_T));

                if (dwResult != sizeof(OLE_DIR_ENTRY_T))
                    return (FALSE);

                if (ConvertEndianLong(stTempDirEntry.dwSIDLeftSib) ==
                    wUnlinkEntryNum)
                {
                    // It is the left sibling

                    stTempDirEntry.dwSIDLeftSib = wDeepestLeafEntryNum;
                    stTempDirEntry.dwSIDLeftSib =
                        ConvertEndianLong(stTempDirEntry.dwSIDLeftSib);
                }
                else
                {
                    // It must be the right sibling

                    stTempDirEntry.dwSIDRightSib = wDeepestLeafEntryNum;
                    stTempDirEntry.dwSIDRightSib =
                        ConvertEndianLong(stTempDirEntry.dwSIDRightSib);
                }

                dwResult = OLESeekWrite512(lpstCallBack,
                                           lpstOLEFile,
                                           &(lpstOLEFile->siStreamInfo),
                                           wParentEntryNum *
                                               (DWORD)sizeof(OLE_DIR_ENTRY_T),
                                           (LPBYTE)&stTempDirEntry,
                                           sizeof(OLE_DIR_ENTRY_T));

                if (dwResult != sizeof(OLE_DIR_ENTRY_T))
                    return (FALSE);
            }
        }
    }

    // Get the entry of the node to unlink

    dwResult = OLESeekRead512(lpstCallBack,
                              lpstOLEFile,
                              &(lpstOLEFile->siStreamInfo),
                              wUnlinkEntryNum *
                                  (DWORD)sizeof(OLE_DIR_ENTRY_T),
                              (LPBYTE)&stUnlinkDirEntry,
                              sizeof(OLE_DIR_ENTRY_T));

    if (dwResult != sizeof(OLE_DIR_ENTRY_T))
        return (FALSE);

    // Now zero out the name and invalidate the sibling and child
    //  pointers

    for (i=0;i<MAX_OLE_NAME_LEN;i++)
        stUnlinkDirEntry.uszName[i] = 0;

    stUnlinkDirEntry.dwSIDLeftSib = 0xFFFFFFFF;
    stUnlinkDirEntry.dwSIDRightSib = 0xFFFFFFFF;
    stUnlinkDirEntry.dwSIDChild = 0xFFFFFFFF;

    // Invalidate the type

    stUnlinkDirEntry.byMSE = 0;

    // Update the node to unlink

    dwResult = OLESeekWrite512(lpstCallBack,
                               lpstOLEFile,
                               &(lpstOLEFile->siStreamInfo),
                               wUnlinkEntryNum *
                                   (DWORD)sizeof(OLE_DIR_ENTRY_T),
                               (LPBYTE)&stUnlinkDirEntry,
                               sizeof(OLE_DIR_ENTRY_T));

    if (dwResult != sizeof(OLE_DIR_ENTRY_T))
        return (FALSE);

    // Success

    return(TRUE);
}

#endif

//********************************************************************
//
// Function:
//  BOOL OLEUnlinkEntry()
//
// Description:
//  Unlinks the entry by doing the following:
//      1. If the entry does not have any siblings, just delete
//         the entry and set its parent pointer to 0xFFFFFFFF.
//      2. If the entry has only one sibling, replace the
//         entry with the sibling entry and update the parent
//         pointer accordingly.
//      3. Otherwise, replace the entry with the leftmost child
//         of the right sibling.
//      4. Set the left and right siblings pointers to 0xFFFFFFFF
//      5. Set the child pointer to 0xFFFFFFFF
//      6. Zero out the stream name
//
// Returns:
//  TRUE            On success
//  FALSE           On error
//
//********************************************************************

BOOL OLEUnlinkEntry
(
    LPCALLBACKREV1  lpstCallBack,           // File op callbacks
    LPOLE_FILE      lpstOLEFile,            // OLE file structure
    WORD            wParentStorageEntryNum, // Parent storage of stream
    WORD            wUnlinkEntryNum         // Entry to unlink
)
{
    OLE_DIR_ENTRY_T stParentStorageDirEntry;
    OLE_DIR_ENTRY_T stUnlinkDirEntry;
    OLE_DIR_ENTRY_T stReplacementDirEntry;
    OLE_DIR_ENTRY_T stTempDirEntry;
    DWORD           dwReplacementEntryNum;
    DWORD           dwReplacementParentEntryNum;
    DWORD           dwParentEntryNum;
    DWORD           dwResult;
    WORD            wCurEntryNum;
    int             nNumWaitingSibs;
    WORD            awSibs[MAX_UNLINK_DEPTH];
    DWORD           dwLeftSibling;
    DWORD           dwRightSibling;
    int             i;

    if (OLEOpenPreprocess(lpstCallBack,
                          lpstOLEFile->hFile,
                          lpstOLEFile) != OLE_OK)
    {
        return(FALSE);
    }

    // Get the entry of the parent storage

    dwResult = OLESeekRead512(lpstCallBack,
                              lpstOLEFile,
                              &(lpstOLEFile->siStreamInfo),
                              wParentStorageEntryNum *
                                  (DWORD)sizeof(OLE_DIR_ENTRY_T),
                              (LPBYTE)&stParentStorageDirEntry,
                              sizeof(OLE_DIR_ENTRY_T));

    if (dwResult != sizeof(OLE_DIR_ENTRY_T))
        return (FALSE);

    // Get the entry of the node to unlink

    dwResult = OLESeekRead512(lpstCallBack,
                              lpstOLEFile,
                              &(lpstOLEFile->siStreamInfo),
                              wUnlinkEntryNum *
                                  (DWORD)sizeof(OLE_DIR_ENTRY_T),
                              (LPBYTE)&stUnlinkDirEntry,
                              sizeof(OLE_DIR_ENTRY_T));

    if (dwResult != sizeof(OLE_DIR_ENTRY_T))
        return (FALSE);


    /////////////////////////////////////////////////////////////
    // Search through the siblings to find the following:
    //  - The parent of the entry to unlink
    /////////////////////////////////////////////////////////////

    dwParentEntryNum = 0xFFFF;

    nNumWaitingSibs = 1;
    awSibs[0] = (WORD)ConvertEndianLong(stParentStorageDirEntry.dwSIDChild);
    while (nNumWaitingSibs > 0)
	{
        // Get a sibling from the stack

        wCurEntryNum = awSibs[--nNumWaitingSibs];
        if (wCurEntryNum == wUnlinkEntryNum)
        {
            // Must be the child of the storage

            break;
        }

        // Read the entry

        dwResult = OLESeekRead512(lpstCallBack,
                                  lpstOLEFile,
                                  &(lpstOLEFile->siStreamInfo),
                                  wCurEntryNum *
                                      (DWORD)sizeof(OLE_DIR_ENTRY_T),
                                  (LPBYTE)&stTempDirEntry,
                                  sizeof(OLE_DIR_ENTRY_T));

        if (dwResult != sizeof(OLE_DIR_ENTRY_T))
            return (FALSE);

        // Check the left sibling

        dwLeftSibling = ConvertEndianLong(stTempDirEntry.dwSIDLeftSib);

        if (dwLeftSibling != 0xFFFFFFFF)
        {
            // Is this the parent of the entry to unlink?

            if (dwLeftSibling == wUnlinkEntryNum)
            {
                // This is the parent of the entry to unlink

                dwParentEntryNum = wCurEntryNum;
                break;
            }

            // No need to check if there is room for another
            //  sibling on the stack  because the parent was
            //  just removed.

            awSibs[nNumWaitingSibs++] = (WORD)dwLeftSibling;
        }

        // Check the right sibling

        dwRightSibling = ConvertEndianLong(stTempDirEntry.dwSIDRightSib);

        if (dwRightSibling != 0xFFFFFFFF)
        {
            // Is this the parent of the entry to unlink?

            if (dwRightSibling == wUnlinkEntryNum)
            {
                // This is the parent of the entry to unlink

                dwParentEntryNum = wCurEntryNum;
                break;
            }

            if (nNumWaitingSibs >= MAX_UNLINK_DEPTH)
            {
                // Out of room to trace another sibling

                return(FALSE);
            }

            // Add the sibling to the list

            awSibs[nNumWaitingSibs++] = (WORD)dwRightSibling;
        }
    }


    /////////////////////////////////////////////////////////////
    // Find the replacement entry
    /////////////////////////////////////////////////////////////

    // Get the sibling entries

    dwLeftSibling = ConvertEndianLong(stUnlinkDirEntry.dwSIDLeftSib);
    dwRightSibling = ConvertEndianLong(stUnlinkDirEntry.dwSIDRightSib);

    if (dwLeftSibling != 0xFFFFFFFF && dwRightSibling != 0xFFFFFFFF)
    {
        // Replace with the leftmost child of the right sibling

        // Prevent an infinite loop

        dwReplacementParentEntryNum = wUnlinkEntryNum;
        dwReplacementEntryNum = dwRightSibling;
        for (i=0;i<4096;i++)
        {
            dwResult = OLESeekRead512(lpstCallBack,
                                      lpstOLEFile,
                                      &(lpstOLEFile->siStreamInfo),
                                      dwReplacementEntryNum *
                                          sizeof(OLE_DIR_ENTRY_T),
                                      (LPBYTE)&stReplacementDirEntry,
                                      sizeof(OLE_DIR_ENTRY_T));

            if (dwResult != sizeof(OLE_DIR_ENTRY_T))
                return (FALSE);

            dwLeftSibling =
                ConvertEndianLong(stReplacementDirEntry.dwSIDLeftSib);

            if (dwLeftSibling == 0xFFFFFFFF)
            {
                // Found the left most one

                break;
            }

            dwReplacementParentEntryNum = dwReplacementEntryNum;
            dwReplacementEntryNum = dwLeftSibling;
        }

        if (i == 4096)
        {
            // Deeper than 4096 levels!?

            return(FALSE);
        }

        // Remove the leftmost replacement entry from the parent
        //  if the parent is not the entry to unlink

        if (dwReplacementParentEntryNum != (DWORD)wUnlinkEntryNum)
        {
            dwResult = OLESeekRead512(lpstCallBack,
                                      lpstOLEFile,
                                      &(lpstOLEFile->siStreamInfo),
                                      dwReplacementParentEntryNum *
                                          sizeof(OLE_DIR_ENTRY_T),
                                      (LPBYTE)&stTempDirEntry,
                                      sizeof(OLE_DIR_ENTRY_T));

            if (dwResult != sizeof(OLE_DIR_ENTRY_T))
                return(FALSE);

            // Set the parent's left sibling to point to the
            //  replacement entry's right sibling

            stTempDirEntry.dwSIDLeftSib =
                ConvertEndianLong(stReplacementDirEntry.dwSIDRightSib);

            // Write out the updated parent of the leaf

            dwResult = OLESeekWrite512(lpstCallBack,
                                       lpstOLEFile,
                                       &(lpstOLEFile->siStreamInfo),
                                       dwReplacementParentEntryNum *
                                           sizeof(OLE_DIR_ENTRY_T),
                                       (LPBYTE)&stTempDirEntry,
                                       sizeof(OLE_DIR_ENTRY_T));

            if (dwResult != sizeof(OLE_DIR_ENTRY_T))
                return(FALSE);

            // Set the replacement entry's right sibling to point
            //  to the entry to unlink's right sibling

            stReplacementDirEntry.dwSIDRightSib =
                stUnlinkDirEntry.dwSIDRightSib;
        }

        // Set the replacement entry's left sibling to point
        //  to the unlink entry's left sibling

        stReplacementDirEntry.dwSIDLeftSib = stUnlinkDirEntry.dwSIDLeftSib;

        // Write the updated entry

        dwResult = OLESeekWrite512(lpstCallBack,
                                   lpstOLEFile,
                                   &(lpstOLEFile->siStreamInfo),
                                   dwReplacementEntryNum *
                                       sizeof(OLE_DIR_ENTRY_T),
                                   (LPBYTE)&stReplacementDirEntry,
                                   sizeof(OLE_DIR_ENTRY_T));

        if (dwResult != sizeof(OLE_DIR_ENTRY_T))
            return (FALSE);
    }
    else
    {
        if (dwLeftSibling != 0xFFFFFFFF)
        {
            // Replace with the left sibling

            dwReplacementEntryNum = dwLeftSibling;
        }
        else
        if (dwRightSibling != 0xFFFFFFFF)
        {
            // Replace with the right sibling

            dwReplacementEntryNum = dwRightSibling;
        }
        else
        {
            // Just delete the entry

            dwReplacementEntryNum = 0xFFFFFFFF;
        }
    }

    /////////////////////////////////////////////////////////////
    // Update the parent of the entry to unlink
    /////////////////////////////////////////////////////////////

    if (dwParentEntryNum == 0xFFFF)
    {
        // No parent, meaning that the parent of the node
        //  to unlink is the parent storage

        stParentStorageDirEntry.dwSIDChild =
            ConvertEndianLong(dwReplacementEntryNum);

        dwResult = OLESeekWrite512(lpstCallBack,
                                   lpstOLEFile,
                                   &(lpstOLEFile->siStreamInfo),
                                   wParentStorageEntryNum *
                                       (DWORD)sizeof(OLE_DIR_ENTRY_T),
                                   (LPBYTE)&stParentStorageDirEntry,
                                   sizeof(OLE_DIR_ENTRY_T));

        if (dwResult != sizeof(OLE_DIR_ENTRY_T))
            return (FALSE);
    }
    else
    {
        // Determine whether it is the left or right sibling
        //  of the parent of the node to unlink that needs
        //  to change

        dwResult = OLESeekRead512(lpstCallBack,
                                  lpstOLEFile,
                                  &(lpstOLEFile->siStreamInfo),
                                  dwParentEntryNum *
                                      (DWORD)sizeof(OLE_DIR_ENTRY_T),
                                  (LPBYTE)&stTempDirEntry,
                                  sizeof(OLE_DIR_ENTRY_T));

        if (dwResult != sizeof(OLE_DIR_ENTRY_T))
            return (FALSE);

        dwLeftSibling = ConvertEndianLong(stTempDirEntry.dwSIDLeftSib);
        if (dwLeftSibling == wUnlinkEntryNum)
        {
            // It is the left sibling

            stTempDirEntry.dwSIDLeftSib =
                ConvertEndianLong(dwReplacementEntryNum);
        }
        else
        {
            // It must be the right sibling

            stTempDirEntry.dwSIDRightSib =
                ConvertEndianLong(dwReplacementEntryNum);
        }

        // Write out the new parent of the entry to unlink

        dwResult = OLESeekWrite512(lpstCallBack,
                                   lpstOLEFile,
                                   &(lpstOLEFile->siStreamInfo),
                                   dwParentEntryNum *
                                       (DWORD)sizeof(OLE_DIR_ENTRY_T),
                                   (LPBYTE)&stTempDirEntry,
                                   sizeof(OLE_DIR_ENTRY_T));

        if (dwResult != sizeof(OLE_DIR_ENTRY_T))
            return (FALSE);
    }

    // The entry to unlink should still be valid at this point

    // Now zero out the name and invalidate the sibling and child
    //  pointers

    for (i=0;i<MAX_OLE_NAME_LEN;i++)
        stUnlinkDirEntry.uszName[i] = 0;

    stUnlinkDirEntry.dwSIDLeftSib = 0xFFFFFFFF;
    stUnlinkDirEntry.dwSIDRightSib = 0xFFFFFFFF;
    stUnlinkDirEntry.dwSIDChild = 0xFFFFFFFF;

    // Invalidate the type

    stUnlinkDirEntry.byMSE = 0;

    // Update the node to unlink

    dwResult = OLESeekWrite512(lpstCallBack,
                               lpstOLEFile,
                               &(lpstOLEFile->siStreamInfo),
                               wUnlinkEntryNum *
                                   (DWORD)sizeof(OLE_DIR_ENTRY_T),
                               (LPBYTE)&stUnlinkDirEntry,
                               sizeof(OLE_DIR_ENTRY_T));

    if (dwResult != sizeof(OLE_DIR_ENTRY_T))
        return (FALSE);

    // Success

    return(TRUE);
}


//********************************************************************
//
// Function:
//  BOOL OLESetStreamLen()
//
// Description:
//  Modifies the length field of the directory entry for the
//  given stream number to the given length.
//
// Returns:
//  TRUE            On success
//  FALSE           On error
//
//********************************************************************

BOOL OLESetStreamLen
(
    LPCALLBACKREV1  lpstCallBack,           // File op callbacks
    LPOLE_FILE      lpstOLEFile,            // OLE file structure
    DWORD           dwEntryNum,             // Entry to change
    DWORD           dwNewLen                // New stream length
)
{
    DWORD           dwResult;
    OLE_DIR_ENTRY_T stTempDirEntry;

    if (OLEOpenPreprocess(lpstCallBack,
                          lpstOLEFile->hFile,
                          lpstOLEFile) != OLE_OK)
    {
        return(FALSE);
    }

    // Get the entry to change

    dwResult = OLESeekRead512(lpstCallBack,
                              lpstOLEFile,
                              &(lpstOLEFile->siStreamInfo),
                              dwEntryNum *
                                  (DWORD)sizeof(OLE_DIR_ENTRY_T),
                              (LPBYTE)&stTempDirEntry,
                              sizeof(OLE_DIR_ENTRY_T));

    if (dwResult != sizeof(OLE_DIR_ENTRY_T))
        return (FALSE);

    // Set the new length

    stTempDirEntry.dwStreamLen = ConvertEndianLong(dwNewLen);

    // Write the modified entry back out

    dwResult = OLESeekWrite512(lpstCallBack,
                               lpstOLEFile,
                               &(lpstOLEFile->siStreamInfo),
                               dwEntryNum *
                                   (DWORD)sizeof(OLE_DIR_ENTRY_T),
                               (LPBYTE)&stTempDirEntry,
                               sizeof(OLE_DIR_ENTRY_T));

    if (dwResult != sizeof(OLE_DIR_ENTRY_T))
        return (FALSE);

    // Success

    return(TRUE);
}

#endif  // #ifndef SYM_NLM



