// Copyright 1998 Symantec, SABU
//************************************************************************
//
// $Header:   S:/NAVEX/VCS/msxdb.cpv   1.1   09 Dec 1998 17:45:30   DCHI  $
//
// Description:
//  Contains the following functions:
//
//      void MSXDBMRUMove()
//      BOOL MSXDBL1Match()
//      BOOL MSXDBL2Match()
//      BOOL MSXDBAdd()
//      void MSXDBZero()
//      void MSXDBMergeMRUEntries()
//
//************************************************************************
// $Log:   S:/NAVEX/VCS/msxdb.cpv  $
// 
//    Rev 1.1   09 Dec 1998 17:45:30   DCHI
// Added #include "gdefines.h"
// 
//    Rev 1.0   08 Dec 1998 12:53:04   DCHI
// Initial revision.
// 
//************************************************************************

#include "gdefines.h"

#include "platform.h"
#include "callback.h"
#include "datadir.h"

#include "olessapi.h"
#include "crc32.h"

#include "wd7api.h"
#include "xl5api.h"

#include "msxi.h"

// MSX globals

#ifdef SYM_WIN16
#pragma data_seg("FAR_DATA_0")
#endif

MSX_DB_T FAR    gstMSXDB;

#ifdef SYM_WIN16
#pragma data_seg()
#endif

LPVOID          glpvMSXMutex;

//********************************************************************
//
// Function:
//  void MSXDBMRUMove()
//
// Parameters:
//  lpstDB              Ptr to MSX database
//  nIndex              Index to move
//
// Description:
//  Moves the entry at the given index to the top of the MRU
//  list.
//
// Returns:
//  Nothing
//
//********************************************************************

void MSXDBMRUMove
(
    LPMSX_DB            lpstDB,
    int                 nIndex
)
{
    LPMSX_DBENTRY       lpstEntry;

    // If it is already the MRU entry, leave it

    if (lpstDB->byMRU == nIndex)
        return;

    // Get a pointer to the entry

    lpstEntry = lpstDB->astEntries + nIndex;

    // Is it the LRU entry?

    if (lpstDB->byLRU == nIndex)
    {
        // Take off the end

        lpstDB->byLRU = lpstEntry->byPrev;
    }
    else
    {
        // Take it out of the middle

        lpstDB->astEntries[lpstEntry->byPrev].byNext =
            lpstEntry->byNext;

        lpstDB->astEntries[lpstEntry->byNext].byPrev =
            lpstEntry->byPrev;
    }

    // Move it to the top

    lpstDB->astEntries[lpstDB->byMRU].byPrev = nIndex;
    lpstEntry->byNext = lpstDB->byMRU;
    lpstDB->byMRU = nIndex;

    // Increment the MRU depth

    if (lpstDB->byMRUDepth < MSX_DB_NUM_ENTRIES)
        lpstDB->byMRUDepth++;
}


//********************************************************************
//
// Function:
//  BOOL MSXDBL1Match()
//
// Parameters:
//  lpstMSX             Ptr to MSX structure
//  dwCRC               CRC to check
//  dwCRCSize           CRC size to check
//
// Description:
//  The function iterates through all entries of the database
//  looking for the given L1 CRC and CRC size match.  If found,
//  the function returns TRUE.  Otherwise, it returns FALSE.
//
// Returns:
//  TRUE                If a match is found
//  FALSE               If a match is not found
//
//********************************************************************

BOOL MSXDBL1Match
(
    LPMSX               lpstMSX,
    DWORD               dwCRC,
    DWORD               dwCRCSize
)
{
    int                 i;
    LPMSX_DBENTRY       lpstEntry;

    // Make sure CRC sizes of zero never match

    if (dwCRCSize == 0)
        return(FALSE);

    // If we don't have a mutex, we should be able to just
    //  read, since writes aren't allowed.
    // Otherwise, wait for the mutex

    if (glpvMSXMutex == NULL ||
        ((LPCALLBACKREV2)lpstMSX->lpvCookie)->
            WaitMutex(glpvMSXMutex,100) != 0)
    {
        // Perform a linear search

        lpstEntry = lpstMSX->lpstDB->astEntries;
        for (i=0;i<MSX_DB_NUM_ENTRIES;i++)
        {
            // Is it a match on the CRC and the low WORD of the CRC size?

            if (dwCRC == lpstEntry->dwL1CRC &&
                (WORD)(dwCRCSize & 0xFFFF) ==
                lpstEntry->wL1CRCSize)
            {
                if (glpvMSXMutex != NULL)
                {
                    MSXDBMRUMove(lpstMSX->lpstDB,i);
                    ((LPCALLBACKREV2)lpstMSX->lpvCookie)->
                        ReleaseMutex(glpvMSXMutex);
                }
                return(TRUE);
            }

            ++lpstEntry;
        }

        // Release the mutex

        if (glpvMSXMutex != NULL)
            ((LPCALLBACKREV2)lpstMSX->lpvCookie)->
                ReleaseMutex(glpvMSXMutex);
    }

    // It is not in the database

    return(FALSE);
}


//********************************************************************
//
// Function:
//  BOOL MSXDBL2Match()
//
// Parameters:
//  lpstMSX             Ptr to MSX structure
//  dwCRC               CRC to check
//  dwCRCSize           CRC size to check
//
// Description:
//  The function iterates through all entries of the database
//  looking for the given L2 CRC and CRC size match.  If found,
//  the function returns TRUE.  Otherwise, it returns FALSE.
//
// Returns:
//  TRUE                If a match is found
//  FALSE               If a match is not found
//
//********************************************************************

BOOL MSXDBL2Match
(
    LPMSX               lpstMSX,
    DWORD               dwCRC,
    DWORD               dwCRCSize
)
{
    int                 i;
    LPMSX_DBENTRY       lpstEntry;

    // Make sure CRC sizes of zero never match

    if (dwCRCSize == 0)
        return(FALSE);

    // If we don't have a mutex, we should be able to just
    //  read, since writes aren't allowed.
    // Otherwise, wait for the mutex

    if (glpvMSXMutex == NULL ||
        ((LPCALLBACKREV2)lpstMSX->lpvCookie)->
            WaitMutex(glpvMSXMutex,100) != 0)
    {
        // Perform a linear search

        lpstEntry = lpstMSX->lpstDB->astEntries;
        for (i=0;i<MSX_DB_NUM_ENTRIES;i++)
        {
            // Is it a match on the CRC and the low WORD of the CRC size?

            if (dwCRC == lpstEntry->dwL2CRC &&
                dwCRCSize == lpstEntry->dwL2CRCSize)
            {
                if (glpvMSXMutex != NULL)
                {
                    MSXDBMRUMove(lpstMSX->lpstDB,i);
                    ((LPCALLBACKREV2)lpstMSX->lpvCookie)->
                        ReleaseMutex(glpvMSXMutex);
                }
                return(TRUE);
            }

            ++lpstEntry;
        }

        // Release the mutex

        if (glpvMSXMutex != NULL)
            ((LPCALLBACKREV2)lpstMSX->lpvCookie)->
                ReleaseMutex(glpvMSXMutex);
    }

    // It is not in the database

    return(FALSE);
}


//********************************************************************
//
// Function:
//  BOOL MSXDBAdd()
//
// Parameters:
//  lpstMSX             Ptr to MSX structure
//  dwL1CRC             L1 CRC to add
//  dwL1CRCSize         Size of L1 CRC
//  dwL2CRC             L2 CRC to add
//  dwL2CRCSize         Size of L2 CRC
//
// Description:
//  The function modifies the LRU entry of the database with the
//  given entry and moves the entry to the MRU position.
//
// Returns:
//  TRUE                On success
//  FALSE               On failure
//
//********************************************************************

BOOL MSXDBAdd
(
    LPMSX               lpstMSX,
    DWORD               dwL1CRC,
    DWORD               dwL1CRCSize,
    DWORD               dwL2CRC,
    DWORD               dwL2CRCSize
)
{
    LPMSX_DBENTRY       lpstEntry;
    LPMSX_DB            lpstDB;

    // Do not add if both sizes are zero

    if (dwL1CRCSize == 0 && dwL2CRCSize == 0)
        return(TRUE);

    // Wait for the mutex

    if (glpvMSXMutex != NULL &&
        ((LPCALLBACKREV2)lpstMSX->lpvCookie)->
            WaitMutex(glpvMSXMutex,100) != 0)
    {
        lpstDB = lpstMSX->lpstDB;

        // Modify the LRU entry

        lpstEntry = lpstDB->astEntries + lpstDB->byLRU;

        lpstEntry->wL1CRCSize = (WORD)(dwL1CRCSize & 0xFFFF);
        lpstEntry->dwL1CRC = dwL1CRC;
        lpstEntry->dwL2CRCSize = dwL2CRCSize;
        lpstEntry->dwL2CRC = dwL2CRC;

        // Move the entry to the top

        MSXDBMRUMove(lpstDB,lpstDB->byLRU);

        // Release the mutex

        ((LPCALLBACKREV2)lpstMSX->lpvCookie)->
            ReleaseMutex(glpvMSXMutex);

		return(TRUE);
    }

    return(FALSE);
}


//********************************************************************
//
// Function:
//  void MSXDBZero()
//
// Parameters:
//  lpstDB              Ptr to database to zero
//
// Description:
//  The function sets the database signature to MSX_SIGNATURE,
//  the version to MSX_VERSION, and then zeroes out the
//  remaining fields of the database.
//
// Returns:
//  Nothing
//
//********************************************************************

void MSXDBZero
(
    LPMSX_DB            lpstDB
)
{
    int                 i;
    LPMSX_DBENTRY       lpstEntry;

    lpstDB->dwSignature = MSX_SIGNATURE;
    lpstDB->dwVersion = MSX_VERSION;
    lpstDB->byMonth = 0;
    lpstDB->byDay = 0;
    lpstDB->byYear = 0;
    lpstDB->byFiller = 0;
    lpstDB->dwCRCCheck = 0;
    lpstDB->byMRU = 0;
    lpstDB->byLRU = MSX_DB_NUM_ENTRIES - 1;
    lpstDB->byMRUDepth = 0;
    lpstDB->byFiller2 = 0;

    lpstEntry = lpstDB->astEntries;
    for (i=0;i<MSX_DB_NUM_ENTRIES;i++)
    {
        lpstEntry->byPrev = (BYTE)(i - 1);
        lpstEntry->byNext = (BYTE)(i + 1);
        lpstEntry->wL1CRCSize = 0;
        lpstEntry->dwL1CRC = 0;
        lpstEntry->dwL2CRCSize = 0;
        lpstEntry->dwL2CRC = 0;

        ++lpstEntry;
    }
}


//********************************************************************
//
// Function:
//  void MSXDBMergeMRUEntries()
//
// Parameters:
//  lpstSrcDB           Ptr to source database
//  lpstDstDB           Ptr to destination database
//
// Description:
//  The function assumes that it has mutually exclusive access
//  to the databases.
//
//  The function takes the top MRU entries as given by the
//  byMRUDepth field of the source database and either adds or
//  moves them in the same order to the destination database.
//
//  For each entry to move/add in the source database, a linear
//  search is made for its presence in the destination database.
//  It is added to the destination database if it is not already
//  present.  The entry is then moved to the top of the list.
//
// Returns:
//  Nothing
//
//********************************************************************

void MSXDBMergeMRUEntries
(
    LPMSX_DB            lpstSrcDB,
    LPMSX_DB            lpstDstDB
)
{
    int                 i;
    int                 j;
    int                 nIndex;
    LPMSX_DBENTRY       lpstSrcEntry;
    LPMSX_DBENTRY       lpstDstEntry;

    // Make sure that there is at least one entry to move/add

    if (lpstSrcDB->byMRUDepth == 0)
        return;

    // Iterate to the least MRU entry to move/add

    nIndex = lpstSrcDB->byMRU;
    for (i=1;i<lpstSrcDB->byMRUDepth;i++)
        nIndex = lpstSrcDB->astEntries[nIndex].byNext;

    // Now go in reverse order

    for (i=0;i<lpstSrcDB->byMRUDepth;i++)
    {
        // Get a ptr to the entry

        lpstSrcEntry = lpstSrcDB->astEntries + nIndex;

        // Is the entry in the destination?

        lpstDstEntry = lpstDstDB->astEntries;
        for (j=0;j<MSX_DB_NUM_ENTRIES;j++)
        {
            if (lpstSrcEntry->wL1CRCSize  == lpstDstEntry->wL1CRCSize  &&
                lpstSrcEntry->dwL1CRC     == lpstDstEntry->dwL1CRC     &&
                lpstSrcEntry->dwL2CRCSize == lpstDstEntry->dwL2CRCSize &&
                lpstSrcEntry->dwL2CRC     == lpstDstEntry->dwL2CRC)
            {
                // Found it

                break;
            }

            ++lpstDstEntry;
        }

        if (j == MSX_DB_NUM_ENTRIES)
        {
            // Add a new entry by modifying the LRU entry

            j = lpstDstDB->byLRU;
            lpstDstEntry = lpstDstDB->astEntries + j;

            lpstDstEntry->wL1CRCSize  = lpstSrcEntry->wL1CRCSize;
            lpstDstEntry->dwL1CRC     = lpstSrcEntry->dwL1CRC;
            lpstDstEntry->dwL2CRCSize = lpstSrcEntry->dwL2CRCSize;
            lpstDstEntry->dwL2CRC     = lpstSrcEntry->dwL2CRC;
        }

        // Move it to the top

        MSXDBMRUMove(lpstDstDB,j);

        // Get the next(previous) entry

        nIndex = lpstSrcEntry->byPrev;
    }
}





