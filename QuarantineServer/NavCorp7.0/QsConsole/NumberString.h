// NumberString.h: interface for the CNumberString class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NUMBERSTRING_H__C708F263_D1AF_11D2_969A_00104BF3F36A__INCLUDED_)
#define AFX_NUMBERSTRING_H__C708F263_D1AF_11D2_969A_00104BF3F36A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CNumberString : public CString  
{
public:
	CNumberString( DWORD dwNumber );
	virtual ~CNumberString();
    
    void SetNumber( DWORD dwNumber );
    operator DWORD() { return m_dwNumber; }

private:
    void FormatNumber();

private:
    DWORD   m_dwNumber;
};

#endif // !defined(AFX_NUMBERSTRING_H__C708F263_D1AF_11D2_969A_00104BF3F36A__INCLUDED_)
