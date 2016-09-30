// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
#include <stdlib.h>
#include "stdafx.h"
#include <ctype.h>
#include "siwdbcs.h"
#include "StringHe.h"

LPTSTR kEXE = "EXE";
LPTSTR kDLL = "DLL";
LPTSTR kVXD = "VXD";
LPTSTR k386 = "386";

LPTSTR CStringHelper::SkipWhiteSpace(LPTSTR string)
{
	while (*string != '\0' && *string <= ' ')
		string = AnsiNext(string);
	return string;
}

LPTSTR CStringHelper::FindChar(LPTSTR string, TCHAR character)
{
	while (*string != '\0' && *string != character)
	{
		string = AnsiNext(string);
	}
	return string;
}

LPTSTR CStringHelper::ReverseFindChar(LPTSTR string, TCHAR character)
{
	LPTSTR ret = NULL;
	LPTSTR backwards = ::dbcsLastChar(string);
	while (backwards >= string)
	{
		if (!isleadbyte(*backwards) && *backwards == character)
		{
			ret = backwards;
			break;
		}
		else
			backwards = AnsiPrev(string, backwards);
	}

	return ret;
}
//RSS!!! not dbcs friendly
LPTSTR CStringHelper::StripWhiteSpace(LPTSTR string)
{
	LPTSTR copyOfString = new TCHAR[lstrlen(string) + 1];
	lstrcpy(copyOfString, string);
	LPTSTR outString = string;
	LPTSTR inString = copyOfString;
	BOOL inQuote = FALSE;
	while (*inString != '\0')
	{
		if (*inString == '"')
			inQuote = !inQuote;
		if (*inString > ' ' || inQuote)
			*outString++ = *inString;
		inString++;
	}
	*outString = '\0';
	delete [] copyOfString;
	return string;
}

LPTSTR CStringHelper::ExtractString(LPTSTR beginPtr, LPTSTR endPtr, LPTSTR dest)
{
	LPTSTR out = dest;
	while (beginPtr <= endPtr)
	{
		*out++ = *beginPtr++;
	}
	*out = NULL;

	return dest;
}

LPTSTR CStringHelper::GetParameters(LPTSTR string, LPTSTR dest)
{
	CStringHelper helper(string);

	LPTSTR leftParen  = helper.FindChar('(');
	LPTSTR rightParen = helper.ReverseFindChar(')');
	ExtractString(leftParen + 1, rightParen - 1, dest);
	
	return dest;
}

LPTSTR CStringHelper::ParseParameter(LPTSTR params, int paramIndex, LPTSTR dest)
{
	BOOL inQuotes = FALSE;
	LPTSTR startParam = params;
	int index = 0;
	while (*params != '\0')
	{
		if (*params == '"')
			inQuotes = !inQuotes;
		if ((!inQuotes && *params == ',') || *AnsiNext(params) == '\0')
		{
			index++;
			if (*AnsiNext(params) == '\0')
				params = AnsiNext(params);
			if (index == paramIndex)
			{
				LPTSTR dp = dest;
				while (startParam < params)
				{
					*dp++ = *startParam++;					
				}
				*dp = '\0';
				break;
			}
			else
				startParam = AnsiNext(params);
		}
		params = AnsiNext(params);
	}
	StripQuotes(dest);
	return dest;
}

void CStringHelper::ParseParameter(LPTSTR params, int paramIndex, int &integer)
{
	TCHAR temp[StringHelperMax];
	CStringHelper::ParseParameter(params, paramIndex, temp);
	integer = atoi(temp);
}

LPTSTR CStringHelper::StripQuotes(LPTSTR string)
{
	LPTSTR temp = new TCHAR[lstrlen(string) + 1];
	if (temp != NULL)
	{
		lstrcpy(temp, string);
		LPTSTR p = string;
		if (*p == '"')
			p++;
		lstrcpy(string, p);
		if (*dbcsLastChar(string) == '"')
			*dbcsLastChar(string) = '\0';
		delete [] temp;
	}
	return string;
}

BOOL CStringHelper::HasWildcards(LPTSTR fileSpec)
{
	BOOL found = FALSE;
	while ( fileSpec != NULL && *fileSpec != '\0' && !found )
	{
		switch (*fileSpec)
		{
			case '*':
			case '?':
				found = TRUE;
				break;
		}
		fileSpec++;
	}
	return found;
}

BOOL CStringHelper::IsDllOrExe(LPCTSTR filename)
{
	LPTSTR temp = new TCHAR[lstrlen(filename) + 1];
	dbcsJustExtension(temp, filename);
	AnsiUpper(temp);
	BOOL ret = ( !lstrcmp(temp, kEXE) ||
				 !lstrcmp(temp, kDLL) ||
				 !lstrcmp(temp, kVXD) ||
				 !lstrcmp(temp, k386) );
	delete [] temp;
	return ret;
}
