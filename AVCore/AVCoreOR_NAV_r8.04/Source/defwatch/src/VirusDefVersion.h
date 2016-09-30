// VirusDefVersion.h: interface for the CVirusDefVersion class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_VIRUSDEFVERSION_H__8022D526_76A2_11D2_8E55_72C9EE000000__INCLUDED_)
#define AFX_VIRUSDEFVERSION_H__8022D526_76A2_11D2_8E55_72C9EE000000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CVirusDefVersion
{
public:
	CVirusDefVersion();
    CVirusDefVersion( WORD wYear, WORD wMonth, WORD wDay, DWORD dwRev );
	~CVirusDefVersion();

    BOOL Load( const LPTSTR szRegKey );
    BOOL Save( const LPTSTR szRegKey );

    int operator==( const CVirusDefVersion& other );
    int operator!=( const CVirusDefVersion& other );
    CVirusDefVersion& operator=( const CVirusDefVersion& copy );
    

public:
    SYSTEMTIME  m_stDefDate;
    DWORD       m_dwDefRevision;
};

#endif // !defined(AFX_VIRUSDEFVERSION_H__8022D526_76A2_11D2_8E55_72C9EE000000__INCLUDED_)
