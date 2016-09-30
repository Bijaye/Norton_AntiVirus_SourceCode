#include "platform.h"
#include <windowsx.h>
#include <tchar.h>

#include "DefUtlEx.h"

const char AVENGEDEFS_VAL[] = "AVENGEDEFS";
const char SYMANTEC_INSTALLED_APPS_KEY[] = "SOFTWARE\\Symantec\\InstalledApps";


//*******************************************************************
//*******************************************************************
CDefUtilsEx::CDefUtilsEx() : 
m_hDefsUpdated( INVALID_HANDLE_VALUE )
{
    m_DefUtilsLoader.CreateObject(m_pDefUtils.m_p);
}

//*******************************************************************
//*******************************************************************
CDefUtilsEx::~CDefUtilsEx()
{
	//
	// Opss...  Someone didn't call ColseNotification!!!
	//
	SYM_ASSERT( m_hDefsUpdated == INVALID_HANDLE_VALUE );

    // Release the internal object before the loader goes out of scope
    if( m_pDefUtils.m_p )
        m_pDefUtils.Release();
}

//*******************************************************************
//
// Function: char StringGetLastChar(LPCTSTR lpszString)
//
// Description: This function returns the last character in a string.
//              Note that it is a WCHAR.
//
//*******************************************************************
char CDefUtilsEx::StringGetLastChar(LPCSTR lpszString)
{
    LPCSTR     lpszLast = lpszString;
    char       cLast;

    while (*lpszString)
        {
        lpszLast = lpszString;
        lpszString = AnsiNext(lpszString);
        }
    //if (IsDBCSLeadByte(*lpszLast))
    //    cLast = *((LPWSTR) lpszLast);
    //else
        cLast = *lpszLast;
    return(cLast);
}

//*******************************************************************
//
// Function: RemoveLastBackslash()
//
// Description: This function removes any trailing backslash from 
//              a full path.
//
//*******************************************************************

void CDefUtilsEx::RemoveLastBackslash(LPSTR pszDir)
{
    char   cLast;
    cLast = StringGetLastChar(pszDir);
    if (cLast == '\\')
        *(pszDir + strlen(pszDir) - 1) = '\0';
}

//*******************************************************************
//
// Function: DirectoryExists()
//
// Description: This function determines if given directory exists on
//              the hard drive.
//
//*******************************************************************

BOOL CDefUtilsEx::DirectoryExists(LPCSTR pszDir)
{
    char szDir[_MAX_PATH];
    strcpy(szDir, pszDir);
    RemoveLastBackslash(szDir);
    DWORD dwAttrib = GetFileAttributes(szDir);
    BOOL bRetVal = FALSE;
    if ((dwAttrib != 0xffffffff) && (dwAttrib & FILE_ATTRIBUTE_DIRECTORY))
        bRetVal = TRUE;

    return (bRetVal);
}

//*******************************************************************
//
// Function: GetVirusDefsDir()
//
// Description: This finds the location of the shared virus 
//              definitions.  It returns TRUE if the directory
//              was found, FALSE otherwise.
//
//*******************************************************************

BOOL CDefUtilsEx::GetVirusDefsDir(LPSTR pszDefsDir, int nMaxCount)
{
    char szDir[_MAX_PATH];
    DWORD dwType = 0;
    DWORD dwSize = (DWORD) nMaxCount;
    HKEY  hKey = NULL;
    BOOL  bFoundDir = FALSE;
    
    // open "InstalledApps" key
    if (ERROR_SUCCESS == RegOpenKeyEx(HKEY_LOCAL_MACHINE, SYMANTEC_INSTALLED_APPS_KEY, 0, KEY_READ, &hKey))
        {
        // check for "AVENGEDEFS" value
        if (ERROR_SUCCESS == RegQueryValueEx(hKey, AVENGEDEFS_VAL, 0, &dwType, (LPBYTE)szDir, &dwSize))
            {
            if ((dwType == REG_SZ) && strlen(szDir))
                {
                // convert pathname to short name, if possible
                char szShort[_MAX_PATH];
                if (GetShortPathName(szDir, szShort, sizeof(szShort)))
                    strcpy(szDir, szShort);
                    
                strcpy(pszDefsDir, szDir);
                
                if (DirectoryExists(pszDefsDir))
                    bFoundDir = TRUE;
                }
            }
        RegCloseKey(hKey);
        }

    return (bFoundDir);
}

//****************************************************************************
//
//****************************************************************************
BOOL CDefUtilsEx::GetNotificationHandle( HANDLE* pNotifyHandle )
{
	TCHAR szIncomingFolder[ SYM_MAX_PATH ];

	*pNotifyHandle = INVALID_HANDLE_VALUE;	

	if( ! GetVirusDefsDir( szIncomingFolder, sizeof(szIncomingFolder) ) )
	{ 
		return FALSE;
	}

	m_hDefsUpdated = FindFirstChangeNotification( szIncomingFolder, 
	                                              TRUE, 
	                                              FILE_NOTIFY_CHANGE_FILE_NAME );

	if( INVALID_HANDLE_VALUE == m_hDefsUpdated )
	{
		return FALSE;
	}
	else
	{
		*pNotifyHandle = m_hDefsUpdated;
	}

	return TRUE;
}

//****************************************************************************
//
//////////////////////////////////////////////////////////////////////////////
// Author: CBROWN - 08/30/99
//****************************************************************************
void CDefUtilsEx::SetNextNotification( void )
{
	//
	// Opps...  We've got an invalied hadle!
	//
	SYM_ASSERT( INVALID_HANDLE_VALUE != m_hDefsUpdated );

	FindNextChangeNotification( m_hDefsUpdated );
}

//****************************************************************************
//
//////////////////////////////////////////////////////////////////////////////
// Author: CBROWN - 08/30/99
//****************************************************************************
void CDefUtilsEx::CloseNotification( void )
{
	//
	// Opps...  We've got an invalied hadle!
	//
	SYM_ASSERT( INVALID_HANDLE_VALUE != m_hDefsUpdated );

    FindCloseChangeNotification( m_hDefsUpdated );
 
	//
	// Reset out state just in case...
	//
    m_hDefsUpdated = INVALID_HANDLE_VALUE;
}	


//****************************************************************************
//
//////////////////////////////////////////////////////////////////////////////
// DefUtils Pass through functions to support using the defutils
// interface.
//****************************************************************************
bool CDefUtilsEx::InitWindowsApp(const char *pszAppID)
{
    if( !m_pDefUtils )
    {
        CCTRACEE("CDefUtilsEx::InitWindowsApp() - No defutils object");
        return false;
    }

    return m_pDefUtils->InitWindowsApp(pszAppID);
}

bool CDefUtilsEx::GetCurrentDefs(char *pszDir, int nByteSize)
{
    if( !m_pDefUtils )
    {
        CCTRACEE("CDefUtilsEx::GetCurrentDefs() - No defutils object");
        return false;
    }

    return m_pDefUtils->GetCurrentDefs(pszDir, nByteSize);
}

bool CDefUtilsEx::SetCurrentDefs(const char *pszDir)
{
    if( !m_pDefUtils )
    {
        CCTRACEE("CDefUtilsEx::SetCurrentDefs() - No defutils object");
        return false;
    }

    return m_pDefUtils->SetCurrentDefs(pszDir);
}

DURESULT CDefUtilsEx::GetLastResult()
{
    if( !m_pDefUtils )
    {
        CCTRACEE("CDefUtilsEx::GetLastResult() - No defutils object");
        return DU_E_INVALID_PARAMETER;
    }

    return m_pDefUtils->GetLastResult();
}

bool CDefUtilsEx::SetDoAuthenticationCheck(bool bAuthenticate)
{
    if( !m_pDefUtils )
    {
        CCTRACEE("CDefUtilsEx::SetDoAuthenticationCheck() - No defutils object");
        return false;
    }

    return m_pDefUtils->SetDoAuthenticationCheck(bAuthenticate);
}

bool CDefUtilsEx::GetCurrentDefsDate(LPWORD pwYear, LPWORD pwMonth, LPWORD pwDay, LPDWORD pdwRev)
{
    if( !m_pDefUtils )
    {
        CCTRACEE("CDefUtilsEx::GetCurrentDefsDate() - No defutils object");
        return false;
    }

    return m_pDefUtils->GetCurrentDefsDate(pwYear, pwMonth, pwDay, pdwRev);
}

bool CDefUtilsEx::UseNewestDefs(bool *pbChanged)
{
    if( !m_pDefUtils )
    {
        CCTRACEE("CDefUtilsEx::UseNewestDefs() - No defutils object");
        return false;
    }

    return m_pDefUtils->UseNewestDefs(pbChanged);
}

bool CDefUtilsEx::IsAuthenticDefs(const char *pcszDefsDir, bool *pbTrusted)
{
    if( !m_pDefUtils )
    {
        CCTRACEE("CDefUtilsEx::IsAuthenticDefs() - No defutils object");
        return false;
    }

    return m_pDefUtils->IsAuthenticDefs(pcszDefsDir, pbTrusted);
}

bool CDefUtilsEx::AreCurrentDefsAuthentic( void )
{
    if( !m_pDefUtils )
    {
        CCTRACEE("CDefUtilsEx::AreCurrentDefsAuthentic() - No defutils object");
        return false;
    }

    char szCurDefsDir[MAX_PATH+1] = {0};
    if( m_pDefUtils->GetCurrentDefs(szCurDefsDir, MAX_PATH) )
    {
        bool bTrusted = true;
        CCTRACEI(_T("CDefUtilsEx::AreCurrentDefsAuthentic() - Authentication check begin."));
        if( m_pDefUtils->IsAuthenticDefs(szCurDefsDir, &bTrusted) && !bTrusted)
        {
            CCTRACEE(_T("CDefUtilsEx::AreCurrentDefsAuthentic() - The current definiton set in use is not valid. %s"), szCurDefsDir);
            return false;
        }
        CCTRACEI(_T("CDefUtilsEx::AreCurrentDefsAuthentic() - Authentication check end."));
    }
    else
    {
        CCTRACEE(_T("CDefUtilsEx::AreCurrentDefsAuthentic() - Failed to get the current defs directory. Assuming current defs are bad"));
        return false;
    }

    CCTRACEI(_T("CDefUtilsEx::AreCurrentDefsAuthentic() - The current defs are authentic: %s"), szCurDefsDir);
    return true;
}