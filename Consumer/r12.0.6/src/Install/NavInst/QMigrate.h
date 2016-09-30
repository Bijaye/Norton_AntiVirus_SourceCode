// QMigrate.h: interface for the CQMigrate class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_QMIGRATE1_H__AF8006EF_3D82_44C8_A782_C359BB982BB9__INCLUDED_)
#define AFX_QMIGRATE1_H__AF8006EF_3D82_44C8_A782_C359BB982BB9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Quar32Legacy.h"
#include "AutoProtectWrapper.h"

class CQMigrate  
{
public:
	bool UpdateOldFormatFiles();
	bool Initialize(char* szQuarantinePath);
	bool EnableAutoProtect(bool bEnable);
	CQMigrate();
	virtual ~CQMigrate();

private:
	bool IsOldFormatFile(char* szFileName);
	bool ConvertFile(char* szFileName);
	BOOL TruncateFile(LPCTSTR lpcszFilePath);
	bool ReadHeader(char *szFileName, QFILE_HEADER_STRUCT &fileHeader);
	HRESULT SetFileSecurityDesc( LPSTR lpszFileName, LPBYTE pDesc );
	HRESULT GetFileSecurityDesc( LPSTR lpszFileName, LPBYTE *pDesc );
	HRESULT TransferFile( LPCTSTR lpszSourceName, HANDLE hSource, LPCTSTR lpszDestName, HANDLE hDest );
	bool RestoreItem(char* szQuarFile, char* szUnpackagedFile);
	BOOL IsWinNT( void );

	bool m_bInitialized;
	char m_szQuarantinePath[MAX_PATH];

	CAutoProtectWrapper m_APWrapper;

};

#endif // !defined(AFX_QMIGRATE1_H__AF8006EF_3D82_44C8_A782_C359BB982BB9__INCLUDED_)
