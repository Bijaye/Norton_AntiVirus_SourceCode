#include "StdAfx.h"
#include "WindowsSecurityAccount.h"

#include <Lmcons.h>

#include <Iptypes.h>
#include <Iphlpapi.h>

#define SECURITY_WIN32
#include <Security.h>

#ifdef _SUPPORTS_HARDWARE_ID
#include "ccHardwareId.h"
#endif 

// Link up
#pragma comment(lib, "Rpcrt4.lib")
#pragma comment(lib, "Iphlpapi.lib")

typedef BOOL (WINAPI *LPFNGetUserNameEx)(EXTENDED_NAME_FORMAT  NameFormat, LPSTR lpNameBuffer, PULONG nSize);
typedef BOOL (WINAPI *LPFNProcessIdToSessionId)(DWORD dwProcessId, LPDWORD pdwSessionId);

//****************************************************************************
//****************************************************************************
CWindowsSecurityAccount::CWindowsSecurityAccount(void)
{
	Initialize();
}

CWindowsSecurityAccount::~CWindowsSecurityAccount(void)
{
}

//****************************************************************************
//****************************************************************************
void CWindowsSecurityAccount::Initialize()
{

	return;
}


//****************************************************************************
//****************************************************************************
HRESULT CWindowsSecurityAccount::GetDomainName(CString &cszDomainName)
{
	//
	//  This routine is designed to only work on WinNT
	//  systems for now.  If a requirement for Win9x
	//  based systems comes up for Domain Name, then
	//  modify this code.
	//

	HRESULT hrReturn = E_NOTIMPL;
	
	CString cszUserName;

	return GetUserNameNT(cszUserName, cszDomainName);
}

//****************************************************************************
//****************************************************************************
HRESULT CWindowsSecurityAccount::GetUserName9x(CString &cszUserName)
{
	DWORD dwSize = UNLEN + 1;
	BOOL bRet = FALSE;

	bRet = ::GetUserName(cszUserName.GetBuffer(dwSize), &dwSize);
	cszUserName.ReleaseBufferSetLength(dwSize + 1);
	
	return bRet ? S_OK : HRESULT_FROM_WIN32(GetLastError());

}

//****************************************************************************
//****************************************************************************
HRESULT CWindowsSecurityAccount::GetUserNameNT(CString &cszUserName, CString &cszDomainName)
{
	HRESULT hrReturn = E_FAIL;

	HMODULE hmSecur32 = LoadLibrary(_T("SECUR32.DLL"));
	if(!hmSecur32)
		return HRESULT_FROM_WIN32(GetLastError());

	LPFNGetUserNameEx FNGetUserNameEx = NULL;
	FNGetUserNameEx = (LPFNGetUserNameEx)GetProcAddress(hmSecur32, _T("GetUserNameExA"));
	if(FNGetUserNameEx)
	{
		DWORD dwUserNameSize = 512;
		CString cszNTUserName;
		BOOL bRet = FNGetUserNameEx(NameSamCompatible , cszNTUserName.GetBuffer(dwUserNameSize), &dwUserNameSize);
		cszNTUserName.ReleaseBuffer();
		if(bRet)
		{
			int iWhack = cszNTUserName.Find('\\');
			if(-1 != iWhack)
			{
				cszDomainName = cszNTUserName.Left(iWhack);
				cszUserName = cszNTUserName.Right(cszNTUserName.GetLength() - iWhack - 1);
			}
			else
			{
				cszDomainName = _T("");
				cszUserName = cszNTUserName;
			}
		}

		hrReturn = bRet ? S_OK : E_FAIL;
	}

	FreeLibrary(hmSecur32);

	return S_OK;
}

//****************************************************************************
//****************************************************************************
HRESULT CWindowsSecurityAccount::GetUserName(CString &cszUserName)
{
	HRESULT hrReturn = E_NOTIMPL;
	if(m_osVersionInfo.IsWin2K(TRUE))
	{
		CString cszDomainName;
		hrReturn = GetUserNameNT(cszUserName, cszDomainName);
	}
	else
	{
		hrReturn = GetUserName9x(cszUserName);
	}


	return hrReturn;
}

//****************************************************************************
//****************************************************************************
HRESULT CWindowsSecurityAccount::GetSessionId(DWORD &dwSessionId)
{
    if(!m_osVersionInfo.IsTerminalServer())
    {
        dwSessionId = 0;
        return S_OK;
    }
    
    // Get a handle to Kernel32
    HMODULE hmKernel32 = GetModuleHandle(_T("Kernel32.dll"));
    if(!hmKernel32)
        return E_FAIL;

    // Get a function ptr for ProcessIdToSessionId
    LPFNProcessIdToSessionId FNProcessIdToSessionId = NULL;
    FNProcessIdToSessionId = (LPFNProcessIdToSessionId)GetProcAddress(hmKernel32, _T("ProcessIdToSessionId"));
    if(!FNProcessIdToSessionId)
        return E_FAIL;

    // Use the current process id to get a current session id
    DWORD dwProcessId = GetCurrentProcessId();
    BOOL bRet = FNProcessIdToSessionId(dwProcessId, &dwSessionId);
    if(!bRet)
        return E_FAIL;

    return S_OK;
}

//****************************************************************************
//****************************************************************************
HRESULT CWindowsSecurityAccount::GetComputerName(CString &cszComputerName)
{
	DWORD dwSize = MAX_COMPUTERNAME_LENGTH + 1;
	BOOL bRet;

	bRet = ::GetComputerName(cszComputerName.GetBuffer(dwSize), &dwSize);
	cszComputerName.ReleaseBufferSetLength(dwSize + 1);
	
	return bRet ? S_OK : HRESULT_FROM_WIN32(GetLastError());
}

//****************************************************************************
//****************************************************************************
HRESULT CWindowsSecurityAccount::CreateUUID(CString &cszUUID)
{
	//
	//  Create a unique ID in the form of a UUID
	//
	UUID uuidVal;

	HRESULT hr = E_FAIL;
	hr = UuidCreate(&uuidVal);
	if(FAILED(hr))
		return hr;

	HRESULT hrResult = RPC_S_OK;
	BYTE *pUUID;

	hrResult = UuidToString(&uuidVal, &pUUID);
	if(RPC_S_OK == hrResult)
	{
		cszUUID = "{";
		cszUUID += (LPTSTR)pUUID;
		cszUUID += "}";
		cszUUID.MakeUpper();

		RpcStringFree(&pUUID);
		hrResult = S_OK;
	}
	else
	{
		hrResult = E_OUTOFMEMORY;
	}

	return hrResult;
}

#ifdef _SUPPORTS_HARDWARE_ID

//****************************************************************************
//****************************************************************************
HRESULT CWindowsSecurityAccount::GetSysID(CString &cszSysID)
{
	ccMacEnc::CHardwareId::HardwareId cHardwareInfo;
	ccMacEnc::CHardwareId cHwid;
	
	cHwid.Create();
	cHwid.Generate(cHardwareInfo);
	cHwid.IdToString(cHardwareInfo, cszSysID.GetBuffer(20));
	cszSysID.ReleaseBuffer();
	cHwid.Destroy();

	return S_OK;
}

HRESULT CWindowsSecurityAccount::CompareSysID(CString cszSysId1, CString cszSysId2, long &nFailedCount)
{
	HRESULT hrReturn = E_NOTIMPL;

	ccMacEnc::CHardwareId::HardwareId cHardwareId1, cHardwareId2;
	ccMacEnc::CHardwareId cHwid;

	cHwid.Create();
	cHwid.IdFromString(cszSysId1, cHardwareId1);
	cHwid.IdFromString(cszSysId2, cHardwareId2);

	ccMacEnc::CHardwareId::ErrorType errorType = ccMacEnc::CHardwareId::eNoError;
	errorType = cHwid.Compare(cHardwareId1, cHardwareId2, NULL, nFailedCount);
	if(ccMacEnc::CHardwareId::eNoError == errorType)
	{
		hrReturn = S_OK;
	}
	else if(ccMacEnc::CHardwareId::eHardwareIdCompareError == errorType && nFailedCount != 0)
	{
		hrReturn = S_FALSE;
	}
	else
	{
		hrReturn = E_FAIL;
	}

	cHwid.Destroy();

	return hrReturn;
}

#endif // _SUPPORTS_HARDWARE_ID

//****************************************************************************
//****************************************************************************
HRESULT CWindowsSecurityAccount::GetUserSID(LPCTSTR cszUserName, CString &cszUserSID)
{
    return E_NOTIMPL;
}

//****************************************************************************
//****************************************************************************
HRESULT CWindowsSecurityAccount::GetUserSIDFromWTSSession(UINT iSessionId, CString &cszUserSID)
{
    return E_NOTIMPL;
}

//****************************************************************************
//****************************************************************************
HRESULT CWindowsSecurityAccount::GetCurrentUserSID(CString &cszUserSID)
{
	HRESULT hrReturn = E_NOTIMPL;
	DWORD dwUserSidSize = 256;
	CString cszDomainName;
	DWORD dwDomainNameSize = 256;
	SID_NAME_USE sidNameUse = SidTypeUser ;
	PSID pSid = new SID[4];
	DWORD dwSIDSize = 4 * sizeof(SID);
	HRESULT hr;
	BOOL bRet;
	CString cszUserName;
	
	hr = GetUserName(cszUserName);
	if(FAILED(hr))
		return hr;
	
	typedef BOOL (WINAPI *LPFNLookupAccountName)(LPCSTR lpSystemName, LPCSTR lpAccountName, PSID Sid, LPDWORD cbSid, LPSTR ReferencedDomainName, LPDWORD cbReferencedDomainName, PSID_NAME_USE peUse);
	typedef BOOL (WINAPI *LPFNConvertSidToStringSid)(PSID Sid, LPSTR *StringSid);

	HMODULE hmAdvapi = GetModuleHandle(_T("ADVAPI32.DLL"));
	if(!hmAdvapi)
		return HRESULT_FROM_WIN32(GetLastError());
	
	LPFNLookupAccountName FNLookupAccountName = NULL;
	FNLookupAccountName = (LPFNLookupAccountName)GetProcAddress(hmAdvapi, _T("LookupAccountNameA"));
	if(!FNLookupAccountName)
		return HRESULT_FROM_WIN32(GetLastError());

	LPFNConvertSidToStringSid FNConvertSidToStringSid = NULL;
	FNConvertSidToStringSid = (LPFNConvertSidToStringSid)GetProcAddress(hmAdvapi, _T("ConvertSidToStringSidA"));
	if(!FNLookupAccountName)
		return HRESULT_FROM_WIN32(GetLastError());

	bRet = FNLookupAccountName( NULL,
		cszUserName,
		pSid,
		&dwSIDSize,
		cszDomainName.GetBuffer(dwDomainNameSize),
		&dwDomainNameSize,
		&sidNameUse );
	cszDomainName.ReleaseBuffer();

	if(bRet)
	{
		DWORD dwErr = GetLastError();
		ATLTRACE2("LookupAccountName() successful. 0x%08X\r\n", dwErr);

		LPTSTR pSidString = NULL;
		bRet = FNConvertSidToStringSid(pSid, &pSidString);
		if(bRet)
		{
			cszUserSID = pSidString;
			LocalFree(pSidString);
            hrReturn = S_OK;
		}
        else
        {
            DWORD dwErr = GetLastError();
            ATLTRACE2("FNConvertSidToStringSid() failed.  Error: 0x%08X\r\n", dwErr);
            hrReturn = HRESULT_FROM_WIN32(dwErr);
        }
	}
	else
	{
		DWORD dwErr = GetLastError();
		ATLTRACE2("LookupAccountName() failed.  Error: 0x%08X\r\n", dwErr);
        hrReturn = HRESULT_FROM_WIN32(dwErr);
	}

	return hrReturn;
}

//****************************************************************************
//****************************************************************************
HRESULT CWindowsSecurityAccount::GetMACAddress(CString &cszMacAddress)
{

	IP_ADAPTER_INFO AdapterInfo[16];
	DWORD dwBufLen = sizeof(AdapterInfo);

	DWORD dwStatus = GetAdaptersInfo( AdapterInfo, &dwBufLen);
	
	int iSep = 0;
	PIP_ADAPTER_INFO pAdapterInfo = AdapterInfo;
	do 
	{
		if(iSep == 1)
			cszMacAddress += _T(", ");
		
		cszMacAddress += pAdapterInfo->Description;
		cszMacAddress += _T(" {");
		GetStringFromBYTE(pAdapterInfo->Address, pAdapterInfo->AddressLength, cszMacAddress);
		cszMacAddress += _T("}");
		iSep = 1;

		pAdapterInfo = pAdapterInfo->Next;
	}
	while(pAdapterInfo);                  

	return S_OK;

}


//****************************************************************************
//****************************************************************************
HRESULT CWindowsSecurityAccount::GetStringFromBYTE(BYTE *pData, DWORD dwLen, CString &cszMacAddress)
{
	CString cszTemp;
	int iSpace = 0;
	for (DWORD i=0; i<dwLen; i++)  
	{
		if(iSpace == 1)
			cszMacAddress += _T("-");

		cszTemp.Format(_T("%02X"), pData[i]);
		cszMacAddress += cszTemp;

		iSpace = 1;
	}

	return S_OK;
}