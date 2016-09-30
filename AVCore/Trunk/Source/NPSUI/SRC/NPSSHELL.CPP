#include "platform.h"

#ifdef SYM_WIN32

#include "stdhelp.h"
#include "xapi.h"
#include "file.h"
#include "nps.h"
#include <stdlib.h>
#include <ole2.h>
#include <shlguid.h>
#include <shlobj.h>

// ---------------------------------------------------------------------------
//  Static variable that is set to true if the application called 
//  NpsInitShellLink() and the initialization was successful.
// ---------------------------------------------------------------------------

typedef IShellLink FAR* LPSHELLLINK;

static  BOOL        bCanUseShellAPI =   FALSE;
static  LPSHELLLINK g_pShellLink    =   NULL;
static  LPUNKNOWN   g_pUnkOuter     =   NULL;
static  HRESULT     g_hr            =   NULL;

// ---------------------------------------------------------------------------
//                  LOCAL FUNCTION PROTOTYPES
// ---------------------------------------------------------------------------

BOOL LOCAL ShellLinkBuildPath (
    LPCSTR  lpFolder,                    // NULL for startup folder
    LPCSTR  lpCmdLine,
    LPCSTR  lpName,
    LPSTR   lpOutput
    );


// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

BOOL WINAPI InitShellLink (
    VOID
    )
{
    bCanUseShellAPI = FALSE;

    if ( SUCCEEDED(CoInitialize(NULL)) )
        {
        g_hr = CoCreateInstance (
                                CLSID_ShellLink,
                                g_pUnkOuter,
                                CLSCTX_ALL/*CLSCTX_INPROC*/,
                                IID_IShellLink,
                                (LPVOID *)&g_pShellLink
                                );

        if ( (FAILED(g_hr) ) )
            {
            EndShellLink();
            }
        else
            {
            bCanUseShellAPI = TRUE;
            }
        }

    return ( bCanUseShellAPI );
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

VOID WINAPI EndShellLink (
    VOID
    )
{
    CoUninitialize();
    bCanUseShellAPI =   FALSE;
    g_pShellLink    =   NULL;
    g_pUnkOuter     =   NULL;
    g_hr            =   NULL;
}


// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

BOOL WINAPI NpsShellLinkCreate (
    LPCSTR  lpFolder,                    // NULL for startup folder
    LPCSTR  lpCmdLine,
    LPCSTR  lpArguments,
    LPCSTR  lpName,
    LPCSTR  lpIconPath,
    int     nIconIndex,
    int     nShowCommand,
    LPCSTR  lpDefDir
    )
{
    char szPath[SYM_MAX_PATH];

    if ( bCanUseShellAPI == FALSE ||
        !lpCmdLine ||
        !ShellLinkBuildPath(lpFolder,lpCmdLine,lpName,szPath) )
        {
        return FALSE;
        }

    SYM_ASSERT ( g_pShellLink );
    SYM_ASSERT ( (FAILED(g_hr) == FALSE ) );

    STRCAT (szPath,".LNK");

    g_pShellLink->SetShowCmd(nShowCommand);
    g_pShellLink->SetPath(lpCmdLine);

    if ( lpArguments )
        g_pShellLink->SetArguments ( lpArguments );

    if ( lpName )
        g_pShellLink->SetDescription(lpName);

    if ( lpIconPath )
        g_pShellLink->SetIconLocation(lpIconPath,nIconIndex);

    if ( lpDefDir )
        g_pShellLink->SetWorkingDirectory(lpDefDir);
        
    BOOL bReturn = FALSE;

    LPPERSISTFILE pFile = NULL;

    if ( !FAILED(g_pShellLink->QueryInterface (
                                            IID_IPersistFile,
                                            (LPVOID *)&pFile
                                            ) ) )
        {
        WCHAR szOlePath[SYM_MAX_PATH];

        if ( MultiByteToWideChar (
                                CP_OEMCP,
                                MB_PRECOMPOSED,
                                szPath,
                                -1,
                                szOlePath,
                                SYM_MAX_PATH
                                ) )
            {
            bReturn = (pFile->Save(szOlePath,FALSE) == S_OK) ? TRUE : FALSE;
            }

        pFile->Release();
        }

    g_pShellLink->Release();

    return ( bReturn );
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

BOOL WINAPI NpsShellLinkDestroy (
    LPCSTR  lpFolder,                   // NULL for startup folder
    LPCSTR  lpCmdLine,                  // Required
    LPCSTR  lpName                      // Optional
    )
{
    auto    BOOL    bReturn = FALSE;
    auto    char    szPath1 [ SYM_MAX_PATH + 1 ] = { 0 };
    auto    char    szPath2 [ SYM_MAX_PATH + 1 ] = { 0 };

    if ( bCanUseShellAPI == TRUE &&
        ShellLinkBuildPath ( lpFolder, lpCmdLine, lpName, szPath1 ) )
        {
        STRCPY ( szPath2, szPath1 );
        STRCAT ( szPath1, ".LNK" );
        STRCAT ( szPath2, ".PIF" );

        if ( DeleteFile ( szPath1 ) || DeleteFile ( szPath2 ) )
            bReturn = TRUE;
        }

    return ( bReturn );
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

BOOL WINAPI NpsShellLinkExists (
    LPCSTR  lpFolder,                   // NULL for startup folder
    LPCSTR  lpCmdLine,                  // Required
    LPCSTR  lpName                      // Optional
    )
{
    auto    BOOL    bReturn = FALSE;
    auto    char    szPath1 [ SYM_MAX_PATH + 1 ] = { 0 };
    auto    char    szPath2 [ SYM_MAX_PATH + 1 ] = { 0 };

    if ( bCanUseShellAPI == TRUE &&
         ShellLinkBuildPath ( lpFolder, lpCmdLine, lpName, szPath1 ) )
        {
        STRCPY ( szPath2, szPath1 );
        STRCAT ( szPath1, ".LNK" );
        STRCAT ( szPath2, ".PIF" );

        if ( FileExists ( szPath1 ) || FileExists ( szPath2 ) )
            bReturn = TRUE;
        }

    return ( bReturn );
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

BOOL LOCAL ShellLinkBuildPath (
    LPCSTR  lpFolder,                    // NULL for startup folder
    LPCSTR  lpCmdLine,
    LPCSTR  lpName,
    LPSTR   lpOutput
    )
{
    auto    BOOL    bReturn = FALSE;
    auto    char    szName [ SYM_MAX_PATH + 1 ] = { 0 };

    if ( bCanUseShellAPI == TRUE && lpCmdLine && *lpCmdLine )
        {
        if ( lpName && *lpName )
            STRCPY ( szName, lpName );
        else
            NameReturnFile ( lpCmdLine, szName );

        NameStripExtension(szName);

        if ( STRLEN ( szName ) )
            {
            if ( lpFolder == NULL )
                {
                LPITEMIDLIST pidl;

                SHGetSpecialFolderLocation ( NULL, CSIDL_STARTUP, &pidl );
                SHGetPathFromIDList ( pidl, lpOutput );
                }
            else
                STRCPY ( lpOutput, lpFolder );

            NameAppendFile ( lpOutput, szName );

            /*
            STRCAT ( lpOutput, "\\" );
            STRCAT ( lpOutput, szName );
            */

            bReturn = TRUE;
            }
        }

    return ( bReturn );
}

#endif                                  // SYM_WIN32

