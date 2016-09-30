// Support.cpp
// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright 2000, 2005 by Symantec Corporation.  All rights reserved.

/////////////////////////////////////////////////////////////////////////////
// Headers

// Fix the defect in IBM's header files for V3.x of Visual Age for C++.
#include "fixibmps.h"

#include "dec_assert.h"
#include <ctype.h>

// Project headers
#include "DecPlat.h"
#include "Decstdio.h"
#include "Support.h"
#include "asc_char.h"

#define SUPPORT_BUFFER_SIZE (1024 * 32)

//
// Local function prototypes
//
static void shift_add(DWORD *low, DWORD *high, int bits, DWORD num);
static void big_multiply(DWORD num1, DWORD num2, DWORD *low, DWORD *high);
static DWORD big_divide(DWORD low, DWORD high, DWORD divisor);
static int highest_one_bit(DWORD val);
static void shift_subtract(DWORD *low, DWORD *high, int bits, DWORD num);

//
// Some defines we need
//
#define EOS ((char)0)

#if !defined PATH_SEP
	#if defined (_WINDOWS)
		#define PATH_SEP	ASC_CHR_FSLASH
	#else
		#define PATH_SEP	ASC_CHR_BSLASH
	#endif
#endif

#if !defined MB_LEN_MAX
	#define MB_LEN_MAX MB_CUR_MAX 
#endif

#if !defined MAX_PATH
	#define MAX_PATH PATH_MAX 
#endif

//
// EBCDIC-friendly fgets function...
//
char *ascii_fgets(char *s, int n, FILE *stream)
{
	int c;
	char *p = s;

	if(n <= 0) 
	{
		dec_assert(0);
		return NULL;
	}

	while(1) 
	{
		if(n == 1) 
		{
			*p = 0;
			return s;
		}

		c = dec_fgetc(stream);
		if(c == EOF) 
		{
			*p = 0;
			return NULL;
		}

		*p++ = c;
		--n;

		if(c == '\x0A') 
		{
			*p = 0;
			return s;
		}
	}
}


//*************************************************************************************************
// bool Support_CopyFile(const char *pszExistingFile, const char *pszNewFile, bool bFailIfExists)
//
// Purpose
//
//  This function emulates the Win32 function CopyFile, but uses the Dec3 stream support I/O
//  functions.  I put it here because CopyFile is used by several different engines not to 
//  mention all of our test harnesses as well.
//
//  The implementation was adapted from the shim layer but was changed quite a bit.
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
//*************************************************************************************************

bool Support_CopyFile(const char *pszExistingFileName, const char *pszNewFileName, bool bFailIfExists)
{
	FILE *pExistingFile = NULL;
	FILE *pNewFile = NULL;
	size_t byte_count;

	const size_t buffer_size = SUPPORT_BUFFER_SIZE;
	BYTE *buffer = NULL;   
	bool rc = false;

	//
	// Validate input...
	//
	if(!pszExistingFileName || !pszNewFileName) 
	{
		dec_assert(0);
		return rc;
	}

	if((strlen(pszExistingFileName) == 0) || (strlen(pszNewFileName) == 0))
	{
		dec_assert(0);
		return rc;
	}

	//
	// Allocate memory...
	//
	buffer = new BYTE[buffer_size];

	if(!buffer) 
	{
		return rc;
	}

	if(bFailIfExists) 
	{
		//
		// If it already exists then bail...
		//
		if(dec_access(pszNewFileName, 0) == 0)
		{
			goto bailOut;
		}
	}

	//
	// Otherwise, it doesn't matter if the if the file exists or not.
	// Just open in truncation mode...
	//
	pNewFile = dec_fopen(pszNewFileName, "wb");
	
	//
	// If we can't open it, then bail...
	//
	if(!pNewFile)
	{
		goto bailOut;
	}

	//
	// Try to open the existing file...
	//
	pExistingFile = dec_fopen(pszExistingFileName, "rb");

	if(!pExistingFile) 
	{
		//
		// Unable to open the existing file
		//
		goto bailOut;	
	}

	while(true) 
	{
		byte_count = dec_fread(buffer, 1, buffer_size, pExistingFile);

		if(byte_count == 0) 
		{
			break;
		}

		if(dec_fwrite(buffer, 1, byte_count, pNewFile) != byte_count) 
		{
			break;
		}
	}

	//
	// Check for errors...
	//
	if(dec_ferror(pNewFile) || dec_ferror(pExistingFile)) 
	{
		goto bailOut;
	}

	rc = true;

bailOut:
	//
	// Clean up...
	//
	if(buffer)
	{
		delete [] buffer;
		buffer = NULL;
	}

	if(pNewFile)
	{
		if(EOF == dec_fclose(pNewFile))
		{
			rc = false;
		}

		pNewFile = NULL;
	}

	if(pExistingFile)
	{
		if(EOF == dec_fclose(pExistingFile))
		{
			rc = false;
		}

		pExistingFile = NULL;
	}

	return rc;
}


//*************************************************************************************************
// bool Support_SetEndOfFile(const char *pszOriginalFilename, 
//                           FILE *pOriginalFile,
//                           const char *pszOriginalMode)
//
// Purpose
//
//  This function emulates the Win32 function SetEndOfFile, but uses the Dec3 stream support I/O
//  functions.  There is no portable way to do this.  Unix offers ftruncate, but Windows doesn't.
//  Even if they did, we'd still need to implement it as a dec_XXX function. 
//
//  The only portable way to implement this function is to create a temporary file, copy as much
//  data as is requested from the original file (indicated by where the file pointer is currently
//  set in the original file), close the original file, delete it, then rename the temp file 
//  back to the name of the original.  Yes, this will be very slow, but this
//  function is only used by SymLHA and then only when doing a change/replace/delete operation,
//  so the performance effects can be mitigated somewhat.
//
//  I don't know what happens if you set the new EOF beyond the end of the original file
//  (if you are extending the original file).  The Win32 version says "This function can be used 
//  to truncate or extend a file. If the file is extended, the contents of the file between the 
//  old EOF position and the new position are not defined."  That being the case, I will fill
//  this are with a buffer of characters (garbage) to make sure that we extend the file correctly.
//
// Parameters:
//
//  pszOriginalFilename - Original filename
//  pOriginalFile - Pointer to the file to operate on.
//  pszOriginalMode - Mode that pOriginalFile was opened in.
//
// Returns:
//
//  true for success
//  false on error
//
//*************************************************************************************************

bool Support_SetEndOfFile(const char *pszOriginalFilename, FILE *pOriginalFile, const char *pszOriginalMode)
{ 
	////////////////////////////////////////////////////////////
	// This function is left here only as a reference in case
	// we need to roll our own version for a platform that 
	// doesn't have ftruncate or chsize.  As of this writing
	// this function has been replaced by dec_ftruncate and 
	// should not be used at all!
	//
	// 4/2/01
	// Chris Benson 
	//
	dec_assert(0);
	return false;
	//
	////////////////////////////////////////////////////////////

	size_t sizeBuffer = SUPPORT_BUFFER_SIZE;
	size_t sizeByteCount = 0;
	long lBytesNeededFromOriginalFile = 0;
	bool rc = false;
	BYTE *pBuffer = NULL;
	FILE *pTempFile = NULL;
   
	char pszTempFilename[2 * L_tmpnam];
	char *pszFilenameNoSlash = pszTempFilename;

	//
	// Validate input...
	//
	if(!pOriginalFile || !pszOriginalFilename) 
	{
		dec_assert(0);
		return rc;
	}

	//
	// Allocate memory...
	//
	pBuffer = new BYTE[sizeBuffer];

	if(!pBuffer) 
	{
		return rc;
	}

	//
	// Create a temporary filename
	//
	if(!tmpnam(pszTempFilename))
	{
		goto bailOut;
	}

	//
	// Ignore leading slashes
	//
	if('\\' == pszTempFilename[0] || '/' == pszTempFilename[0])
	{
		pszFilenameNoSlash++;
	}

	//
	// Open the temporary file. 
	//
	// IMPLEMENTATION NOTE:  Support_SetEndOfFile always creates its temp file on disk.
	//                       This is because we don't have a dec_tmpnam function.  It
	//                       wouldn't buy us a whole lot, but if we ever do want to make
	//                       this function work with the in-memory file system, we'd need
	//                       to implement this.
	if(!(pTempFile = dec_fopen(pszFilenameNoSlash, "w+b")))
	{
		goto bailOut;
	}

	//
	// Next, determine where our file pointer is - this may actually be beyond the
	// end of the file, in which case we are being asked to extend the file. In either
	// case, this tells us how much data to write to the temp file...It can also be the
	// case that we don't need to do anything...
	//
	if(-1 == (lBytesNeededFromOriginalFile = dec_ftell(pOriginalFile)))
	{
		goto bailOut;
	}

	//
	// Special case:  If lBytesNeededFromOriginalFile == 0, then we don't need to 
	// do anything except delete the original file and rename the new one...
	//
	if(0 == lBytesNeededFromOriginalFile)
	{
		goto truncateOnly;
	}

	//
	// Seek back to the start of the original file
	//
	if(0 != dec_fseek(pOriginalFile, 0, SEEK_SET))
	{
		goto bailOut;
	}

	//
	// Copy the original file into the temp file...
	//
	while(true)
	{
		//
		// Set the buffer size correctly...we don't want to copy more than was requested...
		//
		sizeBuffer = lBytesNeededFromOriginalFile > SUPPORT_BUFFER_SIZE ? 
													SUPPORT_BUFFER_SIZE : 
													lBytesNeededFromOriginalFile;

		
		lBytesNeededFromOriginalFile -= (sizeByteCount = dec_fread(pBuffer, 1, sizeBuffer, pOriginalFile));

		if(sizeByteCount == 0) 
		{
			break;
		}

		if(dec_fwrite(pBuffer, 1, sizeByteCount, pTempFile) != sizeByteCount) 
		{
			break;
		}
	}

	//
	// At this point, if we were being asked to truncate the file we are done.  Otherwise,
	// we are extending the file, so we'll need to write out some number of bytes to the
	// temp file (lBytesNeededFromOriginalFile tells us how many).
	//
	if(lBytesNeededFromOriginalFile > 0)
	{
		//
		// IMPLEMENTATION NOTE: We could zero out this buffer here, but there really is no
		//                      good reason to do that.  The spec says that everything
		//                      between the original EOF and the new one is undefined.
		//
		while(lBytesNeededFromOriginalFile > 0)
		{	 
			sizeBuffer = lBytesNeededFromOriginalFile > SUPPORT_BUFFER_SIZE ? 
														SUPPORT_BUFFER_SIZE : 
														lBytesNeededFromOriginalFile;

			//
			// Write out the buffer to the temp file...
			//
			if(dec_fwrite(pBuffer, 1, sizeBuffer, pTempFile) != sizeBuffer) 
			{
				goto bailOut;
			}

			lBytesNeededFromOriginalFile -= sizeBuffer;
		}
	}

	//
	// Make sure there were no errors...
	//
	if(dec_ferror(pTempFile) || dec_ferror(pOriginalFile)) 
	{
		goto bailOut;
	}

truncateOnly:
	//
	// The last step is the delete the original file and rename the temp file to the
	// original filename...
	//
	if(EOF == dec_fclose(pOriginalFile))
	{
		goto bailOut;
	}

	//
	// Close the temp file too
	//
	if(EOF == dec_fclose(pTempFile))
	{
		goto bailOut;
	}

	pTempFile = NULL;
	pOriginalFile = NULL;
	
	if(0 == dec_remove(pszOriginalFilename))
	{
		if(!Support_CopyFile(pszFilenameNoSlash, pszOriginalFilename, false))
		{
			goto bailOut;
		}

		//
		// Reopen the new file and save the file pointer in pOriginalFile...
		//
		if(!(pOriginalFile = dec_fopen(pszOriginalFilename, pszOriginalMode)))
		{
			goto bailOut;
		}

		//
		// Seek to the new EOF position in the new file
		//
		if(0 != dec_fseek(pOriginalFile, 0, SEEK_END))
		{
			goto bailOut;
		}
	}

	else
	{
		goto bailOut;
	}

	rc = true;

bailOut:
	//
	// Clean up...
	//
	if(pBuffer)
	{
		delete [] pBuffer;
		pBuffer = NULL;
	}

	if(pTempFile)
	{
		if(EOF == dec_fclose(pTempFile))
		{
			rc = false;
		}

		pTempFile = NULL;
	}

	dec_remove(pszFilenameNoSlash);

	return rc;
}


WORD Support_GetWord (LPWORD pWord)
{
	BYTE *ptr;
	BYTE byHigh, byLow;
	WORD retval;

	ptr = (BYTE *)pWord;
	byHigh = *ptr++;
	byLow = *ptr;
	retval = ((WORD)byHigh * 256) + byLow;
	return (retval);
}


void Support_SetWord (LPWORD pWord, WORD wValue)
{
	BYTE *ptr;
	BYTE byHigh, byLow;

	byHigh = (BYTE)(wValue >> 8);
	byLow = (BYTE)(wValue & 0x00ff);
	ptr = (BYTE *)pWord;
	*ptr++ = byHigh;
	*ptr = byLow;
}

DWORD Support_GetDword (LPDWORD pDword)
{
	DWORD dwRetVal = 0;
	BYTE *ptr = (BYTE *)pDword;
	BYTE byHighHigh = *ptr++;
	BYTE byHighLow = *ptr++;
	BYTE byLowHigh = *ptr++;
	BYTE byLowLow = *ptr;

	dwRetVal = ((((DWORD)byHighHigh << 24) & 0xff000000) |
				(((DWORD)byHighLow	<< 16) & 0x00ff0000) |
				(((DWORD)byLowHigh	<<	8) & 0x0000ff00) |
				(((DWORD)byLowLow)		  & 0x000000ff));

	return dwRetVal;
}


void Support_SetDword (LPDWORD pDword, DWORD dwValue)
{
	BYTE *ptr;
	BYTE byHighHigh, byHighLow;
	BYTE byLowHigh, byLowLow;
	
	byHighHigh = (BYTE)(dwValue >> 24);
	byHighLow = (BYTE)(dwValue >> 16);
	byLowHigh = (BYTE)(dwValue >> 8);
	byLowLow = (BYTE)(dwValue & 0x000000ff);
	ptr = (BYTE *)pDword;
	*ptr++ = byHighHigh;
	*ptr++ = byHighLow;
	*ptr++ = byLowHigh;
	*ptr = byLowLow;
}


//************************************************************************
// DWORD Support_FileSize(FILE *stream)
//
// Purpose
//
//  This function returns the file length of the specified stream.
//
// Parameters:
//
//  lpstStream: IN=Input stream
//
// Returns:
//
//  File size or (DWORD)-1 for error
//
//************************************************************************

DWORD Support_FileSize (FILE *stream)
{
	DWORD	 dwOldOff, dwNewOff;

	//
	// Do some input validation...
	//
	if(!stream)
	{
		return ((DWORD)-1);
	}

	dwOldOff = dec_ftell(stream);
	if (dwOldOff == (DWORD)-1)
	{
		return ((DWORD)-1);
	}

	if (dec_fseek(stream, 0, SEEK_END) != 0)
	{
		return ((DWORD)-1);
	}

	dwNewOff = dec_ftell(stream);
	if (dwNewOff == (DWORD)-1)
	{
		dec_fseek(stream, dwOldOff, SEEK_SET);
		return ((DWORD)-1);
	}

	if (dec_fseek(stream, dwOldOff, SEEK_SET) != 0)
	{
		return ((DWORD)-1);
	}

	return (dwNewOff);
}


//************************************************************************
// bool Support_GetNextToken(lpszString,
//                           lpszSeparators,
//                           lpnIndex,
//                           lpnLength,
//                           lpszToken,
//                           nMaxLen)
//
// Purpose
//
//  This function retrieves the next token from lpszString, using
//  lpszSeparators as a delimiters.  *lpnIndex is set to index the character
//  after the returned token.  lpszToken is filled with the token.  *lpnLength
//  is set to the length of the token.
//
// Parameters:
//
//  lpszString: The string to tokenize
//  lpszSeparators: Zero terminated string of separator characters
//  lpnIndex: IN=Index pointing to first character to start tokenizing from
//            OUT=Updated index pointing to character after tokenized word
//  lpnLength: OUT=Length of tokenized item
//  lpszToken: OUT=Token
//  nMaxLen:   IN=Max allowable length of token, not including zero terminator.
//
// Returns:
//  true if a token is retrieved
//  false if the EOS has been reached and no token was available
//
//************************************************************************

bool Support_GetNextToken
(
	char			*lpszString,
	char			*lpszSeparators,
	LPINT			lpnIndex,
	LPINT			lpnLength,
	char			*lpszToken,
	int				nMaxLen
)
{
	//
	// Do some input validation...
	//
	if(!lpszString || !lpszSeparators || !lpszToken || !lpnIndex || !lpnLength)
	{
		return (false);
	}

	lpszString += *lpnIndex;
	*lpnLength = 0;

	// locate non-separator character

	while (*lpszString && strchr(lpszSeparators,*lpszString))
	{
		lpszString++;
		(*lpnIndex)++;
	}

	// if we're not at the EOS, grab our token!

	if (*lpszString)
	{
		while (*lpszString && !strchr(lpszSeparators,*lpszString) &&
			  *lpnLength < nMaxLen)
		{
			*lpszToken++ = *lpszString++;
			(*lpnLength)++;
			(*lpnIndex)++;

		}

		*lpszToken = 0;
		return (true);
	}
	else
	{
		*lpnLength = 0;
		return (false);
	}
}


// Support_UnixTimeToFileTime
//
//    Description:
//
//      Convert a UNIX time_t to a Windows FILETIME. In order to simplify the
//      math, the conversion is only guaranteed accurate to within a 15 second
//      interval.
//
//      This is not a Windows API, but it is used both elsewhere in this module
//      and in other modules.
//
//    Arguments:
//
//      unix_time:               A pointer to UNIX time value, which is the 
//                               number of seconds that have elapsed since 
//                               midnight on January 1, 1970.
//
//      file_time:               A pointer to a Windows FILETIME structure, which
//                               contains two DWORDs. Together, these make a 64-
//                               bit value, which is the number of 100 nanosecond
//                               intervals since midnight on January 1, 1601.
//
//    Return value:
//
//      None. 

void Support_UnixTimeToFileTime(time_t *unix_time, LPFILETIME file_time)
{
	time_t ut;

	const time_t s_minute = 4;			// In units of 15 seconds 
	const time_t s_hour = s_minute * 60;
	const time_t s_day = s_hour * 24;
	const time_t s_year = s_day * 365;
	const time_t s_leapyear = s_year + s_day;
	const time_t s_hundred_years = (s_year * 76) + (s_leapyear * 24);
	const time_t s_sixty_nine_years = (s_year * 52) + (s_leapyear * 17);

	if(unix_time == NULL)
	{
		dec_assert(0);
		return;
	}

	if(file_time == NULL)
	{
		dec_assert(0);
		return;
	}

	ut = *unix_time;

	//
	// Scale down the UNIX time value to simplify the math a little 
	//
	ut /= 15;

	//
	// Convert from a value relative to 1970 to a value relative to 1601 
	//
	ut += ((3 * s_hundred_years) + s_sixty_nine_years);

	//
	// Multiply by 150,000,000 to scale it from 15 second intervals to
	// 100 nanosecond intervals. The result will be a 64-bit quantity that is
	// placed directly into the FILETIME struct.
	//
	big_multiply(ut, 150000000, &(file_time->dwLowDateTime), &(file_time->dwHighDateTime));
}


// DosDateTimeToFileTime
//
//    Description:
//
//      Convert MS-DOS date and time values to a FILETIME instance.
//
//    Arguments:
//
//      wFatDate:                The MS-DOS date, which is a date value packed
//                               into a 16-bit quantity. The bit fields are as
//                               follows:
//
//                               0-4:   Day of month (1-31)
//
//                               5-8:   Month (1-12)
//
//                               9-15:  Year offset from 1980. (Add 1980 to this
//                                      value to get the actual year.)
//
//      wFatTime:                The MS-DOS time, which is a time value packed
//                               into a 16-bit quantity. The bit fields are as
//                               follows:
//
//                               0-4:   Second divided by 2
//
//                               5-10:  Minute (0-59)
//
//                               11-15: Hour (0-23)
//
//      lpFileTime:              Pointer to a FILETIME structure to receive the
//                               converted date/time. This structure has the
//                               following members:
//
//                               dwLowDateTime
//                               dwHighDateTime
//
//                               Together, these make up a single 64-bit number
//                               which is the quantity of 100 nanosecond
//                               intervals that have elapsed since January 1,
//                               1601.
//
//    Return value:
//
//      Returns true on success, false on failure.

bool Support_DosDateTimeToFileTime(WORD wFatDate, WORD wFatTime, LPFILETIME lpFileTime)
{
	int month;
	int day;
	int year;
	int hour;
	int minute;
	int second;
	struct tm ts;
	time_t unix_time;

	if(lpFileTime == NULL)
	{
		dec_assert(0);
		return false;
	}

	month = (wFatDate & 0x01E0) >> 5;
	day = wFatDate & 0x001F;
	year = ((wFatDate & 0xFE00) >> 9) + 1980;
	hour = (wFatTime & 0xF800) >> 11;
	minute = (wFatTime & 0x07E0) >> 5;
	second = (wFatTime & 0x001F) * 2;

	if((month < 1) || (month > 12)) 
	{
		return false;
	}

	if((day < 1) || (day > 31)) 
	{
		return false;
	}

	//
	// Based on my calculations, the greatest date that can be represented
	// in POSIX is some time in 2037, but not all dates in 2037 can be thus
	// represented. For our purposes, reject any year greater than 2036. If
	// this assertion fires, your DOS date time cannot be represented in UNIX,
	// and can therefore not be used by this API. 
	//
	if((year < 0) || (year > 2036)) 
	{
		return false;
	}

	if((hour < 0) || (hour > 23)) 
	{
		return false;
	}

	if((minute < 0) || (minute > 59))
	{
		return false;
	}

	if((second < 0) || (second > 61))
	{
		return false;
	}

	ts.tm_sec = second;
	ts.tm_min = minute;
	ts.tm_hour = hour;
	ts.tm_mday = day;
	ts.tm_mon = month - 1;
	ts.tm_year = year - 1900;
	ts.tm_wday = 0;
	ts.tm_yday = 0;
	ts.tm_isdst = -1;

	unix_time = mktime(&ts);

	if(unix_time == (time_t)-1) 
	{
		return false;
	}

	Support_UnixTimeToFileTime(&unix_time, lpFileTime);

	return true;
}


// Support_FileTimeToUnixTime
//
//    Description:
//
//      Convert a Windows FILETIME to a UNIX time_t value. To simplify the
//      math, the conversion is only accurate to within 15 seconds.
//
//    Arguments:
//
//      file_time:               The FILETIME to convert. This represents a
//                               64-bit value, which is the number of 100
//                               nanosecond intervals since midnight on
//                               January 1, 1601.
//
//      unix_time:               A pointer to the time_t where the result will
//                               be placed. This value will be a UNIX time,
//                               which is the number of seconds since midnight
//                               on January 1, 1970.
//
//    Return value:
//
//      None. 

void Support_FileTimeToUnixTime(const FILETIME *file_time, time_t *unix_time)
{
	time_t ut;

	const time_t s_minute = 4;				// In units of 15 seconds 
	const time_t s_hour = s_minute * 60;
	const time_t s_day = s_hour * 24;
	const time_t s_year = s_day * 365;
	const time_t s_leapyear = s_year + s_day;
	const time_t s_hundred_years = (s_year * 76) + (s_leapyear * 24);
	const time_t s_sixty_nine_years = (s_year * 52) + (s_leapyear * 17);

	if(file_time == NULL) 
	{
		dec_assert(0);
		return;
	}

	if(unix_time == NULL) 
	{
		dec_assert(0);
		return;
	}

	//
	// Scale down by 150,000,000. This puts the value in terms of 15 second
	// intervals instead of 100 nanosecond intervals. 
	//
	ut = (time_t)big_divide(file_time->dwLowDateTime, file_time->dwHighDateTime, 150000000);

	//
	// Move it in reference from the year 1601 to 1970. 
	//
	ut -= ((3 * s_hundred_years) + s_sixty_nine_years);

	//
	// Scale it back up to seconds 
	//
	ut *= 15;

	//
	// Store the result
	//
	*unix_time = ut;
}


// Support_FileTimeToDosDateTime
//
//    Description:
//
//      Convert a FILETIME instance to MS-DOS date and time values.
//
//    Arguments:
//
//      lpFileTime:              A pointer to a FILETIME structure with the time
//                               and date value that will be converted. This
//                               structure has the following members:
//
//                               dwLowDateTime
//                               dwHighDateTime
//
//                               Together, these make up a single 64-bit number
//                               which is the quantity of 100 nanosecond
//                               intervals that have elapsed since January 1,
//                               1601.
//
//      lpFatDate:               A pointer to a 16-bit word that will receive the
//                               converted MS-DOS date. The bit fields are as
//                               follows:
//
//                               0-4:   Day of month (1-31)
//
//                               5-8:   Month (1-12)
//
//                               9-15:  Year offset from 1980. (Add 1980 to this
//                                      value to get the actual year.)
//
//      lpFatTime:               A pointer to a 16-bit word that will receive the
//                               converted MS-DOS time. The bit fields are as
//                               follows:
//
//                               0-4:   Second divided by 2
//
//                               5-10:  Minute (0-59)
//
//                               11-15: Hour (0-23)
//
//    Return value:
//
//      Returns nonzero on success, zero on failure. 

bool Support_FileTimeToDosDateTime(const FILETIME *lpFileTime, LPWORD lpFatDate, LPWORD lpFatTime)
{
	time_t ut;
	struct tm ts;
	int hr, mn, sc, m, d, y;

	if(lpFileTime == NULL) 
	{
		dec_assert(0);
		return false;
	}

	if(lpFatDate == NULL) 
	{
		dec_assert(0);
		return false;
	}

	if(lpFatTime == NULL) 
	{
		dec_assert(0);
		return false;
	}

	//
	// Convert the FILETIME to a UNIX time 
	//
	Support_FileTimeToUnixTime(lpFileTime, &ut);

	//
	// Convert that to a struct tm 
	//
	ts.tm_hour = 0;
	ts.tm_min = 0;
	ts.tm_sec = 0;
	ts.tm_mon = 1;
	ts.tm_mday = 1;
	ts.tm_year = 1980;

#if !defined TLS_REENTRANCY && !defined _WINDOWS
	localtime_r(&ut, &ts);
#else
	struct tm *pmytime;

	pmytime = localtime(&ut);
	if (pmytime)
		ts = *pmytime;
#endif

	//
	// Package it up into the DOS format 
	//
	hr = ts.tm_hour;
	mn = ts.tm_min;
	sc = ts.tm_sec / 2;
	m = ts.tm_mon + 1;
	d = ts.tm_mday;
	y = ts.tm_year - 80;

	if(y < 0) 
	{
		//
		// This date is too far back 
		//
		return false;
	}

	*lpFatDate = ((y << 9) & 0xFE00) | ((m << 5) & 0x01E0) | (d & 0x001F);
	*lpFatTime = ((hr << 11) & 0xF800) | ((mn << 5) & 0x07E0) | (sc & 0x001F);

	return true;
}


// ***************************Support_FileTimeToLocalFileTime********************
//
//   Support_FileTimeToLocalFileTime function converts a file time based on the
//   Coordinated Universal Time (UTC) to a local file time. 
//
// Input Parameter:lpFileTime,      Pointer to UTC file time to convert 
//                 lpLocalFileTime  Pointer to converted file time 
//
// Return Type: bool, true for success, false for failure 
//
// ******************************************************************************

bool Support_FileTimeToLocalFileTime(const FILETIME *filetime, LPFILETIME localfiletime)
{
	//
	// Our implementation is a pass-through. There are portability problems
	// with implementing this, and server products should know nothing of a 
	// specific time zone. 
	//
	*localfiletime = *filetime;    
	return true;
}


// shift_add
//
//    Description:
//
//      Shift a number left a given number of bits (with 64-bit resolution),
//      and add the result with carry to an existing 64-bit number. This is
//      part of our implementation of a portable 64-bit multiplication
//      function.
//
//    Arguments:
//
//      low:                     The low-order DWORD of the accumulator.
//
//      high:                    The high-order DWORD of the accumulator.
//
//      bits:                    The number of bits to shift.
//
//      num:                     The number to shift, then add.
//
//    Return value:
//
//      None. 

static void shift_add(DWORD *low, DWORD *high, int bits, DWORD num)
{
	DWORD val_low;
	DWORD val_high;
	DWORD mask;
	DWORD original_low;

	//
	// 64-bit shift left by bits 
	//
	mask = ((1 << bits) - 1) << (32 - bits);
	val_high = (num & mask) >> (32 - bits);
	val_low = num << bits;

	//
	// 64-bit addition. The bit masking is necessary to remain portable for
	// weird cases where a DWORD might actually be larger than 32-bits in size.
	//
	original_low = (*low & 0xFFFFFFFF);
	*low = ((*low + val_low) & 0xFFFFFFFF);

	if(*low < original_low)
	{
		// Overflow detected. Carry one.
		++val_high;
	}

	*high += val_high;
}


// big_multiply
//
//    Description:
//
//      Multiplies two DWORDs and stores the result in a 64-bit pair. This is
//      done in (as much as possible) a portable way.
//
//      This function takes advantage of the distributive property of
//      multiplication that says:
//
//       x * (a + b + c ...) = xa + xb + xc ...
//
//      64-bit multiplication is easiest to do in powers of two by shifting
//      a 64-bit quantity left by some number of bits b to multiply by 2 ^ b.
//
//      All we need to do, then, is determine what quantities of b are necessary
//      to make up num2, and then repeatedly shift num1 left and add the result
//      to a 64-bit accumulator using an add-with-carry. This determination is
//      easy, since the bits of num2 are, in effect, the answer.
//
//    Arguments:
//
//      num1:                    The first number to multiply.
//
//      num2:                    The second number to multiply.
//
//      low:                     Pointer to the low DWORD for the result.
//
//      high:                    Pointer to the high DWORD for the result.
//
//    Return value:
//
//      None.

static void big_multiply(DWORD num1, DWORD num2, DWORD *low, DWORD *high)
{
	int i;
	DWORD mask;

	dec_assert(low != NULL);
	dec_assert(high != NULL);

	*low = 0;
	*high = 0;

	for(i = 0; i < 32; ++i) 
	{
		mask = 1 << i;

		if((num2 & mask) != 0) 
		{
			shift_add(low, high, i, num1);
		}
	}
}


// big_divide
//
//    Description:
//
//      Divide a 64-bit quantity by a 32-bit quantity and return the 32-bit
//      result. The assumption is that the result will fit into 32 bits. If that
//      is not true, this function raises an assertion failure in debug builds.
//
//      This function uses the distributive property of division that says:
//
//        (a + b + c ...) / y = (a / y) + (b / y) + (c / y) ...
//
//      Based on this property, we conclude that if we can find some large
//      quantity k, which is less than the dividend, and which can be divided
//      evenly by the divisor resulting in some known value q, we can reduce
//      the problem by subtracting k from the dividend and accumulating q
//      such that it will eventually be added to the result. Thus, we convert
//      the problem:
//
//       x / y
//
//      into the problem:
//
//      ((x - k) / y) + (k / y) or...
//
//      ((x - k) / y) + q
//
//      Now let x become the quantity x - k. We can repeat the process by
//      choosing a new k, constrained by the new x, subtracting it from the new
//      x, while adding the quantity k / y to our accumulator q. This again
//      results in the form:
//
//      ((x - k) / y) + q becoming
//      (x / y) + q
//
//      where x is now smaller and q is now larger. If we repeat this process
//      enough times, we eventually have a value of x that can be represented
//      in 32 bits. At that time we can get the result by dividing this by
//      the divisor and adding the result to our accumulated q.
//
//      Because we don't have a 64-bit division primitive, the problem now
//      becomes how to choose a value of k with a known quotient q which is
//      sufficiently large that we don't have to repeat the process too many
//      times. This is easily done by shifting the divisor left by some
//      number of bits n, and calculating q as 2 ^ n. If we always choose n
//      such that our resulting highest 1 bit in k is in the position just lower
//      than the highest one bit in the dividend, we can then subtract this k
//      either 1 or 2 times in order to eliminate that 1 bit in the dividend.
//      Next we repeat the process, shifting up to just under the next highest
//      one bit in the dividend. Eventually we eliminate all of the 1 bits in
//      the high DWORD. When the high DWORD is zero we can complete the process
//      by simple division and addition.
//
//    Arguments:
//
//      low:                     The low DWORD of the dividend
//
//      high:                    The high DWORD of the dividend
//
//      divisor:                 The divisor.
//
//    Return value:
//
//      The function returns the result of the divison, which the caller must
//      be able to guarantee will fit into a 32-bit DWORD. 

static DWORD big_divide(DWORD low, DWORD high, DWORD divisor)
{
	DWORD q;
	DWORD this_q;
	int divisor_highbit;
	int bits;

	if(divisor == 0) 
	{
		//
		// Division by zero!
		//
		dec_assert(0);
		return 0xFFFFFFFF;
	}

	if(high >= divisor) 
	{
		//
		// Overflow--the result won't fit into 32 bits 
		//
		dec_assert(0);
		return 0xFFFFFFFF;
	}

	q = 0;
	divisor_highbit = highest_one_bit(divisor);

	while(high != 0) 
	{
		bits = highest_one_bit(high) + 32;

		bits -= divisor_highbit;
		--bits;

		if(bits > 31) 
		{
			//
			// Overflow--the result won't fit into 32 bits 
			//
			dec_assert(0);
			return 0xFFFFFFFF;
		}

		shift_subtract(&low, &high, bits, divisor);

		this_q = 1 << bits;

		if(q > (0xFFFFFFFF - this_q))
		{
			//
			// Overflow--the result won't fit into 32 bits 
			//
			dec_assert(0);
			return 0xFFFFFFFF;
		}

		q += this_q;
	}

	this_q = low / divisor;

	if(q > (0xFFFFFFFF - this_q)) 
	{
		//
		// Overflow--the result won't fit into 32 bits 
		//
		dec_assert(0);
		return 0xFFFFFFFF;
	}

	return (q + this_q);
}


// highest_one_bit
//
//    Description:
//
//      Find the position of the highest bit in the given value that is set.
//      For example, for 1, this value is 0. For 7 this value is 2. This can
//      also be viewed as the greatest value of n for which it is true that
//      (2 ^ n) <= val.
//
//    Arguments:
//
//      val:                     The value for which to find the highest one bit.
//
//    Return value:
//
//      The bit position of the highest one bit, or the greatest n for which it
//      holds that (2 ^ n) <= val. 

static int highest_one_bit(DWORD val)
{
	int rc;
	DWORD mask;

	dec_assert(val != 0);

	rc = 31;
	mask = 1 << rc;

	while(1) 
	{
		if((val & mask) != 0)
		{
			return rc;
		}

		mask = (mask >> 1);
		dec_assert(mask != 0);

		--rc;
		dec_assert(rc >= 0);
	}
}


// shift_subtract
//
//    Description:
//
//      Shift a number left by some quantity of bits, and then subtract the
//      result from a 64-bit quantity. The shift is done with 64-bit resolution.
//      this is part of our implementation of a portable 64-bit divide operation
//      for platforms that don't support 64-bit math.
//
//    Arguments:
//
//      low:                     A pointer to the low-order DWORD of the
//                               accumulator. The result of the shift is
//                               subtracted from this accumulator, and the result
//                               of this operation is stored back into it.
//
//      high:                    A pointer to the high-order DWORD of the
//                               accumulator.
//
//      bits:                    The number of bits to shift our number left
//                               before subtracting. This shift is done with 64
//                               bit resolution.
//
//      num:                     The number that will be shifted and then
//                               subtracted from the accumulator.
//
//    Return value:
//
//      None. 

static void shift_subtract(DWORD *low, DWORD *high, int bits, DWORD num)
{
	DWORD val_low;
	DWORD val_high;
	DWORD mask;
	DWORD original_low;

	//
	// 64-bit shift left by bits 
	//
	mask = ((1 << bits) - 1) << (32 - bits);
	val_high = (num & mask) >> (32 - bits);
	val_low = num << bits;

	//
	// 64-bit subtract. The bit masking is necessary to remain portable in
	// weird cases where a DWORD might be larger than 32 bits in size. 
	//
	original_low = *low & 0xFFFFFFFF;
	*low = ((*low - val_low) & 0xFFFFFFFF);

	if(*low > original_low) 
	{
		//
		// Overflow detected. Borrow one. 
		//
		++val_high;
	}

	*high -= val_high;
}


//
//  Support_memicmp
//
//    Description:
//
//      Compares characters in two buffers (case insensitive).
//
//    Arguments:
//
//      buf1:               The first buffer.
//
//      buf2:               The second buffer.
//
//      count:              The number of elements of type 'char' to compare.
//                          Note that the buffers point to untyped memory. By
//                          definition, this interface works on bytes, which is
//                          a nonportable way to define it. It's not totally
//                          correct to assume that sizeof(char) is always 1,
//                          though this will typically be true. We will define
//                          the interface in terms of units of char, since a
//                          case-insensitive comparison can only really mean a
//                          character comparison.
//
//    Return value:
//
//      Returns a value less than zero if the contents of buf1 are less than the
//      contents of buf2 in a character-by-character, case-insensitive comparison
//      with the earlier characters having higher precedence.
//
//      Returns zero if the contents of the buffers are equal, ignoring case
//      differences.
//
//      Returns a value greater than zero if the contents of buf1 are greater
//      than the contents of buf2, ignoring case differences.
//
//      Note that a case-insensitive comparison cannot, in theory, always work
//      correctly in all locales. There is not always a one-to-one mapping
//      between lowercase and uppercase letters, and the issue of mbcs characters
//      remains unsolved since tolower/toupper deal with a single character at
//      a time. The best solution to this problem is not to do case-insensitive
//      comparisons. Be aware that functions like this one will only see
//      characters as equal that have a one-to-one match upper-to-lowercase,
//      excluding multibyte characters, and that they will use the code page
//      order as the collating order by definition. Use of this function should
//      be highly discouraged in any software that must be internationally
//      aware. 
//

int Support_memicmp(const void *buf1, const void *buf2, unsigned int count)
{
	char *b1 = (char*)buf1;
	char *b2 = (char*)buf2;

	//
	// Check arguments 
	//
	if((b1 == NULL) || (b2 == NULL)) 
	{
		dec_assert(0);
		return 0;
	}

	//
	// No-op condition--two empty strings are always equal 
	//
	if(count == 0) 
	{
		return 0;
	}

	//
	// Do the actual comparison 
	//
	while(count != 0) 
	{
		if(tolower(*b1) < tolower(*b2)) 
		{
			return -1;
		}

		if(tolower(*b1) > tolower(*b2)) 
		{
			return 1;
		}

		++b1;
		++b2;
		--count;
	}

	return 0;
}


//
//  Support_GetTempPath
//
//    Description:
//
//      Returns the path to the temporary folder on the system.
//
//    Arguments:
//
//      nBufferLength:      The length of lpBuffer
//
//      lpBuffer:           The buffer to hold the path
//
//
//    Return value:
//      The number of characters copied to lpBuffer.  
//      Zero if it failed.
//

DWORD Support_GetTempPath(DWORD nBufferLength, char *lpBuffer)
{
#if defined (_WINDOWS)
	return GetTempPath(nBufferLength, lpBuffer);
#elif defined (UNIX)
	const char *dir;
	size_t len;

#ifndef NO_P_TMPDIR
	dir = P_tmpdir;
#else
	dir = "/tmp";
#endif

	len = strlen(dir);
	strcpy(lpBuffer, dir);

	return len;
#elif defined (SYM_NLM)

	#pragma message("Support_GetTempPath(): Confirm this is correct for Netware!")

	const char *dir;
	size_t len;

	dir = "SYS:tmp";
	len = strlen(dir);
	strcpy(lpBuffer, dir);
	return len;
#else
	#error ("***PORTABILITY ERROR: Support_GetTempPath() not defined on this platform!");
#endif
}


/*
	Support_strlwr

	Description:

	 Converts a string to lowercase.

	Arguments:

	 string:			 The string to convert to lowercase

	Return value:

	 This function returns a pointer to the altered string. Since the string
	 is altered in place, the return value is the same as 'string'. No return
	 value is reserved to indicate an error. 
*/

char * Support_strlwr(char *string)
{
#if defined (UNIX)
	DWORD i;
	DWORD cchLength;

	if(string == NULL) 
	{
		dec_assert(0);
		return 0;
	}

	cchLength = strlen(string);

	/* NOTE: this does not correctly handle MBCS characters, nor does it handle
	any characters for which there is not a one-to-one mapping upper-to-lower
	case. It is not possible to handle such situations in POSIX environments,
	so it is apparently standard practice in the UNIX world to ignore them.
	The best solution is to avoid using functions like this one for anything.
	(Most things are case sensitive in UNIX environments for this very
	reason.) 
	*/
	for(i = 0; i < cchLength; ++i) 
	{
		string[i] = tolower(string[i]);
	}

	return string;
#else
	return strlwr(string);
#endif
}


/* 
	Support_strupr:

	Description:

	 Converts a string to uppercase.

	Arguments:

	 string:			 The string to capitalize

	Return value:

	 This function returns a pointer to the altered string. Since the string
	 is altered in place, the return value is the same as 'string'. No return
	 value is reserved to indicate an error. */

char * Support_strupr(char *string)
{
#if defined (UNIX)
	DWORD i;
	DWORD cchLength;

	if(string == NULL)
	{
		dec_assert(0);
		return NULL;
	}

	cchLength = strlen(string);

	for(i = 0; i < cchLength; ++i) 
	{
		string[i] = toupper(string[i]);
	}

	return string;
#else
	return strupr(string);
#endif
}


/* 
	Support_CharNext:

	Description:

	 Returns a pointer to the next character in a string.

	Arguments:

	 lpsz:				 Pointer to a character in an EOS-terminated string.
						 Once again, this argument is unfortunately named.
						 There is a subtle difference between a string itself
						 and a pointer to one character in that string.

	Return value:

	 The return value is a pointer to the next character in the string, or
	 to the terminating EOS character at the end of the string. If lpsz points
	 to an EOS, then the function returns lpsz. */

char * Support_CharNext(const char *lpsz)
{
	int len;

	if(lpsz == NULL) 
	{
		dec_assert(0);
		return NULL;
	}

	if(*lpsz == EOS) 
	{
		return (char *)lpsz;
	}

#if defined(SYM_AIX) || defined(SYM_LINUX)
	mbstate_t state;
	memset(&state,'\0',sizeof(state));
	len = mbrlen(lpsz, MB_LEN_MAX,&state);
#else
	len = mblen(lpsz, MB_LEN_MAX);
#endif
	if(len < 1) 
	{
		len = 1;
	}

	return (char *)(lpsz + len);
}

/* 
	Support_CharPrev:

	Description:

	 Returns a pointer to the preceding character in a string.

	Arguments:

	 lpszStart:		 A pointer to the beginning of the string

	 lpszCurrent:		 A pointer to a character in an EOS-terminated string.
						 This argument is unfortunately named. There is a
						 subtle difference between a string itself (as with
						 lpszStart) and a pointer to one character in that
						 string.

	Return value:

	 The return value is a pointer to the preceding character of the string,
	 or to lpszStart. If lpszCurrent is equal to lpszStart, then lpszStart is
	 returned. */

char * Support_CharPrev(const char *lpszStart, const char *lpszCurrent)
{
	char *p;
	char *rc;

	if((lpszStart == NULL) || (lpszCurrent == NULL)) 
	{
		dec_assert(0);
		return NULL;
	}

	if(lpszCurrent < lpszStart)
	{
		/* lpszCurrent is before the beginning of the string */
		dec_assert(0);
		return (char *)lpszCurrent;
	}

	if(lpszCurrent == lpszStart) 
	{
		return (char *)lpszCurrent;
	}

	/* It is necessary to walk the string from the beginning in order to
	account for every possibility. Any shortcuts don't work under certain
	circumstances (that will probably be encountered in the field after we
	ship). 
	*/
	rc = (char *)lpszStart;

	for(p = (char *)lpszStart; p < lpszCurrent; p = Support_CharNext(p)) 
	{
		if(*p == EOS) 
		{
			/* lpszCurrent is after the end of the string */
			dec_assert(0);
			return rc;
		}

		rc = p;
	}

	return rc;
}


//************************************************************************
// DWORD Support_GetCurrentProcessId()
//
// Purpose
//
//  This function retrieves the next token from lpszString, using
//  lpszSeparators as a delimiters.  *lpnIndex is set to index the character
//  after the returned token.  lpszToken is filled with the token.  *lpnLength
//  is set to the length of the token.
//
// Parameters:
//
//  lpszString: The string to tokenize
//  lpszSeparators: Zero terminated string of separator characters
//  lpnIndex: IN=Index pointing to first character to start tokenizing from
//            OUT=Updated index pointing to character after tokenized word
//  lpnLength: OUT=Length of tokenized item
//  lpszToken: OUT=Token
//  nMaxLen:   IN=Max allowable length of token, not including zero terminator.
//
// Returns:
//  true if a token is retrieved
//  false if the EOS has been reached and no token was available
//
//************************************************************************
DWORD Support_GetCurrentProcessId()
{
#if defined(UNIX)
	return (DWORD)getpid();
#elif defined(_WINDOWS)
	return (DWORD)GetCurrentProcessId();
#elif defined(SYM_NLM)
	return (DWORD)GetNLMHandle();
#else
	#error ("***PORTABILITY ERROR: GetCurrentProcessId() not defined on this platform!");
#endif
}

