// UUEncode.cpp : Encode/decode routines for UUE.
// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright 2001, 2005 Symantec, Peter Norton Product Group. All rights reserved.

#include <math.h>

// Fix the defect in IBM's header files for V3.x of Visual Age for C++.
#include "fixibmps.h"

#include "DecPlat.h"
#include "Dec2.h"
#include "Dec2EX.h"
#include "Dec2LIB.h"
#include "Decstdio.h"

//
// Note: The AS400 build system performs its builds one directory
// level higher than the other platforms.  It is easier to change
// just the specific AS400 cases rather than change every workspace's
// include search path.
//
#if defined(OS400)
	#include "TextFile.h"
#else
	#include "../TextFile.h"
#endif

#include "Support.h"
#include "UUE.h"
#include "asc_char.h"
#include "asc_ctype.h"
#include "bstricmp.h"

#define MY_BUFFER_SIZE (8 * 1024)

#define DEC(c) (char)(((c)- ASC_CHR_SPACE ) &077 )
#define ENC(c) (((c) & 077) + ASC_CHR_SPACE )

// global variables
extern volatile DWORD g_dwTextMaxExtractSize;
extern volatile bool g_bTextAbort;

// ascii-strings
static const char ASC_CHR_begin[] = // "begin"
{
   ASC_CHR_b,
   ASC_CHR_e,
   ASC_CHR_g,
   ASC_CHR_i,
   ASC_CHR_n,
   ASC_CHR_NUL
};

static const char ASC_CHR_BEGIN[] = // "BEGIN"
{
   ASC_CHR_B,
   ASC_CHR_E,
   ASC_CHR_G,
   ASC_CHR_I,						
   ASC_CHR_N,						
   ASC_CHR_NUL
};

static const char ASC_CHR_end[] =  // "end"
{
   ASC_CHR_e,
   ASC_CHR_n,
   ASC_CHR_d,
   ASC_CHR_NUL
};

static const char ASC_CHR_END[] =  // "END"                
{						 
   ASC_CHR_E,
   ASC_CHR_N,
   ASC_CHR_D,
   ASC_CHR_NUL
};

static const char ASC_CHR_begin_644_[] = // "begin 644 "
{
   ASC_CHR_b,
   ASC_CHR_e,
   ASC_CHR_g,
   ASC_CHR_i,
   ASC_CHR_n,
   ASC_CHR_SPACE,
   ASC_CHR_6,
   ASC_CHR_4,
   ASC_CHR_4,
   ASC_CHR_SPACE,
   ASC_CHR_NUL
};


unsigned char g_uulookup[256] =
{
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,		  // 0-7
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,		  // 8-15
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,		  // 16-23
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,		  // 24-31
	0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,		  // 32-39  (' ' == 32)
	0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,		  // 40-47
	0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,		  // 48-55
	0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,		  // 56-63
	0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27,		  // 64-71  ('A' == 65)
	0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f,		  // 72-79
	0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,		  // 80-87
	0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f,		  // 88-95  ('Z' == 90)
	0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,		  // 96-103 ('`' == 96)
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff
};


// local prototypes
static void EncodeTriToUUQuad(LPBYTE lpbyTri, char *lpszQuad);


//************************************************************************
// bool ascii_octal_decode(const char *input_string, int *len_out, char **ptr_next_out, 
//   DWORD *val_out)
//
// Purpose:
//
//  Decodes an ASCII octal string to a DWORD value.
//
// Parameters:
//
//  input_string: IN=The string to decode. At least the first character must
//                be a valid octal digit in order for this function to
//                succeed. The function stops decoding when it encounters any
//                character that is not a valid octal digit.
//  len_out:      OUT=The number of octal digits decoded. May be NULL if this
//                information is not required.
//  ptr_next_out: OUT=A pointer to the character that terminated the sequence.
//                If all characters in the string are octal digits, this will
//                point to the null terminator on return. May be NULL if this
//                information is not required.
//  val_out:      OUT=The DWORD value of the octal number, decoded. May be
//                NULL if this information is not required.
//
// Returns:
//
//  true on success. false on failure. There must be at least one valid octal
//  digit for decoding to succeed.
//************************************************************************
static bool ascii_octal_decode(const char *input_string, int *len_out, char **ptr_next_out, 
  DWORD *val_out)
{
	unsigned char *p = (unsigned char*)input_string;
	int len = 0;
	DWORD value = 0;
	const unsigned char asc_zero = 0x30;
	const unsigned char asc_seven = 0x37;

	dec_assert(input_string != NULL);

	// First character must be valid or we fail
	if((*p < asc_zero) || (*p > asc_seven))
	{
		return false;
	}

	do 
	{
		value = (value << 3);
		value |= (*p - asc_zero);
		++len;
		++p;
	} while((*p >= asc_zero) && (*p <= asc_seven));

	if(len_out != NULL)
	{
		*len_out = len;
	}

	if(ptr_next_out != NULL)
	{
		*ptr_next_out = (char*)p;
	}

	if(val_out != NULL)
	{
		*val_out = value;
	}

	return true;
}


//************************************************************************
// bool IsUUECandidate(lpszLine, pdwMode, lpszAttachFileName, dwNameSize)
//
// Purpose
//
//  This function determines whether the specified string is a valid uuencode
//  header.  If so, the string should be of the form: "begin xxx filename.ext"
//
// Parameters:
//
//  lpszLine: IN=String to check
//  pdwMode: OUT=Value of mode portion of header line
//  lpszAttachFileName: OUT=Filename extracted from the UU-encoded header line
//  dwNameSize: IN=Size of lpszAttachFileName buffer (in bytes)
//
// Returns:
//
//  true if the string is a valid UU-encoded header line.
//  false otherwise
//************************************************************************
bool CUUEParser::IsUUECandidate(char *lpszLine, DWORD *pdwMode, char *lpszAttachFileName, DWORD dwNameSize)
{

#define BEGIN_STRING_LEN	5
	const char szBegin[BEGIN_STRING_LEN] = { ASC_CHR_b,
											ASC_CHR_e,
											ASC_CHR_g,
											ASC_CHR_i,
											ASC_CHR_n };

	// See if input buffer is large enough to even be UUE...
	if (strlen(lpszLine) >= BEGIN_STRING_LEN)
	{
		// Scan the input line for "begin ".
		// See if the first character matches the beginning of the
		// szBegin string.  If so, then we may have a match.
		if (*lpszLine == szBegin[0])
		{
			if (memcmp (lpszLine, szBegin, BEGIN_STRING_LEN) == 0)
			{
				*lpszAttachFileName = 0;

				// Found "begin" at the beginning of a line!
				// Make sure it is followed by a single whitespace character
				// Must be at least one whitespace character.
				if (lpszLine[BEGIN_STRING_LEN] && ascii_isspace(lpszLine[BEGIN_STRING_LEN]))
				{
					// OK, it looks like UUE so far (up to "begin " anyway).
					// Now capture the octal mode and filename fields (if present).
					char	*ptr;
					int		iOctalLen = 0;

					// Convert the octal string to a DWORD value here.
					if (!ascii_octal_decode(&lpszLine[BEGIN_STRING_LEN+1], &iOctalLen, &ptr, pdwMode)) 
					{
						// Either the octal value is missing or invalid.  In either case,
						// just take the filename to be whatever follows "begin ".
						ptr = &lpszLine[BEGIN_STRING_LEN];
						goto skip_whitespace;
					}

					if (*ptr != 0)
					{
						// Octal string must be followed by whitespace
						if (!ascii_isspace(*ptr))
							return (false);

skip_whitespace:
						// Skip any whitespace between the octal mode and the filename.
						do
						{
							++ptr;
						} while (ascii_isspace(*ptr));

						// Copy out the filename here.
						strncpy(lpszAttachFileName, ptr, dwNameSize);
						lpszAttachFileName[dwNameSize - 1] = 0;
					}
				}

				return (true);	// Looks like UUE...
			}
		}
	}

	return (false);
}

//************************************************************************
//
//  Purpose
//
//  This function determines whether the specified string is a valid uuencode
//  header.  The routine supports fast identification with well formed headers
//  and slower identification on weak headers. Here are the rules:
//
//  begin 'octal number' filename   <--- Well formed header.  No data validation
//  begin 'octal number'            <--- Well formed header.  No data validation
//		^ Note the required space after the octal number
//  begin  filename                 <--- Well formed header.  No data validation
//		^^  Note the required double space if no mode number specified.
//  begin 'all other variations'    <--- Weak header. data validation required.
//
//
//  Parameters:
//
//  bIdentified                Has the data stream been identified as UUE
//  bRangeValid                Are the quads within the expected range
//  m_fStrongHeader            Do we have a weak or strong header format
//  szLine                     Stream to examine
//  nLineLength                Length of stream
//  nReturnCode                Parser return code
//
//  Returns:
//
//  True if the string is a valid UU-encoded header line.
//  False otherwise
//
//************************************************************************
bool CUUEParser::IsValidUUEContainer( bool* bIdentified,
                                    bool fStrongHeader,
                                    const unsigned char* szLine,
                                    unsigned int nLineLength,
                                    int* nReturnCode )
{
	bool bIsUULineValid	= false;

	if (!*bIdentified )
	{
		// If m_fValidateUUEBody is true, always check the UUE body before declaring that we have valid UUE.
		if ( !fStrongHeader || m_fValidateUUEBody )
		{
			// Test the first character of the UUE ( length character of pre-encoded data ) against the actual
			// length of the encoded data.
			bIsUULineValid = IsUULineValid( ( unsigned char* )szLine, nLineLength, NULL );

			// A weak header MUST be immediately followed by valid UUE.
			if( bIsUULineValid ) // Length check
			{
				m_fNumValidUUELines++;

				// Declare valid only once we have examined the mimimum number of lines.
				if( m_fNumValidUUELines > 0 )
				{
					*bIdentified	= true;
					*nReturnCode	= PARSER_IDENTIFIED;
				}
			}
			else 
			{
				*bIdentified = false;
				*nReturnCode = PARSER_DONE;
			}
		}
		else
		{
			// Strong header
			*bIdentified	= true;
			*nReturnCode	= PARSER_IDENTIFIED;
		}
	}

	return *bIdentified;
}

//************************************************************************
//
// Purpose
//
// This function returns the first line in the multi-line buffer
//
// Parameters
//
// szBuffer       Pointer to the multi-line encode buffer
// nLengthIn      Number of characters in the encode buffer
// nLineLenOut    Number of characters in the first line

// Returns        True if we found a cr/lf
//
//************************************************************************
bool CUUEParser::GetFirstLine( unsigned char* szBuffer,
                               long  nLengthIn,
                               long* nLineLenOut )
{
	*nLineLenOut	= -1;

	bool fFoundLine	= false;

	// Start at -1 so that we don't count the first character in determining the length
	long cnt = -1;

	// Determine length of encoded line
	for( long j = 0; j < nLengthIn; j++ )
	{
		if( IsCR( szBuffer[ j ] ) || IsLF( szBuffer[ j ] ) )
		{
			szBuffer[ j ] = 0;
			fFoundLine = true;
			break;
		}

		cnt++; // Length of encoded   
	}

	szBuffer[ cnt + 1 ] = 0;

	*nLineLenOut = cnt;

	return fFoundLine;
}

//************************************************************************
//
// Purpose
//
// Checks for a low buffer ( incomplete line ) condition and reads more
// bytes if necessary. This function also guarantees that the line is not
// preceeded by whitespace. This is done by seeking backward within the
// container and clearing the line if it is preceeded by whitespace. 
//
// Parameters
//
// lpstInput                Input file pointer
// szLine                   Input buffer
// nBytesInMultiLineBuffer  Number of characters in our multi-line buffer
// nLineLen                 The length of the first line in the multi-line buffer
// nExpectedLength          Length of the line as indicated by the first character          
// pfSuccess                Did we encounter any errors.
//
// Returns in szLine
// 1) The original unmodified line or
// 2) A longer line or
// 3) NULL if the line is preceeded by whitespace
//
//************************************************************************
void CUUEParser::ReadAhead( FILE* lpstInput,
                            unsigned char* szLine,
                            const size_t nBytesInMultiLineBuffer,
                            long* nLineLen,
                            unsigned long nBytesToAlloc,
                            bool* pfSuccess )
{
	// Allocate memory for a local copy of the input buffer
	unsigned char* szNewLine = NULL;

	// General purpose error flag.
	*pfSuccess = true;

	// Allocate a buffer
	szNewLine = ( unsigned char* )malloc( nBytesToAlloc + (1 * sizeof(unsigned char)));

	// Make sure it's valid
	if( szNewLine == NULL )
	{
		*pfSuccess = false;

		return;
	}

	// Terminate it for safety
	if(szNewLine[nBytesToAlloc] != 0)
	{
		szNewLine[nBytesToAlloc] = 0;
	}

	// Has our caller passed in a file pointer?
	if( !lpstInput )
	{
		// < We are in the header identification phase. >

		// Here we will use the main file pointer and the offset to find the relevant data.
		FILE* fp = m_pArchive->m_pFile;

		// Save our current location
		long lOffset = dec_ftell( fp );

		// Continue if our location is valid
		if( lOffset != -1 )
		{
			// Before we read ahead, backup one and look for whitespace
			if( dec_fseek( fp, m_nOffset - 1, SEEK_SET ) == 0 )
			{
				unsigned char c[2];

				c[1] = 0;

				// Read the preceding character
				size_t nBytesRead = dec_fread( &c, 1, sizeof( unsigned char ), fp );

				// We should get one byte
				if( nBytesRead == 1 )
				{
					// Is it whitespace?
					if( c[0] == ASC_CHR_SPACE || c[0] == ASC_CHR_TAB )
					{
						// A valid uue line will not be preceded by whitespace
						if( szNewLine )
						{
							free( szNewLine );
						}

						// Reset the file pointer!
						dec_fseek( fp, lOffset, SEEK_SET );

						// Clear the line
						szLine[0] = 0;

						*pfSuccess = false;

						return;
					}
				}
			}

			// Note: We must read beyond the expected length "+1" in order to test for lines that are too long.
			if( *nLineLen + 1 >= ( signed long )nBytesInMultiLineBuffer )
			{
				// Reset our starting location to that of the current token
				if( dec_fseek( fp, m_nOffset, SEEK_SET ) == 0 )
				{
					// Read beyond the current token
					size_t nBytesRead = dec_fread( szNewLine, 1, nBytesToAlloc, fp );

					// If we dont have a file read error
					if( !dec_ferror( fp ) )
					{
						// Copy the bytes to the output buffer
						memcpy( szLine, szNewLine, nBytesRead );
						
						*pfSuccess = true;
					}

				}
			}
		}

		// Reset the file pointer!
		dec_fseek( fp, lOffset, SEEK_SET );

	}
	else
	{
		// < We are in the decode phase. >

		// Is our multiline buffer low on bytes i.e. does it appear to contain only one line or less?
		if( *nLineLen + 1 >= ( signed long )nBytesInMultiLineBuffer )
		{
			// Remember our location in the file
			long lOffset = dec_ftell( lpstInput );

			// Continue if we have a valid location.
			if( lOffset != - 1)
			{
				// We should only need to move back a few bytes. Back up to the start of our line
				// so that we can reread the entire line.
				signed long nBytesBack = - ( ( signed long )*nLineLen + 1 );

				// Backup
				if ( dec_fseek( lpstInput, nBytesBack, SEEK_CUR ) != 0 )
				{
					// Cleanup and return if we are unsuccessful in backing up
					if( szNewLine )
					{
						free( szNewLine );
					}

					*pfSuccess = false;

					return;
				}

				// Try to read more this time
				size_t nBytesRead = dec_fread( szNewLine, 1, nBytesToAlloc, lpstInput );

				if( dec_ferror( lpstInput ) )
				{
					// Cleanup and return if we are unsuccessful
					if( szNewLine )
					{
						free( szNewLine );
					}

					*pfSuccess = false;

					return;
				}

				// Copy the bytes to the output buffer
				memcpy( szLine, szNewLine, nBytesRead );

				// Back up the file pointer to where we were
				if( dec_fseek( lpstInput, lOffset, SEEK_SET ) != 0 )
				{
					// Cleanup and return if we are unsuccessful in backing up
					if( szNewLine )
					{
						free( szNewLine );
					}

					*pfSuccess = false;

					return;
				}

				// Did we get more bytes this time?
				if( nBytesRead <= nBytesInMultiLineBuffer )
				{
					*pfSuccess = false;
				}
			}
		}
	}

	if( szNewLine )
	{
		free( szNewLine );
	}
}

//************************************************************************
//
// Purpose
//
// This function examines a line of uue to determine if the length of the
// line is valid.
//
// Parameters
//
// szBuffer       Pointer to the multiline encode buffer.
// nBytesInBuffer Number of bytes in the encode buffer.
//
// Returns        True if valid, false otherwise.
//
//************************************************************************
bool CUUEParser::IsUULineValid( const unsigned char* szBuffer,
                                const size_t nBytesInMultiLineBuffer,
                                FILE* lpstInput )
{
	unsigned char* szLine = NULL;

	bool fValid        = false;
	bool fSuccess      = false;

	// Safeguard length calculation from rollover on binary data.
	// Character 95 - 32 = 63 or 111111 binary. Character 96 exceeds range and rolls over to 0.
	if( (int)szBuffer[0] > 95 )
	{
		fValid = false;
		return fValid;
	}

	// Decode the first character to get the length
	long nPreEncodedLength = DEC( szBuffer[0] );

	// This is how big we expect a line to be plus one extra line of length 60
	size_t nLineBytes = ( nPreEncodedLength + 1 + 60 ) * sizeof(unsigned char);

	// This is how big we expect our buffer of multiple lines to be. This could be small if our buffer is low.
	size_t nBufferBytes = ( nBytesInMultiLineBuffer + 1 ) * sizeof(unsigned char);

	// This buffer will hold atleast one line.
	int nBytesToAlloc = max( nLineBytes, nBufferBytes );

	// Allocate memory for a local copy of the input buffer
	// 01-14-2004 THURTT -- Adding 1 byte to the allocation to ensure
	// the line is always null terminated.
	szLine = ( unsigned char* )malloc( nBytesToAlloc + (1 * sizeof(unsigned char)));

	// Make sure it's valid
	if( szLine == NULL )
	{
		fValid = false;
		return fValid;
	}

	// Make a copy of the input buffer
	void* pDest = memcpy( szLine, szBuffer, nBytesInMultiLineBuffer );

	// Make sure the copy was successful
	if( pDest == NULL )
	{
		free( szLine );
		fValid = false;
		return fValid;
	}

	long nLineLen = 0;

	// Get the first line of the multi-line buffer. If our buffer is low, this may not be a complete line.
	GetFirstLine( szLine, nBytesInMultiLineBuffer, &nLineLen ); // ( Get the line length for ReadAhead() )

	// Check to see if we have a full line. If not read some more.
	ReadAhead( lpstInput, szLine, nBytesInMultiLineBuffer, &nLineLen, nBytesToAlloc, &fSuccess );

	// 01-14-2004 THURTT -- The following tests prevent the strlen functions
	// called later down the line to count past the buffer.
	if(szLine[nBytesToAlloc] != 0)
	{
		szLine[nBytesToAlloc] = 0;
	}

	// Get the first line of the multi-line buffer
	GetFirstLine( szLine, strlen( (char*)szLine ), &nLineLen );

	// Use the length byte to determine how many bytes it took to encode the original stream.

#if defined( SYM_AIX )

	// Aix does not support Posix floor() and fmod()
	double nNumTri = (int)( nPreEncodedLength / 3 );
	double nEncRemainder = nPreEncodedLength - ( nNumTri * 3 );

#else

	double nNumTri = floor( (double)nPreEncodedLength / 3 );
	double nEncRemainder = fmod( (double)nPreEncodedLength, (double) 3 );

#endif

	long   nEncCalculatedLen = ( (long)nNumTri * 4 );

	if( nEncRemainder > 0 )
	{
		// If we have trailing bytes, add another quad
		nEncCalculatedLen+= 4;
	}

	// We have alread subtracted one from the line length to account for the length character
	if( nLineLen == nEncCalculatedLen )
	{
		fValid = true;
	}
	
	// Some encoders append an extra character to the end of the line
	else if( nLineLen == nEncCalculatedLen + 1 ) // i.e. Regression/378031/qz.uue
	{
		fValid = true;
	}
	else if( nEncRemainder > 0 )
	{
		// Our test set contains a file that breaks the 3 in 4 out rule. When we have a one or two
		// trailing bytes allow for error in the final quad.

		// What is the difference between the actual line length and our calculated length?
		long nDifference = nLineLen - nEncCalculatedLen;

		// Our accuracy is limited to a quad. If our trailing bytes go to zero during encoding as
		// a result of an incomplete tri-byte input, then the actual length may be shorter than
		// expected for some encoders.
		if( nDifference < 0 && nDifference > -4 ) // i.e. Regression/DEC3-R11j/1-1JNIXS/BIN-31-uue.eml
		{
			fValid = true;
		}
	}

	// Release our buffers
	if( szLine )
	{
		free( szLine );
	}

	// Return whether the length is valid
	return fValid;
}

//************************************************************************
// int DecodeUU(lpstInput,
//              nStartOffset,
//              lpstOutput)
//
// Purpose
//
//  This function reads from the input stream (lpstInput) starting at offset
//  nStartOffset and decodes the referenced uuencoded attachment.  It saves
//  the decoded file to lpstOutput, overwriting the old contents of the file.
//
// Parameters:
//
//  lpstInput: IN/OUT=The input (UUE) stream that contains the encoded text
//             stream
//  nStartOffset: IN=The offset of the "begin ..." line in lpstInput stream
//  lpstOutput: IN/OUT=The output stream where the decoded file is saved
//
// Returns:
//
//  PARSER_OK if the data was successfully decoded;
//  PARSER_ERROR otherwise
//************************************************************************
int CUUEParser::DecodeUU(
	FILE *lpstInput,
	size_t nStartOffset,
	FILE *lpstOutput)
{
	int		rc = PARSER_ERROR;
	BYTE	byTri[3];
	size_t	nBytesRead;
	size_t	nTotalBytesRead;
	DWORD	dwExtractedSize = 0;
	bool	bGotEOL;
	bool	bNewLine;
	unsigned char *pinbuffer = NULL;

	bool  bIsUULineValid = false;

	// Seek to the start of the UU-encoded data stream.
	if (dec_fseek(lpstInput, nStartOffset, SEEK_SET) != 0)
		goto decodeuu_done;

	// Seek to the top of the output file stream.
	if (	lpstOutput != NULL
		&&	dec_fseek(lpstOutput, 0, SEEK_SET) != 0)
		goto decodeuu_done;

	// Allocate the input and output buffers that we need.
	pinbuffer = (unsigned char *)malloc(MY_BUFFER_SIZE + 4);
	if (!pinbuffer)
		goto decodeuu_done;

	// Read in/skip over the "begin UNIX-permission-mode filename.ext" line
	if (ReadLine(lpstInput,
				(char *)pinbuffer,
				RFC822_MAX_LINE_LENGTH,
				(LPINT) &nBytesRead,
				&bGotEOL) == LEXER_ERROR)
		goto decodeuu_done;

	if (bGotEOL == false)
		goto decodeuu_done;

	// Reset the terminator type for this UUE section.
	m_nTerminator = TERMINATOR_NONE;

	// Now decode the file.  We do this by reading large blocks of data and
	// decoding the buffer in blocks of 4 bytes (quads).  The 3 output
	// bytes (tri) for each quad are added to an output buffer.  When the
	// output buffer is full or we reach the end of the input data, the
	// output buffer is flushed.
	
	bNewLine = true;
	while (true)
	{
		bool	brc;
		unsigned char *ptr;
		unsigned char uch;
		size_t	nLength = 0;
		size_t	nBytesDecoded;
		long	lOffset;

		nBytesDecoded = 0;

		// Save the current file position.
		lOffset = dec_ftell(lpstInput);
		if (lOffset == -1)
			goto decodeuu_done;	// ftell failed.

		// Get a block of input data.
		nBytesRead = dec_fread(pinbuffer, 1, MY_BUFFER_SIZE, lpstInput);
		if (nBytesRead == 0)
			goto decodeuu_done;	// Read failed.

		nTotalBytesRead = nBytesRead;
		if (nTotalBytesRead <= 3)
		{
			size_t	nCount;

			// Fill the input buffer with enough data to make a quad.
			ptr = &pinbuffer[nTotalBytesRead];
			nCount = 4 - nTotalBytesRead;

			while (nCount)
			{
				*ptr = ASC_CHR_GRAVE;
				ptr++;
				nCount--;
			}

			ptr = pinbuffer;
			nTotalBytesRead = 4;
			nBytesRead = nTotalBytesRead;
			goto do_decode;	// Decode the small amount of data left in the file.
		}

		// Loop through the input data, decoding as we go...
		ptr = pinbuffer;
		while (nBytesRead > 3)
		{
			uch = *ptr;
			nBytesDecoded++;

			// Skip over CR and LF characters.
			if (IsCR(uch) || IsLF(uch))
			{
				bNewLine = true;
				goto decode_next_char;
			}

			if (bNewLine)
			{
				bNewLine = false;

				// This is the first character on a new line.  If it is a '`'
				// character, there should be 0 encoded bytes on the line.
				// So, we need to skip over the rest of the data on the line.
				nLength = 0;
				if (uch == ASC_CHR_GRAVE)
					goto decode_next_char;

				// Look for "end" - case insensitive.
				if ((*ptr == 0x65 || *ptr == 0x45) &&
					(*(ptr + 1) == 0x6E || *(ptr + 1) == 0x4E) &&
					(*(ptr + 2) == 0x64 || *(ptr + 2) == 0x44))
				{
					// Make a note that the encoded data is terminated by
					// the word "end".
					m_nTerminator = TERMINATOR_END;

					// This is the end of the encoded data.
					// Since the large read that we did above most likely
					// read past this point, we need to back up the file
					// pointer.
					if (dec_fseek(lpstInput, lOffset + nBytesDecoded - 2, SEEK_SET) == 0)
					{
						// Return whatever has been decoded up to this point.
						rc = PARSER_OK;
					}

					goto decodeuu_done;
				}

				// This character represents the number of bytes of encoded
				// data that follow it on the line.
				// Make sure the length character is in range.
				if (uch < ASC_CHR_SPACE ||
					uch > ASC_CHR_GRAVE)
				{
					goto decode_next_char;
				}

				nLength = (size_t)(uch - ASC_CHR_SPACE);

				bIsUULineValid = false;

				// If and only if we have a new line, do we validate the entire line here.
				bIsUULineValid = IsUULineValid( ptr, nBytesRead, lpstInput );

				goto decode_next_char;
			}

do_decode:
			// Note that the number of bytes available here at ptr is
			// guaranteed to be at least 4.  Skip over any extraneous data
			// on the line once we have decoded the number of bytes specified
			// by the first character on the line.  Note that some UUE encoders
			// put extra information on the end of each line.
			if (nLength)
			{
				// Check for case where input data contains less than 24-bits (4 bytes)
				// remaining on the line.
				if (nLength < 3)
				{
					char	ch;
					unsigned char byQuad[4];
					size_t	nIndex = 0;

					while (nIndex < 4)
					{
						ch = ptr[nIndex];
						if (ch == ASC_CHR_CR ||
							ch == ASC_CHR_LF)
							byQuad[nIndex] = 0x60;
						else
							byQuad[nIndex] = ch;
						nIndex++;
					}

					// *************************************************************
					// In-line version of previous DecodeUUQuadToTri function.

					byQuad[0] = m_uulookup[byQuad[0]];
					byQuad[1] = m_uulookup[byQuad[1]];
					byQuad[2] = m_uulookup[byQuad[2]];
					byQuad[3] = m_uulookup[byQuad[3]];

					brc = true;
					if (byQuad[0] == 0xff || byQuad[1] == 0xff || byQuad[2] == 0xff || byQuad[3] == 0xff)
					{
						brc = false;
					}
					else
					{
						byTri[0] = (byQuad[0] << 2) | (byQuad[1] >> 4);
						byTri[1] = (byQuad[1] << 4) | (byQuad[2] >> 2);
						byTri[2] = (byQuad[2] << 6) | byQuad[3];
					}
					// *************************************************************

					nBytesDecoded += nLength;

					if (nBytesRead >= 2)
						nBytesRead -= 2;
					else
						nBytesRead = 0;

					ptr += 2;

					if (brc == false)
					{
						nLength = 0;
						goto decode_next_char;
					}

					goto quad_decoded;
				}

				// *************************************************************
				// In-line version of previous DecodeUUQuadToTri function.

				ptr[0] = m_uulookup[ptr[0]];
				ptr[1] = m_uulookup[ptr[1]];
				ptr[2] = m_uulookup[ptr[2]];
				ptr[3] = m_uulookup[ptr[3]];

				brc = true;
				if (ptr[0] == 0xff || ptr[1] == 0xff || ptr[2] == 0xff || ptr[3] == 0xff)
				{
					brc = false;
				}
				else
				{
					byTri[0] = (ptr[0] << 2) | (ptr[1] >> 4);
					byTri[1] = (ptr[1] << 4) | (ptr[2] >> 2);
					byTri[2] = (ptr[2] << 6) | ptr[3];
				}
				// *************************************************************

				nBytesDecoded += 3;

				if (nBytesRead >=3)
					nBytesRead -= 3;
				else
					nBytesRead = 0;

				ptr += 3;  // The logic at decode_next_char below will increment
						  // ptr once more for a total of 4.

				if (brc == false)
				{
					nLength = 0;
					goto decode_next_char;
				}

quad_decoded:

				if( bIsUULineValid ) // Filter invalid embedded text.
				{
					if ( lpstOutput != NULL
						&&	dec_fwrite(byTri,
								  sizeof(BYTE),
								  nLength > 3 ? 3 : nLength,
								  lpstOutput) != (nLength > 3 ? 3 : nLength))
						goto decodeuu_done;
				}

				if (nLength > 3)
				{
					dwExtractedSize += 3;
					nLength -= 3;
				}
				else
				{
					dwExtractedSize += nLength;
					nLength = 0;
				}

				rc = PARSER_OK;	// We successfully decoded something.
			}

			if (g_bTextAbort)
			{
				rc = PARSER_USER_ABORT;
				goto decodeuu_done;
			}

			if (g_dwTextMaxExtractSize &&
				dwExtractedSize >= g_dwTextMaxExtractSize)
			{
				rc = PARSER_MAX_EXTRACT_SIZE;
				goto decodeuu_done;
			}

decode_next_char:
			nBytesRead--;
			ptr++;

		}

		if (nBytesRead != 0)
		{
			// The input buffer did not end up decoding on an
			// even decoding boundary.  There are some bytes left
			// that we still need to look at.
			// Back up the input file a few bytes to read them back in if/when
			// we loop back to the top.
			if (dec_fseek(lpstInput, lOffset + nTotalBytesRead - nBytesRead, SEEK_SET) != 0)
				goto decodeuu_done;

			if (nBytesRead >= 3)
			{
				// Look for "end" - case insensitive.
				if ((*ptr == 0x65 || *ptr == 0x45) &&
					(*(ptr + 1) == 0x6E || *(ptr + 1) == 0x4E) &&
					(*(ptr + 2) == 0x64 || *(ptr + 2) == 0x44))
				{
					// This is the end of the encoded data.
					// Return whatever has been decoded up to this point.
					break;
				}
			}
		}

	}

	rc = PARSER_OK;

decodeuu_done:
	if (pinbuffer)
		free(pinbuffer);

	return (rc);
}


//************************************************************************
// int SkipDecodeUU(lpstInput,
//                  nStartOffset)
//
// Purpose
//
//  This function reads from the input stream (lpstInput) starting at offset
//  nStartOffset and decodes the referenced uuencoded attachment.
//  It does NOT write the decoded data, but simply skips over it in
//  the input file.
//
// Parameters:
//
//  lpstInput: IN/OUT=The input (UUE) stream that contains the encoded text
//             stream
//  nStartOffset: IN=The offset of the "begin ..." line in lpstInput stream
//
// Returns:
//
//  true if the contents of lpszQuad are valid uuencode characters
//  false otherwise
//************************************************************************
int CUUEParser::SkipDecodeUU(FILE *lpstInput, size_t nStartOffset)
{
	return ( DecodeUU(lpstInput,nStartOffset,NULL) );
}


//************************************************************************
// void EncodeTriToUUQuad(lpbyTri, lpszQuad)
//
// Purpose
//
//  This function encodes three 8-bit bytes into 4 valid BASE64 characters.
//
// Parameters:
//
//  lpbyTri: IN=Three byte, 8-bit binary input array
//  lpszQuad: OUT=Four character, 7-bit uuencoded text
//
// Returns:
//
//  nothing
//************************************************************************
inline void EncodeTriToUUQuad(LPBYTE lpbyTri, char *lpszQuad)
{
	BYTE byQuad[4];
	int	i;

	byQuad[0] = lpbyTri[0] >> 2;
	byQuad[1] = ((lpbyTri[0] & 0x3) << 4) | (lpbyTri[1] >> 4);
	byQuad[2] = ((lpbyTri[1] & 0xf) << 2) | (lpbyTri[2] >> 6);
	byQuad[3] = lpbyTri[2] & 0x3F;

	for (i = 0; i < 4; i++)
	{
		if (byQuad[i] == 0)
			lpszQuad[i] = ASC_CHR_GRAVE;
		else
			lpszQuad[i] = byQuad[i] + ASC_CHR_SPACE;	// space
	}
}


//************************************************************************
// int EncodeUU(lpstInput,
//              lpstOutput,
//              lpszFileName)
//
// Purpose
//
//  This function uuencodes the specified input stream and writes the results
//  to the specified output stream.  It will create a "begin 644 filename"
//  line using the specified filename (lpszFileName).  It will always use 644
//  for permission bits.
//
// Parameters:
//
//  lpstInput:IN/OUT=Input file to be uuencoded
//  lpstOutput:IN/OUT=Output file where data will be written
//  lpszFileName:IN=Filename of attachment to uuencode
//
// Returns:
//
//  PARSER_OK if the file is properly encoded
//  PARSER_ERROR if there was an error encoding the file
//************************************************************************
int CUUEParser::EncodeUU(FILE *lpstInput, FILE *lpstOutput,
						const char *lpszFileName)  // no path!
{
	BYTE  byTri[3];
	char  szLine[RFC822_MAX_LINE_LENGTH+10];
	int	 i, nTri, nStringIndex, nChar, nInputBytes;

	if (dec_fseek(lpstInput, 0, SEEK_SET) != 0)
		return (PARSER_ERROR);

	// write out "begin UNIX-permission-mode filename.ext" line
	// dec_fprintf(lpstOutput, "begin 644 %s\r\n", lpszFileName) == EOF)
	if (dec_fputs(ASC_CHR_begin_644_, lpstOutput) == EOF ||
		dec_fputs(lpszFileName, lpstOutput) == EOF ||
		dec_fputc(ASC_CHR_CR, lpstOutput) == EOF ||
		dec_fputc(ASC_CHR_LF, lpstOutput) == EOF)
	{
		return (PARSER_ERROR);
	}

	while (!dec_feof(lpstInput))
	{
		szLine[0] = 0;
		byTri[0] = byTri[1] = byTri[2] = 0;
		nStringIndex = 1;
		nTri = 0;
		nInputBytes = 0;

		for (i = 0; i < UU_MAX_OCTETS_PER_LINE && !dec_feof(lpstInput); i++)
		{
			nChar = dec_fgetc(lpstInput);
			if (nChar != EOF)
			{
				nInputBytes++;
				byTri[nTri++] = nChar;
				if (nTri == 3)
				{
					EncodeTriToUUQuad(byTri, szLine+nStringIndex);
					nStringIndex += 4;
					nTri = 0;
					byTri[0] = byTri[1] = byTri[2] = 0;
				}
			}
		}

		if (nTri != 0)
		{
			// hit the EOF before hitting EOL...
			EncodeTriToUUQuad(byTri, szLine+nStringIndex);

			// We need to always write 4 bytes to the output file, so if
			// we did not process 3 bytes above we still need to account for
			// the missing bytes when nStringIndex is updated.
			if (nTri == 1 || nTri == 2)
				nStringIndex += 4;
		}

		if (nStringIndex > 1)
		{
			// We still have some data to write out.

			szLine[nStringIndex] = 0;
			szLine[0] = nInputBytes + ASC_CHR_SPACE;

			// dec_fprintf(lpstOutput, "%s\r\n", szLine) == EOF)
			if (dec_fputs(szLine, lpstOutput) == EOF ||
				dec_fputc(ASC_CHR_CR, lpstOutput) == EOF ||
				dec_fputc(ASC_CHR_LF, lpstOutput) == EOF)
			{
				return (PARSER_ERROR);
			}
		}

		if (dec_feof(lpstInput))
		{
			if (m_nTerminator != TERMINATOR_GRAVE)
			{
				//if (dec_fprintf(lpstOutput, "`\r\nend\r\n") == EOF)
				if (dec_fputc(ASC_CHR_GRAVE, lpstOutput) == EOF ||
					dec_fputc(ASC_CHR_CR, lpstOutput) == EOF ||
					dec_fputc(ASC_CHR_LF, lpstOutput) == EOF)
				{
					return (PARSER_ERROR);
				}
			}

			// We *never* output the word "end" as part of the encoding
			// because there really isn't a good way to make sure that
			// we can locate the original true end of the encoded data.
			// So, instead we simply assume that whatever terminated the
			// encoded data before is fine.
		}
	}

	return (PARSER_OK);
}


//************************************************************************
// int WriteUU(lpstFile,
//             nStartOffset,
//             lpszFileName)
//
// Purpose
//
//  This function opens the specified output file and truncates its contents.
//  It then proceeds to decode a UU-encoded section of the file that starts
//  at dwCur.  Finally, it closes the output file.
//
// Parameters:
//
//  dwCur: IN=Start of the uuencoded section
//  lpstFile: IN/OUT=UUE data stream
//  lpszFileName: IN=Filename of the file to store the attachment to
//
// Returns:
//
//  PARSER_OK if the UU-encoded section is properly decoded
//  PARSER_ERROR otherwise
//************************************************************************
int CUUEParser::WriteUU(
	FILE *lpstFile,
	size_t nStartOffset,
	const char *lpszFileName)
{
	FILE		*stream;
	int			rc;

	stream = dec_fopen(lpszFileName,"wb");
	if (!stream)
		return (PARSER_ERROR);

	rc = DecodeUU(lpstFile,
				 nStartOffset,
				 stream);
	if (EOF == dec_fclose(stream))
		rc = PARSER_ERROR;

	if (rc != PARSER_OK)
	{
		// kill the file!
		dec_remove(lpszFileName);
	}

	stream = NULL;
	return (rc);
}


//************************************************************************
// int ReadLine(lpstFile,
//              lpszData,
//              nBufferSize,
//              lpnLineLength,
//              lpbGotEOL)
//
// Purpose
//
//  This function reads the next line from the input stream.
//
// Parameters:
//
//  lpstFile: IN/OUT=Input stream containing period-stripped UUE content.
//  lpszData: OUT=Place the line in this buffer.
//  nBufferSize: IN=Max allowable length of line, not including zero
//               terminator.
//  lpnLineLength: OUT=Actual length of line read from file.
//  lpbGotEOL: OUT=Tells whether or not there was a CRLF at the end of the
//             line or whether we hit the EOF.
//
// Returns:
//
//  PARSER_OK if all is OK
//  PARSER_ERROR if an error occurs.
//
//  Errors: Bad character in UUE stream, line too long.
//************************************************************************
int CUUEParser::ReadLine(FILE *lpstFile, char *lpszData, int nBufferSize,
						LPINT lpnLineLength, bool *lpbGotEOL)
{
	int		i = 0;
	int		ch;
	char	cData = '\0';

	while (1)
	{
		// Get the next character from the file.
		ch = dec_fgetc(lpstFile);
		if (ch == EOF)
		{
			// Hit EOF
			lpszData[i] = 0;
			*lpnLineLength = i;
			*lpbGotEOL = false;
			return (PARSER_OK);
		}

		// Make sure it's a valid character!
		cData = (char)ch;
		if (cData == '\0')
			continue;  // Skip over NULLs.

		//
		// If we see either a CR or LF, we need to take
		// some special action:
		//
		// '\x0D' (CR)
		// If the next character is a LF, then we will consume it
		//
		// '\x0A' (LF)
		// Terminal state - just consume LF
		//
		// If the next character is neither, then it must belong
		// to the next line in the file, so NULL-terminate what
		// we have so far and return without consuming the next
		// character.
		//
		// Allowable line termination sequences:
		// CR
		// CRLF
		// LF
		//
		if (cData == ASC_CHR_LF)
		{
			// Terminate the string.
			lpszData[i] = 0;

			// Return with the string, and we *DID* hit the EOL.
			*lpnLineLength = i;
			*lpbGotEOL = true;
			return (PARSER_OK);
		}

		if (cData == ASC_CHR_CR)
		{
			// Peek at the next character
			ch = dec_fgetc(lpstFile);
			if (ch == EOF)
			{
				// Hit EOF.  Terminate the string, return it and we *DID NOT* hit the EOL.
				lpszData[i] = 0;
				*lpnLineLength = i;
				*lpbGotEOL = false;
				return (PARSER_OK);
			}

			// If the next character is not a LF, back up the file pointer one byte.
			cData = (char)ch;
			if (cData != ASC_CHR_LF)
			{
				// Back up the file position one byte.
				if (dec_fseek(lpstFile, -1, SEEK_CUR) != 0)
				{
					// Terminate the string.
					lpszData[i] = 0;
					return (PARSER_ERROR);
				}
			}

			// Terminate the string.
			lpszData[i] = 0;

			// Return with the string, and we *DID* hit the EOL.
			*lpnLineLength = i;
			*lpbGotEOL = true;
			return (PARSER_OK);
		}

		// If we have any more room in our buffer, insert the character.
		if (i < nBufferSize)
		{
			lpszData[i++] = cData;

			// Added to ensure EOF returns immediately.
			if (dec_feof(lpstFile) != 0)
			{
			  lpszData[i] = 0;
			  *lpnLineLength = i;
			  *lpbGotEOL = false;
			  return (PARSER_OK);
			}
		}
		else
		{
			// We have exceeded our buffer length.  NULL-terminate and return.
			lpszData[i] = 0;
			*lpnLineLength = i;
			*lpbGotEOL = false;
			return (PARSER_OK);
		}
	}
}

