// HTMLFormat.cpp: implementation of the CHTMLFormat class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "HTMLFormat.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CHTMLFormat::CHTMLFormat()
{

}

CHTMLFormat::~CHTMLFormat()
{

}
HRESULT CHTMLFormat::HTMLEncode(TCHAR cChar, 
                                LPTSTR szBuffer,
                                DWORD& dwSize)
{
    if (dwSize == 0)
    {
        return E_MORE_DATA;
    }
    ZeroMemory(szBuffer, dwSize);

    // Replace all invalid chars
    TCHAR szTemp[16];
    if (cChar == 0x22) // "
    {
        lstrcpy(szTemp, _T("&quot;"));
    }
    else if (cChar == 0x26) 
    {
        lstrcpy(szTemp, _T("&amp;"));
    }
    else if (cChar == 0x3C) // <
    {
        lstrcpy(szTemp, _T("&lt;"));
    }
    else if (cChar == 0x3E) // >
    {
        lstrcpy(szTemp, _T("&gt;"));
    }
    else if (cChar == 0xA0) // no break space
    {
        lstrcpy(szTemp, _T("&nbsp;"));
    }
    else if (cChar > 127)
    {
        // The web disply must be capable of correctly handling high ascii
        wsprintf(szTemp, _T("%c"), cChar);
    }
	else if (cChar == '\r') // carriage return
	{
		lstrcpy(szTemp, _T("<br>"));
	}
	else if (cChar == '\n') // line feed
	{
		lstrcpy(szTemp, _T("<br>"));
	}
    else
    {
        wsprintf(szTemp, _T("%c"), cChar);
    }

    if (dwSize < (DWORD)lstrlen(szTemp) + 1)
    {
        dwSize += lstrlen(szTemp) + 1;
        return E_MORE_DATA;
    }
    lstrcpy(szBuffer, szTemp);

    return S_OK;
}

HRESULT CHTMLFormat::HTMLEncode(LPCTSTR szString, 
                                LPTSTR szBuffer,
                                DWORD& dwSize)
{
    if (dwSize == 0)
    {
        return E_MORE_DATA;
    }
    ZeroMemory(szBuffer, dwSize);

    // Replace all invalid chars
    TCHAR szTemp[16];
    LPCTSTR szThisString = szString;
    LPCTSTR szNextString = NULL;
    long nSize = dwSize;
    DWORD dwTempSize = 0;
    LONG_PTR nByteCount = 0;
    while (*szThisString != _T('\0'))
    {
        ZeroMemory(szTemp, sizeof(szTemp));

        // Get the next string offset
        szNextString = CharNext(szThisString);

        // If the next string is more than one TCHAR away then it is MBCS
        nByteCount = szNextString - szThisString;
        if (nByteCount > sizeof(TCHAR))
        {
            // BYTE copy all the MBCS
            CopyMemory(szTemp, szThisString, nByteCount);
        }
        else
        {
            if('\r' == *szThisString && '\n' == *szNextString)
			{
				// skip this char
				szThisString = szNextString;
				continue;
			}

			dwTempSize = CCDIMOF(szTemp);
            if (HTMLEncode(*szThisString,
                           szTemp,
                           dwTempSize) != S_OK)
            {
                return E_FAIL;
            }
        }

        if (nSize < lstrlen(szTemp) + 1)
        {
            dwSize += lstrlen(szTemp) + 1;
            return E_MORE_DATA;
        }
        lstrcat(szBuffer, szTemp);
        nSize -= lstrlen(szTemp);

        szThisString = szNextString;
    }

    return S_OK;
}

HRESULT CHTMLFormat::HTMLEncode(LPCTSTR szString, 
                                tstring& sHTMLString)
{
    try
    {
        sHTMLString = _T("");

        // Replace all invalid chars
        TCHAR szTemp[16];
        LPCTSTR szThisString = szString;
        LPCTSTR szNextString = NULL;
        DWORD dwTempSize = 0;
        LONG_PTR nByteCount = 0;
        while (*szThisString != _T('\0'))
        {
            ZeroMemory(szTemp, sizeof(szTemp));

            // Get the next string offset
            szNextString = CharNext(szThisString);

            // If the next string is more than one TCHAR away then it is MBCS
            nByteCount = szNextString - szThisString;
            if (nByteCount > sizeof(TCHAR))
            {
                // BYTE copy all the MBCS
                CopyMemory(szTemp, szThisString, nByteCount);
            }
            else
            {
				if('\r' == *szThisString && '\n' == *szNextString)
				{
					// skip this char
					szThisString = szNextString;
					continue;
				}

				dwTempSize = CCDIMOF(szTemp);
                if (HTMLEncode(*szThisString,
                               szTemp,
                               dwTempSize) != S_OK)
                {
                    return E_FAIL;
                }
            }

            sHTMLString += szTemp;

            szThisString = szNextString;
        }
    }
    catch(...)
    {
        return E_OUTOFMEMORY;
    }
    return S_OK;
}

HRESULT CHTMLFormat::HTMLEncode(LPCTSTR szString, 
                                BOOL bConvert,  
                                tstring& sHTMLString)
{
    if (bConvert != FALSE)
    {
        return HTMLEncode(szString,
                          sHTMLString);
    }
    else
    {
        try
        {
            if (szString == NULL)
            {
                sHTMLString = _T("");
            }
            else
            {
                sHTMLString = szString;
            }
        }
        catch(...)
        {
            return E_OUTOFMEMORY;
        }
    }
    return S_OK;
}

HRESULT CHTMLFormat::HTMLEncode(LPCTSTR szString, 
                                BOOL bConvert,  
                                _variant_t& vHTMLString)
{
    tstring szTemp;
    HRESULT hr = E_FAIL;
    vHTMLString = "";

    hr = HTMLEncode ( szString, bConvert, szTemp );

    if ( FAILED (hr))
        return hr;
    
    vHTMLString = szTemp.c_str();

    return S_OK;
}
