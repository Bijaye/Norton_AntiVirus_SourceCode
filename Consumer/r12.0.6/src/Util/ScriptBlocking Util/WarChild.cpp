// WarChild.cpp: implementation of the CWarChild class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "WarChild.h"
#include "tchar.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CWarChild::CWarChild() :
    m_VerifyFileA(0),
    m_VerifyFileW(0),
    m_SetSignatureA(0),
    m_SetSignatureW(0),
    m_GetSignatureA(0),
    m_GetSignatureW(0),
    m_ApplySignatureA(0),
    m_ApplySignatureW(0),
    m_GetExclusionA(0),
    m_GetExclusionW(0),
    m_RegisterCallbackA(0),
    m_RegisterCallbackW(0),
    m_Enable(0),
    m_Uninstall(0),
    m_DllRegisterServer(0),
    m_hInst(0)
{
}

CWarChild::~CWarChild()
{
    if (m_hInst)
        FreeLibrary(m_hInst);
}

LONG CWarChild::VerifyFile(LPSTR szFileName)
{
    if (m_VerifyFileA)
        return m_VerifyFileA(szFileName);

    return E_WC_ACCESSDENIED;
}

LONG CWarChild::VerifyFile(LPWSTR wszFileName)
{
    if (m_VerifyFileW)
        return m_VerifyFileW(wszFileName);

    return E_WC_ACCESSDENIED;
}

LONG CWarChild::SetSignature(LPSTR szSignature)
{
    if (m_SetSignatureA)
        return m_SetSignatureA(szSignature);

    return E_WC_ACCESSDENIED;
}

LONG CWarChild::SetSignature(LPWSTR wszSignature)
{
    if (m_SetSignatureW)
        return m_SetSignatureW(wszSignature);

    return E_WC_ACCESSDENIED;
}

LONG CWarChild::GetSignature(LPSTR szSignature, size_t dwSigSize)
{
    if (m_GetSignatureA)
        return m_GetSignatureA(szSignature, dwSigSize);

    return E_WC_ACCESSDENIED;
}

LONG CWarChild::GetSignature(LPWSTR wszSignature, size_t dwSigSize)
{
    if (m_GetSignatureW)
        return m_GetSignatureW(wszSignature, dwSigSize);

    return E_WC_ACCESSDENIED;
}

LONG CWarChild::ApplySignature(LPSTR szFileName)
{
    if (m_ApplySignatureA)
        return m_ApplySignatureA(szFileName);

    return E_WC_ACCESSDENIED;
}

LONG CWarChild::ApplySignature(LPWSTR wszFileName)
{
    if (m_ApplySignatureW)
        return m_ApplySignatureW(wszFileName);

    return E_WC_ACCESSDENIED;
}

LONG CWarChild::GetExclusion(DWORD dwIndex, LPSTR szFileName, size_t dwFileSize)
{
    if (m_GetExclusionA)
        m_GetExclusionA(dwIndex, szFileName, dwFileSize);

    return E_WC_ACCESSDENIED;
}

LONG CWarChild::GetExclusion(DWORD dwIndex, LPWSTR wszFileName, size_t dwFileSize)
{
    if (m_GetExclusionW)
        m_GetExclusionW(dwIndex, wszFileName, dwFileSize);

    return E_WC_ACCESSDENIED;
}

LONG CWarChild::RegisterCallback(LPSTR szFileName, LPSTR szRoutineName)
{
    if (m_RegisterCallbackA)
        return m_RegisterCallbackA(szFileName, szRoutineName);

    return E_WC_ACCESSDENIED;
}

LONG CWarChild::RegisterCallback(LPWSTR wszFileName, LPWSTR wszRoutineName)
{
    if (m_RegisterCallbackW)
        return m_RegisterCallbackW(wszFileName, wszRoutineName);

    return E_WC_ACCESSDENIED;
}

LONG CWarChild::Enable(DWORD dwEnable)
{
    if (m_Enable)
        return m_Enable(dwEnable);

    return E_WC_ACCESSDENIED;
}

LONG CWarChild::Uninstall(BOOL bRemove)
{
    if (m_Uninstall)
        return m_Uninstall(bRemove);

    return E_WC_ACCESSDENIED;
}

LONG CWarChild::DllRegisterServer()
{
    if (m_DllRegisterServer)
        return m_DllRegisterServer();

    return E_WC_ACCESSDENIED;
}

LONG CWarChild::Register()
{
    if (m_Register)
        return m_Register();

    return E_WC_ACCESSDENIED;
}


BOOL CWarChild::Init( LPCTSTR lpszWarChildPath /* NULL */)
{
    // If this is installing WC then the lpszWarChildPath parameter will be set so we know
    // where to load it from. Otherwise we just look at the reg key.
    //
    TCHAR szDLL[MAX_PATH];
    LPTSTR lpszDLL = szDLL;

    if ( !lpszWarChildPath )
    {
        HKEY hKey;

        if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, WC_REG_PATH, 0, KEY_QUERY_VALUE, &hKey) != ERROR_SUCCESS)
            return FALSE;

        DWORD dwSize = sizeof(szDLL);

        if (RegQueryValueEx(hKey, "", NULL, NULL, (LPBYTE) szDLL, &dwSize) != ERROR_SUCCESS)
            return FALSE;

        RegCloseKey(hKey);
    }
    else
    {
        _tcscpy ( lpszDLL, lpszWarChildPath );
    }


    m_hInst = LoadLibraryEx(lpszDLL, NULL, LOAD_WITH_ALTERED_SEARCH_PATH);

    if (m_hInst == NULL)
        return FALSE;

    m_VerifyFileA = (type_VerifyFileA) GetProcAddress(m_hInst, "VerifyFileA");
    if (m_VerifyFileA == NULL)
        return FALSE;
    m_VerifyFileW = (type_VerifyFileW) GetProcAddress(m_hInst, "VerifyFileW");
    if (m_VerifyFileW == NULL)
        return FALSE;
    m_SetSignatureA = (type_SetSignatureA) GetProcAddress(m_hInst, "SetSignatureA");
    if (m_SetSignatureA == NULL)
        return FALSE;
    m_SetSignatureW = (type_SetSignatureW) GetProcAddress(m_hInst, "SetSignatureW");
    if (m_SetSignatureW == NULL)
        return FALSE;
    m_GetSignatureA = (type_GetSignatureA) GetProcAddress(m_hInst, "GetSignatureA");
    if (m_GetSignatureA == NULL)
        return FALSE;
    m_GetSignatureW = (type_GetSignatureW) GetProcAddress(m_hInst, "GetSignatureW");
    if (m_GetSignatureW == NULL)
        return FALSE;
    m_ApplySignatureA = (type_ApplySignatureA) GetProcAddress(m_hInst, "ApplySignatureA");
    if (m_ApplySignatureA == NULL)
        return FALSE;
    m_ApplySignatureW = (type_ApplySignatureW) GetProcAddress(m_hInst, "ApplySignatureW");
    if (m_ApplySignatureW == NULL)
        return FALSE;
    m_GetExclusionA = (type_GetExclusionA) GetProcAddress(m_hInst, "GetExclusionA");
    if (m_GetExclusionA == NULL)
        return FALSE;
    m_GetExclusionW = (type_GetExclusionW) GetProcAddress(m_hInst, "GetExclusionW");
    if (m_GetExclusionW == NULL)
        return FALSE;
    m_RegisterCallbackA = (type_RegisterCallbackA) GetProcAddress(m_hInst, "RegisterCallbackA");
    if (m_RegisterCallbackA == NULL)
        return FALSE;
    m_RegisterCallbackW = (type_RegisterCallbackW) GetProcAddress(m_hInst, "RegisterCallbackW");
    if (m_RegisterCallbackW == NULL)
        return FALSE;
    m_Enable = (type_Enable) GetProcAddress(m_hInst, "Enable");
    if (m_Enable == NULL)
        return FALSE;
    m_Uninstall = (type_Uninstall) GetProcAddress(m_hInst, "Uninstall");
    if (m_Uninstall == NULL)
        return FALSE;
    m_DllRegisterServer = (type_DllRegisterServer) GetProcAddress(m_hInst, "DllRegisterServer");
    if (m_DllRegisterServer == NULL)
        return FALSE;
    m_Register = (type_DllRegisterServer) GetProcAddress(m_hInst, "Register");
    if (m_Register == NULL)
        return FALSE;

    return TRUE;
}
