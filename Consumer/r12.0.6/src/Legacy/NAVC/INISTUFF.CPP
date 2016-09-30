// Copyright 1993 Symantec Corporation
//***************************************************************************
//
// $Header:   S:/NAVC/VCS/inistuff.cpv   1.4   01 Oct 1998 12:18:14   tcashin  $
//
// Description:
//      Contains INI and NNS routines.
//
// Contains:
//
//***************************************************************************
// $Log:   S:/NAVC/VCS/inistuff.cpv  $
// 
//    Rev 1.4   01 Oct 1998 12:18:14   tcashin
// Changed the navopts version back to 5.0.
// 
//    Rev 1.3   23 Sep 1998 17:23:16   tcashin
// Bump the navopts version number for Barstow.
// 
//    Rev 1.2   31 Jul 1998 10:08:30   tcashin
// Updated the NAVOPTS.DAT version for Atomic.
// 
//    Rev 1.1   20 Jun 1997 11:24:08   DBUCHES
// Bumped NAVOPTS file header version.
// 
//    Rev 1.0   06 Feb 1997 20:56:32   RFULLER
// Initial revision
// 
//    Rev 1.2   13 Jan 1997 19:07:20   MKEATIN
// Fixed Bug in ReadNavBin()
// 
//    Rev 1.1   13 Jan 1997 14:13:48   MKEATIN
// Now we check for NAVOPTS.DAT before NAVCOPTS.DAT
// 
//    Rev 1.0   31 Dec 1996 15:21:26   MKEATIN
// Initial revision.
// 
//    Rev 1.1   11 Dec 1996 17:46:18   JBELDEN
// setup code so navcopts is looked for and if not found navopts.dat is used
// 
//    Rev 1.0   11 Dec 1996 17:12:34   JBELDEN
// Initial revision.
// 
//    Rev 1.11   17 Oct 1995 14:26:20   JWORDEN
// Include common.h file
// Change Get____INI and Set____INI functions to use new SectionSuffix struct
// 
//    Rev 1.10   13 Sep 1995 10:53:34   JWORDEN
// Alter Get...Ini functions to use SectionSuffix info to load
// NNS info for multiple platforms
// Alter Set....Ini functions to save NNS information for a
// desired platform
// Add ReadConfigValue and ReadConfigString functions
// 
//    Rev 1.9   29 Jun 1995 10:12:56   BARRY
// Fix FileRead() bug introduced in previous rev (duh)
// 
//    Rev 1.8   28 Jun 1995 22:07:04   BARRY
// Check FileRead against number of bytes expected -- not HFILE_ERROR
// 
//    Rev 1.7   14 Jun 1995 20:18:44   MARKL
// No longer calls TalkToVxD.  This is done in OPTIONS.CPP in the COMMON
// project, where it belongs.  The VxD needs to know about changes to config
// options as well as exclusions.
// 
//    Rev 1.6   22 Mar 1995 15:58:44   MARKL
// Once the configuration file is loaded in windows, the VxD is notified
// to reload its configuration.
// 
//    Rev 1.5   23 Jan 1995 12:59:58   DALLEE
// Forgot one switch to LoadString() for DOS.
// 
//    Rev 1.4   20 Jan 1995 19:09:36   MFALLEN                                        
// All the DOS and VXD strings are accessed using the LoadString() functions
// that emulates it's Windows counterpart. All the string ID's are stored in the
// stbexts.h file that identical for all the platforms. DOS and VXD strings are
// contained in the UTILSTR.STR file.
// 
//    Rev 1.3   19 Jan 1995 17:41:44   RSTANEV
// Added ReadNavBinFromOpenedFile().
//
//    Rev 1.2   29 Dec 1994 17:41:18   DALLEE
// extern "C" around DOS external strings.
//
//    Rev 1.1   16 Dec 1994 11:00:06   MFALLEN
// Temporarely commented out TSR_
//
//    Rev 1.0   16 Dec 1994 10:36:44   MFALLEN
// Initial revision.
//
//    Rev 1.5   15 Dec 1994 20:13:48   MFALLEN
// From NIRVANA
//
//    Rev 1.4   02 Jun 1994 14:11:48   MFALLEN
// Version checking of NAVOPTS.DAT file.
//
//    Rev 1.3   16 Sep 1993 10:52:08   REESE
// Oops! Fixed typo the prevented successful compilation.
//
//    Rev 1.2   16 Sep 1993 10:46:38   REESE
// Don't create a local copy of NavOptions on the stack when in DOS.
//
//    Rev 1.1   15 Sep 1993 13:57:36   REESE
// Initialize navopts.dat header from DEFAULT_NAV_BIN_HEADER instead of just
// zeros.
//
//    Rev 1.0   12 Aug 1993 11:54:50   MFALLEN
// Initial Revision
//
//    Rev 1.33   02 Aug 1993 14:45:14   MFALLEN
// Make a copy of navoptions before converting them to OEM.
//
//    Rev 1.32   28 Jul 1993 17:20:22   DALLEE
// Use TSR_ON and TSR_OFF macros instead of ugly DOW WIN ifdefs.
//
//    Rev 1.31   25 Jul 1993 23:44:52   BRAD
// Changed data file extension from BIN to DAT.
//
//    Rev 1.30   23 Jul 1993 04:59:26   BRAD
// Don't put dwords in file with commas.
//
//    Rev 1.29   22 Jul 1993 19:21:40   DALLEE
// #ifdef AnsiToOem and OemToAnsi out of DOS compiles.
//
//    Rev 1.28   22 Jul 1993 14:09:36   MFALLEN
// AnsoToOem() OemToAnsi()
//
//    Rev 1.27   20 Jul 1993 00:27:46   DALLEE
// Fixed compiler warnings about using bogus (in DOS) hInstance variable
// without initializing it.
//
//    Rev 1.26   09 Jul 1993 17:34:02   DALLEE
//
//    Rev 1.25   26 Jun 1993 18:42:58   BRAD
//
//    Rev 1.24   26 Jun 1993 18:36:00   BRAD
//
//    Rev 1.23   26 Jun 1993 18:22:54   BRAD
// Added /DEFAULT, for restoring NAVOPTS.DAT to its default state.
//
//    Rev 1.22   23 May 1993 14:37:58   BRAD
// Added DWORD Get/Set.
//
//    Rev 1.21   16 May 1993 22:26:30   BRAD
// Don't modify path into Read/WriteNavBinFrom/ToDir().
//
//    Rev 1.20   16 May 1993 22:17:38   BRAD
// Now use GetNavBinFilename().
//
//    Rev 1.19   16 May 1993 19:57:38   BRAD
// Created ReadNavBinFromDir() and WriteNavBinToDir().
//
//    Rev 1.18   11 May 1993 02:28:04   BRAD
// Put in define, so we only write out short version of 'navOptions'.
//
//    Rev 1.17   30 Apr 1993 22:37:28   BRAD
// Put back the INI routines, since they are needed for NavNet.
// Also, removed the SetDefaultValues(), since this is an error
// condition that should be reported to the user.  Scanners should
// report message, then user can fill in new values.
//
//    Rev 1.16   30 Apr 1993 18:02:26   MFALLEN
//
//    Rev 1.15   30 Apr 1993 16:08:02   MFALLEN
// Default extensions
//
//
//    Rev 1.14   30 Apr 1993 15:25:52   MFALLEN
//
//    Rev 1.13   29 Apr 1993 14:49:44   MFALLEN
//
//    Rev 1.12   29 Apr 1993 03:44:16   BRAD
//
//    Rev 1.11   28 Apr 1993 23:14:38   BRAD
// Added ReadNavBin() and WriteNavBin().
//
//    Rev 1.10   28 Apr 1993 18:11:10   BRAD
//
//    Rev 1.9   28 Apr 1993 18:00:52   BRAD
//
//    Rev 1.8   28 Apr 1993 17:15:42   BRAD
// Added ByteIni stuff.
//
//    Rev 1.7   04 Apr 1993 19:33:50   BRAD
// Marked Windows-specific stuff with SYM_WIN.
//
//    Rev 1.6   04 Apr 1993 18:10:26   BRAD
// Removed references to GetModuleFileName()and DOSGetProgramName().
//
//    Rev 1.5   01 Apr 1993 07:28:26   MFALLEN
//
//    Rev 1.4   31 Mar 1993 21:45:04   MFALLEN
//
//    Rev 1.3   31 Mar 1993 13:02:26   MFALLEN
// Added funcs to get/set network objects
//
//
//    Rev 1.2   31 Mar 1993 01:23:22   BRAD
//
//    Rev 1.1   30 Mar 1993 23:31:44   BRAD
// Renamed NAVWUTIL.H to NAVUTIL.H.
//
//    Rev 1.0   30 Mar 1993 23:21:44   BRAD
// Initial revision.
//
//    Rev 1.8   25 Mar 1993 19:26:24   MFALLEN
//
//    Rev 1.7   25 Mar 1993 10:07:52   MARKL
// InitConfig() now calls ConfigFileTerminate() if there was a previous INI
// file.
//
//    Rev 1.6   24 Mar 1993 14:44:02   MARKL
// Added the InitIni(), InitConfig(), TerminateConfig() routines.  All
// of the GetWordIni(), SetWordIni(), GetStringIni(), and SetStringIni()
// routines have been modified to accept a LPCFGFILE as the first
// parameter.  All INI settings should come from SYMCFG via the information
// contained in this buffer.
//
//
//***************************************************************************

//***************************************************************************
// DEFINES
//***************************************************************************
                                        // This will insure that we only
                                        // read/write the short portion of
                                        // 'navOptions' (of type NAVOPTIONS).
#define CREATE_NAVBIN

#include <stdio.h> //??????????
#include "platform.h"
#include "symcfg.h"
#include "xapi.h"
#include "file.h"

#include "common.h"
#include "options.h"
#include "navbin.h"
#include "navutil.h"
#include "tsr.h"
#include "tsrcomm.h"
#include "stbexts.h"
#include "navopt32.h"
#include "optnames.h"
#include "APOptNames.h"
                                        // ------------------------------------
                                        // Global Data
                                        //------------------------------------
#ifdef __cplusplus
extern "C" {
     #endif

extern BOOL PASCAL FileExistsInDirPath( LPSTR, LPSTR );
extern LPSTR SYM_EXPORT WINAPI GetNavcBinFilename(LPSTR);

#ifdef __cplusplus
}
#endif






                                        // ------------------------------------
                                        // Local function protos
                                        // ------------------------------------
#ifdef SYM_WIN
  VOID LOCAL NavOptsOemToAnsi (LPNAVOPTIONS lpNavOptions);
  VOID LOCAL NavOptsAnsiToOem (LPNAVOPTIONS lpNavOptions);

  void LOCAL PASCAL ConvertBoolean
  (
      UINT FAR *lpwValue,
      LPSTR lpEnd
  );
#else
  #define NavOptsOemToAnsi(x)
#endif

//***************************************************************************
// CODE
//***************************************************************************

//***************************************************************************
//
// BOOL WINAPI ReadNavBinFromOpenedFile (
//      HFILE        hFile,
//      LPNAVOPTIONS lpNavOptions
//      );
//
// Reads the NAV configuration from an opened configuration file.  The
// current file position should be 0.  If you make any changes for backwards
// compatibility, please make them in this function, or any function called
// from here on...  This is because NAVAP.VXD calls here to read the
// configuration...
//
// Entry:
//      hFile        - handle to an opened configuration file ( usually
//                     NAVOPTS.DAT or NAVDEF.DAT )
//      lpNavOptions - pointer to NAVOPTIONS structure where to place the
//                     configuration.
//
// Exit:
//      TRUE  if function succeeded
//            *lpNavOptions contains configuration
//      FALSE if function failed
//            *lpNavOptions is undefined
//
//***************************************************************************
BOOL WINAPI ReadNavBinFromOpenedFile (
    HFILE        hFile,
    LPNAVOPTIONS lpNavOptions
    )
    {
    auto BOOL         bResult = FALSE;
    auto NAVBINHEADER navBinHeader;

                                        // Skip over header

    if ( FileRead ( hFile, &navBinHeader, sizeof ( navBinHeader ) ) ==
         sizeof( navBinHeader ) )
        {
#ifdef BANKERS
        DWORD dwVersion = NAV_VERSION_30_BANKERS;
#else
        DWORD dwVersion = CURRENT_NAVOPT_VERSION;
#endif
                                        // Read option settings

        if ( navBinHeader.dwVersion == dwVersion &&
             FileRead ( hFile, lpNavOptions, sizeof ( NAVOPTIONS ) ) ==
                        sizeof( NAVOPTIONS ) )
            {
            bResult = TRUE;

#ifdef SYM_WIN
            NavOptsOemToAnsi ( lpNavOptions );
#endif
            }
        }

    return (bResult);
    }

#if !defined (SYM_VXD)

//***************************************************************************
// InitIni()
//
// Description:
//      This rouitne initializes the INI file.
//
// Parameters:
//      lpszFileName                    [in] File name to use
//      lpCfgFile                       [out] Buffer to receive cfg file spec
//
// Return Value:
//
//***************************************************************************
// 03/24/1993 MARKL Function Created.
//***************************************************************************
WORD SYM_EXPORT WINAPI InitIni (            // Initailizes INI file
    LPCSTR      lpszFileName,           // [in] INI file to use
    LPCFGFILE   lpCfgFile)              // [out] Buffer to receive cfg spec
{
    auto        char        szBuffer[SYM_MAX_PATH + 1];
#ifdef SYM_WIN
    extern      HINSTANCE       hInstance;
#else
    auto        HINSTANCE       hInstance;      // bogus!
    hInstance;                          // Get rid of uninitialized warning.
#endif

    GetStartDir(hInstance, szBuffer, sizeof(szBuffer) - 1);
    NameAppendFile (szBuffer, lpszFileName);
    return (InitConfig (szBuffer, lpCfgFile));
} // InitIni()



//***************************************************************************
// InitConfig()
//
// Description:
//      Initializes the configuration file
//
// Parameters:
//      lpszFileName                    [in] Configuration file to use
//      lpCfgFile                       [in/out] Cfg File recored to update
//
// Return Value:
//
//***************************************************************************
// 03/24/1993 MARKL Function Created.
// 03/25/1993 MARKL If we have a previous INI file, we deallocate is memory.
//***************************************************************************
WORD SYM_EXPORT WINAPI InitConfig (         // Initailizes configuration file
    LPCSTR      lpszFileName,           // [in] Configuration file to use
    LPCFGFILE   lpCfgFile)              // [in/out] Buffer to update
{
                                        // If we have a previous INI file,
                                        // make sure we deallocate its memory
    if (lpCfgFile->hMem)
        {
        ConfigFileTerminate (lpCfgFile, 0);
        }

                                        // Clear all flags
    MEMSET (lpCfgFile, 0, sizeof (CFGFILE));

                                        // Initialize.  Assigning lpBaseName
                                        // to lpszFileName is okay since
                                        // ConfigFileInit() will copy the
                                        // contents into a buffer.
    lpCfgFile->lpBaseName = (LPBYTE)lpszFileName;
    lpCfgFile->wFlags     = CFG_FULLPATH_TEXT;

    return (ConfigFileInit (lpCfgFile, 0));
} // InitConfig()



//***************************************************************************
// TerminateConfig()
//
// Description:
//      Terminates the use of a configuration file.
//
// Parameters:
//      lpCfgFile                       // [in] Configuartion file info
//
// Return Value:
//
// See Also:
//
//***************************************************************************
// 03/24/1993 MARKL Function Created.
//***************************************************************************
WORD SYM_EXPORT WINAPI TerminateConfig (    // Terminates config session
    LPCFGFILE   lpCfgFile)              // [in] Configuration file info
{
    return (ConfigFileTerminate (lpCfgFile, 0));
} // TerminateConfig()


//***************************************************************************
//***************************************************************************
WORD SYM_EXPORT WINAPI GetByteIni (LPCFGFILE lpCfgFile,
                                   LPBYTEINI lpByte,
                                   LPSECTIONSUFFIX  lpSectionSuffix)
{
   WORD wReturn = INI_OK;

   CFGTEXTREC   cfgTextRec;

   UINT         uVarIndex,
                uPlatformIndex,
                uSuffixIndex,
                uValue,
                uErr;

   char         szSection[80];

   BOOL         bFoundEntry;

   cfgTextRec.lpSwitch = NULL;
   cfgTextRec.wFlags   = CFG_LOCAL_FIRST;

   for (uVarIndex=0; lpByte[uVarIndex].lpSection; uVarIndex++)
       {

       // ************************************************** //
       // Make sure that we need to load this variable for   //
       // the primary platform.                              //
       // ************************************************** //

       if ((lpByte[uVarIndex].wPlatform & lpSectionSuffix->wPlatform[0]) == NULL)
           continue;

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
               // Does this entry exist for this platform?                       //
               // ************************************************** //

               cfgTextRec.lpSection           = szSection;
               cfgTextRec.lpEntry             = lpByte[uVarIndex].lpEntry;

               // ************************************************** //
               // Load the section name and add the primary modifier //
               // ************************************************** //

               STRCPY (szSection, lpByte[uVarIndex].lpSection);
               STRCAT (szSection, lpSectionSuffix->Suffix[uSuffixIndex].lpszSectionSuffix);

               // ************************************************** //
               // Read the value                                     //
               // ************************************************** //

               #ifdef SYM_WIN
                   uErr = ReadConfigValue (lpCfgFile, &cfgTextRec, &uValue);
               #else
                   uErr = ConfigFileReadValue (lpCfgFile,
                                               &cfgTextRec,
                                               &uValue,
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

       // ************************************************** //
       // If read was successful, store the value and        //
       // continue with the next variable. Otherwise return  //
       // the default                                        //
       // ************************************************** //

       if (bFoundEntry)
           *(LPBYTE)(lpByte[uVarIndex].lpVar) = (BYTE)uValue;
       else
           *(LPBYTE)(lpByte[uVarIndex].lpVar) = lpByte[uVarIndex].byDefault;
       }

   return (NOERR);
}


//***************************************************************************
//***************************************************************************
VOID SYM_EXPORT WINAPI SetByteIni (LPCFGFILE lpCfgFile,
                                   LPBYTEINI lpByte,
                                   LPSECTIONSUFFIX  lpSectionSuffix)
{
   CFGTEXTREC   cfgTextRec;

   UINT         uVarIndex,
                uSuffixIndex;

   char         szSection[80];

   cfgTextRec.lpSwitch = NULL;
   cfgTextRec.wFlags   = CFG_LOCAL_FIRST;

   cfgTextRec.lpSection = szSection;

   for (uSuffixIndex = 0; uSuffixIndex < MAX_SUFFIXES; uSuffixIndex++)
     {
     if (lpSectionSuffix->wPlatform[0] & lpSectionSuffix->Suffix[uSuffixIndex].wPlatform)
       break;
     }


   if (uSuffixIndex >= MAX_SUFFIXES)
        return;

   for (uVarIndex = 0; lpByte[uVarIndex].lpSection; uVarIndex++)
      {
      // ************************************************** //
      // Make sure that we need to save this variable for   //
      // the primary platform.                              //
      // ************************************************** //

      if ((lpByte[uVarIndex].wPlatform & lpSectionSuffix->wPlatform[0]) == NULL)
          continue;

      // ************************************************** //
      // Build the section name by appending the primary    //
      // section modifier                                   //
      // ************************************************** //

      STRCPY (szSection, lpByte[uVarIndex].lpSection);
      STRCAT (szSection, lpSectionSuffix->Suffix[uSuffixIndex].lpszSectionSuffix);

      cfgTextRec.lpEntry   = lpByte[uVarIndex].lpEntry;

      ConfigFileWriteValue (lpCfgFile,
                           &cfgTextRec,
                           *(LPBYTE)lpByte[uVarIndex].lpVar);
      }
}


//***************************************************************************
//***************************************************************************
WORD SYM_EXPORT WINAPI GetWordIni (LPCFGFILE lpCfgFile,
                                   LPWORDINI lpWord,
                                   LPSECTIONSUFFIX  lpSectionSuffix)
{
   WORD wReturn = INI_OK;

   CFGTEXTREC   cfgTextRec;

   UINT         uVarIndex,
                uPlatformIndex,
                uSuffixIndex,
                uValue,
                uErr;

   char         szSection[80];

   BOOL         bFoundEntry;

   cfgTextRec.lpSwitch = NULL;
   cfgTextRec.wFlags   = CFG_LOCAL_FIRST;

    for (uVarIndex=0; lpWord[uVarIndex].lpSection; uVarIndex++)
        {

       // ************************************************** //
       // Make sure that we need to load this variable for   //
       // the primary platform.                              //
       // ************************************************** //

       if ((lpWord[uVarIndex].wPlatform & lpSectionSuffix->wPlatform[0]) == NULL)
           continue;

        bFoundEntry = FALSE;

        for (uPlatformIndex = 0;    uPlatformIndex < MAX_SUFFIXES 
                                 && lpSectionSuffix->wPlatform[uPlatformIndex] != PLATFORM_NONE;
                                 uPlatformIndex++)
            {
    
            for (uSuffixIndex = 0; uSuffixIndex < MAX_SUFFIXES; uSuffixIndex++)
                {
                // ************************************************** //
                // Is this suffix valid for this platform?            //
                // ************************************************** //

                if (!(lpSectionSuffix->Suffix[uSuffixIndex].wPlatform & lpSectionSuffix->wPlatform[uPlatformIndex]))
                    continue;

                // ************************************************** //
                // Does this entry exist for this platform?           //                
                // ************************************************** //

                cfgTextRec.lpSection           = szSection;
                cfgTextRec.lpEntry             = lpWord[uVarIndex].lpEntry;

                // ************************************************** //
                // Load the section name and add the primary modifier //
                // ************************************************** //

                STRCPY (szSection, lpWord[uVarIndex].lpSection);
                STRCAT (szSection, lpSectionSuffix->Suffix[uSuffixIndex].lpszSectionSuffix);

                // ************************************************** //
                // Read the value                                     //
                // ************************************************** //

                #ifdef SYM_WIN
                    uErr = ReadConfigValue (lpCfgFile, &cfgTextRec, &uValue);
                #else
                    uErr = ConfigFileReadValue (lpCfgFile,
                                                &cfgTextRec,
                                                &uValue,
                                                NULL);
                #endif

                // ************************************************** //
                // If the read was successful, we need to find a way  //
                // to get out of this mess                            //
                // ************************************************** //

                if (NOERR == uErr)
                    {
                    bFoundEntry = TRUE;
                    break;
                    }
                }

            // *************************************************** //
            // Did we find anything?                               //
            // *************************************************** //

            if (bFoundEntry)
                break;
            }

          // ************************************************** //
          // If read was successful, store the value and        //
          // continue with the next variable. Otherwise return  //
          // the default                                        //
          // ************************************************** //

          if (bFoundEntry)
              *(LPWORD)(lpWord[uVarIndex].lpVar) = (WORD)uValue;
          else
              *(LPWORD)(lpWord[uVarIndex].lpVar) = lpWord[uVarIndex].wDefault;
          }


      return (NOERR);
}


//***************************************************************************
//***************************************************************************
VOID SYM_EXPORT WINAPI SetWordIni (LPCFGFILE lpCfgFile,
                                   LPWORDINI lpWord,
                                   LPSECTIONSUFFIX  lpSectionSuffix)
{
   CFGTEXTREC   cfgTextRec;

   UINT         uVarIndex,
                uSuffixIndex;

   char         szSection[80];

   cfgTextRec.lpSwitch = NULL;
   cfgTextRec.wFlags   = CFG_LOCAL_FIRST;

   cfgTextRec.lpSection = szSection;

   for (uSuffixIndex = 0; uSuffixIndex < MAX_SUFFIXES; uSuffixIndex++)
     {
     if (lpSectionSuffix->wPlatform[0] & lpSectionSuffix->Suffix[uSuffixIndex].wPlatform)
       break;
     }


   if (uSuffixIndex >= MAX_SUFFIXES)
        return;

   for (uVarIndex = 0; lpWord[uVarIndex].lpSection; uVarIndex++)
      {
      // ************************************************** //
      // Make sure that we need to save this variable for   //
      // the primary platform.                              //
      // ************************************************** //

      if ((lpWord[uVarIndex].wPlatform & lpSectionSuffix->wPlatform[0]) == NULL)
          continue;

      // ************************************************** //
      // Build the section name by appending the primary    //
      // section modifier                                   //
      // ************************************************** //

      STRCPY (szSection, lpWord[uVarIndex].lpSection);
      STRCAT (szSection, lpSectionSuffix->Suffix[uSuffixIndex].lpszSectionSuffix);

      cfgTextRec.lpEntry   = lpWord[uVarIndex].lpEntry;

      ConfigFileWriteValue (lpCfgFile,
                           &cfgTextRec,
                           *(LPWORD)lpWord[uVarIndex].lpVar);
      }
}


//***************************************************************************
//***************************************************************************
WORD SYM_EXPORT WINAPI GetDWordIni (LPCFGFILE lpCfgFile,
                                    LPDWORDINI lpDWord,
                                    LPSECTIONSUFFIX  lpSectionSuffix)
{
    WORD wReturn = INI_OK;

    CFGTEXTREC   cfgTextRec;

    UINT         uVarIndex,
                 uPlatformIndex,
                 uSuffixIndex,
                 uErr;

    char         szSection[80],
                    szBuffer[40];
  
    DWORD        dwValue;


    BOOL            bFoundEntry;

    cfgTextRec.lpSwitch = NULL;
    cfgTextRec.wFlags   = CFG_LOCAL_FIRST;

    for (uVarIndex=0; lpDWord[uVarIndex].lpSection; uVarIndex++)
        {

       // ************************************************** //
       // Make sure that we need to load this variable for   //
       // the primary platform.                              //
       // ************************************************** //

       if ((lpDWord[uVarIndex].wPlatform & lpSectionSuffix->wPlatform[0]) == NULL)
           continue;

        bFoundEntry = FALSE;

        for (uPlatformIndex = 0;    uPlatformIndex < MAX_SUFFIXES 
                                 && lpSectionSuffix->wPlatform[uPlatformIndex] != PLATFORM_NONE;
                                 uPlatformIndex++)
            {
    
            for (uSuffixIndex = 0; uSuffixIndex < MAX_SUFFIXES; uSuffixIndex++)
                {
                // ************************************************** //
                // Is this suffix valid for this platform?            //
                // ************************************************** //

                if (!(lpSectionSuffix->Suffix[uSuffixIndex].wPlatform & lpSectionSuffix->wPlatform[uPlatformIndex]))
                    continue;

                // ************************************************** //
                // Does this entry exist for this platform?           //                
                // ************************************************** //

                cfgTextRec.lpSection           = szSection;
                cfgTextRec.lpEntry             = lpDWord[uVarIndex].lpEntry;

                // ************************************************** //
                // Load the section name and add the primary modifier //
                // ************************************************** //

                STRCPY (szSection, lpDWord[uVarIndex].lpSection);
                STRCAT (szSection, lpSectionSuffix->Suffix[uSuffixIndex].lpszSectionSuffix);

                // ************************************************** //
                // Read the value                                     //
                // ************************************************** //

               #ifdef SYM_WIN
                   uErr = ReadConfigString (lpCfgFile,
                                            &cfgTextRec,
                                            szBuffer,
                                            sizeof (szBuffer));
               #else
                   uErr = ConfigFileReadString (lpCfgFile,
                                                &cfgTextRec,
                                                szBuffer,
                                                sizeof (szBuffer));
               #endif

                // ************************************************** //
                // If the read was successful, we need to find a way  //
                // to get out of this mess                            //
                // ************************************************** //

                if (NOERR == uErr)
                    {
                    bFoundEntry = TRUE;
                    break;
                    }
                }

            // *************************************************** //
            // Did we find anything?                               //
            // *************************************************** //

            if (bFoundEntry)
                        break;
                    }

          // ************************************************** //
          // If read was successful, store the value and        //
          // continue with the next variable. Otherwise return  //
          // the default                                        //
          // ************************************************** //

          if (bFoundEntry)
              {
              ConvertStringToLong (&dwValue, szBuffer);
              *(LPDWORD)(lpDWord[uVarIndex].lpVar) = dwValue;
              }
          else
              *(LPDWORD)(lpDWord[uVarIndex].lpVar) = lpDWord[uVarIndex].dwDefault;
          }


      return (NOERR);
}


//***************************************************************************
//***************************************************************************
VOID SYM_EXPORT WINAPI SetDWordIni (LPCFGFILE lpCfgFile,
                                    LPDWORDINI lpDWord,
                                    LPSECTIONSUFFIX  lpSectionSuffix)
{
   CFGTEXTREC      cfgTextRec;

   UINT            uVarIndex,
                   uSuffixIndex;

   char            szBuffer[40],
                   szSection[80];

   cfgTextRec.lpSwitch = NULL;
   cfgTextRec.wFlags   = CFG_LOCAL_FIRST;

   cfgTextRec.lpSection = szSection;

   for (uSuffixIndex = 0; uSuffixIndex < MAX_SUFFIXES; uSuffixIndex++)
     {
     if (lpSectionSuffix->wPlatform[0] & lpSectionSuffix->Suffix[uSuffixIndex].wPlatform)
       break;
     }


   if (uSuffixIndex >= MAX_SUFFIXES)
        return;

   for (uVarIndex = 0; lpDWord[uVarIndex].lpSection; uVarIndex++)
      {
      // ************************************************** //
      // Make sure that we need to save this variable for   //
      // the primary platform.                              //
      // ************************************************** //

      if ((lpDWord[uVarIndex].wPlatform & lpSectionSuffix->wPlatform[0]) == NULL)
          continue;

      // ************************************************** //
      // Build the section name by appending the primary    //
      // section modifier                                   //
      // ************************************************** //

      STRCPY (szSection, lpDWord[uVarIndex].lpSection);
      STRCAT (szSection, lpSectionSuffix->Suffix[uSuffixIndex].lpszSectionSuffix);

      cfgTextRec.lpEntry   = lpDWord[uVarIndex].lpEntry;
                                        // Convert to a string
                                        // Don't use the ',' character
      _ConvertLongToString(*(LPDWORD) lpDWord[uVarIndex].lpVar, szBuffer, 10, 0);

      ConfigFileWriteString (lpCfgFile,
                             &cfgTextRec,
                             szBuffer);
      }
}


//***************************************************************************
//***************************************************************************
WORD SYM_EXPORT WINAPI GetStringIni (LPCFGFILE lpCfgFile,
                                     LPSTRINGINI lpStr,
                                     LPSECTIONSUFFIX  lpSectionSuffix)
{
    WORD wReturn = INI_OK;

    CFGTEXTREC   cfgTextRec;

    UINT         uVarIndex,
                 uPlatformIndex,
                 uSuffixIndex,
                 uErr;

    char         szSection[80];
      
    BOOL         bFoundEntry;

    cfgTextRec.lpSwitch = NULL;
    cfgTextRec.wFlags   = CFG_LOCAL_FIRST;

    for (uVarIndex=0; lpStr[uVarIndex].lpSection; uVarIndex++)
        {

       // ************************************************** //
       // Make sure that we need to load this variable for   //
       // the primary platform.                              //
       // ************************************************** //

       if ((lpStr[uVarIndex].wPlatform & lpSectionSuffix->wPlatform[0]) == NULL)
           continue;

        bFoundEntry = FALSE;

        for (uPlatformIndex = 0;    uPlatformIndex < MAX_SUFFIXES 
                                 && lpSectionSuffix->wPlatform[uPlatformIndex] != PLATFORM_NONE;
                                 uPlatformIndex++)
            {
    
            for (uSuffixIndex = 0; uSuffixIndex < MAX_SUFFIXES; uSuffixIndex++)
                {
                // ************************************************** //
                // Is this suffix valid for this platform?            //
                // ************************************************** //

                if (!(lpSectionSuffix->Suffix[uSuffixIndex].wPlatform & lpSectionSuffix->wPlatform[uPlatformIndex]))
                    continue;

                // ************************************************** //
                // Does this entry exist for this platform?           //                
                // ************************************************** //

                cfgTextRec.lpSection           = szSection;
                cfgTextRec.lpEntry             = (LPSTR)lpStr[uVarIndex].lpEntry;

                // ************************************************** //
                // Load the section name and add the primary modifier //
                // ************************************************** //

                STRCPY (szSection, lpStr[uVarIndex].lpSection);
                STRCAT (szSection, lpSectionSuffix->Suffix[uSuffixIndex].lpszSectionSuffix);

                // ************************************************** //
                // Read the value                                     //
                // ************************************************** //

               #ifdef SYM_WIN
                   uErr = ReadConfigString (lpCfgFile,
                                            &cfgTextRec,
                                            (LPSTR)lpStr[uVarIndex].lpVar,
                                            lpStr[uVarIndex].uSize);
               #else
                   uErr = ConfigFileReadString (lpCfgFile,
                                                &cfgTextRec,
                                                (LPSTR)lpStr[uVarIndex].lpVar,
                                                lpStr[uVarIndex].uSize);
               #endif

                // ************************************************** //
                // If the read was successful, we need to find a way  //
                // to get out of this mess                            //
                // ************************************************** //

                if (NOERR == uErr)
                    {
                    bFoundEntry = TRUE;
                    break;
                    }
                }

            // *************************************************** //
            // Did we find anything?                               //
            // *************************************************** //

            if (bFoundEntry)
                break;
            }

          // ************************************************** //
          // If read was successful, store the value and        //
          // continue with the next variable. Otherwise return  //
          // the default                                        //
          // ************************************************** //

          if (!bFoundEntry)
            {
            
               STRNCPY ((LPSTR)lpStr[uVarIndex].lpVar, lpStr[uVarIndex].lpDefault, lpStr[uVarIndex].uSize);
               ((LPSTR)lpStr[uVarIndex].lpVar)[lpStr[uVarIndex].uSize - 1] = EOS;
            }

          }

      return (NOERR);
}


//***************************************************************************
//***************************************************************************
VOID SYM_EXPORT WINAPI SetStringIni (LPCFGFILE lpCfgFile,
                                     LPSTRINGINI lpStr,
                                     LPSECTIONSUFFIX  lpSectionSuffix)
{
   CFGTEXTREC   cfgTextRec;

   UINT         uVarIndex,
                uSuffixIndex;

   char         szSection[80];

   cfgTextRec.lpSwitch = NULL;
   cfgTextRec.wFlags   = CFG_LOCAL_FIRST;

   cfgTextRec.lpSection = szSection;

   for (uSuffixIndex = 0; uSuffixIndex < MAX_SUFFIXES; uSuffixIndex++)
     {
     if (lpSectionSuffix->wPlatform[0] & lpSectionSuffix->Suffix[uSuffixIndex].wPlatform)
       break;
     }


   if (uSuffixIndex >= MAX_SUFFIXES)
        return;

   for (uVarIndex = 0; lpStr[uVarIndex].lpSection; uVarIndex++)
      {
      // ************************************************** //
      // Make sure that we need to save this variable for   //
      // the primary platform.                              //
      // ************************************************** //

      if ((lpStr[uVarIndex].wPlatform & lpSectionSuffix->wPlatform[0]) == NULL)
          continue;

      // ************************************************** //
      // Build the section name by appending the primary    //
      // section modifier                                   //
      // ************************************************** //

      STRCPY (szSection, lpStr[uVarIndex].lpSection);
      STRCAT (szSection, lpSectionSuffix->Suffix[uSuffixIndex].lpszSectionSuffix);

      cfgTextRec.lpEntry   = (LPSTR)lpStr[uVarIndex].lpEntry;

      ConfigFileWriteString (lpCfgFile,
                             &cfgTextRec,
                             (LPSTR)lpStr[uVarIndex].lpVar);
      }
}


//***************************************************************************
//***************************************************************************
STATUS SYM_EXPORT WINAPI ReadNavBin (LPNAVOPTIONS lpNavOptions)
{
    auto        char            szBuffer[SYM_MAX_PATH + 1];
#ifdef SYM_WIN
    extern      HINSTANCE       hInstance;
#else
    auto        HINSTANCE       hInstance;      // bogus!
    hInstance;                          // Get rid of uninitialized warning.
#endif


    GetStartDir(hInstance, szBuffer, sizeof(szBuffer) - 1);
    return ( ReadNavBinFromDir(szBuffer, lpNavOptions) );
}

#define S( o, b, d ) \
    if (NAVOPTS32_OK != (nos = NavOpts32_GetStringValue(hNavOpts, o, lpNavOptions->b \
                                                                   , sizeof(lpNavOptions->b), d)) \
     && NAVOPTS32_VALUE_NOT_FOUND != nos) \
         uStatus = ERR;
#define N( o, b, d ) \
    if (NAVOPTS32_OK != (nos = NavOpts32_GetDwordValue(hNavOpts, o, &l, d)) \
     && NAVOPTS32_VALUE_NOT_FOUND != nos) \
         uStatus = ERR; \
    else \
         lpNavOptions->b = l;
//***************************************************************************
// Get options from file or set to reasonable defaults
//***************************************************************************
STATUS WINAPI GetNewOptionsFile(LPNAVOPTIONS lpNavOptions, HNAVOPTS32 hNavOpts)
{
    auto DWORD            l;
    auto STATUS           uStatus = NOERR;
    auto NAVOPTS32_STATUS nos;

    ::memset(lpNavOptions, 0, sizeof(NAVOPTIONS));
    /////////////////////////
    // USER Options
    /////////////////////////
    S(USER_Name,    user.szName,    "")
    S(USER_Company, user.szCompany, "")
    /////////////////////////
    // TSR Options
    /////////////////////////
    // Scan
    N(TSR_ScanRun,        tsr.bScanRun,      1)
    N(TSR_ScanMasterBoot, tsr.bScanMBoot,    1)
    N(TSR_ScanBootRecs,   tsr.bScanBootRecs, 1)
    N(TSR_ScanUnknown,    tsr.bScanUnknown,  0)
    N(TSR_ScanScanOpen,   tsr.bScanOpen,     1)
    N(TSR_ScanCreate,     tsr.bScanCreate,   1)
    N(TSR_ScanAll,        tsr.bScanAll,      0)
    N(TSR_ScanMem,        tsr.bScanMem,      1)
    // Actions
    N(TSR_ActionKnown,    tsr.uActionKnown,   1)
    N(TSR_ActionUnknown,  tsr.uActionUnknown, 1)
    // Memory use
    N(TSR_UseHMA,         tsr.bUseHMA, 0)
    N(TSR_UseUMB,         tsr.bUseUMB, 0)
    N(TSR_UseEMS,         tsr.bUseEMS, 0)
    N(TSR_UseXMS,         tsr.bUseXMS, 0)
    // Button display
    N(TSR_DispRepair,     tsr.bDispRepair,     1)
    N(TSR_DispDelete,     tsr.bDispDelete,     1)
    N(TSR_DispContinue,   tsr.bDispContinue,   1)
    N(TSR_DispStop,       tsr.bDispStop,       1)
    N(TSR_DispExclude,    tsr.bDispExclude,    1)
    N(TSR_DispQuarantine, tsr.bDispQuarantine, 1)
    N(TSR_DispUnknownRepair,   tsr.bDispUnknownRepair,   1)
    N(TSR_DispUnknownDelete,   tsr.bDispUnknownDelete,   1)
    N(TSR_DispUnknownContinue, tsr.bDispUnknownContinue, 1)
    N(TSR_DispUnknownExclude,  tsr.bDispUnknownExclude,  1)
    // Heuristics
    N(TSR_EnableHeuristicScan, tsr.bEnableHeuristicScan, 1)
    N(TSR_HeuristicLevel,      tsr.uHeuristicLevel,      2)
    // Disk I/O activity
    N(TSR_Format,    tsr.uFormat,    1)
    N(TSR_WriteHard, tsr.uWriteHard, 1)
    N(TSR_WriteFlop, tsr.uWriteFlop, 0)
    N(TSR_WriteExe,  tsr.uWriteExe,  0)
    N(TSR_ROAttrib,  tsr.uROAttrib,  0)
    // Floppy drives
    N(TSR_ChkFlopAccess, tsr.bChkFlopAccess, 1)
    N(TSR_ChkFlopOnBoot, tsr.bChkFlopOnBoot, 1)
    N(TSR_ChkBothFlops,  tsr.bChkBothFlops,  0)
    // Misc.
    N(TSR_BypassKey,  tsr.uBypassKey,  2)
    N(TSR_Unloadable, tsr.bUnloadable, 1)
    N(TSR_HideIcon,   tsr.bHideIcon,   0)
    /////////////////////////
    // ACTIVITY Options
    /////////////////////////
    N(ACTIVITY_LogKnown,      activity.bKnown,      1)
    N(ACTIVITY_LogUnknown,    activity.bUnknown,    1)
    N(ACTIVITY_LogInocChange, activity.bInocChange, 1)
    N(ACTIVITY_LogVirusLike,  activity.bVirusLike,  1)
    N(ACTIVITY_LogStartEnd,   activity.bStartEnd,   1)
    N(ACTIVITY_LogVirusList,  activity.bVirusList,  1)
    N(ACTIVITY_LogKeepLast,   activity.bKeepLast,   0)
    N(ACTIVITY_LogQuarantine, activity.bQuarantine, 1)
    N(ACTIVITY_LogEntries,    activity.uEntries,    0)
    N(ACTIVITY_LogKeepUpTo,   activity.bKeepUpTo,   1)
    N(ACTIVITY_LogKiloBytes,  activity.uKiloBytes,  0)
    S(ACTIVITY_LogFile,       activity.szLogFile,  "ACTIVITY.LOG")
#ifdef BANKERS
    N(ACTIVITY_LogReqChange,           activity.bReqChange,           1)
    N(ACTIVITY_LogConfigurationChange, activity.bConfigurationChange, 1)
#endif // BANKERS
    /////////////////////////
    // ALERT Options
    /////////////////////////
    S(ALERT_AlertMsg,      alert.szAlertMsg,    "")
    N(ALERT_DispAlertMsg,  alert.bDispAlertMsg,  0)
    N(ALERT_RemoveAfter,   alert.bRemoveAfter,   0)
    N(ALERT_Seconds,       alert.uSeconds,       5)
    N(ALERT_Beep,          alert.bBeep,          1)
    N(ALERT_AlertNetUsr,   alert.bAlertNetUsr,   0)
    N(ALERT_AlertNetCon,   alert.bAlertNetCon,   0)
    N(ALERT_AlertNLM,      alert.bAlertNLM,      0)
    N(ALERT_Known,         alert.bKnown,         0)
    N(ALERT_Unknown,       alert.bUnknown,       0)
    N(ALERT_Inoculation,   alert.bInoculation,   0)
    N(ALERT_VirusLike,     alert.bVirusLike,     0)
    N(ALERT_StartEnd,      alert.bStartEnd,      0)
    N(ALERT_VirListChange, alert.bVirListChange, 0)
    N(ALERT_Quarantine,    alert.bVirusLike,     0)
#ifdef BANKERS
    N(ALERT_AlertOthersRegistration, alert.bRegChange, 1)
#endif // BANKERS
    /////////////////////////
    // INOCULATION Options
    /////////////////////////
    N(INOCULATION_UseSystemInoc, inoc.bUseSystemInoc, 1)
    N(INOCULATION_UseInoc,       inoc.bUseInoc,       0)
    N(INOCULATION_InocFloppies,  inoc.bInocFloppies,  0)
    N(INOCULATION_InocFast,      inoc.bInocFast,      1)
    N(INOCULATION_ActionNewInoc, inoc.uActionNewInoc, 2)
    N(INOCULATION_ActionInocChg, inoc.uActionInocChg, 1)
    // Buttons display
    N(INOCULATION_DispRepair, inoc.bDispRepair, 1)
    N(INOCULATION_DispDelete, inoc.bDispDelete, 0)
    N(INOCULATION_DispInoc,   inoc.bDispInoc,   1)
    N(INOCULATION_DispCont,   inoc.bDispCont,   1)
    N(INOCULATION_DispStop,   inoc.bDispStop,   1)
    N(INOCULATION_DispExcl,   inoc.bDispExcl,   0)
    S(INOCULATION_NetInocPath, inoc.szNetInocPath, "\\NCDTREE")
    /////////////////////////
    // SCANNER Options
    /////////////////////////
    N(SCANNER_Memory,         scanner.bMemory,         1)
    N(SCANNER_MasterBootRec,  scanner.bMasterBootRec,  1)
    N(SCANNER_BootRecs,       scanner.bBootRecs,       1)
    N(SCANNER_ScanAll,        scanner.bScanAll,        1)
    N(SCANNER_Action,         scanner.uAction,         1)
    N(SCANNER_DispRepair,     scanner.bDispRepair,     1)
    N(SCANNER_DispDelete,     scanner.bDispDelete,     1)
    N(SCANNER_DispCont,       scanner.bDispCont,       1)
    N(SCANNER_DispExcl,       scanner.bDispExcl,       1)
    N(SCANNER_DispQuarantine, scanner.bDispQuarantine, 1)
    N(SCANNER_AllowNetScan,   scanner.bAllowNetScan,   1)
    N(SCANNER_AllowScanStop,  scanner.bAllowScanStop,  1)
    N(SCANNER_PrestoNotify,   scanner.bPrestoNotify,   0)
    N(SCANNER_PresFlop,       scanner.bPresFlop,       0)
    N(SCANNER_PresHard,       scanner.bPresHard,       1)
    N(SCANNER_PresNet,        scanner.bPresNet,        1)
    N(SCANNER_EnableHeuristicScan, scanner.bEnableHeuristicScan, 1)
    N(SCANNER_HeuristicLevel,      scanner.uHeuristicLevel,      2)
    /////////////////////////
    // PASSWORD Options
    /////////////////////////
    N(PASSWORD_Protect, password.bProtect,             0)
    N(PASSWORD_MaxProtection, password.bMaxProtection, 1)
    N(PASSWORD_Password, password.dwPassword,          0)
    /////////////////////////
    // GENERAL Options
    /////////////////////////
    N(SCANNER_BackupRep,      general.bBackupRep,      1)
    N(GENERAL_ScanZipFiles,   general.bScanZipFiles,   1)
    N(GENERAL_ScanHighMemory, general.bScanHighMemory, 0)
    S(GENERAL_BackExt,        general.szBackExt,      "VIR")
    S(SCANNER_Ext, general.szExt, "386"
                                 " ACM"
                                 " ACV"
                                 " ADT"
                                 " AX"
                                 " BAT"
                                 " BIN"
                                 " BTM"
                                 " CLA"
                                 " COM"
                                 " CPL"
                                 " CSC"
                                 " CSH"
                                 " DLL"
                                 " DO?"
                                 " DRV"
                                 " EXE"
                                 " HLP"
                                 " HT?"
                                 " IN?"
                                 " JS?"
                                 " JTD"
                                 " MDB"
                                 " MSO"
                                 " MP?"
                                 " OB?"
                                 " OCX"
                                 " OV?"
                                 " PDF"
                                 " PIF"
                                 " PL"
                                 " POT"
                                 " PM"
                                 " PP?"
                                 " PRC"
                                 " RTF"
                                 " SH?"
                                 " SCR"
                                 " SH"
                                 " SMM"
                                 " SYS"
                                 " VB?"
                                 " VSD"
                                 " VSS"
                                 " VST"
                                 " VXD"
                                 " WS?"
                                 " XL?")
    S(GENERAL_Def, general.szDef, "386"
                                 " ACM"
                                 " ACV"
                                 " ADT"
                                 " AX"
                                 " BAT"
                                 " BIN"
                                 " BTM"
                                 " CLA"
                                 " COM"
                                 " CPL"
                                 " CSC"
                                 " CSH"
                                 " DLL"
                                 " DO?"
                                 " DRV"
                                 " EXE"
                                 " HLP"
                                 " HT?"
                                 " IN?"
                                 " JS?"
                                 " JTD"
                                 " MDB"
                                 " MSO"
                                 " MP?"
                                 " OB?"
                                 " OCX"
                                 " OV?"
                                 " PDF"
                                 " PIF"
                                 " PL"
                                 " POT"
                                 " PM"
                                 " PP?"
                                 " PRC"
                                 " RTF"
                                 " SH?"
                                 " SCR"
                                 " SH"
                                 " SMM"
                                 " SYS"
                                 " VB?"
                                 " VSD"
                                 " VSS"
                                 " VST"
                                 " VXD"
                                 " WS?"
                                 " XL?")
    /////////////////////////
    // QUARANTINE Options
    /////////////////////////
    N(QUARANTINE_QuarantineAP,   quarantineset.bQuarantineAP,   0)
    N(QUARANTINE_QuarantineScan, quarantineset.bQuarantineScan, 0)
    N(QUARANTINE_QuarantineInoc, quarantineset.bQuarantineInoc, 0)
    S(QUARANTINE_QuarantineDir,  quarantineset.szQuarantineDir,  "\\QUARANTINE")
    /////////////////////////
    // Custom AP Options
    /////////////////////////
    N(AP_Selected, customAP.bSelected, 0)
    N(AP_Macro,    customAP.uchMacro,  0)
    N(AP_File,     customAP.uchFile,   0)
    N(AP_Boot,     customAP.uchBoot,   0)
    /////////////////////////
    // Custom RESPONSE Options
    /////////////////////////
    N(RESPONSE_Selected,  customScan.bSelected, 0)
    N(RESPONSE_Macro,     customScan.uchMacro,  0)
    N(RESPONSE_File,      customScan.uchFile,   0)
    N(RESPONSE_Boot,      customScan.uchBoot,   0)
    
    return uStatus;
}

//***************************************************************************
//***************************************************************************
STATUS SYM_EXPORT WINAPI ReadNavBinFromDir (LPSTR lpszDir, LPNAVOPTIONS lpNavOptions)
{
#ifdef SYM_WIN
    extern      HINSTANCE       hInstance;
#else
    auto        HINSTANCE       hInstance;
#endif
    auto        STATUS          uStatus = ERR;
    auto        HFILE           hFile = HFILE_ERROR;
    auto        char            szFilename[SYM_MAX_PATH+1];
    auto        char            szPath[SYM_MAX_PATH+1];
    auto        char            szDefault[80];
    auto        CFGTEXTREC      cfgRec;
    auto        HNAVOPTS32      hNavOpts;


    STRCPY(szPath, lpszDir);
    if ( FileExistsInDirPath(szPath, GetNavBinFilename(szFilename) ))
         NameAppendFile(szPath, GetNavBinFilename(szFilename));
    else
         NameAppendFile(szPath, GetNavcBinFilename(szFilename));


                                        // Special DEFAULT switch.
                                        // Will use the alternative
                                        // NAVDEF.DAT file
    LoadString(hInstance, IDS_DEFAULT, szDefault, sizeof(szDefault));

    cfgRec.lpSwitch = szDefault;
    cfgRec.wFlags = CFG_SW_EXACT;
    if ( ConfigSwitchSet(&cfgRec, NULL) )
        {
        auto    char    szDefaultPath[SYM_MAX_PATH+1];


        STRCPY(szDefaultPath, lpszDir);
        NameAppendFile (szDefaultPath, "NAVDEF.DAT");

        FileDelete(szPath);             // Delete the old file
        FileCopy(szDefaultPath, szPath, NULL);
        }

	if (NAVOPTS32_OK == NavOpts32_Allocate(&hNavOpts))
            {
            if (NAVOPTS32_OK == NavOpts32_Load(szPath, hNavOpts, FALSE))
                // Get options from file or set to reasonable defaults
                uStatus = GetNewOptionsFile(lpNavOptions, hNavOpts);
            NavOpts32_Free(hNavOpts);
            }
    return(uStatus);
}

//***************************************************************************
//***************************************************************************
STATUS SYM_EXPORT WINAPI WriteNavBin(LPNAVOPTIONS lpNavOptions)
{
    auto        char            szBuffer[SYM_MAX_PATH + 1];
#ifdef SYM_WIN
    extern      HINSTANCE       hInstance;
#else
    auto        HINSTANCE       hInstance;      // bogus!
    hInstance;                          // Get rid of uninitialized warning.
#endif


    GetStartDir(hInstance, szBuffer, sizeof(szBuffer) - 1);
    return ( WriteNavBinToDir(szBuffer, lpNavOptions) );
}


//***************************************************************************
// WriteNavBinToDir()
//
// This routine writes the NavOptions to the NAVOPTS.DAT file, first disabling
// the NAV tsr if present.
//
// Parameters:
//      LPSTR           lpszDir         Directory to write NAVOPTS.DAT.
//      LPNAVOPTIONS    lpNavOptions    Options settings.
//
// Returns:
//      NOERR                           Success.
//      ERR                             FileOpen or FileCreate error.
//                                          NAVOPTS.DAT not written.
//***************************************************************************
// 7/9/93 DALLEE, Moved TSR disabling code here from COMMON::OPTIONS.C
//***************************************************************************

STATUS SYM_EXPORT WINAPI WriteNavBinToDir (LPSTR lpszDir, LPNAVOPTIONS lpNavOptions)
{
    auto    STATUS          uStatus = ERR;
    auto    HFILE           hFile = HFILE_ERROR;
    auto    NAVBINHEADER    NavBinHeader = DEFAULT_NAV_BIN_HEADER;
#ifdef SYM_WIN //[
    auto    NAVOPTIONS      NavOptions;
#endif //]
    auto    char            szFilename[SYM_MAX_PATH+1];
    auto    char            szPath[SYM_MAX_PATH+1];

    STRCPY(szPath, lpszDir);
    if ( FileExistsInDirPath(szPath, GetNavBinFilename(szFilename) ))
         NameAppendFile(szPath, GetNavBinFilename(szFilename));
    else
         NameAppendFile(szPath, GetNavcBinFilename(szFilename));

    if ((hFile = FileOpen(szPath,OF_WRITE)) == HFILE_ERROR )
       if ((hFile = FileCreate(szPath,FA_NORMAL)) != HFILE_ERROR )
          {
#ifdef BANKERS
          NavBinHeader.dwVersion = NAV_VERSION_30_BANKERS;
#else
          NavBinHeader.dwVersion = CURRENT_NAVOPT_VERSION;
#endif
          FileWrite (hFile, &NavBinHeader, sizeof(NavBinHeader));
          }

    if (hFile != HFILE_ERROR)
        {
                                        // Skip over header
        if ( FileSeek(hFile, sizeof(NAVBINHEADER), SEEK_FROM_BEGINNING) != HFILE_ERROR )
            {
#ifdef SYM_WIN //[
                                        // -----------------------------------
                                        // Note that it is required for Windows
                                        // to maintain the lpNavOptions in ANSI
                                        // format since they are still being
                                        // used.
                                        // -----------------------------------
            MEMCPY (&NavOptions, lpNavOptions, sizeof(NavOptions));
            NavOptsAnsiToOem (&NavOptions);
                                        // Write option settings
            if ( FileWrite(hFile, &NavOptions, sizeof(NavOptions)) != HFILE_ERROR )
#else //][
            if ( FileWrite(hFile, lpNavOptions, sizeof(*lpNavOptions)) != HFILE_ERROR )
#endif //]
                uStatus = NOERR;
            }
        }
                                        // Close the file
    if ( hFile != HFILE_ERROR )
        FileClose(hFile);

    return(uStatus);
} // End WriteNavBinToDir()

#ifdef SYM_WIN

//************************************************************************
// NavOptsAnsiToOem()
//
// This routine converts all the NAV text options from Ansi to Oem strings.
//
// *** WARNING! *** WARNING! *** WARNING! *** WARNING! *** WARNING! ***
//
// This function is #define'd out of DOS compiles.  If changes are made
// here that cause the navoptions to be written to different buffers or
// that could break DOS in other fashions, please cover the routine for
// DOS.
//
// Parameters:
//      LPNAVOPTIONS lpNavOptions       'nuff said
//
// Returns:
//      Nothing
//************************************************************************
// 7/22/93 DALLEE, #ifdef for Windows only.
//************************************************************************

VOID LOCAL NavOptsAnsiToOem (LPNAVOPTIONS lpNavOptions)
{
    AnsiToOem (lpNavOptions->user.szName,
               lpNavOptions->user.szName);
    AnsiToOem (lpNavOptions->user.szCompany,
               lpNavOptions->user.szCompany);
    AnsiToOem (lpNavOptions->activity.szLogFile,
               lpNavOptions->activity.szLogFile);
    AnsiToOem (lpNavOptions->alert.szAlertMsg,
               lpNavOptions->alert.szAlertMsg);
    AnsiToOem (lpNavOptions->general.szBackExt,
               lpNavOptions->general.szBackExt);
    AnsiToOem (lpNavOptions->general.szExt,
               lpNavOptions->general.szExt);
    AnsiToOem (lpNavOptions->general.szDef,
               lpNavOptions->general.szDef);
    AnsiToOem (lpNavOptions->inoc.szNetInocPath,
               lpNavOptions->inoc.szNetInocPath);

} // End NavOptsAnsiToOem


//************************************************************************
// NavOptsOemToAnsi()
//
// This routine converts all the NAV text options from Oem to Ansi strings.
//
// *** WARNING! *** WARNING! *** WARNING! *** WARNING! *** WARNING! ***
//
// This function is #define'd out of DOS compiles.  If changes are made
// here that cause the navoptions to be written to different buffers or
// that could break DOS in other fashions, please cover the routine for
// DOS.
//
// Parameters:
//      LPNAVOPTIONS lpNavOptions       'nuff said
//
// Returns:
//      Nothing
//************************************************************************
// 7/22/93 DALLEE, #ifdef for Windows only.
//************************************************************************

VOID LOCAL NavOptsOemToAnsi (LPNAVOPTIONS lpNavOptions)
{
    OemToAnsi (lpNavOptions->user.szName,
               lpNavOptions->user.szName);
    OemToAnsi (lpNavOptions->user.szCompany,
               lpNavOptions->user.szCompany);
    OemToAnsi (lpNavOptions->activity.szLogFile,
               lpNavOptions->activity.szLogFile);
    OemToAnsi (lpNavOptions->alert.szAlertMsg,
               lpNavOptions->alert.szAlertMsg);
    OemToAnsi (lpNavOptions->general.szBackExt,
               lpNavOptions->general.szBackExt);
    OemToAnsi (lpNavOptions->general.szExt,
               lpNavOptions->general.szExt);
    OemToAnsi (lpNavOptions->general.szDef,
               lpNavOptions->general.szDef);
    OemToAnsi (lpNavOptions->inoc.szNetInocPath,
               lpNavOptions->inoc.szNetInocPath);

} // End NavOptsOemToAnsi()

//**********************************************************************
// ReadConfigValue()
//
//
// Description:
//      This function retrieves a value from an entry in the NNS
//      file.  Boolean strings (TRUE, FALSE, YES, NO, ON, OFF) in
//      INI files are converted to TRUE/FALSE values.
//
// Parameters:
//      lpFile          The NNS file parameter block
//      lpConfig        The text config record.
//      lpwValue        Returns the value of the setting.
//
// Return Value:
//      NOERR - the entry (section and entry) exists
//      ERR - The entry is not found
//
// Note:
//      This function was constructed in place of ConfigFileReadValue
//      because we need to know if an entry was found or not
//
// See Also:
//
//**********************************************************************
// 09/07/1995 JWorden Created
//**********************************************************************


UINT SYM_EXPORT WINAPI ReadConfigValue
(
    LPCFGFILE           lpFile,
    LPCFGTEXTREC        lpConfig,
    UINT FAR            *lpwValue
)
{
    UINT        uErr = (UINT) ERR;

    LPSTR       lpEnd;

    char        szValue [MAX_INI_LINE + 1];

    #define DEFAULT_NO_DATA   "$#* No Data &^%"

                                        // get the string value for the
                                        // setting
    uErr = ReadConfigString (lpFile,
                             lpConfig,
                             szValue,
                             sizeof (szValue));

    if (uErr == NOERR)
        {                               // we got something

        if (lpwValue)                   // do we really care about the value?
            {
            lpEnd = (LPSTR) szValue;

            while (IS_WHITE(*lpEnd))    // clear leading white space
                lpEnd++;

                                        // convert the numerical value, or
                                        // convert a boolean string value to
                                        // a zero or one.
            if (CharIsNumeric (*lpEnd))
                ConvertStringToWord (lpwValue, lpEnd);
            else
                ConvertBoolean (lpwValue, lpEnd);
            }
        }

    return (uErr);
}

//**********************************************************************
// ReadConfigString()
//
//
// Description:
//      This function retrieves a string value from an entry in the
//      NNS file.
//
// Parameters:
//      lpFile          The NNS file parameter block
//      lpConfig        The text config record.
//      lpwValue        Returns the value of the setting.
//      wSize           Maximum string size
//
// Return Value:
//      NOERR - the entry (section and entry) exists
//      ERR - The entry is not found
//
// Note:
//      This function was constructed in place of ConfigFileReadValue
//      because we need to know if an entry was found or not
//
// See Also:
//
//**********************************************************************
// 09/07/1995 JWorden Created
//**********************************************************************


UINT SYM_EXPORT WINAPI ReadConfigString
(
    LPCFGFILE           lpFile,
    LPCFGTEXTREC        lpConfig,
    LPSTR               lpszValue,
    WORD                wSize
)
{
    UINT        uErr = (UINT) ERR;

    #define DEFAULT_NO_DATA   "$#* No Data &^%"

                                        // get the string value for the
                                        // setting
    GetPrivateProfileString (lpConfig->lpSection,
                             lpConfig->lpEntry,
                             DEFAULT_NO_DATA,
                             lpszValue,
                             wSize,
                             lpFile->szPath);

    if (STRCMP (lpszValue, DEFAULT_NO_DATA))
        {
        uErr = NOERR;                   // we got something
        }
    else                                // not found
        {
        uErr = (UINT) ERR;              // nope, wasn't there
        lpszValue[0] = 0;               // return a null string
        }

    return (uErr);
}

//**********************************************************************
// ConvertBoolean ()
//
//
// Description:
//      This function converts a string containing a token representing
//      a boolean value into the corresponding boolean value (TRUE/FALSE)
//
// Parameters:
//      lpFile          The NNS file parameter block
//      lpConfig        The text config record.
//      lpwValue        Returns the value of the setting.
//      wSize           Maximum string size
//
// Return Value:
//      NOERR - the entry (section and entry) exists
//      ERR - The entry is not found
//
// See Also:
//
//**********************************************************************
// 09/07/1995 JWorden Stolen shamelessly from CFG_TEXT.CPP
//**********************************************************************


static struct
    {
    LPSTR       lpName;
    UINT        wValue;
    UINT        wLength;
    } rBoolRec[] =
        {
            { "TRUE",   TRUE  , 1},
            { "FALSE",  FALSE , 1},
            { "ON",     TRUE  , 2},
            { "OFF",    FALSE , 2},
            { "YES",    TRUE  , 1},
            { "NO",     FALSE , 1},
            { NULL }
        };

void LOCAL PASCAL ConvertBoolean
(
    UINT FAR *lpwValue,
    LPSTR lpEnd
)
{
    UINT        x = 0;

    STRUPR (lpEnd);                     // uppercase for ease

    while (rBoolRec[x].lpName)
        {
        if (STRNCMP(rBoolRec[x].lpName, lpEnd, rBoolRec[x].wLength) == 0)
            {
            *lpwValue = rBoolRec[x].wValue;
            return;
            }

        x++;
        }

    *lpwValue = FALSE;
}


#endif  // SYM_WIN

#endif // !defined (SYM_VXD)
