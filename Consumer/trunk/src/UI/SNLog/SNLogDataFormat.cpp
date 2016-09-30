////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

// SNLogDataFormat.cpp: implementation of the DataFormat class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SNLogDataFormat.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDataFormat::CDataFormat()
{

}

CDataFormat::~CDataFormat()
{

}

// Taken from Integrator MainIntWnd.cpp
HRESULT CDataFormat::MoveDBCSHotkey(LPCTSTR pszString, 
                                    LPTSTR pszFixedStr, 
                                    BOOL bKeepHotkey, 
                                    BOOL bUSStyle)
{
	TCHAR szHotkey[2] = {0};
	LPTSTR pszLoc = NULL;
	BOOL bPutBackTogether = FALSE;

	*pszFixedStr = _T('\0');

	// Find the begining "(&" for DBCS
	pszLoc = _tcsstr(pszString,  _T("(&"));
	if(pszLoc)
	{
		// Check if there is a space before the (... if so, remove it
		pszLoc = CharPrev(pszString, pszLoc);
		if(pszLoc[0] != _T(' '))
			pszLoc = CharNext(pszLoc);

		// Copy everything before the "(&"
		_tcsncpy(pszFixedStr, pszString, (pszLoc - pszString) / sizeof(TCHAR));
		pszFixedStr[(pszLoc - pszString) / sizeof(TCHAR)] = _T('\0');

		if(*pszLoc == _T(' '))
			pszLoc = CharNext(pszLoc);

		pszLoc = CharNext(pszLoc);
		pszLoc = CharNext(pszLoc);
		
		// Save the hotkey
		*szHotkey = pszLoc[0];

		// Find the end ")"
		pszLoc = _tcsstr(pszLoc, _T(")"));

		if(!pszLoc)
		{
			_tcscpy(pszFixedStr, pszString);
		}
		else
		{
			// Copy everthing after the ")"
			pszLoc = CharNext(pszLoc);
			_tcscat(pszFixedStr, pszLoc);
			bPutBackTogether = TRUE;
		}
	}
	else if(pszLoc = _tcschr(pszString,  _T('&')))
	{
		// Found the begining for non DBCS...
		// Make sure this is not "&&"
		LPTSTR pszNext = CharNext(pszLoc);
		if(pszNext[0] == _T('&'))
		{
			pszLoc = CharNext(pszNext);
			pszLoc = _tcschr(pszLoc, _T('&'));
		}

		if(pszLoc)
		{
			// Copy everything before the "&"
			_tcsncpy(pszFixedStr, pszString, (pszLoc - pszString) / sizeof(TCHAR));
			pszFixedStr[(pszLoc - pszString) / sizeof(TCHAR)] = _T('\0');

			// Move past the "&"
			pszLoc = CharNext(pszLoc);

			// Save the hotkey
			*szHotkey = pszLoc[0];
			
			// Copy everything after the "&"
			_tcscat(pszFixedStr, pszLoc);
			bPutBackTogether = TRUE;
		}
	}
	else
	{
		lstrcpy(pszFixedStr, pszString); 
		return S_OK;
	}


	// Rebuild the string
	if(bKeepHotkey && bPutBackTogether)
	{
		if(bUSStyle)
		{
			LPTSTR pszStr = new TCHAR[lstrlen(pszFixedStr) + 3];
            if ( NULL == pszStr )
                return E_OUTOFMEMORY;

			pszLoc = _tcschr(pszFixedStr, szHotkey[0]);
			if(!pszLoc)
			{
				// Try to find the opposite case hotkey (ie. if lowercase find uppercase)
				if(_istlower(*szHotkey))
					*szHotkey = _toupper(*szHotkey);
				else
					*szHotkey = _tolower(*szHotkey);
					
				pszLoc = _tcschr(pszFixedStr, szHotkey[0]);
				if(!pszLoc)
				{
					// return the original string
					lstrcpy(pszFixedStr, pszString);
					delete[] pszStr;
					return S_OK;
				}
			}

			// Insert the &
			_tcsncpy(pszStr, pszFixedStr, (pszLoc - pszFixedStr) / sizeof(TCHAR));
			pszStr[(pszLoc - pszFixedStr) / sizeof(TCHAR)] = _T('\0');
			if(pszLoc - pszFixedStr == 0)
				_tcscpy(pszStr, _T("&"));
			else
				_tcscat(pszStr, _T("&"));
			_tcscat(pszStr, pszLoc);

			_tcscpy(pszFixedStr, pszStr);

			delete[] pszStr;
		}
		else
		{
			if(_istlower(*szHotkey))
				*szHotkey = _toupper(*szHotkey);

			wsprintf(pszFixedStr, _T("%s (&%s)"), pszFixedStr, szHotkey);
		}
	}

    return S_OK;
}