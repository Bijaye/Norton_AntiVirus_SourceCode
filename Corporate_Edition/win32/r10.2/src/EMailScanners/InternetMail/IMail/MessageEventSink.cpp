// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
// MessageEventSink.cpp: implementation of the CMessageEventSink class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <new>
#include <mlang.h>

#include "ModifyMail.h"
#include "IMIME.h"
#include <exception>
#include "PrependText.h"

//for dec2text integration for I-EMAIL 
#include "decinterface.h"

#include "MessageEventSink.h"

/////////////////////////////////////////////////////////////////////////////
// CMessageEventSink::CMessageEventSink()

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMessageEventSink::CMessageEventSink(): m_pDecObj(NULL), m_szWarningMsg(NULL)
{
    // Depth limit of one...
    // Assuming text content which will be displayed to the user
    // will not be encoded past this level.

    m_dwDepthLimit = 1;

    m_bProcessingBodySections = false;  // Set to true once we begin receiving body sections in OnChildFound() and OnObjectBegin()
    m_dwCurrentBodySection    = 0;      // 0-based, current body section.
}

CMessageEventSink::~CMessageEventSink()
{

}


//*************************************************************************
// CMessageEventSink::OnChildFound()
//
// Notification from Decomposer that a child of the current object is
// available for decomposition.
//
// Used to track the current MIME_BODY item being scanned. This will be used
// to determine which sections to modify within OnObjectBegin().
//
// DECRESULT CMessageEventSink::OnChildFound ( IDecContainerObject *pObject,
//                                             DWORD       dwType,
//                                             const char  *pszChildName,
//                                             DWORD       dwCharset,
//                                             bool        *pbContinue )
//
// Parameters:
//      *pObject
//      dwType
//      *pszChildName
//      dwCharset
//      *pbContinue
//
// Return Values:
//
// Remarks:
//
//*************************************************************************
// 2003.08.29 DALLEE - created.
//*************************************************************************

DECRESULT CMessageEventSink::OnChildFound ( IDecContainerObject *pObject,
                                            DWORD       dwType,
                                            const char  *pszChildName,
                                            DWORD       dwCharset,
                                            bool        *pbContinue )
{
    auto    DWORD   dwDepth;

	pszChildName;
	dwCharset;

	pObject->GetDepth( &dwDepth );

    // Tell the decomposer to stop processing the container
    // if the depth of the child is greater than the specified limit

    if ( dwDepth >= m_dwDepthLimit )
        *pbContinue = false;
    else
	    *pbContinue = true;

    // Once we see the first MIME_BODY item at depth=0, begin 0-based count of current body section.
    //
    // Notes: DEC_ITEMTYPE_MIME_BODY appears to be reported only once per archive.
    //        Subsequent calls to OnChildFound() will specify DEC_ITEMTYPE_NORMAL even though
    //        we're processing MIME bodies. Thus setting a flag rather than checking dwType each time.
    //
    //        Checking the count first before setting m_bProcessingBodySections. Since
    //        our count is zero-based, we don't want to increment the first time.

    if ( 0 == dwDepth )
    {
        if ( m_bProcessingBodySections )
        {
            ++m_dwCurrentBodySection;
        }

        if ( DEC_ITEMTYPE_MIME_BODY == dwType )
        {
            m_bProcessingBodySections = true;
        }
    }

	return DEC_OK; 
}


/////////////////////////////////////////////////////////////////////////////
// CMessageEventSink::OnContainerIdentified()

DECRESULT CMessageEventSink::OnContainerIdentified( IDecContainerObject *pObject, bool *pbContinue )
{
    auto    DWORD   dwDepth;

    auto    DWORD dwType = DEC_TYPE_UNKNOWN;

    if ( SUCCEEDED( pObject->GetContainerType( &dwType ) ) &&
         SUCCEEDED( pObject->GetDepth( &dwDepth ) ) )
    {
        // Special handling when MIME containers are identified.
        if ( dwType == DEC_TYPE_MIME )
        {
            // Fill in all the Body location for all MIME bodies that have
            // a "Content-Type: text/plain" or "Content-Type: text/html".
            if (dwDepth == 0)
            {
                m_objBodyCounters.calc (pObject, m_pDecObj);
            }
        }
    }

    *pbContinue = true;

    return DEC_OK;
}


//*************************************************************************
// CMessageEventSink::OnObjectBegin()
//
// Does warning message insertion into MIME_BODY sections
// determined earlier to be most likely to be displayed as text in
// a mail user agent.
//
// DECRESULT CMessageEventSink::OnObjectBegin( IDecObject  *pObject,
//                                             WORD        *pwResult,
//                                             char        *pszNewDataFile,
//                                             bool        *pbContinue )
//
// Parameters:
//      *pObject
//      *pwResult
//      *pszNewDataFile
//      *pbContinue
//
// Return Values:
//
// Remarks:
//
//*************************************************************************
// 2003.08.29 DALLEE - created.
//*************************************************************************

DECRESULT CMessageEventSink::OnObjectBegin( IDecObject  *pObject,
                                            WORD        *pwResult,
                                            char        *pszNewDataFile,
                                            bool        *pbContinue )
{
    auto    DWORD   dwDepth;

    // Set default response

    *pwResult = DECRESULT_UNCHANGED;
    *pszNewDataFile = '\0';
    *pbContinue = true;
    
    pObject->GetDepth( &dwDepth );

    // Looking for depth one MIME_BODY sections....

    if ( ( 1 == dwDepth ) && m_bProcessingBodySections )
    {
        if ( m_dwCurrentBodySection == m_objBodyCounters.getTextBodyNum( MIME_TOKEN_PLAIN ) )
        {
            modifyMessage( pObject, pwResult, pszNewDataFile, pbContinue, MIME_TOKEN_PLAIN );
        }
        else if ( m_dwCurrentBodySection == m_objBodyCounters.getTextBodyNum( MIME_TOKEN_HTML ) )
        {
            modifyMessage( pObject, pwResult, pszNewDataFile, pbContinue, MIME_TOKEN_HTML );
        }
    }

    return DEC_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CMessageEventSink::OnDiskFileNeeded()

DECRESULT CMessageEventSink::OnDiskFileNeeded(IDecContainerObject *pObject, char *pszdiskfile, int iBufferSize)
{
    auto    char    szDataFile[MAX_PATH];
    auto    DWORD   dwSize;

    // A Decomposer engine needs this object to be on-disk in order to work
    // with it.  So, make the switch here.
    
    // Get the object's temporary (extracted) file name.
    *szDataFile = '\0';
    dwSize = MAX_PATH;
    pObject->GetDataFile( szDataFile, &dwSize );

    if ( (*m_pDecObj)->SwitchFileToDisk( szDataFile, pszdiskfile, iBufferSize ) )
        return DECERR_GENERIC;

    return DEC_OK;
}


//*************************************************************************
// Non EventSink functions:

//*************************************************************************
// CMessageEventSink::SetDecObj()
//
// Save a copy of the CDecomposerSDK object being used to wrap Dec3.
//
// void CMessageEventSink::SetDecompSDK( CDecomposerSDK *pDecObj )
//
// Parameters:
//      *pDecObj
//
// Return Values:
//
// Remarks:
//
//*************************************************************************
// 2003.08.29 DALLEE - created.
//*************************************************************************
void CMessageEventSink::SetDecObj( CDecInterface *pDecObj )
{
    m_pDecObj = pDecObj;
} // CMessageEventSink::SetDecompSDK()


//*************************************************************************
// CMessageEventSink::modifyMessage()
//
// Does warning message insertion into MIME_BODY sections
// determined earlier to be most likely to be displayed as text in
// a mail user agent.
//
// bool CMessageEventSink::modifyMessage( IDecObject   *pObject,
//                                        WORD         *pwResult,
//                                        char         *pszNewDataFile,
//                                        bool         *pbContinue, 
//                                        enum MIMELexerTokens eTextType )
//
// Parameters:
//      *pObject
//      *pwResult
//      *pszNewDataFile
//      *pbContinue
//      eTextType
//
// Return Values:
//
// Remarks:
//
//*************************************************************************
// 2003.08.29 DALLEE - created.
//*************************************************************************

bool CMessageEventSink::modifyMessage( IDecObject   *pObject,
                                       WORD         *pwResult,
                                       char         *pszNewDataFile,
                                       bool         *pbContinue, 
                                       enum MIMELexerTokens eTextType )
{
    auto    bool    bMadeChange;

    auto    char    szDataFile[MAX_PATH];
    auto    bool    bCanModify;
    auto    bool    bCanReplace;
    auto    DWORD   dwSize;

    auto    LPSTR   lpszNewData;
    auto    LPSTR   lpszOriginalData;

    auto    LPSTR   lpszEncodedWarning;
    auto    int     cchEncodedWarning;

    auto    X_FILE  *fp;

    bMadeChange = false;

    lpszNewData = NULL;
    lpszOriginalData = NULL;
    lpszEncodedWarning = NULL;
    fp = NULL;

    // Set default unchanged result

    *pwResult = DECRESULT_UNCHANGED;
    *pszNewDataFile = '\0';
    *pbContinue = true;

    // Determine if the file can be modified or replaced

    pObject->GetCanModify( &bCanModify );
    pObject->GetCanReplace( &bCanReplace );

    if ( bCanModify )
    {
        // Open data file and determine size.

        *szDataFile = '\0';
        dwSize = MAX_PATH;
        if ( FAILED( pObject->GetDataFile( szDataFile, &dwSize ) ) )
            goto BailOut;

        fp = (*m_pDecObj)->x_fopen( szDataFile, "r" );
        if ( NULL == fp )
            goto BailOut;

        if ( 0 != (*m_pDecObj)->x_fseek( fp, 0, SEEK_END ) )
            goto BailOut;

        dwSize = (*m_pDecObj)->x_ftell( fp );
        if ( -1 == dwSize )
            goto BailOut;

        // Rewind and read entire contents.

		try
		{
			lpszOriginalData = new char [dwSize + 1];
		}
		catch (std::bad_alloc &) {}
        if ( NULL == lpszOriginalData )
            goto BailOut;

        if ( 0 != (*m_pDecObj)->x_fseek( fp, 0, SEEK_SET ) )
            goto BailOut;

        if ( dwSize != (*m_pDecObj)->x_fread( lpszOriginalData, 1, dwSize, fp ))
            goto BailOut;

        lpszOriginalData[ dwSize ] = '\0';

        (*m_pDecObj)->x_fclose( fp );
        fp = NULL;

        // Create replacement text

        switch ( eTextType )
        {
        case MIME_TOKEN_PLAIN:
            // Calculate for worse case expansion. Encoding size for iso-2022-jp is 4*bytes (2 byte char + 3 byte prefix + 3 byte postfix)
            cchEncodedWarning = 4 * _tcslen( m_szWarningMsg ) + 8 + 1;
		    try
		    {
			    lpszEncodedWarning = new char [ cchEncodedWarning ];
		    }
		    catch (std::bad_alloc &)
		    {
                goto BailOut;
		    }

            if ( ConvertWindowsString( CP_ACP,
                                       m_objBodyCounters.getTextBodyCharset( MIME_TOKEN_PLAIN ),
                                       m_szWarningMsg,
                                       lpszEncodedWarning, cchEncodedWarning ) )
            {
                PrependPlainText( &lpszNewData, lpszEncodedWarning, lpszOriginalData );
            }
            break;

        case MIME_TOKEN_HTML:
            // Note: ConvertWindowsString is now done by InsertHtmlText. 
            InsertHtmlText( &lpszNewData, m_szWarningMsg, lpszOriginalData, (WORD)(m_objBodyCounters.getTextBodyCharset( MIME_TOKEN_HTML )) );
            break;

        default:
            break;
        }

        if ( NULL == lpszNewData )
            goto BailOut;

        fp = (*m_pDecObj)->x_fopen( szDataFile, "w" );
        if ( NULL == fp )
            goto BailOut;

        dwSize = strlen( lpszNewData );
        if ( dwSize != (*m_pDecObj)->x_fwrite( lpszNewData, 1, dwSize, fp ) )
            goto BailOut;

        (*m_pDecObj)->x_fclose( fp );
        fp = NULL;

        bMadeChange = true;

        *pwResult = DECRESULT_CHANGED;
    }


BailOut:
    if ( NULL != lpszNewData )
        delete [] lpszNewData;

    if ( NULL != lpszEncodedWarning )
        delete [] lpszEncodedWarning;

    if ( NULL != lpszOriginalData )
        delete [] lpszOriginalData;

    if ( NULL != fp )
        (*m_pDecObj)->x_fclose( fp );

    return ( bMadeChange );
} // CMessageEventSink::modifyMessage()


//*************************************************************************************
// bool CMessageEventSink::myCopyFile(const char *pszExistingFile, const char *pszNewFile, bool bFailIfExists)
//
// Purpose
//
//  This function emulates the Win32 function CopyFile, but uses the
//  Dec3 stream support I/O functions.
//
// Parameters:
//
//  pszExistingFile
//  pszNewFile
//  bFailIfExists
//
// Returns:
//
//  true for success
//  false on error
//
//*************************************************************************************

bool CMessageEventSink::myCopyFile(const char *pszExistingFileName, const char *pszNewFileName, bool bFailIfExists)
{
#define MY_COPY_BUFFER_SIZE     32 * 1024

    X_FILE *pExistingFile = NULL;
    X_FILE *pNewFile = NULL;
    size_t byte_count;
    bool bFileExists;

    const size_t buffer_size = MY_COPY_BUFFER_SIZE;
    BYTE *buffer = NULL;   
    bool rc = false;

    //
    // Validate input...
    //
    if (!pszExistingFileName || !pszNewFileName) 
        return rc;

    if ((strlen(pszExistingFileName) == 0) || (strlen(pszNewFileName) == 0))
        return rc;

	try
	{
		//
		// Allocate memory...
		//
		buffer = new BYTE[buffer_size];
	}
	catch (std::bad_alloc &) {}
    if (!buffer) 
    {
        return rc;
    }

    //
    // See if the file already exists...
    //
    bFileExists = ((*m_pDecObj)->x_access(pszNewFileName, 0) == 0 ? true : false);
    if (bFailIfExists) 
    {
        //
        // If it already exists then bail...
        //
        if (bFileExists)
        {
            goto bailOut;
        }
    }
    else 
    {
        //
        // Otherwise, it doesn't matter if the if the file exists or not.
        // Just open in truncation mode...
        //
        pNewFile = (*m_pDecObj)->x_fopen(pszNewFileName, "wb");
        
        //
        // If we can't open it, then bail...
        //
        if (!pNewFile)
        {
            goto bailOut;
        }
    }

    //
    // Try to open the existing file...
    //
    pExistingFile = (*m_pDecObj)->x_fopen(pszExistingFileName, "rb");
    if (!pExistingFile) 
    {
        //
        // Unable to open the existing file
        //
        goto bailOut;   
    }

    for(;;)
    {
        byte_count = (*m_pDecObj)->x_fread(buffer, 1, buffer_size, pExistingFile);

        if (byte_count == 0) 
        {
            break;
        }

        if ((*m_pDecObj)->x_fwrite(buffer, 1, byte_count, pNewFile) != byte_count) 
        {
            break;
        }
    }

    //
    // Check for errors...
    //
    if ((*m_pDecObj)->x_ferror(pNewFile) || (*m_pDecObj)->x_ferror(pExistingFile)) 
    {
        goto bailOut;
    }

    rc = true;

bailOut:
    //
    // Clean up...
    //
    if (buffer)
    {
        delete [] buffer;
        buffer = NULL;
    }

    if (pNewFile)
    {
        if(0 != (*m_pDecObj)->x_fclose(pNewFile))
        {
            rc = false;
        }

		
        pNewFile = NULL;
    }

    if (pExistingFile)
    {
        if (0 != (*m_pDecObj)->x_fclose(pExistingFile))
        {
            rc = false;
        }

		if( 0 != (*m_pDecObj)->x_remove(pszExistingFileName) )
		{
			rc = false;
		}

        pExistingFile = NULL;
    }

    return rc;
}


void CMessageEventSink::CBodyCounters::clear()
{
    for (DWORD xx = 0; xx < m_dwNumElems; ++xx)
    {
        m_arr[xx].dwBodyNum = 0xFFFFFFFF;
        m_arr[xx].eTextType = MIME_TOKEN_LAST;
    }
}

DWORD CMessageEventSink::CBodyCounters::getTextBodyNum (enum MIMELexerTokens eTextType) const
{
    for (DWORD xx = 0; xx < m_dwNumElems; ++xx)
    {
        if (m_arr[xx].eTextType == eTextType)
            return m_arr[xx].dwBodyNum;
    }
    return 0xFFFFFFFF;
}

UINT CMessageEventSink::CBodyCounters::getTextBodyCharset (enum MIMELexerTokens eTextType) const
{
    for (DWORD xx = 0; xx < m_dwNumElems; ++xx)
    {
        if (m_arr[xx].eTextType == eTextType)
            return m_arr[xx].uCharset;
    }
    return CP_ACP;
}

bool CMessageEventSink::CBodyCounters::addNotExist (DWORD dwBodyNum, enum MIMELexerTokens eTextType, UINT uCharset)
{
    DWORD dwExists = getTextBodyNum (eTextType);

    // If the specific type already exists in our array, bail. We do not
    // add duplicate types.
    if (dwExists != 0xFFFFFFFF)
        return true;

    // If we are already max-ed out, return an error.
    if (m_dwNumElems == m_dwMaxElems)
        return false;

    m_arr[m_dwNumElems].dwBodyNum = dwBodyNum;
    m_arr[m_dwNumElems].eTextType = eTextType;
    m_arr[m_dwNumElems].uCharset  = uCharset;
    ++m_dwNumElems;
    
    return true;
}

/////////////////////////////////////////////////////////////////////////
// Function name:	CMessageEventSink::FillInBodyCounters()
//
// Description	:	This method traverses through the MIME tokens looking
//                  for the first MIME_TOKEN_PLAIN or MIME_TOKEN_HTML. It
//                  resets and then fills in the arrCounter table
//
// Return type	:	Returns false if there was a problem parsing the MIME
//					message; true, otherwise.
//						
// Argument		:	pObject - the object to check.
//
/////////////////////////////////////////////////////////////////////////
// Copied from SAVSMTP's Decomposer event sink.
/////////////////////////////////////////////////////////////////////////
bool CMessageEventSink::CBodyCounters::calc (IDecObject *pObject, CDecInterface  *pDecObj)
{
    clear();

	// Get IMIME pointer
	IMIME* pIMIME = NULL;
	DECRESULT hr = pObject->GetEngineInterface( reinterpret_cast<void **>(&pIMIME) );
	if ( ( hr != DEC_OK ) || ( pIMIME == NULL ) )
		return false;	// No MIME engine interface available.

	// Get the token file's filename.
	char szTokenFile[MAX_PATH];

	// No token file most likely means no tokens were emitted (thus, not a MIME file).
	if ( pIMIME->GetTokenFilename( szTokenFile, sizeof(szTokenFile) ) != MIME_ERROR_NONE )
		return false;

	// Open token file read only
	X_FILE* fp = (*pDecObj)->x_fopen( szTokenFile, "rb" );

    // Failed to open the token file.  This is most likely an I/O error.
    if ( fp == NULL )
		return false;

    // Search for this sequence of tokens.
	const size_t arrPrefixTokens[] = 
	{
		MIME_TOKEN_CONTENT_TYPE,
		MIME_TOKEN_TEXT
	};
	size_t nNumPrefixTokens = sizeof(arrPrefixTokens) / sizeof(*arrPrefixTokens);

    size_t               nCurPrefixToken   = 0;
	size_t               nRead             = 0;
    size_t               nCharsetStart     = 0;
    size_t               nCharsetEnd       = 0;
    bool                 bReadingCharset   = false;
    bool                 bPrefixTokenMatch = false;
    DWORD                dwCurBodyCnt      = 0;
    enum MIMELexerTokens eCurContentType   = MIME_TOKEN_LAST;
	MIMEEMITTOKEN met;

	do
	{
		// Read a token from the token file.
		nRead = (*pDecObj)->x_fread( &met, 1, sizeof(met), fp );

		if ( nRead == sizeof(met) )
		{
            // If we're reading the character set field, record the end of the
            // field when the next token is received.
            if ( bReadingCharset )
            {
                nCharsetEnd = met.nTokenOffset;
                bReadingCharset = false;
            }

            // If we have a Boundary start, we clear our recorded state.
            if (met.iTokenID == MIME_TOKEN_BOUNDARY_START)
            {
                eCurContentType   = MIME_TOKEN_LAST;
                bPrefixTokenMatch = false;

                nCharsetStart     = 0;
                nCharsetEnd       = 0;
                bReadingCharset   = false;
            }
            // Else, see if the token matches one of our PrefixTokens.
			else if (met.iTokenID == arrPrefixTokens[nCurPrefixToken])	
			{
				++nCurPrefixToken;

				if (nCurPrefixToken == nNumPrefixTokens)
                {
                    bPrefixTokenMatch = true;
                    nCurPrefixToken   = 0;
                }
			}
            // Else, if we have already matched the prefix tokens and this
            // token is a TEXT or HTML, we know we have the
            // "Content-Type: text/plain" or "Content-Type: text/html".
            else if (bPrefixTokenMatch && 
                     (met.iTokenID == MIME_TOKEN_PLAIN || 
                      met.iTokenID == MIME_TOKEN_HTML))
            {
                // Remember our type for later.
                eCurContentType = (enum MIMELexerTokens)met.iTokenID;
            }
            // If we've previously found "Content-Type: text/plain" or "Content-Type: text/html",
            // save the location of the character set token for lookup later.
            else if ( ( eCurContentType != MIME_TOKEN_LAST ) && ( met.iTokenID == MIME_TOKEN_CHARSET ) )
            {
                // Save start here. Save end of field later based on offset of the very next token.
                nCharsetStart = met.nTokenOffset;
                bReadingCharset = true;
            }
            // Else, if we have bound a body ...
            else if (met.iTokenID == MIME_TOKEN_BODY)
            {
                // We now have a body. If we previously saw a
                // "Content-Type: text/plain" or "Content-Type: text/html",
                // record our current body count in our results structure.
                if (eCurContentType != MIME_TOKEN_LAST)
                {
                    UINT    uCharset = CP_ACP;

                    if ( nCharsetStart && ( nCharsetEnd > nCharsetStart ) )
                    {
                        uCharset = getCharset( pObject, pDecObj, nCharsetStart, nCharsetEnd - nCharsetStart );
                    }

                    if ( CP_ACP == uCharset )
                        uCharset = GetDefaultEmailCodePage();

                    addNotExist (dwCurBodyCnt, eCurContentType, uCharset);
                }
                ++dwCurBodyCnt;
            }
			
			// These tokens need to be handled specially because they have extra data
			// appended after them.
			if ( ( met.iTokenID == MIME_TOKEN_FILENAME ) || ( met.iTokenID == MIME_TOKEN_NAME ) )
			{
				// special case -- need to skip past filename data
				unsigned long ulLength;

				if ( (*pDecObj)->x_fread( &ulLength, 1, sizeof( ulLength), fp ) != sizeof(ulLength) )
				{
					(*pDecObj)->x_fclose(fp);		
					return false;
				}

				// Since we don't care about these filenames, seek past the filename 
				// info to keep offsets in sync.
				if ( (*pDecObj)->x_fseek( fp, ulLength, SEEK_CUR ) != 0)
				{
					(*pDecObj)->x_fclose(fp);		
					return false;
				}
			}
		}
		else	// read failed, we may be at EOF
		{
			if ( ( nRead != 0 ) || ( (*pDecObj)->x_feof( fp ) == 0 ) )
			{
				(*pDecObj)->x_fclose(fp);		
				return false;
			}

			// Found EOF
			break;
		}
	}
	while ( nRead == sizeof(met) );

	if ( (*pDecObj)->x_fclose(fp) != 0 )
		return false;

	return true;
}



//*************************************************************************
// CMessageEventSink::CBodyCounters::getCharset()
//
// Returns the windows codepage corresponding to the charset value identified
// in the current decomposer object.
//
//*************************************************************************
// 2004.02.23 DALLEE - created.
//*************************************************************************

UINT CMessageEventSink::CBodyCounters::getCharset ( IDecObject *pObject,
                                                    CDecInterface  *m_pDecObj,
                                                    size_t nCharsetStart,
                                                    size_t nCharsetLength )
{
    auto    char    szDataFile [ MAX_PATH ];
    auto    LPSTR   pszCharsetField;            // Allocated buffer for entire charset header field
    auto    LPSTR   pszCharsetValue;            // Pointer to charset value inside pszCharsetField
    auto    LPWSTR  pwszCharsetValue;
    auto    BSTR    bstrCharsetValue;
    auto    DWORD   dwSize;
    auto    UINT    uCodePage;
    auto    X_FILE  *fp;

    auto    IMultiLanguage  *pMultiLanguage;
    auto    MIMECSETINFO    mimeCsetInfo;

    pszCharsetField  = NULL;
    pszCharsetValue  = NULL;
    pwszCharsetValue = NULL;
    bstrCharsetValue = NULL;
    fp = NULL;
    pMultiLanguage = NULL;

    // Default to CP_ACP. Can look up default mail encoding later, or leave text in internal windows representation.
    uCodePage = CP_ACP;

    if ( ( 0 == nCharsetStart ) || ( 0 == nCharsetLength ) )
    {
        // No charset tag to look up, return default
        goto BailOut;
    }

    // Open data file

    *szDataFile = '\0';
    dwSize = sizeof( szDataFile ) / sizeof( *szDataFile );

    if ( FAILED( pObject->GetDataFile( szDataFile, &dwSize ) ) )
        goto BailOut;

    fp = (*m_pDecObj)->x_fopen( szDataFile, "r" );
    if ( NULL == fp )
        goto BailOut;

    // Read the charset field

    if ( 0 != (*m_pDecObj)->x_fseek( fp, nCharsetStart, SEEK_SET ) )
        goto BailOut;

	try
	{
		pszCharsetField = new char [ nCharsetLength + 1 ];
	}
	catch (std::bad_alloc &){}
    if ( NULL == pszCharsetField )
        goto BailOut;

    if ( nCharsetLength != (*m_pDecObj)->x_fread( pszCharsetField, 1, nCharsetLength, fp ) )
        goto BailOut;

    pszCharsetField[ nCharsetLength ] = '\0';

    // Find the charset value in the charset field, stripping whitespace and quotes.
    // Ex: From [charset="iso-8859-1"] to [iso-8859-1]

    pszCharsetValue = _tcschr( pszCharsetField, '=' );
    if ( NULL == pszCharsetValue )
        goto BailOut;

    // Find character set value ( token delimited by whitepace, CRLF, =, ;, or " )
    pszCharsetValue = _tcstok( pszCharsetValue, " \t\r\n=;\"" );
    if ( NULL == pszCharsetValue )
        goto BailOut;

    //
    // Use IMultiLanguage to retrieve the codepage used to decode this character set.
    //

    dwSize = _tcslen( pszCharsetValue ) + 1;
	try
	{
		pwszCharsetValue = new WCHAR [ dwSize ];
	}
	catch (std::bad_alloc &){}
    if ( NULL == pwszCharsetValue  )
        goto BailOut;

    if ( 0 == MultiByteToWideChar( CP_ACP, 0, pszCharsetValue, -1, pwszCharsetValue, dwSize ) )
        goto BailOut;

    bstrCharsetValue = SysAllocString( pwszCharsetValue );

    CoCreateInstance( CLSID_CMultiLanguage,
                      NULL,
                      CLSCTX_INPROC_SERVER,
                      IID_IMultiLanguage,
                      (void**) &pMultiLanguage );

    if ( NULL == pMultiLanguage )
        goto BailOut;

    if ( FAILED( pMultiLanguage->GetCharsetInfo( bstrCharsetValue, &mimeCsetInfo ) ) )
        goto BailOut;

    uCodePage = mimeCsetInfo.uiInternetEncoding;

BailOut:
    if ( NULL != pMultiLanguage )
        pMultiLanguage->Release();

    if ( NULL != bstrCharsetValue )
        SysFreeString( bstrCharsetValue );

    if ( NULL != pwszCharsetValue )
        delete [] pwszCharsetValue;

    if ( NULL != pszCharsetField )
        delete [] pszCharsetField;

    if ( NULL != fp )
        (*m_pDecObj)->x_fclose( fp );

    return ( uCodePage );
}
