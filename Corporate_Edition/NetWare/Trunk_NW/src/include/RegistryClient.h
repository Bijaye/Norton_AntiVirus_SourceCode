// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright © 2005 Symantec Corporation. All rights reserved.
//----------------------------------------------------------------------------
// Provides Registry services over IPC.
//----------------------------------------------------------------------------


#ifndef REGISTRY_CLIENT_H_INCLUDED
#define REGISTRY_CLIENT_H_INCLUDED


#ifdef __cplusplus
extern "C"
{
#endif

#if !(defined(NLM) || defined(SYM_LIBC_NLM))
DWORD ipcRegOpenKey(HKEY base, LPCSTR key, PHKEY out);
DWORD ipcRegOpenKeyEx(HKEY hKey, LPCSTR lpSubKey, DWORD ulOptions, DWORD samDesired, HKEY *phkResult);
DWORD ipcRegCreateKey(HKEY base, LPCSTR key, PHKEY out);
DWORD ipcRegCreateKeyEx(HKEY hKey, LPCSTR lpSubKey, DWORD Reserved, LPSTR lpClass, DWORD dwOptions,
                        DWORD samDesired, DWORD *lpSecurityAttributes, HKEY *phkResult, DWORD  *lpdwDisposition);
DWORD ipcRegDeleteKey(HKEY base, LPCSTR key);
DWORD ipcRegDeleteValue(HKEY base, LPCSTR value);
DWORD ipcRegCreateKey(HKEY base, LPCSTR key, PHKEY out);
DWORD ipcRegSetValueEx(HKEY hkey, LPCSTR value, DWORD reserved, DWORD type, const BYTE* data, DWORD cbdata);
DWORD ipcRegQueryValueEx(HKEY hkey, LPCSTR value, LPDWORD reserved, LPDWORD type, LPBYTE data, LPDWORD cbdata);
DWORD ipcRegEnumValue(HKEY hkey, DWORD index, LPSTR value, LPDWORD cbvalue, LPDWORD reserved, LPDWORD type, LPBYTE data, LPDWORD cbdata);
DWORD ipcRegEnumKey(HKEY hkey, DWORD index, LPSTR key, DWORD cbkey);
DWORD ipcRegEnumKeyEx(HKEY hkey, DWORD index, LPSTR key, LPDWORD cbKey, LPDWORD reserved, LPSTR keyClass, LPDWORD cbClass, PFILETIME lastWriteTime);
DWORD ipcRegCloseKey(HKEY hkey);
LONG  ipcRegNotifyChangeKeyValue(HKEY hKey, BOOL bWatchSubtree, DWORD dwNotifyFilter, HANDLE hEvent, BOOL fAsynchronous);

#if !defined(REGISTRY_CLIENT_IPC_EXPLICIT)
#undef RegOpenKey
#undef RegOpenKeyEx
#undef RegCreateKey
#undef RegCreateKeyEx
#undef RegDeleteKey
#undef RegDeleteValue
#undef RegCloseKey
#undef RegSetValueEx
#undef RegQueryValueEx
#undef RegEnumValue
#undef RegEnumKeyEx
#undef RegEnumKey
#undef RegFlushKey
#undef RegNotifyChangeKeyValue
#undef DuplicateHandle
#undef RegOpenKeyEx

#define RegOpenKey              ipcRegOpenKey
#define RegOpenKeyEx            ipcRegOpenKeyEx
#define RegCreateKey            ipcRegCreateKey
#define RegCreateKeyEx          ipcRegCreateKeyEx
#define RegDeleteKey            ipcRegDeleteKey
#define RegDeleteValue          ipcRegDeleteValue
#define RegCloseKey             ipcRegCloseKey
#define RegSetValueEx           ipcRegSetValueEx
#define RegQueryValueEx         ipcRegQueryValueEx
#define RegEnumValue            ipcRegEnumValue
#define RegEnumKeyEx            ipcRegEnumKeyEx
#define RegEnumKey              ipcRegEnumKey
#define RegFlushKey             ipcRegFlushKey
#define RegNotifyChangeKeyValue ipcRegNotifyChangeKeyValue
#define DuplicateHandle         ipcDuplicateHandle
#define RegOpenKeyEx            ipcRegOpenKeyEx
#endif // !defined(REGISTRY_CLIENT_IPC_EXPLICIT)

#endif // !(defined(NLM) || defined(SYM_LIBC_NLM))

#ifdef __cplusplus
}
#endif

#endif // REGISTRY_CLIENT_H_INCLUDED
