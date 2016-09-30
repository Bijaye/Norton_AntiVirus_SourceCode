//************************************************************************
//
// $Header:   S:/NAVEX/VCS/mvp.cpv   1.14   15 Jan 1999 18:28:58   MKEATIN  $
//
// Description:
//      Contains Macro Virus Protection (MVP) functions.
//
//************************************************************************
// $Log:   S:/NAVEX/VCS/mvp.cpv  $
// 
//    Rev 1.14   15 Jan 1999 18:28:58   MKEATIN
// Fixed an array issue in MPVLoadData.
// 
//    Rev 1.13   11 Nov 1998 11:19:48   DCHI
// Changes to support NAVIEG1.5.
// 
//    Rev 1.12   09 Nov 1998 13:54:48   DCHI
// Remove far keyword from declaration of gstMVP.
// 
//    Rev 1.11   10 Jul 1998 17:48:16   DCHI
// In MVPLoad() under NT, added check for EXE name to determine
// reg key to check for NAVNOTES.
// 
//    Rev 1.10   08 Jul 1998 15:42:28   DCHI
// In MVPLoad() under NT, added check for EXE name to determine reg key to check.
// 
//    Rev 1.9   13 Apr 1998 17:29:52   CEATON
// Modified memory flags for GlobalAlloc() from GMEM_FIXED to GMEM_MOVEABLE.
// 
// 
//    Rev 1.8   18 Sep 1997 14:26:06   DDREW
// Just a little cleanup for NLM (unused functions)
// 
//    Rev 1.7   17 Sep 1997 17:17:02   DDREW
// Now this thing works on NAVNLM
// 
//    Rev 1.6   15 Jul 1997 16:00:58   MKEATIN
// Don't go through the register on WIN16 platforms to find the mvp data
// file - or WIN32 platforms that are not NAVEX15 enabled.
// 
//    Rev 1.5   11 Jul 1997 14:59:08   MKEATIN
// Fixed a missing brace error on the NTK platform.
// 
//    Rev 1.3   10 Jul 1997 18:42:48   MKEATIN
// Under WIN32 MPVLoadData() now looks in the NAV directory.
// 
//    Rev 1.2   18 Apr 1997 17:56:20   AOONWAL
// Modified during DEV1 malfunction
// 
//    Rev ABID  16 Apr 1997 19:45:04   DCHI
// Changes so that mvpdef.dat is only loaded from NAV directory.
// 
//    Rev 1.1   09 Apr 1997 11:39:20   DCHI
// Fixed problem with trying to allocate zero bytes when there is no
// name nor CRC data.
// 
//    Rev 1.0   07 Apr 1997 18:09:46   DCHI
// Initial revision.
// 
//************************************************************************

#include "platform.h"
#include "mvp.h"

#if defined(MVP_ENABLED)

#define MVP_MAGIC_NUM       0xDCDCCDCD

#define MVP_FLAG_WD7_DISAPPROVE_ALL     0x00000001
#define MVP_FLAG_WD8_DISAPPROVE_ALL     0x00000002
#define MVP_FLAG_XL97_DISAPPROVE_ALL    0x00000004

#define MVP_DAT_FILENAME _T("mvpdef.dat")

// If gdwMVPMagicNum is 0xDCDCCDCD, then the data has been initialized

DWORD   gdwMVPMagicNum = 0;

typedef struct tagMVP_MACRO_INF
{
    DWORD           dwCRC;
    DWORD           dwNameOffset;
} MVP_MACRO_INF_T, FAR *LPMVP_MACRO_INF;

typedef struct tagHMVP_MEMORY
{
#if defined(SYM_WIN)
    HGLOBAL         hlpbyData;
#endif

    LPBYTE          lpbyData;
} HMVP_MEMORY_T, FAR *LPHMVP_MEMORY;

#define MVP_MAX_DATA_SIZE   32000

typedef struct tagMVP
{
    DWORD           dwFlags;

    DWORD           dwDataSize;

    HMVP_MEMORY_T   hData;

    DWORD           dwNameBufSize;
    LPBYTE          lpabyNameBuf;

    DWORD           dwWD7Count;
    LPMVP_MACRO_INF lpastWD7MacroInf;

    DWORD           dwWD8Count;
    LPMVP_MACRO_INF lpastWD8MacroInf;

    DWORD           dwXL97Count;
    LPMVP_MACRO_INF lpastXL97MacroInf;

} MVP_T, FAR *LPMVP;

MVP_T gstMVP;

#if defined(SYM_WIN) || defined(SYM_VXD) || defined(SYM_NTK) || defined(SYM_NLM)
typedef HFILE           HMVPFILE;
#else
typedef FILE *          HMVPFILE;
#endif

typedef HMVPFILE FAR *  LPHMVPFILE;

#ifdef SYM_NLM
#include "file.h"
#endif

// To find this #if defined (...) mating #endif, look for //## Mate1
#if defined(SYM_VXD) || defined(SYM_NTK) || defined(SYM_WIN32)

#include "symcfg.h"
#include "xapi.h"
#include "file.h"

//---------------------------------------------------------------------------
//
// Names of external NAV files.  No need of translation.
//
//---------------------------------------------------------------------------
#if defined(SYM_UNIX)
TCHAR gszMVPDataFileName[] = _T("mvpdef.dat");
#else
TCHAR gszMVPDataFileName[] = _T("MVPDEF.DAT");
#endif

//---------------------------------------------------------------------------
//
// Names of registry keys and internal registry settings.  None of these
// strings should be translated.
//
//---------------------------------------------------------------------------

#if defined(SYM_WIN32)
  TCHAR gszNAVRegSection[] = _T("SOFTWARE\\Symantec\\InstalledApps");
#else
  TCHAR gszNAVRegSection[] = _T("InstalledApps");
#endif

#if defined(SYM_NTK)
  TCHAR gszNAVRegKeyname[10] = _T("NAVNT");
#else 
  TCHAR gszNAVRegKeyname[] = _T("NAV95");
#endif

//////////////////////////////////////////////////////////////////////
// M V P   data file location   S E T S
//////////////////////////////////////////////////////////////////////

#if defined(SYM_WIN32)

LPTSTR gaszNAVMSENames[] =
{
    _T("NAVESRV.EXE"), _T("NAVESCAN.EXE"), _T("NAVEAP.EXE")
};

LPTSTR gaszNAVMSERegKeys[] =
{
    _T("NAVMSE1.0"), _T("NAVMSE1.5")
};

typedef struct tagMVP_SET
{
    LPTSTR *        lpaszNames;
    int             nNumNames;
    LPTSTR *        lpaszKeys;
    int             nNumKeys;
} MVP_SET_T, FAR *LPMVP_SET;

MVP_SET_T gastMVPSets[] =
{
    {
        gaszNAVMSENames,    sizeof(gaszNAVMSENames) / sizeof(LPTSTR),
        gaszNAVMSERegKeys,  sizeof(gaszNAVMSERegKeys) / sizeof(LPTSTR)
    }
};

#define MVP_NUM_SETS    (sizeof(gastMVPSets) / sizeof(MVP_SET_T))

//********************************************************************
//
// Function:
//  BOOL MVPGetDirectory()
//
// Parameters:
//  pszDestination  Ptr to SYM_MAX_PATH size buffer for directory
//
// Description:
//  The function iterates through the array gastMVPSets[] looking
//  for a match on the names of each structure.  For each match,
//  the function gets the value of the given key from the registry.
//  The value is interpreted as a directory path.  If the path name
//  of the module is in the given path, the function assumes that
//  the value is the MVP directory.
//
// Returns:
//  TRUE            On success
//  FALSE           On error
//
//********************************************************************

BOOL MVPGetDirectory
(
    PTSTR           pszDestination
)
{
    DWORD           dwModuleNameLen;
    TCHAR           szModuleName[SYM_MAX_PATH];
    DWORD           dwCandNameLen;
    HKEY            hBaseKey;
    int             nSet;
    int             j;

    dwModuleNameLen = GetModuleFileName(NULL,szModuleName,SYM_MAX_PATH);
    if (dwModuleNameLen == 0)
        return(FALSE);

    // Change all forward slashes in the name to backslashes

    for (j=0;j<dwModuleNameLen;j++)
        if (szModuleName[j] == '/')
            szModuleName[j] = '\\';

    // Open the base key

    if (RegOpenKeyEx(HKEY_LOCAL_MACHINE,
                     gszNAVRegSection,
                     0,
                     KEY_READ,
                     &hBaseKey) != ERROR_SUCCESS)
        return(FALSE);

    // Iterate through the sets to determine whether there is
    //  a match on the module name

    for (nSet=0;nSet<MVP_NUM_SETS;nSet++)
    {
        for (j=0;j<gastMVPSets[nSet].nNumNames;j++)
        {
            dwCandNameLen = STRLEN(gastMVPSets[nSet].lpaszNames[j]);
            if (dwModuleNameLen >= dwCandNameLen &&
                STRICMP(gastMVPSets[nSet].lpaszNames[j],
                        szModuleName + dwModuleNameLen -
                            dwCandNameLen) == 0)
            {
                int     nKey;

                // There is a match on the name

                // Now check for a reg key match on the directory

                for (nKey=0;nKey<gastMVPSets[nSet].nNumKeys;nKey++)
                {
                    DWORD   dwRegKeyType;
                    DWORD   dwRegEntryLen;
                    TCHAR   szRegEntry[SYM_MAX_PATH];

                    // Get the value and verify that
                    //  the registry key is a string

                    dwRegEntryLen = sizeof(szRegEntry);
                    if (RegQueryValueEx(hBaseKey,
                                        gastMVPSets[nSet].lpaszKeys[nKey],
                                        0,
                                        &dwRegKeyType,
                                        (LPBYTE)szRegEntry,
                                        &dwRegEntryLen) == ERROR_SUCCESS &&
                        dwRegKeyType == REG_SZ)
                    {
                        // Calculate path length
                        // Change all slashes to backslashes
                        // If the last character of the entry is
                        //  a slash remove it

                        dwRegEntryLen = 0;
                        while (szRegEntry[dwRegEntryLen])
                        {
                            if (szRegEntry[dwRegEntryLen] == '/')
                                szRegEntry[dwRegEntryLen] = '\\';

                            if (szRegEntry[dwRegEntryLen] == '\\' &&
                                szRegEntry[dwRegEntryLen+1] == 0)
                                szRegEntry[dwRegEntryLen] = 0;
                            else
                                ++dwRegEntryLen;
                        }
                    }
                    else
                        dwRegEntryLen = 0;

                    // Compare the paths if the following
                    // conditions are all met:
                    // - The registry key is not empty
                    // - The registry key will fit in the result
                    // - The module name is at least
                    //   the length of the potential path plus
                    //   the size of the module name plus a
                    //   backslash separator
                    // - There is a path separator in the module
                    //   name where it should be

                    if (dwRegEntryLen != 0 &&
                        dwRegEntryLen < SYM_MAX_PATH &&
                        dwModuleNameLen >= dwRegEntryLen +
                            1 + dwCandNameLen &&
                        szModuleName[dwRegEntryLen] == '\\')
                    {
                        // Temporary set the last backslash
                        //  of the module name to zero
                        //  for the comparison

                        szModuleName[dwRegEntryLen] = 0;

                        // Compare the paths

                        if (STRICMP(szRegEntry,szModuleName) == 0)
                        {
                            // Found a match

                            STRCPY(pszDestination,szRegEntry);
                            RegCloseKey(hBaseKey);
                            return(TRUE);
                        }

                        // Add back the backslash

                        szModuleName[dwRegEntryLen] = '\\';
                    }
                } // key iterator of set
            } // name comparator
        } // name iterator of set
    } // set iterator

    // Couldn't find a match

    RegCloseKey(hBaseKey);
    return(FALSE);
}

#endif // #if defined(SYM_WIN32)

//---------------------------------------------------------------------------
//
// BOOL MVPGetNAVDirectory (
//      PTSTR pszDestination
//      );
//
// This function searches for the main NAV directory.  The places
// which it looks at are:
//      SYM_WIN32: Registry
//        SYM_NTK: Registry
//        SYM_VXD: Registry, NAVAP's own section in SYSTEM.INI and at last
//                 it scans through the directories of each device in the
//                 [386Enh] section.
//
// Entry:
//      pszDestination - buffer to store the configuration directory.  This
//                       buffer should be at least SYM_MAX_PATH characters
//                       long.
//
// Exit:
//      TRUE  if successful
//            pszDestination contains a fully qualified path
//            SYM_VXD: path in OEM.
//      FALSE if error
//            pszDestination may be destroyed
//
// Synchronization:
//      This function is not reentrant.
//
//---------------------------------------------------------------------------
BOOL MVPGetNAVDirectory
(
    PTSTR pszDestination
)
{
    auto   DWORD     dwRegKeyType, dwDestinationLength;
    auto   TCHAR     szRegEntry[SYM_MAX_PATH];

#if defined(SYM_WIN32)
    if (GetVersion() < 0x80000000)      // Are we on NT?
    {
        DWORD       dwLen;
        TCHAR       szName[SYM_MAX_PATH];

        dwLen = GetModuleFileName(NULL,szName,SYM_MAX_PATH);
        if (dwLen == 0)
            return(FALSE);

        if (dwLen >= 10 &&
            STRICMP(_T("NAVWNT.EXE"),szName + dwLen - 10) == 0)
            STRCPY(gszNAVRegKeyname, _T("NAVNT"));
        else
        if (dwLen >= 10 &&
            STRICMP(_T("NNTASK.EXE"),szName + dwLen - 10) == 0)
            STRCPY(gszNAVRegKeyname, _T("NAVNOTES"));
        else
            return MVPGetDirectory(pszDestination);
    }
#endif

    static CFGREGKEY rRegistryLocation = {
                                             HKEY_LOCAL_MACHINE,
                                             NULL,
                                             NULL,
                                             NULL,
                                             NULL,
                                             gszNAVRegSection,
                                             gszNAVRegKeyname,
                                             NULL
                                         };

                                        // Validate input parameters

    SYM_VERIFY_BUFFER ( pszDestination, sizeof(TCHAR) * SYM_MAX_PATH );

                                        // First try the Registry.  This
                                        // entry will be in ANSI (SYM_VXD)
                                        // or Unicode (SYM_NTK)

    dwDestinationLength = sizeof(szRegEntry);

#if defined(SYM_VXD) || defined(SYM_NTK)

    if ( ConfigRegQueryValue ( &rRegistryLocation,
                               NULL,
                               &dwRegKeyType,
                               szRegEntry,
                               &dwDestinationLength ) == ERROR_SUCCESS )

#elif defined(SYM_WIN32)

   HKEY  hBaseKey = 0;

   if ((RegOpenKeyEx(HKEY_LOCAL_MACHINE, 
                     gszNAVRegSection, 
                     0, 
                     KEY_READ, 
                     &hBaseKey) == ERROR_SUCCESS) &&
       (RegQueryValueEx(hBaseKey, 
                        gszNAVRegKeyname,
                        0, 
                        &dwRegKeyType, 
                        (LPBYTE)szRegEntry, 
                        &dwDestinationLength ) == ERROR_SUCCESS))

#endif

        {

#if defined(SYM_VXD)
        auto char szRegConvert[SYM_MAX_PATH * sizeof(WCHAR)];

        VxDGenericStringConvert ( szRegEntry, CHARSET_ANSI,
                                  szRegConvert, CHARSET_CURRENT );

        TrueNameGet ( szRegConvert, szRegEntry );

        VxDGenericStringConvert ( szRegEntry, CHARSET_CURRENT,
                                  pszDestination, CHARSET_OEM );

        if ( STRLEN ( pszDestination ) < SYM_MAX_PATH )
            {
            return ( TRUE );
            }

#else

  #if defined(SYM_WIN32)
        RegCloseKey(hBaseKey);
  #endif

        if ( STRLEN ( szRegEntry ) < SYM_MAX_PATH )
            {
            STRCPY ( pszDestination, szRegEntry );

            return ( TRUE );
            }

#endif
        return ( FALSE );
        }

#if defined(SYM_WIN32)
    if (hBaseKey)
        RegCloseKey(hBaseKey);
#endif

    return ( FALSE );
}

//## Mate1
#endif // #if defined(SYM_VXD) || defined(SYM_NTK) || defined(SYM_WIN32)


//********************************************************************
//
// Function:
//  BOOL MVPFileOpen()
//
// Parameters:
//  lpszFileName    Name of file to open
//  lphMVPFile      Ptr to handle variable for handle of open file
//
// Description:
//  Opens the file with the given name in read-only mode.
//
// Returns:
//  TRUE        On success
//  FALSE       On failure
//
//********************************************************************

BOOL MVPFileOpen
(
    LPTSTR      lpszFileName,
    LPHMVPFILE  lphMVPFile
)
{
#if defined(SYM_WIN)
    OFSTRUCT    stOF;

    *lphMVPFile = OpenFile(lpszFileName,
                           &stOF,
                           OF_READ | OF_SHARE_DENY_WRITE);

    if (*lphMVPFile == HFILE_ERROR)
        return(FALSE);
#elif defined(SYM_VXD) || defined(SYM_NTK)
    *lphMVPFile = FileOpen(lpszFileName,
                           OF_READ | OF_SHARE_DENY_WRITE);

#elif defined(SYM_NLM)
    *lphMVPFile = FileOpen(lpszFileName,
                           OF_READ);

    if (*lphMVPFile == HFILE_ERROR)
        return(FALSE);
#else
    *lphMVPFile = fopen(lpszFileName,"rb");

    if (*lphMVPFile == NULL)
        return(FALSE);
#endif

    return(TRUE);
}


//********************************************************************
//
// Function:
//  BOOL MVPFileRead()
//
// Parameters:
//  hMVPFile        Handle to file to close
//  lpbyBuf         Ptr to buffer to write
//  dwNumBytes      Number of bytes to write
//
// Description:
//  Writes the given number of bytes from the given buffer to the
//  given file.
//
// Returns:
//  TRUE        On success
//  FALSE       On failure
//
//********************************************************************

BOOL MVPFileRead
(
    HMVPFILE    hMVPFile,
    LPBYTE      lpbyBuf,
    DWORD       dwNumBytes
)
{
#if defined(SYM_WIN)
    if (_lread(hMVPFile,
               lpbyBuf,
               (UINT)dwNumBytes) != (UINT)dwNumBytes)
        return(FALSE);
#elif defined(SYM_VXD) || defined(SYM_NTK) || defined(SYM_NLM)
    if (FileRead(hMVPFile,
                 lpbyBuf,
                 dwNumBytes) != dwNumBytes)
        return(FALSE);
#else
    if (fread(lpbyBuf,sizeof(BYTE),dwNumBytes,hMVPFile) != dwNumBytes)
        return(FALSE);
#endif

    return(TRUE);
}


//********************************************************************
//
// Function:
//  BOOL MVPFileSeekSet()
//
// Parameters:
//  hMVPFile        Handle to file to close
//  dwOffset        Offset to seek to
//
// Description:
//  Seeks to the given offset relative to the beginning of the file.
//
// Returns:
//  TRUE        On success
//  FALSE       On failure
//
//********************************************************************

BOOL MVPFileSeekSet
(
    HMVPFILE    hMVPFile,
    DWORD       dwOffset
)
{
#if defined(SYM_WIN)
    if (_llseek(hMVPFile,
                dwOffset,
                0) != dwOffset)
        return(FALSE);
#elif defined(SYM_VXD) || defined(SYM_NTK) || defined(SYM_NLM)
    if (FileSeek(hMVPFile,
                 dwOffset,
                 SEEK_SET) != dwOffset)
        return(FALSE);
#else
    if (fseek(hMVPFile,dwOffset,SEEK_SET) != 0)
        return(FALSE);
#endif

    return(TRUE);
}


//********************************************************************
//
// Function:
//  BOOL MVPFileClose()
//
// Parameters:
//  hMVPFile        Handle to file to close
//
// Description:
//  Closes a file.
//
// Returns:
//  TRUE        On success
//  FALSE       On failure
//
//********************************************************************

BOOL MVPFileClose
(
    HMVPFILE    hMVPFile
)
{
#if defined(SYM_WIN)
    if (_lclose(hMVPFile) != 0)
        return(FALSE);
#elif defined(SYM_VXD) || defined(SYM_NTK) || defined(SYM_NLM)
    if (FileClose(hMVPFile) != 0)
        return(FALSE);
#else
    if (fclose(hMVPFile) != 0)
        return(FALSE);
#endif
    return(TRUE);
}


//********************************************************************
//
// Function:
//  BOOL MVPMemAlloc()
//
// Parameters:
//  lphMem          Ptr to handle to memory
//  dwNumBytes      Number of bytes to allocate
//
// Description:
//  Allocates the given number of bytes
//
// Returns:
//  TRUE        On success
//  FALSE       On failure
//
//********************************************************************

BOOL MVPMemAlloc
(
    LPHMVP_MEMORY   lphMem,
    DWORD           dwNumBytes
)
{
#if defined(SYM_WIN)
    lphMem->hlpbyData = GlobalAlloc(GMEM_MOVEABLE | GMEM_ZEROINIT,dwNumBytes);
    if (lphMem->hlpbyData == NULL)
        lphMem->lpbyData = NULL;
    else
        lphMem->lpbyData = (LPBYTE)GlobalLock(lphMem->hlpbyData);
#elif defined(SYM_VXD) || defined(SYM_NTK) || defined(SYM_NLM)
    lphMem->lpbyData = (LPBYTE)MemAllocPtr(GMEM_FIXED,dwNumBytes);
#else
    lphMem->lpbyData = (LPBYTE)malloc(dwNumBytes);
#endif

    if (lphMem->lpbyData == NULL)
        return(FALSE);

    return(TRUE);
}


//********************************************************************
//
// Function:
//  BOOL MVPMemFree()
//
// Parameters:
//  lphMem          Handle to memory to free
//
// Description:
//  Frees the given memory
//
// Returns:
//  TRUE        On success
//  FALSE       On failure
//
//********************************************************************

BOOL MVPMemFree
(
    LPHMVP_MEMORY   lphMem
)
{
#if defined(SYM_WIN)
    GlobalUnlock(lphMem->hlpbyData);
    lphMem->lpbyData = (LPBYTE)GlobalFree(lphMem->hlpbyData);
#elif defined(SYM_VXD) || defined(SYM_NTK)
    lphMem->lpbyData = (LPBYTE)MemFreePtr((LPVOID)lphMem->lpbyData);
#elif defined(SYM_NLM)
    MemFreePtr(lphMem->lpbyData);
    lphMem->lpbyData = NULL;
#else
    free(lphMem->lpbyData);
    lphMem->lpbyData = NULL;
#endif

    if (lphMem->lpbyData != NULL)
        return(FALSE);

    return(TRUE);
}


//********************************************************************
//
// Function:
//  void MVPDecryptData()
//
// Parameters:
//  lpbyData        Ptr to data to decrypt
//  dwSize          Number of bytes of data to decrypt
//
// Description:
//  The function decrypts the data using the following algorithm:
//      1. Start with the second byte
//      2. Decrypt the byte by XORing it with the previous byte
//
// Returns:
//  TRUE        On success
//  FALSE       On failure
//
//********************************************************************

void MVPDecryptData
(
    LPBYTE      lpbyData,
    DWORD       dwSize
)
{
    DWORD       dw;

    if (dwSize < 2)
        return;

    for (dw=1;dw<dwSize;dw++)
        lpbyData[dw] ^= lpbyData[dw-1];
}


//********************************************************************
//
// Function:
//  BOOL MVPLoadData()
//
// Parameters:
//  None
//
// Description:
//  Loads the MVPDEF.DAT data file which has the following format:
//      abyHdr[256] = 0xDC 0xDC 0xDC 0xDC 0xCD 0xCD 0xCD 0xCD
//                    0x01 0x00
//
//      dwInfoSize
//      dwFlags
//      dwNameBufSize
//      dwWD7Count
//      dwWD8Count
//      dwXL97Count
//
//      dwDataSize
//      abyNameBuf[dwNameBufSize]
//      astWD7MacroInf[dwWD7Count]
//      astWD8MacroInf[dwWD8Count]
//      astXL97MacroInf[dwXL97Count]
//
//  The function performs the following validity checks on the data:
//      1. Ensures that the first eight bytes are:
//          0xDC 0xDC 0xDC 0xDC 0xCD 0xCD 0xCD 0xCD
//      2. Ensures that the version is at least 0x01 0x00.
//      3. Ensures that all the name pointers are within the
//          limits of the name buffer.
//      4. Ensures that the size of the data does not exceed
//          MVP_MAX_DATA_SIZE.
//
// Returns:
//  TRUE        On success
//  FALSE       On error
//
//********************************************************************

BOOL MVPLoadData
(
    LPVOID              lpvExtra
)
{
    HMVPFILE    hFile;
    BYTE        abyHdr[10];
    LPBYTE      lpbyDataDst;
    DWORD       dw;
    DWORD       dwSize;
    DWORD       dwDesiredSize;

#if defined(SYM_VXD)
    char        szDatFileName[2 * SYM_MAX_PATH * sizeof(WCHAR)];
#elif defined(SYM_NTK)
    TCHAR       szDatFileName[2 * SYM_MAX_PATH];
#elif defined(SYM_WIN)
    TCHAR       szDatFileName[2 * SYM_MAX_PATH];
#elif defined(SYM_NLM)
    TCHAR       szDatFileName[2 * SYM_MAX_PATH];
#else
    char        szDatFileName[] = MVP_DAT_FILENAME;
#endif

#if defined(SYM_VXD) || defined(SYM_NTK) || (defined(SYM_WIN32) && defined(NAVEX15))

    (void)lpvExtra;

    // Get the pathname of the data file 

    if (MVPGetNAVDirectory(szDatFileName) == FALSE)
        return(FALSE);

#if defined(SYM_UNIX)
    STRCAT(szDatFileName, _T("/"));
#else
    STRCAT(szDatFileName, _T("\\"));
#endif

    STRCAT(szDatFileName,MVP_DAT_FILENAME);

#elif defined(SYM_WIN)

    dwSize = GetModuleFileName((HINSTANCE)lpvExtra,
                               szDatFileName,
                               SYM_MAX_PATH);

    if (dwSize == 0)
        return(FALSE);

    // Search for the preceding backslash

    dw = dwSize - 1;
    while (dw != 0)
    {
        if (szDatFileName[dw] == '\\')
            break;

        --dw;
    }

    if (szDatFileName[dw] != '\\')
        szDatFileName[dw] = '\\';

    dwSize = dw + 1;

    for (dw=0;MVP_DAT_FILENAME[dw];dw++)
        szDatFileName[dwSize++] = MVP_DAT_FILENAME[dw];

    szDatFileName[dwSize] = 0;

#elif defined(SYM_NLM)

    VOID GetStartPath( LPSTR *lpStartPath );

    LPSTR   lpStartPath;

    GetStartPath( &lpStartPath );
    STRCPY( szDatFileName, lpStartPath );
    STRCAT( szDatFileName, MVP_DAT_FILENAME );

#endif

    // Open the data file

    if (MVPFileOpen(szDatFileName,&hFile) == FALSE)
        return(FALSE);

    // Read the header

    if (MVPFileRead(hFile,abyHdr,10) == FALSE)
    {
        MVPFileClose(hFile);
        return(FALSE);
    }

    // Verify the header

    if (abyHdr[0] != 0xDC ||
        abyHdr[1] != 0xDC ||
        abyHdr[2] != 0xDC ||
        abyHdr[3] != 0xDC ||
        abyHdr[4] != 0xCD ||
        abyHdr[5] != 0xCD ||
        abyHdr[6] != 0xCD ||
        abyHdr[7] != 0xCD)
    {
        // Header signature did not match

        MVPFileClose(hFile);
        return(FALSE);
    }

    if (abyHdr[8] < 0x01)   // Major version
    {
        // Incorrect version

        MVPFileClose(hFile);
        return(FALSE);
    }

    /////////////////////////////////////////////////////////////
    // Info section
    /////////////////////////////////////////////////////////////

    // Seek to the beginning of the info section

    if (MVPFileSeekSet(hFile,256) == FALSE)
    {
        MVPFileClose(hFile);
        return(FALSE);
    }

    // Read the info size

    if (MVPFileRead(hFile,
                    (LPBYTE)&dwSize,
                    sizeof(DWORD)) == FALSE)
    {
        MVPFileClose(hFile);
        return(FALSE);
    }

    dwDesiredSize = sizeof(DWORD) +    // dwFlags
                    sizeof(DWORD) +    // dwNameBufSize
                    sizeof(DWORD) +    // dwWD7Count
                    sizeof(DWORD) +    // dwWD8Count
                    sizeof(DWORD);     // dwXL97Count

    if (dwSize < dwDesiredSize)
    {
        MVPFileClose(hFile);
        return(FALSE);
    }

    // Read the flags

    if (MVPFileRead(hFile,
                    (LPBYTE)&gstMVP.dwFlags,
                    sizeof(DWORD)) == FALSE)
    {
        MVPFileClose(hFile);
        return(FALSE);
    }

    // Read name buffer size, assure alignment on DWORD boundary

    if (MVPFileRead(hFile,
                    (LPBYTE)&gstMVP.dwNameBufSize,
                    sizeof(DWORD)) == FALSE ||
        (gstMVP.dwNameBufSize & 3))
    {
        MVPFileClose(hFile);
        return(FALSE);
    }

    // Read WD7 count

    if (MVPFileRead(hFile,
                    (LPBYTE)&gstMVP.dwWD7Count,
                    sizeof(DWORD)) == FALSE)
    {
        MVPFileClose(hFile);
        return(FALSE);
    }

    // Read WD8 count

    if (MVPFileRead(hFile,
                    (LPBYTE)&gstMVP.dwWD8Count,
                    sizeof(DWORD)) == FALSE)
    {
        MVPFileClose(hFile);
        return(FALSE);
    }

    // Read XL97 count

    if (MVPFileRead(hFile,
                    (LPBYTE)&gstMVP.dwXL97Count,
                    sizeof(DWORD)) == FALSE)
    {
        MVPFileClose(hFile);
        return(FALSE);
    }

    /////////////////////////////////////////////////////////////
    // Data section
    /////////////////////////////////////////////////////////////

    // Seek to the beginning of the data section

    if (MVPFileSeekSet(hFile,256 + sizeof(DWORD) + dwSize) == FALSE)
    {
        MVPFileClose(hFile);
        return(FALSE);
    }

    // Read the data size

    if (MVPFileRead(hFile,
                    (LPBYTE)&dwSize,
                    sizeof(DWORD)) == FALSE)
    {
        MVPFileClose(hFile);
        return(FALSE);
    }

    dwDesiredSize = gstMVP.dwNameBufSize +
                    gstMVP.dwWD7Count * sizeof(MVP_MACRO_INF_T) +
                    gstMVP.dwWD8Count * sizeof(MVP_MACRO_INF_T) +
                    gstMVP.dwXL97Count * sizeof(MVP_MACRO_INF_T);

    // Ensure it is within the allowable data size and that
    //  there is enough data in the file

    if (dwDesiredSize > MVP_MAX_DATA_SIZE ||
        dwDesiredSize > dwSize)
    {
        MVPFileClose(hFile);
        return(FALSE);
    }

    if (dwDesiredSize != 0)
    {
        // Allocate the memory

        if (MVPMemAlloc(&gstMVP.hData,dwDesiredSize) == FALSE)
        {
            MVPFileClose(hFile);
            return(FALSE);
        }

        // Read the data

        if (MVPFileRead(hFile,
                        gstMVP.hData.lpbyData,
                        dwDesiredSize) == FALSE)
        {
            MVPMemFree(&gstMVP.hData);
            MVPFileClose(hFile);
            return(FALSE);
        }
    }

    // Close the file

    MVPFileClose(hFile);

    /////////////////////////////////////////////////////////////
    // Initialize the data pointers
    /////////////////////////////////////////////////////////////

    lpbyDataDst = gstMVP.hData.lpbyData;

    // Set name buffer pointer

    gstMVP.lpabyNameBuf = lpbyDataDst;

    MVPDecryptData(lpbyDataDst,
                   gstMVP.dwNameBufSize);

    lpbyDataDst += gstMVP.dwNameBufSize;

    // Set the WD7 information pointer

    gstMVP.lpastWD7MacroInf = (LPMVP_MACRO_INF)lpbyDataDst;

    MVPDecryptData(lpbyDataDst,
                   sizeof(MVP_MACRO_INF_T) * gstMVP.dwWD7Count);

    lpbyDataDst += gstMVP.dwWD7Count * sizeof(MVP_MACRO_INF_T);

    // Check the validity of the name pointers

    for (dw=0;dw<gstMVP.dwWD7Count;dw++)
    {
        if (gstMVP.lpastWD7MacroInf[dw].dwNameOffset >=
            gstMVP.dwNameBufSize)
        {
            MVPMemFree(&gstMVP.hData);
            return(FALSE);
        }
    }

    // Set the WD8 information pointer

    gstMVP.lpastWD8MacroInf = (LPMVP_MACRO_INF)lpbyDataDst;

    MVPDecryptData(lpbyDataDst,
                   sizeof(MVP_MACRO_INF_T) * gstMVP.dwWD8Count);

    lpbyDataDst += gstMVP.dwWD8Count * sizeof(MVP_MACRO_INF_T);

    // Check the validity of the name pointers

    for (dw=0;dw<gstMVP.dwWD8Count;dw++)
    {
        if (gstMVP.lpastWD8MacroInf[dw].dwNameOffset >=
            gstMVP.dwNameBufSize)
        {
            MVPMemFree(&gstMVP.hData);
            return(FALSE);
        }
    }

    // Set the XL97 information pointer

    gstMVP.lpastXL97MacroInf = (LPMVP_MACRO_INF)lpbyDataDst;

    MVPDecryptData(lpbyDataDst,
                   sizeof(MVP_MACRO_INF_T) * gstMVP.dwXL97Count);

    lpbyDataDst += gstMVP.dwXL97Count * sizeof(MVP_MACRO_INF_T);

    // Check the validity of the name pointers

    for (dw=0;dw<gstMVP.dwXL97Count;dw++)
    {
        if (gstMVP.lpastXL97MacroInf[dw].dwNameOffset >=
            gstMVP.dwNameBufSize)
        {
            MVPMemFree(&gstMVP.hData);
            return(FALSE);
        }
    }

    // Set the magic number to indicate successful loading

    gdwMVPMagicNum = MVP_MAGIC_NUM;

    return(TRUE);
}


//********************************************************************
//
// Function:
//  void MVPUnloadData()
//
// Parameters:
//  None
//
// Description:
//  Frees the MVP data buffer if the magic number variable was
//  initialized with the magic number.
//
// Returns:
//  Nothing
//
//********************************************************************

void MVPUnloadData
(
    void
)
{
    if (gdwMVPMagicNum == MVP_MAGIC_NUM)
    {
        MVPMemFree(&gstMVP.hData);
        gdwMVPMagicNum = 0;
    }
}


//********************************************************************
//
// Function:
//  BOOL MVPCheck()
//
// Parameters:
//  nType       One of MVP_WD7, MVP_WD8, MVP_XL97 for specifying
//              the set to use
//  lpabyName   The name of the macro module to check
//  dwCRC       The CRC of the macro module to check
//
// Description:
//  Searches through the given set for an approved macro with the
//  given information.
//
//  If the definition set has not been loaded, everything is approved.
//  If the list is empty for a given set, everything is approved
//  for that set unless the DISAPPROVE_ALL flag is set for that set,
//  in which case all macros for that set all disapproved.
//
//  Otherwise, only macro modules whose information matches any of the
//  items in the set are approved.
//
//  The function assumes that the macro module information for each
//  set is sorted in ascending order based on the CRCs.  A binary
//  search is used.
//
// Returns:
//  TRUE            If approved
//  FALSE           If not approved
//
//********************************************************************

BOOL MVPCheck
(
    int                 nType,
    LPBYTE              lpabyName,
    DWORD               dwCRC
)
{
    long                lLow, lMid, lHigh, l;
    LPMVP_MACRO_INF     lpastMacroInf;
    DWORD               dwApprovedCRC;
    LPBYTE              lpbyApprovedName;
    int                 i;

    if (gdwMVPMagicNum != MVP_MAGIC_NUM)
    {
        return(TRUE);
    }

    // Get the set to compare against

    switch (nType)
    {
        case MVP_WD7:
            if (gstMVP.dwFlags & MVP_FLAG_WD7_DISAPPROVE_ALL)
                return(FALSE);

            if (gstMVP.dwWD7Count == 0)
                return(TRUE);

            lHigh = (long)gstMVP.dwWD7Count - 1;
            lpastMacroInf = gstMVP.lpastWD7MacroInf;
            break;

        case MVP_WD8:
            if (gstMVP.dwFlags & MVP_FLAG_WD8_DISAPPROVE_ALL)
                return(FALSE);

            if (gstMVP.dwWD8Count == 0)
                return(TRUE);

            lHigh = (long)gstMVP.dwWD8Count - 1;
            lpastMacroInf = gstMVP.lpastWD8MacroInf;
            break;

        case MVP_XL97:
            if (gstMVP.dwFlags & MVP_FLAG_XL97_DISAPPROVE_ALL)
                return(FALSE);

            if (gstMVP.dwXL97Count == 0)
                return(TRUE);

            lHigh = (long)gstMVP.dwXL97Count - 1;
            lpastMacroInf = gstMVP.lpastXL97MacroInf;
            break;

        default:
            return(TRUE);
    }

    // Do a binary search

    lLow = 0;
    while (lLow <= lHigh)
    {
        lMid = (lLow + lHigh) / 2;

        dwApprovedCRC = lpastMacroInf[lMid].dwCRC;
        if (dwCRC < dwApprovedCRC)
        {
            // In lower half

            lHigh = lMid - 1;
        }
        else
        if (dwCRC > dwApprovedCRC)
        {
            // In upper half

            lLow = lMid + 1;
        }
        else
        {
            // Found a match

            break;
        }
    }

    // Is it within range?

    if (lLow > lHigh)
    {
        // No match

        return(FALSE);
    }

    // Search backwards

    l = lMid - 1;
    while (l >= 0)
    {
        if (lpastMacroInf[l].dwCRC != dwCRC)
            break;

        // Check the name

        lpbyApprovedName = gstMVP.lpabyNameBuf +
            lpastMacroInf[l].dwNameOffset;

        for (i=0;i<256;i++)
        {
            if (lpabyName[i] == 0 || lpbyApprovedName[i] == 0)
            {
                // Found a match

                return(TRUE);
            }

            if (lpabyName[i] != lpbyApprovedName[i])
                break;
        }

        --l;
    }

    // Search forwards

    l = lMid;
    while (l <= lHigh)
    {
        if (lpastMacroInf[l].dwCRC != dwCRC)
            break;

        // Check the name

        lpbyApprovedName = gstMVP.lpabyNameBuf +
            lpastMacroInf[l].dwNameOffset;

        for (i=0;i<256;i++)
        {
            if (lpabyName[i] == 0 && lpbyApprovedName[i] == 0)
            {
                // Found a match

                return(TRUE);
            }

            if (lpabyName[i] != lpbyApprovedName[i])
                break;
        }

        ++l;
    }

    // No match

    return(FALSE);
}

#ifdef SYM_NLM

LONG FileRead( int hHandle, char *cBuffer, long lLen )
    {
    return (read( hHandle, cBuffer, lLen ));
    }

//FileWrite( hHandle, cBuffer, lLen )
//    {
//    return (write( hHandle, cBuffer, lLen ));
//    }

#endif

#endif // #if defined(MVP_ENABLED)

