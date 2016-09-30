// Copyright 1995 - 1998 Symantec, Peter Norton Product Group
//***************************************************************************
//
// $Header:   S:/APEVENT/VCS/OEFILTER.CPv   1.2   17 Feb 1998 16:44:52   RStanev  $
//
// Description:
//      This file defines functions for working with the OpenAfterExec
//      filter.
//
// Contains:
//      CreateOpenAfterExecEntry()
//      IsItOpenAfterExec()
//      CleanupOpenAfterExec()
//
// See Also:
//      filter.h
//
// Notes:
//      Event Manager module
//
//      Supports SYM_VXD platform only
//
//***************************************************************************
// $Log:   S:/APEVENT/VCS/OEFILTER.CPv  $
// 
//    Rev 1.2   17 Feb 1998 16:44:52   RStanev
// Updated copyright.
//
//    Rev 1.1   15 May 1997 01:11:04   RStanev
// Too many changes to describe.
//
//    Rev 1.0   16 Mar 1997 12:59:44   RStanev
// Initial revision.
//
//***************************************************************************

#if !defined(SYM_VXD)
#error This file supports only SYM_VXD!
#endif

#include "platform.h"
#include "navap.h"
#include "filter.h"

//---------------------------------------------------------------------------
//
// Define constants
//
//---------------------------------------------------------------------------
#define MAX_OEFILTER_ENTRIES    5

//---------------------------------------------------------------------------
//
// Define local structures and variables
//
//---------------------------------------------------------------------------
typedef struct tagOPENAFTEREXECREC
    {
    tagOPENAFTEREXECREC *prNextEntry;
    PTCB                hThread;
    HVM                 hVM;
    UINT                uEnumerator;
    TCHAR               szOEMFile[SYM_MAX_PATH];
    } OPENAFTEREXECREC, *POPENAFTEREXECREC;

POPENAFTEREXECREC prOpenAfterExecListStart;

//---------------------------------------------------------------------------
//
// VOID CreateOpenAfterExecEntry (
//      PEDEXTRA prEDExtra
//      );
//
// This function adds an entry to the list with the latest executed files,
// if the current event is application execute.  This list keeps the last
// MAX_OEFILTER_ENTRIES executed files per Thread/VM.  If the limit has
// been reached, the oldest entry in the record will be replaced with the
// new one.  If the new entry matches one of the entries in the list, then
// the entry in the list will be replaced.
//
// Entry:
//      prEDExtra - an event data structure
//
// Exit:
//      -
//
//---------------------------------------------------------------------------
VOID CreateOpenAfterExecEntry (
    PEDEXTRA prEDExtra
    )
    {
    auto PTCB              hThread;
    auto HVM               hVM;
    auto UINT              uNumEntries;
    auto POPENAFTEREXECREC prCurrentEntry, prMatchedEntry;
    auto POPENAFTEREXECREC prEarliestEntry, prLatestEntry;

                                        // Validate input parameters.

    SYM_ASSERT ( prEDExtra );

                                        // Make sure the event is app exec.

    if ( prEDExtra->uEventType != SE_LDR_APPLICATION_EXECUTE )
        {
        return;
        }
                                        // Get task information.

    hThread = Get_Cur_Thread_Handle();

    hVM = Get_Cur_VM_Handle();

                                        // Search for entries.

    uNumEntries = 0;

    prMatchedEntry = prEarliestEntry = NULL;

    for ( prCurrentEntry = prOpenAfterExecListStart;
          prCurrentEntry;
          prCurrentEntry = prCurrentEntry->prNextEntry
        )
        {
                                        // See if entry matches the filter.

        if ( prCurrentEntry->hThread == hThread &&
             prCurrentEntry->hVM == hVM )
            {
                                        // Update total entry count.

            uNumEntries++;

                                        // See if entry matches filename.

            if ( STRCMP ( prCurrentEntry->szOEMFile,
                          EDXpszOEMFile(prEDExtra) ) == 0 )
                {
                prMatchedEntry = prCurrentEntry;
                }

                                        // Determine which is the earliest
                                        // and which is the latest entry.

            if ( prEarliestEntry )
                {
                if ( prCurrentEntry->uEnumerator <
                     prEarliestEntry->uEnumerator )
                    {
                    prEarliestEntry = prCurrentEntry;
                    }

                if ( prCurrentEntry->uEnumerator >
                     prLatestEntry->uEnumerator )
                    {
                    prLatestEntry = prCurrentEntry;
                    }
                }
            else
                {
                prEarliestEntry = prLatestEntry = prCurrentEntry;
                }
            }
        }

                                        // If entry not found, then create a
                                        // new one.

    if ( !prMatchedEntry )
        {
        if ( uNumEntries < MAX_OEFILTER_ENTRIES )
            {
                                        // Allocate new entry if there is
                                        // space.

            prCurrentEntry = (POPENAFTEREXECREC) MemAllocPtr (
                                                    GHND,
                                                    sizeof(*prCurrentEntry)
                                                    );

            if ( !prCurrentEntry )
                {
                UIOutOfMemoryError();
                }

                                        // Partially initialize the entry.

            if ( prCurrentEntry )
                {
                prCurrentEntry->prNextEntry = prOpenAfterExecListStart;
                prOpenAfterExecListStart    = prCurrentEntry;

                prCurrentEntry->hThread     = hThread;
                prCurrentEntry->hVM         = hVM;
                }
            }
        else
            {
                                        // Reuse the earliest entry.

            prCurrentEntry = prEarliestEntry;
            }

        if ( prCurrentEntry )
            {
            STRCPY ( prCurrentEntry->szOEMFile,
                     EDXpszOEMFile (prEDExtra) );
            }
        }
    else
        {
                                        // If entry matched, then just
                                        // update it.

        prCurrentEntry = prMatchedEntry;
        }

                                        // Set the count of the new entry.
                                        // Since it would take years to
                                        // execute 2^32 files, we won't check
                                        // for count overflow.

    if ( prCurrentEntry )
        {
        prCurrentEntry->uEnumerator = ( !prEarliestEntry ) ?
                                          0 :
                                          prLatestEntry->uEnumerator + 1;
        }
    }

//---------------------------------------------------------------------------
//
// BOOL IsItOpenAfterExec (
//      PEDEXTRA prEDExtra
//      );
//
// This function checks whether an open file is happening due to the exec
// of the same file.
//
// Entry:
//      prEDExtra - an event data structure
//
// Exit:
//      TRUE  if the open is right after an exec
//      FALSE if the open is not after an exec
//
//---------------------------------------------------------------------------
BOOL IsItOpenAfterExec (
    PEDEXTRA prEDExtra
    )
    {
    auto BOOL              bResult;
    auto PTCB              hThread;
    auto HVM               hVM;
    auto POPENAFTEREXECREC prCurrentEntry, prLastEntry;

                                        // Validate input parameters.

    SYM_ASSERT ( prEDExtra );

                                        // Make sure the event is file open,
                                        // and it is not upon open completed.
                                        // Also we may want to check the
                                        // open mode and make sure that the
                                        // mode allows writing to the file.

    if ( prEDExtra->uEventType != SE_FS_FILE_OPEN ||
         prEDExtra->bEventBack )
        {
                                        // Just in case, we should notify of
                                        // any file that is being opened.

        return ( FALSE );
        }
                                        // Get task information.

    hThread = Get_Cur_Thread_Handle();

    hVM = Get_Cur_VM_Handle();

                                        // Search for entry.

    prLastEntry = NULL;

    prCurrentEntry = prOpenAfterExecListStart;

    while ( prCurrentEntry &&
            ( prCurrentEntry->hThread != hThread ||
              prCurrentEntry->hVM != hVM ||
              STRCMP ( prCurrentEntry->szOEMFile, EDXpszOEMFile(prEDExtra) ) != 0 )
          )
        {
        prLastEntry = prCurrentEntry;

        prCurrentEntry = prCurrentEntry->prNextEntry;
        }

                                        // If entry is found, then check the
                                        // open mode.

    bResult = FALSE;

    if ( prCurrentEntry )
        {
                                        // Set the result.

        bResult = prEDExtra->prSEEventData->SE_ED_dwAttribute == OF_READ ?
                      TRUE : FALSE;

                                        // Remove the entry from the queue.

        if ( !prLastEntry )
            {
            prOpenAfterExecListStart = prCurrentEntry->prNextEntry;
            }
        else
            {
            prLastEntry->prNextEntry = prCurrentEntry->prNextEntry;
            }

        MemFreePtr ( prCurrentEntry );
        }

                                        // Not during exec.

    return ( bResult );
    }

//---------------------------------------------------------------------------
//
// VOID CleanupOpenAfterExec (
//      BOOL bCleanupAll,
//      PTCB hThread,
//      HVM  hVM
//      );
//
// This function cleans up the the list created by IsItOpenAfterExec().
//
// Entry:
//      bCleanupAll - TRUE if complete cleanup, FALSE if hThread/hVM
//                    cleanup
//      hThread     - Specifies the thread entries to be cleaned up.  Used
//                    only if bCleanupAll is FALSE.
//      hVM         - Specifies the VM entries to be cleaned up.  Used only
//                    if bCleanupAll is FALSE.
//
// Exit:
//      -
//
//---------------------------------------------------------------------------
VOID CleanupOpenAfterExec (
    BOOL bCleanupAll,
    PTCB hThread,
    HVM  hVM
    )
    {
    auto POPENAFTEREXECREC prCurrentEntry, prPrevEntry, prNextEntry;

                                        // Initialize local variables.

    prCurrentEntry = prOpenAfterExecListStart;
    prPrevEntry    = \
    prNextEntry    = NULL;

                                        // Browse through the list.

    while ( prCurrentEntry )
        {
        prNextEntry = prCurrentEntry->prNextEntry;

                                        // See if entry should be cleaned up.

        if ( bCleanupAll ||
             ( hThread == prCurrentEntry->hThread ||
               hVM == prCurrentEntry->hVM ) )
            {
                                        // Unlink it.

            if ( !prPrevEntry )
                {
                prOpenAfterExecListStart = prNextEntry;
                }
            else
                {
                prPrevEntry->prNextEntry = prNextEntry;
                }

                                        // Clean it up.

            MemFreePtr ( prCurrentEntry );
            }
        else
            {
            prPrevEntry = prCurrentEntry;
            }

        prCurrentEntry = prNextEntry;
        }
    }
