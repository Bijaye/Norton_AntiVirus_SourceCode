//*******************************************************************
// Copyright 1996-1997 Symantec Corporation
//*******************************************************************
//
//
//
//*******************************************************************
#ifndef _DEFUTLEX_H_
#define _DEFUTLEX_H_

#include "DefUtils.h"

class  CDefUtilsEx : public CDefUtils
{

public:
	
	CDefUtilsEx();
	virtual ~CDefUtilsEx();

	BOOL GetNotificationHandle( HANDLE* pNotifyHandle );
	void SetNextNotification( void );
	void CloseNotification( void );

private:
    BOOL DirectoryExists(LPCSTR pszDir);
    BOOL GetVirusDefsDir(LPSTR pszDefsDir, int nMaxCount);
    char StringGetLastChar(LPCSTR lpszString);
    void RemoveLastBackslash(LPSTR pszDir);

    HANDLE m_hDefsUpdated;    // handle for directory change notification
};

#endif  // _DEFUTLEX_H_