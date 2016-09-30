/*
 * $Workfile::   pmemlist.c                                                  $
 * $Revision::   1.0                                                         $
 * $Modtime::   30 Jan 1996 15:48:06                                         $
 *
 * LIST MANAGER - Global Header file
 *
 * LANGUAGE    : Microsoft C9.0
 * MODEL       : 32Bit Flat
 * ENVIRONMENT : Microsoft Windows 95 SDK
 *
 * This module contains the source for the submem and list manager module.
 *
 */

#ifdef SYM_BUILD_SM
#include "fsv_pch.h"
#pragma hdrstop
#include "fsv.p"
#include "wnfsv.seg"
#else

// Common includes for Pre-compiled Header
// This should go before any other includes or definitions
#include "..\inc\fsv_pch.h"
#pragma hdrstop
// Put all other includes after this

// File Services include files (not in PCH)
#include "..\inc\fsv.p"

//
// SEGMENT FILE
//
#include "..\inc\wnfsv.seg"
#endif

/*==========================================================================*/
/*                              DEFINES                                     */
/*==========================================================================*/
  
/* each far heap starts with 4K */
#define INITIALALLOC 0x1000
  
/* allocations bigger than 4K get their own segments */
#define SUBMAX       0x1000
  
#define HF_GLOBAL    1
#define HF_LOCAL     2

#define OVERRUN_SIGNATURE  0x0f0f0f0f
  
/* must be greater than 0x10+sizeof(SEGHDR) */
#define SEGRESERVED ((((sizeof(SEGHDR) + 0x0f) >> 4) + 1) << 4)
#define GETHDR(lpg) ((LPHDR)(MAKELP( SELECTOROF(lpg), 0x10 )))

#define ListOrigCount(lpList)          (((LPLISTHEADSTRUCT)lpList)[-1].idxOrigCount)
#define ListAllocType(lpList)          (((LPLISTHEADSTRUCT)lpList)[-1].wAllocType)

//===========================================================================
//                             TYPEDEFS
//===========================================================================

typedef struct{
   LPVOID      lpgNext;
   LPVOID      lpgPrev;
   LPVOID      lpgLast;
   WORD        nAllocs;
   WORD        wHeapFlag;
   WORD        nReserved[7];
} SEGHDR, FAR *LPHDR;
  
//===========================================================================
//                       SUBALLOCATION FUNCTIONS
//===========================================================================

//===========================================================================
//
// Mem_SubMallocHeap( dwBytes, lphHeap ) : LPVOID;
//
//    dwBytes        Number of bytes to allocate.
//    lphHeap        far pointer to a heap handle.
//
// This function will allocate 'dwBytes' using EXTRA's suballocation.
// The suballocation will come of the supplied heap handle.
//
// Returns: Address of the allocated memory.
//
//
  
LPVOID WINAPI Mem_SubMallocHeap(
   DWORD       dwBytes,
   LPVOID      lphHeap
)
{
   LPVOID   lpvRtn = malloc(dwBytes);

   if (lpvRtn)
      memset( lpvRtn, 0, dwBytes );

   return lpvRtn;
}
  
//===========================================================================
//
// Mem_SubCallocHeap( dwCount, dwSize, lphHeap ) : LPVOID;
//
//    dwCount        Number of elements to allocate.
//    dwSize         Size (in bytes) of each element.
//    lphHeap        far pointer to a heap handle.
//
// This function will allocate 'dwCount * dwSize' bytes using EXTRA's
// suballocation. The memory will be initialized with zeros.
// The suballocation will come of the supplied heap handle.
//
// Returns: Address of the allocated memory.
//
  
LPVOID WINAPI Mem_SubCallocHeap(
   DWORD       dwCount,
   DWORD       dwSize,
   LPVOID      lphHeap
)
{
   return calloc( dwCount, dwSize );
}
  
/**/
  
//===========================================================================
//
// Mem_SubReallocHeap( lpMem, dwBytes, lphHeap ) : LPVOID;
//
//    lpMem          Address of the original block.
//    dwBytes        Number of bytes to allocate.
//    lphHeap        far pointer to a heap handle.
//
// This function will reallocate 'lpMem' to 'dwBytes'. This can only be used
// on memory allocated with Mem_SubMalloc or Mem_SubCalloc.
//
// Returns: Address of the newly allocated memory.
//
  
LPVOID WINAPI Mem_SubReallocHeap(
   LPVOID      lpMem,
   DWORD       dwBytes,
   LPVOID      lphHeap
)
{
   return realloc( lpMem, dwBytes );
}
  
//===========================================================================
//
// Mem_SubFreeHeap( lpMem, lphHeap ) : void;
//
//    lpMem          Address of the memory block.
//    lphHeap        far pointer to a heap handle.
//
// This function will free 'lpMem'. This can only be used on memory allocated
// or reallocated with Mem_SubMalloc, Mem_SubCalloc, or Mem_SubRealloc.
//
// Returns: none
//
  
void WINAPI Mem_SubFreeHeap(
   LPVOID      lpMem,
   LPVOID      lphHeap
)
{
   free( lpMem );
}

//===========================================================================
//                            LIST FUNCTIONS
//===========================================================================

/*
 * ListMalloc( idxCount, entrySize, wAllocType ) : LPVOID;
 *
 *    idxCount       Number of entries required
 *    entrySize      Size (bytes) of each entry
 *    wAllocType     Type of allocation (LISTALLOC_???)
 *
 * This routine performs an allocation for a list with 'idxCount' items
 * required and each item is 'entrySize' bytes. A special header for list
 * management is hidden in front of the allocated space and is accessable
 * via a macro. This header contains the current count, the maximum count,
 * and the size of each item.
 *
 * The memory allocated by this routine is not cleared.
 *
 * Returns: A pointer to the allocated list.
 *
 */

LPVOID WINAPI ListMalloc(
   LISTINDEX         idxCount,
   ENTRYSIZE         entrySize,
   WORD              wAllocType )
{
   long              lReqSize;
   long              lAllocSize;
   LPLISTHEADSTRUCT  lpListHead = NULL;
   LPVOID            lpListRtn = NULL;

   lReqSize = idxCount * entrySize + sizeof(LISTHEADSTRUCT);

   /* Verify reguest is within allowed size. */
   if (lReqSize <= LIST_SIZEMAX)
   {
      /* Compute lAllocSize as 20 * 1.5 ^ n (20 bytes minimum) */
      for (lAllocSize = 20; lAllocSize < lReqSize; lAllocSize += lAllocSize >> 1);

      if (lAllocSize > LIST_SIZEMAX)
         lAllocSize = LIST_SIZEMAX;

      switch (wAllocType)
      {
         case LIST_ALLOC_SHARED:
            lpListHead = (LPLISTHEADSTRUCT)GlobalAllocPtr(
               GMEM_SHARE | GMEM_MOVEABLE,
               (DWORD)lAllocSize
            );
         break;

         case LIST_ALLOC_GLOBAL:
            lpListHead = (LPLISTHEADSTRUCT)GlobalAllocPtr(
               GMEM_MOVEABLE,
               (DWORD)lAllocSize
            );
         break;

         default:
         break;
      }

      /* Verify that the allocation was successful. */
      if (lpListHead)
      {
         lpListHead->idxCount = 0;
         lpListHead->entrySize = entrySize;
         lpListHead->wAllocType = wAllocType;
         lpListHead->idxMaxCount = lpListHead->idxOrigCount =
            (LISTINDEX)(((size_t)lAllocSize - sizeof(LISTHEADSTRUCT)) / entrySize);

         lpListRtn = (LPVOID)(lpListHead + 1);
      }
   }

   return lpListRtn;
}

/**/

/*
 * ListRealloc( lplpList, idxNewMaxCount ) : BOOL;
 *
 *    lplpList       address of a list pointer
 *    idxNewMaxCount new maximum count for the list
 *
 * This routine reallocates a list to a smaller/larger size.
 * The memory reallocated by this routine is not cleared.
 *
 * Returns: A pointer to the reallocated list.
 *
 */

BOOL WINAPI ListRealloc(
   LPVOID FAR *      lplpList,
   LISTINDEX         idxNewMaxCount )
{
   long              lAllocSize;
   LPLISTHEADSTRUCT  lpListHead;
   BOOL              fSuccess = FALSE;

   if (lplpList && *lplpList)
   {
      lAllocSize = idxNewMaxCount * ListEntrySize(*lplpList) + sizeof(LISTHEADSTRUCT);
      lpListHead = (LPLISTHEADSTRUCT)(*lplpList) - 1;

      switch (lpListHead->wAllocType)
      {
         case LIST_ALLOC_SHARED:
         case LIST_ALLOC_GLOBAL:
         {
            LPLISTHEADSTRUCT     lplhNew;
      
            lplhNew = (LPLISTHEADSTRUCT)GlobalReAllocPtr(
               lpListHead,
               (DWORD)lAllocSize,
               GMEM_ZEROINIT
            );
            lpListHead = lplhNew;
         }
         break;

         default:
         break;
      }

      /* Verify reallocation success. */
      if (lpListHead)
      {
         lpListHead->idxMaxCount = (LISTINDEX) (((size_t)lAllocSize -
         sizeof(LISTHEADSTRUCT)) / lpListHead->entrySize);

         *lplpList = (LPVOID)(lpListHead + 1);
         fSuccess = TRUE;
      }
   }

   return fSuccess;
}

/**/

/*
 * ListFree( lpList ) : BOOL;
 *
 *    lpList         list to free
 *
 * This routine should be used to free a list created with the ListMalloc
 * routine.
 *
 * Returns: TRUE if successful, FALSE if not.
 *
 */

BOOL WINAPI ListFree(
   LPVOID      lpList )
{
   BOOL        fSuccess = FALSE;

   if (lpList)
   {
      switch (ListAllocType( lpList ))
      {
         case LIST_ALLOC_SHARED:
         case LIST_ALLOC_GLOBAL:
         {
            LPLISTHEADSTRUCT  lpListHead = (LPLISTHEADSTRUCT)(lpList) - 1;
            GlobalFreePtr( lpListHead );
            fSuccess = TRUE;
         }
         break;

         default:
         break;
      }
   }

   return fSuccess;
}

/**/

/*
 * ListInsert( lplpList, idx, lpData ) : LISTINDEX;
 *
 *    lplpList       Pointer to a list pointer.
 *    idx            Insertion point in the list.
 *    lpData         Pointer to the data to insert.
 *
 * This routine will insert an array element in a dynamically growable
 * list at a specified position. If the new position is not past the end
 * of the list, the existing array entries starting at 'idx' up to the
 * end of the current list length will be shuffled down one. The entry
 * data will then be copied into the 'idx' element of the array. If
 * 'idx' is LIST_INDEXMAX, the insertion will be at the end of the list.
 *
 * The ListAppend macro is a shorthand for ListInsert with LIST_INDEXMAX
 * passed.
 *
 * Returns: The index of the item just inserted or appended. If the return
 * value is LIST_INDEXNONE, then an ERROR occured.
 *
 */

LISTINDEX WINAPI ListInsert(
   LPVOID FAR *lplpList,
   LISTINDEX   idx,
   LPVOID      lpData )
{
   LISTINDEX   idxLast;
   LISTINDEX   idxNewMaxCount;
   LPVOID      lpList;
   LPVOID      lpNewEntry;
   ENTRYSIZE   entrySize;
    LISTINDEX   idxRtn = LIST_INDEXNONE;
   BOOL        fStatus = TRUE;

   if (lpData && lplpList && *lplpList)
   {
      lpList    = *lplpList;
      entrySize = ListEntrySize(lpList);

      /* Incrementing the current list length */
      idxLast = ListCount(lpList);
      if (idxLast <= LIST_COUNTMAX)
      {
         /* Grow list to handle new entry (if necessary) */
         if (idxLast + 1 > ListMaxCount(lpList))
         {
            // JJS. 01/06/94. Cast values to LONG to avoid overflow.
            idxNewMaxCount = (LISTINDEX) min(
               (LONG)ListMaxCount(lpList) + (LONG)(ListMaxCount(lpList) >> 1),
               (LONG)LIST_COUNTMAX
            );
            idxNewMaxCount = min(
               idxNewMaxCount,
               (LISTINDEX)(LIST_SIZEMAX-sizeof(LISTHEADSTRUCT)) / ListEntrySize(lpList)
            );

            // JJS. 01/06/94. Check for list already at maximum size.
            if ( idxNewMaxCount <= ListMaxCount(lpList) )
            {
               fStatus = FALSE;
            }
            else
            {
               fStatus = ListRealloc( lplpList, idxNewMaxCount );
            }
         }

         /* If all is well, then proceed to insert the item. */
         if (fStatus)
         {
            lpList = *lplpList;

            if (idx > idxLast)
               idx = idxLast;
            lpNewEntry = (LPSTR)lpList + idx * entrySize;

            /* Shuffle elements from index to the end */
            if (idx < idxLast)
            {
               _fmemmove(
                  (LPSTR)lpNewEntry + entrySize,
                  lpNewEntry,
                  (idxLast - idx) * entrySize
               );
            }

            /* Copy the entry data */
            _fmemcpy(lpNewEntry, lpData, entrySize);

            ListCount(lpList)++;
            *lplpList = lpList;
            idxRtn = idx;
         }
      }
   }

   return idxRtn;
}

/**/

/*
 * ListInsertMultiple( lplpList, idx, count, lpData ) : LISTINDEX;
 *
 *    lplpList       Pointer to a list pointer.
 *    idx            Insertion point in the list.
 *    count          Number of items to insert.
 *    lpData         Pointer to the block of data to insert ('count' items).
 *
 * This routine will insert 'count' array elements in a dynamically growable
 * list at a specified position. If the new position is not past the end
 * of the list, the existing array entries starting at 'idx' up to the
 * end of the current list length will be shuffled down 'count' positions. The
 * entry data will then be copied into the array starting with the 'idx'
 * position. If 'idx' is LIST_INDEXMAX, the insertions will be at the end of
 * the list.
 *
 * The ListAppendMultiple macro is a shorthand for ListInsertMultiple with
 * LIST_INDEXMAX passed.
 *
 * Returns: The index of the first item just inserted or appended.
 *
 */

LISTINDEX WINAPI ListInsertMultiple(
   LPVOID FAR *lplpList,
   LISTINDEX   idx,
   LISTINDEX   count,
   LPVOID      lpData )
{
   LISTINDEX   idxLast;
   LPVOID      lpList;
   LPVOID      lpNewEntry;
   ENTRYSIZE   entrySize;
   LISTINDEX   idxNewMaxCount;
   LISTINDEX   idxRtn = LIST_INDEXNONE;
   BOOL        fStatus = TRUE;

   if ( lpData && lplpList && *lplpList )
   {
      lpList    = *lplpList;
      entrySize = ListEntrySize( lpList );

      /* Incrementing the current list length */
      idxLast = ListCount(lpList);
      if (idxLast <= LIST_COUNTMAX)
      {
         /* Grow list to handle new entry (if necessary) */
         if (idxLast + count > ListMaxCount(lpList))
         {
            idxNewMaxCount = ListMaxCount(lpList);
            while ((idxLast + count >= idxNewMaxCount)
               && (LIST_COUNTMAX > idxNewMaxCount)
               && (((LISTINDEX)(LIST_SIZEMAX-sizeof(LISTHEADSTRUCT))/ListEntrySize(lpList)) > idxNewMaxCount))
            {
               idxNewMaxCount += (idxNewMaxCount >> 1);
            }

            idxNewMaxCount = min( idxNewMaxCount, (LISTINDEX)LIST_COUNTMAX );
            idxNewMaxCount = min(
               idxNewMaxCount,
               (LISTINDEX)(LIST_SIZEMAX-sizeof(LISTHEADSTRUCT))/ListEntrySize(lpList)
            );

            if (idxLast+count <= idxNewMaxCount)
               fStatus = ListRealloc( lplpList, idxNewMaxCount );
            else
               fStatus = FALSE;
         }

         /* If all is well, then proceed to insert the item. */
         if (fStatus)
         {
            lpList = *lplpList;

            if (idx > idxLast)
               idx = idxLast;
            lpNewEntry = (LPSTR)lpList + idx * entrySize;

            /* Shuffle elements from index to the end */
            if (idx < idxLast)
            {
               _fmemmove(
                  (LPSTR)lpNewEntry + entrySize,
                  lpNewEntry,
                  (idxLast - idx) * entrySize
               );
            }

            /* Copy the entry data */
            _fmemcpy(lpNewEntry, lpData, entrySize);

            *lplpList = lpList;
            idxRtn = idx;
         }
      }
   }

   return idxRtn;
}

/**/

/*
 * ListDelete( lplpList, idx ) : BOOL;
 *
 *    lplpList       Address of a pointer to the list.
 *    idx            Index to delete from the list.
 *
 * This routine will remove an entry from a dynamically growable array by
 * shuffling any entries in the list following the specified index up over
 * the deleted entry. The number of list entries will be decremented. As with
 * ListInsert, the pointer to the list pointer is passed since the list may be
 * reallocated to a smaller size.
 *
 * Returns: TRUE if successful, FALSE if NOT
 *
 */

BOOL WINAPI ListDelete(
   LPVOID FAR *lplpList,
   LISTINDEX   idx )
{
   LPSTR       lpEntry;
   LPVOID      lpList;
   LISTINDEX   idxShuffle;
   LISTINDEX   idxNewMaxCount;
   ENTRYSIZE   entrySize;
   BOOL        fSuccess = FALSE;

   /* Validate incoming parameters. */
   if (lplpList)
   {
      lpList = *lplpList;
        if (lpList && idx <= ListCount(lpList))
      {
         entrySize = ListEntrySize(lpList);

         /* Collapse List */
         idxShuffle = ListCount(lpList) - idx - 1;
         if (idxShuffle > 0)
         {
            lpEntry = (LPSTR)lpList + idx * entrySize;
            _fmemmove( lpEntry, lpEntry + entrySize, entrySize * idxShuffle);
         }
         ListCount(lpList)--;

         fSuccess = TRUE;

         /* Shrink list if possible. */
         if (ListCount(lpList) < (ListMaxCount(lpList) >> 1))
         {
            /* Don't allow the list to shrink past the original size. */
            idxNewMaxCount = max( ListMaxCount(lpList) * 2 / 3, ListOrigCount(lpList));

            if (idxNewMaxCount != ListMaxCount(lpList))
               fSuccess = ListRealloc(lplpList, idxNewMaxCount);
         }
      }
   }

   return fSuccess;
}

/**/

/*
 * ListDeleteMultiple( lplpList, idx, count ) : BOOL;
 *
 *    lplpList       Address of a pointer to the list.
 *    idx            Starting index in the range to delete from the list.
 *    count          Number of items to delete from the list.
 *
 * This routine works like ListDelete, except multiple entries will be removed.
 *
 * Returns: TRUE if successful, FALSE if NOT
 *
 */

BOOL WINAPI ListDeleteMultiple(
   LPVOID FAR *lplpList,
   LISTINDEX   idx,
   LISTINDEX   count )
{
   LPSTR       lpEntry;
   LPVOID      lpList;
   LISTINDEX   idxShuffle;
   LISTINDEX   idxNewMaxCount;
   ENTRYSIZE   entrySize;
   BOOL        fSuccess = FALSE;

   if (lplpList && *lplpList)
   {
      lpList = *lplpList;
      idx = max( idx, 0 );
      count = min( count, ListCount(lpList)-idx );
      if (count)
      {
         entrySize = ListEntrySize(lpList);

         /* Collapse List */
         idxShuffle = ListCount(lpList) - count - idx;
         if (idxShuffle > 0)
         {
            lpEntry = (LPSTR)lpList + idx * entrySize;
            _fmemmove(
               lpEntry,
               lpEntry + entrySize * (idx + count),
               entrySize * idxShuffle
            );
         }

         /* Readjust list count in header. */
         ListCount(lpList) -= count;

         fSuccess = TRUE;

         /* Shrink list if possible. */
         if (ListCount(lpList) < (ListMaxCount(lpList) >> 1))
         {
            /* Don't allow the list to shrink past the original size. */
            idxNewMaxCount = max( ListMaxCount(lpList) * 2 / 3, ListOrigCount(lpList));
            if (idxNewMaxCount != ListMaxCount(lpList))
               fSuccess = ListRealloc(lplpList, idxNewMaxCount);
         }
      }
   }

   return fSuccess;
}

/**/

/*
 * ListBFind( lpList, sDType, wOffset, lpKey, lpIdx) : LPVOID;
 *
 *    lpList         Pointer to the list.
 *    wDType         Datatype of the 'key' (LIST_DTYPE_???).
 *    wOffset        Offset within the entry for the 'key' (in Bytes).
 *    lpKey          Pointer to the 'key' to find.
 *    lpIdx          Pointer to a LISTINDEX variable to hold the index found.
 *
 * Do a BINARY search for an entry in a list for a match with the
 * specified 'key'. The variables 'wDType' and 'wOffset' are used to find
 * 'lpKey' within the entry. The index of the entry (or the insert
 * position if entry is not found) is placed in 'lpIdx'.
 *
 * Returns: A pointer to the entry is returned (NULL if not found).
 *
 */

LPVOID WINAPI ListBFind(
   LPVOID      lpList,
   WORD        wDType,
   WORD        wOffset,
   LPVOID      lpKey,
   LPLISTINDEX lpIdx )
{
   LISTINDEX   idxLow, idxMid, idxHigh;
   int         compare;
   LPSTR       lpEntry, lpstrEntry;
   short       sEntry;
   WORD        wEntry;
   long        lEntry;
   DWORD       dwEntry;
   ENTRYSIZE   entrySize;

   if (!lpList)
      idxHigh = 0;
   else
   {
      entrySize = ListEntrySize(lpList);

      for (idxLow = 0, idxHigh = ListCount(lpList); idxLow < idxHigh; )
      {
         /* Loop until idxLow=idxHigh */
         idxMid = (idxLow + idxHigh) / 2;
         lpEntry = (LPSTR)lpList + entrySize * idxMid;
         compare = 0;
         switch (wDType)
         {
            case LIST_DTYPE_STR:
               lpstrEntry = *((LPSTR FAR *)(lpEntry+wOffset));
               compare = lstrcmp( (LPSTR)lpKey, lpstrEntry );
            break;

                case LIST_DTYPE_CHARARRAY:
                    lpstrEntry = (LPSTR)(lpEntry+wOffset);
               compare = lstrcmp( (LPSTR)lpKey, lpstrEntry );
            break;

                case LIST_DTYPE_SHORT:
               sEntry = *(short FAR *)(lpEntry+wOffset);
               if (*(short FAR *)lpKey < sEntry)
                  compare = -1;
               else if (*(short FAR *)lpKey > sEntry)
                  compare = 1;
            break;

            case LIST_DTYPE_WORD:
            case LIST_DTYPE_HWND:
               wEntry = *(LPWORD)(lpEntry+wOffset);
               if (*(LPWORD)lpKey < wEntry)
                  compare = -1;
               else if (*(LPWORD)lpKey > wEntry)
                  compare = 1;
            break;

            case LIST_DTYPE_LONG:
               lEntry = *(LPLONG)(lpEntry+wOffset);
               if (*(LPLONG)lpKey< lEntry)
                  compare = -1;
               else if (*(LPLONG)lpKey > lEntry)
                  compare = 1;
            break;

            case LIST_DTYPE_LP:
            case LIST_DTYPE_DWORD:
               dwEntry = *(LPDWORD)(lpEntry+wOffset);
               if (*(LPDWORD)lpKey< dwEntry)
                  compare = -1;
               else if (*(LPDWORD)lpKey > dwEntry)
                  compare = 1;
            break;

            default:
               return NULL;
         }

         if (compare > 0)
            idxLow = idxMid + 1;
         else if (compare < 0)
            idxHigh = idxMid;
         else
         {
            if (lpIdx)
               *lpIdx = idxMid;
            return (LPVOID)lpEntry;
         }
      }
   }

   if (lpIdx)
      *lpIdx = idxHigh;

   return NULL;
}

/**/

/*
 * ListLFind( lpList, sDType, wOffset, lpKey, lpIdx) : LPVOID;
 *
 *    lpList         Pointer to the list.
 *    wDType         Datatype of the 'key' (LIST_DTYPE_???).
 *    wOffset        Offset within the entry for the 'key' (in Bytes).
 *    lpKey          Pointer to the 'key' to find.
 *    lpIdx          Pointer to a LISTINDEX variable to hold the index found.
 *
 * Do a LINEAR search for an entry in a list for a match with the
 * specified 'key'. The variables 'wDType' and 'wOffset' are used to find
 * 'lpKey' within the entry. The index of the entry (or the insert
 * position if entry is not found) is placed in 'lpIdx'.
 *
 * Returns: A pointer to the entry is returned (NULL if not found).
 *
 */

LPVOID WINAPI ListLFind(
   LPVOID      lpList,
   WORD        wDType,
   WORD        wOffset,
   LPVOID      lpKey,
   LPLISTINDEX lpIdx )
{
   LISTINDEX   idx;
   int         compare;
   LPSTR       lpEntry, lpstrEntry;
   short       sEntry;
   WORD        wEntry;
   long        lEntry;
   DWORD       dwEntry;
   ENTRYSIZE   entrySize;

   if (!lpList)
      idx = 0;
   else
   {
      entrySize = ListEntrySize(lpList);

      for (idx = 0; idx < ListCount(lpList); idx++ )
      {
         lpEntry = (LPSTR)lpList + entrySize * idx;
         compare = 0;
         switch (wDType)
         {
            case LIST_DTYPE_STR:
               lpstrEntry = *((LPSTR FAR *)(lpEntry+wOffset));
               compare = lstrcmp( (LPSTR)lpKey, lpstrEntry );
            break;

                case LIST_DTYPE_CHARARRAY:
                    lpstrEntry = (LPSTR)(lpEntry+wOffset);
               compare = lstrcmp( (LPSTR)lpKey, lpstrEntry );
            break;

                case LIST_DTYPE_SHORT:
               sEntry = *(short FAR *)(lpEntry+wOffset);
               if (*(short FAR *)lpKey < sEntry)
                  compare = -1;
               else if (*(short FAR *)lpKey > sEntry)
                  compare = 1;
            break;

            case LIST_DTYPE_WORD:
            case LIST_DTYPE_HWND:
               wEntry = *(LPWORD)(lpEntry+wOffset);
               if (*(LPWORD)lpKey < wEntry)
                  compare = -1;
               else if (*(LPWORD)lpKey > wEntry)
                  compare = 1;
            break;

            case LIST_DTYPE_LONG:
               lEntry = *(LPLONG)(lpEntry+wOffset);
               if (*(LPLONG)lpKey< lEntry)
                  compare = -1;
               else if (*(LPLONG)lpKey > lEntry)
                  compare = 1;
            break;

            case LIST_DTYPE_LP:
            case LIST_DTYPE_DWORD:
               dwEntry = *(LPDWORD)(lpEntry+wOffset);
               if (*(LPDWORD)lpKey < dwEntry)
                  compare = -1;
               else if (*(LPDWORD)lpKey > dwEntry)
                  compare = 1;
            break;

            default:
               return NULL;
         }

         if (compare == 0)
         {
            if (lpIdx)
               *lpIdx = idx;
            return (LPVOID)lpEntry;
         }
      }
   }

   if (lpIdx)
      *lpIdx = idx;

   return NULL;
}
