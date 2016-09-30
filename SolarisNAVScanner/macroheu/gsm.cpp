//************************************************************************
//
// $Header:   S:/MACROHEU/VCS/GSM.CPv   1.0   30 Jun 1997 16:15:34   DCHI  $
//
// Description:
//  Macro emulation memory management functions.
//
//************************************************************************
// $Log:   S:/MACROHEU/VCS/GSM.CPv  $
// 
//    Rev 1.0   30 Jun 1997 16:15:34   DCHI
// Initial revision.
// 
//************************************************************************

#include <assert.h>

#include "olessapi.h"
#include "gsm.h"

//*************************************************************************
//
// BOOL GSMCreate()
//
// Parameters:
//  dwSizeBytes     Bytes to allocate for global shared memory
//
// Description:
//  Initializes the global shared memory structure for global shared
//  memory allocation.
//
//  The function allocates approximately dwSizeBytes rounded up to the
//  next GSM_HDR_T size plus room for another GSM_HDR_T.
//  The extra header is used to keep track of the
//  free list and is located at the beginning of the allocated memory.
//  It is initialized to point to a single free block containing the
//  rest of the allocated memory.
//
// Returns:
//  LPGSM_HDR       On success
//  NULL            On error
//
//*************************************************************************

LPGSM_HDR GSMCreate
(
    LPVOID          lpvRootCookie,
    DWORD           dwSizeBytes
)
{
    LPGSM_HDR       lpstHdr;
    DWORD           dwNumUnitsToAlloc;

    // Appx. round up to next multiple of HDR units plus one

    dwNumUnitsToAlloc = (dwSizeBytes / sizeof(GSM_HDR_T)) + 2;

    // Allocate global shared memory

    if (SSMemoryAlloc(lpvRootCookie,
                      dwNumUnitsToAlloc * sizeof(GSM_HDR_T),
                      (LPLPVOID)&lpstHdr) != SS_STATUS_OK)
        return(NULL);

    // Initialize with the single free block

    lpstHdr->lpstNextBlk = lpstHdr + 1;
    lpstHdr->dwSizeHdrUnits = 0;

    (lpstHdr + 1)->lpstNextBlk = NULL;
    (lpstHdr + 1)->dwSizeHdrUnits = dwNumUnitsToAlloc - 1;

    return(lpstHdr);
}


//*************************************************************************
//
// BOOL GSMDestroy()
//
// Parameters:
//  lpstHdr         Ptr to initialize GSM structure
//
// Description:
//  Frees the allocated global shared memory.
//
// Returns:
//  TRUE            On success
//  FALSE           On error
//
//*************************************************************************

BOOL GSMDestroy
(
    LPVOID          lpvRootCookie,
    LPGSM_HDR       lpstHdr
)
{
    if (SSMemoryFree(lpvRootCookie,
                     lpstHdr) != SS_STATUS_OK)
        return(FALSE);

    return(TRUE);
}


//*************************************************************************
//
// void GSMInit()
//
// Parameters:
//  lpstHdr         Ptr to created GSM structure
//  dwSizeBytes     Bytes allocated for global shared memory
//
// Description:
//  Initializes the global shared memory structure for global shared
//  memory allocation.
//
//  This function should be called when the memory is in an unknown
//  state and should be initialized to the all free state.
//
// Returns:
//  Nothing.
//
//*************************************************************************

void GSMInit
(
    LPGSM_HDR       lpstHdr,
    DWORD           dwSizeBytes
)
{
    DWORD           dwNumUnitsToAlloc;

    // Appx. round up to next multiple of HDR units plus one

    dwNumUnitsToAlloc = (dwSizeBytes / sizeof(GSM_HDR_T)) + 2;

    // Initialize with the single free block

    lpstHdr->lpstNextBlk = lpstHdr + 1;
    lpstHdr->dwSizeHdrUnits = 0;

    (lpstHdr + 1)->lpstNextBlk = NULL;
    (lpstHdr + 1)->dwSizeHdrUnits = dwNumUnitsToAlloc - 1;
}


//*************************************************************************
//
// LPVOID GSMAlloc()
//
// Parameters:
//  lpstHdr         Ptr to initialized GSM structure
//  dwSizeBytes     Number of bytes to allocate
//
// Description:
//  Allocates a block of size dwSizeBytes from the global shared memory.
//
//  The function searches from the beginning of the free list.  As soon
//  as a free block at least dwSizeBytes is found, the tail end of that
//  free block is allocated.
//
//  The function returns error if dwSizeBytes is zero.
//
//  *lplpvMemoryBlock returns as non-NULL if enough memory was
//  available.  Otherwise, it is set to NULL.
//
// Returns:
//  LPVOID          Ptr to allocated memory
//  NULL            On error
//
//*************************************************************************

LPVOID GSMAlloc
(
    LPGSM_HDR       lpstHdr,
    DWORD           dwSizeBytes
)
{
    DWORD           dwNumUnitsToAlloc;
    LPGSM_HDR       lpstPrevHdr;

   // Return error in case of zero byte allocation

    if (dwSizeBytes == 0)
        return(NULL);

    // Round up to next multiple of HDR units plus one for header

    dwNumUnitsToAlloc = (dwSizeBytes + sizeof(GSM_HDR_T) - 1) /
        sizeof(GSM_HDR_T) + 1;

    // Search from the beginning of the free list

    lpstPrevHdr = lpstHdr;
    while (1)
    {
        if (lpstHdr->dwSizeHdrUnits >= dwNumUnitsToAlloc)
        {
            // Is it an exact match?

            if (lpstHdr->dwSizeHdrUnits == dwNumUnitsToAlloc)
            {
                // Update to previous blocks next pointer to point
                //  to the next block

                lpstPrevHdr->lpstNextBlk = lpstHdr->lpstNextBlk;
            }
            else
            {
                // Return the tail end of the block

                lpstHdr->dwSizeHdrUnits -= dwNumUnitsToAlloc;
                lpstHdr += lpstHdr->dwSizeHdrUnits;
                lpstHdr->dwSizeHdrUnits = dwNumUnitsToAlloc;
            }

            return(lpstHdr + 1);
        }
        else
        if (lpstHdr->lpstNextBlk == NULL)
        {
            // Not enough memory

            return(NULL);
        }
        else
        {
            // Advance to the next block

            lpstPrevHdr = lpstHdr;
            lpstHdr = lpstHdr->lpstNextBlk;
        }
    }
}


//*************************************************************************
//
// BOOL GSMFree()
//
// Parameters:
//  lpstHdr         Ptr to initialized GSM structure
//  lpvMemoryBlock  Ptr to allocated block
//
// Description:
//  Frees a block of allocated global shared memory.
//
//  The function searches from the beginning of the free list looking
//  for the ordered spot in which to insert the block to free.
//  The function also merges free blocks if they are adjacent.
//
//  The function returns error if lpvMemoryBlock is NULL.
//
// Returns:
//  TRUE            On success
//  FALSE           On error
//
//*************************************************************************

BOOL GSMFree
(
    LPGSM_HDR       lpstHdr,
    LPVOID          lpvMemoryBlock
)
{
    LPGSM_HDR       lpstHdrToFree;
    LPGSM_HDR       lpstFreeHdr;

    // Return error in case of NULL pointer

    if (lpvMemoryBlock == NULL)
        return(FALSE);

    // Point to the block's header

    lpstHdrToFree = (LPGSM_HDR)lpvMemoryBlock - 1;

    // Find the location in the free list to insert it

    lpstFreeHdr = lpstHdr;
    while (1)
    {
        if (lpstFreeHdr < lpstHdrToFree &&
            lpstHdrToFree < lpstFreeHdr->lpstNextBlk)
        {
            // Freed block belongs between this and the next free block

            break;
        }

        if (lpstFreeHdr->lpstNextBlk == NULL)
        {
            // Freed block belongs at end of free list

            break;
        }

        // Advance to next free block

        lpstFreeHdr = lpstFreeHdr->lpstNextBlk;
    }

    // Join to upper block?

    if (lpstHdrToFree + lpstHdrToFree->dwSizeHdrUnits ==
        lpstFreeHdr->lpstNextBlk)
    {
        // Merge the block to free and the upper block

        lpstHdrToFree->dwSizeHdrUnits +=
            lpstFreeHdr->lpstNextBlk->dwSizeHdrUnits;

        lpstHdrToFree->lpstNextBlk =
            lpstFreeHdr->lpstNextBlk->lpstNextBlk;
    }
    else
    {
        // Link this block and the upper block

        lpstHdrToFree->lpstNextBlk = lpstFreeHdr->lpstNextBlk;
    }

    // Join to lower block?

    if (lpstFreeHdr + lpstFreeHdr->dwSizeHdrUnits == lpstHdrToFree)
    {
        // Merge the lower block and the block to free

        lpstFreeHdr->dwSizeHdrUnits += lpstHdrToFree->dwSizeHdrUnits;
        lpstFreeHdr->lpstNextBlk = lpstHdrToFree->lpstNextBlk;
    }
    else
    {
        // Link the lower block and the block to free

        lpstFreeHdr->lpstNextBlk = lpstHdrToFree;
    }

    return(TRUE);
}


