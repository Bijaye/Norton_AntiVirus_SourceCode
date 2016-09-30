// WarChild.h: interface for the CWarChild class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WARCHILD_H__5C3DE549_91BC_40FF_ABBB_AE6DCB91E783__INCLUDED_)
#define AFX_WARCHILD_H__5C3DE549_91BC_40FF_ABBB_AE6DCB91E783__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "warchild_i.h"

// Should this be in WarChild_i.h ???
//
#define WC_REG_PATH _T("Software\\Symantec\\ScriptBlocking")

class CWarChild  
{
private:
	HMODULE m_hInst;
    type_VerifyFileA m_VerifyFileA;
    type_VerifyFileW m_VerifyFileW;
    type_SetSignatureA m_SetSignatureA;
    type_SetSignatureW m_SetSignatureW;
    type_GetSignatureA m_GetSignatureA;
    type_GetSignatureW m_GetSignatureW;
    type_ApplySignatureA m_ApplySignatureA;
    type_ApplySignatureW m_ApplySignatureW;
    type_GetExclusionA m_GetExclusionA;
    type_GetExclusionW m_GetExclusionW;
    type_RegisterCallbackA m_RegisterCallbackA;
    type_RegisterCallbackW m_RegisterCallbackW;
    type_Enable m_Enable;
    type_Uninstall m_Uninstall;
    type_DllRegisterServer m_DllRegisterServer;
    type_DllRegisterServer m_Register;

public:
	BOOL Init( LPCTSTR lpszWarChildPath = NULL );
	LONG Enable(DWORD dwEnable);
	LONG RegisterCallback(LPWSTR wszFileName, LPWSTR wszRoutineName);
	LONG RegisterCallback(LPSTR szFileName, LPSTR szRoutineName);
	LONG GetExclusion(DWORD dwIndex, LPWSTR wszFileName, size_t dwFileSize);
	LONG GetExclusion(DWORD dwIndex, LPSTR szFileName, size_t dwFileSize);
	LONG ApplySignature(LPWSTR wszFileName);
	LONG ApplySignature(LPSTR szFileName);
	LONG GetSignature(LPWSTR wszSignature, size_t dwSigSize);
	LONG GetSignature(LPSTR szSignature, size_t dwSigSize);
	LONG SetSignature(LPWSTR wszSignature);
	LONG SetSignature(LPSTR szSignature);
	LONG VerifyFile(LPWSTR wszFileName);
	LONG VerifyFile(LPSTR szFileName);
    LONG Uninstall(BOOL bRemove);
    LONG DllRegisterServer();
    LONG Register();
	CWarChild();
	virtual ~CWarChild();

};

#endif // !defined(AFX_WARCHILD_H__5C3DE549_91BC_40FF_ABBB_AE6DCB91E783__INCLUDED_)
