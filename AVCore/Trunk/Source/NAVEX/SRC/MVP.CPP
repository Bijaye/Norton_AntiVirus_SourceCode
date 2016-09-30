//************************************************************************
//
// $Header:   S:/NAVEX/VCS/mvp.cpv   1.2   18 Apr 1997 17:56:20   AOONWAL  $
//
// Description:
//      Contains Macro Virus Protection (MVP) functions.
//
//************************************************************************
// $Log:   S:/NAVEX/VCS/mvp.cpv  $
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

#if defined(SYM_NTK)
#define MVP_DAT_FILENAME L"mvpdef.dat"
#else
#define MVP_DAT_FILENAME "mvpdef.dat"
#endif

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

MVP_T far gstMVP;


#if defined(SYM_WIN) || defined(SYM_VXD) || defined(SYM_NTK)
typedef HFILE           HMVPFILE;
#else
typedef FILE *          HMVPFILE;
#endif

typedef HMVPFILE FAR *  LPHMVPFILE;


#if defined(SYM_VXD) || defined(SYM_NTK)

#include "symcfg.h"
#include "xapi.h"
#include "file.h"

//---------------------------------------------------------------------------
//
// Names of registry keys and internal registry settings.  None of these
// strings should be translated.
//
//---------------------------------------------------------------------------
TCHAR gszNAVAPConfigRegSection[] = _T("InstalledApps");
#if defined(SYM_NTK)
TCHAR gszNAVAPConfigRegKeyname[] = _T("NAVNT");
#elif defined(SYM_VXD)
TCHAR gszNAVAPConfigRegKeyname[] = _T("NAV95");
#endif

//---------------------------------------------------------------------------
//
// Names of external NAV files.  No need of translation.
//
//---------------------------------------------------------------------------
TCHAR gszMVPDataFileName[] = _T("MVPDEF.DAT");

//---------------------------------------------------------------------------
//
// BOOL MVPGetNAVAPConfigurationDirectory (
//      PTSTR pszDestination
//      );
//
// This function searches for NAVAP's configuration directory.  The places
// which it looks at are:
//      SYM_NTK: Registry
//      SYM_VXD: Registry, NAVAP's own section in SYSTEM.INI and at last
//               it scans through the directories of each device in the
//               [386Enh] section.
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
BOOL MVPGetNAVAPConfigurationDirectory
(
    PTSTR pszDestination
)
{
    auto   DWORD     dwRegKeyType, dwDestinationLength;

#if defined(SYM_VXD)
    auto   char      szRegEntry[SYM_MAX_PATH * sizeof(WCHAR)];
#else
    auto   TCHAR     szRegEntry[SYM_MAX_PATH];
#endif

    static CFGREGKEY rNAVAPRegistryLocation = {
                                              HKEY_LOCAL_MACHINE,
                                              NULL,
                                              NULL,
                                              NULL,
                                              NULL,
                                              gszNAVAPConfigRegSection,
                                              gszNAVAPConfigRegKeyname,
                                              NULL
                                              };

                                        // Validate input parameters

    SYM_VERIFY_BUFFER ( pszDestination, sizeof(TCHAR) * SYM_MAX_PATH );

                                        // First try the Registry.  This
                                        // entry will be in ANSI (SYM_VXD)
                                        // or Unicode (SYM_NTK)

    dwDestinationLength = sizeof(szRegEntry);

    if ( ConfigRegQueryValue ( &rNAVAPRegistryLocation,
                               NULL,
                               &dwRegKeyType,
                               szRegEntry,
                               &dwDestinationLength ) == ERROR_SUCCESS )
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

        if ( STRLEN ( szRegEntry ) < SYM_MAX_PATH )
            {
            STRCPY ( pszDestination, szRegEntry );

            return ( TRUE );
            }

#endif

        return ( FALSE );
        }


    return ( FALSE );
}

#endif // #if defined(SYM_VXD) || defined(SYM_NTK)


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
#elif defined(SYM_VXD) || defined(SYM_NTK)
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
#elif defined(SYM_VXD) || defined(SYM_NTK)
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
#elif defined(SYM_VXD) || defined(SYM_NTK)
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
    lphMem->hlpbyData = GlobalAlloc(GMEM_FIXED,dwNumBytes);
    if (lphMem->hlpbyData == NULL)
        lphMem->lpbyData = NULL;
    else
        lphMem->lpbyData = (LPBYTE)GlobalLock(lphMem->hlpbyData);
#elif defined(SYM_VXD) || defined(SYM_NTK)
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
#else
    char        szDatFileName[] = MVP_DAT_FILENAME;
#endif

#if defined(SYM_VXD) || defined(SYM_NTK)

    (void)lpvExtra;

    // Get the pathname of the data file

    if (MVPGetNAVAPConfigurationDirectory(szDatFileName) == FALSE)
        return(FALSE);

#if defined(SYM_NTK)
    STRCAT(szDatFileName,L"\\");
#else
    STRCAT(szDatFileName,"\\");
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

#endif // #if defined(MVP_ENABLED)

