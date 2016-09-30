// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright 2003, 2005 Symantec Corporation. All rights reserved.
//***************************************************************************
// ModifyMail.cpp:
//
// Implementation of the CIMailStorage notifications
//      Insertion of warning messages
//*************************************************************************

#include "stdafx.h"
#include <new>
#include <mlang.h>
#include <assert.h>

#include "vpcommon.h"
#include "clientreg.h"
#include "SymSaferStrings.h"

#include "MailComm.h"
#include "IMailStorage.h"
#include "ModifyMail.h"

#include "decinterface.h"

#include "MessageEventSink.h"
#include <exception>
#include "asc_char.h"

static const TCHAR s_szFieldSubject[] = _T("Subject:");

static int WideCharToWindowsChar ( IMLangConvertCharset *pConvert,
                                   UINT         uCodePage,
                                   LPCWSTR      pszWide,
                                   LPSTR        pszWindows,
                                   int          cchWindows );

static int WindowsCharToWideChar ( IMLangConvertCharset *pConvert,
                                   UINT         uCodePage,
                                   LPCSTR       pszWindows,
                                   LPWSTR       pszWide,
                                   int          cchWide );

static void MIMEEncode64TriToQuad( LPBYTE lpbyTri, LPSTR lpszQuad );
static DWORD EncodeBase64Buffer (char *pszInput, DWORD dwSize, char *pszOutput, DWORD dwOutputBufferSize);


static bool GetTempFileDir (TCHAR *szTruncDir, DWORD nNumTruncDirChars, const TCHAR *szFullDir)
{
    _tcsncpy( szTruncDir, szFullDir, nNumTruncDirChars );
    szTruncDir [ nNumTruncDirChars - 1 ] = '\0';

    char *pchTempDirEnd = _tcsrchr( szTruncDir, '\\' );

    if ( NULL == pchTempDirEnd )
    {
        return false;
    }

    *pchTempDirEnd = '\0';
    return true;
}

//*************************************************************************
// InsertMsgUsingDecomposer()
//
// static WORD InsertMsgUsingDecomposer ( LPCTSTR lpszMessageImage, LPCTSTR lpszWarning, LPCTSTR lpszNewSubject )
//
// Parameters:
//      lpszMessageImage            Full path to file with image of message to modify.
//      lpszWarning                 Warning message to insert in text sections of message.
//
// Return Values:
//
// Remarks:
//
//*************************************************************************
// 2003.08.29 DALLEE - created.
//*************************************************************************

static WORD InsertMsgUsingDecomposer ( LPCTSTR lpszMessageImage, LPCTSTR lpszWarning )
{
    auto    CMessageEventSink   mySink;

    auto    char        szTempDir [ MAX_PATH ];

    auto    char        szNewDataFile [ MAX_PATH ];
    auto    WORD        wResult;

    // Piggybacking on email proxy's choice of temp directories.

    if (false == GetTempFileDir (szTempDir,
                                 sizeof( szTempDir ) / sizeof( *szTempDir ),
                                 lpszMessageImage))
    {
        return (WORD)DAPI_ERROR;
    }

    // Get pre-allocated shared decomposer

 	//create an instance of the cDecObject to 
	//replace calls that used to be in decomposer
	CDecInterface cDecObj;
	if( cDecObj.CheckFileSystem() == false ){
		return (WORD)DAPI_ERROR;
	}

    mySink.SetDecObj( &cDecObj );
    mySink.SetWarningMsg( lpszWarning );

	IDecContainerObjectEx *pIDecObject;
    if ( DAPI_OK == cDecObj.NewObject( lpszMessageImage, lpszMessageImage, szTempDir, &pIDecObject) )
    {
        if ( cDecObj.GetDecomposer() != NULL && 
            DAPI_OK == cDecObj.GetDecomposer()->Process(pIDecObject, 
			            							    &mySink,
											            cDecObj.GetDecIO(),
											            &wResult, 
											            szNewDataFile) )
        {
			if ( DECRESULT_TO_BE_REPLACED == wResult )
            {
                mySink.myCopyFile( szNewDataFile, lpszMessageImage, FALSE );
            }
        }
    }
    cDecObj.DeleteObject(pIDecObject);

    return DAPI_OK;
}

//*************************************************************************
// InsertWarningMessage()
//
// Insert warning message and alter subject of infected emails.
//
// DWORD InsertWarningMessage( LPCTSTR lpszMessageImage, LPCTSTR lpszWarning, LPCTSTR lpszNewSubject )
//
// Parameters:
//      lpszMessageImage            Full path to file with image of message to modify.
//      lpszWarning                 Warning message to insert in text sections of message.
//      lpszNewSubject              New message subject ( value only, does not include "Subject:" field name.
//                                  May be null if no subject modification is desired.
//
// Return Values:
//      false on error,
//      non-zero for success.
// Remarks:
//
//*************************************************************************
// 2003.08.27 DALLEE - created.
//*************************************************************************

DWORD InsertWarningMessage( LPCTSTR lpszMessageImage, LPCTSTR lpszWarning, LPCTSTR lpszNewSubject )
{
    auto    DWORD   dwSubjectStart;
    auto    DWORD   dwSubjectEnd;
    auto    DWORD   dwHeaderEnd;

    auto    MAIL_MODIFICATION_LIST  rMod;

    auto    TCHAR   szTempFile [ MAX_PATH ];
    auto    TCHAR   szTempDir [ MAX_PATH ];

    auto    char    szEncodedSubject [ MAX_RFC821_LINE ];
    auto    char    szNewSubjectField [ MAX_RFC821_LINE ];    // Complete subject field.

    auto    bool    bCreatedTempFile;
    auto    DWORD   dwResult;

    bCreatedTempFile = false;
    dwResult = (DWORD)-1;

    //*******************************************
    // Warning message insertion

    if ( DAPI_OK != InsertMsgUsingDecomposer ( lpszMessageImage, lpszWarning ) )
    {
        goto CleanUp;
    }

    //*******************************************
    // Subject modifications

    // Attempt to retrieve replacement information

    if ( NULL == lpszNewSubject )
    {
        dwResult = ERROR_SUCCESS;
        goto CleanUp;
    }

    if ( ERROR_SUCCESS != GetMailModificationInfo( lpszMessageImage,
                                                   &dwSubjectStart,
                                                   &dwSubjectEnd,
                                                   &dwHeaderEnd ) )
    {
        goto CleanUp;
    }



    EncodeSubject( szEncodedSubject, lpszNewSubject, sizeof( szEncodedSubject ) );

    _tcscpy( szNewSubjectField, s_szFieldSubject );
    _tcscat( szNewSubjectField, _T(" ") );
    _tcsncat( szNewSubjectField, szEncodedSubject, sizeof( szNewSubjectField ) - _tcslen( szNewSubjectField ) - 2 ); // - 2 for "\n\000"
    _tcscat( szNewSubjectField, _T("\n") );

    memset( &rMod, 0, sizeof( rMod ) );
    rMod.dwStart = dwSubjectStart;
    rMod.dwEnd   = dwSubjectEnd;
    rMod.nAction = MODIFICATION_REPLACE;
    rMod.lpszNewData = szNewSubjectField;

    // Get temporary file to work modifications from...

    if (false == GetTempFileDir (szTempDir,
                                 sizeof( szTempDir ) / sizeof( *szTempDir ),
                                 lpszMessageImage))
    {
        goto CleanUp;
    }

    if ( 0 == GetTempFileName( szTempDir, "sav", 0, szTempFile ) )
    {
        goto CleanUp;
    }

    bCreatedTempFile = true;

    // Make message modifications

    if ( ERROR_SUCCESS != ModifyMail( szTempFile, lpszMessageImage, &rMod ) )
    {
        goto CleanUp;
    }

    if ( CopyFile( szTempFile, lpszMessageImage, FALSE ) )
    {
        dwResult = ERROR_SUCCESS;
    }

CleanUp:
    if ( bCreatedTempFile )
    {
        DeleteFile( szTempFile );
    }

    return ( dwResult );
} // InsertWarningMessage()


//*************************************************************************
// GetMailModificationInfo()
//
// Finds the location and characteristics of the given message items.
//
// Start and End positions are give in 1-based line numbers. A position of 0
// indicates the item was not found
//
// DWORD GetMailModificationInfo( LPCTSTR  szFileName,
//                                LPDWORD  pdwSubjectStart,
//                                LPDWORD  pdwSubjectEnd,
//                                LPDWORD  pdwHeaderEnd )
//
// Parameters:
//      szFileName                  [in] File containing email message
//      pdwSubjectStart             [out] First line of subject field
//      pdwSubjectEnd               [out] Last line of subject field
//      pdwHeaderEnd                [out] Last line of header (before empty separator line)
//
// Return Values:
//      ERROR_SUCCESS on success, otherwise non-zero.
//
// Remarks:
//
//*************************************************************************
// 2003.08.27 DALLEE - created.
//*************************************************************************

DWORD GetMailModificationInfo( LPCTSTR  szFileName,
                               LPDWORD  pdwSubjectStart,
                               LPDWORD  pdwSubjectEnd,
                               LPDWORD  pdwHeaderEnd )
{
    auto    FILE    *pFile = NULL;
    auto    DWORD   dwCurrentLine;

    auto    DWORD   dwResult = ERROR_SUCCESS;

    auto    TCHAR   szBuffer[ MAX_RFC821_LINE ];
    auto    TCHAR   szHeader[ MAX_RFC821_LINE * 2 ]; // Arbitrary size choice. Obscenely large for a MIME field.

    auto    enum    tagHEADER_FIELD {
        FIELD_UNKNOWN = 0,
        FIELD_SUBJECT
    } nCurrentField;

    *pdwSubjectStart = 0;
    *pdwSubjectEnd   = 0;
    *pdwHeaderEnd    = 0;

    *szHeader = _T('\0');

    nCurrentField = FIELD_UNKNOWN;
    dwCurrentLine = 0;

    // Open the file.

    pFile = _tfopen( szFileName, _T("r") );

    if ( NULL == pFile )
    {
        dwResult = 1; //&? open error
        goto CleanUp;
    }

    // Go through the header fields and pick out the info we want.
    // MIME headers may be split across multiple lines. Lines that begin with
    // a space are treated as a continuation of the previous line.

    while ( _fgetts( szBuffer, sizeof( szBuffer ) / sizeof( *szBuffer ), pFile ) )
    {
        // Are we done with headers?

        if ( ( 0 == _tcscmp( szBuffer, "" ) )   ||
             ( 0 == _tcscmp( szBuffer, "\r" ) ) ||
             ( 0 == _tcscmp( szBuffer, "\n" ) ) ||
             ( 0 == _tcscmp( szBuffer, "\r\n" ) ) )
        {
            break;
        }

        // Increment line counter each time.
        // Not quite legitimate, since reading MAX_RFC821_LINE characters is not guaranteed to
        // pull a full line, however we'll be using the same bogus counting scheme
        // when performing replacements...

        ++dwCurrentLine;

        // If the line starts with a whitespace, it's a continuation of the current header.

        if ( ( _T(' ') == szBuffer[0] ) || ( _T('\t') == szBuffer[0] ) )
        {
            // Add it to the current header.
            // Not too concerned if I chop the ends off of very long header fields --
            // the fields I'm interested in are very unlikey to grow long.

            _tcsncat( szHeader, szBuffer, sizeof( szHeader ) / sizeof( *szHeader ) - _tcslen( szHeader ) - 1 );

            // We're done for now...go back and read another line.

            continue;
        }

        // Otherwise we have a new header field. Time to save the current header info.

        switch ( nCurrentField )
        {
        default:
        case FIELD_UNKNOWN:
            break;

        case FIELD_SUBJECT:
            *pdwSubjectEnd = dwCurrentLine - 1;
            break;
        }

        // Determine what new header field is.
        // Field name must be entirely contained in first line of field,
        // so this check vs. szBuffer is legitimate.

        if ( 0 == _tcsnicmp( szBuffer, s_szFieldSubject, _tcslen( s_szFieldSubject ) ) )
        {
            *pdwSubjectStart = dwCurrentLine;
            nCurrentField = FIELD_SUBJECT;
        }
        else
        {
            nCurrentField = FIELD_UNKNOWN;
        }

        // Make the new header the current header.

        _tcscpy( szHeader, szBuffer );
    }

    // Verify that we quit because we successfully reached the end of input...

    if ( 0 != ferror( pFile ) )
    {
        dwResult = 3;  //&? Input read error
        goto CleanUp;
    }

    // Update any relevant data upon reading last header line

    switch ( nCurrentField )
    {
    default:
    case FIELD_UNKNOWN:
        break;

    case FIELD_SUBJECT:
        *pdwSubjectEnd = dwCurrentLine;     // Note that dwCurrentLine hasn't advanced so no -1...
        break;
    }

    *pdwHeaderEnd = dwCurrentLine;

CleanUp:
    // Close the file.

    if ( NULL != pFile )
    {
        fclose(pFile);
    }

    return ( dwResult );
} // GetMailModificationInfo()


//*************************************************************************
// ModifyMail()
//
// Copies mail message from szOldFile to szNewFile, making requested modifications.
//
// DWORD ModifyMail( LPCTSTR szToFile,
//                   LPCTSTR szFromFile,
//                   PMAIL_MODIFICATION_LIST pModifications )
//
// Parameters:
//      szToFile
//      szFromFile
//      pModifications
//
// Return Values:
//      ERROR_SUCCESS on success, non-zero on error
//
// Remarks:
//
//*************************************************************************
// 2003.08.27 DALLEE - created.
//*************************************************************************

DWORD ModifyMail( LPCTSTR szToFile,
                  LPCTSTR szFromFile,
                  PMAIL_MODIFICATION_LIST pModifications )
{
    auto    FILE    *pFromFile = NULL;
    auto    FILE    *pToFile = NULL;

    auto    PMAIL_MODIFICATION_LIST pCurrentMod;

    auto    DWORD   dwCurrentLine;
    auto    bool    bDeleteLine;

    auto    TCHAR   szBuffer[ MAX_RFC821_LINE ];

    auto    DWORD   dwResult = ERROR_SUCCESS;

    dwCurrentLine = 0;

    // Open the file.

    pFromFile = _tfopen( szFromFile, _T("r") );

    if ( NULL == pFromFile )
    {
        dwResult = 1;  //&? open error
        goto CleanUp;
    }

    pToFile = _tfopen( szToFile, _T("w") );

    if ( NULL == pToFile )
    {
        dwResult = 1;  //&? open error
        goto CleanUp;
    }

    // Read line by line, performing requested modifications along the way...

    while ( ( ERROR_SUCCESS == dwResult ) &&
            ( NULL != _fgetts( szBuffer, sizeof( szBuffer ) / sizeof( *szBuffer ), pFromFile ) ) )
    {
        // Increment line counter each time.
        // Not quite legitimate, since reading MAX_RFC821_LINE characters is not guaranteed to
        // pull a full line, however this is the same counting scheme used to acquire
        // the replacement locations....

        ++dwCurrentLine;
        bDeleteLine = false;

        // Check for required modifications
        //&? Inefficient, to say the least...

        for ( pCurrentMod = pModifications; pCurrentMod; pCurrentMod = pCurrentMod->pNext )
        {
            switch ( pModifications->nAction )
            {
            case MODIFICATION_DELETE:
                if ( ( dwCurrentLine >= pModifications->dwStart ) &&
                     ( dwCurrentLine <= pModifications->dwEnd ) )
                {
                    bDeleteLine = true;
                }
                break;

            case MODIFICATION_REPLACE:
                if ( ( dwCurrentLine >= pModifications->dwStart ) &&
                     ( dwCurrentLine <= pModifications->dwEnd ) )
                {
                    bDeleteLine = true;

                    if ( dwCurrentLine == pModifications->dwStart )
                    {
                        if ( EOF == _fputts( pModifications->lpszNewData, pToFile ) )
                        {
                            dwResult = 2;  //&? write error
                        }
                    }
                }
                break;

            case MODIFICATION_INSERT:
                if ( dwCurrentLine == pModifications->dwStart )
                {
                    if ( EOF == _fputts( pModifications->lpszNewData, pToFile ) )
                    {
                        dwResult = 2;  //&? write error
                    }
                }
                break;

            default:
                break;
            }
        }

        // Copy current line
        
        if ( !bDeleteLine )
        {
            if ( EOF == _fputts( szBuffer, pToFile ) )
            {
                dwResult = 2;  //&? write error
            }
        }
    }

    // Verify that we quit because we successfully reached the end of input...

    if ( ERROR_SUCCESS == dwResult )
    {
        if ( 0 != ferror( pFromFile ) )
        {
            dwResult = 3;  //&? Input read error
        }
    }

CleanUp:
    // Close the file.

    if ( NULL != pFromFile )
    {
        fclose( pFromFile );
    }

    if ( NULL != pToFile )
    {
        if ( EOF == fclose( pToFile ) )
        {
            dwResult = 2; //&? Write error
        }
    }

    return ( dwResult );
} // ModifyMail()


//************************************************************************
// Character conversion and Base64 encoding
//************************************************************************

//*************************************************************************
// EncodeSubject()
//
// This routine takes a bare subject string and returns a formatted
// email header subject. If necessary, the data is transformed into a "B"
// type (base64) encoded word.
//
// void EncodeSubject( LPSTR pszNewField, LPCSTR pszSubject, int cchNewField )
//
// Parameters:
//      pszNewField             [out] New encoded subject
//      pszSubject              [in]  Unencoded bare subject
//      cchNewField             [in]  size of pszNewField in bytes
//
// Return Values:
//
// Remarks:
//      If encoded, the character set used will be the default email header encoding for 
//      the current ANSI codepage according to MLANG.
//      See the MLANG IMultiLanguage interface in MSDN.
//*************************************************************************
// 2004.02.02 DALLEE - created.
//*************************************************************************

void EncodeSubject( LPSTR pszNewField, LPCSTR pszSubject, int cchNewField )
{
    auto    IMultiLanguage          *pMultiLanguage;
    auto    IMLangConvertCharset    *pConvertCharset;

    auto    MIMECPINFO      codePageInfo;
    auto    MIMECSETINFO    mimeCsetInfo;
    auto    BSTR            bstrCharacterSet;
    auto    char            szCharacterSet[100];
    auto    bool            bEncodingRequired;
    auto    bool            bPerformedEncoding;

    auto    LPSTR           pszCheckInput;

    auto    LPSTR           pszSubjectNoCRLF;

    // Initialize locals

    pMultiLanguage      = NULL;
    pConvertCharset     = NULL;
    bstrCharacterSet    = NULL;
    pszSubjectNoCRLF    = NULL;
    szCharacterSet[0]   = '\0';
    bEncodingRequired   = false;
    bPerformedEncoding  = false;

    // Copy new subject to work buffer.

    pszSubjectNoCRLF = new char [_tcslen(pszSubject) + 1];

    if ( NULL == pszSubjectNoCRLF )
    {
        // Unable to allocate memory... default to copying the
        // subject over and hope for a mail server that accepts 8-bit data in the header fields.
        if ( cchNewField >= 1 )
        {
            pszNewField[0] = '\0';
            _tcsncat( pszNewField, pszSubject, cchNewField - 1 );
        }

        return;
    }

    _tcscpy( pszSubjectNoCRLF, pszSubject );

    // Replace CR and LF with spaces. CRLF signals an end to the subject line and following data
    // will be misinterpreted.

    for ( pszCheckInput = pszSubjectNoCRLF; *pszCheckInput; pszCheckInput = _tcsinc( pszCheckInput ) )
    {
        if ( ( '\r' == *pszCheckInput ) ||
             ( '\n' == *pszCheckInput ) )
        {
            *pszCheckInput = ' ';
        }
    }

    // Slight optimization - only perform encoding if we have 8-bit data in the subject line
    // Also nice to keep the header field human readable.
    // NOTE: Specifically incrementing pszCheckInput by bytes, not by characters -- this loop
    // is not performing character manipulation, it is merely checking for any 8-bit data.

    for ( pszCheckInput = pszSubjectNoCRLF; *pszCheckInput; ++pszCheckInput )
    {
        if ( 0x80 & *pszCheckInput )
        {
            bEncodingRequired = true;
            break;
        }
    }

    if ( bEncodingRequired )
    {
        CoCreateInstance( CLSID_CMultiLanguage,
                          NULL,
                          CLSCTX_INPROC_SERVER,
                          IID_IMultiLanguage,
                          (void**) &pMultiLanguage );

        if ( pMultiLanguage )
        {
            // Get codepage information -- Most importantly, email header character set name.
            if ( SUCCEEDED( pMultiLanguage->GetCodePageInfo( GetACP(), &codePageInfo ) ) )
            {
                // Get the preferred encoding for mail headers
                bstrCharacterSet = SysAllocString( codePageInfo.wszHeaderCharset );

                if ( bstrCharacterSet )
                {
                    WideCharToMultiByte( CP_ACP, 0, bstrCharacterSet, -1, szCharacterSet, sizeof( szCharacterSet ), NULL, NULL );

                    if ( SUCCEEDED ( pMultiLanguage->GetCharsetInfo( bstrCharacterSet, &mimeCsetInfo ) ) )
                    {
                        // Create conversion object to translate from UNICODE (1200) to the preferred mail header encoding
                        // for the default ANSI codepage.

                        pMultiLanguage->CreateConvertCharset( 1200, mimeCsetInfo.uiInternetEncoding, MLCONVCHARF_AUTODETECT, &pConvertCharset );

                        if ( pConvertCharset )
                        {
                            auto    LPSTR   pszHeaderEncoding = NULL;
                            auto    LPWSTR  pszWide = NULL;
                            auto    LPSTR   pszB64 = NULL;
                            auto    int     cchOutput;
                            auto    int     nResult;
							try
							{
								cchOutput = strlen( pszSubjectNoCRLF ) * 2 + 1;              // Allow for worst cast splitting all characters + 1 for EOS.
								pszWide           = new WCHAR [ cchOutput ];
								pszHeaderEncoding = new char [ cchOutput ];
								pszB64            = new char [ cchOutput * 4 / 3 + 2 ]; // Base64 encoding is a 33% expansion with up to 2 bytes padding of '=='

								if ( pszWide && pszHeaderEncoding && pszB64 )
								{
									// Using a 2-step conversion process since MLANG won't always convert from 8-bit to 8-bit character sets...

									nResult = MultiByteToWideChar( CP_ACP, 0, pszSubjectNoCRLF, -1, pszWide, cchOutput );

									if ( nResult )
									{
										nResult = WideCharToWindowsChar( pConvertCharset, mimeCsetInfo.uiInternetEncoding, pszWide, pszHeaderEncoding, cchOutput );

										if ( nResult )
										{
											EncodeBase64Buffer( pszHeaderEncoding, strlen( pszHeaderEncoding ), pszB64, cchOutput * 4 / 3 + 2 );

											if ( 0 > sssnprintf( pszNewField, cchNewField, "=?%s?B?%s?=", szCharacterSet, pszB64 ) )
											{
												// Out of space, null terminating and hope for the best.

												if ( cchNewField >= 1 )
												{
													pszNewField[ cchNewField - 1 ] = '\0';
												}
											}
											bPerformedEncoding = true;
										}
									}
								}
							}
							catch(std::exception e) {}

                            // Deletes on NULL ok.
                            delete [] pszB64;
                            delete [] pszHeaderEncoding;
                            delete [] pszWide;

                            pConvertCharset->Release();
                        }
                    }

                    SysFreeString( bstrCharacterSet );
                }
            }

            pMultiLanguage->Release();
        }
    }

    // If for any reason, we didn't perform the encoding, default to copying the
    // subject over and hope for a mail server that accepts 8-bit data in the header fields.
    if ( false == bPerformedEncoding )
    {
        if ( cchNewField >= 1 )
        {
            pszNewField[0] = '\0';
            _tcsncat( pszNewField, pszSubjectNoCRLF, cchNewField - 1 );
        }
    }

    if ( NULL != pszSubjectNoCRLF )
    {
        delete [] pszSubjectNoCRLF;
    }
} // EncodeSubject()

//*************************************************************************
// ConvertWindowsString()
//
// Translate a string from one Windows codepage to another.
//
// int ConvertWindowsString ( UINT         uSrcCodePage,
//                            UINT         uDstCodePage,
//                            LPCSTR       pszSrc,
//                            LPSTR        pszDst,
//                            int          cchDst )
//
// Parameters:
//      uSrcCodePage            [in] Source string Windows Ansi codepage
//      uDstCodePage            [in] Desired destination Windows Ansi codepage
//      pszSrc                  [in] Null terminated source string
//      pszDst                  [out] Output buffer
//      cchDst                  [in] Size of output buffer in bytes
//
// Return Values:
//      
//      If the function succeeds, the return value is the number of characters
//      written to the buffer pointed to by pszDst. 
//
//      If the function fails, the return value is zero. To get extended error
//      information, call GetLastError. GetLastError may return one of the
//      following error codes:
//
//          ERROR_INSUFFICIENT_BUFFER
//          ERROR_INVALID_PARAMETER
//          ERROR_NOT_ENOUGH_MEMORY
//*************************************************************************
// 2004.02.24 DALLEE - created.
//*************************************************************************

int ConvertWindowsString ( UINT         uSrcCodePage,
                           UINT         uDstCodePage,
                           LPCSTR       pszSrc,
                           LPSTR        pszDst,
                           int          cchDst )
{
    auto    IMLangConvertCharset    *pConvertCharset;

    auto    LPWSTR      pwszWide;                
    auto    int         cchWide;

    auto    int         nResult;

    pConvertCharset = NULL;
    pwszWide        = NULL;
    nResult         = 0;

    // Allocate intermediate wide character buffer.
    // Allowing for worst case expansion -- all characters translate to composite characters.
    // Limiting factor should now be only the the destination buffer size.

    cchWide = 2 * _tcslen( pszSrc ) + 1;

	try
	{
		pwszWide = NULL;
		pwszWide = new WCHAR [ cchWide ];
	}
	catch (std::bad_alloc &){}

    if ( NULL == pwszWide )
    {
        SetLastError( ERROR_NOT_ENOUGH_MEMORY );
        goto BailOut;
    }

    // Attempt to retrive MLANG character conversion interface.

    CoCreateInstance( CLSID_CMLangConvertCharset,
                      NULL,
                      CLSCTX_INPROC_SERVER,
                      IID_IMLangConvertCharset,
                      (void**) &pConvertCharset );
    // Okay if this fails -- conversion functions used here will default to
    // old-style MultiByteToWideChar() functions using NLS data if this interface
    // pointer is NULL.

    // Convert to Windows 2-byte Unicode

    nResult = WindowsCharToWideChar( pConvertCharset,
                                     uSrcCodePage,
                                     pszSrc,
                                     pwszWide,
                                     cchWide );

    if ( nResult )
    {
        // 2nd half of conversion: 2-byte Unicode to ANSI

        nResult = WideCharToWindowsChar( pConvertCharset,
                                         uDstCodePage,
                                         pwszWide,
                                         pszDst,
                                         cchDst );
    }

BailOut:
    if ( NULL != pConvertCharset )
        pConvertCharset->Release();

    if ( NULL != pwszWide )
        delete [] pwszWide;

    return ( nResult );
} // ConvertWindowsString()


//*************************************************************************
// WindowsCharToWideChar()
//
// Translate a string from the specified Windows codepage to a wide character
// string (Windows 2-byte UNICODE).
//
// static int WindowsCharToWideChar ( IMLangConvertCharset *pConvert,
//                                    UINT         uCodePage,
//                                    LPCSTR       pszWindows,
//                                    LPWSTR       pszWide,
//                                    int          cchWide )
//
// Parameters:
//      pConvert            MLang character conversion interface. May be NULL, if this
//                              interface is not available (requires IE 4.0+). Will
//                              default to using old NLS through MultiByteToWideChar()
//      uCodePage           Windows code page ID for pszWindows
//      pszWindows          Windows input string
//      pszWide             Wide character output string
//      cchWide             Size of output buffer in characters
//
// Return Values:
//      
//      If the function succeeds, the return value is the number of characters
//      written to the buffer pointed to by pszWide. 
//
//      If the function fails, the return value is zero. To get extended error
//      information, call GetLastError. GetLastError may return one of the
//      following error codes:
//
//          ERROR_INSUFFICIENT_BUFFER
//          ERROR_INVALID_PARAMETER
//          ERROR_NOT_ENOUGH_MEMORY
//*************************************************************************
// 2003.12.10 DALLEE - created.
//*************************************************************************

static int WindowsCharToWideChar ( IMLangConvertCharset *pConvert,
                                   UINT         uCodePage,
                                   LPCSTR       pszWindows,
                                   LPWSTR       pszWide,
                                   int          cchWide )
{
    auto    UINT        uInputSize;
    auto    UINT        uOutputSize;

    auto    HRESULT     hResult;
    auto    int         nResult;

    nResult = 0;

    //
    // Convert to Windows 2-byte Unicode
    //

    if ( NULL == pConvert )
    {
        // With no MLANG interface, default to using old-style MultiByteToWideChar()
        // This may not have all the code pages available to MLANG, but does a fair job.

        nResult = MultiByteToWideChar( uCodePage, 0, pszWindows, -1, pszWide, cchWide );
    }
    else
    {
        // Use MLANG

        // Attempt to set up conversion.
        //
        // Note that the result from Initialize() is not 100% reliable --
        // The first attempt to Initialize to codepages which are not available will correctly return
        // S_FALSE. A second attempt to intialize to the same codepages (legal according to MSDN's docs)
        // will return a bogus S_OK.

        if ( CP_ACP == uCodePage )
        {
            uCodePage = GetACP();
        }
        else if ( CP_OEMCP == uCodePage )
        {
            uCodePage = GetOEMCP();
        }

        hResult = pConvert->Initialize( uCodePage, 1200, MLCONVCHARF_AUTODETECT );

        if ( S_OK == hResult )
        {
            uInputSize = strlen( pszWindows ) + 1;      // +1 to force inclusion of EOS in conversion
            uOutputSize = cchWide;                      // Size in number of (wide) characters

            hResult = pConvert->DoConversionToUnicode( const_cast<LPSTR>(pszWindows), &uInputSize, pszWide, &uOutputSize );

            // DoConversionToUnicode() fails in more ways than are listed in MSDN's help...
            // S_FALSE for an unsupported conversion, but also in some cases uInputSize is reset to 0,
            // or uOutputSize may be set to 0.

            if ( ( S_OK == hResult ) && uInputSize && uOutputSize )
            {
                // Success

                nResult = wcslen( pszWide );
            }
            else
            {
                SetLastError( ERROR_INVALID_PARAMETER );
            }
        }
        else
        {
            if (FAILED(hResult))
            {
                SetLastError( ERROR_NOT_ENOUGH_MEMORY );
            }
            else
            {
                // This is S_FALSE for unsupported conversion...
                SetLastError( ERROR_INVALID_PARAMETER );
            }
        }
    }

    return ( nResult );
} // WindowsCharToWideChar()


//*************************************************************************
// WideCharToWindowsChar()
//
// Translate a string from Windows 2-byte UNICODE to the specified Windows codepage.
//
// static int WideCharToWindowsChar ( IMLangConvertCharset *pConvert,
//                                    UINT         uCodePage,
//                                    LPCWSTR      pszWide,
//                                    LPSTR        pszWindows,
//                                    int          cchWindows )
//
// Parameters:
//      pConvert            MLang character conversion interface. May be NULL, if this
//                              interface is not available (requires IE 4.0+). Will
//                              default to using old NLS through MultiByteToWideChar()
//      uCodePage           Windows code page ID for pszWindows
//      pszWide             Wide character input string
//      pszWindows          Windows output string
//      cchWindows          Size of output buffer in characters
//
// Return Values:
//      
//      If the function succeeds, the return value is the number of characters
//      written to the buffer pointed to by pszWide. 
//
//      If the function fails, the return value is zero. To get extended error
//      information, call GetLastError. GetLastError may return one of the
//      following error codes:
//
//          ERROR_INSUFFICIENT_BUFFER
//          ERROR_INVALID_PARAMETER
//          ERROR_NOT_ENOUGH_MEMORY
//*************************************************************************
// 2004.02.02 DALLEE - created.
//*************************************************************************

static int WideCharToWindowsChar ( IMLangConvertCharset *pConvert,
                                   UINT         uCodePage,
                                   LPCWSTR      pszWide,
                                   LPSTR        pszWindows,
                                   int          cchWindows )
{
    auto    UINT        uInputSize;
    auto    UINT        uOutputSize;

    auto    HRESULT     hResult;
    auto    int         nResult;

    nResult = 0;

    //
    // Convert to Windows 2-byte Unicode
    //

    if ( NULL == pConvert )
    {
        // With no MLANG interface, default to using old-style MultiByteToWideChar()
        // This may not have all the code pages available to MLANG, but does a fair job.

        nResult = WideCharToMultiByte( uCodePage, 0, pszWide, -1, pszWindows, cchWindows, NULL, NULL );
    }
    else
    {
        // Use MLANG

        // Attempt to set up conversion.
        //
        // Note that the result from Initialize() is not 100% reliable --
        // The first attempt to Initialize to codepages which are not available will correctly return
        // S_FALSE. A second attempt to intialize to the same codepages (legal according to MSDN's docs)
        // will return a bogus S_OK.

        if ( CP_ACP == uCodePage )
        {
            uCodePage = GetACP();
        }
        else if ( CP_OEMCP == uCodePage )
        {
            uCodePage = GetOEMCP();
        }

        hResult = pConvert->Initialize( 1200, uCodePage, MLCONVCHARF_AUTODETECT );

        if ( S_OK == hResult )
        {
            uInputSize = wcslen( pszWide ) + 1;     // +1 to force inclusion of EOS in conversion
            uOutputSize = cchWindows;               // Size in number of characters

            hResult = pConvert->DoConversionFromUnicode( const_cast<LPWSTR>(pszWide), &uInputSize, pszWindows, &uOutputSize );

            // DoConversionToUnicode() fails in more ways than are listed in MSDN's help...
            // S_FALSE for an unsupported conversion, but also in some cases uInputSize is reset to 0,
            // or uOutputSize may be set to 0.

            if ( ( S_OK == hResult ) && uInputSize && uOutputSize )
            {
                // Success

                nResult = strlen( pszWindows );
            }
            else
            {
                SetLastError( ERROR_INVALID_PARAMETER );
            }
        }
        else
        {
            if (FAILED(hResult))
            {
                SetLastError( ERROR_NOT_ENOUGH_MEMORY );
            }
            else
            {
                // This is S_FALSE for unsupported conversion...
                SetLastError( ERROR_INVALID_PARAMETER );
            }
        }
    }

    return ( nResult );
} // WindowsCharToWideChar()


//*************************************************************************
// GetDefaultEmailCodePage()
//
// UINT GetDefaultEmailCodePage( )
//
// This routine returns the default character set used for encoding
// email based on the current Ansi codepage.
//
// Return Values:
//      Codepage identifier for encoding email.
//      CP_ACP on error.
//*************************************************************************
// 2004.02.24 DALLEE - created.
//*************************************************************************

UINT GetDefaultEmailCodePage( )
{
    auto    IMultiLanguage  *pMultiLanguage;
    auto    MIMECPINFO      codePageInfo;
    auto    MIMECSETINFO    mimeCsetInfo;
    auto    BSTR            bstrCharacterSet;
    auto    int             nCodePage;

    // Initialize locals

    pMultiLanguage      = NULL;
    bstrCharacterSet    = NULL;
    nCodePage           = CP_ACP;

    // Get MLANG interface

    CoCreateInstance( CLSID_CMultiLanguage,
                      NULL,
                      CLSCTX_INPROC_SERVER,
                      IID_IMultiLanguage,
                      (void**) &pMultiLanguage );

    if ( pMultiLanguage )
    {
        // Get codepage information -- Most importantly, email body character set name.
        if ( SUCCEEDED( pMultiLanguage->GetCodePageInfo( GetACP(), &codePageInfo ) ) )
        {
            // Get the preferred encoding for mail bodies
            bstrCharacterSet = SysAllocString( codePageInfo.wszBodyCharset );

            if ( bstrCharacterSet )
            {
                if ( SUCCEEDED ( pMultiLanguage->GetCharsetInfo( bstrCharacterSet, &mimeCsetInfo ) ) )
                {
                    nCodePage = mimeCsetInfo.uiInternetEncoding;
                }

                SysFreeString( bstrCharacterSet );
            }
        }

        pMultiLanguage->Release();
    }

    return ( nCodePage );
} // GetDefaultEmailCodePage()


//
// Base64 encoding
//

static const char g_Base64Alphabet[64 + 1] =
{
   ASC_CHR_A,  ASC_CHR_B,  ASC_CHR_C,  ASC_CHR_D,
   ASC_CHR_E,  ASC_CHR_F,  ASC_CHR_G,  ASC_CHR_H,
   ASC_CHR_I,  ASC_CHR_J,  ASC_CHR_K,  ASC_CHR_L,
   ASC_CHR_M,  ASC_CHR_N,  ASC_CHR_O,  ASC_CHR_P,
   ASC_CHR_Q,  ASC_CHR_R,  ASC_CHR_S,  ASC_CHR_T,
   ASC_CHR_U,  ASC_CHR_V,  ASC_CHR_W,  ASC_CHR_X,
   ASC_CHR_Y,  ASC_CHR_Z,  ASC_CHR_a,  ASC_CHR_b,
   ASC_CHR_c,  ASC_CHR_d,  ASC_CHR_e,  ASC_CHR_f,
   ASC_CHR_g,  ASC_CHR_h,  ASC_CHR_i,  ASC_CHR_j,
   ASC_CHR_k,  ASC_CHR_l,  ASC_CHR_m,  ASC_CHR_n,
   ASC_CHR_o,  ASC_CHR_p,  ASC_CHR_q,  ASC_CHR_r,
   ASC_CHR_s,  ASC_CHR_t,  ASC_CHR_u,  ASC_CHR_v,
   ASC_CHR_w,  ASC_CHR_x,  ASC_CHR_y,  ASC_CHR_z,

   ASC_CHR_0,  ASC_CHR_1,  ASC_CHR_2,  ASC_CHR_3,
   ASC_CHR_4,  ASC_CHR_5,  ASC_CHR_6,  ASC_CHR_7,
   ASC_CHR_8,  ASC_CHR_9, 

   ASC_CHR_PLUS,
   ASC_CHR_FSLASH,
   ASC_CHR_NUL
};


//************************************************************************
// DWORD EncodeBase64Buffer(char *pszInput,
//                          DWORD dwSize,
//                          char *pszOutput,
//                          DWORD dwOutputBufferSize)
//
// Purpose
//
// Parameters:
//
// Returns:
//
//************************************************************************
// 2/2/2004 DALLEE, "borrowed" from decomposer text engine source. Thanks, Carey!
//************************************************************************

static DWORD EncodeBase64Buffer (char *pszInput, DWORD dwSize, char *pszOutput, DWORD dwOutputBufferSize)
{
	BYTE	byTri[3];
	int		nTri, nChar;
	DWORD	dwOutputCount = 0;

	byTri[0] = 0;
	byTri[1] = 0;
	byTri[2] = 0;
	nTri = 0;

	while (dwSize > 0)
	{
		nChar = (int)*pszInput++;
		byTri[nTri++] = (BYTE)nChar;
		if (nTri == 3)
		{
			// OK, have a complete set of three characters.
			// Encode the trio into 4 bytes and write them to the output.
            if ( dwOutputBufferSize > 4 )
            {
			    MIMEEncode64TriToQuad (byTri, pszOutput);
			    pszOutput += 4;
                dwOutputBufferSize -= 4;
            }
			nTri = 0;
			byTri[0] = 0;
			byTri[1] = 0;
			byTri[2] = 0;
			dwOutputCount += 4;
		}

		dwSize--;
	}

	if (nTri != 0)
	{
		// Hit the end of the data stream with at least one character
		// in the trio buffer.  Encode whatever we got so far and
		// write it to the output. Fill in end of quad with one or two '='
        if ( dwOutputBufferSize > 4 )
        {
    		MIMEEncode64TriToQuad (byTri, pszOutput);

            for ( ; nTri < 3; ++nTri )
            {
                pszOutput[1+nTri] = ASC_CHR_EQ;
            }

    		pszOutput += 4;
            dwOutputBufferSize -= 4;
        }

		dwOutputCount += 4;
	}

    // Null terminate output

    if ( dwOutputBufferSize )
    {
        *pszOutput = '\0';
    }

	return (dwOutputCount);
}


//************************************************************************
// void MIMEEncode64TriToQuad(lpbyTri, lpszQuad)
//
// Purpose
//
//  This function encodes 3 8-bit bytes into 4 valid BASE64 bytes.
//
// Parameters:
//
//  lpbyTri: IN=Array of 3, 8-bit bytes to be encoded
//  lpszQuad: OUT=Array of 4, valid BASE64 characters to be outputted
//
// Returns:
//
//  nothing
//
//************************************************************************
// 8/19/96 Carey created.
// 2/2/2004 DALLEE, "borrowed" from decomposer text engine source. Thanks, Carey!
//************************************************************************

static void MIMEEncode64TriToQuad
(
	LPBYTE				lpbyTri,
	LPSTR				lpszQuad
)
{
	// fast version!
	lpszQuad[0] = g_Base64Alphabet[lpbyTri[0] >> 2];
	lpszQuad[1] = g_Base64Alphabet[((lpbyTri[0] & 0x3) << 4) | (lpbyTri[1] >> 4)];
	lpszQuad[2] = g_Base64Alphabet[((lpbyTri[1] & 0xf) << 2) | (lpbyTri[2] >> 6)];
	lpszQuad[3] = g_Base64Alphabet[lpbyTri[2] & 0x3F];
}
