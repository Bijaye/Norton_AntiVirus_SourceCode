// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
#ifndef _SERVER_INFO
#define _SERVER_INFO

#include "RegKey.h"
#include "AppInfo.h"

#define SYMANTEC_DIRECTORY		"NAV"
#define	VP6_ADDRESSCACHE_REG_KEY	_T("Software\\INTEL\\LANDesk\\VirusProtect6\\CurrentVersion\\AddressCache")
#define VP6_CURRVER_REG_KEY		_T("Software\\INTEL\\LANDesk\\VirusProtect6\\CurrentVersion")
#define VP6_ADMIN_SECURITY_KEY		_T("Software\\INTEL\\LANDesk\\VirusProtect6\\CurrentVersion\\AdministratorOnly\\Security")

#define CLIENTTYPE_CONNECTED		0x00000001
#define CLIENTTYPE_STANDALONE		0x00000002
#define CLIENTTYPE_SERVER		0x00000004
#define SERVER_CHECK_KEY		"SOFTWARE\\INTEL\\LANDesk\\VirusProtect6\\CurrentVersion"
#define CLIENTTYPE_KEY			"ClientType"



class CTargetInfo
{
public:
	CString m_sPath;			// Full UNC path to target
	CString m_sServer;			// Server component
	CString m_sShare;			// Share component
	CString m_sDir;				// Directory component
	CString m_sLocalRep;		// Local rep from targets view
	CString m_sNOS;				// Target operating system 
	CString m_sOU;				// NDS context
	CString m_sPassword;		// NDS password to containter
	CString m_sUserName;		// NDS username to container
	CString m_nw32Path;			// Netware Client32 path
	DWORD m_dwDisplayType;		// Target graphical display type
	DWORD m_dwError;			// Last network error reported
	DWORD m_dwFlags;			// Flags for Setup COM object 
	CString m_sParent;			// Name of parent server
	CString m_sDomain;			// Name of LDVP domain
	CString m_sDomPasswd;		// LDVP domain password
};

BOOL UpdatePDS(BOOL bOnInstall);
HRESULT StartComputerUpdate(BOOL bSilent, LPCSTR sLangID = EMPTY, DWORD dwFlags = 0);
DWORD AddServerInfo(CTargetInfo *pInfo, CString sPath = EMPTY);
DWORD ServerMajorRelease(CString sServer);
DWORD GetOldVersion();
void RemoveOldVersion();
DWORD GetServerInfo(CTargetInfo* pInfo,CString sServer,DWORD dwAction,LPTM pTrans);
DWORD GetDomainInfo(CString sServer, CString& sDomain, CString& sDomPasswd, LPTM pTrans);
DWORD GetDomainNameAndPassword(CString sServer, CString& sDomain, CString& sDomPasswd, LPTM pTrans);
BOOL ClientIsServer();


#endif
