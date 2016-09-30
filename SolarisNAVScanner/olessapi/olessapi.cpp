//************************************************************************
//
// $Header:   S:/OLESSAPI/VCS/olessapi.cpv   1.28   08 Sep 1998 12:54:04   DCHI  $
//
// Description:
//  Core OLE structured storage access implementation.
//
//************************************************************************
// $Log:   S:/OLESSAPI/VCS/olessapi.cpv  $
// 
//    Rev 1.28   08 Sep 1998 12:54:04   DCHI
// Fixed copy forward problem in SSCopyBytes().
// 
//    Rev 1.27   24 Jul 1998 16:34:58   DCHI
// Added implementation of SSGetStreamIndexCB().
// 
//    Rev 1.26   16 Jul 1998 10:55:20   DCHI
// Added SSLimitStreamLen() to limit stream lengths to size of file.
// 
//    Rev 1.25   24 Jun 1998 11:09:34   DCHI
// Added implementations of SSEndianDWORD() and SSEndianWORD().
// 
//    Rev 1.24   26 Mar 1998 11:59:50   DCHI
// Correction to SSGetNamedSiblingID().
// 
//    Rev 1.23   26 Mar 1998 11:21:38   DCHI
// Added implementation of SSGetNamedSiblingID().
// 
//    Rev 1.22   03 Mar 1998 12:22:26   DCHI
// Modified SSToUpper() and SSToLower().
// 
//    Rev 1.21   04 Dec 1997 12:56:20   DCHI
// Limited number of iterations in SSEnumDirEntriesCB() and
// SSEnumSiblingEntriesCB() to 16K.
// 
//    Rev 1.20   24 Nov 1997 17:29:52   DCHI
// Added SSGetParentStorage() and supporting callback function.
// 
//    Rev 1.19   20 Nov 1997 11:41:56   DCHI
// Made cache alignment consistent.
// 
//    Rev 1.18   28 Oct 1997 18:21:48   DCHI
// Fixed SSWStrNCmp() to properly decrement wMaxLen.
// 
//    Rev 1.17   14 Oct 1997 17:34:14   DCHI
// Added SSToLower() and SSToUpper() implementations.
// 
//    Rev 1.16   04 Sep 1997 11:09:24   DCHI
// Added check for invalid link to follow in SSSeekStreamBlock().
// 
//    Rev 1.15   22 Aug 1997 17:47:46   DCHI
// Reverted changes back to 1.13.
// 
//    Rev 1.14   22 Aug 1997 14:10:32   DDREW
// Temporarly removed SYM_NLM progress callback
// 
//    Rev 1.13   11 Aug 1997 14:30:02   DCHI
// Change so that open CB can take NULL stream ptr if open is not desired.
// 
//    Rev 1.12   08 Aug 1997 21:08:06   DCHI
// Corrected buffer overrun in AddSector().
// 
//    Rev 1.11   18 Jul 1997 15:00:36   DCHI
// In addition to check for ENDOFCHAIN, added check for FREESECT
// in SSSeekStreamBlock().
// 
//    Rev 1.10   25 Jun 1997 17:04:54   DCHI
// Added call to SSProgress() in SSSeekStreamBlock() for SYM_NLM.
// 
//    Rev 1.9   11 Jun 1997 14:05:48   DCHI
// Took care of unreferenced parameter in SSOpenStreamByNameCB().
// 
//    Rev 1.8   11 Jun 1997 13:58:58   DCHI
// Added SSOpenStreamByNameCB().
// 
//    Rev 1.7   09 Jun 1997 14:08:48   DCHI
// Reworked pack pragmas to work on UNIX gcc which doesn't replace tokens
// within pragmas.
// 
//    Rev 1.6   30 May 1997 16:29:16   DCHI
// Added handling of SS_ENUM_CB_STATUS_ERROR in enumeration functions.
// 
//    Rev 1.5   27 May 1997 14:14:38   DCHI
// Corrected #define of PACDIF_MAX_DIF from 255 to 254 and corrected
// comparison usage of PACDIF_MAX_DIF due to change from 255 to 254.
// 
//    Rev 1.4   07 May 1997 13:30:28   DCHI
// Corrected final calculation of dwOffset in SeekStream() for a mini-sector
// stream.
// 
//    Rev 1.3   06 May 1997 12:26:18   DCHI
// Removed unreferenced local variables.
// 
//    Rev 1.2   28 Apr 1997 18:33:00   DCHI
// Added seek to offset 0 before reading hearder in CreateRoot().
// 
//    Rev 1.1   25 Apr 1997 12:34:42   DCHI
// Added SSWStrNCmp().
// 
//    Rev 1.0   23 Apr 1997 15:10:06   DCHI
// Initial revision.
// 
//************************************************************************

#include "olessapi.h"

//////////////////////////////////////////////////////////////////////
// BAT cache constants and structures
//////////////////////////////////////////////////////////////////////

#define MAX_PACKETS         84
#define MAX_SEC_PER_PAC     0xFFFFu

#define MAX_SEC_PER_SUBPAC_ABS      0xFFFFu
#define MAX_SEC_PER_SUBPAC_DIF      0xFFu

#define MAX_SUBPACS         255

#define MAX_SUBPAC_SIZE     8

#define PACTYPE_ABS         255
#define PACTYPE_FIRST_DIF   0

#define PACDIF_MAX_DIF      254

#if defined(SYM_WIN32) || defined(SYM_NTK) || defined(SYM_VXD)
#include "pshpack1.h"
#elif defined(__MACINTOSH__)
#pragma options align=packed
#else
#pragma pack(1)
#endif

typedef struct
{
    DWORD       dwStartSector;
    WORD        wCount;
} SUBPAC_ABS_T, FAR *LPSUBPAC_ABS_T;

typedef struct
{
    BYTE            byFlags;
    BYTE            byCountSubpacs;

    // Pad to next DWORD boundary

    BYTE            byPadding0;
    BYTE            byPadding1;
} PACKET_HDR_T, FAR *LPPACKET_HDR_T;

typedef struct
{
    SUBPAC_ABS_T    subpacs[MAX_SUBPACS];
} PACKET_ABS_T, FAR *LPPACKET_ABS_T;

typedef struct
{
    DWORD           dwStartSector;
    BYTE            byCounts[MAX_SUBPACS];
} PACKET_DIF_T, FAR *LPPACKET_DIF_T;

typedef struct
{
    DWORD       dwCountBlocksCached;
    WORD        wCountPackets;
    WORD        wLastPacketOffset;
    DWORD       dwCacheSize;
} LAT_CACHE_HDR_T, FAR *LPLAT_CACHE_HDR_T;

#if defined(SYM_WIN32) || defined(SYM_NTK) || defined(SYM_VXD)
#include "poppack.h"
#elif defined(__MACINTOSH__)
#pragma options align=reset
#else
#pragma pack()
#endif


//********************************************************************
//
// Function:
//  BOOL SSCreateNDIF()
//
// Parameters:
//  lpstCacheHdr        Pointer to cache header
//  lpstPacketHdr       Pointer to packet dest
//  byN                 N dif between subpackets
//  dwStartSector       Starting sector of packet
//  byCount0            Count for subpacket #0
//  byCount1            Count for subpacket #1
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
//  FALSE   If the CACHE has no more room for the packet
//  TRUE    On successful creation of the packet
//
//********************************************************************

BOOL SSCreateNDIF
(
    LPLAT_CACHE_HDR_T   lpstCacheHdr,
    LPPACKET_HDR_T      lpstPacketHdr,
    BYTE                byN,
    DWORD               dwStartSector,
    BYTE                byCount0,
    BYTE                byCount1
)
{
    WORD                wLastPacketOffset;
    LPPACKET_DIF_T      lpstPacketDif;

    wLastPacketOffset = (LPBYTE)lpstPacketHdr - (LPBYTE)lpstCacheHdr;

    lpstPacketDif = (LPPACKET_DIF_T)
        ((LPBYTE)lpstPacketHdr + sizeof(PACKET_HDR_T));

    if (byCount1 == 0)
    {
        if ((wLastPacketOffset + sizeof(PACKET_HDR_T) +
            sizeof(DWORD) + sizeof(BYTE)) > lpstCacheHdr->dwCacheSize)
            return(FALSE);

        lpstPacketHdr->byCountSubpacs = 1;
    }
    else
    {
        if ((wLastPacketOffset + sizeof(PACKET_HDR_T) +
            sizeof(DWORD) + 2 * sizeof(BYTE)) > lpstCacheHdr->dwCacheSize)
            return(FALSE);

        lpstPacketHdr->byCountSubpacs = 2;
        lpstPacketDif->byCounts[1] = byCount1;
    }

    lpstCacheHdr->wCountPackets++;

    lpstCacheHdr->wLastPacketOffset = wLastPacketOffset;
    lpstCacheHdr->dwCountBlocksCached++;;
    lpstPacketHdr->byFlags = byN;

    lpstPacketDif->dwStartSector = dwStartSector;
    lpstPacketDif->byCounts[0] = byCount0;

    return(TRUE);
}


//********************************************************************
//
// Function:
//  BOOL SSInitMacroPacket()
//
// Parameters:
//  lpbyCache           Pointer to cache memory
//  dwCacheSize         Size of BAT cache memory
//  dwStartSector       First sector of stream
//
// Description:
//  Initializes a stream BAT cache by creating the cache header
//  and an NDIF packet with one subpacket containing the first sector
//  of the stream.
//
// Returns:
//  FALSE   On error
//  TRUE    On successful intiailization of the cache
//
//********************************************************************

BOOL SSInitMacroPacket
(
    LPBYTE              lpbyCache,
    DWORD               dwCacheSize,
    DWORD               dwStartSector
)
{
    ((LPLAT_CACHE_HDR_T)lpbyCache)->dwCountBlocksCached = 0;
    ((LPLAT_CACHE_HDR_T)lpbyCache)->wCountPackets = 0;
    ((LPLAT_CACHE_HDR_T)lpbyCache)->dwCacheSize = dwCacheSize;

    return SSCreateNDIF((LPLAT_CACHE_HDR_T)lpbyCache,
                        (LPPACKET_HDR_T)(lpbyCache + sizeof(LAT_CACHE_HDR_T)),
                        PACTYPE_FIRST_DIF,
                        dwStartSector,
                        1,
                        0);
}


//********************************************************************
//
// Function:
//  BOOL SSAddSector()
//
// Parameters:
//  lpbyCache           Pointer to the cache
//  dwSector            The sector to add
//
// Description:
//  Adds the next consecutive sector of a stream to its BAT cache.
//
// Returns:
//  FALSE   If the CACHE has no more room for the new block
//  TRUE    On successful addition of the sector to the cache
//
//********************************************************************

BOOL SSAddSector
(
    LPBYTE              lpbyCache,
    DWORD               dwSector
)
{
	BYTE				byFlags;
	WORD				wCountSubpacs;
	LPBYTE				lpbyPtrCache;
	DWORD				dwDif, dwStartSector;
    LPLAT_CACHE_HDR_T   lpstCacheHdr;
    LPPACKET_HDR_T      lpstPacketHdr;
    LPSUBPAC_ABS_T      lpstSubpac;
    int                 nNumPadBytes;

	// Is there room on the last subpacket?

    lpstCacheHdr = (LPLAT_CACHE_HDR_T)lpbyCache;

    lpstPacketHdr = (LPPACKET_HDR_T)(lpbyCache +
        lpstCacheHdr->wLastPacketOffset);
    byFlags = lpstPacketHdr->byFlags;
    wCountSubpacs = lpstPacketHdr->byCountSubpacs;

    lpbyPtrCache = (LPBYTE)lpstPacketHdr + sizeof(PACKET_HDR_T);
	switch (byFlags)
	{
		case PACTYPE_ABS:
		{
			DWORD			dwContiguousSector;
			WORD			wCount;

            lpstSubpac = ((LPPACKET_ABS_T)lpbyPtrCache)->subpacs +
				wCountSubpacs - 1;

            dwStartSector = lpstSubpac->dwStartSector;
            wCount = lpstSubpac->wCount;
			dwContiguousSector = dwStartSector + wCount;

            // Can the new sector be placed in this subpacket?
            //  It can if it immediately follows the last sector
            //  in this subpacket and there is still room in this
            //  subpacket for it.

			if (dwContiguousSector == dwSector &&
				wCount < MAX_SEC_PER_SUBPAC_ABS)
			{
                lpstSubpac->wCount++;
                lpstCacheHdr->dwCountBlocksCached++;
                return(TRUE);
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
                wCountSubpacs >= 2 &&
                wCount < MAX_SEC_PER_SUBPAC_DIF)
			{
                // Decrement count of subpackets for this ABS packet

                lpstPacketHdr->byCountSubpacs--;

				// Create NDIF with two subpacs

                return SSCreateNDIF(lpstCacheHdr,
                                    (LPPACKET_HDR_T)lpstSubpac,
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

                return SSCreateNDIF(lpstCacheHdr,
                                    (LPPACKET_HDR_T)(lpstSubpac + 1),
                                    PACTYPE_FIRST_DIF,
                                    dwSector,
                                    1,
                                    0);
			}

            // At this point, either the difference between the new sector
            //  and the last cached sector is greater than the maximum
            //  allowed by an NDIF or the number of subpackets was one.

			// New ABS subpac

            if (lpstCacheHdr->wLastPacketOffset + sizeof(PACKET_HDR_T) +
				(wCountSubpacs + 1) * sizeof(SUBPAC_ABS_T) >
                lpstCacheHdr->dwCacheSize)
                return(FALSE);

            lpstCacheHdr->dwCountBlocksCached++;
            lpstPacketHdr->byCountSubpacs++;
            (++lpstSubpac)->dwStartSector = dwSector;
            lpstSubpac->wCount = 1;

            return(TRUE);
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
                    lpstCacheHdr->dwCountBlocksCached++;
                    return(TRUE);
				}

				// Convert to ABS

                lpstPacketHdr->byCountSubpacs--;
				if (wCountSubpacs != 1)
				{
                    // Convert last subpac to an ABS subpac

                    // Align on a DWORD boundary

                    if ((lpstPacketHdr->byCountSubpacs & 0x03) == 0)
                        nNumPadBytes = 0;
                    else
                        nNumPadBytes = 4 - (lpstPacketHdr->byCountSubpacs & 0x03);

                    if (lpbyCount - lpbyCache + nNumPadBytes +
                        sizeof(PACKET_HDR_T) + sizeof(SUBPAC_ABS_T) >
                        lpstCacheHdr->dwCacheSize)
                        return(FALSE);

                    lpstCacheHdr->wCountPackets++;

                    // Begin on DWORD boundary

                    lpstPacketHdr = (LPPACKET_HDR_T)(lpbyCount + nNumPadBytes);

                    lpstCacheHdr->wLastPacketOffset =
                        (LPBYTE)lpstPacketHdr - (LPBYTE)lpstCacheHdr;
                }

                lpstCacheHdr->dwCountBlocksCached++;

                lpstPacketHdr->byFlags = PACTYPE_ABS;
                lpstPacketHdr->byCountSubpacs = 1;

                lpstSubpac = (LPSUBPAC_ABS_T)(lpstPacketHdr + 1);

                lpstSubpac->dwStartSector = dwStartSector -
                    MAX_SEC_PER_SUBPAC_DIF;
                lpstSubpac->wCount = MAX_SEC_PER_SUBPAC_DIF + 1;

                return(TRUE);
			}

			if (dwDif <= PACDIF_MAX_DIF)
			{
                if (lpstCacheHdr->wLastPacketOffset + sizeof(PACKET_HDR_T) +
                    sizeof(DWORD) + (wCountSubpacs + 1) * sizeof(BYTE) >
                    lpstCacheHdr->dwCacheSize)
                    return(FALSE);

				if (byFlags == PACTYPE_FIRST_DIF ||
					byFlags == dwDif &&
					wCountSubpacs < MAX_SEC_PER_SUBPAC_DIF)
				{
					// Convert to NDIF and add new NDIF

                    lpstCacheHdr->dwCountBlocksCached++;
                    lpstPacketHdr->byFlags = (BYTE)dwDif;
                    lpstPacketHdr->byCountSubpacs++;
					*++lpbyCount = 1;

                    return(TRUE);
				}
			}

			// Create FIRST_DIF

            // The 1 is to account for lpbyCount pointing
            //  to the last subpacket

            if ((wCountSubpacs & 0x03) == 0)
                nNumPadBytes = 1;
            else
                nNumPadBytes = 1 + 4 - (wCountSubpacs & 0x03);

            return SSCreateNDIF(lpstCacheHdr,
                                (LPPACKET_HDR_T)(lpbyCount + nNumPadBytes),
                                PACTYPE_FIRST_DIF,
                                dwSector,
                                1,
                                0);
		}
	}
}


//********************************************************************
//
// Function:
//  BOOL SSGetSector()
//
// Parameters:
//  lpbyCache           Pointer to the cache
//  dwFindBlock         The block to find
//  lpdwSector          Ptr to DWORD of sector containing block
//
// Description:
//  Returns in *lpdwSector the sector containing dwFindBlock.
//  In the case where the cache does not containg the mapping,
//  *lpdwSector is set to the last known link
//
// Returns:
//  FALSE   If the block is not present in the cache
//  TRUE    If the function found the mapping
//
//********************************************************************

BOOL SSGetSector
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
                LPSUBPAC_ABS_T lpstSubpac;

                lpstSubpac = ((LPPACKET_ABS_T)lpbyCache)->subpacs;
				for (j=0;j<wCountSubpacs;j++)
				{
					if (dwCurBlock <= dwFindBlock &&
                        dwFindBlock < dwCurBlock + lpstSubpac->wCount)
					{
                        *lpdwSector = lpstSubpac->dwStartSector +
							(dwFindBlock - dwCurBlock);
                        return(TRUE);
					}

                    dwCurBlock += lpstSubpac->wCount;
                    ++lpstSubpac;
				}
                *lpdwSector = (lpstSubpac-1)->dwStartSector +
                    (lpstSubpac-1)->wCount - 1;
                lpbyCache = (LPBYTE)lpstSubpac;
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
                        return(TRUE);
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

    return(FALSE);
}


//********************************************************************
//
// Function:
//  SS_STATUS SSGetMiniFATSector()
//
// Parameters:
//  lpstRoot            Ptr to root structure
//  dwLink              A link in the mini FAT sector to get
//  lplpdwFAT           Ptr to ptr to store ptr to FAT block
//
// Description:
//  Checks the FAT cache for the presence of the desired mini
//  FAT block.  If the desired mini FAT block is not present,
//  the function reads it from the mini FAT block stream.
//
//  The function sets *lplpdwFAT to point to the block in the FAT
//  cache containing the desired mini FAT block.
//
// Returns:
//  SS_STATUS_OK        On success
//  SS_STATUS_ERROR     On error
//
//********************************************************************

SS_STATUS SSGetMiniFATSector
(
    LPSS_ROOT       lpstRoot,
    DWORD           dwLink,
    LPLPDWORD       lplpdwFAT
)
{
    DWORD           dwTemp;
    DWORD           dwFATBlkWanted;
    DWORD           dwLRUSlot;
    DWORD           dwMiniType;

    dwFATBlkWanted = dwLink >> SS_FAT_SECTOR_LINK_SHIFT;

    /////////////////////////////////////////////////////////////
    // Check the cache for the presence of the block
    /////////////////////////////////////////////////////////////

    dwLRUSlot = 0;
    dwMiniType = lpstRoot->dwFATCacheIsMiniTypeBitArray;
    for (dwTemp=0;dwTemp<lpstRoot->dwNumFATCacheSlots;dwTemp++)
    {
        // Does this slot contain the desired block?

        if (lpstRoot->lpdwFATCacheSlotBlock[dwTemp] == dwFATBlkWanted &&
            (dwMiniType & 1))
        {
            // Update the LRU value of the slot

            lpstRoot->lpdwFATCacheSlotLRU[dwTemp] =
                lpstRoot->dwFATCacheCurLRU++;

            // Set the ptr to point to the slot

            *lplpdwFAT = (LPDWORD)(lpstRoot->lpbyFATCacheBlocks +
                (dwTemp << SS_SECTOR_SHIFT));

            return(SS_STATUS_OK);
        }

        // Check LRU

        if (lpstRoot->lpdwFATCacheSlotLRU[dwTemp] <
            lpstRoot->lpdwFATCacheSlotLRU[dwLRUSlot])
            dwLRUSlot = dwTemp;

        dwMiniType >>= 1;
    }

    /////////////////////////////////////////////////////////////
    // Read the block from the mini FAT stream
    /////////////////////////////////////////////////////////////

    // Read the block into the LRU slot

    if (SSSeekRead(&lpstRoot->stMiniFATStream,
                   dwFATBlkWanted << SS_SECTOR_SHIFT,
                   lpstRoot->lpbyFATCacheBlocks +
                       (dwLRUSlot << SS_SECTOR_SHIFT),
                   SS_SECTOR_SIZE,
                   &dwTemp) != SS_STATUS_OK ||
        dwTemp != SS_SECTOR_SIZE)
    {
        // Error reading

        return(SS_STATUS_ERROR);
    }

    // Update the slot info

    lpstRoot->lpdwFATCacheSlotLRU[dwLRUSlot] =
        lpstRoot->dwFATCacheCurLRU++;

    lpstRoot->lpdwFATCacheSlotBlock[dwLRUSlot] = dwFATBlkWanted;

    // Set the type to mini FAT

    lpstRoot->dwFATCacheIsMiniTypeBitArray |= 1 << dwLRUSlot;

    // Set the ptr to point to the slot

    *lplpdwFAT = (LPDWORD)(lpstRoot->lpbyFATCacheBlocks +
        (dwLRUSlot << SS_SECTOR_SHIFT));

    return(SS_STATUS_OK);
}


//********************************************************************
//
// Function:
//  SS_STATUS SSGetFATSector()
//
// Parameters:
//  lpstRoot            Ptr to root structure
//  dwLink              A link in the FAT sector to get
//  lplpdwFAT           Ptr to ptr to store ptr to FAT block
//
// Description:
//  Checks the FAT cache for the presence of the desired FAT block.
//  If the desired FAT block is not present, the function reads it
//  from the structured storage.
//
//  The function sets *lplpdwFAT to point to the block in the FAT
//  cache containing the desired FAT block.
//
// Returns:
//  SS_STATUS_OK        On success
//  SS_STATUS_ERROR     On error
//
//********************************************************************

SS_STATUS SSGetFATSector
(
    LPSS_ROOT       lpstRoot,
    DWORD           dwLink,
    LPLPDWORD       lplpdwFAT
)
{
    DWORD           dwTemp;
    DWORD           dwFATBlkWanted;
    DWORD           dwLRUSlot;
    DWORD           dwMiniType;
    DWORD           dwSector;
    DWORD           dwDIFATBlockWanted;
    DWORD           dwBlock;

    dwFATBlkWanted = dwLink >> SS_FAT_SECTOR_LINK_SHIFT;

    /////////////////////////////////////////////////////////////
    // Check the cache for the presence of the block
    /////////////////////////////////////////////////////////////

    dwLRUSlot = 0;
    dwMiniType = lpstRoot->dwFATCacheIsMiniTypeBitArray;
    for (dwTemp=0;dwTemp<lpstRoot->dwNumFATCacheSlots;dwTemp++)
    {
        // Does this slot contain the desired block?

        if (lpstRoot->lpdwFATCacheSlotBlock[dwTemp] == dwFATBlkWanted &&
            (dwMiniType & 1) == 0)
        {
            // Update the LRU value of the slot

            lpstRoot->lpdwFATCacheSlotLRU[dwTemp] =
                lpstRoot->dwFATCacheCurLRU++;

            // Set the ptr to point to the slot

            *lplpdwFAT = (LPDWORD)(lpstRoot->lpbyFATCacheBlocks +
                (dwTemp << SS_SECTOR_SHIFT));

            return(SS_STATUS_OK);
        }

        // Check LRU

        if (lpstRoot->lpdwFATCacheSlotLRU[dwTemp] <
            lpstRoot->lpdwFATCacheSlotLRU[dwLRUSlot])
            dwLRUSlot = dwTemp;

        dwMiniType >>= 1;
    }

    /////////////////////////////////////////////////////////////
    // Determine where the FAT block is located
    /////////////////////////////////////////////////////////////

    if (dwFATBlkWanted < SS_CDFAT)
    {
        // The location can be gotten from the header

        dwSector = DWENDIAN(lpstRoot->stHdr.dwFATSector[dwFATBlkWanted]);
    }
    else
    {
        // Iteration through double-indirect FAT required

        dwSector = DWENDIAN(lpstRoot->stHdr.dwDIFATStartSector);
        dwDIFATBlockWanted =
            (dwFATBlkWanted - SS_CDFAT) / SS_LINKS_PER_DIFAT_SECTOR;

        if (dwDIFATBlockWanted >=
            DWENDIAN(lpstRoot->stHdr.dwDIFATSectorCount))
        {
            // Out of range

            return(SS_STATUS_ERROR);
        }

        dwBlock = 0;
        while (1)
        {
            if (dwBlock++ == dwDIFATBlockWanted)
            {
                // Read the sector

                if (SSFileSeek(lpstRoot->lpvRootCookie,
                               lpstRoot->lpvFile,
                               ((dwSector + 1) << SS_SECTOR_SHIFT) +
                                   ((dwFATBlkWanted - SS_CDFAT) %
                                       SS_LINKS_PER_DIFAT_SECTOR) *
                                           sizeof(DWORD),
                               SS_SEEK_SET,
                               NULL) != SS_STATUS_OK)
                {
                    // Error seeking to DWORD containing sector
                    //  number of desired FAT block

                    return(SS_STATUS_ERROR);
                }

                if (SSFileRead(lpstRoot->lpvRootCookie,
                               lpstRoot->lpvFile,
                               &dwSector,
                               sizeof(DWORD),
                               &dwTemp) != SS_STATUS_OK ||
                    dwTemp != sizeof(DWORD))
                {
                    // Error reading DWORD containing sector
                    //  number of desired FAT block

                    return(SS_STATUS_ERROR);
                }

                dwSector = DWENDIAN(dwSector);

                break;
            }
            else
            {
                // Read the location of the next DIFAT block

                if (SSFileSeek(lpstRoot->lpvRootCookie,
                               lpstRoot->lpvFile,
                               ((dwSector + 1) << SS_SECTOR_SHIFT) +
                                   SS_LINKS_PER_DIFAT_SECTOR * sizeof(DWORD),
                               SS_SEEK_SET,
                               NULL) != SS_STATUS_OK)
                {
                    // Error seeking to DWORD containing sector
                    //  number of desired FAT block

                    return(SS_STATUS_ERROR);
                }

                if (SSFileRead(lpstRoot->lpvRootCookie,
                               lpstRoot->lpvFile,
                               &dwSector,
                               sizeof(DWORD),
                               &dwTemp) != SS_STATUS_OK ||
                    dwTemp != sizeof(DWORD))
                {
                    // Error reading DWORD containing sector
                    //  number of desired FAT block

                    return(SS_STATUS_ERROR);
                }

                dwSector = DWENDIAN(dwSector);
            }
        }
    }

    /////////////////////////////////////////////////////////////
    // Read the block
    /////////////////////////////////////////////////////////////

    // Read the block into the LRU slot

    if (SSFileSeek(lpstRoot->lpvRootCookie,
                   lpstRoot->lpvFile,
                   (dwSector + 1) << SS_SECTOR_SHIFT,
                   SS_SEEK_SET,
                   NULL) != SS_STATUS_OK)
    {
        // Error seeking

        return(SS_STATUS_ERROR);
    }

    if (SSFileRead(lpstRoot->lpvRootCookie,
                   lpstRoot->lpvFile,
                   lpstRoot->lpbyFATCacheBlocks +
                       (dwLRUSlot << SS_SECTOR_SHIFT),
                   SS_SECTOR_SIZE,
                   &dwTemp) != SS_STATUS_OK ||
        dwTemp != SS_SECTOR_SIZE)
    {
        // Error reading

        return(SS_STATUS_ERROR);
    }

    // Update the slot info

    lpstRoot->lpdwFATCacheSlotLRU[dwLRUSlot] =
        lpstRoot->dwFATCacheCurLRU++;

    lpstRoot->lpdwFATCacheSlotBlock[dwLRUSlot] = dwFATBlkWanted;

    // Set the type to regular FAT

    lpstRoot->dwFATCacheIsMiniTypeBitArray &= ~(1 << dwLRUSlot);

    // Set the ptr to point to the slot

    *lplpdwFAT = (LPDWORD)(lpstRoot->lpbyFATCacheBlocks +
        (dwLRUSlot << SS_SECTOR_SHIFT));

    return(SS_STATUS_OK);
}


//********************************************************************
//
// Function:
//  SS_STATUS SSSeekStreamBlock()
//
// Parameters:
//  lpstStream          Ptr to stream structure
//  dwBlock             The block to get
//  lpdwSector          Ptr to DWORD to store sector containing block
//
// Description:
//  The function returns in *lpdwSector the sector containing the
//  desired block.
//
//  The stream can be either a mini stream or a regular stream.
//
// Returns:
//  SS_STATUS_OK        On success
//  SS_STATUS_ERROR     On error
//
//********************************************************************

SS_STATUS SSSeekStreamBlock
(
    LPSS_STREAM     lpstStream,
    DWORD           dwBlock,
    LPDWORD         lpdwSector
)
{
    DWORD           dwCurBlk;
    DWORD           dwLink;
    DWORD           dwMinLink, dwMaxLink;
    LPDWORD         lpdwFAT;

#ifdef SYM_NLM
    DWORD dwRelinguishCount = 0;

    SSProgress(lpstStream->lpstRoot->lpvRootCookie);
#endif

    if (lpstStream->lpbyCache != NULL)
    {
        // Check the cache

        if (SSGetSector(lpstStream->lpbyCache,
                        dwBlock,
                        lpdwSector) == TRUE)
		{
            return(SS_STATUS_OK);
		}

        dwCurBlk = ((LPLAT_CACHE_HDR_T)lpstStream->lpbyCache)->
			dwCountBlocksCached - 1;
        dwLink = *lpdwSector;
	}
	else
	{
        // See if the links can be followed forward

        if (dwBlock < lpstStream->dwStreamBlk)
        {
            dwCurBlk = 0;
            dwLink = lpstStream->dwStartSector;
        }
        else
        {
            dwCurBlk = lpstStream->dwStreamBlk;
            dwLink = lpstStream->dwLink;
        }
    }

    // Make sure we have a link that we can follow

    if (dwLink == 0xFFFFFFFF)
        return(SS_STATUS_ERROR);

    dwMinLink = dwMaxLink = 0xFFFFFFFF;
    while (dwCurBlk != dwBlock)
    {
        if (dwLink < dwMinLink || dwMaxLink < dwLink)
        {
#ifdef SYM_NLM
            if ((dwRelinguishCount++ % 32) == 0)
                SSProgress(lpstStream->lpstRoot->lpvRootCookie);
#endif

            if (lpstStream->dwFlags & SS_STREAM_FLAG_MINI_STREAM)
            {
                if (SSGetMiniFATSector(lpstStream->lpstRoot,
                                       dwLink,
                                       &lpdwFAT) != SS_STATUS_OK)
                {
                    // Failure getting mini FAT sector

                    return(SS_STATUS_ERROR);
                }
            }
            else
            {
                if (SSGetFATSector(lpstStream->lpstRoot,
                                   dwLink,
                                   &lpdwFAT) != SS_STATUS_OK)
                {
                    // Failure getting FAT sector

                    return(SS_STATUS_ERROR);
                }
            }

            dwMinLink = dwLink & ~(SS_LINKS_PER_FAT_SECTOR - 1);
            dwMaxLink = dwMinLink + SS_LINKS_PER_FAT_SECTOR - 1;
        }

        // Get the next sector

        dwLink = DWENDIAN(lpdwFAT[dwLink - dwMinLink]);
        if (dwLink == SS_FAT_ENDOFCHAIN ||
            dwLink == SS_FAT_FREESECT)
        {
            // Error, attempt to seek past the end, so reset

            lpstStream->dwStreamBlk = 0xFFFFFFFF;
            return(SS_STATUS_ERROR);
        }

        // Update current block number

        ++dwCurBlk;

        // Update the cache

        if (lpstStream->lpbyCache != NULL)
        {
            if (((LPLAT_CACHE_HDR_T)(lpstStream->lpbyCache))->
                dwCountBlocksCached == dwCurBlk)
                SSAddSector(lpstStream->lpbyCache,dwLink);
        }
    }

    // Remember the last block and link

    lpstStream->dwStreamBlk = dwCurBlk;
    lpstStream->dwLink = dwLink;

    // Return the sector

    *lpdwSector = dwLink;

    return(SS_STATUS_OK);
}


//********************************************************************
//
// Function:
//  SS_STATUS SSSeekStream()
//
// Parameters:
//  lpstStream          Ptr to stream structure
//  dwOffset            Offset to read from
//  dwBytesToRead       Number of bytes to read
//  lpdwSeekOffset      Ptr to DWORD for start of bytes at offset
//  lpdwNumBytes        Number of contiguous bytes at offset
//
// Description:
//  Given a seek offset within a stream and the number of bytes
//  to access at that offset, the function returns in *lpdwSeekOffset
//  the offset in the structured storage of the given offset in the
//  stream and returns in *lpdwNumBytes the number of contiguous
//  bytes at that offset.
//
//  If the stream is in the mini-sector stream, the number of
//  contiguous bytes may not actually be the maximum number
//  of contiguous bytes due to the extra complexity required
//  in the extra indirection.
//
// Returns:
//  SS_STATUS_OK        On success
//  SS_STATUS_ERROR     On error
//
//********************************************************************

SS_STATUS SSSeekStream
(
    LPSS_STREAM     lpstStream,
    DWORD           dwOffset,
    DWORD           dwBytesToRead,
    LPDWORD         lpdwSeekOffset,
    LPDWORD         lpdwNumBytes
)
{
    DWORD           dwBlk, dwEndBlk;
    DWORD           dwFirstSector;
    DWORD           dwLastSector;
    DWORD           dwSector;
    DWORD           dwNumBytes;

    if (lpstStream->dwFlags & SS_STREAM_FLAG_MINI_STREAM)
    {
        /////////////////////////////////////////////////////////
        // Mini sector stream seek
        /////////////////////////////////////////////////////////

        // Determine contiguous chunk in mini stream

        // Determine the starting block, ending block, and the
        //  number of bytes

        dwBlk = dwOffset >> SS_MINI_SECTOR_SHIFT;
        dwEndBlk = (dwOffset + dwBytesToRead - 1) >> SS_MINI_SECTOR_SHIFT;
        dwNumBytes = SS_MINI_SECTOR_SIZE -
            (dwOffset & SS_MINI_SECTOR_OFFSET_MASK);

        if (SSSeekStreamBlock(lpstStream,
                              dwBlk,
                              &dwFirstSector) != SS_STATUS_OK)
        {
            // Failure getting the first sector

            return(SS_STATUS_ERROR);
        }

        // Iterate while the sectors are contiguous

        dwLastSector = dwFirstSector;
        while (dwBlk++ < dwEndBlk)
        {
            if (SSSeekStreamBlock(lpstStream,
                                  dwBlk,
                                  &dwSector) != SS_STATUS_OK)
            {
                // Failure getting intermediate sector

                return(SS_STATUS_ERROR);
            }

            if (dwLastSector + 1 != dwSector)
            {
                // No longer contiguous

                break;
            }

            dwLastSector = dwSector;

            dwNumBytes += SS_MINI_SECTOR_SIZE;
        }

        // Store offset of chunk and number of contiguous bytes

        dwOffset = (dwFirstSector << SS_MINI_SECTOR_SHIFT) +
            (dwOffset & SS_MINI_SECTOR_OFFSET_MASK);

        if (dwNumBytes < dwBytesToRead)
            dwBytesToRead = dwNumBytes;

        // Now determine contiguous chunk from mini stream

        lpstStream = &lpstStream->lpstRoot->stMiniSectorStream;
    }

    /////////////////////////////////////////////////////////
    // Regular stream seek
    /////////////////////////////////////////////////////////

    // Determine the starting block, ending block, and the
    //  number of bytes

    dwBlk = dwOffset >> SS_SECTOR_SHIFT;
    dwEndBlk = (dwOffset + dwBytesToRead - 1) >> SS_SECTOR_SHIFT;
    dwNumBytes = SS_SECTOR_SIZE - (dwOffset & SS_SECTOR_OFFSET_MASK);

    if (SSSeekStreamBlock(lpstStream,
                          dwBlk,
                          &dwFirstSector) != SS_STATUS_OK)
    {
        // Failure getting the first sector

        return(SS_STATUS_ERROR);
    }

    // Iterate while the sectors are contiguous

    dwLastSector = dwFirstSector;
    while (dwBlk++ < dwEndBlk)
    {
        if (SSSeekStreamBlock(lpstStream,
                              dwBlk,
                              &dwSector) != SS_STATUS_OK)
        {
            // Failure getting intermediate sector

            return(SS_STATUS_ERROR);
        }

        if (dwLastSector + 1 != dwSector)
        {
            // No longer contiguous

            break;
        }

        dwLastSector = dwSector;

        dwNumBytes += SS_SECTOR_SIZE;
    }

    // Store offset of chunk and number of contiguous bytes

    *lpdwSeekOffset = ((dwFirstSector + 1) << SS_SECTOR_SHIFT) +
        (dwOffset & SS_SECTOR_OFFSET_MASK);

    if (dwNumBytes > dwBytesToRead)
        *lpdwNumBytes = dwBytesToRead;
    else
        *lpdwNumBytes = dwNumBytes;

    return(SS_STATUS_OK);
}


//********************************************************************
//
// Function:
//  SS_STATUS SSSeekRead()
//
// Parameters:
//  lpstStream          Ptr to stream structure
//  dwOffset            Offset to read from
//  lpvBuffer           Ptr to buffer to read into
//  dwBytesToRead       Number of bytes to read
//  lpdwBytesRead       Number of bytes actually read
//
// Description:
//  Reads from the given stream dwBytesToRead into the buffer
//  pointed to by lpvBuffer.
//
//  If the number of bytes to read overruns the end of the stream,
//  only the number of bytes up to the end is read.
//
// Returns:
//  SS_STATUS_OK        On success
//  SS_STATUS_ERROR     On error
//
//********************************************************************

SS_STATUS SSSeekRead
(
    LPSS_STREAM         lpstStream,
    DWORD               dwOffset,
    LPVOID              lpvBuffer,
    DWORD               dwBytesToRead,
    LPDWORD             lpdwBytesRead
)
{
    DWORD               dwSeekOffset, dwNumBytes;
    DWORD               dwBytesRead;

    // Check for read past the end

    if (dwOffset + dwBytesToRead > lpstStream->dwStreamLen)
    {
        if (dwOffset >= lpstStream->dwStreamLen)
            dwBytesToRead = 0;
        else
            dwBytesToRead = lpstStream->dwStreamLen - dwOffset;
    }

    *lpdwBytesRead = dwBytesToRead;

    // Read in chunks of contiguous bytes

    while (dwBytesToRead != 0)
    {
        if (SSSeekStream(lpstStream,
                         dwOffset,
                         dwBytesToRead,
                         &dwSeekOffset,
                         &dwNumBytes) != SS_STATUS_OK)
        {
            // Failed to find the offset in the file

            return(SS_STATUS_ERROR);
        }

        if (SSFileSeek(lpstStream->lpstRoot->lpvRootCookie,
                       lpstStream->lpstRoot->lpvFile,
                       dwSeekOffset,
                       SS_SEEK_SET,
                       NULL) != SS_STATUS_OK)
        {
            // Failed to seek to the necessary location

            return(SS_STATUS_ERROR);
        }

        if (SSFileRead(lpstStream->lpstRoot->lpvRootCookie,
                       lpstStream->lpstRoot->lpvFile,
                       lpvBuffer,
                       dwNumBytes,
                       &dwBytesRead) != SS_STATUS_OK ||
            dwBytesRead != dwNumBytes)
        {
            // Failed to read the contiguous set of bytes

            return(SS_STATUS_ERROR);
        }

        dwOffset += dwNumBytes;
        dwBytesToRead -= dwNumBytes;
        lpvBuffer = (LPBYTE)lpvBuffer + dwNumBytes;
    }

    return(SS_STATUS_OK);
}


//********************************************************************
//
// Function:
//  SS_STATUS SSSeekWrite()
//
// Parameters:
//  lpstStream          Ptr to stream structure
//  dwOffset            Offset to write from
//  lpvBuffer           Ptr to buffer to write from
//  dwBytesToWrite      Number of bytes to write
//  lpdwBytesWritten    Number of bytes actually written
//
// Description:
//  Writes dwBytesToWrite bytes from the given buffer to the given
//  stream.
//
//  The function does not perform an initial check for overrunning
//  the length of the stream.
//
// Returns:
//  SS_STATUS_OK        On success
//  SS_STATUS_ERROR     On error
//
//********************************************************************

SS_STATUS SSSeekWrite
(
    LPSS_STREAM         lpstStream,
    DWORD               dwOffset,
    LPVOID              lpvBuffer,
    DWORD               dwBytesToWrite,
    LPDWORD             lpdwBytesWritten
)
{
    DWORD               dwSeekOffset, dwNumBytes;
    DWORD               dwBytesWritten;

    // Just set this now

    *lpdwBytesWritten = dwBytesToWrite;

    // Write in chunks of contiguous bytes

    while (dwBytesToWrite != 0)
    {
        if (SSSeekStream(lpstStream,
                         dwOffset,
                         dwBytesToWrite,
                         &dwSeekOffset,
                         &dwNumBytes) != SS_STATUS_OK)
        {
            // Failed to find the offset in the file

            return(SS_STATUS_ERROR);
        }

        if (SSFileSeek(lpstStream->lpstRoot->lpvRootCookie,
                       lpstStream->lpstRoot->lpvFile,
                       dwSeekOffset,
                       SS_SEEK_SET,
                       NULL) != SS_STATUS_OK)
        {
            // Failed to seek to the necessary location

            return(SS_STATUS_ERROR);
        }

        if (SSFileWrite(lpstStream->lpstRoot->lpvRootCookie,
                        lpstStream->lpstRoot->lpvFile,
                        lpvBuffer,
                        dwNumBytes,
                        &dwBytesWritten) != SS_STATUS_OK ||
            dwBytesWritten != dwNumBytes)
        {
            // Failed to write the contiguous set of bytes

            return(SS_STATUS_ERROR);
        }

        dwOffset += dwNumBytes;
        dwBytesToWrite -= dwNumBytes;
        lpvBuffer = (LPBYTE)lpvBuffer + dwNumBytes;
    }

    return(SS_STATUS_OK);
}


//********************************************************************
//
// Function:
//  SS_STATUS SSWriteZeroes()
//
// Parameters:
//  lpstStream          Ptr to stream structure
//  dwStartOffset       Offset to write from
//  dwNumBytes          Number of zero bytes to write
//
// Description:
//  Writes dwNumBytes zero bytes to the stream starting from the
//  given offset.
//
// Returns:
//  SS_STATUS_OK        On success
//  SS_STATUS_ERROR     On error
//
//********************************************************************

SS_STATUS SSWriteZeroes
(
    LPSS_STREAM         lpstStream,
    DWORD               dwStartOffset,
    DWORD               dwNumBytes
)
{
    int                 i;
    DWORD               dwNumBytesToWrite;
    DWORD               dwNumBytesWritten;
    BYTE                abyWorkBuffer[512];

    // Create a buffer of zeroes

    for (i=0;i<512;i++)
        abyWorkBuffer[i] = 0;

    // Write the zeroes

    dwNumBytesToWrite = 512;
    while (dwNumBytes != 0)
    {
        if (dwNumBytes < 512)
        {
            dwNumBytesToWrite = dwNumBytes;
        }

        if (SSSeekWrite(lpstStream,
                        dwStartOffset,
                        abyWorkBuffer,
                        dwNumBytesToWrite,
                        &dwNumBytesWritten) != SS_STATUS_OK ||
            dwNumBytesWritten != dwNumBytesToWrite)
        {
            // Failed to write buffer of zeroes

            return(SS_STATUS_ERROR);
        }

        dwStartOffset += dwNumBytesToWrite;
        dwNumBytes -= dwNumBytesToWrite;
    }

    return(SS_STATUS_OK);
}


//********************************************************************
//
// Function:
//  SS_STATUS SSWriteZeroes()
//
// Parameters:
//  lpstStream          Ptr to stream structure
//  dwSrcOffset         The source offset to copy from
//  dwDstOffset         The destination offset to copy to
//  dwNumBytes          Number of zero bytes to write
//
// Description:
//  Copies an array of bytes from one part of a stream to another
//  part.  The function copies in 512 byte chunks.
//
//  The function works correctly even if the arrays overlap.
//
// Returns:
//  SS_STATUS_OK        On success
//  SS_STATUS_ERROR     On error
//
//********************************************************************

SS_STATUS SSCopyBytes
(
    LPSS_STREAM         lpstStream,
    DWORD               dwSrcOffset,
    DWORD               dwDstOffset,
    DWORD               dwNumBytes
)
{
    BOOL                bForward;
    DWORD               dwNumBytesToCopy;
    DWORD               dwNumBytesCopied;
    BYTE                abyWorkBuffer[512];

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

    dwNumBytesToCopy = 512;
    while (dwNumBytes != 0)
    {
        if (dwNumBytes < 512)
        {
            dwNumBytesToCopy = dwNumBytes;
        }

        if (SSSeekRead(lpstStream,
                       dwSrcOffset,
                       abyWorkBuffer,
                       dwNumBytesToCopy,
                       &dwNumBytesCopied) != SS_STATUS_OK ||
            dwNumBytesCopied != dwNumBytesToCopy)
        {
            // Failed to read source bytes

            return(SS_STATUS_ERROR);
        }

        if (SSSeekWrite(lpstStream,
                        dwDstOffset,
                        abyWorkBuffer,
                        dwNumBytesToCopy,
                        &dwNumBytesCopied) != SS_STATUS_OK ||
            dwNumBytesCopied != dwNumBytesToCopy)
        {
            // Failed to write bytes to destination

            return(SS_STATUS_ERROR);
        }

        // Update number of bytes left to copy

        dwNumBytes -= dwNumBytesToCopy;

        // Update source and destination offsets

        if (bForward == FALSE)
        {
            dwSrcOffset += dwNumBytesToCopy;
            dwDstOffset += dwNumBytesToCopy;
        }
        else
        {
            if (dwNumBytes < 512)
                dwNumBytesToCopy = dwNumBytes;

            dwSrcOffset -= dwNumBytesToCopy;
            dwDstOffset -= dwNumBytesToCopy;
        }
    }

    return(SS_STATUS_OK);
}


//********************************************************************
//
// Function:
//  SS_STATUS SSStreamLen()
//
// Parameters:
//  lpstStream          Ptr to stream structure
//
// Description:
//  Returns the length of the given stream.
//
// Returns:
//  DWORD               Length of the given stream
//
//********************************************************************

DWORD SSStreamLen
(
    LPSS_STREAM         lpstStream
)
{
    return(lpstStream->dwStreamLen);
}


//********************************************************************
//
// Function:
//  SS_STATUS SSStreamID()
//
// Parameters:
//  lpstStream          Ptr to stream structure
//
// Description:
//  Returns the ID of the given stream.
//
// Returns:
//  DWORD               ID of the given stream
//
//********************************************************************

DWORD SSStreamID
(
    LPSS_STREAM         lpstStream
)
{
    return(lpstStream->dwSID);
}


//********************************************************************
//
// Function:
//  SS_STATUS SSGetDirBlock()
//
// Parameters:
//  lpstRoot            Root structure for structured storage
//  dwBlock             The directory block to get
//  lplpstDirEntry      Ptr to ptr to store ptr to directory block
//
// Description:
//  Checks the directory cache for the presence of the given
//  directory block.  If present, the function sets *lplpstDirEntry
//  to point to the given block in the cache.  If not present,
//  the function reads in the given block and then sets
//  *lplpstDirEntry to point to the given block read into the cache.
//
// Returns:
//  SS_STATUS_OK        On success
//  SS_STATUS_ERROR     On error
//
//********************************************************************

SS_STATUS SSGetDirBlock
(
    LPSS_ROOT           lpstRoot,
    DWORD               dwBlock,
    LPLPSS_DIR_ENTRY    lplpstDirEntry
)
{
    DWORD               dwTemp;
    DWORD               dwLRUSlot;

    /////////////////////////////////////////////////////////////
    // Check the cache for the presence of the block
    /////////////////////////////////////////////////////////////

    dwLRUSlot = 0;
    for (dwTemp=0;dwTemp<lpstRoot->dwNumDirCacheSlots;dwTemp++)
    {
        // Does this slot contain the desired block?

        if (lpstRoot->lpdwDirCacheSlotBlock[dwTemp] == dwBlock)
        {
            // Update the LRU value of the slot

            lpstRoot->lpdwDirCacheSlotLRU[dwTemp] =
                lpstRoot->dwDirCacheCurLRU++;

            // Set the ptr to point to the slot

            *lplpstDirEntry = (LPSS_DIR_ENTRY)
                (lpstRoot->lpbyDirCacheBlocks +
                 (dwTemp << SS_SECTOR_SHIFT));

            return(SS_STATUS_OK);
        }

        // Check LRU

        if (lpstRoot->lpdwDirCacheSlotLRU[dwTemp] <
            lpstRoot->lpdwDirCacheSlotLRU[dwLRUSlot])
            dwLRUSlot = dwTemp;
    }

    /////////////////////////////////////////////////////////////
    // Read the block from the dir stream
    /////////////////////////////////////////////////////////////

    // Read the block into the LRU slot

    if (SSSeekRead(&lpstRoot->stDirStream,
                   dwBlock << SS_SECTOR_SHIFT,
                   lpstRoot->lpbyDirCacheBlocks +
                       (dwLRUSlot << SS_SECTOR_SHIFT),
                   SS_SECTOR_SIZE,
                   &dwTemp) != SS_STATUS_OK ||
        dwTemp != SS_SECTOR_SIZE)
    {
        // Error reading

        return(SS_STATUS_ERROR);
    }

    // Update the slot info

    lpstRoot->lpdwDirCacheSlotLRU[dwLRUSlot] =
        lpstRoot->dwDirCacheCurLRU++;

    lpstRoot->lpdwDirCacheSlotBlock[dwLRUSlot] = dwBlock;

    // Set the ptr to point to the slot

    *lplpstDirEntry = (LPSS_DIR_ENTRY)
        (lpstRoot->lpbyDirCacheBlocks + (dwLRUSlot << SS_SECTOR_SHIFT));

    return(SS_STATUS_OK);
}


//********************************************************************
//
// Function:
//  SS_STATUS SSGetDirEntry()
//
// Parameters:
//  lpstRoot            Root structure for structured storage
//  dwID                ID of entry to get
//  lplpstDirEntry      Ptr to ptr to store ptr to directory entry
//
// Description:
//  Uses SSGetDirBlock() to read the block containing the entry
//  and then sets *lplpstDirEntry to point to the appropriate
//  entry within that block.
//
// Returns:
//  SS_STATUS_OK        On success
//  SS_STATUS_ERROR     On error
//
//********************************************************************

SS_STATUS SSGetDirEntry
(
    LPSS_ROOT           lpstRoot,
    DWORD               dwID,
    LPLPSS_DIR_ENTRY    lplpstDirEntry
)
{
    if (SSGetDirBlock(lpstRoot,
                      dwID >> SS_DIR_ENTRIES_SECTOR_SHIFT,
                      lplpstDirEntry) != SS_STATUS_OK)
    {
        return(SS_STATUS_ERROR);
    }

    *lplpstDirEntry += dwID & SS_DIR_ENTRIES_ENTRY_MASK;

    return(SS_STATUS_OK);
}


//********************************************************************
//
// Function:
//  SS_STATUS SSUpdateDirEntry()
//
// Parameters:
//  lpstRoot            Root structure for structured storage
//  dwID                ID of entry to update
//
// Description:
//  This function is called after directly updating a directory
//  entry.  The function writes the updated entry to the physical
//  file.  For simplicity, the function writes the entire
//  directory block containing the updated entry.
//
// Returns:
//  SS_STATUS_OK        On success
//  SS_STATUS_ERROR     On error
//
//********************************************************************

SS_STATUS SSUpdateDirEntry
(
    LPSS_ROOT           lpstRoot,
    DWORD               dwID
)
{
    DWORD               dwTemp;
    DWORD               dwBlock;

    dwBlock = dwID >> SS_DIR_ENTRIES_SECTOR_SHIFT;

    // Search the cache for the block to update

    for (dwTemp=0;dwTemp<lpstRoot->dwNumDirCacheSlots;dwTemp++)
    {
        // Does this slot contain the desired block?

        if (lpstRoot->lpdwDirCacheSlotBlock[dwTemp] == dwBlock)
        {
            // Update the LRU value of the slot

            lpstRoot->lpdwDirCacheSlotLRU[dwTemp] =
                lpstRoot->dwDirCacheCurLRU++;

            // Write the block

            if (SSSeekWrite(&lpstRoot->stDirStream,
                            dwBlock << SS_SECTOR_SHIFT,
                            lpstRoot->lpbyDirCacheBlocks +
                                (dwTemp << SS_SECTOR_SHIFT),
                            SS_SECTOR_SIZE,
                            &dwTemp) != SS_STATUS_OK ||
                dwTemp != SS_SECTOR_SIZE)
            {
                // Error reading

                return(SS_STATUS_ERROR);
            }

            return(SS_STATUS_OK);
        }
    }

    // The cache did not contain the block to update.
    //  This is fatal!

    return(SS_STATUS_ERROR);
}


//********************************************************************
//
// Function:
//  SS_STATUS SSLimitStreamLen()
//
// Parameters:
//  lpstStream          Ptr to stream to limit length of
//
// Description:
//  The function limits the length of the given stream to the
//  length of the dir stream.
//
// Returns:
//  Nothing
//
//********************************************************************

void SSLimitStreamLen
(
    LPSS_STREAM         lpstStream
)
{
    DWORD               dwMaxStreamLen;

    dwMaxStreamLen = lpstStream->lpstRoot->stDirStream.dwStreamLen;
    if (lpstStream->dwStreamLen > dwMaxStreamLen)
        lpstStream->dwStreamLen = dwMaxStreamLen;
}


//********************************************************************
//
// Function:
//  SS_STATUS SSCreateRoot()
//
// Parameters:
//  lplpstRoot          Ptr to ptr to store root ptr
//  lpvRootCookie       Root cookie for OS routines
//  lpvFile             Pointer to file object for file ops
//  dwFlags             Initialization flags
//
// Description:
//  The function does the following:
//      1. Allocates memory for the root structure
//      2. Reads the header of the structured storage object
//      3. Checks to see that the header values matches
//          assumed known values
//      4. Allocates memory for the following caches:
//          a. Directory cache
//          b. FAT cache
//          c. Dir stream BAT cache
//          d. Mini FAT stream BAT cache
//          e. Mini sector stream BAT cache
//      5. Initializes directory and FAT cache
//      6. Initializes dir and FAT streams
//      7. Reads the first block of the dir stream
//      8. Initializes mini sector stream
//
// Returns:
//  SS_STATUS_OK        On success
//  SS_STATUS_ERROR     On error allocating memory or reading
//
//********************************************************************

SS_STATUS SSCreateRoot
(
    LPLPSS_ROOT         lplpstRoot,
    LPVOID              lpvRootCookie,
    LPVOID              lpvFile,
    DWORD               dwFlags
)
{
    LPSS_ROOT           lpstRoot;
    DWORD               dwBytes;
    DWORD               dwTemp;
    LPBYTE              lpbyMem;
    LPSS_DIR_ENTRY      lpstDirEntry;

    // Allocate root structure

    if (SSMemoryAlloc(lpvRootCookie,
                      sizeof(SS_ROOT_T),
                      (LPLPVOID)&lpstRoot) != SS_STATUS_OK)
    {
        return(SS_STATUS_ERROR);
    }

    // Read header

    if (SSFileSeek(lpvRootCookie,
                   lpvFile,
                   0,
                   SS_SEEK_SET,
                   (LPLONG)&dwTemp) != SS_STATUS_OK)
    {
        SSMemoryFree(lpvRootCookie,lpstRoot);
        return(SS_STATUS_ERROR);
    }

    if (SSFileRead(lpvRootCookie,
                   lpvFile,
                   &(lpstRoot->stHdr),
                   sizeof(SS_HDR_T),
                   &dwBytes) != SS_STATUS_OK ||
        dwBytes != sizeof(SS_HDR_T))
    {
        SSMemoryFree(lpvRootCookie,lpstRoot);
        return(SS_STATUS_ERROR);
    }

    // Endianize here

    // Verify assumptions in header

    if ((lpstRoot->stHdr.abySig[0] & 0xF0) != 0xD0 ||
        lpstRoot->stHdr.abySig[1] != 0xCF ||
        lpstRoot->stHdr.abySig[2] != 0x11 ||
        lpstRoot->stHdr.abySig[3] != 0xE0 ||
        lpstRoot->stHdr.abySig[4] != 0xA1 ||
        lpstRoot->stHdr.abySig[5] != 0xB1 ||
        lpstRoot->stHdr.abySig[6] != 0x1A ||
        lpstRoot->stHdr.abySig[7] != 0xE1 ||
        WENDIAN(lpstRoot->stHdr.wSectorShift) != 9 ||
        WENDIAN(lpstRoot->stHdr.wMiniSectorShift) != 6 ||
        DWENDIAN(lpstRoot->stHdr.dwMiniSectorCutoff) != 0x1000)
    {
        SSMemoryFree(lpvRootCookie,lpstRoot);
        return(SS_STATUS_ERROR);
    }

    lpstRoot->lpvRootCookie = lpvRootCookie;
    lpstRoot->lpvFile = lpvFile;

    /////////////////////////////////////////////////////////////
    // Calculate additional memory needed
    /////////////////////////////////////////////////////////////

    dwBytes = 0;

    // Directory cache

    lpstRoot->dwNumDirCacheSlots =
        dwFlags & SS_ROOT_FLAG_DIR_CACHE_MASK;

    if (lpstRoot->dwNumDirCacheSlots == 0)
        lpstRoot->dwNumDirCacheSlots = 2;

    dwBytes += lpstRoot->dwNumDirCacheSlots *
        (sizeof(DWORD) +        // lpdwDirCacheSlotLRU
         sizeof(DWORD) +        // lpdwDirCacheSlotSector
         SS_SECTOR_SIZE);       // lpstDirCacheSectors

    // FAT cache

    lpstRoot->dwNumFATCacheSlots =
        (dwFlags & SS_ROOT_FLAG_FAT_CACHE_MASK) >> 4;

    if (lpstRoot->dwNumFATCacheSlots == 0)
        lpstRoot->dwNumFATCacheSlots = 4;

    dwBytes += lpstRoot->dwNumFATCacheSlots *
        (sizeof(DWORD) +        // lpdwFATCacheSlotLRU
         sizeof(DWORD) +        // lpdwFATCacheSlotSector
         SS_SECTOR_SIZE);       // lpdwFATCacheSectors

    // Mini FAT, mini sector , and dir stream BAT caches

    dwBytes += SS_MINI_FAT_BAT_CACHE_SIZE +
               SS_MINI_SECTOR_BAT_CACHE_SIZE +
               SS_DIR_BAT_CACHE_SIZE;

    // Allocate additional memory

    lpstRoot->dwExtraMemorySize = dwBytes;

    if (SSMemoryAlloc(lpvRootCookie,
                      dwBytes,
                      &lpstRoot->lpvExtraMemory) != SS_STATUS_OK)
    {
        SSMemoryFree(lpvRootCookie,lpstRoot);
        return(SS_STATUS_ERROR);
    }

    /////////////////////////////////////////////////////////////
    // Assign additional memory
    /////////////////////////////////////////////////////////////

    lpbyMem = (LPBYTE)lpstRoot->lpvExtraMemory;

    // Assign dir cache memory

    lpstRoot->lpdwDirCacheSlotLRU = (LPDWORD)lpbyMem;
    lpbyMem += lpstRoot->dwNumDirCacheSlots * sizeof(DWORD);

    lpstRoot->lpdwDirCacheSlotBlock = (LPDWORD)lpbyMem;
    lpbyMem += lpstRoot->dwNumDirCacheSlots * sizeof(DWORD);

    lpstRoot->lpbyDirCacheBlocks = lpbyMem;
    lpbyMem += lpstRoot->dwNumDirCacheSlots * SS_SECTOR_SIZE;

    // Assign FAT cache memory

    lpstRoot->lpdwFATCacheSlotLRU = (LPDWORD)lpbyMem;
    lpbyMem += lpstRoot->dwNumFATCacheSlots * sizeof(DWORD);

    lpstRoot->lpdwFATCacheSlotBlock = (LPDWORD)lpbyMem;
    lpbyMem += lpstRoot->dwNumFATCacheSlots * sizeof(DWORD);

    lpstRoot->lpbyFATCacheBlocks = lpbyMem;
    lpbyMem += lpstRoot->dwNumFATCacheSlots * SS_SECTOR_SIZE;

    // Assign mini FAT stream BAT cache memory

    lpstRoot->stMiniFATStream.lpbyCache = lpbyMem;
    lpbyMem += SS_MINI_FAT_BAT_CACHE_SIZE;

    // Assign mini sector stream BAT cache memory

    lpstRoot->stMiniSectorStream.lpbyCache = lpbyMem;
    lpbyMem += SS_MINI_SECTOR_BAT_CACHE_SIZE;

    // Assign dir stream BAT cache memory

    lpstRoot->stDirStream.lpbyCache = lpbyMem;
    lpbyMem += SS_DIR_BAT_CACHE_SIZE;

    /////////////////////////////////////////////////////////////
    // Initialize dir and FAT sector caches
    /////////////////////////////////////////////////////////////

    // Initialize dir sector cache

    lpstRoot->dwDirCacheCurLRU = 1;
    for (dwTemp=0;dwTemp<lpstRoot->dwNumDirCacheSlots;dwTemp++)
    {
        lpstRoot->lpdwDirCacheSlotLRU[dwTemp] = 0;
        lpstRoot->lpdwDirCacheSlotBlock[dwTemp] = 0xFFFFFFFF;
    }

    // Initialize FAT sector cache

    lpstRoot->dwFATCacheCurLRU = 1;
    for (dwTemp=0;dwTemp<lpstRoot->dwNumFATCacheSlots;dwTemp++)
    {
        lpstRoot->lpdwFATCacheSlotLRU[dwTemp] = 0;
        lpstRoot->lpdwFATCacheSlotBlock[dwTemp] = 0xFFFFFFFF;
    }

    /////////////////////////////////////////////////////////////
    // Initialize core streams
    /////////////////////////////////////////////////////////////

    // Initialize dir stream

    lpstRoot->stDirStream.lpstRoot = lpstRoot;
    lpstRoot->stDirStream.dwFlags = 0;
    lpstRoot->stDirStream.dwStartSector =
        DWENDIAN(lpstRoot->stHdr.dwDirStartSector);

    lpstRoot->stDirStream.dwStreamBlk = 0xFFFFFFFF;
    SSInitMacroPacket(lpstRoot->stDirStream.lpbyCache,
                      SS_DIR_BAT_CACHE_SIZE,
                      lpstRoot->stDirStream.dwStartSector);

    // Set the dir stream length to the file size,
    //  it can then later be used to limit the stream lengths

    if (SSFileSeek(lpvRootCookie,
                   lpvFile,
                   0,
                   SS_SEEK_END,
                   (LPLONG)&dwTemp) != SS_STATUS_OK)
    {
        SSMemoryFree(lpvRootCookie,lpstRoot->lpvExtraMemory);
        SSMemoryFree(lpvRootCookie,lpstRoot);
        return(SS_STATUS_ERROR);
    }

    lpstRoot->stDirStream.dwStreamLen = dwTemp;

    // Initialize mini FAT stream

    lpstRoot->stMiniFATStream.lpstRoot = lpstRoot;
    lpstRoot->stMiniFATStream.dwFlags = 0;
    lpstRoot->stMiniFATStream.dwStartSector =
        DWENDIAN(lpstRoot->stHdr.dwMiniFATStartSector);
    lpstRoot->stMiniFATStream.dwStreamLen =
        DWENDIAN(lpstRoot->stHdr.dwMiniFATSectorCount) * SS_SECTOR_SIZE;
    SSLimitStreamLen(&lpstRoot->stMiniFATStream);
    lpstRoot->stMiniFATStream.dwStreamBlk = 0xFFFFFFFF;
    SSInitMacroPacket(lpstRoot->stMiniFATStream.lpbyCache,
                      SS_MINI_FAT_BAT_CACHE_SIZE,
                      lpstRoot->stMiniFATStream.dwStartSector);

    // Read first block of dir stream

    if (SSGetDirBlock(lpstRoot,
                      0,
                      &lpstDirEntry) != SS_STATUS_OK)
    {
        SSMemoryFree(lpvRootCookie,lpstRoot->lpvExtraMemory);
        SSMemoryFree(lpvRootCookie,lpstRoot);
        return(SS_STATUS_ERROR);
    }

    // Initialize mini sector stream
    //  The starting sector of the mini sector stream and the
    //  length of the mini sector stream come from the root
    //  directory entry.

    lpstRoot->stMiniSectorStream.lpstRoot = lpstRoot;
    lpstRoot->stMiniSectorStream.dwFlags = 0;
    lpstRoot->stMiniSectorStream.dwStartSector =
        DWENDIAN(lpstDirEntry->dwStartSector);
    lpstRoot->stMiniSectorStream.dwStreamLen =
        DWENDIAN(lpstDirEntry->dwStreamLen);
    SSLimitStreamLen(&lpstRoot->stMiniSectorStream);
    lpstRoot->stMiniSectorStream.dwStreamBlk = 0xFFFFFFFF;
    SSInitMacroPacket(lpstRoot->stMiniSectorStream.lpbyCache,
                      SS_MINI_SECTOR_BAT_CACHE_SIZE,
                      lpstRoot->stMiniSectorStream.dwStartSector);

    *lplpstRoot = lpstRoot;

    return(SS_STATUS_OK);
}


//********************************************************************
//
// Function:
//  SS_STATUS SSDestroyRoot()
//
// Parameters:
//  lpstRoot            Ptr to root structure
//
// Description:
//  Frees root structure memory.
//
// Returns:
//  SS_STATUS_OK        On success
//  SS_STATUS_ERROR     On error
//
//********************************************************************

SS_STATUS SSDestroyRoot
(
    LPSS_ROOT           lpstRoot
)
{
    // Free extra memory

    if (SSMemoryFree(lpstRoot->lpvRootCookie,
                     lpstRoot->lpvExtraMemory) != SS_STATUS_OK)
    {
        return(SS_STATUS_ERROR);
    }

    // Free main structure memory

    if (SSMemoryFree(lpstRoot->lpvRootCookie,
                     lpstRoot) != SS_STATUS_OK)
    {
        return(SS_STATUS_ERROR);
    }

    return(SS_STATUS_OK);
}


//********************************************************************
//
// Function:
//  SS_STATUS SSAllocStreamStruct()
//
// Parameters:
//  lpstRoot            Ptr to the root structure
//  lplpstStream        Ptr to ptr for storing ptr to created stream
//  dwFlags             Creation flags
//
// Description:
//  The function does the following:
//      1. Allocates memory for the stream structure and
//          conditionally the BAT cache if the flag
//          SS_STREAM_FLAG_DEF_BAT_CACHE is specified.
//      2. Sets the lpstRoot field of the allocated stream structure
//      3. Sets *lplpstStream to point to the allocated structure
//
// Returns:
//  SS_STATUS_OK        On success
//  SS_STATUS_ERROR     On error allocating memory
//
//********************************************************************

SS_STATUS SSAllocStreamStruct
(
    LPSS_ROOT           lpstRoot,
    LPLPSS_STREAM       lplpstStream,
    DWORD               dwFlags
)
{
    LPSS_STREAM         lpstStream;

    if (dwFlags & SS_STREAM_FLAG_DEF_BAT_CACHE)
    {
        if (SSMemoryAlloc(lpstRoot->lpvRootCookie,
                          sizeof(SS_STREAM_T) +
                              SS_DEF_STREAM_BAT_CACHE_SIZE,
                          (LPLPVOID)&lpstStream) != SS_STATUS_OK)
        {
            return(SS_STATUS_ERROR);
        }

        lpstStream->lpbyCache = (LPBYTE)(lpstStream + 1);
    }
    else
    {
        if (SSMemoryAlloc(lpstRoot->lpvRootCookie,
                          sizeof(SS_STREAM_T),
                          (LPLPVOID)&lpstStream) != SS_STATUS_OK)
        {
            return(SS_STATUS_ERROR);
        }

        lpstStream->lpbyCache = NULL;
    }

    lpstStream->lpstRoot = lpstRoot;

    *lplpstStream = lpstStream;

    return(SS_STATUS_OK);
}


//********************************************************************
//
// Function:
//  SS_STATUS SSFreeStreamStruct()
//
// Parameters:
//  lpstStream          Ptr to the stream structure to free
//
// Description:
//  Frees the stream structure memory.
//
// Returns:
//  SS_STATUS_OK        On success
//  SS_STATUS_ERROR     On error
//
//********************************************************************

SS_STATUS SSFreeStreamStruct
(
    LPSS_STREAM         lpstStream
)
{
    // Free the stream structure memory

    if (SSMemoryFree(lpstStream->lpstRoot->lpvRootCookie,
                     lpstStream) != SS_STATUS_OK)
    {
        return(SS_STATUS_ERROR);
    }

    return(SS_STATUS_OK);
}


//********************************************************************
//
// Function:
//  void SSInitOpenStream()
//
// Parameters:
//  lpstStream          Ptr to the stream structure
//  dwID                ID (directory index) of the entry
//  lpstDirEntry        Ptr to the directory entry
//
// Description:
//  Initializes the stream structure for access.  The following
//  fields are initialized:
//
//      - dwSID
//      - dwStartSector
//      - dwStreamLen
//      - dwFlags
//      - dwStreamBlk
//      - lpbyCache contents
//
// Returns:
//  Nothing.
//
//********************************************************************

void SSInitOpenStream
(
    LPSS_STREAM         lpstStream,
    DWORD               dwID,
    LPSS_DIR_ENTRY      lpstDirEntry
)
{
    lpstStream->dwSID = dwID;

    lpstStream->dwStartSector = DWENDIAN(lpstDirEntry->dwStartSector);
    lpstStream->dwStreamLen = DWENDIAN(lpstDirEntry->dwStreamLen);
    SSLimitStreamLen(lpstStream);

    if (DWENDIAN(lpstDirEntry->dwStreamLen) <
        SS_MAX_MINI_SECTOR_STREAM_LEN)
        lpstStream->dwFlags = SS_STREAM_FLAG_MINI_STREAM;
    else
        lpstStream->dwFlags = 0;

    lpstStream->dwStreamBlk = 0xFFFFFFFF;

    if (lpstStream->lpbyCache != NULL)
    {
        SSInitMacroPacket(lpstStream->lpbyCache,
                          SS_DEF_STREAM_BAT_CACHE_SIZE,
                          DWENDIAN(lpstDirEntry->dwStartSector));
    }
}


//********************************************************************
//
// Function:
//  void SSOpenStreamAtIndex()
//
// Parameters:
//  lpstStream          Ptr to the stream structure
//  dwID                ID (directory index) of the entry to open
//
// Description:
//  Initializes the stream structure for access to the stream
//  at the given index.
//
// Returns:
//  SS_STATUS_OK        On success
//  SS_STATUS_ERROR     On error
//
//********************************************************************

SS_STATUS SSOpenStreamAtIndex
(
    LPSS_STREAM         lpstStream,
    DWORD               dwID
)
{
    LPSS_DIR_ENTRY      lpstDirEntry;

    // Read a directory block containing the entry

    if (SSGetDirEntry(lpstStream->lpstRoot,
                      dwID,
                      &lpstDirEntry) != SS_STATUS_OK)
    {
        // Illegal entry

        return(SS_STATUS_ERROR);
    }

    // Prepare the stream structure for access to the stream

    SSInitOpenStream(lpstStream,
                     dwID,
                     lpstDirEntry);

    return(SS_STATUS_OK);
}


//********************************************************************
//
// Function:
//  SS_STATUS SSEnumDirEntriesCB()
//
// Parameters:
//  lpstRoot            Ptr to root structure
//  lpfnEnumCB          The user callback (must be non-NULL)
//  lpvCookie           The user cookie
//  lpdwDirEntryNum     The starting entry number
//  lpstStream          Ptr to created stream structure
//
// Description:
//  Iterates sequentially through the directory entries, calling
//  the callback function for each entry to determine what action
//  to take.
//
//  A given name may appear multiple times in the directory,
//  so *lpdwDirEntryNum is used to continue searching beginning
//  with that index in the directory.
//
//  lpvCookie may be NULL if the callback function never uses it.
//
//  lpstStream may be NULL if the caller does not desire to open
//  the stream.
//
//  lpfnEnumCB must point to a valid callback function.
//
// Returns:
//  SS_STATUS_OK        On success
//  SS_STATUS_ERROR     On error or no more entries
//
//********************************************************************

SS_STATUS SSEnumDirEntriesCB
(
    LPSS_ROOT           lpstRoot,
    LPFNSS_ENUM_CB      lpfnEnumCB,
    LPVOID              lpvCookie,
    LPDWORD             lpdwDirEntryNum,
    LPSS_STREAM         lpstStream
)
{
    DWORD               dwID;
    LPSS_DIR_ENTRY      lpstDirEntry;
    DWORD               dwEntry;
    int                 nStatus;

    dwID = *lpdwDirEntryNum;

    // Limit to 16K entries

    while (dwID < 16384)
    {
        // Read a directory block

        if (SSGetDirBlock(lpstRoot,
                          dwID >> SS_DIR_ENTRIES_SECTOR_SHIFT,
                          &lpstDirEntry) != SS_STATUS_OK)
        {
            // No more entries

            return(SS_STATUS_ERROR);
        }

        // Iterate through the entries in this block

        dwEntry = dwID & SS_DIR_ENTRIES_ENTRY_MASK;
        while (dwEntry < SS_DIR_ENTRIES_PER_SECTOR)
        {
            nStatus = lpfnEnumCB(lpstDirEntry+dwEntry,
                                 dwID,
                                 lpvCookie);

            switch (nStatus)
            {
                case SS_ENUM_CB_STATUS_OPEN:
                    // Initialize stream structure

                    if (lpstStream != NULL)
                    {
                        SSInitOpenStream(lpstStream,
                                         dwID,
                                         lpstDirEntry+dwEntry);
                    }

                    // Update index to next entry

                    *lpdwDirEntryNum = dwID + 1;
                    return(SS_STATUS_OK);

                case SS_ENUM_CB_STATUS_RETURN:
                    // Update index to next entry

                    *lpdwDirEntryNum = dwID + 1;
                    return(SS_STATUS_OK);

                case SS_ENUM_CB_STATUS_CONTINUE:
                    // Just continue

                    ++dwID;
                    ++dwEntry;
                    break;

                default:
                    // Return error

                    return(SS_STATUS_ERROR);
            }
        }
    }

    return(SS_STATUS_ERROR);
}


//********************************************************************
//
// Function:
//  SS_STATUS SAllocEnumSibsStruct()
//
// Parameters:
//  lpstRoot            Ptr to the root structure
//  lplpstEnumSibs      Ptr to ptr for storing ptr to created struct
//  dwMaxWaitingSibs    Maximum number of waiting siblings
//
// Description:
//  The function allocates memory for the base enum sibs structure
//  and the array of DWORDs for the sibling indexes.
//
//  dwMaxWaitingSibs should be at least one.
//
// Returns:
//  SS_STATUS_OK        On success
//  SS_STATUS_ERROR     On error allocating memory
//
//********************************************************************

SS_STATUS SSAllocEnumSibsStruct
(
    LPSS_ROOT           lpstRoot,
    LPLPSS_ENUM_SIBS    lplpstEnumSibs,
    DWORD               dwMaxWaitingSibs
)
{
    LPSS_ENUM_SIBS      lpstEnumSibs;

    if (SSMemoryAlloc(lpstRoot->lpvRootCookie,
                      sizeof(SS_ENUM_SIBS_T) +
                          dwMaxWaitingSibs * sizeof(DWORD),
                      (LPLPVOID)&lpstEnumSibs) != SS_STATUS_OK)
    {
        return(SS_STATUS_ERROR);
    }

    lpstEnumSibs->dwMaxWaitingSibs = dwMaxWaitingSibs;
    lpstEnumSibs->lpdwSibs = (LPDWORD)(lpstEnumSibs + 1);

    *lplpstEnumSibs = lpstEnumSibs;

    return(SS_STATUS_OK);
}


//********************************************************************
//
// Function:
//  SS_STATUS SSFreeEnumSibsStruct()
//
// Parameters:
//  lpstRoot            Ptr to the root structure
//  lpstEnumSibs        Ptr to the enum sibs structure to free
//
// Description:
//  Frees the enum sibs structure memory.
//
// Returns:
//  SS_STATUS_OK        On success
//  SS_STATUS_ERROR     On error
//
//********************************************************************

SS_STATUS SSFreeEnumSibsStruct
(
    LPSS_ROOT           lpstRoot,
    LPSS_ENUM_SIBS      lpstEnumSibs
)
{
    // Free the stream structure memory

    if (SSMemoryFree(lpstRoot->lpvRootCookie,
                     lpstEnumSibs) != SS_STATUS_OK)
    {
        return(SS_STATUS_ERROR);
    }

    return(SS_STATUS_OK);
}


//********************************************************************
//
// Function:
//  void SSInitEnumSibsStruct()
//
// Parameters:
//  lpstEnumSibs        Ptr to the enum sibs structure
//  dwRootID            The root sibling entry
//
// Description:
//  Initializes the enum sibs structure for use by setting the
//  number of waiting siblings to one and setting the given root
//  entry as the first sibling.
//
// Returns:
//  Nothing.
//
//********************************************************************

void SSInitEnumSibsStruct
(
    LPSS_ENUM_SIBS      lpstEnumSibs,
    DWORD               dwRootID
)
{
    lpstEnumSibs->dwNumWaitingSibs = 1;
    lpstEnumSibs->lpdwSibs[0] = dwRootID;
}


//********************************************************************
//
// Function:
//  SS_STATUS SSEnumSiblingEntriesCB()
//
// Parameters:
//  lpstRoot            Ptr to root structure
//  lpfnEnumCB          The user callback (must be non-NULL)
//  lpvCookie           The user cookie
//  lpstEnumSibs        Ptr to the enum sibs structure
//  lpstStream          Ptr to created stream structure
//
// Description:
//  Iterates breadth first through the siblings, calling
//  the callback function for each entry to determine what action
//  to take.
//
//  lpstEnumSibs is used to hold the state of the search.
//
//  lpvCookie may be NULL if the callback function never uses it.
//
//  lpstStream may be NULL if the caller does not desire to open
//  the stream.
//
//  lpfnEnumCB must point to a valid callback function.
//
// Returns:
//  SS_STATUS_OK        On success
//  SS_STATUS_ERROR     On error or no more entries
//
//********************************************************************

SS_STATUS SSEnumSiblingEntriesCB
(
    LPSS_ROOT           lpstRoot,
    LPFNSS_ENUM_CB      lpfnEnumCB,
    LPVOID              lpvCookie,
    LPSS_ENUM_SIBS      lpstEnumSibs,
    LPSS_STREAM         lpstStream
)
{
    DWORD               dwID;
    LPSS_DIR_ENTRY      lpstDirEntry;
    int                 nStatus;
    int                 nIterationCount;

    nIterationCount = 0;

    // Limit number of iterations to 16K

    while (nIterationCount++ < 16384 &&
        lpstEnumSibs->dwNumWaitingSibs != 0)
	{
        // Get a sibling from the stack

        dwID = lpstEnumSibs->
            lpdwSibs[--(lpstEnumSibs->dwNumWaitingSibs)];

        // Get the directory entry

        if (SSGetDirEntry(lpstRoot,
                          dwID,
                          &lpstDirEntry) != SS_STATUS_OK)
        {
            // No such entry

            return(SS_STATUS_ERROR);
        }

        // Store the siblings, pushing right, then left

        if (DWENDIAN(lpstDirEntry->dwSIDRightSib) != 0xFFFFFFFF)
        {
            lpstEnumSibs->lpdwSibs[lpstEnumSibs->dwNumWaitingSibs++] =
                DWENDIAN(lpstDirEntry->dwSIDRightSib);
        }

        if (DWENDIAN(lpstDirEntry->dwSIDLeftSib) != 0xFFFFFFFF)
        {
            if (lpstEnumSibs->dwNumWaitingSibs <
                lpstEnumSibs->dwMaxWaitingSibs)
            {
                lpstEnumSibs->lpdwSibs[lpstEnumSibs->dwNumWaitingSibs++] =
                    DWENDIAN(lpstDirEntry->dwSIDLeftSib);
            }
        }

        // Call the callback function to determine what to do

        nStatus = lpfnEnumCB(lpstDirEntry,
                             dwID,
                             lpvCookie);

        switch (nStatus)
        {
            case SS_ENUM_CB_STATUS_OPEN:
                // Initialize stream structure

                if (lpstStream != NULL)
                {
                    SSInitOpenStream(lpstStream,
                                     dwID,
                                     lpstDirEntry);
                }

                return(SS_STATUS_OK);

            case SS_ENUM_CB_STATUS_RETURN:
                return(SS_STATUS_OK);

            case SS_ENUM_CB_STATUS_CONTINUE:
                // Just continue

                break;

            default:
                // Return error

                return(SS_STATUS_ERROR);
        }
	}

    return(SS_STATUS_ERROR);
}


//********************************************************************
//
// Function:
//  SS_STATUS SSGetChildOfStorage()
//
// Parameters:
//  lpstRoot            Ptr to root structure
//  dwStorageID         Directory ID of the desired storage
//  lpdwChildID         ID of the root child of the storage
//
// Description:
//  Gets the child of the storage at the given entry number.
//
//  If the return value is SS_STATUS_OK, *lpwChildID is set
//  to the entry number of the child.
//
// Returns:
//  SS_STATUS_OK        On success
//  SS_STATUS_ERROR     On error
//
//********************************************************************

SS_STATUS SSGetChildOfStorage
(
    LPSS_ROOT           lpstRoot,
    DWORD               dwStorageID,
    LPDWORD             lpdwChildID
)
{
    LPSS_DIR_ENTRY      lpstDirEntry;

    if (SSGetDirEntry(lpstRoot,
                      dwStorageID,
                      &lpstDirEntry) != SS_STATUS_OK)
    {
        // No such entry

        return(SS_STATUS_ERROR);
    }

    *lpdwChildID = DWENDIAN(lpstDirEntry->dwSIDChild);

    return(SS_STATUS_OK);
}


//********************************************************************
//
// Function:
//  int SSFindStorageGetRootChildCB()
//
// Parameters:
//  lpstEntry           Ptr to current entry
//  dwIndex             Index of current entry
//  lpvCookie           Ptr to DWORD to store child of storage
//
// Description:
//  Searches for a storage and stores the child of the storage at
//  the DWORD pointed to by lpvCookie.
//
//  The root counts as a storage.
//
// Returns:
//  SS_ENUM_CB_STATUS_RETURN        If it is a storage
//  SS_ENUM_CB_STATUS_CONTINUE      If it is not a storage
//
//********************************************************************

int SSFindStorageGetRootChildCB
(
    LPSS_DIR_ENTRY      lpstEntry,
    DWORD               dwIndex,
    LPVOID              lpvCookie
)
{
    (void)dwIndex;

    if (lpstEntry->byMSE == STGTY_STORAGE ||
        lpstEntry->byMSE == STGTY_ROOT)
    {
        // Found a storage

        // Store the child

        *(LPDWORD)lpvCookie = DWENDIAN(lpstEntry->dwSIDChild);
        return(SS_ENUM_CB_STATUS_RETURN);
    }

    return(SS_ENUM_CB_STATUS_CONTINUE);
}


//********************************************************************
//
// Function:
//  int SSMatchEntryIDCB()
//
// Parameters:
//  lpstEntry           Ptr to current entry
//  dwIndex             Index of current entry
//  lpvCookie           Ptr to DWORD entry ID to match
//
// Description:
//  Searches for an entry with the given ID.
//
// Returns:
//  SS_ENUM_CB_STATUS_RETURN        If the entry ID matches
//  SS_ENUM_CB_STATUS_CONTINUE      If the entry ID does not match
//
//********************************************************************

int SSMatchEntryIDCB
(
    LPSS_DIR_ENTRY      lpstEntry,
    DWORD               dwIndex,
    LPVOID              lpvCookie
)
{
    (void)lpstEntry;

    if (dwIndex == *(LPDWORD)lpvCookie)
    {
        // Found a match

        return(SS_ENUM_CB_STATUS_RETURN);
    }

    return(SS_ENUM_CB_STATUS_CONTINUE);
}


//********************************************************************
//
// Function:
//  SS_STATUS SSGetParentStorage()
//
// Parameters:
//  lpstRoot            Ptr to root structure
//  dwEntryID           Entry ID of which to get the parent
//  lpdwParentID        ID of the parent of the storage
//  lpdwChildID         ID of the root child of the parent storage
//
// Description:
//  Gets the parent and root child of the given entry number.
//
//  Either of lpdwParentID or lpdwChildID may be NULL if the
//  ID is not desired.
//
//  If the return value is SS_STATUS_OK, *lpdwParentID is set
//  to the entry number of the parent storage and *lpwChildID is
//  set to the entry number of the child of the parent storage.
//
// Returns:
//  SS_STATUS_OK        On success
//  SS_STATUS_ERROR     On error
//
//********************************************************************

SS_STATUS SSGetParentStorage
(
    LPSS_ROOT           lpstRoot,
    DWORD               dwEntryID,
    LPDWORD             lpdwParentID,
    LPDWORD             lpdwChildID
)
{
    LPSS_ENUM_SIBS      lpstSibs;
    DWORD               dwSearchID;
    DWORD               dwChildID;

    // Allocate a sibling enumeration structure

    if (SSAllocEnumSibsStruct(lpstRoot,
                              &lpstSibs,
                              SS_DEF_MAX_WAITING_SIBS) != SS_STATUS_OK)
    {
        return(SS_STATUS_ERROR);
    }

    // Go through all storages

    dwSearchID = 0;
    while (SSEnumDirEntriesCB(lpstRoot,
                              SSFindStorageGetRootChildCB,
                              &dwChildID,
                              &dwSearchID,
                              NULL) == SS_STATUS_OK)
    {
        // Found a storage, so go through children looking
        //  for what we need

        SSInitEnumSibsStruct(lpstSibs,
                             dwChildID);

        if (SSEnumSiblingEntriesCB(lpstRoot,
                                   SSMatchEntryIDCB,
                                   &dwEntryID,
                                   lpstSibs,
                                   NULL) == SS_STATUS_OK)
        {
            // Found it

            if (lpdwParentID != NULL)
                *lpdwParentID = dwSearchID - 1;

            if (lpdwChildID != NULL)
                *lpdwChildID = dwChildID;

            // Free the enumeration structure

            if (SSFreeEnumSibsStruct(lpstRoot,lpstSibs) != SS_STATUS_OK)
                return(SS_STATUS_ERROR);

            return(SS_STATUS_OK);
        }
    }

    // Free the enumeration structure

    SSFreeEnumSibsStruct(lpstRoot,lpstSibs);

    // Couldn't find the parent storage

    return(SS_STATUS_ERROR);
}


//********************************************************************
//
// Function:
//  SS_STATUS SSUnlinkEntry()
//
// Parameters:
//  lpstRoot            Ptr to root structure
//  dwStorageID         Directory ID of the storage containing
//                          the entry to unlink
//  dwUnlinkEntryID     ID of the entry to unlink
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
//  SS_STATUS_OK        On success
//  SS_STATUS_ERROR     On error
//
//********************************************************************

SS_STATUS SSUnlinkEntry
(
    LPSS_ROOT           lpstRoot,
    DWORD               dwParentStorageID,
    DWORD               dwUnlinkEntryID
)
{
    DWORD               dwParentStorageChildID;
    int                 i, nNumWaitingSibs;
    DWORD               dwSibs[SS_DEF_MAX_WAITING_SIBS];
    DWORD               dwParentID;
    DWORD               dwCurEntryID;
    LPSS_DIR_ENTRY      lpstDirEntry;
    DWORD               dwReplacementParentID;
    DWORD               dwReplacementEntryID;
    DWORD               dwReplacementEntryRightSib;
    DWORD               dwUnlinkEntryLeftSib;
    DWORD               dwUnlinkEntryRightSib;
    DWORD               dwLeftSibling;
    DWORD               dwRightSibling;

    // Get the child of the parent storage

    if (SSGetChildOfStorage(lpstRoot,
                            dwParentStorageID,
                            &dwParentStorageChildID) != SS_STATUS_OK)
    {
        return(SS_STATUS_ERROR);
    }


    /////////////////////////////////////////////////////////////
    // Search through the siblings to find the following:
    //  - The parent of the entry to unlink
    /////////////////////////////////////////////////////////////

    dwParentID = 0xFFFFFFFF;

    nNumWaitingSibs = 1;
    dwSibs[0] = dwParentStorageChildID;
    while (nNumWaitingSibs > 0)
	{
        // Get a sibling from the stack

        dwCurEntryID = dwSibs[--nNumWaitingSibs];
        if (dwCurEntryID == dwUnlinkEntryID)
        {
            // Found the entry to unlink
            //  Must be the child of the parent storage

            break;
        }

        // Read the entry

        if (SSGetDirEntry(lpstRoot,
                          dwCurEntryID,
                          &lpstDirEntry) != SS_STATUS_OK)
        {
            // No such entry

            return(SS_STATUS_ERROR);
        }

        // Check the left sibling

        dwLeftSibling = DWENDIAN(lpstDirEntry->dwSIDLeftSib);

        if (dwLeftSibling != 0xFFFFFFFF)
        {
            // Is this the parent of the entry to unlink?

            if (dwLeftSibling == dwUnlinkEntryID)
            {
                // This is the parent of the entry to unlink

                dwParentID = dwCurEntryID;
                break;
            }

            // No need to check if there is room for another
            //  sibling on the stack  because the parent was
            //  just removed.

            dwSibs[nNumWaitingSibs++] = dwLeftSibling;
        }

        // Check the right sibling

        dwRightSibling = DWENDIAN(lpstDirEntry->dwSIDRightSib);

        if (dwRightSibling != 0xFFFFFFFF)
        {
            // Is this the parent of the entry to unlink?

            if (dwRightSibling == dwUnlinkEntryID)
            {
                // This is the parent of the entry to unlink

                dwParentID = dwCurEntryID;
                break;
            }

            if (nNumWaitingSibs >= SS_DEF_MAX_WAITING_SIBS)
            {
                // Out of room to trace another sibling

                return(SS_STATUS_ERROR);
            }

            // Add the sibling to the list

            dwSibs[nNumWaitingSibs++] = dwRightSibling;
        }
    }


    /////////////////////////////////////////////////////////////
    // Find the replacement entry
    /////////////////////////////////////////////////////////////

    // Get the sibling entries of the entry to unlink

    if (SSGetDirEntry(lpstRoot,
                      dwUnlinkEntryID,
                      &lpstDirEntry) != SS_STATUS_OK)
    {
        return(SS_STATUS_ERROR);
    }

    dwUnlinkEntryLeftSib = DWENDIAN(lpstDirEntry->dwSIDLeftSib);
    dwUnlinkEntryRightSib = DWENDIAN(lpstDirEntry->dwSIDRightSib);

    if (dwUnlinkEntryLeftSib != 0xFFFFFFFF &&
        dwUnlinkEntryRightSib != 0xFFFFFFFF)
    {
        // Replace with the leftmost child of the right sibling

        // Prevent an infinite loop

        dwReplacementParentID = dwUnlinkEntryID;
        dwReplacementEntryID = dwUnlinkEntryRightSib;
        for (i=0;i<4096;i++)
        {
            // Get the entry of the candidate replacement ID

            if (SSGetDirEntry(lpstRoot,
                              dwReplacementEntryID,
                              &lpstDirEntry) != SS_STATUS_OK)
            {
                return(SS_STATUS_ERROR);
            }

            dwLeftSibling = DWENDIAN(lpstDirEntry->dwSIDLeftSib);

            if (dwLeftSibling == 0xFFFFFFFF)
            {
                // Found the left most one

                dwReplacementEntryRightSib =
                    DWENDIAN(lpstDirEntry->dwSIDRightSib);
                break;
            }

            dwReplacementParentID = dwReplacementEntryID;
            dwReplacementEntryID = dwLeftSibling;
        }

        if (i == 4096)
        {
            // Deeper than 4096 levels!?

            return(SS_STATUS_ERROR);
        }

        // Remove the leftmost replacement entry from the parent
        //  if the parent is not the entry to unlink

        if (dwReplacementParentID != dwUnlinkEntryID)
        {
            // Get the replacement entry's parent entry

            if (SSGetDirEntry(lpstRoot,
                              dwReplacementParentID,
                              &lpstDirEntry) != SS_STATUS_OK)
            {
                return(SS_STATUS_ERROR);
            }

            // Set the parent's left sibling to point to the
            //  replacement entry's right sibling

            lpstDirEntry->dwSIDLeftSib =
                DWENDIAN(dwReplacementEntryRightSib);

            // Write out the updated parent of the leaf

            if (SSUpdateDirEntry(lpstRoot,
                                 dwReplacementParentID) != SS_STATUS_OK)
            {
                return(SS_STATUS_ERROR);
            }
        }

        // Get the replacement entry

        if (SSGetDirEntry(lpstRoot,
                          dwReplacementEntryID,
                          &lpstDirEntry) != SS_STATUS_OK)
        {
            return(SS_STATUS_ERROR);
        }

        // Set the replacement entry's left sibling to point
        //  to the unlink entry's left sibling

        lpstDirEntry->dwSIDLeftSib =
            DWENDIAN(dwUnlinkEntryLeftSib);

        if (dwReplacementParentID != dwUnlinkEntryID)
        {
            // Set the replacement entry's right sibling to point
            //  to the entry to unlink's right sibling
            //  if the parent of the replacement entry is not the
            //  entry to unlink

            lpstDirEntry->dwSIDRightSib =
                DWENDIAN(dwUnlinkEntryRightSib);
        }

        // Write out the updated replacement entry

        if (SSUpdateDirEntry(lpstRoot,
                             dwReplacementEntryID) != SS_STATUS_OK)
        {
            return(SS_STATUS_ERROR);
        }
    }
    else
    {
        if (dwUnlinkEntryLeftSib != 0xFFFFFFFF)
        {
            // Replace with the left sibling

            dwReplacementEntryID = dwUnlinkEntryLeftSib;
        }
        else
        if (dwUnlinkEntryRightSib != 0xFFFFFFFF)
        {
            // Replace with the right sibling

            dwReplacementEntryID = dwUnlinkEntryRightSib;
        }
        else
        {
            // Just delete the entry

            dwReplacementEntryID = 0xFFFFFFFF;
        }
    }

    /////////////////////////////////////////////////////////////
    // Update the parent of the entry to unlink
    /////////////////////////////////////////////////////////////

    if (dwParentID == 0xFFFFFFFF)
    {
        // No parent, meaning that the parent of the node
        //  to unlink is the parent storage

        if (SSGetDirEntry(lpstRoot,
                          dwParentStorageID,
                          &lpstDirEntry) != SS_STATUS_OK)
        {
            return(SS_STATUS_ERROR);
        }

        lpstDirEntry->dwSIDChild = DWENDIAN(dwReplacementEntryID);

        // Write out the updated replacement entry

        if (SSUpdateDirEntry(lpstRoot,
                             dwParentStorageID) != SS_STATUS_OK)
        {
            return(SS_STATUS_ERROR);
        }
    }
    else
    {
        // Determine whether it is the left or right sibling
        //  of the parent of the node to unlink that needs
        //  to change

        if (SSGetDirEntry(lpstRoot,
                          dwParentID,
                          &lpstDirEntry) != SS_STATUS_OK)
        {
            return(SS_STATUS_ERROR);
        }

        dwLeftSibling = DWENDIAN(lpstDirEntry->dwSIDLeftSib);
        if (dwLeftSibling == dwUnlinkEntryID)
        {
            // It is the left sibling

            lpstDirEntry->dwSIDLeftSib =
                DWENDIAN(dwReplacementEntryID);
        }
        else
        {
            // It must be the right sibling

            lpstDirEntry->dwSIDRightSib =
                DWENDIAN(dwReplacementEntryID);
        }

        // Write out the new parent of the entry to unlink

        if (SSUpdateDirEntry(lpstRoot,
                             dwParentID) != SS_STATUS_OK)
        {
            return(SS_STATUS_ERROR);
        }
    }

    // Get the entry to unlink

    if (SSGetDirEntry(lpstRoot,
                      dwUnlinkEntryID,
                      &lpstDirEntry) != SS_STATUS_OK)
    {
        return(SS_STATUS_ERROR);
    }

    // Now zero out the name and invalidate the sibling and child
    //  pointers

    for (i=0;i<SS_MAX_NAME_LEN;i++)
        lpstDirEntry->uszName[i] = 0;

    lpstDirEntry->dwSIDLeftSib = 0xFFFFFFFF;
    lpstDirEntry->dwSIDRightSib = 0xFFFFFFFF;
    lpstDirEntry->dwSIDChild = 0xFFFFFFFF;

    // Invalidate the type

    lpstDirEntry->byMSE = 0;

    // Update the node to unlink

    if (SSUpdateDirEntry(lpstRoot,
                         dwUnlinkEntryID) != SS_STATUS_OK)
    {
        return(SS_STATUS_ERROR);
    }

    // Success

    return(SS_STATUS_OK);
}


//********************************************************************
//
// Function:
//  SS_STATUS SSSetStreamLen()
//
// Parameters:
//  lpstRoot            Ptr to root structure
//  dwEntryID           ID of the entry to change
//  dwNewLen            New length of the stream
//
// Description:
//  Modifies the stream length in the directory entry with the
//  given ID.
//
// Returns:
//  SS_STATUS_OK        On success
//  SS_STATUS_ERROR     On error
//
//********************************************************************

SS_STATUS SSSetStreamLen
(
    LPSS_ROOT           lpstRoot,
    DWORD               dwEntryID,
    DWORD               dwNewLen
)
{
    LPSS_DIR_ENTRY      lpstDirEntry;

    // Get the entry to change

    if (SSGetDirEntry(lpstRoot,
                      dwEntryID,
                      &lpstDirEntry) != SS_STATUS_OK)
    {
        return(SS_STATUS_ERROR);
    }

    // Set the stream length

    lpstDirEntry->dwStreamLen = DWENDIAN(dwNewLen);

    // Update the entry

    if (SSUpdateDirEntry(lpstRoot,
                         dwEntryID) != SS_STATUS_OK)
    {
        return(SS_STATUS_ERROR);
    }

    // Success

    return(SS_STATUS_OK);
}


//********************************************************************
//
// Function:
//  int SSWStrNCmp()
//
// Parameters:
//  lpwStrA             Ptr to first wide character string
//  lpwStrB             Ptr to second wide character string
//  wMaxLen             Maximum number of characters to compare
//
// Description:
//  Compares up to wMaxLen wide characters.
//
// Returns:
//  0                   If A is the same as B
//  -1                  If A comes before B
//  1                   If A goes after B
//
//********************************************************************

int SSWStrNCmp
(
    LPWORD          lpwStrA,
    LPWORD          lpwStrB,
    WORD            wMaxLen
)
{
    WORD            wA;
    WORD            wB;

    while (wMaxLen-- != 0)
    {
        wA = *lpwStrA++;
        wB = *lpwStrB++;

        wA = WENDIAN(wA);
        wB = WENDIAN(wB);

        if (wA == 0 && wB == 0)
            return(0);

        if (wA < wB)
            return(-1);

        if (wA > wB)
            return(1);
    }

    return(0);
}


//********************************************************************
//
// Function:
//  int SSOpenStreamByNameCB()
//
// Parameters:
//  lpstEntry               Ptr to the entry
//  dwIndex                 The entry's index in the directory
//  lpvCookie               Ptr to the buffer containing the
//                              wide character name to match
//
// Description:
//  The cookie is assumed to point to a wide character string
//  for comparison against the names of streams.
//
// Returns:
//  SS_ENUM_CB_STATUS_OPEN          If the names match
//  SS_ENUM_CB_STATUS_CONTINUE      Always
//
//********************************************************************

int SSOpenStreamByNameCB
(
    LPSS_DIR_ENTRY      lpstEntry,
    DWORD               dwIndex,
    LPVOID              lpvCookie
)
{
    (void)dwIndex;

    if (lpstEntry->byMSE == STGTY_STREAM)
    {
        // Is it the desired stream?

        if (SSWStrNCmp(lpstEntry->uszName,
                       (LPWORD)lpvCookie,
                       SS_MAX_NAME_LEN) == 0)
        {
            return(SS_ENUM_CB_STATUS_OPEN);
        }
    }

    return(SS_ENUM_CB_STATUS_CONTINUE);
}


//********************************************************************
//
// Function:
//  BYTE SSToLower()
//
// Parameters:
//  byChar              Character to convert
//
// Description:
//  Converts the given character to lowercase.  The function
//  converts 'A' through 'Z' to 'a' through 'z'.  The function
//  assumes the Windows character set for the following ranges:
//
//      Upper char      Lower char
//      ----------      ----------
//      C0-D6           E0-F6
//      D8-DE           F8-FE
//      8A              9A
//      8C              9C
//      9F              FF
//
// Returns:
//  BYTE                Lowercased character
//
//********************************************************************

BYTE SSToLower
(
    BYTE                byChar
)
{
    if ('A' <= byChar && byChar <= 'Z')
        return(byChar + ('a' - 'A'));

    if (0xC0 <= byChar && byChar <= 0xD6)
        return(byChar + (0xE0 - 0xC0));

    if (0xD8 <= byChar && byChar <= 0xDE)
        return(byChar + (0xF8 - 0xD8));

    if (byChar == 0x8A)
        return(0x9A);

    if (byChar == 0x8C)
        return(0x9C);

    if (byChar == 0x9F)
        return(0xFF);

    return(byChar);
}


//********************************************************************
//
// Function:
//  BYTE SSToUpper()
//
// Parameters:
//  byChar              Character to convert
//
// Description:
//  Converts the given character to uppercase.  The function
//  converts 'a' through 'z' to 'A' through 'Z'.  The function
//  assumes the Windows character set for the following ranges:
//
//      Upper char      Lower char
//      ----------      ----------
//      C0-D6           E0-F6
//      D8-DE           F8-FE
//      8A              9A
//      8C              9C
//      9F              FF
//
// Returns:
//  BYTE                Uppercased character
//
//********************************************************************

BYTE SSToUpper
(
    BYTE                byChar
)
{
    if ('a' <= byChar && byChar <= 'z')
        return(byChar - 'a' + 'A');

    if (0xE0 <= byChar && byChar <= 0xF6)
        return(byChar - 0xE0 + 0xC0);

    if (0xF8 <= byChar && byChar <= 0xFE)
        return(byChar - 0xF8 + 0xD8);

    if (byChar == 0x9A)
        return(0x8A);

    if (byChar == 0x9C)
        return(0x8C);

    if (byChar == 0xFF)
        return(0x9F);

    return(byChar);
}


//********************************************************************
//
// Function:
//  int SSGetNamedSiblingCB()
//
// Parameters:
//  lpstEntry               Ptr to the entry
//  dwIndex                 The entry's index in the directory
//  lpvCookie               Ptr to SS_NAMED_SIB_T structure
//
// Description:
//  The cookie is assumed to point to a SS_NAMED_SIB_T structure
//  for receiving the index of the found sibling.
//
// Returns:
//  SS_ENUM_CB_STATUS_RETURN        If the sibling is found
//  SS_ENUM_CB_STATUS_CONTINUE      If the sibling not found
//
//********************************************************************

typedef struct tagSS_NAMED_SIB
{
    LPBYTE              lpabywszSiblingName;
    DWORD               dwSiblingID;
} SS_NAMED_SIB_T, FAR *LPSS_NAMED_SIB;

int SSGetNamedSiblingCB
(
    LPSS_DIR_ENTRY      lpstEntry,
    DWORD               dwIndex,
    LPVOID              lpvCookie
)
{
    // Is it the desired entry?

    if (SSWStrNCmp(lpstEntry->uszName,
                   (LPWORD)((LPSS_NAMED_SIB)lpvCookie)->lpabywszSiblingName,
                   SS_MAX_NAME_LEN) == 0)
    {
        ((LPSS_NAMED_SIB)lpvCookie)->dwSiblingID = dwIndex;
        return(SS_ENUM_CB_STATUS_RETURN);
    }

    return(SS_ENUM_CB_STATUS_CONTINUE);
}


//********************************************************************
//
// Function:
//  BOOL SSGetNamedSiblingID()
//
// Parameters:
//  lpstRoot                Ptr to root structure for storage
//  dwDocStreamID           Doc stream ID of info streams to search
//  lpabywszSiblingName     Name of sibling to search for
//  lpdwSiblingID           Ptr to DWORD for sibling ID
//
// Description:
//  Given the ID of an entry, the function searches for a sibling
//  of the given entry with the given name.
//
//  If a sibling with the given name is found, the function sets
//  *lpdwSiblingID to the ID of the sibling and returns TRUE,
//  otherwise, the function sets *lpdwSiblingID to zero and
//  returns FALSE.
//
// Returns:
//  TRUE                    On success
//  FALSE                   On error
//
//********************************************************************

BOOL SSGetNamedSiblingID
(
    LPSS_ROOT           lpstRoot,
    DWORD               dwDocStreamID,
    LPBYTE              lpabywszSiblingName,
    LPDWORD             lpdwSiblingID
)
{
    LPSS_ENUM_SIBS      lpstSibs;
    DWORD               dwParentID;
    DWORD               dwChildID;
    SS_NAMED_SIB_T      stNamedSib;

    // Get the parent and child of the parent of the document stream

    if (SSGetParentStorage(lpstRoot,
                           dwDocStreamID,
                           &dwParentID,
                           &dwChildID) != SS_STATUS_OK)
        return(FALSE);

    // Allocate a sibling enumeration structure

    if (SSAllocEnumSibsStruct(lpstRoot,
                              &lpstSibs,
                              SS_DEF_MAX_WAITING_SIBS) != SS_STATUS_OK)
        return(FALSE);

    SSInitEnumSibsStruct(lpstSibs,dwChildID);

    // Initialize to not found

    stNamedSib.lpabywszSiblingName = lpabywszSiblingName;
    stNamedSib.dwSiblingID = 0;

    // Find the sibling

    SSEnumSiblingEntriesCB(lpstRoot,
                           SSGetNamedSiblingCB,
                           &stNamedSib,
                           lpstSibs,
                           NULL);

    // Free the enumeration structure

    SSFreeEnumSibsStruct(lpstRoot,lpstSibs);

    *lpdwSiblingID = stNamedSib.dwSiblingID;
    if (stNamedSib.dwSiblingID == 0)
        return(FALSE);

    return(TRUE);
}


//********************************************************************
//
// Function:
//  DWORD SSEndianDWORD()
//
// Parameters:
//  lpabyDWORD  Ptr to value to convert
//  bLitEnd     If the value is in little endian format
//
// Description:
//  Given an array of four bytes representing a DWORD value
//  from a binary stream and a boolean value specifying whether
//  the value is in little endian or big endian format, the
//  function returns the value of the correctly converted DWORD.
//
// Returns:
//  DWORD       The converted DWORD value
//
//********************************************************************

DWORD SSEndianDWORD
(
    LPBYTE      lpabyDWORD,
    BOOL        bLitEnd
)
{
    if (bLitEnd == FALSE)
    {
        return((((DWORD)lpabyDWORD[0]) << 24) |
               (((DWORD)lpabyDWORD[1]) << 16) |
               (((DWORD)lpabyDWORD[2]) << 8) |
               ((DWORD)lpabyDWORD[3]));
    }
    else
    {
        return(((DWORD)lpabyDWORD[0]) |
               (((DWORD)lpabyDWORD[1]) << 8) |
               (((DWORD)lpabyDWORD[2]) << 16) |
               (((DWORD)lpabyDWORD[3]) << 24));
    }
}


//********************************************************************
//
// Function:
//  WORD SSEndianWORD()
//
// Parameters:
//  lpabyWORD   Ptr to WORD value to convert
//  bLitEnd     If the value is in little endian format
//
// Description:
//  Given an array of two bytes representing a WORD value
//  from a binary stream and a boolean value specifying whether
//  the value is in little endian or big endian format, the
//  function returns the value of the correctly converted WORD.
//
// Returns:
//  WORD        The converted WORD value
//
//********************************************************************

WORD SSEndianWORD
(
    LPBYTE      lpabyWORD,
    BOOL        bLitEnd
)
{
    if (bLitEnd == FALSE)
    {
        return((((WORD)lpabyWORD[0]) << 8) |
               ((WORD)lpabyWORD[1]));
    }
    else
    {
        return(((WORD)lpabyWORD[0]) |
               (((WORD)lpabyWORD[1]) << 8));
    }
}


//********************************************************************
//
// Function:
//  BOOL SSGetStreamIndexCB()
//
// Parameters:
//  lpstRoot            Ptr to root structure
//  lpfnEnumCB          Ptr to function for stream filtering
//  dwChildEntry        Entry number of storage child
//  lpabypsName         Ptr to module name to search
//  lpdwIndex           Ptr to DWORD for index of module
//
// Description:
//  The function iterates through the storage beginning with the
//  given child entry, looking for a stream with the given name.
//  The function returns in *lpdwIndex the order of the stream
//  in the siblings within the storage.
//
//  The callback function is used as a filter for the files that
//  should be counted in the list and should return the name in
//  the cookie as is and zero-terminate it.  The name, not including
//  the zero-terminator, should not exceed SS_MAX_NAME_LEN bytes.
//
//  The name match is case insensitive.
//
// Returns:
//  TRUE                On success
//  FALSE               On error
//
//********************************************************************

BOOL SSGetStreamIndexCB
(
    LPSS_ROOT           lpstRoot,
    LPFNSS_ENUM_CB      lpfnEnumCB,
    DWORD               dwChildEntry,
    LPBYTE              lpabypsName,
    LPDWORD             lpdwIndex
)
{
    LPSS_ENUM_SIBS      lpstSibs;
    DWORD               dwCurIndex;
    BYTE                abyszName[SS_MAX_NAME_LEN + 1];
    int                 i;
    int                 nNameLen;

    // Allocate a sibiling enumeration structure

    if (SSAllocEnumSibsStruct(lpstRoot,
                              &lpstSibs,
                              SS_DEF_MAX_WAITING_SIBS) != SS_STATUS_OK)
        return(FALSE);

    SSInitEnumSibsStruct(lpstSibs,
                         dwChildEntry);

    nNameLen = *lpabypsName++;
    for (dwCurIndex=0;dwCurIndex<16384;dwCurIndex++)
    {
        if (SSEnumSiblingEntriesCB(lpstRoot,
                                   lpfnEnumCB,
                                   abyszName,
                                   lpstSibs,
                                   NULL) != SS_STATUS_OK)
            break;

        // Compare the name

        for (i=0;i<nNameLen && abyszName[i];i++)
        {
            if (SSToLower(lpabypsName[i]) !=
                SSToLower(abyszName[i]))
                break;
        }

        if (i == nNameLen)
        {
            if (SSFreeEnumSibsStruct(lpstRoot,
                                     lpstSibs) != SS_STATUS_OK)
                return(FALSE);

            *lpdwIndex = dwCurIndex;
            return(TRUE);
        }
    }

    SSFreeEnumSibsStruct(lpstRoot,lpstSibs);

    return(FALSE);
}



