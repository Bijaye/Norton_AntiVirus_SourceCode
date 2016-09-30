////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

// HTMLFormat.h: interface for the CHTMLFormat class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_HTMLFORMAT_H__1ABEF57C_34BA_4960_86D8_99306AC9211A__INCLUDED_)
#define AFX_HTMLFORMAT_H__1ABEF57C_34BA_4960_86D8_99306AC9211A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string>
#include "ccLogViewerInterface.h"

class CHTMLFormat  
{
public:
	CHTMLFormat();
	virtual ~CHTMLFormat();

    static HRESULT HTMLEncode(LPCTSTR szString, 
                              BOOL bConvert,  
                              tstring& sHTMLString);

    static HRESULT HTMLEncode(LPCTSTR szString, 
                              BOOL bConvert,  
                              _variant_t& vHTMLString);

    static HRESULT HTMLEncode(LPCTSTR szString, 
                              tstring& sHTMLString);

    static HRESULT HTMLEncode(LPCTSTR szString, 
                              LPTSTR szBuffer,
                              DWORD& dwSize);

    static HRESULT HTMLEncode(TCHAR cChar, 
                              LPTSTR szBuffer,
                              DWORD& dwSize);
};

#endif // !defined(AFX_HTMLFORMAT_H__1ABEF57C_34BA_4960_86D8_99306AC9211A__INCLUDED_)
