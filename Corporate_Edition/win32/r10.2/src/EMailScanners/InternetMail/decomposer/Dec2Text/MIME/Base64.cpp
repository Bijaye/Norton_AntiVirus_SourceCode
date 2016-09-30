// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright 2000, 2005 Symantec, Peter Norton Product Group. All rights reserved.
//************************************************************************
//
// Description:
//
// Contains:
//  InBase64Range()
//  Decode64QuadToTri()
//  DecodeBase64()
//  Encode64TriToQuad()
//  EncodeBase64()
//  WriteBase64()
//  DecodeBase64Buffer()
//  EncodeBase64Buffer()
//
//************************************************************************

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

#include "MIME.h"
#include "asc_ctype.h"
#include "dectmpf.h"

// NOTE: This definition must be a multiple of 4.
#define BASE64_DECODE_BUFFER_SIZE		(8 * 1024)

extern volatile DWORD g_dwTextMaxExtractSize;
extern volatile bool g_bTextAbort;

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

// 0 = invalid, 1 = decodable, 2 = white space
static const int g_nBase64CharTypeTable[256] = 
{
   0,  0,  0,  0,	0,	0,	0,	0,	  0,  2,  2,  0,	0,	2,	0,	0,
   0,  0,  0,  0,	0,	0,	0,	0,	  0,  0,  0,  0,	0,	0,	0,	0,
   2,  0,  0,  0,	0,	0,	0,	0,	  0,  0,  0,  1,	0,	0,	0,	1, 
   1,  1,  1,  1,	1,	1,	1,	1,	  1,  1,  0,  0,	0,	1,	0,	0,
   0,  1,  1,  1,	1,	1,	1,	1,	  1,  1,  1,  1,	1,	1,	1,	1,
   1,  1,  1,  1,	1,	1,	1,	1,	  1,  1,  1,  0,	0,	0,	0,	0,
   0,  1,  1,  1,	1,	1,	1,	1,	  1,  1,  1,  1,	1,	1,	1,	1, 
   1,  1,  1,  1,	1,	1,	1,	1,	  1,  1,  1,  0,	0,	0,	0,	0,
   0,  0,  0,  0,	0,	0,	0,	0,	  0,  0,  0,  0,	0,	0,	0,	0,
   0,  0,  0,  0,	0,	0,	0,	0,	  0,  0,  0,  0,	0,	0,	0,	0,
   0,  0,  0,  0,	0,	0,	0,	0,	  0,  0,  0,  0,	0,	0,	0,	0, 
   0,  0,  0,  0,	0,	0,	0,	0,	  0,  0,  0,  0,	0,	0,	0,	0,
   0,  0,  0,  0,	0,	0,	0,	0,	  0,  0,  0,  0,	0,	0,	0,	0, 
   0,  0,  0,  0,	0,	0,	0,	0,	  0,  0,  0,  0,	0,	0,	0,	0,
   0,  0,  0,  0,	0,	0,	0,	0,	  0,  0,  0,  0,	0,	0,	0,	0, 
   0,  0,  0,  0,	0,	0,	0,	0,	  0,  0,  0,  0,	0,	0,	0,	0
};


#define MIMEInBase64Range(nChar) (g_nBase64CharTypeTable[nChar] == 1)

#define XX 127
static const int g_Base64Reverse[256] = 
{
   XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX,
   XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX,
   XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,62, XX,XX,XX,63,
   52,53,54,55, 56,57,58,59, 60,61,XX,XX, XX, 0,XX,XX,
   XX, 0, 1, 2,	3, 4, 5, 6,  7, 8, 9,10, 11,12,13,14,
   15,16,17,18, 19,20,21,22, 23,24,25,XX, XX,XX,XX,XX,
   XX,26,27,28, 29,30,31,32, 33,34,35,36, 37,38,39,40,
   41,42,43,44, 45,46,47,48, 49,50,51,XX, XX,XX,XX,XX,
   XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX,
   XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX,
   XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX,
   XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX,
   XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX,
   XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX,
   XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX,
   XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX,
};



//************************************************************************
// size_t MIMEDecode64QuadToTri(lpszQuad, lpbyTri)
//
// Purpose
//
//  This function takes a 4-byte character array that contains valid BASE64
//  characters and produces a 3-byte BYTE array with the decoded contents.
//  The *lpnOutputLen value is changed to specify how many bytes should be
//  decoded and written, based on the contents of lpszQuad.
//
// Parameters:
//
//  lpszQuad: IN=The 4-byte array to convert (All valid low-ascii)
//  lpbyTri: OUT=The 3-bytes of decoded data
//  lpnOutputLen: OUT=The number of bytes to be written to the decoded file
//
// Returns:
//
//  The number of decoded bytes.
//************************************************************************

inline static size_t MIMEDecode64QuadToTri(
	char				*lpszQuad,
	LPBYTE				lpbyTri)
{
	int nOutputLen = 3;

	if (lpszQuad[0] == ASC_CHR_EQ)
		--nOutputLen;
	if (lpszQuad[1] == ASC_CHR_EQ)
		--nOutputLen;
	if (lpszQuad[2] == ASC_CHR_EQ)
		--nOutputLen;
	if (lpszQuad[3] == ASC_CHR_EQ)
		--nOutputLen;
	if (nOutputLen <= 0)
		return ((size_t)0);

	lpszQuad[0] = g_Base64Reverse[lpszQuad[0]];
	lpszQuad[1] = g_Base64Reverse[lpszQuad[1]];
	lpszQuad[2] = g_Base64Reverse[lpszQuad[2]];
	lpszQuad[3] = g_Base64Reverse[lpszQuad[3]];

	lpbyTri[0] = (lpszQuad[0] << 2) | (lpszQuad[1] >> 4);
	lpbyTri[1] = (lpszQuad[1] << 4) | (lpszQuad[2] >> 2);
	lpbyTri[2] = (lpszQuad[2] << 6) | lpszQuad[3];

	return ((size_t)nOutputLen);
}


//************************************************************************
// int DecodeBase64(lpstInput,
//                  dwStartOffset,
//                  dwEndOffset,
//                  lpstOutput)
//
// Purpose
//
//  This function decodes a BASE64 stream of data starting at offset
//  dwStartOffset and ending at dwEndOffset.  It writes the output file to
//  lpstOutput.
//
// Parameters:
//
//  lpstInput: IN/OUT=The MIME input stream
//  dwStartOffset: IN=Start offset of the encoded BASE64 data in the MIME
//                 stream
//  dwEndOffset: IN=End offset of the encoded BASE64 data in the MIME stream
//  lpstOutput: IN/OUT=The destination stream where to write our data
//
// Returns:
//
//  PARSER_OK if the BASE64 section is properly decoded
//  PARSER_ERROR otherwise
//************************************************************************
int CMIMEParser::DecodeBase64(
	FILE	*lpstInput,
	DWORD	dwStartOffset,
	DWORD	dwEndOffset,
	FILE	*lpstOutput)
{
	bool			bGotTerminator = false;
	char			szQuad[4];
	int				nCount;
	int				nType;
	int				nIndex;
	int				rc = PARSER_ERROR;	// Assume error.
	size_t			nLen;
	unsigned char	uch;
	size_t			nTotalBytesWritten = 0;
	size_t			nBytesWritten = 0;
	size_t			nWriteBufferIndex;
	size_t			nBytesLeft;
	size_t			nBytesToRead;
	unsigned char	*pbufferin = NULL;
	unsigned char	*pbufferout = NULL;

	// The end offset is inclusive so bump by 1
	dwEndOffset++;

	// Make sure the end offset is not before the start offset.
	if (dwEndOffset < dwStartOffset)
	{
		rc = PARSER_OK;
		goto decode_done;
	}

	// Seek to the start of the BASE64 data to be decoded.
	if (dec_fseek(lpstInput, dwStartOffset, SEEK_SET) != 0)
		goto decode_done;

	// Seek to the TOF in the output stream.
	if (dec_fseek(lpstOutput, 0, SEEK_SET) != 0)
		goto decode_done;

	// Allocate a read buffer.
	pbufferin = (unsigned char *)malloc(BASE64_DECODE_BUFFER_SIZE);
	if (!pbufferin)
		goto decode_done;

	// Allocate a write buffer.
	pbufferout = (unsigned char *)malloc(BASE64_DECODE_BUFFER_SIZE);
	if (!pbufferout)
		goto decode_done;

	nBytesLeft = dwEndOffset - dwStartOffset;
	nCount = 0;
	while (nBytesLeft)
	{
		// Fill the read buffer.
		nBytesToRead = BASE64_DECODE_BUFFER_SIZE;
		if (nBytesToRead > nBytesLeft)
			nBytesToRead = nBytesLeft;

		if (dec_fread(pbufferin, 1, nBytesToRead, lpstInput) != nBytesToRead)
			goto decode_done;

		nBytesLeft -= nBytesToRead;

		// Loop through the buffer, decoding as we go...
		nIndex = 0;
		nWriteBufferIndex = 0;
		while (nBytesToRead)
		{
			uch = pbufferin[nIndex++];
			if (uch == ASC_CHR_EQ)
				bGotTerminator = true;

			nType = g_nBase64CharTypeTable[uch];
			if (nType == 1)	// decodable
			{
				szQuad[nCount++] = uch;
				if (nCount == 4)
				{
					if (nWriteBufferIndex > BASE64_DECODE_BUFFER_SIZE - 6)
					{
						// The output buffer is full so flush it.
						nBytesWritten = dec_fwrite(pbufferout, 1, nWriteBufferIndex, lpstOutput);
						if (nBytesWritten != nWriteBufferIndex)
							goto decode_done;

						nWriteBufferIndex = 0;
					}

					nCount = 0;
					nLen = MIMEDecode64QuadToTri(szQuad, &pbufferout[nWriteBufferIndex]);
					if (nLen)
					{
						nWriteBufferIndex += nLen;
						nTotalBytesWritten += nLen;
				
						// Does this object violate our MaxExtractSize policy?
						if (g_dwTextMaxExtractSize &&
							nTotalBytesWritten >= g_dwTextMaxExtractSize)
						{
							rc = PARSER_MAX_EXTRACT_SIZE;
							goto decode_done;
						}
							
						// Should we abort?
						if (g_bTextAbort)
						{
							rc = PARSER_USER_ABORT;
							goto decode_done;
						}

						if (bGotTerminator)
						{
							nBytesLeft = 0;
							break;
						}
					}
				}
			}
			else if (nType == 0) // illegal or EOF
			{
				// There is a non-decodable character in the BASE64 stream.
// *** NOTE *** Removed this logic in order to match the way MS-Outlook does its
//              base64 decoding.  It skips over any non-decodable characters.
//              // Stop decoding, but return whatever we have decoded so far.
//              rc = PARSER_OK;
//              goto decode_done;
			}

			nBytesToRead--;
		}

		if (nWriteBufferIndex > 0)
		{
			// Write the decoded output buffer.
			nBytesWritten = dec_fwrite(pbufferout, 1, nWriteBufferIndex, lpstOutput);
			if (nBytesWritten != nWriteBufferIndex)
				goto decode_done;

			nWriteBufferIndex = 0;
		}
	}

	rc = PARSER_OK;

decode_done:
	if (pbufferin)
		free(pbufferin);
	if (pbufferout)
		free(pbufferout);
	return (rc);
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
//************************************************************************

static inline void MIMEEncode64TriToQuad
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

//************************************************************************
// int EncodeBase64(lpstInput, lpstOutput)
//
// Purpose
//
//  This function encodes the specified input stream as a BASE64 attachment
//  and writes the output to the current write pointer in the lpstOutput
//  stream.  The calling function should make sure to seek to the proper
//  location in the output stream before invoking this function.
//
// Parameters:
//
//  lpstInput: IN/OUT=Input file stream
//  lpstOutput: IN/OUT=Output file stream, with file position set by calling
//              function
//
// Returns:
//
//  PARSER_OK on successful encoding
//  PARSER_ERROR if an error occurs during encoding
//************************************************************************
int CMIMEParser::EncodeBase64(FILE *lpstInput, FILE *lpstOutput)
{
	char	szLine[BASE64_LINE_LEN+1];
	BYTE	byTri[3] = { 0 };
	int		nCount = 0, nStringIndex = 0;

	// seek to start of input file
	if (dec_fseek(lpstInput,0,SEEK_SET) != 0)
		return (PARSER_ERROR);
   
	while (dec_feof(lpstInput) == 0)
	{
		nCount = dec_fread(byTri, 1, 3, lpstInput);
		if (nCount == 3)
		{
			// encode the 3 into 4 valid BASE64 characters and add them
			// to our output line
			MIMEEncode64TriToQuad(byTri,szLine + nStringIndex);
		
			nCount = 0;
			nStringIndex += 4;
			byTri[0] = byTri[1] = byTri[2] = 0;

			if (nStringIndex == BASE64_LINE_LEN)
			{
				// Once we have reached the proper line length, output the
				// BASE64 line to the output file.
				szLine[nStringIndex] = 0;
				if (dec_fputs(szLine, lpstOutput) == EOF ||
					dec_fputc(ASC_CHR_CR, lpstOutput) == EOF ||
					dec_fputc(ASC_CHR_LF, lpstOutput) == EOF)
					return (PARSER_ERROR);
		
				// time to start on the next line
				nStringIndex = 0;
			}
		}
		else break;
	}

	// see if we have a partially full byTri (< 3 bytes).  add it to the
	// current line.
	if (nCount != 0)
	{
		MIMEEncode64TriToQuad(byTri, szLine + nStringIndex);
		if (nCount == 1)
			nStringIndex += 2;
		else if (nCount == 2)
			nStringIndex += 3;
	}

	// if there is a non-empty last line, write it out
	if (nStringIndex != 0)
	{
		while (nStringIndex % 4 != 0)
			szLine[nStringIndex++] = ASC_CHR_EQ; /* = */

		szLine[nStringIndex] = 0;
		if (dec_fputs(szLine, lpstOutput) == EOF ||
			dec_fputc(ASC_CHR_CR, lpstOutput) == EOF ||
			dec_fputc(ASC_CHR_LF, lpstOutput) == EOF)
			return (PARSER_ERROR);
	}

	return (PARSER_OK);
}


//************************************************************************
// int WriteBase64(dwCur,
//                 dwNext,
//                 lpstFile,
//                 lpszOutputFileName)
//
// Purpose
//
//  This function opens the specified output file and truncates its contents.
//  It then proceeds to decode a BASE64 encoded section of the file that starts
//  at dwCur and ends at dwNext.  Finally, it closes the output file.
//
// Parameters:
//
//  dwCur: IN=Start of the BASE64 section
//  dwNext: OUT=Start of the next section (end of the base64 section)
//  lpstFile: IN/OUT=MIME data stream
//  lpszOutputFileName: IN=Filename of the file to store the attachment to
//
// Returns:
//
//  PARSER_OK if the BASE64 section is properly decoded
//  PARSER_ERROR otherwise
//************************************************************************
int CMIMEParser::WriteBase64(
	DWORD		dwCur,
	DWORD		dwNext,
	FILE		*lpstFile,
	const char	*lpszOutputFileName)
{
	FILE		*stream;
	int			rc;

	stream = dec_fopen(lpszOutputFileName,"wb");
	if (!stream)
		return (PARSER_ERROR);

	rc = DecodeBase64(lpstFile,
					 dwCur,
					 dwNext,
					 stream);
	if (EOF == dec_fclose(stream))
		rc = PARSER_ERROR;

	if (rc != PARSER_OK)
	{
		// kill the file!
		dec_remove(lpszOutputFileName);
	}

	stream = NULL;
	return (rc);
}


//************************************************************************
// DWORD DecodeBase64Buffer(char *pszInput,
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
DWORD CMIMEParser::DecodeBase64Buffer (char *pszInput, DWORD dwSize, char *pszOutput, DWORD dwOutputBufferSize)
{
	char				szQuad[4];
	BYTE				byTri[3];
	int					nChar, nCount;
	size_t				index;
	size_t				nLen;
	DWORD				dwOutputCount = 0;

	nCount = 0;

	while (dwSize > 0)
	{
		nChar = (int)*pszInput++;
		nChar &= 0x00FF;
		int nType = g_nBase64CharTypeTable[nChar];
		if (nType == 1)	 // decodable Base64
		{
			szQuad[nCount++] = nChar;

			if (nCount == 4)
			{
				nCount = 0;

				nLen = MIMEDecode64QuadToTri(szQuad, byTri);
				if (nLen == 0)
					return (0);

				for (index = 0; index < nLen; index++)
				{
					*pszOutput++ = byTri[index];
					dwOutputCount++;
					if (dwOutputBufferSize < (dwOutputCount + 1))
						return (0);
				}
			}
		}
		else if (nType == 0) // non-decodable, non-whitspace
		{
			return (0);
		}

		dwSize--;
	}

	// Append a NULL-terminator.
	*pszOutput++ = '\0';

	return (dwOutputCount);
}


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
DWORD CMIMEParser::EncodeBase64Buffer (char *pszInput, DWORD dwSize, char *pszOutput, DWORD dwOutputBufferSize)
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
		byTri[nTri++] = nChar;
		if (nTri == 3)
		{
			// OK, have a complete set of three characters.
			// Encode the trio into 4 bytes and write them to the output.
			EncodeTriToUUQuad (byTri, pszOutput);
			pszOutput += 4;
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
		// write it to the output.
		EncodeTriToUUQuad (byTri, pszOutput);
		pszOutput += 4;
		dwOutputCount += 4;
	}

	return (dwOutputCount);
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
void CMIMEParser::EncodeTriToUUQuad(LPBYTE lpbyTri, char *lpszQuad)
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
