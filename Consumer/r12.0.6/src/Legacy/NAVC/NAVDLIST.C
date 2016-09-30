// Copyright 1993,1996 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/navc/VCS/navdlist.c_v   1.0   06 Feb 1997 20:56:22   RFULLER  $
//
// Description:
//      These are the functions for the NAV variable length lists.
//      Before using these functions, a ListRec structure must be
//      initialized with List.userData pointing to a NAVListData structure
//      NAVData with NAVData.wEntryMaxSize equal to the maximum size
//      of each list entry, including the EOS for strings.
//      NAVData.hPointers and NAVData.hEntries should also be NULL pointers.
//
// Contains:
//      WORD PASCAL NAVListAdd              (ListRec *lplrList)
//      WORD PASCAL NAVListDelete           (ListRec *lplrList)
//      VOID PASCAL NAVListMemFree          (ListRec *lplrList)
//      WORD PASCAL NAVListDuplicateString  (ListRec *lplrList, char *szString)
//
//
// See Also:
//      NAVD.H                          definition of NAVListData
//************************************************************************
// $Log:   S:/navc/VCS/navdlist.c_v  $
// 
//    Rev 1.0   06 Feb 1997 20:56:22   RFULLER
// Initial revision
// 
//    Rev 1.0   31 Dec 1996 15:20:06   MKEATIN
// Initial revision.
// 
//    Rev 1.0   02 Oct 1996 12:30:42   JBELDEN
// Initial revision.
//************************************************************************

#include "Platform.h"
#include "xapi.h"
#include "stddos.h"
#include "stddlg.h"
#include "undoc.h"

#include "navdprot.h"

MODULE_NAME;

#define LIST_ALLOC_ENTRIES  (10)

//************************************************************************
// LOCAL PROTOTYPES
//************************************************************************

VOID LOCAL PASCAL NAVListResetPointers (ListRec *lplrList);
WORD LOCAL PASCAL NAVListResize (ListRec *lplrList);


//************************************************************************
// NAVListAdd()
//
// This routine inserts lpItem into lplrList at the current position.
// IMPORTANT: lplrList->userData must point to a valid NAVListData structure.
// If necessary, it will initialize the memory handles in NAVListData and
// dynamically allocate memory for the list entries.
// The memory can be freed and the handles zeroed with a call to
// NAVListMemFree.
//
// Parameters:
//      ListRec *   lplrList
//      LPVOID      lpItem
//
// Returns:
//      TRUE                            Item successfully added.
//      FALSE                           Memory allocation error.
//************************************************************************
// 2/23/93 DALLEE Function created.
//************************************************************************

WORD PASCAL NAVListAdd (ListRec *lplrList, LPVOID lpItem)
{
    NAVListData *lprNAVList;
    WORD        wEntrySize;
    BYTE        *lpbyInsert;
    DWORD       dwInsert;

    lprNAVList = (NAVListData *) lplrList->userData;
    wEntrySize = lprNAVList->wEntryMaxSize;

    //--------------------------------------------------------------------
    // INITIALIZATION:
    //--------------------------------------------------------------------
                                        // If necessary, initialize handles,
                                        // allocate first chunk of memory, and
                                        // lock blocks.
    if (lprNAVList->hPointers == NULL)
        {
        lprNAVList->hPointers =
         MemAlloc(GHND, (LIST_ALLOC_ENTRIES + 1) * sizeof(LPVOID));

        lprNAVList->hEntries  =
         MemAlloc(GHND, LIST_ALLOC_ENTRIES * wEntrySize);

        lplrList->entries = MemLock(lprNAVList->hPointers);

        lprNAVList->lpEntries = MemLock(lprNAVList->hEntries);

                                        // Check for errors in alloc / lock
        if ((lprNAVList->hPointers == NULL) || (lprNAVList->hEntries == NULL)
             || (lplrList->entries == NULL) || (lprNAVList->lpEntries == NULL))
            {
            return (FALSE);
            }
                                        // NULL terminate the array
        ((LPVOID *)lplrList->entries)[0] = NULL;
        lplrList->num = 0;              // Set number of entries to 0

                                        // There should be no active entry
        lplrList->active_entry = (DWORD) -1;
        }                               // END INITIALIZATION


    //--------------------------------------------------------------------
    // REALLOCATION:
    //--------------------------------------------------------------------
                                        // Do we need to allocate more memory?
    if ( ((lplrList->num+1) * wEntrySize) > MemSize(lprNAVList->hEntries) ||
         ((lplrList->num+2) * sizeof(LPVOID)) > MemSize(lprNAVList->hPointers) )
        {
	if (NAVListResize(lplrList) == FALSE)
	    return (FALSE);
        }                               // END REALLOCATION


    //--------------------------------------------------------------------
    // INSERTION:
    //--------------------------------------------------------------------
    if ((lplrList->entries == NULL) || (lprNAVList->lpEntries == NULL))
        {
        return (FALSE);
        }
    else
        {
                                        // If we have a valid active entry
                                        // insert at that point.  If not,
                                        // append to the list
        dwInsert = (lplrList->active_entry < lplrList->num)
                   ? lplrList->active_entry : lplrList->num;

                                        // Insert item into entry buffer
        lpbyInsert = (BYTE *) lprNAVList->lpEntries +
                     (dwInsert * wEntrySize);

        memmove(lpbyInsert + wEntrySize,
                lpbyInsert,
                (WORD)(lplrList->num - dwInsert) * wEntrySize);

        memmove(lpbyInsert, lpItem, wEntrySize);

                                        // Update the ListRec pointers to
                                        // entries and the number of entries
        lplrList->num++;
        NAVListResetPointers(lplrList);

        return (TRUE);
        }
}


//************************************************************************
// NAVListDelete()
//
// This routine deletes the specified entry from lplrList.
// IMPORTANT: lplrList->userData must point to a valid NAVListData structure.
// If the number of entries shrinks by 2 * LIST_ALLOC_ENTRIES, or goes
// to zero, then the allocated memory is resized.
//
// Parameters:
//      ListRec *   lplrList
//      DWORD       dwIndex             Entry number to delete
//                                       first entry is 0
//
// Returns:
//      TRUE                            Item successfully deleted.
//      FALSE                           Invalid list index, or list had
//                                       null pointers
//************************************************************************
// 2/24/93 DALLEE Function created.
//************************************************************************

WORD PASCAL NAVListDelete (ListRec *lplrList, DWORD dwIndex)
{
    NAVListData *lprNAVList;
    WORD        wEntrySize;
    BYTE        *lpbyDelete;

    lprNAVList = (NAVListData *) lplrList->userData;
    wEntrySize = lprNAVList->wEntryMaxSize;

                                        // Check for invalid index, or
                                        // attempted NULL pointer assignment
    if ( (dwIndex >= lplrList->num)  ||
         (lplrList->entries == NULL) ||
         (lprNAVList->lpEntries == NULL) )
        {
        return (FALSE);
        }


    //--------------------------------------------------------------------
    // DELETE ENTRY
    //--------------------------------------------------------------------

                                        // Delete item from entry buffer
    lpbyDelete = (BYTE *) lprNAVList->lpEntries +
                 (dwIndex * wEntrySize);

    memmove(lpbyDelete,
            lpbyDelete + wEntrySize,
            (WORD)(lplrList->num - dwIndex - 1) * wEntrySize);

                                        // Update ListRec data
    lplrList->num--;
    if ( (lplrList->active_entry > dwIndex) ||
         (lplrList->active_entry >= lplrList->num))
        {
        lplrList->active_entry--;
        }
    NAVListResetPointers(lplrList);


    //--------------------------------------------------------------------
    // RESIZE BUFFERS
    //--------------------------------------------------------------------

                                        // If no more entries, free memory
    if (lplrList->num == 0)
        {
        NAVListMemFree(lplrList);
	return (TRUE);
	}

					// If > 2 * LIST_ALLOC_ENTRIES free
					// space, then resize
    if ( ((lplrList->num + 2 * LIST_ALLOC_ENTRIES) * wEntrySize) <
         MemSize(lprNAVList->hEntries) )
        {
        return (NAVListResize(lplrList));
	}

    return (TRUE);
} // End NAVListDelete()


//************************************************************************
// NAVListResetPointers()
//
// This routine resets the ListRec's entry pointers.  It is called after
// adding or deleting entries, and reallocating buffers.
//
// Parameters:
//      ListRec *   lplrList
//
// Returns:
//      nothing
//************************************************************************
// 2/24/93 DALLEE Function created.
//************************************************************************

VOID LOCAL PASCAL NAVListResetPointers (ListRec *lplrList)
{
    NAVListData     *lprNAVList;
    WORD            wEntry;

    if (lplrList->entries != NULL)      // Check for NULL pointer assignment
        {
        lprNAVList = (NAVListData *) lplrList->userData;

        for (wEntry = 0; wEntry < lplrList->num; wEntry++)
            {
            ((LPVOID *)lplrList->entries)[wEntry] = (BYTE *)lprNAVList->lpEntries +
                                (wEntry * lprNAVList->wEntryMaxSize);
            }
        ((LPVOID *)lplrList->entries)[wEntry] = NULL;
        }
}


//************************************************************************
// NAVListResize()
//
// This routine resizes the entry pointers buffer and the entries buffer
// to accept lplrList->num + LIST_ALLOC_ENTRIES items.
//
// Parameters:
//      ListRec *   lplrList
//
// Returns:
//      TRUE                            Buffers successfully resized
//      FALSE                           Error in resizing
//************************************************************************
// 2/24/93 DALLEE Function created.
//************************************************************************

WORD LOCAL PASCAL NAVListResize (ListRec *lplrList)
{
    NAVListData *lprNAVList;
    HGLOBAL     hTempHandle;
    WORD        wEntrySize;

    lprNAVList = (NAVListData *) lplrList->userData;
    wEntrySize = lprNAVList->wEntryMaxSize;

                                        // Resize and relock pointers buffer
                                        // Add one so we can have a NULL
                                        // terminated array
    MemUnlock(lprNAVList->hPointers, lplrList->entries);
    hTempHandle = MemReAlloc( lprNAVList->hPointers,
			      (lplrList->num + LIST_ALLOC_ENTRIES + 1) *
			      sizeof(BYTE *), 0);

    if (hTempHandle == NULL)
	{
	return (FALSE);
	};

    lprNAVList->hPointers = hTempHandle;
    lplrList->entries = MemLock(lprNAVList->hPointers);
    if (lplrList->entries == NULL)
	{
	return (FALSE);
	};

					// Resize and relock list entry buffer
    MemUnlock(lprNAVList->hEntries, lprNAVList->lpEntries);
    hTempHandle = MemReAlloc( lprNAVList->hEntries,
			      (lplrList->num + LIST_ALLOC_ENTRIES) *
			      wEntrySize, 0);
    if (hTempHandle == NULL)
	{
	return (FALSE);
	}

    lprNAVList->hEntries = hTempHandle;
    lprNAVList->lpEntries = MemLock(lprNAVList->hEntries);
    if (lprNAVList->lpEntries == NULL)
	{
	return (FALSE);
	};

    return (TRUE);
} // End NAVListResize()


//************************************************************************
// NAVListMemFree()
//
// This routine frees the memory allocated for a dynamic length list
// and zeros all pointers and handles.
//
// Parameters:
//      ListRec *   lplrList
//
// Returns:
//	nothing
//************************************************************************
// 2/24/93 DALLEE Function created.
//************************************************************************

VOID PASCAL NAVListMemFree (ListRec *lplrList)
{
    NAVListData *lprNAVList;

    if ( lplrList == NULL )
        return ;

    lprNAVList = (NAVListData *) lplrList->userData;

                                        // Free entry pointers (if allocated)

    if ( lprNAVList->hPointers != NULL )
        {
        MemUnlock(lprNAVList->hPointers, lplrList->entries);
        MemFree(lprNAVList->hPointers);
        }

    if ( lprNAVList->hEntries != NULL )
        {
        MemUnlock(lprNAVList->hEntries, lprNAVList->lpEntries);
        MemFree(lprNAVList->hEntries);
        }

    lprNAVList->hPointers = lprNAVList->hEntries  = NULL ;
    lplrList->entries     = lprNAVList->lpEntries = NULL ;

    lplrList->num = 0;                  // Zero the number of entries
    lplrList->active_entry = 0;         // Reset active entry
} // End NAVListMemFree()


//************************************************************************
// NAVListDuplicateString()
//
// This routine checks if szString is already in lplrList.
//
// Parameters:
//      ListRec     *lplrList
//      char        *szString
//
// Returns:
//      TRUE                            szString is already in lplrList
//      FALSE                           szString not in lplrList
//************************************************************************
// 2/26/93 DALLEE Function created.
//************************************************************************

WORD PASCAL NAVListDuplicateString (ListRec *lplrList, char *szString)
{
    WORD    wEntry;
    char    **lpszListEntries;

    lpszListEntries = (char **)lplrList->entries;

    for (wEntry = 0; wEntry < lplrList->num; wEntry++)
        {
        if (!STRCMP(lpszListEntries[wEntry], szString))
            {
            return (TRUE);
            }
        }
    return (FALSE);
} // End NAVListDuplicateString()
