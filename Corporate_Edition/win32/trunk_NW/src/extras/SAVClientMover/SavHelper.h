// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
#if !defined(SAVHELPER_H_INCLUDED_)
#define SAVHELPER_H_INCLUDED_

DWORD GetRemoteRegValue( const TCHAR *szServer, TCHAR *szKeyName, TCHAR *szValueName, LPBYTE Value, DWORD dwValueSize, int nRetries );
DWORD SetRemoteRegValue( const TCHAR *szServer, TCHAR *szKeyName, TCHAR *szValueName, DWORD dwValueType, LPBYTE Value, DWORD dwValueSize, int nRetries );
DWORD GetMachineAddressCacheValue( const TCHAR *szMachine, TCHAR *szAddrCacheEntry, TCHAR *szValueName, BYTE* Value, DWORD &dwSize, int nRetries );
DWORD SendFileToMachine( LPSTR szMachine, const char *szLocalPath, const char *szRemotePath, int nRetries );
DWORD BuildGRCFile( FILE *fGRCFile, const char *szParent, BOOL bNewGUID, LPBYTE GUID );
DWORD PushGRCToClient( const char *szClient, const char *szFileName, int nRetries );
DWORD NukeRemoteKey( const char *CName, const char *Root, int nRetries );
DWORD MoveClient( CBA_Addr address, const CString &szFullClientName, const CString &szClientName, const CString &szSourceParent, const CString &szTargetParent, CString &szStatus );


#endif // !defined(SAVHELPER_H_INCLUDED_)

