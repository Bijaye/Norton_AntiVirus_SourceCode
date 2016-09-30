// SNLogDataFormat.h: interface for the DataFormat class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CCSNLOGDATAFORMAT_H__D3C9970E_C1EF_430A_A1EC_EE648553AA3E__INCLUDED_)
#define AFX_CCSNLOGDATAFORMAT_H__D3C9970E_C1EF_430A_A1EC_EE648553AA3E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CDataFormat  
{
public:
	CDataFormat();
	virtual ~CDataFormat();

    static HRESULT MoveDBCSHotkey(LPCTSTR pszString, 
                                  LPTSTR pszFixedStr, 
                                  BOOL bKeepHotkey, 
                                  BOOL bUSStyle);
};

#endif // !defined(AFX_CCSNLOGDATAFORMAT_H__D3C9970E_C1EF_430A_A1EC_EE648553AA3E__INCLUDED_)
