#pragma once

#include "ccOSInfo.h"

class CWindowsSecurityAccount
{
public:
    CWindowsSecurityAccount(void);
    virtual ~CWindowsSecurityAccount(void);

	void Initialize();

	HRESULT GetDomainName(CString &cszDomainName);
	
	HRESULT GetUserName(CString &cszDomainName);
	HRESULT GetUserName9x(CString &cszUserName);
	HRESULT GetUserNameNT(CString &cszUserName, CString &cszDomainName);
    
    HRESULT GetSessionId(DWORD &dwSessionId);

	HRESULT GetComputerName(CString &cszComputerName);
	HRESULT GetCurrentUserSID(CString &cszUserSID);
    HRESULT GetUserSID(LPCTSTR cszUserName, CString &cszUserSID);
    HRESULT GetUserSIDFromWTSSession(UINT iSessionId, CString &cszUserSID);

#ifdef _SUPPORTS_HARDWARE_ID
	HRESULT GetSysID(CString &cszSysID);
	HRESULT CompareSysID(CString cszSysId1, CString cszSysId2, long &nFailedCount);
#endif // _SUPPORTS_HARDWARE_ID

	HRESULT CreateUUID(CString &cszUUID);

	HRESULT GetMACAddress(CString &cszMacAddress);
	HRESULT GetStringFromBYTE(BYTE *pData, DWORD dwLen, CString &cszMacAddress);

protected:
	
	ccLib::COSInfo m_osVersionInfo;
};
