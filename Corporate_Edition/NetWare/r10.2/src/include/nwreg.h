// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
#ifndef _NWREG_H_
#define _NWREG_H_

#ifdef __cplusplus
extern "C"
{
#endif

typedef DWORD HKEY;
typedef HKEY *PHKEY;


#define ERROR_NO_PATH			2L		// returned when a key or value specifier is invalid
#define ERROR_NO_MEMORY			14L		// returned when a memory allocation fails when creating a key or value or opening the registry database
#define ERROR_SEMAPHORE_IN_USE	102L	// returned from RegNotifyChangeKeyValue when the HKEY already has an associated semaphore
#define ERROR_BAD_DATABASE		1009L	// Returned when the registry database has become corrupted
#define ERROR_BAD_KEY			1010L	// returned when an invalid HKEY is passed into a Reg function
#define ERROR_NO_ROOM			1013L	// returned when a data buffer or string passed into a function is too small to hold the data
#define ERROR_EMPTY				1015L	// returned when the registry database is first accessed and the databse file does not exists or cannot be opened
#define ERROR_NO_DATABASE		1016L	// Returned when closing or saving the database and a file IO error occurs
#define ERROR_KEY_HAS_CHILDREN	1020L	// returned when RegDeleteKey is called and the specified key has sub-keys

// Registry access flags -- not implemented on NetWare -- provided for cross-platform friendliness
#define KEY_QUERY_VALUE         0
#define KEY_SET_VALUE           0
#define KEY_CREATE_SUB_KEY      0
#define KEY_ENUMERATE_SUB_KEYS  0
#define KEY_NOTIFY              0
#define KEY_CREATE_LINK         0
#define KEY_WOW64_32KEY         0
#define KEY_WOW64_64KEY         0
#define KEY_WOW64_RES           0
#define KEY_READ                0
#define KEY_WRITE               0
#define KEY_EXECUTE             0
#define KEY_ALL_ACCESS          0

// Types of data values:
#define REG_NONE 				( 0 )   // No value type
#define REG_SZ 					( 1 )   // Unicode nul terminated string
#define REG_EXPAND_SZ 			( 2 )   // Unicode nul terminated string (with environment variable references)
#define REG_BINARY 				( 3 )   // Free form binary
#define REG_DWORD 				( 4 )   // 32-bit number
#define REG_DWORD_LITTLE_ENDIAN	( 4 )   // 32-bit number (same as REG_DWORD)
#define REG_DWORD_BIG_ENDIAN	( 5 )   // 32-bit number
#define REG_LINK				( 6 )   // Symbolic Link (unicode)
#define REG_MULTI_SZ 			( 7 )   // Multiple Unicode strings
#define REG_NOTIFY_CHANGE_LAST_SET ( 100 )
#define HKEY_LOCAL_MACHINE          (( HKEY ) 0x80000002 )
#define HKEY_CURRENT_USER        (( HKEY ) 0x80000002 )


#define REG_OPTION_NON_VOLATILE		(0x00000000L)	 // Key is preserved

//
// Key creation/open disposition
//
#define REG_CREATED_NEW_KEY         (0x00000001L)   // New Registry Key created
#define REG_OPENED_EXISTING_KEY     (0x00000002L)   // Existing Key opened

#if defined NLM

// anybody needing dynamic registry should add a define to their make
// and add it here

    #ifdef VPSTART                  
        #define USE_DYNAMIC_REG
    #endif
    
#endif


#ifdef USE_DYNAMIC_REG
typedef void (*tStartRegEditThread)(void*);
typedef DWORD (*tRegOpenKey)(HKEY ,LPCSTR ,PHKEY );
typedef DWORD (*tRegCreateKey)(HKEY ,LPCSTR ,PHKEY );
typedef DWORD (*tRegCreateKeyEx)(HKEY    hKey,
                                LPCTSTR lpSubKey,
                                DWORD   Reserved,
                                LPTSTR  lpClass,
                                DWORD   dwOptions,
                                DWORD   samDesired,
                                DWORD  *lpSecurityAttributes,
                                PHKEY   phkResult,
                                DWORD  *lpdwDisposition);
typedef DWORD (*tRegDeleteKey)(HKEY ,LPCSTR );
typedef DWORD (*tRegDeleteValue)(HKEY ,LPCSTR );
typedef DWORD (*tRegCloseKey)(HKEY );
typedef DWORD (*tRegSetValueEx)(HKEY ,LPCSTR ,DWORD ,DWORD ,const BYTE *,DWORD );
typedef DWORD (*tRegQueryValueEx)(HKEY ,LPCSTR ,LPDWORD ,LPDWORD ,LPBYTE ,LPDWORD );
typedef DWORD (*tRegEnumValue)(HKEY ,DWORD ,LPSTR ,LPDWORD ,LPDWORD ,LPDWORD ,LPBYTE ,LPDWORD );
typedef DWORD (*tRegEnumKeyEx)(HKEY ,DWORD ,LPSTR ,LPDWORD ,LPDWORD ,LPSTR ,LPDWORD ,PFILETIME );
typedef DWORD (*tRegEnumKey)(HKEY ,DWORD ,LPSTR ,DWORD );
typedef DWORD (*tRegFlushKey)(HKEY );
typedef DWORD (*tRegCloseDatabase)(BOOL);
typedef DWORD (*tRegOpenDatabase)(void);
typedef DWORD (*tRegSaveDatabase)(void);
typedef DWORD (*tRegNotifyChangeKeyValue)(HKEY ,BOOL ,DWORD ,HANDLE ,BOOL );
typedef DWORD (*tDuplicateHandle)(DWORD ,HKEY ,DWORD ,HKEY *,DWORD ,BOOL ,DWORD );
typedef DWORD (*tRegOpenKeyEx)(HKEY hKey, LPCSTR lpSubKey, DWORD ulOptions, DWORD samDesired, HKEY *phkResult);

typedef BOOL (*tRawDumpDatabase)(char *);
typedef BOOL (*tFileDumpDatabase)(HKEY,char *);
typedef int tDumpFileNum;

extern tStartRegEditThread StartRegEditThread;
extern tRegOpenKey RegOpenKey;
extern tRegCreateKey RegCreateKey;
extern tRegCreateKeyEx RegCreateKeyEx;
extern tRegDeleteKey RegDeleteKey;
extern tRegDeleteValue RegDeleteValue;
extern tRegCloseKey RegCloseKey;
extern tRegSetValueEx RegSetValueEx;
extern tRegQueryValueEx RegQueryValueEx;
extern tRegEnumValue RegEnumValue;
extern tRegEnumKeyEx RegEnumKeyEx;
extern tRegEnumKey RegEnumKey;
extern tRegFlushKey RegFlushKey;
extern tRegCloseDatabase RegCloseDatabase;
extern tRegOpenDatabase RegOpenDatabase;
extern tRegSaveDatabase RegSaveDatabase;
extern tRegNotifyChangeKeyValue RegNotifyChangeKeyValue;
extern tDuplicateHandle DuplicateHandle;
extern tRegOpenKeyEx RegOpenKeyEx;
extern tRawDumpDatabase RawDumpDatabase;
extern tFileDumpDatabase FileDumpDatabase;
extern tDumpFileNum DumpFileNum;
#ifdef DEF_REG
tStartRegEditThread StartRegEditThread;
tRegOpenKey RegOpenKey;
tRegCreateKey RegCreateKey;
tRegCreateKeyEx RegCreateKeyEx;
tRegDeleteKey RegDeleteKey;
tRegDeleteValue RegDeleteValue;
tRegCloseKey RegCloseKey;
tRegSetValueEx RegSetValueEx;
tRegQueryValueEx RegQueryValueEx;
tRegEnumValue RegEnumValue;
tRegEnumKeyEx RegEnumKeyEx;
tRegEnumKey RegEnumKey;
tRegFlushKey RegFlushKey;
tRegCloseDatabase RegCloseDatabase;
tRegOpenDatabase RegOpenDatabase;
tRegSaveDatabase RegSaveDatabase;
tRegNotifyChangeKeyValue RegNotifyChangeKeyValue;
tDuplicateHandle DuplicateHandle;
tRegOpenKeyEx RegOpenKeyEx;
tRawDumpDatabase RawDumpDatabase;
tFileDumpDatabase FileDumpDatabase;
tDumpFileNum DumpFileNum;
#endif

#else
DWORD RegOpenKey(HKEY base,LPCSTR key,PHKEY out);
DWORD RegCreateKey(HKEY base,LPCSTR key,PHKEY out);
DWORD RegCreateKeyEx(HKEY    hKey,
                     LPCTSTR lpSubKey,
                     DWORD   Reserved,
                     LPTSTR  lpClass,
                     DWORD   dwOptions,
                     DWORD   samDesired,
                     DWORD  *lpSecurityAttributes,
                     PHKEY   phkResult,
                     DWORD  *lpdwDisposition);
DWORD RegDeleteKey(HKEY base,LPCSTR key);
DWORD RegDeleteValue(HKEY base,LPCSTR value);
DWORD RegCloseKey(HKEY hkey);
DWORD RegSetValueEx(HKEY hkey,LPCSTR value,DWORD reserved,DWORD type, const BYTE* data,DWORD cbdata);
DWORD RegQueryValueEx(HKEY hkey,LPCSTR value,LPDWORD reserved,LPDWORD type,LPBYTE data,LPDWORD cbdata);
DWORD RegEnumValue(HKEY  hkey,DWORD index,LPSTR value,LPDWORD cbvalue,LPDWORD reserved,LPDWORD type,LPBYTE data,LPDWORD cbdata);
DWORD RegEnumKeyEx(HKEY  hkey,DWORD index,LPSTR key,LPDWORD cbkey,LPDWORD reserved,LPSTR Class,LPDWORD cbClass,PFILETIME time);
DWORD RegEnumKey(HKEY hkey,DWORD index,LPSTR key,DWORD cbkey);
DWORD RegFlushKey(HKEY hkey);
DWORD RegNotifyChangeKeyValue(HKEY hkey,BOOL fWatchSubTree,DWORD fdwNotifyFilter,HANDLE  hEvent,BOOL  fAsync);
DWORD DuplicateHandle(DWORD inProcess,HKEY in,DWORD outProcess,HKEY *out,DWORD access,BOOL inherant,DWORD options);
DWORD RegOpenKeyEx(HKEY hKey, LPCSTR lpSubKey, DWORD ulOptions, DWORD samDesired, HKEY *phkResult);
DWORD RegCloseDatabase(BOOL);
DWORD RegOpenDatabase(void);
DWORD RegSaveDatabase(void);

BOOL RawDumpDatabase(char *,size_t);
BOOL FileDumpDatabase(HKEY,char *,size_t);
#endif

#ifdef __cplusplus
}
#endif

/******************************************************************************
DWORD RegOpenKey(HKEY base,LPSTR key,PHKEY out);
	The RegOpenKey function opens the specified key. 

Parameters
----------
base
	Identifies a currently open key or HKEY_LOCAL_MACHINE
	The key opened by the RegOpenKey function is a subkey of the key identified by hkey. 

key
	Points to a NULL-terminated string containing the name of the key to open.
	This key must be a subkey of the key identified by the hkey parameter.
	If this parameter is NULL or a pointer to an empty string, the function
	returns the same handle that was passed in. 

out
	Points to a variable that receives the handle of the opened key.

Return Value
------------
If the function succeeds, the return value is ERROR_SUCCESS.
If the function fails, the return value is an error value.

Remarks
-------
Unlike the RegCreateKey function, RegOpenKey does not create the specified
key if the key does not exist in the database.
******************************************************************************
DWORD RegCreateKey(HKEY base,LPSTR key,PHKEY out);
	The RegCreateKey function behaves identically to RegOpenKey except that
	it creates the specified key if the key does not exist in the database.
******************************************************************************
DWORD RegDeleteKey(HKEY base,LPSTR key);
	The RegDeleteKey function deletes the specified key. This function cannot
	delete a key that has subkeys. 

Parameters
----------
base
	Identifies a currently open key or HKEY_LOCAL_MACHINE
	The key specified by the lpszSubKey parameter must be a subkey of the key
	identified by hkey. 

value
	Points to a null-terminated string specifying the name of the key to delete.
	This parameter cannot be NULL, and the specified key must not have subkeys. 

Return Value
------------
If the function succeeds, the return value is ERROR_SUCCESS.
If the function fails, the return value is an error value.

Remarks
-------
If the function succeeds, RegDeleteKey removes the specified key from the
database. The entire key, including all of its values, is removed. 
******************************************************************************
DWORD RegDeleteValue(HKEY base,LPSTR value);
The RegDeleteValue function removes a named value from the specified registry key. 

Parameters
----------
base
	Identifies a currently open key or HKEY_LOCAL_MACHINE

lpszValue
	Points to a null-terminated string that names the value to remove. If
	this parameter is NULL or points to an empty string, the value set by
	the RegSetValue function is removed. 

Return Value
------------
If the function succeeds, the return value is ERROR_SUCCESS.
If the function fails, the return value is an error value. 
******************************************************************************
DWORD RegCloseKey(HKEY hkey);
The RegCloseKey function releases the handle of the specified key. 

Parameters
----------
hkey
	Identifies the open key to close. 

Return Value
------------
If the function succeeds, the return value is ERROR_SUCCESS.
If the function fails, the return value is an error value. 

Remarks
-------
The handle for a specified key should not be used after it has been closed,
because it will no longer be valid. Key handles should not be left open any
longer than necessary. 
The RegCloseKey function does not necessarily write information to the
registry before returning. If an application must explicitly write
registry information to the hard disk, it can use the RegFlushKey function.
RegFlushKey, however, uses many system resources and should be called only
when necessary.
******************************************************************************
DWORD RegSetValueEx(HKEY hkey,LPSTR value,DWORD reserved,DWORD type,LPBYTE data,DWORD cbdata);
The RegSetValueEx function stores data in the value field of an open registry
key. It can also set additional value and type information for the specified key. 

Parameters
----------
hkey
	Identifies a currently open key or HKEY_LOCAL_MACHINE

value
	Points to a string containing the name of the value to set. If a value with
	this name is not already present in the key, the function adds it to the key. 
	
reserved
	Reserved; must be zero.

type
	Specifies the type of information to be stored as the value's data. 

data
	Points to a buffer containing the data to be stored with the specified value name. 

cbData
	Specifies the size, in bytes, of the information pointed to by the lpbData
	parameter. If the data is of type REG_SZ, REG_EXPAND_SZ, or REG_MULTI_SZ,
	cbData must include the size of the terminating null character. 

Return Value
------------
If the function succeeds, the return value is ERROR_SUCCESS.
If the function fails, the return value is an error value. 

Remarks
-------
Value lengths are limited by available memory. Long values should be stored
as files with the filenames stored in the registry.
******************************************************************************
DWORD RegQueryValueEx(HKEY hkey,LPSTR value,LPDWORD reserved,LPDWORD type,LPBYTE data,LPDWORD cbdata);
The RegQueryValueEx function retrieves the type and data for a specified value name associated with an open registry key. 

Parameters
----------
hkey
	Identifies a currently open key or HKEY_LOCAL_MACHINE

value
	Points to a null-terminated string containing the name of the value to be queried. 

reserved
	Reserved; must be NULL. 

type
	Points to a variable that receives the key's value type. 
	The lpdwType parameter can be NULL if the type is not required. 

data
	Points to a buffer that receives the value's data. This parameter can be
	NULL if the data is not required. 

cbData
	Points to a variable that specifies the size, in bytes, of the buffer
	pointed to by the lpbData parameter. When the function returns, this
	variable contains the size of the data copied to lpbData. 
	If the buffer specified by lpbData parameter is not large enough to hold
	the data, the function returns the value ERROR_MORE_DATA, and stores the
	required buffer size, in bytes, into the variable pointed to by lpcbData. 
	If lpbData is NULL, and lpcbData is non-NULL, the function returns
	ERROR_SUCCESS, and stores the size of the data, in bytes, in the variable
	pointed to by lpcbData. This lets an application determine the best way to
	allocate a buffer for the value key's data. 
	If the data has the REG_SZ, REG_MULTI_SZ or REG_EXPAND_SZ type, then lpData
	will also include the size of the terminating null character. 
	The lpcbData parameter can be NULL only if lpbData is NULL. 

Return Value
------------
If the function succeeds, the return value is ERROR_SUCCESS.
If the function fails, the return value is an error value.  
******************************************************************************
DWORD RegEnumValue(HKEY  hkey,DWORD index,LPSTR value,LPDWORD cbvalue,LPDWORD reserved,LPDWORD type,LPBYTE data,LPDWORD cbdata);
The RegEnumValue function enumerates the values for the specified open registry key. The function copies one indexed value name and data block for the key each time it is called. 

Parameters
----------
hkey
	Identifies a currently open key or HKEY_LOCAL_MACHINE
	The enumerated values are associated with the key identified by hkey. 

index
	Specifies the index of the value to retrieve. This parameter should be zero
	for the first call to the RegEnumValue function and then be incremented for
	subsequent calls. 
	Because values are not ordered, any new value will have an arbitrary index.
	This means that the function may return values in any order. 

value
	Points to a buffer that receives the name of the value, including the
	terminating null character. 

cbvalue
	Points to a variable that specifies the size, in characters, of the buffer
	pointed to by the lpszValue parameter. This size should include the terminating
	null character. When the function returns, the variable pointed to by lpcchValue
	contains the number of characters stored in the buffer. The count returned does
	not include the terminating null character. 

reserved
	Reserved; must be NULL. 

type
	Points to a variable that receives the type code for the value entry.
	
lpbData
	Points to a buffer that receives the data for the value entry. This parameter can be NULL if the data is not required. 

lpcbData

Points to a variable that specifies the size, in bytes, of the buffer pointed to by the lpbData parameter. When the function returns, the variable pointed to by the lpcbData parameter contains the number of bytes stored in the buffer. This parameter can be NULL, only if lpbData is NULL. 

Return Value

If the function succeeds, the return value is ERROR_SUCCESS.
If the function fails, the return value is an error value. 

Remarks

To enumerate values, an application should initially call the RegEnumValue function with the iValue parameter set to zero. The application should then increment iValue and call the RegEnumValue function until there are no more values (until the function returns ERROR_NO_MORE_ITEMS). 
The application can also set iValue to the index of the last value on the first call to the function and decrement the index until the value with index 0 is enumerated. To retrieve the index of the last value, use the ReqQueryInfoKey function. 
While using RegEnumValue, an application should not call any registration functions that might change the key being queried. 
The key identified by the hkey parameter must have been opened with KEY_QUERY_VALUE access. To open the key, use the RegCreateKeyEx or RegOpenKeyEx function.

To determine the maximum size of the name and data buffers, use the RegQueryInfoKey function.
******************************************************************************
DWORD RegEnumKeyEx(HKEY  hkey,DWORD index,LPSTR key,LPDWORD cbkey,LPDWORD reserved,LPSTR Class,LPDWORD cbClass,PFILETIME time);
******************************************************************************
DWORD RegEnumKey(HKEY hkey,DWORD index,LPSTR key,DWORD cbkey);
******************************************************************************
DWORD RegFlushKey(HKEY hkey);
******************************************************************************
DWORD RegNotifyChangeKeyValue(HKEY hkey,BOOL fWatchSubTree,DWORD fdwNotifyFilter,HANDLE  hEvent,BOOL  fAsync);
******************************************************************************
DWORD DuplicateHandle(DWORD inProcess,HKEY in,DWORD outProcess,HKEY *out,DWORD access,BOOL inherant,DWORD options);
******************************************************************************
DWORD RegOpenKeyEx(HKEY base,LPSTR key,DWORD res,DWORD sec,HKEY *out);
******************************************************************************/
#endif // _NWREG_H_
