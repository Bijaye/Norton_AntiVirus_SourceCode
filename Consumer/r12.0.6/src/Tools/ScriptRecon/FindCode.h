// FindCode.h: interface for the CFindCode class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FINDCODE_H__28D6FCEA_159A_4DBF_88C8_C1E5045B7BA4__INCLUDED_)
#define AFX_FINDCODE_H__28D6FCEA_159A_4DBF_88C8_C1E5045B7BA4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CFindCode  
{
public:
	DWORD FindLine ( LPCTSTR lpcszFileName,
                     LPCTSTR lpcszResourceName,
                     DWORD dwLineNumber,
                     LPTSTR lpszString,
                     DWORD dwBufferSize );
	CFindCode();
	virtual ~CFindCode();

};

#endif // !defined(AFX_FINDCODE_H__28D6FCEA_159A_4DBF_88C8_C1E5045B7BA4__INCLUDED_)
