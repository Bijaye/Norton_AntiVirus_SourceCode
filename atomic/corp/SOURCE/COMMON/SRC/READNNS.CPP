// Copyright 1993-1994 Symantec Corporation
//***********************************************************************
//                                                                
// $Header:   S:/common/VCS/readnns.cpv   1.0   06 Feb 1997 20:50:06   RFULLER  $
//                                                                
// Description:                                                   
//      Reads the NNS file and puts it into data structures.
//                                                                
// Contains (as exports):                                                      
//                                                                
// See Also:                                                      
//***********************************************************************
// $Log:   S:/common/VCS/readnns.cpv  $
// 
//    Rev 1.0   06 Feb 1997 20:50:06   RFULLER
// Initial revision
// 
//    Rev 1.1   09 Oct 1996 16:21:26   JBRENNA
// Change STRUPR() calls to NavStrUpr() calls. The NavStrUpr() function
// is intelligent enough not to upper case DBCS characters.
// 
//    Rev 1.0   01 Mar 1996 18:26:44   BARRY
// Initial revision.
// 
//    Rev 1.0   30 Jan 1996 09:55:30   BARRY
// Initial revision.
// 
//    Rev 1.4   17 Oct 1995 14:10:36   JWORDEN
// Convert to the new SectionSuffix structure and the new
//     gwPlatform bit mask
// 
//    Rev 1.3   19 Sep 1995 08:26:46   JWORDEN
// Add GetPlatformsFromNNS function
// 
//    Rev 1.1   13 Sep 1995 10:50:12   JWORDEN
// Alter NNS read logic to load Luigi exclusion data
// Alter NNS read logic to pass SectionSuffix info
// 
//    Rev 1.0   20 Apr 1994 15:16:36   DALLEE
// Initial revision.
//
// From NIVANA readnns.c 1.9
//***********************************************************************

#ifdef NAVNETW

#include "platform.h"                   // Platform independence
#include "xapi.h"
#include <ctype.h>                      // isdigit()
#include <stdlib.h>

#include "navutil.h"
#include "common.h"
#include "options.h"

#include "symcfg.h"

//************************************************************************
// GLOBAL VARIABLES
//************************************************************************
char SZ_NUM_NETUSERS[]          = "NumberOfNetUsers";
char SZ_NETUSER[]               = "NetUser";
char SZ_SEPS[]                  = ",\t";
char SZ_ALL[]                   = "ALL";
#ifdef BANKERS
  char SZ_NUM_REGISTRATIONS[]   = "NumberOfRegistrations";
  char SZ_REGISTRATIONS[]       = "RegisteredFiles";
#endif  // BANKERS

//************************************************************************
// External Global Variables
//************************************************************************

extern WORD gwNAVPresent,
            gwPlatform[PLATFORM_COUNT];

#define LOCALE /**/
#include "nns.h"
#undef LOCALE


//************************************************************************
// LOCAL PROTOTYPES
//************************************************************************

WORD LOCAL PASCAL ReadExclusionsFromNNS
(
    LPSECTIONSUFFIX lpSectionSuffix
);

WORD LOCAL PASCAL ReadNetUsersFromNNS
(
    LPSECTIONSUFFIX lpSectionSuffix
);

#ifdef BANKERS
    WORD LOCAL PASCAL ReadRegistrationsFromNNS
    (
        LPSECTIONSUFFIX lpSectionSuffix
    );
#endif  // BANKERS


//************************************************************************
// GetPlatformsFromNNS()
//
// This routine get the platform information from the NNS file.
//
// Parameters:
//  None
//
// Returns:
//  INI_OK        - All entries found as requested
//  INI_DEFAULTED - At least one entry was not found and a default value
//                  was returned
//************************************************************************
// 9/18/95 JWORDEN Function created
//************************************************************************

WORD WINAPI GetPlatformsFromNNS ()
{
    WORD wReturn = INI_OK,
         wGetReturn;

    extern      CFGFILE         cfgFile;

    BYTE byNAVPresent[PLATFORM_COUNT];

    UINT uIndex;

    SECTIONSUFFIX PlatformSuffix;

                                        //-------------------------------
                                        // Setup the special suffixes and
                                        // settings
                                        //-------------------------------
    MEMSET (&PlatformSuffix, 0, sizeof (PlatformSuffix));

    PlatformSuffix.wPlatform[0] = PLATFORM_ALL;

    for (uIndex = 0; uIndex < PLATFORM_COUNT; uIndex++)
    {
        PlatformSettings[uIndex].lpVar = &(byNAVPresent[uIndex]);
        PlatformSuffix.Suffix[uIndex].wPlatform = PLATFORM_ALL;
        PlatformSuffix.Suffix[uIndex].lpszSectionSuffix = "";
    }

                                        //-------------------------------
                                        // Get all Platform settings.
                                        //-------------------------------
    wGetReturn = GetByteIni (&cfgFile, PlatformSettings, &PlatformSuffix);
    if (wGetReturn != INI_OK)
        wReturn = INI_DEFAULTED;

                                        //-------------------------------
                                        // Create the gwNAVPresent mask
                                        //-------------------------------
    gwNAVPresent = PLATFORM_NONE;
    for (uIndex = 0; uIndex < PLATFORM_COUNT; uIndex++)
    {
        if (byNAVPresent[uIndex])
            gwNAVPresent |= gwPlatform[uIndex];
    }

    return (wReturn);
}



//************************************************************************
// ReadNNSInMemory()
//
// This routine updates the NNS file in memory.
//
// Parameters:
//  Section suffix structure
//
// Returns:
//  INI_OK        - All entries found as requested
//  INI_DEFAULTED - At least one entry was not found and a default value
//                  was returned
//************************************************************************
// 2/1/93 DALLEE Function created.
//************************************************************************

WORD WINAPI ReadNNSInMemory(LPSECTIONSUFFIX lpSectionSuffix)
{
    WORD wReturn = INI_OK,
         wGetReturn;

    extern      CFGFILE         cfgFile;
                                        //-------------------------------
                                        // Read all NAV.BIN settings.
                                        //-------------------------------
    wGetReturn = GetByteIni (&cfgFile, byteSettings, lpSectionSuffix);
    if (wGetReturn != INI_OK)
        wReturn = INI_DEFAULTED;

    wGetReturn = GetWordIni (&cfgFile, wordSettings, lpSectionSuffix);
    if (wGetReturn != INI_OK)
        wReturn = INI_DEFAULTED;

    wGetReturn = GetDWordIni (&cfgFile, dwordSettings, lpSectionSuffix);
    if (wGetReturn != INI_OK)
        wReturn = INI_DEFAULTED;

    wGetReturn = GetStringIni (&cfgFile, stringSettings, lpSectionSuffix);
    if (wGetReturn != INI_OK)
        wReturn = INI_DEFAULTED;

                                        //-------------------------------
                                        // Read Exclusions settings.
                                        //-------------------------------
    wGetReturn = ReadExclusionsFromNNS (lpSectionSuffix);
    if (wGetReturn != INI_OK)
        wReturn = INI_DEFAULTED;

                                        //-------------------------------
                                        // Read NetUsers settings.
                                        //-------------------------------
    wGetReturn = ReadNetUsersFromNNS (lpSectionSuffix);
    if (wGetReturn != INI_OK)
        wReturn = INI_DEFAULTED;

#ifdef BANKERS
    wGetReturn = ReadRegistrationsFromNNS (lpSectionSuffix);// Read list of registered files.
    if (wGetReturn != INI_OK)
        wReturn = INI_DEFAULTED;

#endif  // BANKERS

    return (wReturn);
}


//************************************************************************
// ReadExclusionsFromNNS()
//
// This routine reads the exlusions settings from the NNS file.
//
// Parameters:
//  Section suffix structure
//
// Returns:
//  INI_OK        - All entries found as requested
//  INI_DEFAULTED - The specific suffix was not found but a default was
//************************************************************************
// 2/1/93 DALLEE Function created.
//************************************************************************
WORD LOCAL PASCAL ReadExclusionsFromNNS
(
    LPSECTIONSUFFIX lpSectionSuffix
)
{
    extern      CFGFILE         cfgFile;

    extern      NAVOPTIONS      navOptions;

    auto        CFGTEXTREC      cfgTextRec;
    auto        LPSTR           lpszToken;
    auto        BOOL            bValid;
    auto        WORD            wMask,
                                wFlags;

    WORD         wReturn = INI_OK;

    UINT         i,
                 uPlatformIndex,
                 uSuffixIndex,
                 uErr,
                 uNumExclusions;

    char         szSection[80],
                 szBuffer[MAX_INI_LINE + 1],
                 szEntry[MAX_INI_LINE + 1];

    EXCLUDEITEM  ExcludeItem;

    BOOL         bFoundEntry;

    STRCPY (szEntry, SZ_NUM_EXCLUSIONS);
                                        // Get # of exclusions from NNS

    MEMSET(&cfgTextRec, 0, sizeof(cfgTextRec));
    cfgTextRec.lpSection = szSection;
    cfgTextRec.lpEntry   = szEntry;

    bFoundEntry = FALSE;

    for (uPlatformIndex = 0;
         uPlatformIndex < MAX_SUFFIXES && lpSectionSuffix->wPlatform[uPlatformIndex] != PLATFORM_NONE;
         uPlatformIndex++)
       {
       for (uSuffixIndex = 0; uSuffixIndex < MAX_SUFFIXES; uSuffixIndex++)
           {
           // ************************************************** //
           // Is this suffix valid for this platform?                        //
           // ************************************************** //

           if (!(lpSectionSuffix->Suffix[uSuffixIndex].wPlatform & lpSectionSuffix->wPlatform[uPlatformIndex]))
               continue;

           // ************************************************** //
           // Load the section name and add the primary modifier //
           // ************************************************** //

           STRCPY (szSection, SZ_EXCLUSIONS);
           STRCAT (szSection, lpSectionSuffix->Suffix[uSuffixIndex].lpszSectionSuffix);

           // ************************************************** //
           // Read the value                                     //
           // ************************************************** //

           #ifdef SYM_WIN
               uErr = ReadConfigValue (&cfgFile, &cfgTextRec, &uNumExclusions);
           #else
               uErr = ConfigFileReadValue (cfgFile,
                                           &cfgTextRec,
                                           &uNumExclusions,
                                           NULL);
           #endif

           // ************************************************** //
           // If the read was successful, we need to find a way  //
           // to get out of this mess                                                        //
           // ************************************************** //

           if (NOERR == uErr)
               {
               bFoundEntry = TRUE;
               break;
               }
           }

       // *************************************************** //
       // Did we find anything?                                                           //
       // *************************************************** //

       if (bFoundEntry)
           break;
       }

    // *************************************************** //
    // Did we get out of the loops because we found        //
    // something or were we just tired of looking?         //
    // *************************************************** //

    if (!bFoundEntry)
        {
            uNumExclusions = 0;
            wReturn = INI_OK;
        }

    if ( uNumExclusions > MAX_EXCLUDES )
        uNumExclusions = MAX_EXCLUDES;

                                        // Initialize the exclusion storage
    navOptions.exclude.wEntries = 0;

                                        // Allocate memory for private
    navOptions.exclude.lpPrivate =
               (LPEXCLUDEPRIVATE)MemAllocPtr (GHND, sizeof (EXCLUDEPRIVATE));
    if (navOptions.exclude.lpPrivate == NULL)
        return (INI_FAILED);

                                        // Allocate enough memory for 1
                                        // exclusion
    navOptions.exclude.hExcludeMem = MemAlloc (GHND, sizeof (EXCLUDEITEM));

    if (navOptions.exclude.hExcludeMem == NULL)
    {
        MemFreePtr ( navOptions.exclude.lpPrivate );
        return (INI_FAILED);
    }

    navOptions.exclude.lpPrivate->lpExcludeMem =
             (LPEXCLUDEITEM) MemLock (navOptions.exclude.hExcludeMem);
    if (navOptions.exclude.lpPrivate->lpExcludeMem == NULL)
    {
        MemFreePtr ( navOptions.exclude.lpPrivate );
        MemFree    ( navOptions.exclude.hExcludeMem );
        return (INI_FAILED);
    }
                                        // Initialize the LFN exclude
                                        // structures

    navOptions.exclude.lpPrivate->hLExclude =
                 (LPEXCLUDELSTRUCT)MemAllocPtr (GHND, sizeof(EXCLUDELSTRUCT));

    if (navOptions.exclude.lpPrivate->hLExclude == NULL)
    {
        MemFreePtr ( navOptions.exclude.lpPrivate );
        MemRelease ( navOptions.exclude.hExcludeMem );
        MemFree    ( navOptions.exclude.hExcludeMem );
    }

    navOptions.exclude.lpPrivate->hLExclude->wNumEntries     =   0;
    navOptions.exclude.lpPrivate->hLExclude->lpEntries       =   NULL;
    navOptions.exclude.lpPrivate->hLExclude->bIsDirty        =   FALSE;
    navOptions.exclude.lpPrivate->hLExclude->dwEntriesSize   =  ENTRIES_PER_BLOCK
                                                              * sizeof(LEXCLUDE);
    *navOptions.exclude.lpPrivate->hLExclude->szFilePath    =   EOS;

    navOptions.exclude.lpPrivate->hLExclude->lpEntries =
            (LPLEXCLUDE)MemAllocPtr (GHND,
                       navOptions.exclude.lpPrivate->hLExclude->dwEntriesSize);

                                        // Read in each exclude entry

    for (i = 0; i < uNumExclusions; i++)
    {
                                        // Create unique entry
        STRCPY (szEntry, SZ_EXCLUSIONS);
        _ConvertWordToString(i + 1, szEntry + STRLEN(szEntry), 10, 0);

        MEMSET (&ExcludeItem, 0, sizeof (ExcludeItem));

        bValid = FALSE;

        #ifdef SYM_WIN

            uErr = ReadConfigString (&cfgFile,
                                     &cfgTextRec,
                                     szBuffer,
                                     sizeof (szBuffer));

        #else

            uErr = ConfigFileReadString (&cfgFile,
                                         &cfgTextRec,
                                         szBuffer,
                                         sizeof (szBuffer));
        #endif

        if (uErr == ERR)
            break;
                                    //------------------------------
                                    // Parse each entry
                                    //------------------------------
        lpszToken = _fstrtok(szBuffer, SZ_SEPS);
        if ( lpszToken )
            {
                                    // Get exclusion item
            STRNCPY (ExcludeItem.szText, lpszToken, sizeof (ExcludeItem.szText));
            ExcludeItem.szText[sizeof (ExcludeItem.szText) - 1] = EOS;
            NavStrUpr (ExcludeItem.szText);

            lpszToken = _fstrtok (NULL, SZ_SEPS);
            if ( lpszToken )
                {                   // Subdirectory?
                ExcludeItem.bSubDirectory = (*lpszToken == '1') ? TRUE : FALSE;

                lpszToken = _fstrtok(NULL, SZ_SEPS);
                if ( lpszToken )
                    {
                                    // Process bit string
                    for (wFlags = 0, wMask = 0x0001; isdigit(*lpszToken);
                                                 wMask <<= 1, lpszToken++)
                        {
                        if ( *lpszToken == '1' )
                            wFlags |= wMask;
                        }

                    ExcludeItem.wBits = wFlags;
                    bValid = TRUE;
                    }
                }
            }

        if ( !bValid )
            break;

                                    // Add the item to the appropriate
                                    // structure
        ExcludeAdd (&navOptions.exclude, &ExcludeItem);
    }

    return (wReturn);
}


//************************************************************************
// ReadNetUsersFromNNS()
//
// This routine reads the net users settings from the NNS file.
//
// Parameters:
//  Section suffix structure
//
// Returns:
//  INI_OK        - All entries found as requested
//  INI_DEFAULTED - The specific suffix was not found but a default was
//************************************************************************
// 2/1/93 DALLEE Function created.
//************************************************************************
WORD LOCAL PASCAL ReadNetUsersFromNNS(LPSECTIONSUFFIX lpSectionSuffix)
{
    extern      char            SZ_NUM_NETUSERS[];
    extern      char            SZ_NETUSER[];
    extern      char            SZ_ALL[];
    extern      CFGFILE         cfgFile;
    extern      NAVOPTIONS      navOptions;
    auto        LPNETUSERITEM   lpNetUserItem;
    auto        UINT            i;
    auto        char            szBuffer[MAX_INI_LINE+1];
    auto        LPSTR           lpszToken;
    auto        UINT            uNumNetUsers;
    auto        CFGTEXTREC      cfgTextRec;
    auto        char            szEntry[MAX_INI_LINE+1];
    auto        BOOL            bValid;

    WORD        wReturn = INI_OK;

    STRCPY(szEntry, SZ_NUM_NETUSERS);
                                        // Get # of users from NNS
    MEMSET(&cfgTextRec, 0, sizeof(cfgTextRec));
    cfgTextRec.lpSection = SZ_NETUSER;
    cfgTextRec.lpEntry   = szEntry;
    if ( ConfigFileReadValue(&cfgFile, &cfgTextRec,
                             &uNumNetUsers, NULL) == ERR )
        uNumNetUsers = 0;

    if ( uNumNetUsers > MAX_NET_OBJECTS)
        uNumNetUsers = MAX_NET_OBJECTS;

                                        // Allocate memory for the net users
                                        // Always add 1 extra.
    navOptions.netUser.uEntries = uNumNetUsers;
    navOptions.netUser.hNetUserItem = MemAlloc(GHND, (uNumNetUsers + 1) *
                                                     sizeof(NETUSERITEM));
    if ( navOptions.netUser.hNetUserItem == NULL )
        return (INI_FAILED);
    navOptions.netUser.lpNetUserItem = (LPNETUSERITEM)
                                      MemLock(navOptions.netUser.hNetUserItem);

                                        // Read in each entry
    for (i = 0, lpNetUserItem = navOptions.netUser.lpNetUserItem;
                i < uNumNetUsers; i++, lpNetUserItem++)
        {
                                        // Create unique entry
        STRCPY(szEntry, SZ_NETUSER);
        _ConvertWordToString(i + 1, szEntry + STRLEN(szEntry), 10, 0);

        bValid = FALSE;
        if (ConfigFileReadString(&cfgFile, &cfgTextRec,
                                 szBuffer, sizeof(szBuffer)) == ERR)
            {
lastOne:
            navOptions.netUser.uEntries = i;
            break;
            }
        else
            {
                                        //------------------------------
                                        // Parse each entry
                                        //------------------------------
            lpszToken = _fstrtok(szBuffer, SZ_SEPS);
            if ( lpszToken )
                {                       // Get Server
                if ( STRCMP(lpszToken, SZ_ALL) == 0 )
                    {
                                        // Send to all servers
                    lpNetUserItem->szServer[0] = EOS;
                    }
                else
                    {
                    STRNCPY(lpNetUserItem->szServer, lpszToken,
                                              sizeof(lpNetUserItem->szServer));
                    lpNetUserItem->szServer[sizeof(lpNetUserItem->szServer)-1] = EOS;
                    NavStrUpr(lpNetUserItem->szServer);
                    }

                lpszToken = _fstrtok(NULL, SZ_SEPS);
                if ( lpszToken )
                    {                   // Get User/Group name
                    STRNCPY(lpNetUserItem->szObject, lpszToken,
                                              sizeof(lpNetUserItem->szObject));
                    lpNetUserItem->szObject[sizeof(lpNetUserItem->szObject)-1] = EOS;
                    NavStrUpr(lpNetUserItem->szObject);

                    lpszToken = _fstrtok(NULL, SZ_SEPS);
                    if ( lpszToken )
                        {               // User or Group?
                        if (*lpszToken == '1')
                            lpNetUserItem->wFlags = NETUSER_FLAG_ISUSER;
                        else
                            lpNetUserItem->wFlags = 0;

                        bValid = TRUE;
                        }
                    }
                }
                                        // If not valid, quit on this one
            if ( !bValid )
                goto lastOne;
            }
        }

    return (wReturn);
}



#ifdef BANKERS
//************************************************************************
// ReadRegistrationsFromNNS()
//
// This routine reads the list of registered files from the NNS file.
//
// Parameters:
//  Section suffix structure
//
// Returns:
//  INI_OK        - All entries found as requested
//  INI_DEFAULTED - The specific suffix was not found but a default was
//************************************************************************
// 3/08/94 DALLEE, from ReadExclusionsFromNNS()
//************************************************************************

WORD LOCAL PASCAL ReadRegistrationsFromNNS(LPSECTIONSUFFIX lpSectionSuffix)
{
    extern      char            SZ_NUM_REGISTRATIONS[];
    extern      char            SZ_REGISTRATIONS[];
    extern      CFGFILE         cfgFile;
    extern      NAVOPTIONS      navOptions;
    auto        CFGTEXTREC      cfgTextRec;
    auto        char            szEntry[MAX_INI_LINE+1];
    auto        UINT            uNumRegistrations;
    auto        UINT            i;
    auto        char            szBuffer[MAX_INI_LINE+1];
    auto        LPSTR           lpszToken;
    auto        LPEXCLUDEITEM   lpRegisterItem;
    auto        BOOL            bValid;
    auto        WORD            wMask, wFlags;

    WORD        wReturn = INI_OK;

    STRCPY(szEntry, SZ_NUM_REGISTRATIONS);

                                        // Get # of registrations from NNS
    MEMSET(&cfgTextRec, 0, sizeof(cfgTextRec));
    cfgTextRec.lpSection = SZ_REGISTRATIONS;
    cfgTextRec.lpEntry   = szEntry;
    if ( ConfigFileReadValue(&cfgFile, &cfgTextRec,
                             &uNumRegistrations, NULL) == ERR )
        {
        uNumRegistrations = 0;
        }

    if (uNumRegistrations > MAX_EXCLUDES)
        {
        uNumRegistrations = MAX_EXCLUDES;
        }

                                        // Allocate memory for the registrations
                                        // Always add 1 extra.
    navOptions.registration.uEntries = uNumRegistrations;
    navOptions.registration.hExcludeItem = MemAlloc(GHND,
                                                    (uNumRegistrations + 1) *
                                                    sizeof(EXCLUDEITEM));

    if (navOptions.registration.hExcludeItem == NULL)
        {
        return;
        }

    navOptions.registration.lpExcludeItem =
            (LPEXCLUDEITEM) MemLock(navOptions.registration.hExcludeItem);

                                        // Read in each entry
    for (i = 0, lpRegisterItem = navOptions.registration.lpExcludeItem;
         i < uNumRegistrations;
         i++, lpRegisterItem++)
        {
                                        // Create unique entry
        STRCPY(szEntry, SZ_REGISTRATIONS);
        _ConvertWordToString(i + 1, szEntry + STRLEN(szEntry), 10, 0);

        bValid = FALSE;
        if (ConfigFileReadString(&cfgFile, &cfgTextRec,
                                 szBuffer, sizeof(szBuffer)) == ERR)
            {
lastRegistration:
            navOptions.registration.uEntries = i;
            break;
            }
        else
            {
                                        //------------------------------
                                        // Parse each entry
                                        //------------------------------
            lpszToken = _fstrtok(szBuffer, SZ_SEPS);
            if ( lpszToken )
                {
                                        // Get registration item
                STRNCPY(lpRegisterItem->szText,
                        lpszToken,
                        sizeof(lpRegisterItem->szText));

                lpRegisterItem->szText[sizeof(lpRegisterItem->szText)-1] = EOS;
                NavStrUpr(lpRegisterItem->szText);

                lpszToken = _fstrtok(NULL, SZ_SEPS);

                if ( lpszToken )
                    {                   // Subdirectory?
                    lpRegisterItem->bSubDirectory =
                            (*lpszToken == '1') ? TRUE : FALSE;

                    lpszToken = _fstrtok(NULL, SZ_SEPS);
                    if ( lpszToken )
                        {
                                        // Process bit string
                        for (wFlags = 0, wMask = 0x0001; isdigit(*lpszToken);
                                                     wMask <<= 1, lpszToken++)
                            {
                            if ( *lpszToken == '1' )
                                wFlags |= wMask;
                            }

                        lpRegisterItem->wBits = wFlags;
                        bValid = TRUE;
                        }
                    }
                }

            if ( !bValid )
                goto lastRegistration;
            }
        }

    return (wReturn);
} // ReadRegistrationsFromNNS()

#endif  // BANKERS

#endif

