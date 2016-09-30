// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
//
// PLUGIN.CPP
//

#include <windows.h>
#include <tchar.h>
#include <process.h>
#include "Plugins.h"
#include "SymSaferRegistry.h"

//
// Type definitions
//
interface IRTVPlugin : IDispatch 
{
	VI(HRESULT) get_Window(long* phwnd);
	VI(HRESULT) put_Caption(BSTR strCaption);
	VI(HRESULT) get_Caption(BSTR* pstrCaption);
	VI(HRESULT) get_Ready(BOOL *pVal);
	VI(HRESULT) Start(void);
	VI(HRESULT) Stop();
};

//
// Globals
//
extern CPlugin g_plugins[ MAX_PLUGINS ];
HANDLE g_hPluginCloseMutex = NULL;

//
// Constants
//
IID IID_IRTVPlugin = _IID_IRTVPlugin;

//
// Prototypes
//
DWORD WINAPI PluginRunThread( LPVOID pArg ); 

void MBStoCLSID( CLSID& clsid, char* szCLSID, int length ) 
{
	WCHAR wszCLSID[64];

	mbstowcs( wszCLSID, szCLSID, length );
	CLSIDFromString( wszCLSID, &clsid );
}

void LoadPlugins() 
{
	HKEY hkey;
	int i;

	for ( i = 0; i < MAX_PLUGINS; i++ )
	{
		g_plugins[i].m_pUnknown = NULL;
		g_plugins[i].m_dwThreadControl = 0;
	}

	g_hPluginCloseMutex = CreateMutex( NULL, TRUE, NULL );

	if ( RegOpenKey( HKEY_LOCAL_MACHINE, REG_PLUGINS_BASE, &hkey) == ERROR_SUCCESS ) 
	{
		DWORD dwIndex = 0;
		TCHAR* pData  = NULL;
		TCHAR* pName  = NULL;
		try
		{
			pData = new TCHAR[ 64 ];
			pName = new TCHAR[ 128 ];
		}
		catch(std::bad_alloc &){}
		int i = 0;
		DWORD dwSize;
		DWORD dwType;

		dwSize = 64;
		dwType = REG_DWORD;
		*(DWORD*)pData = 1;
		SymSaferRegQueryValueEx( hkey, REG_ENABLE_VALUE, 0, &dwType, (LPBYTE)pData, &dwSize );
		if ( *(DWORD*)pData )
		{
			while( dwIndex < MAX_PLUGINS )
			{
				DWORD dwRet;
				CLSID clsid;
				DWORD dwNameSize = 128;
				DWORD dwDataSize = 64;				

				dwRet = RegEnumValue( hkey, dwIndex++, pName, &dwNameSize, NULL, NULL,
							(LPBYTE)pData, &dwDataSize );
				if ( dwRet != ERROR_SUCCESS )
					break;
			
#ifndef _UNICODE
				MBStoCLSID( clsid, pData, _tcslen( pData ) + 1 );
#else
				CLSIDFromString( pData, &clsid );
#endif
			
				try 
				{
					HRESULT hr;

					hr = CoCreateInstance( clsid, NULL, CLSCTX_INPROC_SERVER, IID_IUnknown,
						(LPVOID*)&(g_plugins[i].m_pUnknown) );
					if ( SUCCEEDED( hr ) )
					{
						DWORD dwThreadId;

						if ( !CreateThread( NULL, 0, PluginRunThread, (LPVOID)i, 0, &dwThreadId ) )
							g_plugins[i].m_pUnknown->Release();
						else
							i++;
					}
				}
				catch(...)
				{
					// do nothing, ignore for now
				}
			}
		}

		RegCloseKey( hkey );

		delete pData;
		delete pName;
	}

}

void UnloadPlugins()
{
	int i;
	BOOL allDone=0;

	ReleaseMutex( g_hPluginCloseMutex );

	while( !allDone )
	{
		i = 0;
		allDone = TRUE;
		while( allDone && ( i < MAX_PLUGINS ) )
		{
			if ( g_plugins[i].m_dwThreadControl )
				allDone = FALSE;
			i++;	
		}
	}

	CloseHandle( g_hPluginCloseMutex );
}

DWORD WINAPI PluginRunThread( LPVOID pArg )
{
	int i = (int)pArg;
	IRTVPlugin* pPlugin = NULL;
	HRESULT hr;

	g_plugins[i].m_dwThreadControl = 1;

	hr = g_plugins[i].m_pUnknown->QueryInterface( IID_IRTVPlugin, (LPVOID*)&pPlugin );
	if ( FAILED( hr ) )
		goto cleanup;
	
	pPlugin->Start();
	WaitForSingleObject( g_hPluginCloseMutex, INFINITE );
	ReleaseMutex( g_hPluginCloseMutex );
	pPlugin->Stop();

cleanup:
	if ( pPlugin )
		pPlugin->Release();
	g_plugins[i].m_pUnknown->Release();

	g_plugins[i].m_dwThreadControl = 0;

	return 0;
}