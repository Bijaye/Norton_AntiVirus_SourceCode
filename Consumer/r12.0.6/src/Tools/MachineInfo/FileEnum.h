// FileEnum.h: interface for the CFileEnum class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FILEENUM_H__47D5F32A_F7EC_4786_AC91_F8C4C8ECDA9B__INCLUDED_)
#define AFX_FILEENUM_H__47D5F32A_F7EC_4786_AC91_F8C4C8ECDA9B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CFileEnum  
{
public:
	LPCTSTR GetCurrentFileName ();
	void Restart();
	bool NextFile ( LPTSTR lpszFileName );
	CFileEnum( LPCTSTR lpcszSourcePath,
               LPCTSTR lpcszFileMask );
	virtual ~CFileEnum();

protected:
	int m_iState;
	HANDLE m_hFindHandle;
    TCHAR m_szFileMask [MAX_PATH];
    TCHAR m_szSourcePath [MAX_PATH];
    TCHAR m_szSearchPath [MAX_PATH];

    WIN32_FIND_DATA m_FindFileData;


};

#endif // !defined(AFX_FILEENUM_H__47D5F32A_F7EC_4786_AC91_F8C4C8ECDA9B__INCLUDED_)
