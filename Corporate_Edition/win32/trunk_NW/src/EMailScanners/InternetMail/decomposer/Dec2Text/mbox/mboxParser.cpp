// MBOXParser.cpp : Parser for the MBOX engine
// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright 2002, 2005 by Symantec Corporation.  All rights reserved.

// Fix the defect in IBM's header files for V3.x of Visual Age for C++.
#include "fixibmps.h"

#include "DecPlat.h"
#include "Dec2.h"
#include "Dec2EX.h"
#include "Dec2LIB.h"
#include "Decstdio.h"
#include "mbox.h"
#include "dec_assert.h"

#include "asc_char.h"
#include "asc_ctype.h"
#include "bstricmp.h"
#include "IMIME.h"
#include "asc_ebc.h"

//#define DEBUGMBOX
//#define DEBUGMBOXSTATE

CMBOXParser::CMBOXParser()
{
	m_pDecomposer        = NULL;
	m_pEngine            = NULL;
	m_pArchive           = NULL;
	m_pMBOXArchive       = NULL;
	m_fpArchive          = NULL;
	m_fFirstLine         = true;
	m_pszReplacementName = NULL;
	m_nStartOffset       = 0;
	m_nCurrentEntry      = 0;
	m_eParseState        = PS_START;
	m_iTokenIDSave       = 0;
	m_pStringSave        = NULL;
	m_nStringLengthSave  = 0;
	m_nOffsetSave        = 0;
	m_nStartOffsetSave   = 0;
	m_nSenderAtoms       = 0;
	m_nTrailingAtoms     = 0;

	// We do not have an end token to process
	m_fHaveEndToken   = false;
	// If the state machine only recognizes a partial "from_" line we have a syntax error.
	m_fSyntaxError    = false;
	// A "simple from" is the keyword from, at the beginning of the line, that is soon followed by a line feed.
	m_fJustSimpleFrom = false;
}

CMBOXParser::~CMBOXParser()
{
	if (m_pMBOXArchive)
	{
		delete m_pMBOXArchive;
		m_pMBOXArchive = NULL;
	}

	if (m_pszReplacementName)
	{
		free(m_pszReplacementName);
		m_pszReplacementName = NULL;
	}
}

bool CMBOXParser::Init(CTextFile *pArchive, CTextEngine *pEngine, IDecomposerEx *pDecomposer)
{
	m_pArchive           = pArchive;
	m_pEngine            = pEngine;
	m_pDecomposer        = pDecomposer;
	m_pszReplacementName = NULL;
	m_nStartOffset       = 0;
	m_nCurrentEntry      = 0;

	DWORD dwValue        = 0;
	bool bEnforceCRC     = false;

	//
	// Set any applicable MBOX archive options here
	//
	if (SUCCEEDED(pDecomposer->GetOption(DEC_OPTION_ENFORCE_CRC, &dwValue)))
		bEnforceCRC = (dwValue ? true : false);

	// Create new MBOX archive object.  This object handles all the decoding/encoding/extraction.
	m_pMBOXArchive = new CMBOXFile(bEnforceCRC);
	if (!m_pMBOXArchive)
		return false;

	return true;
}

int CMBOXParser::ConsumeToken(
	int iTokenID,
	char *pString,
	size_t nStringLength,
	size_t nOffset)
{
	int rc = PARSER_OK;	// Assume parsing needs to continue

	// Copy the input parameters into the parser object.  These parameters
	// now become the things that drive the state machine.
	m_iTokenID      = iTokenID;
	m_pString       = pString;
	m_nStringLength = nStringLength;
	m_nOffset       = nOffset;

	// See if the input token is valid for the state that we are
	// currently in.  If not, then we need to stop parsing because the data
	// is not in a form that this engine is designed to handle.  We check
	// this by simply consulting a matrix of MBOX_PARSER_STATE_xxx vs.
	// MBOX_TOKEN_xxx.
	// For debug purposes, make sure the input token ID and current state
	// are within the range of the matrix.
	dec_assert(iTokenID < MBOX_TOKEN_LAST);

	// Always reset the entire emit structure here.  These fields may
	// get modified as we progress through the state machine.
	EMIT_OFFSET = m_nOffset;
	EMIT_ID     = m_iTokenID;
	EMIT_FLAGS  = 0;
	EMIT_DATA   = 0;

	// "From_" lines, that are not the first line, will have an entry end preceeding them.
	rc = StateMachine( m_fFirstLine );

	return (rc);
}


int CMBOXParser::FindFirstEntry(FILE *fpin, TEXT_FIND_DATA *pData)
{
	dec_assert(pData);

	if (!fpin)
		return (LEXER_ERROR);

	// All mbox filenames are considered to be in the CP850 character set.
	m_pArchive->m_dwFilenameCharset = DEC_CHARSET_CP850;

	if (m_pMBOXArchive->Open(fpin) != DEC_OK)
		return (LEXER_ERROR);

	m_fpArchive = fpin;
	m_nCurrentEntry = (size_t)-1;

	return (FindNextEntry(pData));
}


int CMBOXParser::FindNextEntry(TEXT_FIND_DATA *pData)
{
	char		*pszPrimaryName = NULL;
	DWORD		dwCharset = DEC_CHARSET_ASCII;
	DWORDLONG	dwlSize;
	DECRESULT	hr;

	dec_assert(pData);

	m_nCurrentEntry++;

	// Reset the primary and secondary name strings.
	pData->pszName = NULL;
	pData->pszSecondaryName = NULL;

	pszPrimaryName = (char *)malloc(MAX_PATH);
	if (!pszPrimaryName)
		return LEXER_ERROR;

	hr = m_pMBOXArchive->GetEntryName(m_nCurrentEntry, pszPrimaryName, MAX_PATH);
	if (FAILED(hr))
	{
		free(pszPrimaryName);
		return LEXER_ERROR;  // No more entries to enumerate.
	}

	// Fill in the caller's TEXT_FIND_DATA structure.
	pData->dwAttributes = 0;
	pData->dwAttrType = DEC_ATTRTYPE_DOS;
	pData->dwYear = 1980;
	pData->dwMonth = 1;
	pData->dwDay = 1;
	pData->dwHour = 0;
	pData->dwMinute = 0;
	pData->dwSecond = 0;
	pData->dwMillisecond = 0;
	pData->dwType = DEC_ITEMTYPE_NORMAL;
	pData->nCompressedSize = (size_t)-1;
	pData->nUncompressedSize = (size_t)-1;

	hr = m_pMBOXArchive->GetEntryUncompressedSize(m_nCurrentEntry, &dwlSize);
	if (!FAILED(hr))
	{
		pData->nUncompressedSize = (size_t)dwlSize;
		pData->nCompressedSize = (size_t)dwlSize;
	}

	pData->pszName = pszPrimaryName;
	pData->dwNameCharset = DEC_CHARSET_CP850;

	// Return the enumerated entry to the caller.
	return LEXER_OK;
}

int CMBOXParser::ExtractEntry(char *pszFilename, FILE *fpin)
{
	DECRESULT	hr;

	hr = m_pMBOXArchive->ExtractEntry(m_nCurrentEntry, pszFilename);
	if (hr == DEC_OK)
		return LEXER_OK;

	return LEXER_ERROR;
}


int CMBOXParser::ReplaceEntry(TEXT_CHANGE_DATA *pData, bool *pbKeepChildDataFile, bool *pbKeepChildNewDataFile)
{
	DECRESULT	hr;

	hr = m_pMBOXArchive->ReplaceEntry(m_nCurrentEntry, 
									  pData->pszFilename, 
									  pData->bRenameItem ? pData->pszName : NULL);
	if (FAILED(hr))
		return LEXER_ERROR;

	hr = m_pMBOXArchive->GetEntryOffsets(m_nCurrentEntry, &pData->nStartOffset, &pData->nEndOffset);
	if (FAILED(hr))
		return LEXER_ERROR;

	hr = m_pArchive->MarkForReplace(pData);
	if (FAILED(hr))
		return LEXER_ERROR;

	if (pbKeepChildDataFile)
		*pbKeepChildDataFile = true;
	if (pbKeepChildNewDataFile)
		*pbKeepChildNewDataFile = true;

	return LEXER_OK;
}


int CMBOXParser::AddEntry(TEXT_CHANGE_DATA *pData)
{
	// This function is not supported for mbox containers.
	return LEXER_ERROR;
}


int CMBOXParser::DeleteEntry(TEXT_CHANGE_DATA *pData)
{
	DECRESULT	hr;
	char		*pszOutputFilename = NULL;

	hr = m_pMBOXArchive->DeleteEntry(m_nCurrentEntry);
	if (FAILED(hr))
		return LEXER_ERROR;

	hr = m_pMBOXArchive->GetEntryOffsets(m_nCurrentEntry, &pData->nStartOffset, &pData->nEndOffset);
	if (FAILED(hr))
		return LEXER_ERROR;

	hr = m_pArchive->MarkForDelete(pData);
	if (FAILED(hr))
		return LEXER_ERROR;

	return LEXER_OK;
}


int CMBOXParser::ProcessComplete(void)
{
	DECRESULT	hr;

	hr = m_pMBOXArchive->Close(m_pszReplacementName);

	return (FAILED(hr) ? LEXER_ERROR : LEXER_OK);
}


/////////////////////////////////////////////////////////////////////////////
// GenerateUnknownName()
size_t CMBOXParser::GenerateUnknownName(char *pBuffer, size_t nBufferSize)
{
	size_t  idx;
	char    hex_buf[20];
	int     hex_idx;
	int     hex_len;
	DWORD   dwTemp;
	int     this_digit;

	// We don't expect this function to fail.
	dec_assert(nBufferSize >= 16);

	if (nBufferSize < 16)
		return (0);

	// Build a name string that is (in ASCII) the word "Unknown" followed
	// by the 8-digit hex value of the section offset with a .data extension.
	strcpy(pBuffer, "\x55\x6E\x6B\x6E\x6F\x77\x6E"); // "Unknown"
	idx = 7; // strlen("Unknown");

	// Build the hex part in a separate buffer because we have to build it
	// from right to left.
	hex_idx = sizeof(hex_buf);	// Pre-decremented, so start one past the end of the buffer.
	hex_len = 0;

	// We assume only one item per boundary can be extracted. In order
	// to work like the old MIME engine, name it based on the offset of 
	// the section rather than the actual data being extracted. QA
	// requested this to make validation easier. Also, if m_nBoundaryStart
	// is zero then this must be a single-part MIME message, since we
	// are going to extract an item that is not in a multipart MIME section.
	// In that case, QA would like to see names that have a zero offset, since
	// that's how the old MIME engine did it.
	dwTemp = m_nStartOffset;

	do 
	{
		this_digit = dwTemp & 0x0F;
		dwTemp = (dwTemp >> 4);
		--hex_idx;

		if (this_digit > 9) 
		{
			hex_buf[hex_idx] = (this_digit - 10) + 0x41; /* A */
		}
		else 
		{
			hex_buf[hex_idx] = this_digit + 0x30; /* 0 */
		}

		++hex_len;

	} while (dwTemp != 0);

	// Pad the hex string to the left with zeros to 8 digits..
	while (hex_len < 8)
	{
		--hex_idx;
		hex_buf[hex_idx] = '\x30'; // "0"
		++hex_len;
	}

	// Append the hex string to the name.
	while (hex_len > 0) 
	{
		pBuffer[idx] = hex_buf[hex_idx];
		--hex_len;
		++idx;
		++hex_idx;
	}

	// Append the extension.
	pBuffer[idx] = '\x2E'; // "."
	++idx;
	pBuffer[idx] = '\x64'; // "d"
	++idx;
	pBuffer[idx] = '\x61'; // "a"
	++idx;
	pBuffer[idx] = '\x74'; // "t"
	++idx;
	pBuffer[idx] = '\x61'; // "a"
	++idx;
	pBuffer[idx] = 0;

	return(idx);
}


//-----------------------------------------------------------------------------
//
// Purpose: Prints a message for debugging purposes.
//          See #define DEBUGONMBOX
//
//-----------------------------------------------------------------------------
// 04/28/03 Eric Feldl created.
//-----------------------------------------------------------------------------
inline void CMBOXParser::DebugValueOut( char* szText, char* szCaption )
{
	#ifdef DEBUGMBOX

		char szNative[256];

		strcpy( szNative, szText );

		dec_printf("+MBOX DebugValueOut via atoi: %d, %s\n", atoi( szText ), szCaption );

		ascii_to_native( szNative );

		dec_printf("+MBOX DebugValueOut Native via atoi: %d, %s\n", atoi( szNative ), szCaption );

	#endif	
}

//-----------------------------------------------------------------------------
//
// Purpose: Prints a message for debugging purposes.
//          See #define DEBUGONMBOX
//
//-----------------------------------------------------------------------------
// 04/28/03 Eric Feldl created.
//-----------------------------------------------------------------------------
inline void CMBOXParser::DebugOut( char* szText, char* szCaption )
{
	#ifdef DEBUGMBOX

		char szNative[256];

		strcpy( szNative, szText );

		dec_printf("+MBOX DebugOut: %s, %s\n", szText, szCaption );

		ascii_to_native( szNative );

		dec_printf("+MBOX DebugOut Native: %s, %s\n", szNative, szCaption );

	#endif	
}

//-----------------------------------------------------------------------------
//
// Purpose: Prints a message for debugging purposes.
//          See #define DEBUGONMBOX
//
//-----------------------------------------------------------------------------
// 04/28/03 Eric Feldl created.
//-----------------------------------------------------------------------------
inline void CMBOXParser::DebugStateOut( char* szText, char* szCaption )
{
	#ifdef DEBUGMBOXSTATE

		char szNative[256];

		strcpy( szNative, szText );
		
		dec_printf("+MBOX DebugStateOut: %s, %s\n", szText, szCaption );

		ascii_to_native( szNative );

		dec_printf("+MBOX DebugStateOut Native: %s, %s\n", szNative, szCaption );

	#endif	
}
//-----------------------------------------------------------------------------
//
// Purpose:     The lexer continues to emit tokens after a parser syntax error. 
//              We need to discard these tokens until a new valid sequence is
//              emitted.	
//
// Parameters:  void
//
// Returns:     void
//
//-----------------------------------------------------------------------------
// 05/23/03 Eric Feldl created.
//-----------------------------------------------------------------------------
inline void CMBOXParser::DiscardTokenDebugOut( void )
{
	#ifdef DEBUGMBOX

		if ( m_iTokenID == MBOX_TOKEN_EOF )
		{
			dec_printf("+MBOX DiscardTokenDebugOut: %s, %s\n", m_pString, "Discarding EOF");
		}
		else if ( m_iTokenID == MBOX_TOKEN_FROM_LINE_LF )
		{
			dec_printf("+MBOX DiscardTokenDebugOut: %s, %s\n", m_pString, "Discarding LF");
		}
		else if ( m_iTokenID == MBOX_TOKEN_HEADER_ATOM )
		{
			dec_printf("+MBOX DiscardTokenDebugOut: %s, %s\n", m_pString, "Discarding Atom");
		}
		else if ( m_iTokenID == MBOX_TOKEN_ENTRY_START )
		{
			dec_printf("+MBOX DiscardTokenDebugOut: %s, %s\n", m_pString, "Discarding Entry Start");
		}
		else
		{
			dec_printf("+MBOX DiscardTokenDebugOut: %s, %s\n", m_pString, "Discarding other token");
		}

	#endif
}

//-----------------------------------------------------------------------------
//
// Purpose: Converts the input character to lower case.
//
// Parameters:  char     Input character
//
// Returns:     char     Lower case character
//
//-----------------------------------------------------------------------------
// 04/28/03 Rob Turner created.
//-----------------------------------------------------------------------------
char CMBOXParser::ForceLowercase(char in)
{
	return ( ( in >= '\x41' && in <= '\x5A' ) ? (in |= 0x20) : in );
}

//-----------------------------------------------------------------------------
//
// Purpose: Remove the trailing space from the token if it exists.
//
// Parameters:  char*   Input string
//
// Returns:     bool    True if the string was truncated
//
//-----------------------------------------------------------------------------
// 04/28/03 Eric Feldl created.
//-----------------------------------------------------------------------------
bool CMBOXParser::RemoveWhitespace( char* szString )
{
	bool fTruncated = false;

	// Index into the temp string
	int  iTempString = 0;

	// Length of the original string
	int  nLen = strlen( szString );

	// Temporary string to copy characters into
	char* szTempString = ( char* ) malloc( sizeof(char) * nLen + 1 );
	
	if( !szTempString )
	{
		return fTruncated;
	}

	// Terminate our string
	szTempString[0] = 0;

	// Traverse the entire string to remove whitespace from the end ( and the beginning ) of the string.
	// This is probably overkill, but will provide the highest level of robustness.
	for(int j = 0; j < nLen; j++ )
	{
		// If it's a letter copy it.
		if( szString[ j ] != ASC_CHR_SPACE && szString[ j ] != ASC_CHR_CR &&
	       szString[ j ] != ASC_CHR_LF    && szString[ j ] != ASC_CHR_TAB )
		{
			szTempString[ iTempString++ ] = szString[ j ];
		}
	}

	// Terminate the string
	szTempString[ iTempString ] = 0;

	// The modified length
	int nNewLen = iTempString;

	// If the lengths are different then we must of changed the string or the string is just a space.
	if( nNewLen != nLen )
	{
		// We will return the new string
		strcpy( szString, szTempString );
		
		fTruncated = true;
	}

	free(szTempString);

	return fTruncated;
	
}

//-----------------------------------------------------------------------------
//
// Purpose: Examines the input string for email address characteristics.
//
// Parameters:  char*   Input string
//
// Returns:     bool    True if the string is likely an email account.
//
//-----------------------------------------------------------------------------
// 04/28/03 Eric Feldl created.
//-----------------------------------------------------------------------------
bool CMBOXParser::IsSenderValid( char* szSender )
{
	// Locate that @ sign
	char* pAT  = strchr( szSender, ASC_CHR_AT );

	// Locate the period
	char* pPeriod = strchr( szSender, ASC_CHR_PERIOD );

	// Do they exist?
	if( pAT && pPeriod)
	{
		DebugOut( szSender, "IsSenderValid() true" );
		return true;
	}
	else
	{
		DebugOut( szSender, "IsSenderValid() false" );
		return false;
	}
}

//-----------------------------------------------------------------------------
//
// Purpose: Validates the input string is a day of the week.
//
// Parameters:  char*   Input string
//
// Returns:     bool    True if the string is a day of the week.
//
//-----------------------------------------------------------------------------
// 04/28/03 Eric Feldl created.
//-----------------------------------------------------------------------------
bool CMBOXParser::IsWeekDayValid( char* szWeekDay )
{
	bool fWeekDay = false;

	// Examine the first character of the input stream after converting it to lower case
	switch( ForceLowercase( szWeekDay[0] ) )
	{
		// Monday
		case '\x6D': // m
			if( binary_stricmp( szWeekDay, STR_mon, STR_MON) == 0)
			{
				fWeekDay = true;
			}
			break;
		// Tuesday, Thursday
		case '\x74': // t
			// Examine the second character of the input stream
			if( ForceLowercase(  szWeekDay[1] ) == '\x75' ) // u
			{
				if( binary_stricmp( szWeekDay, STR_tue,STR_TUE) == 0)
				{
					fWeekDay = true;
				}
			}
			else
			{
				if( binary_stricmp( szWeekDay, STR_thu, STR_THU) == 0)
				{
					fWeekDay = true;
				}
			}
			break;
		// Wednesday
		case '\x77': // w
			if( binary_stricmp( szWeekDay, STR_wed, STR_WED) == 0)
			{
				fWeekDay = true;
			}
			break;
		// Friday
		case '\x66': // f
			if( binary_stricmp( szWeekDay, STR_fri, STR_FRI) == 0)
			{
				fWeekDay = true;
			}
			break;
		// Saturday, Sunday
		case '\x73': // s
			// Examine the second character of the input stream
			if( ForceLowercase(  szWeekDay[1] ) == '\x61' ) // a
			{
				if( binary_stricmp( szWeekDay, STR_sat, STR_SAT) == 0)
				{
					fWeekDay = true;
				}
			}
			else
			{
				if( binary_stricmp( szWeekDay, STR_sun, STR_SUN) == 0)
				{
					fWeekDay = true;
				}
			}
			break;
		default:
			break;
	}

	if( fWeekDay )
	{
		DebugOut( szWeekDay, "IsWeekDayValid() true" );
	}
	else
	{
		DebugOut( szWeekDay, "IsWeekDayValid() false" );
	}

	return fWeekDay;
}

//-----------------------------------------------------------------------------
//
// Purpose: Validates the input string is a month.
//
// Parameters:  char*   Input string
//
// Returns:     bool    True if the string is a month.
//
//-----------------------------------------------------------------------------
// 04/28/03 Eric Feldl created.
//-----------------------------------------------------------------------------
bool CMBOXParser::IsMonthValid( char* szMonth )
{
	bool fMonth = false;

	// Examine the first character of the input stream after converting it to
	// lower case
	switch( ForceLowercase( szMonth[0] ) )
	{
		// January, June, July
		case '\x6A': // j
			// Examine the second character of the input stream
			if( ForceLowercase( szMonth[1] ) == '\x61' ) // a
			{
				if( binary_stricmp( szMonth, STR_jan, STR_JAN) == 0)
				{
					fMonth = true;
				}
			}
			// Examine the third character of the input stream
			else if( ForceLowercase( szMonth[2] ) == '\x6E' ) // n
			{
				if( binary_stricmp( szMonth, STR_jun, STR_JUN) == 0)
				{
					fMonth = true;
				}
			}
			else
			{
				if( binary_stricmp( szMonth, STR_jul, STR_JUL) == 0)
				{
					fMonth = true;
				}
			}
			break;
		// February
		case '\x66': // f
			if( binary_stricmp( szMonth, STR_feb,STR_FEB) == 0)
			{
				fMonth = true;
			}
			break;
		// March, May
		case '\x6D': // m
			// Examine the third character of the input stream
			if( ForceLowercase( szMonth[2] ) == '\x72' ) // r
			{
				if( binary_stricmp( szMonth, STR_mar, STR_MAR) == 0)
				{
					fMonth = true;
				}
			}
			else
			{
				if( binary_stricmp( szMonth, STR_may, STR_MAY) == 0)
				{
					fMonth = true;
				}
			}
			break;
		// April, August
		case '\x61': // a
			// Examine the second character of the input stream
			if( ForceLowercase( szMonth[1] ) == '\x70' ) // p
			{
				if( binary_stricmp( szMonth, STR_apr, STR_APR) == 0)
				{
					fMonth = true;
				}
			}
			else
			{
				if( binary_stricmp( szMonth, STR_aug, STR_AUG) == 0)
				{
					fMonth = true;
				}
			}
			break;
		// September
		case '\x73': // s
			if( binary_stricmp( szMonth, STR_sep, STR_SEP) == 0)
			{
				fMonth = true;
			}
			break;
		// October
		case '\x6F': // o
			if( binary_stricmp( szMonth, STR_oct, STR_OCT) == 0)
			{
				fMonth = true;
			}
			break;
		// November
		case '\x6E': // n
			if( binary_stricmp( szMonth, STR_nov, STR_NOV) == 0)
			{
				fMonth = true;
			}
			break;
		// December
		case '\x64': // d
			if( binary_stricmp( szMonth, STR_dec, STR_DEC) == 0)
			{
				fMonth = true;
			}
			break;
		default:
			break;
	}

	if( fMonth )
	{
		DebugOut( szMonth, "IsMonthValid() true" );
	}
	else
	{
		DebugOut( szMonth, "IsMonthValid() false" );
	}

	return fMonth;
}

//-----------------------------------------------------------------------------
//
// Purpose: Validates the input string is a day of the month.
//
// Parameters:  char*   Input string
//
// Returns:     bool    True if the string is a day of the month.
//
//-----------------------------------------------------------------------------
// 04/28/03 Eric Feldl created.
//-----------------------------------------------------------------------------
bool CMBOXParser::IsDayValid( char* szDay )
{
	// Minimum and Maximum calendar days of the month
	const int MIN_DAY =  1;
	const int MAX_DAY = 31;

	DebugValueOut( szDay, "IsDayValid() before conversions");

	// Obtain the value of the day field
	int nDay = ascii_atoi( szDay );

	#ifdef DEBUGMBOX
		dec_printf( "nDay: %d\n", nDay );
	#endif

	// Is the day within our range?
	if( nDay >= MIN_DAY && nDay <= MAX_DAY )
	{
		// Valid day
		DebugOut( szDay, "IsDayValid() true" );
		return true;
	}
	else
	{
		DebugOut( szDay, "IsDayValid() false" );
		return false;
	}
}

//-----------------------------------------------------------------------------
//
// Purpose: Validates the input string is a clock time.
//
// Parameters:  char*   Input string
//
// Returns:     bool    True if the string is a clock time.
//
//-----------------------------------------------------------------------------
// 04/28/03 Eric Feldl created.
//-----------------------------------------------------------------------------
bool CMBOXParser::IsTimeValid( char* szTime )
{
	// time = 1*2 DIGIT ":" 1*2 DIGIT [ ":" 1*2 DIGIT ]

	// Length, Minimum, and Maximum time values.
	const int MAX_LEN_TIME =  8;
	const int MIN_LEN_TIME =  3;
	const int MIN_DIGIT    =  0;
	const int MAX_DIGIT    = 59;
	const int MAX_HOUR     = 23;
	const int MIN_HOUR     =  0;

	bool fSecondsEntry     = false;
	bool fValidSeconds     = false;
	bool fValidHrMin       = false;

	DebugValueOut( szTime, "IsTimeValid() before conversions");

	// How long is our time string
	int nLen = strlen( szTime );

	// Is it the correct length? See bnf.
	if( nLen < MIN_LEN_TIME || nLen > MAX_LEN_TIME )
	{
		DebugOut( szTime, "IsTimeValid() false, invalid length" );
		return false;
	}

	// Find the first ( or possibly only ) colon
	char* pFirstColon  = strchr( szTime, ASC_CHR_COLON );

	char* pSecondColon = NULL;

	if( pFirstColon )
	{
		// Find the second colon
		pSecondColon = strchr( &pFirstColon[1], ASC_CHR_COLON );

		if( pSecondColon )
		{
			// This clock time includes the optional seconds field
			fSecondsEntry = true;

			DebugValueOut( &pSecondColon[1], "IsTimeValid() Seconds before conversions");

			// Get the seconds value to the right of the second colon
			int nSec = ascii_atoi( &pSecondColon[1] );

			#ifdef DEBUGMBOX
				dec_printf( "nSec: %d\n", nSec );
			#endif

			// Are we in within the allowed range
			if( nSec >= MIN_DIGIT && nSec <= MAX_DIGIT )
			{
				// Terminate the string at the second colon to simplify parsing.
				pSecondColon  = 0;
				fValidSeconds = true;
			}
		}

		DebugValueOut( &pFirstColon[1], "IsTimeValid() Minutes before conversions");

		// Get the minutes value to the right of the remaining or only colon
		int nMin = ascii_atoi( &pFirstColon[1] );

		#ifdef DEBUGMBOX
			dec_printf( "nMin: %d\n", nMin );
		#endif

		// Are we in within the allowed range
		if( nMin >= MIN_DIGIT && nMin <= MAX_DIGIT )
		{
			// Terminate the string at the first colon to simplify parsing.
			pFirstColon = 0;

			DebugValueOut( szTime, "IsTimeValid() Hours before conversions");

			// Get the hours
			int nHour = ascii_atoi( szTime );

			#ifdef DEBUGMBOX
				dec_printf( "nHour: %d\n", nHour );
			#endif

			// Are we in within the allowed range
			if( nHour >= MIN_HOUR && nHour <= MAX_HOUR )
			{
				// Valid time
				fValidHrMin = true;
			}
		}
	}

	// Do we have the optional seconds field and is it valid
	if( fSecondsEntry )
	{
		// Are all 3 fields: hours, minutes, & seconds valid?
		if( fValidHrMin && fValidSeconds )
		{
			DebugOut( szTime, "IsTimeValid() true, all 3 fields" );
			return true;
		}
		else
		{
			DebugOut( szTime, "IsTimeValid() false, checking 3 fields" );
			return false;
		}
	}
	else
	{
		// Are the hours and minutes fields valid?
		if( fValidHrMin )
		{
			DebugOut( szTime, "IsTimeValid() true, 2 fields" );
			return true;
		}
		else
		{
			DebugOut( szTime, "IsTimeValid() false, checking 2 fields" );
			return false;
		}
	}
}

//-----------------------------------------------------------------------------
//
// Purpose: Validates the input string is a year.
//
// Parameters:  char*   Input string
//
// Returns:     bool    True if the string is a year
//
//-----------------------------------------------------------------------------
// 04/28/03 Eric Feldl created.
//-----------------------------------------------------------------------------
bool CMBOXParser::IsYearValid( char* szYear )
{
	// Four digit years
	const int MIN_4YEAR = 1971; // Email invented, common in 1977
	const int MAX_4YEAR = 2099;

	// For compatibility reasons with legacy software, two digit years greater
	// than or equal to 70 should be interpreted as the years 1970+, while
	// two digit years less than 70 should be interpreted as the years 2000-2069.

	const int MIN_2YEAR = 0;
	const int MAX_2YEAR = 99;

	bool fValid = false;

	int  nLen = strlen( szYear );

	DebugValueOut( szYear, "IsYearValid() Year before conversions");

	// Get the year
	int nYear = ascii_atoi( szYear );

	#ifdef DEBUGMBOX
		dec_printf( "nYear: %d\n", nYear );
	#endif

	if( nLen == 4 )
	{
		// Are we in within the allowed range for a four digit year
		if( nYear >= MIN_4YEAR && nYear <= MAX_4YEAR )
		{
			// Valid 4 digit year
			DebugOut( szYear, "IsYearValid() true, 4 digit" );
			fValid = true;
		}
	}
	else if( nLen == 2 )
	{
		// Are we in within the allowed range for a two digit year
		if( nYear >= MIN_2YEAR && nYear <= MAX_2YEAR )
		{
			// Valid 2 digit year
			DebugOut( szYear, "IsYearValid() true, 2 digit" );
			fValid = true;
		}
	}

	if( !fValid )
	{
		DebugOut( szYear, "IsYearValid() false" );
	}

	return fValid;
}

//-----------------------------------------------------------------------------
//
// Purpose: Remember this location as a possible entry offset but do not commit
//			it until we have verified it. 
//
// Parameters:  void
//
// Returns:     void
//
//-----------------------------------------------------------------------------
// 05/23/03 Eric Feldl created.
//-----------------------------------------------------------------------------
void CMBOXParser::SaveEntryOffset( void )
{
	m_iTokenIDSave      = m_iTokenID;
	m_pStringSave       = m_pString;
	m_nStringLengthSave = m_nStringLength;
	m_nOffsetSave       = m_nOffset;
	m_nStartOffsetSave  = m_nStartOffset;
}

//-----------------------------------------------------------------------------
//
// Purpose: Adds an entry to our list.
//
// Parameters:  void
//
// Returns:     void
//
//-----------------------------------------------------------------------------
// 04/30/03 Eric Feldl created.
//-----------------------------------------------------------------------------
int CMBOXParser::AddEntry( void )
{
	int rc = PARSER_OK;

	// Verify that we have start and offset numbers that make sense.
	if ( m_nOffsetSave > m_nStartOffsetSave + 1 )
	{
		// Save this entry
		DECRESULT hr = m_pMBOXArchive->AddFoundEntry( m_nStartOffsetSave, m_nOffsetSave - 1 );
		if (FAILED( hr ))
		{
			rc = PARSER_ERROR;
			return rc;
		}

		char szName[32];

		if ( GenerateUnknownName( szName, sizeof( szName ) ) == 0 )
		{
			rc = PARSER_ERROR;
			return rc;
		}
		else
		{
			hr = m_pMBOXArchive->SetEntryName( m_nCurrentEntry++, szName );
			if (FAILED( hr ))
			{
				rc = PARSER_ERROR;
				return rc;
			}
		}

		// We completed varification and processing. We know longer have an end token
		// to process.
		m_fHaveEndToken = false;
	}
	return rc;
}

//-----------------------------------------------------------------------------
//
// Purpose: Sets the current parser state and error status flag.
//
// Parameters:  eState  Parser state
//
//              bool    Error status flag
//
// Returns:     void
//
//-----------------------------------------------------------------------------
// 04/28/03 Eric Feldl created.
//-----------------------------------------------------------------------------
void CMBOXParser::SetState( MBOXParserStates eState, bool fSyntaxError )
{
	m_eParseState   = eState;
	m_fSyntaxError  = fSyntaxError;
}

//-----------------------------------------------------------------------------
//
// Purpose: State machine for the mbox container.
//
// Parameters:  void
//
// Returns:     int    Parser return code.
//
//-----------------------------------------------------------------------------
// 04/28/03 Eric Feldl created.
//-----------------------------------------------------------------------------
int CMBOXParser::StateMachine( bool fFirstLine )
{
	int	rc = PARSER_OK;

	// Example line: From g.c.purgold@larc.nasa.gov Mon Mar 13 09:41:50 2000

	//DebugOut( m_pString, "0000 Entered State Machine");

	// The lexer tokenizes on spaces, remove them.
	RemoveWhitespace( m_pString );

	//DebugOut( m_pString, "0001 Removed white space");

	// The lexer may emit a standalone space if extra spaces are inserted into
	// the stream. Throw these away.
	if( m_iTokenID == MBOX_TOKEN_HEADER_ATOM && strlen( m_pString ) == 0 )
	{
		return rc;
	}

	// Move to the next state
	switch( m_eParseState )
	{
		// State machine start up state
		case PS_START:
		{
			rc = StateStart( fFirstLine );
		}
		break;
		// We have a from_
		case PS_FROM_:
		{
			rc = StateFrom();
		}
		break;
		// We have a sender
		case PS_SENDER:
		{
			rc = StateSender();
		}
		break;
		// We have a week day: Mon .. Sun
		case PS_WEEKDAY:
		{
			rc = StateWeekDay();
		}
		break;
		// We have a month
		case PS_MONTH:
		{
			rc = StateMonth();
		}
		break;
		// We have a day of the month: 1 .. 31
		case PS_DAY:
		{
			rc = StateDay();
		}
		break;
		// We have a time. We should get a timezone or year next
		case PS_TIME:
		{
			rc = StateTime( fFirstLine );
		}
		break;
		case PS_TIME_ZONE1:
		{
			rc = StateTimezone1( fFirstLine );
		}
		break;
		case PS_TIME_ZONE2:
		{
			rc = StateTimezone2( fFirstLine );
		}
		break;
		// End of this line
		case PS_YEAR:
		{
			rc = StateYear( fFirstLine );
		}
		break;
		case PS_STARTENTRY:
		{
			rc = StateStartEntry( fFirstLine );
		}
		break;
		default:
		{
			// Unknown mbox token
			dec_assert(0);
			break;
		}
	}

	return rc;
}

//-----------------------------------------------------------------------------
//
// Purpose: On valid input transitions
//          PS_START                -> PS_FROM_
//          MBOX_TOKEN_ENTRY_END    -> PS_START
//
// Parameters:  bool   Is this the first line of the mbox file
//
// Returns:     int    Parser return code.
//
//-----------------------------------------------------------------------------
// 05/23/03 Eric Feldl created.
//-----------------------------------------------------------------------------
int CMBOXParser::StateStart( bool fFirstLine )
{
	int	rc = PARSER_OK;

	m_nTrailingAtoms = 0;
	m_nSenderAtoms   = 0;

	if( fFirstLine )
	{
		DebugStateOut( "0100 First line of Mbox", "Start State" );

		// Current token
		if( m_iTokenID == MBOX_TOKEN_FROM_ )
		{
			DebugStateOut( "0101 From Token", "Start State" );

			// Next state
			SetState( PS_FROM_, false );
		}
		else
		{
			DiscardTokenDebugOut();
		}
	}
	else
	{
		DebugStateOut( "0102 Not the first line of Mbox", "Start State" );

		// Is the current token a from_
		if( m_iTokenID == MBOX_TOKEN_FROM_ )
		{
			DebugStateOut( "0103 From Token", "Start State" );

			SetState( PS_FROM_, false );
		}
		// An end or eof token signifies the end of an entry, but we can't believe the
		// lexer yet. Save the entry end and check further to verify.
		else if( m_iTokenID == MBOX_TOKEN_ENTRY_END ||  m_iTokenID == MBOX_TOKEN_EOF )
		{
			DebugStateOut( "0104 END || EOF", "Start State" );

			SaveEntryOffset();

			if( m_fJustSimpleFrom )
			{
				DebugStateOut( "0105 Simple From", "Start State" );
				rc = AddEntry();
				m_fJustSimpleFrom = false;
			}

			if( !m_fSyntaxError )
			{
				DebugStateOut( "0106 No sytax errors", "Start State" );

				// We consumed an end or eof token that will require a call to AddFoundEntry().
				m_fHaveEndToken     = true;

				SetState( PS_START, false );
			}
			else
			{
				DebugStateOut( m_pString, "0107 Rejected End or EOF due to error" );
			}

		}
		else
		{
			DiscardTokenDebugOut();
		}
	}
	return rc;
}

//-----------------------------------------------------------------------------
//
// Purpose: On valid input transitions
//          PS_FROM_ -> PS_SENDER
//          PS_FROM_ -> PS_WEEKDAY
//
// Parameters:  void
//
// Returns:     int     Parser return code.
//
//-----------------------------------------------------------------------------
// 05/23/03 Eric Feldl created.
//-----------------------------------------------------------------------------
int CMBOXParser::StateFrom( void )
{
	int	rc = PARSER_OK;

	SetState( PS_START, true );

	// We expect an atom next.
	if( m_iTokenID == MBOX_TOKEN_HEADER_ATOM )
	{
		DebugStateOut( "0200 Header Atom", "From State");

		// We have to be careful here. Adapt to multiple formats.

		// Does this look like an address?
		if( IsSenderValid( m_pString ) )
		{
			DebugStateOut( "0201 Setting next state to Sender", "From State");

			// Normal. Sender address.
			SetState( PS_SENDER, false );
		}
		// Could the sender address be missing and instead we encounter a week day?
		else if( IsWeekDayValid( m_pString ) )
		{
			DebugStateOut( "0202 Setting next state to Weekday", "From State");

			// We have a weekday here. Next look for the month.
			SetState( PS_WEEKDAY, false );
		}
		else
		{
			DebugStateOut( "0203 Catch All Else, Setting next state to Sender", "From State");

			// We could encounter a header such as: From - Wed Feb 05 04:31:50 2003
			// Continue. We will not strictly enforce this.
			SetState( PS_SENDER, false );
		}
	}

	return rc;
}

//-----------------------------------------------------------------------------
//
// Purpose: On valid input transitions
//          PS_SENDER -> PS_WEEKDAY
//
// Parameters:  void
//
// Returns:     int     Parser return code.
//
//-----------------------------------------------------------------------------
// 05/23/03 Eric Feldl created.
//-----------------------------------------------------------------------------
int CMBOXParser::StateSender( void )
{
	int	rc = PARSER_OK;

	if( m_iTokenID == MBOX_TOKEN_FROM_LINE_LF )
	{
		DebugStateOut( "0300 Line Feed", "Sender State");

		// We encountered a from that is definitely not part of a header
		m_fJustSimpleFrom = true;
		SetState( PS_START, true );
		return rc;
	}

	// Allow the sender field to be variable length	but never greater than 6 atoms
	if( m_nSenderAtoms < 5 ) // See StateFrom for first atom
	{
		DebugStateOut( "0301 Atoms++", "Sender State");

		// Example: From Mr Don R. Cahoon Jr. (d.r.cahoon@larc.nasa.gov) Thu Mar 30 11:03:14 -600 1985
		// Stay in this state until we come upon a week day or exceed our max tries
		SetState( PS_SENDER, true );
		m_nSenderAtoms++;
	}
	else
	{
		DebugStateOut( "0302 Atoms++ >= 5", "Sender State");

		SetState( PS_START, true );
		m_nSenderAtoms = 0;
	}

	// Now that we have the failure state set, examine the atom.

	// Is this a general purpose atom?
	if( m_iTokenID == MBOX_TOKEN_HEADER_ATOM )
	{
		DebugStateOut( "0303 Header Atom", "Sender State");

		// Does this look like a day of the week?
		if( IsWeekDayValid( m_pString ) )
		{
			DebugStateOut( "0304 Setting next state to Weekday", "Sender State");

			SetState( PS_WEEKDAY, false );
		}
		else if( IsDayValid( m_pString ) )
		{
			DebugStateOut( "0305 Setting next state to Start", "Sender State");

			// We might have an alternate date/time format, possibly an RFC 822
			// From e.k.feldl@larc.nasa.gov 13 Mar 03 09:41:50 +5000
			SetState( PS_START, true );
		}
	}

	return rc;
}

//-----------------------------------------------------------------------------
//
// Purpose: On valid input transitions
//          PS_WEEKDAY -> PS_MONTH
//
// Parameters:  void
//
// Returns:     int     Parser return code.
//
//-----------------------------------------------------------------------------
// 05/23/03 Eric Feldl created.
//-----------------------------------------------------------------------------
int CMBOXParser::StateWeekDay( void )
{
	int	rc = PARSER_OK;

	// Assume failure
	SetState( PS_START, true );

	// We expect an atom next
	if( m_iTokenID == MBOX_TOKEN_HEADER_ATOM )
	{
		DebugStateOut( "0400 Header Atom", "Weekday State");

		// Does this look like a month?
		if( IsMonthValid( m_pString ) )
		{
			DebugStateOut( "0401 Setting next state to Month", "Weekday State");

			SetState( PS_MONTH, false );
		}
		else if( m_pString[0] == ASC_CHR_COMMA )
		{
			DebugStateOut( "0402 Comma, Setting next state to Start", "Weekday State");

			// We might have an alternate date/time format possibly an RFC 822
			// From e.k.feldl@larc.nasa.gov Mon , 13 Mar 03 09:41:50 +5000
			SetState( PS_START, true );
		}
	}

	return rc;
}

//-----------------------------------------------------------------------------
//
// Purpose: On valid input transitions
//          PS_MONTH -> PS_DAY
//
// Parameters:  void
//
// Returns:     int     Parser return code.
//
//-----------------------------------------------------------------------------
// 05/23/03 Eric Feldl created.
//-----------------------------------------------------------------------------
int CMBOXParser::StateMonth( void )
{
	int	rc = PARSER_OK;

	// Assume failure
	SetState( PS_START, true );

	// Is this a general purpose atom?
	if( m_iTokenID == MBOX_TOKEN_HEADER_ATOM )
	{
		DebugStateOut( "0500 Header Atom", "Month State");

		// Does this look like a day of the month ie 1st to 31st?
		if( IsDayValid( m_pString ) )
		{
			DebugStateOut( "0501 Setting next state to Day", "Month State");

			SetState( PS_DAY, false );
		}
	}

	return rc;
}

//-----------------------------------------------------------------------------
//
// Purpose: On valid input transitions
//          PS_DAY -> PS_TIME
//
// Parameters:  void
//
// Returns:     int    Parser return code.
//
//-----------------------------------------------------------------------------
// 05/23/03 Eric Feldl created.
//-----------------------------------------------------------------------------
int CMBOXParser::StateDay( void )
{
	int	rc = PARSER_OK;

	// Assume failure
	SetState( PS_START, true );

	// Is this a general purpose atom?
	if( m_iTokenID == MBOX_TOKEN_HEADER_ATOM )
	{
		DebugStateOut( "0600 Header Atom", "Day State");

		// ie 09:41:50
		if( IsTimeValid( m_pString ) )
		{
			DebugStateOut( "0601 Setting next state to Time", "Day State");

			SetState( PS_TIME, false );
		}
	}

	return rc;
}

//-----------------------------------------------------------------------------
//
// Purpose: On valid input transitions
//          PS_TIME -> PS_TIME_ZONE1
//          PS_TIME -> PS_YEAR
//
// Parameters:  bool    Is this the first line of the mbox file
//
// Returns:     int     Parser return code.
//
//-----------------------------------------------------------------------------
// 05/23/03 Eric Feldl created.
//-----------------------------------------------------------------------------
int CMBOXParser::StateTime( bool fFirstLine )
{
	int	rc = PARSER_OK;

	// Assume failure
	SetState( PS_START, true );

	// Normally the year is paired with MBOX_TOKEN_FROM_LINE_LF but if the
	// year has an extra trailing space then the year will be paired with
	// MBOX_TOKEN_HEADER_ATOM.

	// We can ignore the m_iTokenID and do this simple test since a valid
	// timezone has ( a minimum value of -1200 and a maximum value of +1300 )
	// or will be a string such as DST. These values fall outside of our
	// year range checking.
	if( IsYearValid( m_pString ) )
	{
		DebugStateOut( "0700 Setting next state to Year", "Time State");

		SetState( PS_YEAR, false );

		// We verified all of the previous tokens. Now we can save the entry.
		if( m_fHaveEndToken && !fFirstLine)
		{
			DebugStateOut( "0701 AddEntry", "Time State");

			rc = AddEntry();
		}
	}
	// Is this a general purpose atom and not a year?
	else if( m_iTokenID == MBOX_TOKEN_HEADER_ATOM )
	{
		DebugStateOut( "0702 Header Atom, Setting next state to Time Zone", "Time State");

		// Return to this state next time and look for the year
		SetState( PS_TIME_ZONE1, false );
	}
	else
	{
		DebugStateOut( "0703 Other Atom ?", "Time State");
	}

	return rc;
}

//-----------------------------------------------------------------------------
//
// Purpose: On valid input transitions
//          PS_TIME_ZONE1 -> PS_TIME_ZONE2
//          PS_TIME_ZONE1 -> PS_YEAR
//
// Parameters:  bool    Is this the first line of the mbox file
//
// Returns:     int     Parser return code.
//
//-----------------------------------------------------------------------------
// 05/23/03 Eric Feldl created.
//-----------------------------------------------------------------------------
int CMBOXParser::StateTimezone1( bool fFirstLine )
{
	int	rc = PARSER_OK;

	// Assume failure
	SetState( PS_START, true );

	// Normally the year is paired with MBOX_TOKEN_FROM_LINE_LF but if the
	// year has extra trailing spaces then the year will be paired with
	// MBOX_TOKEN_HEADER_ATOM.

	// We can ignore the m_iTokenID and do this simple test since a valid
	// timezone has ( a minimum value of -1200 and a maximum value of +1300 )
	// or will be a string such as DST. These values fall outside of our
	// year range checking.
	if( IsYearValid( m_pString ) )
	{
		DebugStateOut( "0800 Setting next state to Year", "Timezone1 State");

		SetState( PS_YEAR, false );

		// We verified all of the previous tokens. Now we can save the entry.
		if( m_fHaveEndToken && !fFirstLine)
		{
			DebugStateOut( "0801 AddEntry", "Time State");

			rc = AddEntry();
		}
	}
	// Is this a general purpose atom and not a year?
	else if( m_iTokenID == MBOX_TOKEN_HEADER_ATOM )
	{
		DebugStateOut( "0802 Header Atom, Setting next state to Time Zone 2", "Time State");

		// Return to this state next time and look for the year
		SetState( PS_TIME_ZONE2, false );
	}
	else
	{
		DebugStateOut( "0803 Other Atom ?", "Time State");
	}

	return rc;
}

//-----------------------------------------------------------------------------
//
// Purpose: On valid input transitions
//          PS_TIME_ZONE2 -> PS_YEAR
//
// Parameters:  bool    Is this the first line of the mbox file
//
// Returns:     int     Parser return code.
//
//-----------------------------------------------------------------------------
// 05/23/03 Eric Feldl created.
//-----------------------------------------------------------------------------
int CMBOXParser::StateTimezone2( bool fFirstLine )
{
	int	rc = PARSER_OK;

	// Assume failure
	SetState( PS_START, true );

	// Normally the year is paired with MBOX_TOKEN_FROM_LINE_LF but if the
	// year has extra trailing spaces then the year will be paired with
	// MBOX_TOKEN_HEADER_ATOM.

	// We can ignore the m_iTokenID and do this simple test since a valid
	// timezone has ( a minimum value of -1200 and a maximum value of +1300 )
	// or will be a string such as DST. These values fall outside of our
	// year range checking.
	if( IsYearValid( m_pString ) )
	{
		DebugStateOut( "0900 Setting next state to Year", "Timezone2 State");

		SetState( PS_YEAR, false );

		// We verified all of the previous tokens. Now we can save the entry.
		if( m_fHaveEndToken && !fFirstLine)
		{
			DebugStateOut( "0901 AddEntry", "Timezone2 State");

			rc = AddEntry();
		}
	}

	return rc;
}

//-----------------------------------------------------------------------------
//
// Purpose: On valid input transitions
//          PS_YEAR -> PS_STARTENTRY
//
// Parameters:  bool    Is this the first line of the mbox file
//
// Returns:     int     Parser return code.
//
//-----------------------------------------------------------------------------
// 05/23/03 Eric Feldl created.
//-----------------------------------------------------------------------------
int CMBOXParser::StateYear( bool fFirstLine )
{
	int	rc = PARSER_OK;

	// Assume failure
	SetState( PS_START, true );

	// Normally the year is paired with MBOX_TOKEN_FROM_LINE_LF but if the
	// year has extra trailing spaces then the year will be paired with
	// MBOX_TOKEN_HEADER_ATOM. In this case we will receive MBOX_TOKEN_FROM_LINE_LF
	// late. Here we absorb it and wait for MBOX_TOKEN_ENTRY_START
	if( m_iTokenID ==  MBOX_TOKEN_FROM_LINE_LF || m_iTokenID ==  MBOX_TOKEN_HEADER_ATOM )
	{
		DebugStateOut( "1000 LF || Header Atom", "Year State");

		if( strlen( m_pString ) != 0 )
		{
			DebugStateOut( "1001 Strlen != 0", "Year State");

			// Cases have been found where the timezone follows the year, allow for this
			if( m_nTrailingAtoms < 2 )
			{
				DebugStateOut( "1002 Trailing Atoms < 2, Setting next state to Year", "Year State");

				// Allow this trailing atom
				SetState( PS_YEAR, false );
				m_nTrailingAtoms++;
			}
			else
			{
				DebugStateOut( "1003 Trailing Atoms >= 2, Setting next state to Start", "Year State");

				// More than the allowed trailing atoms. ie CUT DST for Central
				// European Time, Daylight Savings Time
				SetState( PS_START, true );
				m_nTrailingAtoms = 0;
			}
		}
		else
		{
			DebugStateOut( "1004 Other Atom ?", "Year State");

			// Extra space or tab appended to input.
			SetState( PS_YEAR, false );
		}
	}
	// Save the entry start information. An entry is the section of text
	// between from_ lines.
	else if( m_iTokenID ==  MBOX_TOKEN_ENTRY_START )
	{
		DebugStateOut( "1005 Entry Start Atom, Setting next state to StartEntry", "Year State");

		m_nStartOffset  = m_nOffset + m_nStringLength;

		m_fFirstLine    = false;

		SetState( PS_STARTENTRY, false );

		if( fFirstLine )
		{
			DebugStateOut( "<<* 1006 Parser Identified *>>", "Year State");

			rc = PARSER_IDENTIFIED;
		}
	}

	return rc;
}

//-----------------------------------------------------------------------------
//
// Purpose: On valid input transitions
//          PS_STARTENTRY -> PS_START
//
// Parameters:  bool    Is this the first line of the mbox file
//
// Returns:     int     Parser return code.
//
//-----------------------------------------------------------------------------
// 05/23/03 Eric Feldl created.
//-----------------------------------------------------------------------------
int CMBOXParser::StateStartEntry( bool fFirstLine )
{
	int	rc = PARSER_OK;

	// Assume failure
	SetState( PS_START, true );

	DebugStateOut( "1100", "StartStateEntry State");

	if( !fFirstLine )
	{
		DebugStateOut( "1101 Not First line", "StartStateEntry State");

		// Did we encounter an entry end? This means that we did encounter another from_ and
		// the lexer has emitted and entry end before it.
		if( m_iTokenID ==  MBOX_TOKEN_ENTRY_END )
		{
			DebugStateOut( "1102 Entry End Token, SaveEntryOffset, Setting next state to Start", "StartStateEntry State");

			// Looks like we found another from. Save the boundary info and check a little more.
			SaveEntryOffset();

			// We consumed an end or eof token that will require a call to AddFoundEntry().
			m_fHaveEndToken     = true;

			SetState( PS_START, false );
		}
		// Did we encounter an eof? This denotes an end of section even though we haven't
		// encountered another from_
		else if( m_iTokenID ==  MBOX_TOKEN_EOF )
		{
			DebugStateOut( "1103 EOF, SaveEntryOffset, AddEntry", "StartStateEntry State");

			// Save this entry now. This is our last chance.
			SaveEntryOffset();

			rc = AddEntry();
		}
	}

	return rc;
}








