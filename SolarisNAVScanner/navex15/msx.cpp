// Copyright 1998 Symantec, SABU
//************************************************************************
//
// $Header:   S:/NAVEX/VCS/Msx.cpv   1.1   09 Dec 1998 17:45:32   DCHI  $
//
// Description:
//  Contains the following functions:
//
//      BOOL MSXDBWrite()
//      BOOL MSXDBRead()
//      BOOL MSXDBUpdate()
//      BOOL MSXFileIsExcluded()
//      BOOL MSXAddFile()
//      BOOL MSXStartUp()
//      BOOL MSXShutDown()
//
//************************************************************************
// $Log:   S:/NAVEX/VCS/Msx.cpv  $
// 
//    Rev 1.1   09 Dec 1998 17:45:32   DCHI
// Added #include "gdefines.h"
// 
//    Rev 1.0   08 Dec 1998 12:53:18   DCHI
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
#include "msx.h"

// MSX globals

#if defined(SYM_VXD)
char        gszMSXFileName[2 * SYM_MAX_PATH * sizeof(WCHAR)];
#elif defined(SYM_NTK)
TCHAR       gszMSXFileName[2 * SYM_MAX_PATH];
#elif defined(SYM_WIN)
TCHAR       gszMSXFileName[2 * SYM_MAX_PATH];
#else
char        gszMSXFileName[SYM_MAX_PATH];
#endif

// Assume MSX is enabled

int gnMSXEnabled = 1;

//********************************************************************
//
// BOOL MSXDBWrite()
//
// Parameters:
//  lpstDB              Ptr to database to write
//  lpstCallBack        Ptr to callback structure
//  hFile               Handle to MSX database file
//
// Description:
//  The function computes the CRC on the MRU field, the LRU field,
//  and all entries including.  It then sets the database's
//  dwCRCCheck field to the CRC.
//
//  Finally, the function writes the database to offset zero
//  and truncates the file.
//
// Returns:
//  TRUE                On success
//  FALSE               On error
//
//********************************************************************

BOOL MSXDBWrite
(
    LPMSX_DB            lpstDB,
    LPCALLBACKREV2      lpstCallBack,
    HFILE               hFile
)
{
    // Initialize the CRC

    CRC32Init(lpstDB->dwCRCCheck);

    // CRC the MRU and LRU fields

    CRC32Continue(lpstDB->dwCRCCheck,lpstDB->byMRU);
    CRC32Continue(lpstDB->dwCRCCheck,lpstDB->byLRU);

    // Take a CRC on the entries

    lpstDB->dwCRCCheck = CRC32Compute(MSX_DB_NUM_ENTRIES *
                                          sizeof(MSX_DBENTRY_T),
                                      (LPBYTE)lpstDB->astEntries,
                                      lpstDB->dwCRCCheck);

    // Now write it

    if (lpstCallBack->FileSeek(hFile,0,SEEK_SET) != 0)
        return(FALSE);

    if (lpstCallBack->FileWrite(hFile,
                                lpstDB,
                                sizeof(MSX_DB_T)) != sizeof(MSX_DB_T))
        return(FALSE);

    // Truncate it here

    if (lpstCallBack->FileWrite(hFile,
                                NULL,
                                0) == (UINT)-1)
        return(FALSE);

    return(TRUE);
}


//********************************************************************
//
// BOOL MSXDBRead()
//
// Parameters:
//  lpstDB              Ptr to database to read into
//  lpstCallBack        Ptr to callback structure
//  hFile               Handle to MSX database file
//
// Description:
//  The function seeks to offset zero of the given file
//  and reads the database.  It then computes a CRC on all
//  the entries and compares it against the dwCRCCheck field.
//
//  If the CRC matches, it then verifies that the MRU indexes
//  are all valid using the following checks:
//  1. Starting with the MRU entry, traverse the number of entries
//     in the database by following the byNext links.  The last
//     entry should be given by the LRU entry and all entries
//     should have been touched.
//  2. Starting with the LRU entry, traverse the number of entries
//     in the database by following the byPrev links.  The last
//     entry should be given by the MRU entry and all entries
//     should have been touched.
//  3. While performing the above traversals, the function also
//     verifies that the indexes are within bounds.
//
// Returns:
//  TRUE                On success
//  FALSE               On error
//
//********************************************************************

BOOL MSXDBRead
(
    LPMSX_DB            lpstDB,
    LPCALLBACKREV2      lpstCallBack,
    HFILE               hFile
)
{
    DWORD               dwCRC;
    int                 i;
    int                 nIndex;
    BYTE                abyTouched[MSX_DB_NUM_ENTRIES];

    // Read the database

    if (lpstCallBack->FileSeek(hFile,0,SEEK_SET) != 0)
        return(FALSE);

    if (lpstCallBack->FileRead(hFile,
                               lpstDB,
                               sizeof(MSX_DB_T)) != sizeof(MSX_DB_T))
        return(FALSE);

    // Validate the signature and version

    if (lpstDB->dwSignature != MSX_SIGNATURE ||
        lpstDB->dwVersion != MSX_VERSION)
        return(FALSE);

    ////////////////////////////////////////////////////////
    // Verify the CRC

    // Initialize the CRC

    CRC32Init(dwCRC);

    // CRC the MRU and LRU fields

    CRC32Continue(dwCRC,lpstDB->byMRU);

    // Compilation on DX complains, so using this

    dwCRC = CRC32Compute(1,&lpstDB->byLRU,dwCRC);

    // Take a CRC on the entries

    dwCRC = CRC32Compute(MSX_DB_NUM_ENTRIES * sizeof(MSX_DBENTRY_T),
                         (LPBYTE)lpstDB->astEntries,
                         dwCRC);

    // Validate the CRC

    if (dwCRC != lpstDB->dwCRCCheck)
        return(FALSE);

    ////////////////////////////////////////////////////////
    // Verify the forward links

    // Zero the touched array

    for (i=0;i<MSX_DB_NUM_ENTRIES;i++)
        abyTouched[i] = 0;

    // Follow the next links

    nIndex = lpstDB->byMRU;
    i = 0;
    while (1)
    {
        // Validate the index

        if (nIndex < 0 || MSX_DB_NUM_ENTRIES <= nIndex)
            return(FALSE);

        // Touch it

        abyTouched[nIndex] = 1;

        if (++i == MSX_DB_NUM_ENTRIES)
            break;

        // Get the next index

        nIndex = lpstDB->astEntries[nIndex].byNext;
    }

    // Verify that the last index is the LRU index

    if (nIndex != lpstDB->byLRU)
        return(FALSE);

    // Verify that all entries have been touched

    for (i=0;i<MSX_DB_NUM_ENTRIES;i++)
    {
        if (abyTouched[i] == 0)
            return(FALSE);
    }

    ////////////////////////////////////////////////////////
    // Verify the reverse links

    // Zero the touched array

    for (i=0;i<MSX_DB_NUM_ENTRIES;i++)
        abyTouched[i] = 0;

    // Follow the previous links

    nIndex = lpstDB->byLRU;
    i = 0;
    while (1)
    {
        // Validate the index

        if (nIndex < 0 || MSX_DB_NUM_ENTRIES <= nIndex)
            return(FALSE);

        // Touch it

        abyTouched[nIndex] = 1;

        if (++i == MSX_DB_NUM_ENTRIES)
            break;

        // Get the next index

        nIndex = lpstDB->astEntries[nIndex].byPrev;
    }

    // Verify that the last index is the MRU index

    if (nIndex != lpstDB->byMRU)
        return(FALSE);

    // Verify that all entries have been touched

    for (i=0;i<MSX_DB_NUM_ENTRIES;i++)
    {
        if (abyTouched[i] == 0)
            return(FALSE);
    }

    ////////////////////////////////////////////////////////
    // Initialize depth

    lpstDB->byMRUDepth = 0;

    return(TRUE);
}


//********************************************************************
//
// Function:
//  BOOL MSXDBUpdate()
//
// Parameters:
//  lpstCallBack        Ptr to callback structure
//
// Description:
//  The function assumes that it has mutually exclusive access
//  to the database.
//
//  The function opens the database file and allocates a database
//  structure into which to read the database file.
//
//  If reading the database fails, the in-memory database is
//  simply just written to the file.
//
//  Otherwise, if the date of the database file is different than
//  that of the in-memory database, the database file is copied
//  to the in-memory database.
//
//  In the remaining case, both databases have the same date.
//  The entries updated since the last file update are moved
//  to the top of the MRU list in the database file.
//
// Returns:
//  TRUE                On success
//  FALSE               On error
//
//********************************************************************

BOOL MSXDBUpdate
(
    LPCALLBACKREV2      lpstCallBack
)
{
    BOOL                bResult;
    HFILE               hFile;
    LPMSX_DB            lpstDB;

    /////////////////////////////////////////////////////////////
    // Open VMSCANXC.DAT

    hFile = lpstCallBack->FileOpen(gszMSXFileName,2);

    if (hFile == (HFILE)-1)
    {
        hFile = lpstCallBack->FileCreate(gszMSXFileName,0);
        if (hFile == (HFILE)-1)
            return(TRUE);
    }

    // Allocate memory to load the database from disk

    lpstDB = (LPMSX_DB)lpstCallBack->PermMemoryAlloc(sizeof(MSX_DB_T));
    if (lpstDB == NULL)
    {
        lpstCallBack->FileClose(hFile);
        return(FALSE);
    }

    // Read the database

    bResult = MSXDBRead(lpstDB,
                        lpstCallBack,
                        hFile);

    if (bResult != FALSE)
    {
        // Does the database on disk have a different date?

        if (lpstDB->byMonth != gstMSXDB.byMonth ||
            lpstDB->byDay != gstMSXDB.byDay ||
            lpstDB->byYear != gstMSXDB.byYear)
        {
            // Copy it to our database

            gstMSXDB = *lpstDB;
        }
        else
        {
            // Update the database

            MSXDBMergeMRUEntries(&gstMSXDB,lpstDB);

            // Write the updated database

            bResult = MSXDBWrite(lpstDB,
                                 lpstCallBack,
                                 hFile);
        }
    }
    else
    {
        // Write the database

        bResult = MSXDBWrite(&gstMSXDB,
                             lpstCallBack,
                             hFile);
    }

    // Free the memory

    lpstCallBack->PermMemoryFree(lpstDB);

    // Close the file

    lpstCallBack->FileClose(hFile);

    return(TRUE);
}


//********************************************************************
//
// Function:
//  BOOL MSXFileIsExcluded()
//
// Parameters:
//  lpstCallBack        Ptr to callback structure
//  hFile               Handle to file
//
// Description:
//  The function determines whether the given file is in the
//  macro scan exclusion database.
//
// Returns:
//  TRUE                If yes
//  FALSE               If no or on error
//
//********************************************************************

BOOL MSXFileIsExcluded
(
    LPCALLBACKREV2      lpstCallBack,
    HFILE               hFile
)
{
    LPSS_ROOT           lpstRoot;
    LPSS_STREAM         lpstStream;
    MSX_T               stMSX;
    int                 nResult;

    // Return immediately if MSX is not enabled

    if (gnMSXEnabled == 0)
        return(FALSE);

    /////////////////////////////////////////////////////////////
    // Allocate root and stream structures and working buffer

    // Create root structure

    if (SSCreateRoot(&lpstRoot,
                     lpstCallBack,
                     (LPVOID)hFile,
                     SS_ROOT_FLAG_DIR_CACHE_DEF |
                      SS_ROOT_FLAG_FAT_CACHE_DEF) != SS_STATUS_OK)
    {
        // Failed to create root structure

        return(FALSE);
    }

    // Create stream structure

    if (SSAllocStreamStruct(lpstRoot,
                            &lpstStream,
                            SS_STREAM_FLAG_DEF_BAT_CACHE) != SS_STATUS_OK)
    {
        // First destroy the root structure

        SSDestroyRoot(lpstRoot);

        // Failed to create stream structure

        return(FALSE);
    }

    // Allocate working buffer

    stMSX.lpvCookie = lpstCallBack;
    stMSX.nWorkBufSize = 8192;
    stMSX.lpabyWorkBuf = (LPBYTE)lpstCallBack->
        PermMemoryAlloc(stMSX.nWorkBufSize);
    stMSX.lpstDB = &gstMSXDB;

    if (stMSX.lpabyWorkBuf == NULL)
    {
        SSFreeStreamStruct(lpstStream);
        SSDestroyRoot(lpstRoot);
        return(FALSE);
    }

    /////////////////////////////////////////////////////////////
    // Perform filtering

    // Do L1 filter

    nResult = MSXL1Filter(&stMSX,
                          lpstRoot,
                          lpstStream);

    if (nResult == MSX_L2_FILTER)
    {
        // Do L2 filter

        nResult = MSXL2Filter(&stMSX,
                              lpstRoot,
                              lpstStream);
    }

    /////////////////////////////////////////////////////////////
    // Free the memory

    lpstCallBack->PermMemoryFree(stMSX.lpabyWorkBuf);
    SSFreeStreamStruct(lpstStream);
    SSDestroyRoot(lpstRoot);

    // Return a result

    if (nResult == MSX_ALL_CLEAN)
        return(TRUE);

    // Need to scan

    return(FALSE);
}


//********************************************************************
//
// BOOL MSXAddFile()
//
// Parameters:
//  lpstCallBack        Ptr to callback structure
//  hFile               Handle to file to add
//
// Description:
//
// Returns:
//  TRUE                On success
//  FALSE               On error
//
//********************************************************************

BOOL MSXAddFile
(
    LPCALLBACKREV2      lpstCallBack,
    HFILE               hFile
)
{
    LPSS_ROOT           lpstRoot;
    LPSS_STREAM         lpstStream;
    MSX_T               stMSX;
    BOOL                bResult = TRUE;

    // Return immediately if MSX is not enabled

    if (gnMSXEnabled == 0)
        return(TRUE);

    /////////////////////////////////////////////////////////////
    // Allocate root and stream structures and working buffer

    // Create root structure

    if (SSCreateRoot(&lpstRoot,
                     lpstCallBack,
                     (LPVOID)hFile,
                     SS_ROOT_FLAG_DIR_CACHE_DEF |
                      SS_ROOT_FLAG_FAT_CACHE_DEF) != SS_STATUS_OK)
    {
        // Failed to create root structure

        return(FALSE);
    }

    // Create stream structure

    if (SSAllocStreamStruct(lpstRoot,
                            &lpstStream,
                            SS_STREAM_FLAG_DEF_BAT_CACHE) != SS_STATUS_OK)
    {
        // First destroy the root structure

        SSDestroyRoot(lpstRoot);

        // Failed to create stream structure

        return(FALSE);
    }

    // Allocate working buffer

    stMSX.lpvCookie = lpstCallBack;
    stMSX.nWorkBufSize = 8192;
    stMSX.lpabyWorkBuf = (LPBYTE)lpstCallBack->
        PermMemoryAlloc(stMSX.nWorkBufSize);
    stMSX.lpstDB = &gstMSXDB;

    if (stMSX.lpabyWorkBuf == NULL)
    {
        SSFreeStreamStruct(lpstStream);
        SSDestroyRoot(lpstRoot);
        return(FALSE);
    }

    // Add it

    if (MSXAddCRCs(&stMSX,
                   lpstRoot,
                   lpstStream) == FALSE)
        bResult = FALSE;

    // Wait for the mutex

    if (bResult != FALSE &&
        glpvMSXMutex != NULL &&
        lpstCallBack->WaitMutex(glpvMSXMutex,100) != 0)
    {
        // Update the on-disk database

        if (MSXDBUpdate(lpstCallBack) == FALSE)
            bResult = FALSE;

        // Release the mutex

        lpstCallBack->ReleaseMutex(glpvMSXMutex);
    }
    else
        bResult = FALSE;

    /////////////////////////////////////////////////////////////
    // Free the memory

    lpstCallBack->PermMemoryFree(stMSX.lpabyWorkBuf);
    SSFreeStreamStruct(lpstStream);
    SSDestroyRoot(lpstRoot);

    // Return result

    return(bResult);
}


//********************************************************************
//
// BOOL MSXStartUp()
//
// Parameters:
//  lpstCallBack        Ptr to callback structure
//  lpszDataDir         Ptr to data directory
//
// Description:
//
// Returns:
//  TRUE                On success
//  FALSE               On error
//
//********************************************************************

BOOL MSXStartUp
(
    LPCALLBACKREV2      lpstCallBack,
    LPTSTR              lpszDataDir
)
{
    BOOL                bResult = TRUE;
    HFILE               hFile;
    BYTE                abyV6MonthDayYear[3];

#if defined(SYM_VXD)
    char        szFileName[2 * SYM_MAX_PATH * sizeof(WCHAR)];
#elif defined(SYM_NTK)
    TCHAR       szFileName[2 * SYM_MAX_PATH];
#elif defined(SYM_WIN)
    TCHAR       szFileName[2 * SYM_MAX_PATH];
#else
    char        szFileName[SYM_MAX_PATH];
#endif

    // Initialize the mutex ptr to NULL

    glpvMSXMutex = NULL;

    // Generate the filenames first

#if defined(SYM_UNIX)
    DataDirAppendName(lpszDataDir,_T("VIRSCAN6.DAT"),szFileName);
    DataDirAppendName(lpszDataDir,_T("VSCANMSX.DAT"),gszMSXFileName);
#else
    DataDirAppendName(lpszDataDir,_T("virscan6.dat"),szFileName);
    DataDirAppendName(lpszDataDir,_T("vscanmsx.dat"),gszMSXFileName);
#endif

    // Zero the database

    MSXDBZero(&gstMSXDB);

    /////////////////////////////////////////////////////////////
    // Open VIRSCAN6.DAT to get the date

    hFile = lpstCallBack->FileOpen(szFileName,0);

    if (hFile == (HFILE)-1)
        return(TRUE);

    // Get the month, day, and year from offset eight

    if (lpstCallBack->FileSeek(hFile,8,SEEK_SET) != 8)
    {
        lpstCallBack->FileClose(hFile);
        return(TRUE);
    }

    if (lpstCallBack->FileRead(hFile,abyV6MonthDayYear,3) != 3)
    {
        lpstCallBack->FileClose(hFile);
        return(TRUE);
    }

    // Close the file

    lpstCallBack->FileClose(hFile);

    /////////////////////////////////////////////////////////////
    // Open VMSCANXC.DAT to get the date

    hFile = lpstCallBack->FileOpen(gszMSXFileName,0);

    if (hFile == (HFILE)-1)
        bResult = FALSE;
    else
    {
        // Read the database

        bResult = MSXDBRead(&gstMSXDB,
                            lpstCallBack,
                            hFile);

        // Close the file

        lpstCallBack->FileClose(hFile);
    }

    // If there was an error reading the database or if
    //  the date doesn't match that of VIRSCAN6.DAT
    //  reinitialize it.

    if (bResult == FALSE ||
        abyV6MonthDayYear[0] != gstMSXDB.byMonth ||
        abyV6MonthDayYear[1] != gstMSXDB.byDay ||
        abyV6MonthDayYear[2] != gstMSXDB.byYear)
    {
        // Zero the database

        MSXDBZero(&gstMSXDB);

        // Set the date

        gstMSXDB.byMonth = abyV6MonthDayYear[0];
        gstMSXDB.byDay = abyV6MonthDayYear[1];
        gstMSXDB.byYear = abyV6MonthDayYear[2];

        // Delete the database

        lpstCallBack->FileDelete(gszMSXFileName);
    }

    // Allocate the mutex

    if (lpstCallBack->CreateMutex(&glpvMSXMutex) == 0)
    {
        glpvMSXMutex = NULL;
        return(FALSE);
    }

    return(TRUE);
}


//********************************************************************
//
// BOOL MSXShutDown()
//
// Parameters:
//  lpstCallBack        Ptr to callback structure
//
// Description:
//  If the database has not been modified, the function returns
//  immediately.
//
//  Otherwise it saves the database.
//
// Returns:
//  TRUE                On success
//  FALSE               On error
//
//********************************************************************

BOOL MSXShutDown
(
    LPCALLBACKREV2      lpstCallBack
)
{
    // Destroy the mutex

    if (glpvMSXMutex != NULL)
    {
        lpstCallBack->DestroyMutex(glpvMSXMutex);
        glpvMSXMutex = NULL;
    }

    return(TRUE);
}

