/////////////////////////////////////////////////////////////////////
// INFO.CPP - Created - 06/01/98
//
// Copyright 1992-1998 Symantec, Peter Norton Product Group
/////////////////////////////////////////////////////////////////////
// $Header:   S:/NAVAPI/VCS/info.cpv   1.26   02 Dec 1998 19:44:10   MKEATIN  $
//
// Description:
//    This file contains all of the API implementation for virus
//    list enumeration and information gathering functions.
//
/////////////////////////////////////////////////////////////////////
//
// $Log:   S:/NAVAPI/VCS/info.cpv  $
// 
//    Rev 1.26   02 Dec 1998 19:44:10   MKEATIN
// reverted back to revision 1.24
// 
//    Rev 1.24   28 Oct 1998 15:07:02   MKEATIN
// NAVLoadVirusDef now loads a virus def from a virus ID instead of a
// virus index.
//
//    Rev 1.23   25 Aug 1998 13:51:32   dhertel
//
//    Rev 1.22   24 Aug 1998 18:38:30   dhertel
// Added range check to index parameter in NAVGetVirusDefTableInfo.
//
//    Rev 1.21   10 Aug 1998 13:05:08   DHERTEL
//
//    Rev 1.20   04 Aug 1998 15:28:30   DHERTEL
//
//    Rev 1.19   30 Jul 1998 21:51:52   DHERTEL
// Changes for NLM packaging of NAVAPI.
//
//    Rev 1.18   29 Jul 1998 18:34:02   DHERTEL
// DX and NLM changes
//
//    Rev 1.17   27 Jul 1998 15:12:22   MKEATIN
// Made changed to enable compiling under WIN16 as well as WIN32
//
//    Rev 1.16   23 Jul 1998 22:26:28   ksackin
// Added a function, NAVGetVirusDBInfo to retrieve the internal date and version
// of the virus definitions.
//
//    Rev 1.15   16 Jul 1998 14:32:04   MKEATIN
// Fixed comments and changed a strlen to STRLEN.
//
//    Rev 1.14   16 Jul 1998 14:24:10   MKEATIN
// Added NAVCALLAPI to NAVGetNavapiVersionString
//
//    Rev 1.13   15 Jul 1998 16:46:10   MKEATIN
// Added NAVGetNavapiVersionString
//
//    Rev 1.12   23 Jun 1998 11:45:54   ksackin
// Fixed one more NULL Engine handle crash.
//
//    Rev 1.11   23 Jun 1998 11:27:32   ksackin
// Fixed a couple of cases where we were not validating parameters before we
// used them.
//
//    Rev 1.10   23 Jun 1998 01:11:06   ksackin
// Fixed up all buffer size returns to be number of characters including NULL,
// as opposed to number of bytes.
//
//    Rev 1.9   22 Jun 1998 14:17:10   ksackin
// Fixed a problem with the IsNAVEnumItemBool() where I was not getting every
// BOOL value.
//
//    Rev 1.8   22 Jun 1998 13:06:02   ksackin
// Made sure that we are using the MapNAVVirInfotoVLVirInfo() function
// everywhere that we are passing a VLVirInfo to the virus engine.
//
//    Rev 1.7   20 Jun 1998 17:30:20   ksackin
// Removed the NAV_VI_FILENAME switch item.  We no longer support querying for
// the file name of an infected item, as the item does not have to be a file
// anymore.
//
//    Rev 1.6   17 Jun 1998 14:25:44   ksackin
// Removed some items from the enumeration enumNAVVirInfo, and added a mapping
// function to map our enumeration items back to the VLVIRINFO enumeration type.
//
//    Rev 1.5   09 Jun 1998 19:07:50   ksackin
// Updated return values for many of the functions, and commented function
// headers a little more completely.
//
//    Rev 1.4   08 Jun 1998 00:34:32   ksackin
// Added NAVGetVirusInfo() to query information about an infected virus handle.
//
//    Rev 1.3   08 Jun 1998 00:04:48   ksackin
// Changed the table type virus list loading functions to use a structure which
// contains the number of entries and the entry list instead of simply relying
// on there being a NULL terminator.  Also, added a new API to get the number of
// entries in an HNAVVirusEntryTable structure.  This will report back the
// actual number of entries contained in the loaded list.
//
//    Rev 1.2   01 Jun 1998 22:51:36   ksackin
// Updated functions who call GetTrueNumVirusDefinitions to call new function.
//
//    Rev 1.1   01 Jun 1998 22:49:20   ksackin
// Changed the function name of NAVGetTrueNumVirusDefinitions to
// NAVGetNumVirusDefinitions.  This will be how we access this externally.
// Internally, we still will call to the GetTrueNum function.
//
//    Rev 1.0   01 Jun 1998 22:13:52   ksackin
// Initial revision.
//
/////////////////////////////////////////////////////////////////////

#include "platform.h"
#include "stdlib.h"

#include "nlm_nav.h"
#include "navapi.h"
#include "avapi_l.h"
#include "napiver.h"


// This structure will wrap the VirusTable functionality
// to hide the actual list implementation.
typedef struct tag_VirusDefTable
{
    DWORD               dwVirusDefCount;
#if defined(SYM_WIN16)
    HNAVVIRUSDEF _huge* lpNAVVirusDefList;
#else
    HNAVVIRUSDEF*       lpNAVVirusDefList;
#endif
} VIRUSDEFTABLE, FAR *LPVIRUSDEFTABLE;


// MAX_VIRUS_INFO is the maximum number of characters (TCHARS)
// that can be returned by VlVirusInfo() and VLDefInfo().
// The two biggest data items returned are the virus comment
// (VI_VIRUS_INFO: SIZE_COMMENT) and the file name (VI_ITEM_NAME,
// VI_FILENAME: SYM_MAX_PATH+1).
#define MAX_VIRUS_INFO SIZE_COMMENT


/////////////////////////////////////////////////////////////////////
// Function Prototypes.

LPVIRUSDEFTABLE CreateVirusDefTable
(
    DWORD dwVirusDefCount
);

BOOL IsNAVEnumItemBOOL
(
    NAVVIRINFO enumItem
);

VLVIRINFO MapNAVVirInfoToVLVirInfo
(
    NAVVIRINFO enumNAVVirInfo
);



/////////////////////////////////////////////////////////////////////
// API function implementation.


//************************************************************************
//
// DWORD NAVCALLAPI NAVGetVirusInfo
// (
//     HNAVVIRUS   hNAVVirus,       - [in]  Open virus handle.
//     NAVVIRINFO  enumVirInfo,     - [in]  Item to retrieve information on.
//     LPTSTR      lpszResultBuffer,- [out] Result of query.
//     LPDWORD     lpdwBufferSize   - [in/out]
//                                    in  - Size of return buffer.
//                                    out - Number of characters returned,
//                                          or buffer size needed.
// )
//
// Purpose:
//     This routine will get information on a particular virus.
//     Use the ID's associated with the various information to query for
//     information such as virus name, alias, and infection type.
//
// Returns:
//     BOOL info :
//          TRUE  - Item queried is TRUE.
//          FALSE - Item queried is FALSE.
//
//     String Info:
//          NAV_OK    - Information retrieved successfully.
//          NAV_ERROR - Failed to retrieve requested information.
//
//          NAV_INSUFFICIENT_BUFFER - Buffer was too short.  Actual buffer size
//                                    needed is returned in lpdwBufferSize.
//
//************************************************************************
// 05/28/98 Created by KSACKIN
//************************************************************************
DWORD NAVCALLAPI NAVGetVirusInfo
(
    HNAVVIRUS   hNAVVirus,
    NAVVIRINFO  enumVirInfo,
    LPTSTR      lpszResultBuffer,
    LPDWORD     lpdwBufferSize
)
{
    DWORD   dwReturn = NAV_OK;

    // Make sure we have a valid virus handle.
    if ( NULL == hNAVVirus )
    {
        // Return failure.
        return NAV_ERROR;
    }

    // If we are only querying for a BOOL, perform this query now.
    if ( IsNAVEnumItemBOOL( enumVirInfo )  )
    {
        // Return TRUE or FALSE depending on the query.
        return VLVirusInfo( (HVLVIRUS)hNAVVirus,
                            0, // Hard code infection level to 0 for now.
                            MapNAVVirInfoToVLVirInfo(enumVirInfo),
                            NULL );
    }

    // This is for efficiency.  If our buffer is long enough, pass straight
    // through to API, otherwise, we need to double buffer to ensure correct
    // size.
    if ( *lpdwBufferSize >= MAX_VIRUS_INFO )
    {
        // Call through to NAV's function to get the definition information.
        if ( VLVirusInfo(hNAVVirus,
                         0, // Hard code infection level to 0 for now.
                         MapNAVVirInfoToVLVirInfo(enumVirInfo),
                         lpszResultBuffer ) )
        {
            // Set our return buffer size.
            *lpdwBufferSize = STRLEN( lpszResultBuffer );
        }
        else
        {
            // Set our return value.
            dwReturn = NAV_ERROR;
        }
    }
    else
    {
        // In this scenario, lpszResultBuffer could be too short.  For this
        // reason, we need to allocate a temporary buffer, and make sure our
        // buffer is going to fit all of the information.

        TCHAR       szTempBuffer[MAX_VIRUS_INFO];

        // Initialize our string.
        szTempBuffer[0] = EOS;

        // Call through to NAV's function to get the definition information.
        if ( VLVirusInfo( hNAVVirus,
                          0, // Hard code infection level to 0 for now.
                          MapNAVVirInfoToVLVirInfo(enumVirInfo),
                          szTempBuffer ) )
        {
            // Make sure that our return string is long enough.
            if ( STRLEN( szTempBuffer ) <= *lpdwBufferSize )
            {

                // Copy in our return string.
                STRCPY( lpszResultBuffer, szTempBuffer );

                // Set our return buffer size.
                *lpdwBufferSize = STRLEN( lpszResultBuffer );
            }
            else
            {
                // Return insufficient buffer.
                dwReturn = NAV_INSUFFICIENT_BUFFER;

                // Set our required buffer size (including NULL).
                *lpdwBufferSize = STRLEN( lpszResultBuffer ) + 1;
            }

        }
        else
        {
            // Return failure.
            dwReturn = NAV_ERROR;
        }
    }

    // Return our success.
    return dwReturn;
}


//************************************************************************
//
// NAVSTATUS NAVCALLAPI NAVGetVirusDefCount
// (
//     HNAVENGINE  hNAVEngine     - [in]  Open handle to a NAV Engine.
//     DWORD*      lpdwVirusCount - [out] Number of viruses in definition set.
// )
//
// Purpose:
//     This routine will get the number of virus definitions contained
//     within the definitions files accessible by the passed in HNAVENGINE.
//
// Returns:
//     NAV_OK    - Call successfull.  Virus definition count have been put
//                 into returned DWORD.
//     NAV_ERROR - Failed to get the number of viruses.
//
//************************************************************************
// 05/28/98 Created by KSACKIN
//************************************************************************
NAVSTATUS NAVCALLAPI NAVGetVirusDefCount
(
    HNAVENGINE  hNAVEngine,
    DWORD*      lpdwVirusCount
)
{
    SYM_ASSERT( hNAVEngine );

    // Initialize the return value.
    *lpdwVirusCount = 0;

    // Catch NULL engine handle case.
    if ( NULL == hNAVEngine )
        {
        return NULL;
        }

    // Get the number of viruses from the NAV Engine.
    *lpdwVirusCount = VLTrueNumVirusEntries( (HVCONTEXT) hNAVEngine );

    // If we actually got a number of viruses back, return OK...
    if ( *lpdwVirusCount )
        return NAV_OK;

    // A failure has ocurred.  Return NAV_ERROR.
    return NAV_ERROR;
}


//************************************************************************
//
// HNAVVIRUSDEF NAVCALLAPI NAVLoadVirusDef
// (
//     HNAVENGINE  hNAVEngine, - [in] Handle to an open NAV Engine.
//     DWORD       dwVirusID   - [in] ID of Virus to load.
// )
//
// Purpose:
//     This routine will get a virus definition from the virus database and
//     return a handle to the virus definition.
//
// Returns:
//     HNAVVIRUSDEF - Open handle to a virus definition.
//     NULL         - Failed to open a handle to the selected definition.
//
//************************************************************************
// 05/28/98 Created by KSACKIN
//************************************************************************
HNAVVIRUSDEF NAVCALLAPI NAVLoadVirusDef
(
    HNAVENGINE  hNAVEngine,
    DWORD       dwVirusID
)
{
    HVLVIRUSENTRY   hvVirusEntry;
    DWORD           dwDefIndex;

    SYM_ASSERT( hNAVEngine );

    // Catch NULL engine handle case.
    if ( NULL == hNAVEngine )
    {
        return NULL;
    }

    // Convert our ID to it's virscan definition index
    if (VLDetermineVirusIndex( (HVCONTEXT)hNAVEngine,
                               (WORD)dwVirusID,
                               (LPWORD)&dwDefIndex ));

    // Get this entry from the database.
    hvVirusEntry = VLGetVirusEntry( (HVCONTEXT)hNAVEngine,
                                    (UINT)dwDefIndex );

    // Return this handle as an HNAVVIRUSDEF.
    return (HNAVVIRUSDEF) hvVirusEntry;
}


//************************************************************************
//
// VOID NAVCALLAPI NAVReleaseVirusDef
// (
//     HNAVVIRUSDEF hNAVVirusDef - [in] Handle to an open virus definition.
// )
//
// Purpose:
//     This routine will release a virus definition handle gotten from a
//     call to NavLoadVirusDef().
//
// Returns:
//     None.
//
//************************************************************************
// 05/28/98 Created by KSACKIN
//************************************************************************
VOID NAVCALLAPI NAVReleaseVirusDef
(
    HNAVVIRUSDEF    hNAVVirusDef
)
{
    // Release this definition.
    VLReleaseEntry( (HVLVIRUSENTRY)hNAVVirusDef );
}


//************************************************************************
//
// DWORD NAVCALLAPI NAVGetVirusDefInfo
// (
//     HNAVVIRUSDEF    hNAVVirusDef,    - [in]  Open virus definition.
//     NAVVIRINFO      enumVirInfo,     - [in]  Item to retrieve information on.
//     LPTSTR          lpszResultBuffer,- [out] Result of query.
//     LPDWORD         lpdwBufferSize   - [in/out]
//                                        in  - Size of return buffer.
//                                        out - Number of characters returned,
//                                              or buffer size needed.
// )
//
// Purpose:
//     This routine will get information on a particular virus definition.
//     Use the ID's associated with the various information to query for
//     items such as virus name, alias, and infection type.
//
// Remarks:
//     To improve efficiency and ensure successfull result, you should pass
//     in a string of MAX_VIRUS_INFO size.  A smaller path may be used, but there
//     is a risk of the buffer being too short and requiring a subsequent
//     call to actually get the information requested.
//
// Returns:
//     BOOL info :
//          TRUE  - Item queried is TRUE.
//          FALSE - Item queried is FALSE.
//
//     String Info:
//          NAV_OK    - Information retrieved successfully.
//          NAV_ERROR - Failed to retrieve requested information.
//
//          NAV_INSUFFICIENT_BUFFER - Buffer was too short.  Actual buffer size
//                                    needed is returned in lpdwBufferSize.
//
//************************************************************************
// 05/28/98 Created by KSACKIN
//************************************************************************
DWORD NAVCALLAPI NAVGetVirusDefInfo
(
    HNAVVIRUSDEF    hNAVVirusDef,
    NAVVIRINFO      enumVirInfo,
    LPTSTR          lpszResultBuffer,
    LPDWORD         lpdwBufferSize
)
{
    DWORD   dwReturn = NAV_OK;

    // Make sure we have a valid virus handle.
    if ( NULL == hNAVVirusDef )
    {
        // Return failure.
        return NAV_ERROR;
    }

    // If we are only querying for a BOOL, perform this query now.
    if ( IsNAVEnumItemBOOL( enumVirInfo )  )
    {
        // Return TRUE or FALSE depending on the query.
        return VLDefInfo( (HVLVIRUSENTRY)hNAVVirusDef,
                          MapNAVVirInfoToVLVirInfo(enumVirInfo),
                          lpszResultBuffer );
    }

    // This is for efficiency.  If our buffer is long enough, pass straight
    // through to API, otherwise, we need to double buffer to ensure correct
    // size.
    if ( *lpdwBufferSize >= MAX_VIRUS_INFO )
    {
        // Call through to NAV's function to get the definition information.
        if ( VLDefInfo( (HVLVIRUSENTRY)hNAVVirusDef,
                        MapNAVVirInfoToVLVirInfo(enumVirInfo),
                        lpszResultBuffer ) )
        {
            // Set our return buffer size.
            *lpdwBufferSize = STRLEN( lpszResultBuffer );
        }
        else
        {
            dwReturn = NAV_ERROR;
        }
    }
    else
    {
        // In this scenario, lpszResultBuffer could be too short.  For this
        // reason, we need to allocate a temporary buffer, and make sure our
        // buffer is going to fit all of the information.

        TCHAR       szTempBuffer[MAX_VIRUS_INFO];

        // Call through to NAV's function to get the definition information.
        if ( VLDefInfo( (HVLVIRUSENTRY)hNAVVirusDef,
                        MapNAVVirInfoToVLVirInfo(enumVirInfo),
                        szTempBuffer ) )
        {


            // Make sure that our return string is long enough.
            if ( STRLEN( szTempBuffer ) <= *lpdwBufferSize )
            {

                // Copy in our return string.
                STRCPY( lpszResultBuffer, szTempBuffer );

                // Set our return buffer size.
                *lpdwBufferSize = STRLEN( lpszResultBuffer );
            }
            else
            {
                // Return failure.
                dwReturn = NAV_INSUFFICIENT_BUFFER;

                // Set our required buffer size (including NULL).
                *lpdwBufferSize = STRLEN( lpszResultBuffer ) + 1;
            }

        }
        else
        {
            // Return failure.
            dwReturn = NAV_ERROR;
        }
    }

    // Return our success.
    return dwReturn;
}


//************************************************************************
//
// HNAVVIRUSDEFTABLE NAVCALLAPI NAVLoadVirusDefTable
// (
//     HNAVENGINE  hNAVEngine,   - [in] Handle to an open NAV Engine.
// )
//
// Purpose:
//     This routine will load the entire virus list into a table that can
//     then be querried quickly to retrieve information such as an entire
//     list of virus names.  This method is much quicker than loading every
//     definition individually.
//
// Returns:
//     HNAVVIRUSDEFTABLE - Loaded the virus definition table successfully.
//     NULL              - Failed to load the virus definition table.
//
//************************************************************************
// 05/28/98 Created by KSACKIN
//************************************************************************
HNAVVIRUSDEFTABLE NAVCALLAPI NAVLoadVirusDefTable
(
    HNAVENGINE  hNAVEngine
)
{
#if defined(SYM_DOSX)
    // The DefTable routines actually work for the DX platforms when
    // used in plain DOS, but cause a crash when used in a Windows
    // DOS box.  The crash is in the DX memory allocator, and is
    // probably due to the very large number of selectors being
    // used.  To avoid these problems, we just fail this function.
    return NULL;
#else
    LPVIRUSDEFTABLE lpDefTable = NULL;
    DWORD           dwVirusDefCount;
    BOOL            bSuccess = FALSE;

    SYM_ASSERT( hNAVEngine );

    // Catch NULL engine handle case.
    if ( NULL == hNAVEngine )
        {
        return NULL;
        }

    // Get the number of viruses available in the definition set.
    if ( NAV_OK == NAVGetVirusDefCount( hNAVEngine, &dwVirusDefCount ) )
    {
        // Allocate the virus definition table.
        if ( lpDefTable = CreateVirusDefTable( dwVirusDefCount ) )
        {

            // Fill the virus definition table.
            if ( VLFillListTable ( (HVCONTEXT) hNAVEngine,
                                   (HVLVIRUSENTRYTABLE) lpDefTable->lpNAVVirusDefList ) )
            {
                // All was successfull.  Set our return value to TRUE.
                bSuccess = TRUE;
            }
        }
    }


    // If we were unsuccessfull, free resources.
    if ( FALSE == bSuccess )
    {
        // Release the memory we allocated for this list.
        NAVReleaseVirusDefTable( (HNAVVIRUSDEFTABLE) lpDefTable );

        // NULL out handle, and return.
        lpDefTable = NULL;
    }

    return (HNAVVIRUSDEFTABLE) lpDefTable;
#endif
}


//************************************************************************
//
// VOID NAVCALLAPI NAVReleaseVirusDefTable
// (
//     HNAVVIRUSDEFTABLE hNAVVirusDefTable - [in] Allocated NAV Virus
//                                                Definition table.
// )
//
// Purpose:
//     This function will release a virus definition table allocated with the
//     NAVLoadVirusDefTable function.
//
// Returns:
//     None.
//
//************************************************************************
// 05/28/98 Created by KSACKIN
//************************************************************************
VOID NAVCALLAPI NAVReleaseVirusDefTable
(
    HNAVVIRUSDEFTABLE hNAVVirusDefTable
)
{
    LPVIRUSDEFTABLE lpDefTable = (LPVIRUSDEFTABLE)hNAVVirusDefTable;
    DWORD           dwLoop;

    // Return if the table is NULL.
    if ( NULL == lpDefTable )
        return;

    // Loop through all items read into this list, and release all virus
    // entries allocated by the NAVLoadVirusDefTable() function.
    for ( dwLoop=0; dwLoop < lpDefTable->dwVirusDefCount; dwLoop++ )
    {
        // Free the memory for this definition.
        if ( NULL != lpDefTable->lpNAVVirusDefList[dwLoop] )
            NAVReleaseVirusDef( lpDefTable->lpNAVVirusDefList[dwLoop] );
    }

    // Free the memory associated with the virus definition table.
    MemFreePtr(lpDefTable->lpNAVVirusDefList);

    // Free the actual structure itself.
    MemFreePtr(lpDefTable);
}


//************************************************************************
//
// NAVSTATUS NAVCALLAPI NAVGetVirusDefTableCount
// (
//     HNAVVIRUSDEFTABLE    hNAVVirusDefTable - [in]  Open virus definition table.
//     DWORD*               lpdwVirusDefCount - [out] Number of virus definitions in this table.
// )
//
// Purpose:
//     This function will return the number of virus definitions contained
//     in this
//     list.
//
// Returns:
//     DWORD - Count of virus definitions in this table.
//
//************************************************************************
// 05/28/98 Created by KSACKIN
//************************************************************************
NAVSTATUS NAVCALLAPI NAVGetVirusDefTableCount
(
    HNAVVIRUSDEFTABLE   hNAVVirusDefTable,
    DWORD*              lpdwVirusDefCount
)
{
    LPVIRUSDEFTABLE lpDefTable = (LPVIRUSDEFTABLE) hNAVVirusDefTable;

    // Make sure we got a good pointer.
    if ( NULL != lpDefTable )
        {
        // Fill return DWORD with table count.
        *lpdwVirusDefCount = lpDefTable->dwVirusDefCount;

        // Return success.
        return NAV_OK;
        }

    // Got a NULL pointer, return error.
    return NAV_ERROR;
}


//************************************************************************
//
// DWORD NAVCALLAPI NAVGetVirusDefTableInfo
// (
//     HNAVVIRUSDEFTABLE    hNAVVirusDefTable - [in]  Open virus definition table.
//     DWORD                dwIndex,          - [in]  Index of item to get.
//     NAVVIRINFO           enumVirInfo,      - [in]  Item to retrieve information on.
//     LPTSTR               lpszResultBuffer  - [out] Result of query.
//     LPDWORD              lpdwBufferSize    - [in/out]
//                                              in  - Size of return buffer.
//                                              out - Number of characters returned,
//                                                    or buffer size needed.
// )
//
// Purpose:
//     This routine will get information on a particular virus definition
//     from a virus definition table.  Use the ID's associated with the
//     various information to query for items such as virus name, alias,
//     and infection type.
//
// Returns:
//     BOOL info :
//          TRUE  - Item queried is TRUE.
//          FALSE - Item queried is FALSE.
//
//     String Info:
//          NAV_OK    - Information retrieved successfully.
//          NAV_ERROR - Failed to retrieve requested information.
//
//          NAV_INSUFFICIENT_BUFFER - Buffer was too short.  Actual buffer size
//                                    needed is returned in lpdwBufferSize.
//
// See Also:
//     NAVGetVirusDefInfo - This API is the single def getting API which
//                          returns the actual return value.
//
//************************************************************************
// 05/28/98 Created by KSACKIN
//************************************************************************
DWORD NAVCALLAPI NAVGetVirusDefTableInfo
(
    HNAVVIRUSDEFTABLE   hNAVVirusDefTable,
    DWORD               dwIndex,
    NAVVIRINFO          enumVirInfo,
    LPTSTR              lpszResultBuffer,
    LPDWORD             lpdwBufferSize
)
{
    LPVIRUSDEFTABLE lpDefTable = (LPVIRUSDEFTABLE) hNAVVirusDefTable;

    // Catch the NULL pointer case.
    if ( NULL == lpDefTable )
        {
        return NAV_ERROR;
        }

    // Check for dwIndex out of range
    if (dwIndex >= lpDefTable->dwVirusDefCount)
        {
        return NAV_ERROR;
        }

    // Call through to our single definition API to get def information.
    return NAVGetVirusDefInfo( lpDefTable->lpNAVVirusDefList[dwIndex],
                               enumVirInfo,
                               lpszResultBuffer,
                               lpdwBufferSize );
}



/////////////////////////////////////////////////////////////////////
// Local functions.


//************************************************************************
//
// LPVIRUSDEFTABLE CreateVirusDefTable
// (
//     DWORD dwVirusDefCount - [in] Size to initialize virus definition table to.
// )
//
// Purpose:
//     This function will allocate and initialize a VIRUSDEFTABLE struct
//     to the number of entries you are to read from the database.  It will
//     set the member variable to the size, and allocate the internal virus
//     definition list.
//
// Returns:
//     LPVIRUSDEFTABLE - Open handle to an allocated virus definition table.
//     NULL            - Failed to allocate and initialize table.
//
//************************************************************************
// 06/07/98 Created by KSACKIN
//************************************************************************
LPVIRUSDEFTABLE CreateVirusDefTable
(
    DWORD dwVirusDefCount
)
{
    LPVIRUSDEFTABLE lpDefTable;

    // Allocate a virus definition table structure.
    lpDefTable = (LPVIRUSDEFTABLE) MemAllocPtr(GHND, sizeof(VIRUSDEFTABLE));

    // Check the allocation.
    if ( NULL == lpDefTable )
        return NULL;

    // Make sure our memory is all zero'ed out.
    MEMSET( lpDefTable, 0, sizeof(VIRUSDEFTABLE) );

    // Set our number of virus entries.
    lpDefTable->dwVirusDefCount = dwVirusDefCount;


    // Allocate the new virus definition list based on the number of viruses available.
    if ( lpDefTable->lpNAVVirusDefList = (HNAVVIRUSDEF*) MemAllocPtr(GHND, sizeof(HNAVVIRUSDEF) * dwVirusDefCount) )
    {
        // Make sure our memory is all zero'ed out.
        MEMSET( lpDefTable->lpNAVVirusDefList, 0, (UINT)(sizeof(HNAVVIRUSDEF) * (lpDefTable->dwVirusDefCount)) );
    }
    else
    {
        // Failure to allocate the virus list.  Free memory and return.
        MemFreePtr(lpDefTable);
        lpDefTable = NULL;
    }


    return lpDefTable;
}


//************************************************************************
//
// BOOL IsNAVEnumItemBOOL
// (
//     NAVVIRINFO enumItem - [in] Enum item to check.
// )
//
// Purpose:
//     This function will check to see if the item that we are trying to
//     get information on is a BOOL or if it is something different.
//
// Returns:
//     TRUE  - Item is of BOOLEAN type.
//     FALSE - Item non numerical.  Probably string.
//
//************************************************************************
// 06/07/98 Created by KSACKIN
//************************************************************************
BOOL IsNAVEnumItemBOOL
(
    NAVVIRINFO enumItem
)
{
    // Check to see if this item is outside the range of boolean values.
    if ( ((DWORD)enumItem < NAV_VI_BOOL_INFECT_FILES) ||
         ((DWORD)enumItem > NAV_VI_BOOL_PART_ITEM) )
    {
        // Item is a string value.
        return FALSE;
    }

    // Item is BOOL.  Return TRUE.
    return TRUE;
}

//************************************************************************
//
// VLVIRINFO MapNAVVirInfoToVLVirInfo
// (
//     NAVVIRINFO enumNAVVirInfo - [in] NAV Enum item to convert to
//                                      VLVirInfo enum item.
// )
//
// Purpose:
//     This function will convert a NAVVIRINFO enumeration item into the
//     actual VLVIRINFO item needed to perform engine queries.  We should
//     call this function any time that we pass a NAVVIRINFO into the
//     get information on is a BOOL or if it is something different.
//
// Returns:
//     TRUE  - Item is of BOOLEAN type.
//     FALSE - Item non numerical.  Probably string.
//
//************************************************************************
// 06/07/98 Created by KSACKIN
//************************************************************************
VLVIRINFO MapNAVVirInfoToVLVirInfo
(
    NAVVIRINFO enumNAVVirInfo
)
{
    VLVIRINFO  vlvInfo;

    switch ( enumNAVVirInfo )
    {
        case NAV_VI_VIRUS_NAME:              vlvInfo = VI_VIRUS_NAME;              break;
        case NAV_VI_VIRUS_ALIAS:             vlvInfo = VI_VIRUS_ALIAS;             break;
        case NAV_VI_VIRUS_INFO:              vlvInfo = VI_VIRUS_INFO;              break;
        case NAV_VI_VIRUS_SIZE:              vlvInfo = VI_VIRUS_SIZE;              break;
        case NAV_VI_VIRUS_ID:                vlvInfo = VI_VIRUS_ID;                break;

        case NAV_VI_BOOL_INFECT_FILES:       vlvInfo = VI_BOOL_INFECT_FILES;       break;
        case NAV_VI_BOOL_INFECT_EXE:         vlvInfo = VI_BOOL_INFECT_EXE;         break;
        case NAV_VI_BOOL_INFECT_COM:         vlvInfo = VI_BOOL_INFECT_COM;         break;
        case NAV_VI_BOOL_INFECT_COMCOM:      vlvInfo = VI_BOOL_INFECT_COMCOM;      break;
        case NAV_VI_BOOL_INFECT_SYS:         vlvInfo = VI_BOOL_INFECT_SYS;         break;
        case NAV_VI_BOOL_INFECT_MBR:         vlvInfo = VI_BOOL_INFECT_MBR;         break;
        case NAV_VI_BOOL_INFECT_FLOPPY_BOOT: vlvInfo = VI_BOOL_INFECT_FLOPPY_BOOT; break;
        case NAV_VI_BOOL_INFECT_HD_BOOT:     vlvInfo = VI_BOOL_INFECT_HD_BOOT;     break;

        case NAV_VI_BOOL_INFECT_WILD:        vlvInfo = VI_BOOL_INFECT_WILD;        break;

        case NAV_VI_BOOL_MEMORY_RES:         vlvInfo = VI_BOOL_MEMORY_RES;         break;
        case NAV_VI_BOOL_SIZE_STEALTH:       vlvInfo = VI_BOOL_SIZE_STEALTH;       break;
        case NAV_VI_BOOL_FULL_STEALTH:       vlvInfo = VI_BOOL_FULL_STEALTH;       break;
        case NAV_VI_BOOL_TRIGGER:            vlvInfo = VI_BOOL_TRIGGER;            break;
        case NAV_VI_BOOL_ENCRYPTING:         vlvInfo = VI_BOOL_ENCRYPTING;         break;
        case NAV_VI_BOOL_POLYMORPHIC:        vlvInfo = VI_BOOL_POLYMORPHIC;        break;
        case NAV_VI_BOOL_MULTIPART:          vlvInfo = VI_BOOL_MULTIPART;          break;
        case NAV_VI_BOOL_MAC_VIRUS:          vlvInfo = VI_BOOL_MAC_VIRUS;          break;
        case NAV_VI_BOOL_MACRO_VIRUS:        vlvInfo = VI_BOOL_MACRO_VIRUS;        break;
        case NAV_VI_BOOL_WINDOWS_VIRUS:      vlvInfo = VI_BOOL_WINDOWS_VIRUS;      break;
        case NAV_VI_BOOL_AGENT_VIRUS:        vlvInfo = VI_BOOL_AGENT_VIRUS;        break;
        case NAV_VI_BOOL_REPAIRED:           vlvInfo = VI_BOOL_REPAIRED;           break;
        case NAV_VI_BOOL_REPAIR_FAILED:      vlvInfo = VI_BOOL_REPAIR_FAILED;      break;
        case NAV_VI_BOOL_REPAIRABLE:         vlvInfo = VI_BOOL_REPAIRABLE;         break;
        case NAV_VI_BOOL_DELETABLE:          vlvInfo = VI_BOOL_DELETABLE;          break;
        case NAV_VI_BOOL_NO_ACCESS:          vlvInfo = VI_BOOL_NO_ACCESS;          break;
        case NAV_VI_BOOL_DISEMBODIED:        vlvInfo = VI_BOOL_DISEMBODIED;        break;
        case NAV_VI_BOOL_DELETED:            vlvInfo = VI_BOOL_DELETED;            break;
        case NAV_VI_BOOL_DELETE_FAILED:      vlvInfo = VI_BOOL_DELETE_FAILED;      break;
        case NAV_VI_BOOL_DEF_DELETED:        vlvInfo = VI_BOOL_DEF_DELETED;        break;
        case NAV_VI_BOOL_INFO_VALID:         vlvInfo = VI_BOOL_INFO_VALID;         break;
        case NAV_VI_BOOL_MEMORY_DETECT:      vlvInfo = VI_BOOL_MEMORY_DETECT;      break;
        case NAV_VI_BOOL_FILE_ITEM:          vlvInfo = VI_BOOL_FILE_ITEM;          break;
        case NAV_VI_BOOL_BOOT_ITEM:          vlvInfo = VI_BOOL_BOOT_ITEM;          break;
        case NAV_VI_BOOL_PART_ITEM:          vlvInfo = VI_BOOL_PART_ITEM;          break;

        case NAV_VI_ITEM_NAME:               vlvInfo = VI_ITEM_NAME;               break;
        case NAV_VI_DETECT_DATE:             vlvInfo = VI_DETECT_DATE;             break;
        case NAV_VI_DETECT_TIME:             vlvInfo = VI_DETECT_TIME;             break;
        case NAV_VI_PHYSICAL_DRIVE_NUM:      vlvInfo = VI_PHYSICAL_DRIVE_NUM;      break;
        case NAV_VI_PARTITION_NUM:           vlvInfo = VI_PARTITION_NUM;           break;
        case NAV_VI_PARTITION_TYPE:          vlvInfo = VI_PARTITION_TYPE;          break;
    }

    return vlvInfo;
}

//************************************************************************
//
// NAVSTATUS NAVGetNavapiVersionString
// (
//     LPTSTR lpstVersionBuffer,  - [out] buffer to copy string into
//     UINT   uBufferSize         - [in]  size of lpstVersionBuffer
// )
//
// Purpose:
//     This function will copy the version string into the supplied lpstVersionBuffer.
//     If uBufferSize is not large enough to hold the supplied version string,
//     lpstVersionBuffer will point to an empty string and this function will return
//     NAV_ERROR.  The Version String will look soming like "1.0.0.0".
//
// Returns:
//     NAV_OK on success, NAV_ERROR otherwise
//************************************************************************
// 07/15/98 Created by MKEATIN
//************************************************************************

NAVSTATUS NAVCALLAPI NAVGetNavapiVersionString
(
    LPTSTR lpstVersionBuffer,  // [out] buffer to copy string into
    UINT   uBufferSize         // [in]  size of lpstVersionBuffer
)
{
    //  make sure uBufferSize is large enough

    if (STRLEN(NAVAPI_PRODVERSION_STRING) > (uBufferSize - 1))
    {
        lpstVersionBuffer[0] = EOS;
        return NAV_ERROR;
    }

    STRCPY(lpstVersionBuffer, NAVAPI_PRODVERSION_STRING);

    return NAV_OK;
}


//************************************************************************
//
// NAVSTATUS NAVCALLAPI NAVGetVirusDBInfo
// (
//     HNAVENGINE      hNAVEngine,      - [in]  Initialized NAV Engine handle.
//     LPVIRUSDBINFO   lpVirusDBInfo    - [out] Information about the virus
//                                              database.
// )
//
// Purpose:
//     This routine gets information about the virus database.  Information
//     available includes the date that the virus definition set was created,
//     and the version number of the virus definitions for that date.
//
//
// Returns:
//     NAV_OK    - Information retrieved successfully.
//     NAV_ERROR - Failed to retrieve requested information.
//
//************************************************************************
// 07/23/98 Created by KSACKIN
//************************************************************************
NAVSTATUS NAVCALLAPI NAVGetVirusDBInfo
(
    HNAVENGINE      hNAVEngine,
    LPVIRUSDBINFO   lpVirusDBInfo
)
{
    NAVSTATUS   nsReturn = NAV_ERROR;
    char       szTempString[10];

    // Make sure we have a valid virus handle.
    if ((NULL == hNAVEngine) ||
        (NULL == lpVirusDBInfo))
    {
        // Return failure.
        nsReturn = NAV_ERROR;
        goto Cleanup;
    }

    // Year.
    if ( VS_OK != VLGetDBInfo( (HVCONTEXT) hNAVEngine,
                               DB_YEAR_STAMP,
                               szTempString
                               ))
    {
        goto Cleanup;
    }
    lpVirusDBInfo->wYear = (WORD)atol( szTempString );

    // Month.
    if ( VS_OK != VLGetDBInfo( (HVCONTEXT) hNAVEngine,
                               DB_MONTH_STAMP,
                               szTempString
                               ))
    {
        goto Cleanup;
    }
    lpVirusDBInfo->wMonth = (WORD)atol( szTempString );

    // Day.
    if ( VS_OK != VLGetDBInfo( (HVCONTEXT) hNAVEngine,
                               DB_DAY_STAMP,
                               szTempString
                               ))
    {
        goto Cleanup;
    }
    lpVirusDBInfo->wDay = (WORD)atol( szTempString );

    // Version.
    if ( VS_OK != VLGetDBInfo( (HVCONTEXT) hNAVEngine,
                               DB_VERSION_STAMP,
                               szTempString
                               ))
    {
        goto Cleanup;
    }
    lpVirusDBInfo->dwVersion = (DWORD)atol( szTempString );

    // All went well, return NAV_OK.
    nsReturn = NAV_OK;

Cleanup:

    // Return our success.
    return nsReturn;
}


