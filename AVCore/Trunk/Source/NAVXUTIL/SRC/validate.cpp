// Copyright 1996 Symantec, Peter Norton Product Group
//***************************************************************************
//
// $Header:   S:/navxutil/VCS/validate.cpv   1.0   06 Feb 1997 21:05:04   RFULLER  $
//
// Description:
//      I just put the header in.  Someone else fill in the blanks.
//
// Contains:
//
//***************************************************************************
// $Log:   S:/navxutil/VCS/validate.cpv  $
// 
//    Rev 1.0   06 Feb 1997 21:05:04   RFULLER
// Initial revision
// 
//    Rev 1.2   06 May 1996 22:17:58   RSTANEV
// Merged Ron's Unicode changes.
//
//************************************************************************

#include "platform.h"
#include "xapi.h"
#include "navutil.h"

#include "dbcs.h"

// --------------------------------------------------------------------------
//                          LOCAL FUNCTION PROTOTYPES
// --------------------------------------------------------------------------

BOOL LOCAL Check8dot3 (                 // Make sure file is no longer than
    LPCTSTR  lpItem                      // 8.3
    );

// --------------------------------------------------------------------------

/*@API:*************************************************************************
@Declaration:

BOOL SYM_EXPORT WINAPI IsValidFilename (
    LPCSTR  lpItem,
    BOOL    bItemIsLFN
    );

@Description:
This function validates an entry. You can also use the defined macros
IsValidSFN() and IsValidLFN(). Function was lifted and modified from the
exclude code.

@Parameters:
$lpItem$ exclusion item to validate.
$bItemIsLFN$ TRUE if the item is supposed to be a valid LFN entry, FALSE for
SFN entries.

@Returns:
TRUE if filename is valid, FALSE otherwise

@Compatibility: Win16, Win32, DOS, VXD
*******************************************************************************/

BOOL SYM_EXPORT WINAPI IsValidFilename ( // Validates a filename
    LPTSTR  lpItem,                     // [in] item to validate
    BOOL    bItemIsLFN                  // [in] TRUE if item an LFN
    )
{
    SYM_ASSERT ( lpItem );

    auto    BOOL    bValid = TRUE;

    if ( lpItem )
        {
        UINT    uStep = 1;

        while ( uStep && bValid == TRUE )
            {
            switch ( uStep )
                {
                case 1:
                    {
                                        // ----------------------------------
                                        // Check for invalid characters. Note
                                        // that this differs for LFN and SFN.
                                        // ----------------------------------

                    TCHAR   szIllegalChars [ SYM_MAX_PATH ] = { 0 };
                    WORD    wNumChars;
                    WORD    wFlags;
                    BOOL    bFirstEncounter = TRUE;


                    if ( bItemIsLFN == TRUE )
                        {
                        wFlags = SYM_ILLEGAL_WIN95 | SYM_ILLEGAL_WILDCARDCHARS;
                        }
                    else
                        {
                        wFlags = SYM_ILLEGAL_DOS | SYM_ILLEGAL_SFN_LOWERCASE |
                                 SYM_ILLEGAL_WILDCARDCHARS;
                        }

                    wNumChars = NavReturnIllegalChars (
                                            wFlags,
                                            NULL,
                                            szIllegalChars,
                                            sizeof(szIllegalChars)
                                            );

                    for ( WORD wIndex = 1; wIndex < wNumChars; wIndex++ )
                        {
                        LPTSTR   lpBad   =   NULL;

                                        // ----------------------------------
                                        // Do no check for NULL.
                                        // ----------------------------------

                        if ( szIllegalChars[wIndex] )
                            lpBad = SYMstrchr ( lpItem, szIllegalChars[wIndex] );

                        if ( lpBad )
                            {
                                        // ----------------------------------
                                        // Spare the extension separator in
                                        // a short filename
                                        // ----------------------------------

                            if ( bItemIsLFN == FALSE &&
                                 *lpBad == '.'       &&
                                 bFirstEncounter == TRUE )
                                {
                                bFirstEncounter = FALSE;
                                continue;
                                }
                            else
                                {
                                bValid = FALSE;
                                break;
                                }
                            }
                        }
                    }

                    uStep++;
                    break;

                case 2:
                    if ( NameIsDevice ( lpItem ) )
                        {
                        bValid = FALSE;
                        }

                    uStep++;
                    break;

                case 3:
                    {
                                        // ----------------------------------
                                        // Check for UNC. Note that we should
                                        // work fine with UNC paths.
                                        // ----------------------------------

                    if ( lpItem[0] == '\\' && lpItem[1] == '\\' )
                        bValid = TRUE;
                    else if ( lpItem[1] == ':' )
                        {
                                        // ----------------------------------
                                        // Note that we can't use
                                        // NameValidatePath() since this item
                                        // could contain wildcards and other
                                        // funky formats.
                                        // ----------------------------------

                        if ( CharIsAlpha(lpItem[0]) == FALSE || lpItem[2] != '\\' )
                            {
                            bValid = FALSE;
                            }
                        }
                    }

                    uStep++;
                    break;

                case 4:
                    {
                    const   UINT    MAX_DOS_PATH = 144;

                                        // ----------------------------------
                                        // If item should be s short filename
                                        // ----------------------------------

                    if ( bItemIsLFN == FALSE )
                        {
                        WORD wCount = STRLEN(lpItem);

                                        // ----------------------------------
                                        // Check length of item.
                                        // ----------------------------------

                        if ( wCount > MAX_DOS_PATH )
                            {
                            bValid = FALSE;
                            }
                        else
                            {
                                        // ----------------------------------
                                        // Check for lower-case letters.
                                        // Not valid for Windows or VxD
                                        // due to international chars.
                                        // ----------------------------------
#if defined(SYM_DOS)
                            for ( WORD wIndex = 0; wIndex < wCount; wIndex++ )
                                {
                                if ( CharIsAlpha(lpItem[wIndex] ) == TRUE &&
                                     CharIsLower(lpItem[wIndex] ) == TRUE )
                                    {
                                    bValid = FALSE;
                                    break;
                                    }
                                }
#endif
                            }

                        if ( bValid == TRUE )
                            {
                                        // ----------------------------------
                                        // Make sure that there aren't more
                                        // than 8.3 between slashes.
                                        // ----------------------------------

                            bValid = Check8dot3 ( lpItem );
                            }
                        }
                    }
                    uStep = 0;
                    break;
#ifdef SYM_DEBUG
                default:
                    SYM_ASSERT ( 0 );
                    break;
#endif
                } // Switch()
            } // While ()
        } // If ()

    return ( bValid );
}


//************************************************************************
// IsUNCPath()
//
// This routine checks if the provided path appears to be a valid UNC path.
//
// Parameters:
//      lpPath  [in] path to verify
//
// Returns:
//      TRUE if the path appears to be valid, FALSE otherwise
//
//************************************************************************
// 8/25/94  MARTIN, function created
//************************************************************************

BOOL SYM_EXPORT WINAPI IsUNCPath (      // Checks path to be valid UNC path
    LPTSTR   lpPath                      // [in] path to check
    )
{
    SYM_ASSERT ( lpPath );

    auto    BOOL    bValid = TRUE;
    auto    TCHAR    szPath [ SYM_MAX_PATH + 1 ] = { 0 };

                                        // ----------------------------------
                                        // This is to preserve the case.
                                        // ----------------------------------
    STRCPY ( szPath, lpPath );
    STRUPR ( szPath );

                                        // ----------------------------------
                                        // Stay in loop while there are stuff
                                        // to be verified and the path still
                                        // looks valid.
                                        // ----------------------------------
    UINT uVerifyStep = 1;

    while ( bValid == TRUE && uVerifyStep )
        {
        switch ( uVerifyStep )
            {
            case 1:
                                        // ----------------------------------
                                        // Path must be larger than 2 chars
                                        // ----------------------------------
                if ( STRLEN ( szPath ) > 2 )
                    uVerifyStep++;
                else
                    bValid = FALSE;
                break;

            case 2:
                                        // ----------------------------------
                                        // Check for invalid characters.
                                        // ----------------------------------
                {
                TCHAR   szIllegalChars [ SYM_MAX_PATH ] = { 0 };

                                        // ----------------------------------
                                        // SYM_ILLEGAL_USEVOLUME,
                                        // ----------------------------------
                                        // This was causing problems when
                                        // attached to other computers through
                                        // peer to peer comm. and the area
                                        // attached contained spaces an other
                                        // funky chars.
                                        // ----------------------------------

                WORD wNumChars = NavReturnIllegalChars (
                                        SYM_ILLEGAL_WIN95,
                                        lpPath,
                                        szIllegalChars,
                                        sizeof(szIllegalChars)
                                        );

                for ( WORD wIndex = 0; wIndex < wNumChars; wIndex++ )
                    {
                                        // ----------------------------------
                                        // Skip over NULL character.
                                        // ----------------------------------

                    if ( szIllegalChars[wIndex] &&
                         SYMstrchr ( szPath, szIllegalChars[wIndex] ) )
                        {
                        bValid = FALSE;
                        break;
                        }
                    }

                if ( bValid == TRUE )
                    uVerifyStep++;
                }
                break;

            case 3:
                {
                if ( szPath[0] == '\\' && szPath[1] == '\\' )
                    uVerifyStep++;
                else
                    bValid = FALSE;
                }
                break;

            case 4:
                {
                LPTSTR lpWild = SYMstrchr ( szPath, '*' );

                if ( lpWild )
                    {
                    while ( *lpWild && bValid == TRUE )
                        {
                        if ( *lpWild == '\\' )
                            bValid = FALSE;

                        lpWild = AnsiNext ( lpWild );
                        }
                    }

                if ( bValid == TRUE )
                    {
                    lpWild = SYMstrchr ( szPath, '?' );

                    if ( lpWild )
                        {
                        while ( *lpWild && bValid == TRUE )
                            {
                            if ( *lpWild == '\\' )
                                bValid = FALSE;

                            lpWild = AnsiNext ( lpWild );
                            }
                        }
                    }

                uVerifyStep = 0;
                }
                break;

            default:
                SYM_ASSERT ( 0 );
                break;
            }
        }

    return ( bValid );
}

// --------------------------------------------------------------------------
// --------------------------------------------------------------------------

UINT SYM_EXPORT WINAPI NavReturnIllegalChars ( // Get illegal characters
    WORD        wOsFlag,                // [in] use any SYM_ILLEGAL_xxx
    LPCTSTR     lpVolume,               // [in] pointer to volume, for LFN
    LPTSTR      lpIllegals,             // [out] illegal chars placed here
    UINT        uSizeIllegals           // [in] max size of lpIllegals
    )
{

    UINT uNumChars = NameReturnIllegalChars (
                                        wOsFlag,
                                        lpVolume,
                                        lpIllegals,
                                        uSizeIllegals
                                        );

    if ( lpIllegals && uSizeIllegals )
        {
                                        // -----------------------------------
                                        // Period is valid character for SFN
                                        // directories. This removes the
                                        // period if it's part of the buffer.
                                        // -----------------------------------

        for ( UINT uIndex = 0; uIndex < uNumChars; uIndex++ )
            {
            if ( lpIllegals[uIndex] == '.' ||
                 lpIllegals[uIndex] == EOS )
                {
                MEMMOVE ( &lpIllegals[uIndex], &lpIllegals[uIndex+1],
                          (uNumChars - uIndex) * sizeof(TCHAR) );

                uNumChars --;
                }
            }
        }

    return ( uNumChars );
}

// --------------------------------------------------------------------------
//                          LOCAL FUNCTION PROTOTYPES
// --------------------------------------------------------------------------

BOOL LOCAL Check8dot3 (                 // Make sure file is no longer than
    LPCTSTR  lpItem                      // 8.3
    )
{
    const   UINT    MY_MAX_FILE =   8;
    const   UINT    MY_MAX_EXT  =   3;
    auto    BOOL    bValid      =   TRUE;
    auto    UINT    uCount      =   MY_MAX_FILE;

    for ( UINT uIndex = 0; lpItem[uIndex]; uIndex++ )
        {
        if ( lpItem[uIndex] == '\\' )
            {
            if (!DBCSInstalled())
                uCount = MY_MAX_FILE;
            else
                {
                if ( 0 == uIndex || !(DBCSIsLeadByte (lpItem[uIndex - 1]) ) )
                    uCount = MY_MAX_FILE;
                }
            }
        else if ( lpItem[uIndex] == '.' )
            {
            uCount = MY_MAX_EXT;
            }
        else if ( uCount == 0 )
            {
            bValid = FALSE;
            break;
            }
        else
            {
            uCount--;
            }
        }

    return ( bValid );
}

