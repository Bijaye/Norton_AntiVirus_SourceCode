

#ifndef INSTOSHELPER_H
#define INSTOSHELPER_H

typedef BOOL (WINAPI *LPFN_ISWOW64PROCESS) (HANDLE hProcess,PBOOL Wow64Process);
typedef UINT (WINAPI *LPFN_GETSYSTEMWOW64DIRECTORY)(LPTSTR lpBuffer,UINT uSize);

#ifdef	UNICODE			
#define	GET_SYSTEM_WOW64_DIRECTORY_NAME	GET_SYSTEM_WOW64_DIRECTORY_NAME_A_W
#else
#define	GET_SYSTEM_WOW64_DIRECTORY_NAME GET_SYSTEM_WOW64_DIRECTORY_NAME_A_A
#endif

UINT _GetSystemWOW64Directory(LPTSTR lpBuffer,UINT uSize);
BOOL	SysWow64DirExists();
BOOL	IsOS64Bit();
BOOL	IsWow64();
void	SetRegKeyValueSZ(HKEY hParentKey, int iOption,ATL::CString szKey,ATL::CString szName,ATL::CString szValue);
void	SetRegProductName(ATL::CString szRegKeyName,ATL::CString szRegKeyValue);


const TCHAR szNAVToolbarClsidKey[] = _T("\\CLSID\\{C4069E3A-68F1-403E-B40E-20066696354B}");
const TCHAR szNAVToolbarClsidValueName[] = _T("");

const TCHAR szNAVToolbarKey[] = _T("Microsoft\\Internet Explorer\\Toolbar");
const TCHAR szNAVToolbarGuid[] =_T("{C4069E3A-68F1-403E-B40E-20066696354B}");
#endif
