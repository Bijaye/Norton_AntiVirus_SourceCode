// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
#ifndef __REGKEY_H
#define	__REGKEY_H

class CRegistryKey
{
public:
	CRegistryKey(HKEY hKey, LPCTSTR key, BOOL create = TRUE);
	~CRegistryKey();

  LONG ReadKeyValue(LPTSTR value, long *valueBufferSize);

	LONG ReadValue(LPCTSTR name, LPTSTR value, LPDWORD valueBufferSize);
	LONG WriteValue(LPCTSTR name, LPTSTR value);

  LONG ReadValueEnum(int numberValue, LPTSTR name, LPDWORD nameSize);
	LONG ReadValue(LPCTSTR name, DWORD &numberValue);
	LONG WriteValue(LPCTSTR name, DWORD numberValue);
	LONG ReadValue(LPCTSTR name, LPVOID value, LPDWORD valueSize);
	LONG WriteValue(LPCTSTR name, LPVOID buffer, DWORD bufferSize);

	LONG DeleteKey(LPCTSTR key);
	LONG DeleteValue(LPCTSTR value);
	LONG GetNumberOfValues();
	LONG GetNumberOfSubkeys();
	BOOL IsNewKey() { return (fIsNewKey); }

	LONG Open();
	LONG Create();
private:
	LPTSTR			fKey;
	HKEY			fHKey;
	HKEY			fInHKey;
	LONG			fLastError;
	BOOL			fIsNewKey;
};

#endif
