////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "AvProdLoggingACP.h"

#include <ShellAPI.h>

#include <ISSharedPathProvider.h>
#include <ccOSInfo.h>
#include <ISVersion.h>

using namespace AvProdLogging;

//****************************************************************************
//****************************************************************************
//****************************************************************************
//****************************************************************************
CAvProdLoggingACP::CAvProdLoggingACP(void)
{
	m_dwHasAdminPrivilege = -1;
}

CAvProdLoggingACP::~CAvProdLoggingACP(void)
{
}

//****************************************************************************
//****************************************************************************
bool CAvProdLoggingACP::AllowGlobalClearLogs(bool bPromptForElevation)
{
	return HasAdminPrivilege(bPromptForElevation);
}

//****************************************************************************
//****************************************************************************
bool CAvProdLoggingACP::AllowDeleteThisItem(LPCTSTR szSIDString, bool bPromptForElevation)
{
	return EqualSID(szSIDString) || HasAdminPrivilege(bPromptForElevation);
}

//****************************************************************************
//****************************************************************************
bool CAvProdLoggingACP::AllowRestoreThisItem(LPCTSTR szSIDString, bool bPromptForElevation)
{
	return EqualSID(szSIDString) || HasAdminPrivilege(bPromptForElevation);
}

//****************************************************************************
//****************************************************************************
bool CAvProdLoggingACP::AllowViewItemDetails(LPCTSTR szSIDString, bool bPromptForElevation)
{
	return EqualSID(szSIDString) || HasAdminPrivilege(bPromptForElevation);
}

//****************************************************************************
//****************************************************************************
bool CAvProdLoggingACP::AllowExcludeItem(LPCTSTR szSIDString, bool bPromptForElevation)
{
    return HasAdminPrivilege(bPromptForElevation);
}

//****************************************************************************
//****************************************************************************
bool CAvProdLoggingACP::HasAdminPrivilege(bool bPromptForElevation)
{
	if((DWORD)-1 == m_dwHasAdminPrivilege || bPromptForElevation)
		CalcAdminPrivilege(bPromptForElevation);

	return 0 != m_dwHasAdminPrivilege ? true : false;
}

//****************************************************************************
//****************************************************************************
const SID* CAvProdLoggingACP::GetUserSID()
{
	if(!m_sidCurrentUserSID.GetPSID())
		CalcUserSID();

    return m_sidCurrentUserSID.GetPSID();
}

//****************************************************************************
//****************************************************************************
LPCTSTR CAvProdLoggingACP::GetUserSIDString()
{
    return m_sidCurrentUserSID.Sid();
}

//****************************************************************************
//****************************************************************************
void CAvProdLoggingACP::CalcAdminPrivilege(bool bPromptForElevation)
{
	ccLib::COSInfo osInfo;
	if (osInfo.IsWinVista(true) && bPromptForElevation) // Shell Execute the stub, if it works the user is an admin
	{
		TCHAR szPath[_MAX_PATH] = {0};
		size_t sizePath = _MAX_PATH;
		if ( CISSharedPathProvider::GetPath(szPath, sizePath))
		{
			_tcscat ( szPath, _T("\\isUAC.exe"));
			HINSTANCE hInstance = ::ShellExecute(GetActiveWindow(), _T("open"), szPath, NULL, NULL, SW_SHOWNORMAL);
			if ( hInstance <= (HINSTANCE) 32)
			{
				CCTRCTXE2(_T("Failed to launch %s -- %d"), szPath, ::GetLastError());
				m_dwHasAdminPrivilege = 0;
				return;
			}

			CCTRCTXI0(_T("Admin privilege confirmed."));
			m_dwHasAdminPrivilege = 1;
			return;
		}
		else
		{
			CCTRCTXE0(_T("Failed to get ISShared path.  Falling back to IsAdmin()."));
			m_dwHasAdminPrivilege = osInfo.IsAdministrator() ? 1 : 0;
			return;
		}
	}
	else // XP
	{
		m_dwHasAdminPrivilege = osInfo.IsAdministrator() ? 1 : 0;
	}

	return;
}

//****************************************************************************
//****************************************************************************
void CAvProdLoggingACP::CalcUserSID()
{
    StahlSoft::CSmartHandle shThread = OpenThread(READ_CONTROL, false, GetCurrentThreadId());
    if(!shThread)
    {
        DWORD dwErr = GetLastError();
        CCTRACEE( CCTRCTX _T("Problem getting thread ownser sid. GetLastError() = 0x%08X"), dwErr);
        return;
    }

    if(!AtlGetOwnerSid(shThread, SE_KERNEL_OBJECT, &m_sidCurrentUserSID))
    {
        DWORD dwErr = GetLastError();
        CCTRACEE( CCTRCTX _T("Problem getting thread ownser sid. GetLastError() = 0x%08X"), dwErr);
        return;
    }
    
    CCTRACEI( CCTRCTX _T("User SID for this thread: %s"), m_sidCurrentUserSID.Sid());
    return;
}

//****************************************************************************
//****************************************************************************
bool CAvProdLoggingACP::EqualSID(LPCTSTR szSID1)
{
    if(!GetUserSID())
        return false;

    PSID pSID1 = NULL;
    if(!::ConvertStringSidToSid(szSID1, &pSID1))
        return false;

    ATL::CSid cSID1((SID*)pSID1);
    LocalFree(pSID1);

    return cSID1 == m_sidCurrentUserSID;
}
