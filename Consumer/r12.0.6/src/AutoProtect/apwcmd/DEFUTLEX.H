//*******************************************************************
// Copyright 1996-1997 Symantec Corporation
//*******************************************************************
//
//
//
//*******************************************************************
#ifndef _DEFUTLEX_H_
#define _DEFUTLEX_H_

#include "defutilsinterface.h"
#include "NAVDefutilsLoader.h"

class  CDefUtilsEx
{

public:
	
	CDefUtilsEx();
	virtual ~CDefUtilsEx();

	BOOL GetNotificationHandle( HANDLE* pNotifyHandle );
	void SetNextNotification( void );
	void CloseNotification( void );
    bool AreCurrentDefsAuthentic( void );

    // These pass through to the defutils interface calls of
    // the same functions
    bool InitWindowsApp(const char *pszAppID);
    bool UseNewestDefs(bool *pbChanged);
    bool GetCurrentDefs(char *pszDir, int nByteSize);
    bool SetCurrentDefs(const char *pszDir);
    DURESULT GetLastResult();
    bool SetDoAuthenticationCheck(bool bAuthenticate);
    bool GetCurrentDefsDate(LPWORD pwYear, LPWORD pwMonth, LPWORD pwDay, LPDWORD pdwRev);
    bool IsAuthenticDefs(const char *psDefsDir, bool *pbTrusted);

private:
    BOOL DirectoryExists(LPCSTR pszDir);
    BOOL GetVirusDefsDir(LPSTR pszDefsDir, int nMaxCount);
    char StringGetLastChar(LPCSTR lpszString);
    void RemoveLastBackslash(LPSTR pszDir);
    DefUtilsLoader m_DefUtilsLoader;
    IDefUtilsPtr m_pDefUtils;

    HANDLE m_hDefsUpdated;    // handle for directory change notification
};

#endif  // _DEFUTLEX_H_