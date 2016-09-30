// Copyright 1993-1997 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/NAVWORK/VCS/n_zip.cpv
//
// Description:
//      This implementation file will handle the loading and calling of the
//   Zip engine.  This will now load the enging dynamically so that we can
//   recover if anything bad happens to us.
//
// Contains:
//
// See Also:
//************************************************************************
// $Log:   S:/NAVWORK/VCS/n_zip.cpv  $
// 
//    Rev 1.1   12 Aug 1997 21:02:30   KSACKIN
// Wrapped the zip scanning functions with IfDefs for Dec Alpha.  We should not
// be trying to load the code from the Zip Engine, since we are not even
// scanning zip files.
//
//    Rev 1.0   11 Aug 1997 01:06:06   KSACKIN
// Initial revision.
//************************************************************************

#include "platform.h"
#include "xapi.h"
#include "navwzip.h"
#include "nunavstb.h"


/////////////////////////////////////////////////////////////////////
// Typedefs

typedef ZIP_STATUS (WINAPI *NAVZIPINIT) (VOID);
typedef ZIP_STATUS (WINAPI *NAVZIPCLEANUP) (VOID);
typedef ZIP_STATUS (WINAPI *NAVZIPPROCESSFILE) (LPUNZIP_STRUCT lpUnZipStruct);

STATIC TCHAR NAVZIPINIT_NAME           [] = "_ZipInit@0";
STATIC TCHAR NAVZIPCLEANUP_NAME        [] = "_ZipCleanup@0";
STATIC TCHAR NAVZIPPROCESSFILE_NAME    [] = "_ZipProcessFile@4";

/////////////////////////////////////////////////////////////////////
// Local Zip scanning variables.

                                        // Dll's instance.
extern  HINSTANCE hInstance;


                                        // Zip scanning dll instance.
HINSTANCE hZipInstance=NULL;

STATIC  TCHAR               szNavZip []         = "N32ZIP.DLL";

STATIC  NAVZIPINIT         pfnZipInit          = NULL;
STATIC  NAVZIPPROCESSFILE  pfnZipProcessFile   = NULL;
STATIC  NAVZIPCLEANUP      pfnZipCleanup       = NULL;


/////////////////////////////////////////////////////////////////////
// Local Function Prototypes.

void DisplayErrorMessage(
    HWND  hParent,
    DWORD dwMessageID,
    DWORD dwMessageBoxCaptionID = IDS_STD_ERROR_MESSAGE_CAPTION
    );

void NavZipCleanup( void );
void NavZipInit( void );

/////////////////////////////////////////////////////////////////////
// Implementation.

/////////////////////////////////////////////////////////////////////
// void NavZipInit( void )
//
// Purpose:
//  This will be a function pass through for the Nav Zip engine.  The
//  reason for these functions is so that we can load the NavZip
//  dynamically, and be able to recover from bad zip files.
//
// Parameters:
//    None.
//
// Returns:
//    None.
//
// See Also:
//    NavZip project for actual functions.
//
/////////////////////////////////////////////////////////////////////
// 08-08-97 - KSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
void NavZipInit( void )
{
#ifndef _M_ALPHA

TCHAR szNavZipFileName[SYM_MAX_PATH];
BOOL  bOK=TRUE;

                                        // Fixup DLL file name.
    GetModuleFileName( hInstance, szNavZipFileName, SYM_MAX_PATH );
    NameStripFile( szNavZipFileName );
    NameAppendFile( szNavZipFileName, szNavZip );

                                        // Load our NavZip library.
    hZipInstance = LoadLibrary( szNavZipFileName );

                                        // Safeguard against a failed load.
    if ( NULL == hZipInstance )
        {
        bOK=FALSE;
        goto InitExit;
        }

                                        // Load our function addresses.
    pfnZipInit        = (NAVZIPINIT)       GetProcAddress( hZipInstance, NAVZIPINIT_NAME );
    pfnZipProcessFile = (NAVZIPPROCESSFILE)GetProcAddress( hZipInstance, NAVZIPPROCESSFILE_NAME );
    pfnZipCleanup     = (NAVZIPCLEANUP)    GetProcAddress( hZipInstance, NAVZIPCLEANUP_NAME );

                                        // Make sure we loaded all of our
                                        // functions correctly.
    if ( !(pfnZipInit && pfnZipProcessFile && pfnZipCleanup) )
        {
                                        // An error occurred loading one of our
                                        // function names.  Set all function
                                        // pointers to NULL, and get out of
                                        // here.
        pfnZipInit          = NULL;
        pfnZipProcessFile   = NULL;
        pfnZipCleanup       = NULL;

        bOK = FALSE;
        goto InitExit;
        }
                                        // Call to the NavZip.dll's ZipInit()
                                        // function.
    pfnZipInit();

InitExit:

                                        // If an error occurred...
    if ( FALSE == bOK )
        {
                                        // Display an error loading the Zip
                                        // scanning engine.
        DisplayErrorMessage( NULL, IDS_ERROR_LOADING_NAV_ZIP );
        }
#endif  // _M_ALPHA
}


/////////////////////////////////////////////////////////////////////
// void NavZipProcessFile( void )
//
// Purpose:
//  This function will process a Zip file for NAV.  It will simply
//  be a callthrough for our ZipEngine, but will take care of the
//  case of a bad .Zip file where we have to shut down the engine
//  and reload it.
//
// Parameters:
//    UNZIP_STRUCT    Zip - [IN][OUT] Unzip structure.  We will pass
//                          this struct directly to the NavZip
//                          engine.
//
// Returns:
//  None.
//
// See Also:
//
/////////////////////////////////////////////////////////////////////
// 08-08-97 - KSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
void NavZipProcessFile(
    UNZIP_STRUCT    *lpZip
    )
{
#ifndef _M_ALPHA
    try
        {
                                        // If we have a function address for
                                        // this function...
        if ( NULL != pfnZipProcessFile )
            {
                                        // Call into the Zip scanning engine.
            (NAVZIPPROCESSFILE)(pfnZipProcessFile) (lpZip);
            }
        }
    catch( ... )
        {
                                        // -----------------------------------
                                        // We have experienced an unhandled
                                        // exception.  We should consider the
                                        // Zip scanning DLL to be unstable at
                                        // this point, so we need to reload the
                                        // DLL.
                                        // -----------------------------------

                                        // Call cleanup code, unloading the DLL.
        NavZipCleanup();

                                        // Reinitialize the DLL.  This will
                                        // reload it, and call the Init
                                        // function.
        NavZipInit();
        }

#endif  // _M_ALPHA
}

/////////////////////////////////////////////////////////////////////
// void NavZipCleanup( void )
//
// Purpose:
//  This function will cleanup for our Zip Engine.  This will free
//  the library that we have loaded, and do any other aditional
//  cleanup tasks.
//
// Parameters:
//  None.
//
// Returns:
//  None.
//
// See Also:
//
/////////////////////////////////////////////////////////////////////
// 08-08-97 - KSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
void NavZipCleanup( void )
{
#ifndef _M_ALPHA
                                        // If we have a function pointer...
    if ( NULL != pfnZipCleanup )
        {
                                        // Call through to the DLL Cleanup code.
        pfnZipCleanup();
        }
                                        // Free our zip library.
    if ( NULL != hZipInstance )
        {
        FreeLibrary( hZipInstance );
        hZipInstance = NULL;
        }
#endif  // _M_ALPHA
}

/////////////////////////////////////////////////////////////////////
// DisplayErrorMessage
//
// Purpose:
//    Displays a custom error message using a string ID for the
//    message text, as well as for the message box caption.  This
//    function was designed to make it easier to add custom messages
//    without having to format your own strings and load the string
//    resources.
//
// Parameters:
//    HWND  hParent               - [IN] Owner of this message box.
//    DWORD dwMessageID           - [IN] Message ID for string resouce.
//    DWORD dwMessageBoxCaptionID - [IN] Message Caption ID for string
//                                       resource (IDS_STD_ERROR_MESSAGE_CAPTION
//                                       is default).
//
// Returns:
//    None.
//
// See Also:
//
/////////////////////////////////////////////////////////////////////
// 11/05/96 - KSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
void DisplayErrorMessage(
    HWND  hParent,
    DWORD dwMessageID,
    DWORD dwMessageBoxCaptionID //= IDS_STD_ERROR_MESSAGE_CAPTION
    )
{
TCHAR       szMessageCaption[60];
TCHAR       szMessageText[SYM_MAX_PATH];

                                        // Load string for the Message Caption.
    if ( LoadString( hInstance, dwMessageBoxCaptionID, szMessageCaption, 60 ) )
        {
                                        // Load string for the Message Text.
        if ( LoadString( hInstance, dwMessageID, szMessageText, SYM_MAX_PATH ) )
            {
                                        // Display the custom message box.
            MessageBox( hParent, szMessageText, szMessageCaption, MB_OK | MB_ICONEXCLAMATION );
            }
        else
            {
                                        // Assert if we could not load
                                        // the message box string.
            SYM_ASSERT( 0 );
            }
        }
    else
        {
                                        // Assert if we could not load
                                        // the message box string.
        SYM_ASSERT( 0 );
        }
}
