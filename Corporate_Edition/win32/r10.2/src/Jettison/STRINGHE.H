// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
const StringHelperMax = 512;

class CStringHelper
{
public:
	CStringHelper(LPTSTR string) { fString = string; }
	~CStringHelper() {}
	
	LPTSTR SkipWhiteSpace() { return SkipWhiteSpace(fString); }
	LPTSTR FindChar(TCHAR character) { return FindChar(fString, character); }
	LPTSTR ReverseFindChar(TCHAR character) { return ReverseFindChar(fString, character); }
	LPTSTR StripWhiteSpace() { return StripWhiteSpace(fString); }
	LPTSTR GetParameters(LPTSTR dest) { return GetParameters(fString, dest); }
	LPTSTR ParseParameter(int paramIndex, LPTSTR dest) { return ParseParameter(fString, paramIndex, dest); }
	
	static LPTSTR GetParameters(LPTSTR string, LPTSTR dest);
	static LPTSTR SkipWhiteSpace(LPTSTR string);
	static LPTSTR FindChar(LPTSTR string, TCHAR character);
	static LPTSTR StripWhiteSpace(LPTSTR string);
	static LPTSTR StripQuotes(LPTSTR string);
	static LPTSTR ReverseFindChar(LPTSTR string, TCHAR character);
	static LPTSTR ExtractString(LPTSTR beginPtr, LPTSTR endPtr, LPTSTR dest);
	static LPTSTR ParseParameter(LPTSTR params, int paramIndex, LPTSTR dest);
	static void ParseParameter(LPTSTR params, int paramIndex, int &integer);
	static BOOL HasWildcards(LPTSTR fileSpec);
	static BOOL IsDllOrExe(LPCTSTR filename);
private:
	LPTSTR			fString;
};

