#ifndef WINREGISTRY_H
#define WINREGISTRY_H

class WinRegistry
{
public:
	WinRegistry(HKEY parentKey, const TCHAR* value) : key(0), valid(false)
	{
		valid = (ERROR_SUCCESS == RegOpenKey(parentKey, value, &key));
	};

	WinRegistry(WinRegistry& winReg, const TCHAR* value) : key(0), valid(false)
	{
		if (winReg.ValidKey())
			valid = (ERROR_SUCCESS == RegOpenKey(winReg.key, value, &key));
	};

	~WinRegistry() { if (valid) RegCloseKey(key); };

	bool CreateKey(HKEY parentKey, const TCHAR* newKeyName)
	{
		valid = (ERROR_SUCCESS == RegCreateKeyEx(parentKey, newKeyName, 0, "",
												REG_OPTION_NON_VOLATILE,
												KEY_ALL_ACCESS, NULL,
												&key, NULL));

		return valid;
	}

	bool DeleteValue(const TCHAR* item)
	{
		return ERROR_SUCCESS == RegDeleteValue(key, item);
	}

	bool	QueryValue(const char* item, uchar* value, ulong& valueLength)
	{
		DWORD	type;

		return ERROR_SUCCESS == RegQueryValueEx(key, item, 0, &type, value, &valueLength);
	};

	bool	QueryValue(const TCHAR* item, DWORD* value)
	{
		DWORD	type;
		DWORD	valueLength = sizeof(DWORD);

		return ERROR_SUCCESS == RegQueryValueEx(key, item, 0, &type, (uchar*)value,
												&valueLength);
	}

	bool	SetValue(const TCHAR* item, const uchar* value)
	{
		return ERROR_SUCCESS == RegSetValueEx(key, item, 0, REG_SZ, value, strlen((const char *)value)+1);
	}

	bool	SetValue(const TCHAR* item, const DWORD& value)
	{
		return ERROR_SUCCESS == RegSetValueEx(key, item, 0, REG_DWORD, (const uchar *)&value, sizeof(DWORD));
	}

	bool	ValidKey(void)	{ return valid; };

	HKEY	Key(void)		{ return key; };

	enum ValueType { vtDword, vtString, vtOther  };

	bool	GetValueType(const TCHAR* item, ValueType& vType)
	{
		DWORD	type;

		if (ERROR_SUCCESS == RegQueryValueEx(key, item, 0, &type, NULL, NULL))
		{
			if (REG_SZ == type)
				vType = vtString;
			else if (REG_DWORD == type || REG_DWORD_LITTLE_ENDIAN == type ||
					 REG_DWORD_BIG_ENDIAN == type)
				vType = vtDword;
			else
				vType = vtOther;

			return true;
		}
		else
			return false;
	}

private:
	HKEY	key;
	bool	valid;
};
#endif
