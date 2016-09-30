// unixxapi.cpp
// Copyright 1999 by Symantec Corporation.  All rights reserved.
// A Unix implementation of misc. (xapi) functions in the Symantec 
// core library (SYMKRNL).


#ifdef SYM_UNIX  // whole file


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>

#include "platform.h"
#include "file.h"
#include "xapi.h"
#include "dbcs.h"


static LPCTSTR FindFileName(LPCTSTR lpName, BOOL bSkipSlash);
static void UnixTimeToFileTime(time_t* pt, LPFILETIME pft);
static void FileTimeToUnixTime(LPFILETIME pft, time_t* pt);


// DBCSIsLeadByte()
// Returns: TRUE if byChar is a DBCS Lead Byte

BOOL SYM_EXPORT WINAPI DBCSIsLeadByte(BYTE byChar)
{
    return (FALSE);
}


// UnixTimeToFileTime()
// Convert from Windows FILETIME to Unix time_t.

static void UnixTimeToFileTime(time_t* pt, LPFILETIME pft)
{
    long long ll;  // 64 bits
                                                      
    ll = (*pt * 10000000LL) + 116444736000000000LL;
    pft->dwLowDateTime = (DWORD) ll;
    pft->dwHighDateTime = ll >> 32;
}


// FileTimeToUnixTime()
// Convert from Unix time_t to Windows FILETIME.

static void FileTimeToUnixTime(LPCFILETIME pft, time_t* pt)
{
    long long ll;  // 64 bits

    ll = (long long)pft->dwHighDateTime << 32 + pft->dwLowDateTime;
    *pt = (time_t) ( (ll - 116444736000000000LL) / 10000000LL );
}


// ConvertDateToEncoded()
// Covert month, day, year values to a DOS 16-bit date.
// wMonth: 1-12
// wDay: 1-31
// wYear: 1980-2107 OR 0-127
// Returns: DOS date

UINT SYM_EXPORT WINAPI ConvertDateToEncoded(UINT wMonth, UINT wDay, UINT wYear)
{
    register UINT date;

    if (wYear >= 1980)
        wYear -= 1980;
    else if (wYear >= 80)
        wYear -= 80;
    else  // assume 21st centry
        wYear += 20;

    date = wYear;
    date <<= 4;
    date += wMonth;
    date <<= 5;
    return (date + wDay);
}


// ConvertTimeToEncoded()
// Convert hour, minute, second values to a DOS 16-bit time.
// Seconds are adjusted to the closest even value.
// wHours: 0-23
// wMinutes: 0-59
// wSeconds: 0-59
// Returns: DOS time

UINT SYM_EXPORT WINAPI ConvertTimeToEncoded(UINT wHours, UINT wMinutes, UINT wSeconds)
{
    register UINT time;
    
    time = wHours;
    time <<= 6;
    time += wMinutes;
    time <<= 5;
    time += wSeconds >> 1;

    return (time);
}


// ConvertEncodedTimeToNormal()
// Convert a 16-bit DOS time to hours, minutes, seconds.

void SYM_EXPORT WINAPI ConvertEncodedTimeToNormal(UINT time, UINT FAR * hour, UINT FAR * minute, UINT FAR * seconds)
{
    register UINT encodedTime = time;

    *hour    = (encodedTime >> 11);
    *minute  = (encodedTime >> 5)  & 0x003F;
    *seconds = (encodedTime & 0x001F) << 1; /* encoded as 2-second intervals */
}


// ConvertEncodedDateToNormal()
// Convert a 16-bin DOS date to month, day, year.

void SYM_EXPORT WINAPI ConvertEncodedDateToNormal(UINT date, UINT FAR * month, UINT FAR * day, UINT FAR * year)
{
    register UINT encodedDate = date;

    *year  = (encodedDate >> 9) + 1980;
    *month = (encodedDate >> 5) & 0x000F;
    *day   = encodedDate & 0x001F;
}


// FileTimeToDosTime()
// Convert a FILETIME to a DOS 16-bit date and time.

BOOL SYM_EXPORT WINAPI FileTimeToDosTime(LPCFILETIME lpFileTime, LPWORD lpFatDate, LPWORD lpFatTime)
{
    time_t t;
    FileTimeToUnixTime(lpFileTime, &t);

    // Convert the time to local time and split into separate components.
    struct tm t2;
    gmtime_r(&t, &t2);

    // Convert to DOS format
    *lpFatDate = ConvertDateToEncoded(t2.tm_mon + 1, t2.tm_mday, t2.tm_year + 1900);
    *lpFatTime = ConvertTimeToEncoded(t2.tm_hour, t2.tm_min, t2.tm_sec);

    return (TRUE);
}


// DosTimeToFileTime()
// Convert a DOS 16-bit date and time to a FILETIME.

BOOL SYM_EXPORT WINAPI DosTimeToFileTime(WORD wFatDate, WORD wFatTime, LPFILETIME lpFileTime)
{
    // Convert from DOS time and date to individual fields.
    UINT hour, minute, second, month, day, year;
    ConvertEncodedDateToNormal(wFatDate, &hour, &minute, &second);
    ConvertEncodedTimeToNormal(wFatTime, &month, &day, &year);

    // Convert from individual fields to Unix struct tm.
    struct tm t2;
    t2.tm_sec = second;        // 0-59
    t2.tm_min = minute;        // 0-59
    t2.tm_hour = hour;         // 0-23
    t2.tm_mday = day;          // 1-31
    t2.tm_mon = month - 1;     // 0-11
    t2.tm_year = year - 1900;  // years since 1900
    t2.tm_wday = 0;
    t2.tm_yday = 0;
    t2.tm_isdst = 0;

    // Convert from struct tm to Unix time_t.
    time_t t;
    t = mktime(&t2);

    // Convert from Unix time_t to Windows FILETIME.
    UnixTimeToFileTime(&t, lpFileTime);

    return (TRUE);
}


// DateGetCurrentDateTime()
// Get the current local date and time in the 16 bit format used in DOS directory entries.

VOID SYM_EXPORT WINAPI DateGetCurrentDateTime(UINT FAR * datePtr, UINT FAR * timePtr)
{
    // Get the time in secs since 00:00:00 UTC Jan 1, 1970.
    time_t now;
    time(&now);

    // Convert the time to local time and split into separate components.
    struct tm now2;
    localtime_r(&now, &now2);

    // Convert to DOS format.
    *datePtr = ConvertDateToEncoded(now2.tm_mon + 1, now2.tm_mday, now2.tm_year + 1900);
    *timePtr = ConvertTimeToEncoded(now2.tm_hour, now2.tm_min, now2.tm_sec);
}


// _ConvertWordToString()
// Converts a UINT value into an ASCII string.
// wNumber: The number to be converted.
// lpString: Pointer to a buffer that is to receive the converted string.
// wRadix: The radix to use when converting the string.
// byThousandsChar: The separator to use for designating thousands (zero if none).  
// Returns: Pointer to the converted string.
// WARNING: Only supports wRadix == 10 and byThousandsChar == 0!

LPTSTR SYM_EXPORT WINAPI _ConvertWordToString(UINT wNumber, LPTSTR lpString, UINT wRadix, TCHAR byThousandsChar)
{
    sprintf(lpString, "%ul", (unsigned long) wNumber);
    return (lpString);
}


// _ConvertLongToString()
// Converts a DWORD value into an ASCII string, based on the specified radix.
// wNumber: The number to be converted.
// lpString: Pointer to a buffer that is to receive the converted string.
// wRadix: The radix to use when converting the string.
// byThousandsChar: The separator to use for designating thousands (zero if none).  
// Returns: Pointer to the converted string.
// WARNING: Only supports wRadix == 10 and byThousandsChar == 0!

LPTSTR SYM_EXPORT WINAPI _ConvertLongToString(DWORD dwNumber, LPTSTR lpString, UINT  wRadix, TCHAR byThousandsChar)
{
    sprintf(lpString, "%ul", (unsigned long) dwNumber);
    return (lpString);
}


// StringAppendChar()
// Appends a single character to a string.
// Returns: Pointer to the string.

LPTSTR SYM_EXPORT WINAPI StringAppendChar(LPTSTR lpString, WCHAR wChar)
{
    LPTSTR lpEnd = lpString + strlen(lpString);
    *lpEnd++ = wChar;
    *lpEnd++ = '\0';
    return (lpString);
}


// StringAppendNChars()
// Appends a single character to a string a specifed number of times.
// Returns: Pointer to the string.

LPTSTR SYM_EXPORT WINAPI StringAppendNChars(LPTSTR lpString, WCHAR wChar, int nCount)
{
    LPTSTR lpEnd = lpString + strlen(lpString);
    while (nCount-- > 0)
        *lpEnd++ = wChar;
    *lpEnd++ = '\0';
    return (lpString);
}


// StringGetLastChar()
// Gets the last character in a string.
// Returns: The value of the last character in a string.

WCHAR SYM_EXPORT WINAPI StringGetLastChar(LPCTSTR lpszString)
{
    while (*lpszString != '\0')
        ++lpszString;
    return *lpszString;
}


// StringGetEnd()
// Finds the end of a string.
// Returns: Pointer to the null terminator of a string.

LPTSTR SYM_EXPORT WINAPI StringGetEnd(LPTSTR lpszString)
{
    while (*lpszString != '\0')
        lpszString++;
    return (lpszString);
}


// NameReturnExtension()
// Copies the extension of a filename to a buffer.
// Extensions longer than three characters are truncated.
// Returns: NOERR

UINT SYM_EXPORT WINAPI NameReturnExtension(LPCTSTR lpszFileSpec, LPTSTR lpszExtension)
{
    return (NameReturnExtensionEx(lpszFileSpec, lpszExtension, 4));
}


// NameReturnExtensionEx()
// Copies the extension of a filename to a buffer.
// Returns: NOERR

UINT SYM_EXPORT WINAPI NameReturnExtensionEx(LPCTSTR lpszFileSpec, LPTSTR lpszExtension, UINT cbExtBuffSize)
{
    LPCTSTR p = strrchr(lpszFileSpec, '/');

    if (p == NULL)
        p = lpszFileSpec;

    p = strrchr(p, '.');
    
    if (p == NULL)
        *lpszExtension = '\0';
    else
    {
        strncpy(lpszExtension, p+1, cbExtBuffSize - 1);
        lpszExtension[cbExtBuffSize-1] = '\0';
    }

    return (NOERR);
}


// NameStripExtension()
// Chops the extension off a filename.
// Returns: Pointer to the string.

LPTSTR SYM_EXPORT WINAPI NameStripExtension(LPTSTR lpName)
{
    LPTSTR p = strrchr(lpName, '/');

    if (p == NULL)
        p = lpName;

    p = strrchr(p, '.');
    
    if (p != NULL)
        *p = '\0';

    return (lpName);
}


// NameAppendExtension()
// Appends an extension to a filename.
// A dot is first appended to the filename if neccessary.

VOID SYM_EXPORT WINAPI NameAppendExtension(LPTSTR lpPathname, LPCTSTR lpszExtension)
{
    if (lpszExtension[0] != '.' && StringGetLastChar(lpPathname) != '.')
        StringAppendChar(lpPathname, '.');
    STRCAT(lpPathname, lpszExtension);
}


// NameReturnFile()
// Copies the filename portion of a fully qualified filename to a buffer.
// Returns: NOERR on success.

UINT SYM_EXPORT WINAPI NameReturnFile(LPCTSTR lpszFileSpec, LPTSTR lpszFileName)
{
    LPCTSTR p = FindFileName(lpszFileSpec, TRUE);
    STRCPY(lpszFileName, p);
    return (NOERR);
}


// NameStripFile()
// Chops the filename portion off a fully qualified filename.
// The resulting pathname ends with a path separator only if it's the root.
// Returns: Pointer to the string.

LPTSTR SYM_EXPORT WINAPI NameStripFile(LPTSTR lpName)
{
    LPTSTR p = (LPTSTR) FindFileName(lpName, FALSE);
    *p = '\0';
    return (lpName);
}


// NameAppendFile()
// Appends a filename to a pathname.
// A path separator is first appended to the pathname if necessary.

VOID SYM_EXPORT WINAPI NameAppendFile(LPTSTR lpPathname, LPCTSTR lpFilename)
{
    if (lpFilename[0] != '/' && StringGetLastChar(lpPathname) != '/')
        StringAppendChar(lpPathname, '/');
    STRCAT(lpPathname, lpFilename);
}


// FindFileName()
// Finds the filename portion of a fully qualified filename.
// Note that the UNC names "//SERVER" and "//SERVER/VOLUME" do not contain a filename.
// If bSkipSlash is TRUE, the result does not include the path separator.
// If bSkipSlash is FALSE, the result includes the path separator unless the file 
// is in the root directory.
// Returns: Pointer to filename.

static LPCTSTR FindFileName(LPCTSTR lpName, BOOL bSkipSlash)
{
    LPCTSTR p = strrchr(lpName, '/');

    if (p == NULL)
        return (lpName);

    // If filename is in root, don't return the slash.
    if (p == lpName)
        p++;

    // If caller doesn't want any slashes, don't return the slash.
    if (bSkipSlash && *p == '/')
        p++;

    // If name is of the form "//server" or "//server/volume", 
    // return a pointer to the end of the string.
    if (lpName[0] == '/' && lpName[1] == '/')
    {
        char* p2 = strchr(lpName+2, '/');
        if (p2 == NULL || p2 == p)
            p = lpName + strlen(lpName);
    }

    return (p);
}


#endif // SYM_UNIX


