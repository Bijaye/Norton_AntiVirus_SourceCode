// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright 1999 - 2003, 2005 Symantec Corporation. All rights reserved.
/////////////////////////////////////////////////////////////////////////////
//
// QConvert.h : Quarantine conversion DLL
//
/////////////////////////////////////////////////////////////////////////////
#ifndef _QCONVERT_INCLUDE_
#define _QCONVERT_INCLUDE_

#ifdef QCONVERT_EXPORTS
#define QCONVERT_API extern "C" __declspec(dllexport)
#else
#define QCONVERT_API extern "C" __declspec(dllimport)
#endif

// DLL Initialization
QCONVERT_API DWORD QConvertInit(DWORD dwProduct);
QCONVERT_API DWORD QConvertClose(void);


// Native Quarantine Functions
QCONVERT_API DWORD QConvertCloseNativeQuarantineFile(HANDLE hNative); 
QCONVERT_API DWORD QConvertDeleteNativeQuarantineFile(HANDLE hNative);
QCONVERT_API DWORD QConvertGetNativeFileInfo(HANDLE hNative, DWORD dwFileInfoFlag, LPTSTR lpFileInfoBuffer, DWORD dwBufSize);
QCONVERT_API DWORD QConvertGetNativeQuarantineFile(LPHANDLE lphNative, DWORD dwFileFlags); 
QCONVERT_API DWORD QConvertIsOk(HANDLE hNative, LPBOOL lpOkFlag);
QCONVERT_API DWORD QConvertSaveNativeQuarantineFile(HANDLE hNative); 


// Intermediate Quarantine Functions
QCONVERT_API DWORD QConvertCloseIntermediateQuarantineFile(HANDLE hIntermediate); 
QCONVERT_API DWORD QConvertDeleteIntermediateQuarantineFile(HANDLE hIntermediate);
QCONVERT_API DWORD QConvertGetIntermediateQuarantineFile(LPHANDLE lphIntermediate, LPTSTR lpIntermediateDirectory, DWORD dwFileFlags); 
QCONVERT_API DWORD QConvertSaveIntermediateQuarantineFile(HANDLE hIntermediate, LPTSTR lpIntermediateFile); 


// QConvert Product Values
#define QCONVERT_PRODUCT_NAV5               1   // Atomic
#define QCONVERT_PRODUCT_NAV6               2   // Beluga
#define QCONVERT_PRODUCT_NAV7               3   // Iliad


// QConvert Function Return Values
#define QCONVERT_OK                         0
#define QCONVERT_ERROR                      1   
#define QCONVERT_ERROR_INIT                 2
#define QCONVERT_ERROR_INIT_PACKAGE         3
#define QCONVERT_ERROR_UNKNOWN_PRODUCT      4
#define QCONVERT_ERROR_NO_CONVERT_DIR       5
#define QCONVERT_ERROR_INVALID_HANDLE       6
#define QCONVERT_ERROR_IN_NATIVE_DATA       7
#define QCONVERT_ERROR_NO_INFO				8
#define QCONVERT_ERROR_INTERMEDIATE         9
#define QCONVERT_ERROR_NO_INTER_FILES       10
#define QCONVERT_ERROR_NOT_SUPPORTED        11
#define QCONVERT_ERROR_NOT_INFECTED         12
#define QCONVERT_ERROR_MEMORY               13
#define QCONVERT_ERROR_FILENAME_MISSING     14
#define QCONVERT_ERROR_INVALID_BUFFER       15
#define QCONVERT_ERROR_ALREADY_IN_USE       16
#define QCONVERT_ERROR_NATIVE               17
#define QCONVERT_ERROR_IN_NATIVE_QUARANTINE 18


// Quarantine File Flags 
#define QCONVERT_FILE_FIRSTFILE             0x00000000  // Open first file
#define QCONVERT_FILE_NEXTFILE              0x00000001  // Open next file


// Native File Info Values
#define QCONVERT_INFO_FILE_NAME				1
#define QCONVERT_INFO_VIRUS_NAME			2
#define QCONVERT_INFO_VIRUS_ID              3
#define QCONVERT_INFO_OWNER                 4
#define QCONVERT_INFO_MACHINE				5
#define QCONVERT_INFO_DOMAIN				6
#define QCONVERT_INFO_FILE_DATE_CREATED		7
#define QCONVERT_INFO_FILE_DATE_ACCESSED	8
#define QCONVERT_INFO_FILE_DATE_MODIFIED	9
#define QCONVERT_INFO_FILE_DATE_QUARANTINED 10
#define QCONVERT_INFO_FILE_STATUS           11
#define QCONVERT_INFO_FILE_SIZE             12


#endif // _QCONVERT_INCLUDE_
