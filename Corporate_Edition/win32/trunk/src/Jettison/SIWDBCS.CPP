// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
#include "stdafx.h"
#include <string.h>
#include "siwdbcs.h"
#ifndef SI_MAX_PATH
#define SI_MAX_PATH	512
#endif

BOOL dbcsPathSpecified(LPTSTR path)
{
	return dbcsFindFinalBackslash(path) != NULL;
}

LPTSTR dbcsAppendBackslash(LPTSTR path)
{
	LPTSTR lastChar = dbcsLastChar(path);
	if (*lastChar != '\\')
		lstrcat(path, "\\");
	return path;
}

LPTSTR dbcsFileCat(LPTSTR path, LPCTSTR file)
{
	dbcsAppendBackslash(path);
	lstrcat(path, file);
	return path;
}

LPTSTR dbcsFindChar(LPTSTR path, TCHAR c)
{
	while (*path != '\0' && *path != c)
	{
		path = AnsiNext(path);
	}
	if (*path == c)
		return path;
	else
		return NULL;
}

LPTSTR dbcsFindFinalBackslash(LPTSTR path)
{
	LPTSTR p = dbcsLastChar(path);
	while ((p > path) && (*p != '\\') && (*p != ':'))
		p = AnsiPrev(path, p);
	if (p >= path && (*p == '\\' || *p == ':'))
		return p;
	else
		return NULL;
}

LPTSTR dbcsFindFinalDot(LPTSTR path)
{
	LPTSTR p = dbcsLastChar(path);
	while ((p > path) && (*p != '.') && (*p != ':'))
		p = AnsiPrev(path, p);
	if (p >= path && (*p == '.' || *p == ':'))
		return p;
	else
		return NULL;
}

LPTSTR dbcsJustFileName(LPTSTR dest, LPTSTR path)
{
	LPTSTR fileName = dbcsFindFinalBackslash(path);
	if (fileName == NULL)
		lstrcpy(dest, path);
	else
	{
		fileName = AnsiNext(fileName);
		lstrcpy(dest, fileName);
	}
	return dest;
}

LPTSTR dbcsJustExtension(LPTSTR dest, LPCTSTR path)
{
	LPTSTR extension = NULL;
	LPTSTR filename = new TCHAR[lstrlen(path) + 1];
	*filename = '\0';
	lstrcpy(filename, path);
	extension = dbcsFindFinalDot(filename);
	if (*extension == '\0')
		lstrcpy(dest, path);
	else
	{
		extension = AnsiNext(extension);
		lstrcpy(dest, extension);
	}
	delete filename;
	return dest;
}
LPTSTR dbcsStripFileName(LPTSTR dest, LPTSTR path)
{
	TCHAR	temp[SI_MAX_PATH];
	lstrcpy(temp, path);
	LPTSTR pathName = dbcsFindFinalBackslash(temp);
	if (pathName != NULL)
	{			
		if (*pathName == ':')
			pathName++;															   
		*pathName = '\0';
		lstrcpy(dest, temp);
	}
	else
		if (dest != NULL)
			*dest = '\0';
	return dest;
}
#ifdef SYM_WIN32
BOOL dbcsCharEqual(LPTSTR char1, LPTSTR char2)
{
	size_t len1 = mblen(char1, 2);
	size_t len2 = mblen(char2, 2);
	if (len1 != len2)
		return FALSE;
	else if (len1 == 1)
		return *char1 == *char2;
	else
		return memcmp((LPVOID) char1, (LPVOID) char2, len1) != 0;
}
#endif

LPTSTR dbcsLastChar(LPTSTR path)
{
	LPTSTR last = dbcsEndOfString(path);
	return AnsiPrev(path, last);

}

LPTSTR dbcsEndOfString(LPTSTR path)
{
	while (*path)
		path = AnsiNext(path);
	return path;
}
			 
WORD dbcsStrLen(LPTSTR path)
{
	WORD result = 0;
	while (*path != '\0')
	{
		path = AnsiNext(path);
		if (*path != '\0')
			result++;
	}
	return result;
}


// returns the first occurence of str2 in str1, NULL if not found
LPTSTR dbcsStrStr(LPTSTR str1, LPTSTR str2)
{
	LPTSTR ret = NULL;
	LPTSTR tempstr1;
	LPTSTR tempstr2 = str2;
	LPTSTR beginstr1;
	beginstr1 = tempstr1 = str1;
	beginstr1 = tempstr1 = dbcsFindChar(beginstr1, *tempstr2);
	BOOL FailedMatch = FALSE;
	// This loop continues while it can find a matching first character
	while (tempstr1 != NULL)
	{
		// assume we're onto something and set ret to latest start of str1
		ret = beginstr1;
		// if first char found, move to next char
		tempstr1 = AnsiNext(tempstr1);
		tempstr2 = AnsiNext(tempstr2);
		// while next char isn't null term.
		while (*tempstr2 != '\0')
		{
			// if str1 ends before str2, no match
			if (*tempstr1 == '\0')
				return NULL;
			// if chars don't match restart look for first char at new pos
			if (*tempstr2 != *tempstr1)
			{
				FailedMatch = TRUE;
				break;
			}
			// This part must be an increment rather than AnsiNext
			// because even in double byte, you must verify that
			// both bytes match
			tempstr1++;
			tempstr2++;
		}
		if (FailedMatch)
		{
			// reset tempstr2 to start of str2
			tempstr2 = str2;
			// restart search by moving one char down from last start
			beginstr1 = AnsiNext(beginstr1);
			beginstr1 = tempstr1 = dbcsFindChar(beginstr1, *tempstr2);
			// since a match wasn't found, reset ret to NULL
			ret = NULL;
		}
		else
			return ret;
	}
	return ret;
}

