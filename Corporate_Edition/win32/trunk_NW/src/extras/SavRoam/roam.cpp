// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
// nc7_surf.cpp : Defines the entry point for the console application.  
//

#define _WINSOCKAPI_

#include "stdafx.h"
#include "resource.h"
#include "tchar.h"
#include "windows.h"
#include "stdio.h"
#include "direct.h"
#include "shfolder.h"
#include "ResourceLoader.h"


//#include "ccLibStaticLink.h"
#include "ccTrace.h"
#include "ccSymDebugOutput.h"
#ifndef SERVER
ccSym::CDebugOutput g_DebugOutput(_T("SavRoam"));
IMPLEMENT_CCTRACE(g_DebugOutput);
#define INITIIDS
#endif
#include "ccVerifyTrustLoader.h"
   


#define INCL_WINSOCK_API_TYPEDEFS 1

#include "winsock2.h"
#include "wininet.h"
#include "process.h"
#include "SymSaferRegistry.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "finetime.h"

#ifdef __cplusplus
};
#endif

#include <io.h>
#include <errno.h>

#define MAX_SERVER_NAME			48	  // limit imposed by LDVP technology

#ifndef STANDALONE_ROAM
#define NEEDKERNAL                  // request simple prototypes for transman functions
#endif

#include "transman.h"
#include "ClientReg.h"
#include "vpstrutils.h"

#define ROAM_DEFINE 1           // define the roam.cpp globals and exports
#include "roam.h"
#include "savroam.h"

// globals

static BOOL g_bWin2000plus = FALSE;

BOOL g_bWinsockPresent = FALSE;
BOOL g_bWinInetPresent = FALSE;

BOOL g_bRoamClient = 0;
BOOL g_bRoamESM = 0;
BOOL g_bRoamITA = 0;
   

LPFN_WSASTARTUP fWSAStartup = NULL;
LPFN_WSACLEANUP fWSACleanup = NULL;
LPFN_GETHOSTNAME fgethostname = NULL;
LPFN_GETHOSTBYNAME fgethostbyname = NULL;

static HINSTANCE g_hKernel32 = NULL;
static HINSTANCE g_hAdvapi32 = NULL;
                        
extern CResourceLoader g_ResLoader;
// WinInet functions

typedef BOOL (WINAPI* LPFNINTERNETGETCONNECTEDSTATEEX)( LPDWORD lpdwFlags, LPTSTR lpszConnectionName, DWORD dwNameLen, DWORD dwReserved );
typedef BOOL (WINAPI* LPFNINTERNETGETCONNECTEDSTATE)( LPDWORD lpdwFlags, DWORD dwReserved );

static LPFNINTERNETGETCONNECTEDSTATE fInternetGetConnectedState = NULL;
static LPFNINTERNETGETCONNECTEDSTATEEX fInternetGetConnectedStateEx = NULL;

WSADATA WsaData;

// Win2000+ specific functions

typedef SC_HANDLE (WINAPI* LPFNOPENSCMANAGER)(LPCTSTR lpMachineName, LPCTSTR lpDatabaseName, DWORD dwDesiredAccess);
typedef BOOL (WINAPI* LPFNQUERYSERVICESTATUS)(SC_HANDLE hService, LPSERVICE_STATUS lpServiceStatus);
typedef BOOL (WINAPI* LPFNSTARTSERVICE)(SC_HANDLE hService, DWORD dwNumServiceArgs, LPCTSTR *lpServiceArgVectors);
typedef BOOL (WINAPI* LPFNCONTROLSERVICE)(SC_HANDLE hService, DWORD dwControl, LPSERVICE_STATUS lpServiceStatus);
typedef BOOL (WINAPI* LPFNCLOSESERVICEHANDLE)(SC_HANDLE hSCObject);
typedef HANDLE (WINAPI* LPFNCREATEWAITABLETIMER)(LPSECURITY_ATTRIBUTES lpTimerAttributes, BOOL bManualReset, LPCTSTR lpTimerName);
typedef BOOL (WINAPI* LPFNSETWAITABLETIMER)(HANDLE hTimer, const LARGE_INTEGER *pDueTime, LONG lPeriod, PTIMERAPCROUTINE pfnCompletionRoutine, LPVOID lpArgToCompletionRoutine, BOOL fResume);
typedef BOOL (WINAPI* LPFNCANCELWAITABLETIMER)(HANDLE hTimer);
 
static LPFNOPENSCMANAGER lpfnOpenSCManager = NULL;
static LPFNQUERYSERVICESTATUS lpfnQueryServiceStatus = NULL;
static LPFNSTARTSERVICE lpfnStartService = NULL;
static LPFNCONTROLSERVICE lpfnControlService = NULL;
static LPFNCLOSESERVICEHANDLE lpfnCloseServiceHandle = NULL;
static LPFNCREATEWAITABLETIMER lpfnCreateWaitableTimer = NULL;
static LPFNSETWAITABLETIMER lpfnSetWaitableTimer = NULL;
static LPFNCANCELWAITABLETIMER lpfnCancelWaitableTimer = NULL;

HINSTANCE g_hWinSock = 0;
HINSTANCE g_hWinInet = 0;

DWORD g_dwCheckNewParentInterval = 0;
DWORD g_dwCheckParentInterval = 0;
DWORD g_dwSampleCountForParentCheck = 0;
DWORD g_dwFindNearestParentInterval = 0;

HANDLE   hServerStopEvent = NULL;

char szNoParent[] = "Down-NoParent-5368E1700CBF11d5B8DC0010A4972180";

void WinNTService( void *param );
DWORD CopyFileFromRemoteMachine( LPSTR szParent, SENDCOM_HINTS hint, char *szRemotePath, char *szLocalPath );
void OutputLine( char *szLine, FILE *fFile, BOOL bDetail );
void OutputDebugLine( char *szLine, FILE *fFile, BOOL bDetail );

BOOL     bDone = FALSE;

BYTE *GetNameFromList(BYTE *List,int element)
{
    int i;
    BYTE *ret = NULL;

    for ( i=0;i<=element;i++ )
    {
        ret = List;
        List += strlen((char *)List) + 1;
    }

    return ret;
}

#define GETKEYVALUE_RECURSE     TRUE
#define GETKEYVALUE_NORECURSE   FALSE
#define GETKEYVALUE_VALUES      TRUE
#define GETKEYVALUE_NOVALUES    FALSE

typedef DWORD (*FKEYCALLBACK)(DWORD dwCallbackHandle, TCHAR *szKeyName);
typedef DWORD (*FVALUECALLBACK)(DWORD dwCallbackHandle, TCHAR *szKeyName, TCHAR *szValueName, DWORD dwType, BYTE *Value, DWORD dwValueSize);

static DWORD GetKeyAndValueList( const char *CName,
								 SENDCOM_HINTS hints,
								 char *Root,
                                 DWORD &dwKeyCount,
								 DWORD &dwValueCount,
								 BOOL bRecurse, BOOL bGetValues,
                                 DWORD dwCallbackHandle,
                                 FKEYCALLBACK fKeyCallback,
								 FVALUECALLBACK fValueCallback )
{
    char  *value = NULL;
    BYTE  *data = NULL;
    BYTE  *packet = NULL;
    DWORD cc = ERROR_SUCCESS;

    value = (char *) malloc(IMAX_PATH);
    data = (BYTE *) malloc (IMAX_PATH);
    packet = (BYTE *) malloc(MAX_PACKET_DATA);

	if ( value == NULL || data == NULL || packet == NULL ) {
		if ( value ) free(value);
		if ( data ) free(data);
		if ( packet ) free(packet);
        return ERROR_MEMORY;
	}

    // do the keys, depth first

    {
		DWORD index = 0;
	    DWORD count = 0;

		do
        {
            cc = SendCOM_LIST_KEY_BLOCK(NULL, CName, hints, Root, packet, MAX_PACKET_DATA, &index,&count);
            if ( count && cc == ERROR_SUCCESS )
            {
                for ( DWORD i = 0; i < count; i++ )
                {
                    char *next = (char *)GetNameFromList(packet,i);

                    if ( !*Root ) //(Root == "")
                        strcpy(value, next);
                    else
                        sssnprintf(value, sizeof(IMAX_PATH), _T("%s\\%s"), Root, next);

                    if ( fKeyCallback )
                    {
                        fKeyCallback( dwCallbackHandle, next );
                    }

                    dwKeyCount += 1;

                    if ( bRecurse )
                    {
                        // now do the sub-tree
                        GetKeyAndValueList(CName, hints, value, dwKeyCount, dwValueCount, bRecurse, bGetValues, dwCallbackHandle, fKeyCallback, fValueCallback );
                    }
                }
            }
        } while ( cc == ERROR_SUCCESS && index != 0 );
    }

    if ( cc == ERROR_FILE_NOT_FOUND )
        cc = ERROR_SUCCESS;

    // now do the values in this key

    // if the last failed, the machine is likely dead, and this saves time

    if ( bGetValues )
    {
        if ( ERROR_SUCCESS == cc )
        {
            DWORD index = 0;
		    DWORD count = 0;
			DWORD type = 0;
			DWORD datasize = 0;

			do
            {
                cc = SendCOM_LIST_VALUE_BLOCK(NULL, CName, hints, Root, packet, MAX_PACKET_DATA, &index,&count);

				if ( count && cc == ERROR_SUCCESS )
                {
                    BYTE *pCurrentData = packet;
                    for ( DWORD i = 0; i < count; i++ )
                    {
                        datasize = IMAX_PATH;
                        pCurrentData = GetValueFromEnumBlock(pCurrentData, value, &type, data, &datasize);

                        dwValueCount += 1;

                        if ( fValueCallback )
                        {
                            fValueCallback( dwCallbackHandle, Root, value, type, data, datasize );
                        }
                    }
                }
            } while ( index != 0 && cc == ERROR_SUCCESS );
        }
    }

    if ( cc == ERROR_FILE_NOT_FOUND )
        cc = ERROR_SUCCESS;

    free(value);
    free(data);
    free(packet);

    return cc;
}

DWORD GetLocalServerList( char *szLevelList, char *szType, DWORD *lpdwElapsedTime, LPBOOL lpbByName )
{
    LONG lResult;
    HKEY hRoamServersKey;

    DWORD dwStartTime = GetFineLinearTime( );

    lResult = RegOpenKeyEx( HKEY_LOCAL_MACHINE, _T("SOFTWARE\\Intel\\LANDesk\\VirusProtect6\\CurrentVersion\\ProductControl"), NULL, KEY_READ, &hRoamServersKey );
    if ( ERROR_SUCCESS == lResult )
    {
        char szRegKey[1024];
        DWORD dwValueType = 0;
        DWORD dwValueSize = 1024;

        sssnprintf( szRegKey, sizeof(szRegKey), _T("RoamManaging%sLevel0"), szType );
		//
		// Use ByName key for NMCI first. This needs to be done in order for
		// Roaming clients to retain the Master.ini capability.
		// Note the assumption is that these two methods will not be used concurrently
		//
		sssnprintf( szRegKey, sizeof(szRegKey), _T("RoamManaging%sByName"), szType );
   
        lResult = SymSaferRegQueryValueEx( hRoamServersKey, szRegKey, NULL, &dwValueType, (LPBYTE) szLevelList, &dwValueSize );
		if ( lResult != ERROR_SUCCESS )
        {
 			sssnprintf( szRegKey, sizeof(szRegKey), _T("RoamManaging%sLevel0"), szType );

            lResult = SymSaferRegQueryValueEx( hRoamServersKey, szRegKey, NULL, &dwValueType, (LPBYTE) szLevelList, &dwValueSize );
            if ( lResult == ERROR_SUCCESS )
            {
                *lpbByName = FALSE;
            }
        }
 		else *lpbByName = TRUE;
 


        RegCloseKey( hRoamServersKey );
    }
    else
    {

		LoadString(g_ResLoader.GetResourceInstance(), IDS_ERROR_OPENING_KEY, szBuffer, MAX_BUFFER);
        sssnprintf( szBuffer2, sizeof(szBuffer2), _T("ProductControl"));
        ReportLastError( szBuffer2, NULL );
    }

    // make sure that failure doesn't return a fossil list

    if ( ERROR_SUCCESS != lResult )
    {
        strcpy( szLevelList, "" );
    }

    *lpdwElapsedTime = ElapsedFineLinearTime( dwStartTime, GetFineLinearTime( ) );

    return lResult;
}

DWORD GetRoamServerList( TCHAR *szServer, SENDCOM_HINTS hints, TCHAR *szType, int nLevel, char *szLevelList, DWORD *lpdwElapsedTime )
{
	// For the ByName usage of config file in first entry.
	if (stricmp (szServer, "master.ini") == 0)
 		return E_FAIL;

    BYTE  sendBuf[MAX_PACKET_DATA];

    DWORD dwItemsSent = 1;
    DWORD dwRet;

    memset(sendBuf, 0, MAX_PACKET_DATA);

    // build the registry key to read

    sssnprintf( (char *) sendBuf, sizeof(sendBuf), "RoamManaging%sLevel%d", szType, nLevel );

    // send == root key name,list of values  buf=[name0name0name00]
    // do get values reply. data of values (trunc if not fit) status = number of successful gets
    // reply buf=[WWdataWWdataWWdata]  WW=word len of data

    DWORD dwStartTime = GetFineLinearTime( );

    dwRet = SendCOM_GET_VALUES(NULL, szServer, hints | SENDCOM_REMOTE_IS_SERVER, "ProductControl",
                               (char *) sendBuf, sizeof(sendBuf), &dwItemsSent);

	// moved this calculation before the error check below,
	// we should get the time of the network transaction and not include the
	// time to process the returned string
    *lpdwElapsedTime = ElapsedFineLinearTime( dwStartTime, GetFineLinearTime( ) );

	if ( ERROR_SUCCESS == dwRet )
    {
        // suck the string result returned out

        WORD wBytes = * (WORD *) sendBuf;
        char *szKey = (char *)sendBuf+2;

        if ( wBytes > 0 )
        {
            memcpy( szLevelList, szKey, wBytes );
            szLevelList[wBytes] = 0;
        }
        else
        {
            strcpy( szLevelList, "" );
        }
    }
    else
    {
        strcpy( szLevelList, "" );
		TCHAR szLogMsg[1024];
		_stprintf(szLogMsg, "Error in communicating with Roam Server = %d\n", dwRet);
		OutputLine( szLogMsg, NULL, TRUE );

    }

    return dwRet;
}

void PrintIndent( int nLevel, FILE *fFile )
{
    int i;

    for ( i=0; i<nLevel; i++ )
    {
        OutputLine( _T("  "), fFile, TRUE );
    }
}

int CountServers( char *szServerList )
{
    int nCount = 0;

    LPSTR szRestOfServerList = szServerList;
    LPSTR szWorkingServer = szServerList;

    while ( szRestOfServerList != NULL && szRestOfServerList[0] != 0 )
    {
        nCount += 1;

        szWorkingServer = szRestOfServerList;
        szRestOfServerList = strpbrk( szRestOfServerList, ",>=" );

        if ( szRestOfServerList )
        {
            // there are more servers - skip the , 

            szRestOfServerList += 1;
        }
    }

    return nCount;
}

// PutRemoteVal((char *)address,"ProductControl","NewPatternFile",1);
// note - this works for DWORD values only - otherwise use AddValueToBlock 
// with the appropriate type

DWORD PutRemoteVal(const char *CName, SENDCOM_HINTS hints, const char *root,const char *value,DWORD val)
{

    BYTE buf[128];
    DWORD used;
    memset(buf,0,sizeof(buf));
    AddValueToBlock(buf,value,(DWORD)REG_DWORD,(WORD)sizeof(DWORD),(BYTE*)&val,&used);
    return SendCOM_SET_VALUES(NULL, CName, hints,root,1,buf,used);
}

DWORD GetRemoteVal(const char *CName, SENDCOM_HINTS hints, const char *root, const char *value, DWORD def)
{
    BYTE buf[128];
    DWORD count =1;
    DWORD cc = 0;

    memset(buf,0,sizeof(buf));
    vpstrncpy((char *)buf,value,sizeof (buf));

	cc = SendCOM_GET_VALUES(NULL, CName, hints, root,(char *)buf,sizeof (buf),&count);

    if ( cc == ERROR_SUCCESS )
    {
        WORD Size;
        BYTE *d = GetValueFromBlock(buf,0,(WORD *)&Size);
        if ( Size == sizeof(DWORD) )
            cc = *(DWORD *)d;
        else
            cc = def;
    }
    else
        cc = def;

    return cc;
}

// Cleans up the import file line as follows:
//     1.  Removes newlines
//     2.  Changes tabs to spaces
//     3.  Changes multiple spaces to single space
//     4.  Removes leading spaces
void CleanUpLine( char *szLine )
{
    char *szTemp;
    char *szWalk;

    // clean up the line

    szTemp = szLine;
    szWalk = szLine;

    BOOL bWasSpace = FALSE;
    BOOL bFirstOfLine = TRUE;

    while ( *szWalk )
    {
        if ( *szWalk == '\t' )
        {
            *szWalk = ' ';
        }

        if ( *szWalk == ' ' )
        {
            if ( ! bWasSpace && ! bFirstOfLine )
            {
                *szTemp = *szWalk;
                szTemp += 1;
            }

            bWasSpace = TRUE;
        }
        else if ( *szWalk == '\n' )
        {
            // skip new lines
        }
        else
        {
            bWasSpace = FALSE;
            bFirstOfLine = FALSE;

            *szTemp = *szWalk;
            szTemp += 1;
        }

        szWalk += 1;
    }

    *szTemp = 0;
}

TCHAR* rtrim(TCHAR *szSource )
{
    TCHAR* pszEOS;

    // Set pointer to end of string to point to the character just
    //  before the 0 at the end of the string.
    pszEOS = szSource + _tcslen( szSource ) - 1;

    while( pszEOS >= szSource && _istspace(*pszEOS)) 
	{
		*pszEOS = NULL;
		pszEOS = CharPrev(szSource, pszEOS);
	}
    return szSource;
}

TCHAR* ltrim(TCHAR *szSource)
{
	TCHAR* pPos = szSource;

	int nByteLen = strlen(szSource);

	while(pPos && _istspace(*pPos))
	{
		pPos = CharNext(pPos);
	}

	if(pPos > szSource) 
	{
		// copy all chars after the leading blanks, including NULL
		memmove(szSource, pPos, nByteLen - (pPos - szSource) + 1);
	}
	return szSource;
}

// Remove leading/trailing blanks from server name
TCHAR* FormatServerName(TCHAR* szServerName)
{
	rtrim(szServerName);
	ltrim(szServerName);
	return szServerName;
}

// remove trailing and leading blanks in server names,
//     check for blank server names, leading and trailing delimiter
BOOL FormatServerList(TCHAR* szServerList)
{
	// pPos = current server string in list
	TCHAR* pPos = szServerList;
	// pDest = current end of formatted server list (szServerList)
	TCHAR* pDest = szServerList;
	// pSource = pointer to formatted server name, for copying
	//     to destination in szServerList
	TCHAR* pSource = NULL;
	// Pointer to delimeter (",>=)
	TCHAR* pDelimeter = NULL;
	// buffer to hold formatted server name
	TCHAR szServerName[1024];

	int nByteLen = 0;

	// find delimiter

	BOOL bDone = FALSE;

	while( ! bDone )
	{
		memset(szServerName, NULL, sizeof(szServerName));
		pDelimeter = _tcspbrk(pPos, _T(",>="));
		
		if(pDelimeter)
		{
			// found delimeter, test length of server name
			nByteLen = pDelimeter - pPos;
			if(nByteLen > 1024)
			{
				// error, length greater than array size
				return FALSE;
			}
			else if(nByteLen == 0)
			{
				// error - blank server name
				return FALSE;
			}
			strncpy(szServerName, pPos, nByteLen);
		}
		else
		{
			if(strlen(pPos) > 1024)
			{
				// error, length greater than array size
				return FALSE;
			}
			strcpy(szServerName, pPos);
		}

		FormatServerName(szServerName);

		nByteLen = strlen(szServerName);

		if(nByteLen >= MAX_SERVER_NAME)
		{
			// error - skipping server list
			return FALSE;
		}
		else if(nByteLen == 0)
		{
			// error - blank server name
			return FALSE;
		}

		// copy formatted server name to end of formatted 
		//     portion of szServerList, overwritting original
		//     characters in szServerList

		pSource = szServerName;

		while(*pSource)
			*pDest++ = *pSource++;

		if(pDelimeter)
		{
			*pDest++ = *pDelimeter; // copy delimeter				
			// move on to next server name
			pPos = CharNext(pDelimeter);
			if(!(*pPos))
			{
				// error - trailing delimeter
				return FALSE;
			}
		}
		else
		{
			*pDest = NULL; // add null to end of string
			bDone = TRUE;	// no more delimiters, so last server name was end of list
		}
	}
	return TRUE;
}

BOOL ParseLine( char *szLine, char **szMachine, char **szType, char **szLevel, char **szServerList )
{
    char *szTemp;

    *szMachine = NULL;
    *szType    = NULL;
    *szLevel   = NULL;
    *szServerList = NULL;

    // as long as it isn't a comment

    if ( szLine[0] != '#' && szLine[0] != 0 )
    {
        *szMachine = szLine;
        *szType = strchr( *szMachine, ' ' );

		if(!(*szType))
			return FALSE; // no space delimiter after 'machine'

		szTemp = *szType;

		// skip over spaces between 'machine' and 'type'
		do
		{
			*szType = CharNext( *szType );
		} while(_istspace(**szType));

		if(!(**szType))
			return FALSE; // no 'type' after 'machine'

		// add null to end of 'machine'
		*szTemp = 0;

		// skip over spaces between 'type' and 'level'
        *szLevel = strchr( *szType, ' ' );
		if(!(*szLevel))
			return FALSE; // no space delimiter after 'type'

		szTemp = *szLevel;
		// skip over spaces between 'type' and 'level'
		do
		{
			*szLevel = CharNext(*szLevel);
		} while(_istspace(**szLevel));

		if(!(**szLevel))
			return FALSE; // no 'level' after 'type'

		*szTemp = 0; // add null to end of 'type'

		// skip over spaces between 'level' and 'serverlist'        
		*szServerList = strchr(*szLevel, ' ');
		if(!(*szServerList))
			return FALSE; // no space delimiter after 'level'

		szTemp = *szServerList;
		do
		{
			*szServerList = CharNext( *szServerList );
		} while(_istspace(**szServerList));

		if(!(**szServerList))
			return FALSE; // no 'serverlist' after 'level'

		*szTemp = 0; // add null to end of 'level'

		if(!FormatServerList(*szServerList))
			return FALSE; // error parsing server list
    }

    // comment lines should be treated as empty

    //if ( szLine[0] == '#' )
     //   szLine[0] = 0;

	return TRUE;
}

// Perform some basic checks on import file parameters.  
//     Will require updating as new parameters are added.
//     Uses stricmp for case insensitivity because looking up
//     registry values does not depend on it.
BOOL ValidateLine(char* szMachine, char* szType, char* szLevel, char* szServerList)
{
	BOOL bValid = FALSE;
	if(!(szMachine) || !(szType) || !(szLevel) || !(szServerList))
		return FALSE;
	if(!(*szMachine) || !(*szType) || !(*szLevel) || !(*szServerList))
		return FALSE;
	
	// check for misspelled 'type' parameter
	switch(*szType)
	{
		case 'P': // Parent
		case 'p':
			if(!stricmp(szType, _T("Parent")))
				bValid = TRUE;
			break;
	}
	return bValid;
}

DWORD ImportServerList( char *szServerFile )
{
    DWORD dwRet = 0;
	UINT nLineCount = 0;

    FILE *fServerFile = fopen( szServerFile, "r" );

    char szClientServerList[1024];
    char szClientRegKey[1024];

    if ( fServerFile )
    {
        char szLine[1024];
        char szOutLine[1024];

        while ( fgets( szLine, sizeof( szLine ), fServerFile ) )
        {
            char *szMachine;
            char *szType;
            char *szLevel;
            char *szServerList;

			nLineCount++;

            CleanUpLine( szLine );

			if(strlen(szLine) > 0)
			{
				if(ParseLine( szLine, &szMachine, &szType, &szLevel, &szServerList ))
	            {
					// comment lines should be treated as empty
					if ( szLine[0] == '#' )
					{
						szLine[0] = 0;
						continue;
					}
					if(ValidateLine(szMachine, szType, szLevel, szServerList))
					{
						char szRegKey[ 1024];

			            // set the basic key
						sssnprintf(szRegKey, sizeof(szRegKey), _T("RoamManaging%sLevel%s"), szType, szLevel);

		                if ( ! stricmp( szMachine, "<local>" ) )
						{
		                    HKEY hKey;
							DWORD dwResult;
	
							// this is a special case for the local client machine
	
							HRESULT hResult = RegCreateKeyEx(
															HKEY_LOCAL_MACHINE, 
															"SOFTWARE\\INTEL\\LANDesk\\VirusProtect6\\CurrentVersion\\ProductControl",
															NULL,
															NULL,
															REG_OPTION_NON_VOLATILE,
															KEY_ALL_ACCESS,
															NULL,
															&hKey,
															&dwResult );
							if ( ERROR_SUCCESS == hResult )
							{
		                        if ( szServerList && szServerList[0] != '#' )
								{
		                            // save client list to save on all servers
	
									strcpy( szClientServerList, szServerList );
									strcpy( szClientRegKey, szRegKey );
	
									// set locally first
	
									hResult = RegSetValueEx( hKey, szRegKey, NULL, REG_SZ, (const unsigned char *)szServerList, strlen(szServerList)+1 );
									if ( ERROR_SUCCESS != hResult )
									{

										LoadString(g_ResLoader.GetResourceInstance(), IDS_SETTING_REGKEY, szBuffer, MAX_BUFFER);
										sssnprintf(szBuffer2, sizeof(szBuffer2), szBuffer, _T("local"));
										ReportLastError( szBuffer2, NULL );
									}
									else
									{

										LoadString(g_ResLoader.GetResourceInstance(), IDS_SET_LOCAL_KEY_VALUE, szBuffer, MAX_BUFFER);
										sssnprintf( szOutLine, sizeof(szOutLine), szBuffer, szRegKey, szServerList );
										OutputLine( szOutLine, NULL, TRUE );
									}
								}
								else
								{
		                            hResult = RegDeleteValue( hKey, szRegKey );
									if ( ERROR_SUCCESS != hResult )
									{

										LoadString(g_ResLoader.GetResourceInstance(), IDS_DELETING_LOCAL_KEY, szBuffer, MAX_BUFFER);
										ReportLastError( szBuffer, NULL );
									}
									else
									{

										LoadString(g_ResLoader.GetResourceInstance(), IDS_DELETING_LOCAL_KEY, szBuffer, MAX_BUFFER);
										sssnprintf( szOutLine, sizeof(szOutLine), szBuffer, szRegKey );							
										OutputLine( szOutLine, NULL, TRUE );
									}
								}
	
								RegCloseKey( hKey );
							}
						}
#ifdef ADMIN_ONLY
		                else
						{
		                    DWORD index=0;
	
							BYTE packet[MAX_PACKET_DATA];
	
							DWORD used=0;
							DWORD cc=ERROR_SUCCESS;
	
							// valid server list - set it
	
							if ( szServerList && szServerList[0] != '#' )
							{
		                        memset(packet,0,MAX_PACKET_DATA);
								AddValueToBlock(packet,szRegKey,(DWORD)REG_SZ,(WORD)strlen(szServerList)+1,(unsigned char *)szServerList,&used);
		
								// now build the client list to add as well - this gets put on every server
		
								AddValueToBlock(packet,szClientRegKey,(DWORD)REG_SZ,(WORD)strlen(szClientServerList)+1,(unsigned char *)szClientServerList,&used);
		
								cc = SendCOM_SET_VALUES(NULL, szMachine, SENDCOM_REMOTE_IS_SERVER, "ProductControl",1,packet,used);
								if ( ERROR_SUCCESS != cc )
								{
									//all these error are passed back by SendCOM_SET_VALUES when it tries to send the request to the remote pc
									//trapped all of these so we can give a specific message about communication failure
									//if any errors are modified in the func SendCOM_SET_VALUES we need to come back and reverify
									//if all the errors below mean communication errors or not
									if( (cc == COM_ERROR_TRANSPORT_NOT_OK) || (cc == ERROR_NO_MEM) || (cc == COM_ERROR_BAD_FORMAT_SA) || (cc == ERROR_SEND_FAILED) || (cc == ERROR_TOO_MANY_BUSYS) )
									{
										LoadString(g_ResLoader.GetResourceInstance(), IDS_COMM_ERROR_SETTING_ROAMING_KEYS, szBuffer, MAX_BUFFER);
										sssnprintf( szOutLine, sizeof(szOutLine), szBuffer, szMachine, szRegKey, cc );
									}
									else //it is some other default error
									{
										LoadString(g_ResLoader.GetResourceInstance(), IDS_ERROR_SETTING_ROAMING_KEYS, szBuffer, MAX_BUFFER);
										sssnprintf( szOutLine, sizeof(szOutLine), szBuffer, szMachine, szRegKey, cc );
									}
												
									
									OutputLine( szOutLine, NULL, FALSE );
								}
								else
								{
									LoadString(g_ResLoader.GetResourceInstance(), IDS_SET_SERVER_KEY_VALUE, szBuffer, MAX_BUFFER);
									sssnprintf( szOutLine, sizeof(szOutLine), szBuffer, szMachine, szRegKey, szServerList );
									OutputLine( szOutLine, NULL, TRUE );
								}
							}
							else
							{
		                        // empty server list - delete the key
		
								cc = SendCOM_DEL_VALUE(NULL, szMachine, SENDCOM_REMOTE_IS_SERVER, "ProductControl",szRegKey);
								if ( cc == 2 )
								{
									LoadString(g_ResLoader.GetResourceInstance(), IDS_DELETE_VALUE_OK, szBuffer, MAX_BUFFER);
									sssnprintf( szOutLine, sizeof(szOutLine), szBuffer, szMachine, szRegKey );
									OutputLine( szOutLine, NULL, FALSE );
								}
								else if ( ERROR_SUCCESS != cc )
								{
									LoadString(g_ResLoader.GetResourceInstance(), IDS_ERROR_DELETING_ROAMING_KEY, szBuffer, MAX_BUFFER);
									sssnprintf( szOutLine, sizeof(szOutLine), szBuffer, szMachine, szRegKey, cc );
									OutputLine( szOutLine, NULL, FALSE );
								}
								else
								{
									LoadString(g_ResLoader.GetResourceInstance(), IDS_DELETE_VALUE_ON_SERVER_KEY, szBuffer, MAX_BUFFER);
									sssnprintf( szOutLine, sizeof(szOutLine), szBuffer, szMachine, szRegKey );
									OutputLine( szOutLine, NULL, TRUE );
								}
							}
						}
#endif
					}
					else // couldn't validate line
					{
						LoadString(g_ResLoader.GetResourceInstance(), IDS_ERROR_PARSING_LINE, szBuffer, MAX_BUFFER);
						sssnprintf(szOutLine, sizeof(szOutLine), szBuffer, nLineCount);
						OutputLine(szOutLine, NULL, TRUE);
					}
				}
				else // couldn't parse line
				{
					LoadString(g_ResLoader.GetResourceInstance(), IDS_ERROR_PARSING_LINE, szBuffer, MAX_BUFFER);
					sssnprintf(szOutLine, sizeof(szOutLine), szBuffer, nLineCount);
					OutputLine(szOutLine, NULL, TRUE);
				}

			} // skipping empty line
        }

		fclose( fServerFile );
    }
    else
    {
		LoadString(g_ResLoader.GetResourceInstance(), IDS_ERROR_OPENING_IMPORT_FILE, szBuffer, MAX_BUFFER);
        ReportLastError( szBuffer, NULL );
    }

	return dwRet;
}

DWORD ReportRemoteServerList( char *szMachine, char *szType, int nLevel, FILE *fExportFile )
{
    char szWorkingServerList[1024];
    char szLine[1024];

    DWORD dwRet;
    DWORD dwElapsedTime;

    LPSTR szRestOfServerList;
    char *szWorkingServer;

    // and get his list of servers, if any
    dwRet = GetRoamServerList( szMachine, SENDCOM_COMM_NONE, szType, nLevel, szWorkingServerList, &dwElapsedTime );

    if ( dwRet == ERROR_SUCCESS )
    {
        PrintIndent( nLevel, fExportFile );
		//Start fix for STS 368375
        if( szWorkingServerList != NULL && szWorkingServerList[0] != 0 )
		{
			if(fExportFile) //if we are exporting to a file and we got a 
							//server list successfully do not display the delay in ms following #
							//this causes an issue when importing the file
							//Removed the display of the word Level before the value of nLevel
							//since the import file does not follow that format and import functionality
							//on the file will not put the correct key
				sssnprintf(szLine, sizeof(szLine), "%s %s %d %s\n", szMachine, szType, nLevel, szWorkingServerList );
			else
			{
				//if only for display purposes then go ahead and show the delay in ms follwing the #
				//Removed the display of the word Level before the value of nLevel
				//since the import file does not follow that format and import functionality
				//on the file will not put the correct key
				//Used load string to display delay in ms since they will need to be localized
				LoadString(g_ResLoader.GetResourceInstance(), IDS_DELAYTIMEONSUCESS, szBuffer2, MAX_BUFFER);
				sssnprintf(szBuffer, sizeof(szBuffer), szBuffer2, dwElapsedTime, dwRet);
				sssnprintf(szLine, sizeof(szLine), "%s %s %d %s # %s\n", szMachine, szType, nLevel, szWorkingServerList, szBuffer);
			}
		}
        else
		{
			//if we are able to communicate to the machine but the serverlist does not exist then
			//comment this line as the import file does not need to do set anything in this case
			//this will happen generally for the last node which does not have any setting of a
			//serverlist.
			//Removed the display of the word Level before the value of nLevel
			//since the import file does not follow that format and import functionality
			//on the file will not put the correct key
			//Used load string to display delay in ms since they will need to be localized
			LoadString(g_ResLoader.GetResourceInstance(), IDS_DELAYTIMEONSUCESS, szBuffer2, MAX_BUFFER);
			sssnprintf(szBuffer, sizeof(szBuffer), szBuffer2, dwElapsedTime, dwRet);
			sssnprintf(szLine, sizeof(szLine), "# %s %s %d # %s\n", szMachine, szType, nLevel, szBuffer);
		}
		//End fix for STS 368375
		OutputLine( szLine, fExportFile, TRUE );

        szRestOfServerList = szWorkingServerList;
        szWorkingServer = szWorkingServerList;

        // as long as there are servers on the list
        while ( szRestOfServerList != NULL && szRestOfServerList[0] != 0 )
        {
            szWorkingServer = szRestOfServerList;
            szRestOfServerList = strchr( szRestOfServerList, ',' );

            if ( szRestOfServerList )
            {
                // there are more servers - nuke the , 

                szRestOfServerList[0] = 0;
                szRestOfServerList += 1;
            }

            dwRet = ReportRemoteServerList( szWorkingServer, szType, nLevel+1, fExportFile );
        }
    }
    else
    {
        //Start fix for STS 368375
		//Removed the display of the word Level before the value of nLevel
		//since the import file does not follow that format and import functionality
		//on the file will not put the correct key
		//Used load string to display delay in ms since they will need to be localized

		PrintIndent( nLevel, fExportFile );
		LoadString(g_ResLoader.GetResourceInstance(), IDS_DELAYTIMEONFAILURE, szBuffer2, MAX_BUFFER);
		sssnprintf(szBuffer,sizeof(szBuffer),szBuffer2,dwElapsedTime, dwRet);
        sssnprintf (szLine, sizeof(szLine),"# %s %s %d # %s\n", szMachine, szType, nLevel, szBuffer );
		//End fix for STS 368375
        OutputLine( szLine, fExportFile, TRUE );
    }

    return dwRet;        
}

DWORD ReportServerList( FILE* pFile, char *szType )
{
    char szCurrentServerList[1024];
    char szNextServer[128];

    int  nLevel = 0;

    DWORD dwNextServerListElapsedTime;

    DWORD dwRet; // error status for locating the starting list on client machine

    LPSTR szRestOfServerList;
    LPSTR szWorkingServer;

    char szLine[1024];

	BOOL bByName = FALSE;

    // start with the local list - save error status to indicate if starting list found
	dwRet = GetLocalServerList( szCurrentServerList, szType, &dwNextServerListElapsedTime, &bByName);

    strcpy( szNextServer, "" );
	//Start fix for STS 368375
	if ( dwRet == ERROR_SUCCESS )
    {
		//if we have a serverlist in the local machine only then add the line uncommented in the
		//export file else display and add it commented within the export file since the import
		//does not need to go through it.
		//Removed the printing of the word Level befor displaying 0 which is the level for local
		//machines since the import file does not expect that format.
		if ( szCurrentServerList != NULL && szCurrentServerList[0] != 0 )
			sssnprintf(szLine, sizeof(szLine), "<local> %s 0 %s\n", szType, szCurrentServerList );
		else
			sssnprintf(szLine, sizeof(szLine), "# <local> %s 0\n", szType );
	}
	else //if we failed to get the server list put it in the export line commented so that
		 //import does not catch it.	
		sssnprintf(szLine, sizeof(szLine), "# <local> %s 0\n", szType );
	//End fix for STS 368375
	OutputLine(szLine, pFile, TRUE);

    nLevel = 0;

    szRestOfServerList = szCurrentServerList;
    szWorkingServer = szCurrentServerList;

    // as long as there are servers on the list
    while ( szRestOfServerList != NULL && szRestOfServerList[0] != 0 )
    {
        szWorkingServer = szRestOfServerList;
        szRestOfServerList = strchr( szRestOfServerList, ',' );

        if ( szRestOfServerList )
        {
            // there are more servers - nuke the embedded blank

            szRestOfServerList[0] = 0;
            szRestOfServerList += 1;
        }

        ReportRemoteServerList( szWorkingServer, szType, nLevel+1, pFile );
    }

    return dwRet; // return any errors locating starting list
}

BOOL ExportServerListToFile(TCHAR* szExportFile)
{
	int nResult = 0;
	FILE* pFile = NULL;

	TCHAR tszTempName[TMP_MAX] = {0};
	errno_t err = _ttmpnam_s(tszTempName, sizeof(tszTempName)/sizeof(tszTempName[0]));


	if(err == EINVAL || err == ERANGE)
	{
		return FALSE;
	}

	if(*szExportFile)
	{
		pFile = _tfopen(tszTempName, _T("a"));
		if(!pFile)
		{

			LoadString(g_ResLoader.GetResourceInstance(), IDS_ERROR_OPENING_EXPORT_FILE, szBuffer, MAX_BUFFER);
			_tprintf(szBuffer);
			return FALSE; // error - could not open file
		} 
	}

	ReportServerList(pFile, _T("Parent"));

	if(*szExportFile)
	{

		if(fclose(pFile))
		{
			LoadString(g_ResLoader.GetResourceInstance(), IDS_EXPORT_SERVERLIST, szBuffer, MAX_BUFFER);
			ReportLastError(szBuffer, NULL);
			return FALSE;; // error closing file
		}

		nResult = _taccess(szExportFile, 0);

		if(nResult == EACCES)
		{
			_tremove(tszTempName); // try to delete temp file
			LoadString(g_ResLoader.GetResourceInstance(), IDS_EXPORT_SERVERLIST, szBuffer, MAX_BUFFER);
			ReportLastError(szBuffer, NULL);
			return FALSE;  // file already exists and can't get access.
		}

		if(nResult == 0)
		{
			// file already exists.  Delete it.
			if(_tremove(szExportFile) != 0)
			{
				// error deleting existing file
				_tremove(tszTempName); // try to delete temp file
				LoadString(g_ResLoader.GetResourceInstance(), IDS_EXPORT_SERVERLIST, szBuffer, MAX_BUFFER);
				ReportLastError(szBuffer, NULL);
				return FALSE; // error deleting file.
			}
		}

		// rename temp file 

		if(_trename(tszTempName, szExportFile) != 0)
		{
			_tremove(tszTempName); // try to delete temp file
			LoadString(g_ResLoader.GetResourceInstance(), IDS_EXPORT_SERVERLIST, szBuffer, MAX_BUFFER);
			ReportLastError(szBuffer, NULL);
			return FALSE;
		}
	}
	return TRUE;
}

/**************************************************************************************************/
BOOL IsServiceRunning(SC_HANDLE schSCManager,LPCTSTR szService)
{
    SC_HANDLE   schService;
    SERVICE_STATUS  status;

    schService = OpenService(schSCManager, szService, SERVICE_QUERY_STATUS);

    if ( schService == NULL )
        return FALSE;

    lpfnQueryServiceStatus(schService, &status);

    lpfnCloseServiceHandle(schService);

    return(status.dwCurrentState == SERVICE_RUNNING);
}

BOOL IsSAVRunning( )
{
    BOOL bRet = FALSE;
    SC_HANDLE   schSCManager;

	schSCManager = lpfnOpenSCManager(
								NULL,                   // machine (NULL == local)
								NULL,                   // database (NULL == default)
								SC_MANAGER_ALL_ACCESS   // access required
								);
	if ( schSCManager )
	{
	    bRet = IsServiceRunning( schSCManager, _T(SERVICE_NAME) );
	    lpfnCloseServiceHandle(schSCManager);
	}

	return bRet;
}




//============================================================================
DWORD StartDriver(IN SC_HANDLE SchSCManager,IN LPCTSTR DriverName)
{
    SC_HANDLE  schService;
    DWORD      err;

    schService=OpenService(SchSCManager,DriverName,SERVICE_ALL_ACCESS);
    if ( schService==NULL )
        return P_NO_OPEN_SERV;
    err=lpfnStartService(schService,0,NULL);
    if ( !err )
    {
        err = GetLastError();
        if ( err == ERROR_SERVICE_ALREADY_RUNNING )
            err = 0;
        else
		{

			LoadString(g_ResLoader.GetResourceInstance(), IDS_STARTDRIVER_ERROR, szBuffer, MAX_BUFFER);
            _tprintf(szBuffer,err);
		}
    }
    else
    {
        err = 0;
    }

    lpfnCloseServiceHandle(schService);
    return err;
}

//============================================================================
DWORD StopDriver(IN SC_HANDLE SchSCManager,IN LPCTSTR DriverName)
{
    SC_HANDLE       schService;
    BOOL            ret;
    SERVICE_STATUS  serviceStatus;

    schService=OpenService(SchSCManager,DriverName,SERVICE_ALL_ACCESS);
    if ( schService==NULL )
        return P_NO_OPEN_SERV;
    ret=lpfnControlService(schService,SERVICE_CONTROL_STOP,&serviceStatus);
    if ( !ret )
        ret = GetLastError();
    else
    {
        while ( serviceStatus.dwCurrentState == SERVICE_STOP_PENDING )
        {
            Sleep(100);
            lpfnQueryServiceStatus(schService,&serviceStatus);
        }
        ret = 0;
    }

    lpfnCloseServiceHandle (schService);
    return ret;
}

//////////////////////////////////////////////////////////////////////////
//
// Function: NukeValues()
//
// Parameters:
//		HKEY - Handle to source registry key to copy from
//		HKEY - Handle to destination registry to copy to
//
// Returns:
//		N/A at the moment, need to return true/false
//
// Description:  
//	This function enumerates all the values of hSource and copies them to 
//  hDest.  Currently it only handles DWORD and SZ values.
//
//////////////////////////////////////////////////////////////////////////
// 8/21/03 - SKENNED function created.
//////////////////////////////////////////////////////////////////////////
void NukeValues( HKEY hSKey )
{
	TCHAR szName[MAX_PATH] = {0};
	DWORD dwIndex = 0;
	DWORD dwSize = sizeof( szName );
	DWORD dwType = 0;

	BOOL bSomethingDeleted = FALSE;

	do 
	{
		bSomethingDeleted = FALSE;

		dwIndex = 0;

		LONG lRet = RegEnumValue( hSKey, dwIndex, szName, &dwSize, NULL, &dwType, NULL, NULL );
		while( ERROR_SUCCESS == lRet )
		{
			bSomethingDeleted = TRUE;
			RegDeleteValue( hSKey, szName );

			++dwIndex;
			dwSize = sizeof( szName );

			lRet = RegEnumValue( hSKey, dwIndex, szName, &dwSize, NULL, &dwType, NULL, NULL );
		}
	} while( bSomethingDeleted );
}

//////////////////////////////////////////////////////////////////////////
//
// Function: NukeKeys()
//
// Parameters:
//		HKEY - Handle to source registry key to copy from
//		HKEY - Handle to destination registry to copy to
//		TCHAR * - Name of key to open/create
//
// Returns:
//		N/A at the moment, need to return true/false
//
// Description:  
//	This recursive function copies the hSource keys to the hDest including all
//  associated values.
//
//////////////////////////////////////////////////////////////////////////
// 8/21/03 - SKENNED function created.
//////////////////////////////////////////////////////////////////////////
void NukeKeys( HKEY hSKey, TCHAR *szKeyname )
{
	HKEY hSubkey = NULL;
	TCHAR szSubkeyname[ MAX_PATH ] = {0};
	DWORD dwSize = sizeof( szSubkeyname );
	DWORD dwIndex = 0;

	BOOL bSomethingDeleted;

	do 
	{
		bSomethingDeleted = FALSE;
		dwIndex = 0;
		if( ERROR_SUCCESS == RegOpenKeyEx( hSKey, szKeyname, NULL, KEY_WRITE | KEY_READ, &hSubkey ) )
		{
			LONG lRet = RegEnumKeyEx( hSubkey, dwIndex, szSubkeyname, &dwSize, NULL, NULL, NULL, NULL );
			while( ERROR_SUCCESS == lRet )
			{
				++dwIndex;
				dwSize = sizeof( szSubkeyname );
				
				NukeKeys( hSubkey, szSubkeyname );
				bSomethingDeleted = TRUE;

				lRet = RegEnumKeyEx( hSubkey, dwIndex, szSubkeyname, &dwSize, NULL, NULL, NULL, NULL );
			}

			NukeValues( hSubkey );

			if( hSubkey )
				RegCloseKey( hSubkey );
		}
	} while ( bSomethingDeleted );

	RegDeleteKey( hSKey, szKeyname );
}


//////////////////////////////////////////////////////////////////////////
//
// Function: NukeMigrateTreeHKLM()
//
// Parameters:
//		DWORD - Mode to open reg key in, ie KEY_READ or KEY_WRITE
//
// Returns:
//		HKEY - Handle to the registry key if successful, NULL otherwise
//
// Description:  
//	This opens the Software\\Symantec\\Install\\MigrateSettings\\HKEY_LOCAL_MACHINE
//  registry key.
//
//////////////////////////////////////////////////////////////////////////
// 11/11/03 - JMEADE function created to fix 1-1YTCI4.
//////////////////////////////////////////////////////////////////////////
void NukeMigrateTreeHKLM( )
{
	HKEY hKey = NULL;

	RegCreateKeyEx(	HKEY_LOCAL_MACHINE,
					_T("Software\\Symantec"),
					0,
					NULL,
					REG_OPTION_NON_VOLATILE,
					KEY_WRITE | KEY_READ,
					NULL,
					&hKey,
					NULL );

	NukeKeys( hKey, "XInstall" );

	RegCloseKey( hKey );
}

BOOL ValuesEqual( HKEY hSKey, HKEY hDKey, TCHAR *szValueName, BOOL bExact )
{
    BYTE ValueS[1024];
    BYTE ValueD[1024];
    DWORD dwValType;

	BYTE Zeroes[4] = { 0,0,0,0 };

    BOOL bRet = FALSE;

    DWORD dwSizeS;
    DWORD dwSizeD;

    dwSizeS = sizeof(ValueS);
    dwSizeD = sizeof(ValueD);

    DWORD dwRetS = SymSaferRegQueryValueEx( hSKey, szValueName, NULL, &dwValType, (PBYTE) ValueS, &dwSizeS );
    DWORD dwRetD = SymSaferRegQueryValueEx( hDKey, szValueName, NULL, &dwValType, (PBYTE) ValueD, &dwSizeD );

    if ( ( dwRetS == ERROR_SUCCESS &&               // both present and equal
           dwRetD == ERROR_SUCCESS && 
           dwSizeS == dwSizeD && 
           ! memcmp( ValueS, ValueD, dwSizeS ) ) ||
		 ( dwRetS == ERROR_FILE_NOT_FOUND &&        // both absent
		   dwRetD == ERROR_FILE_NOT_FOUND ) || 
         ( ! bExact &&                              // the next 2 are slightly fuzzy matches
    		 ( dwRetS == ERROR_FILE_NOT_FOUND &&    // one absent, and the other a DWORD of 0
    		   dwRetD == ERROR_SUCCESS &&
    		   dwSizeD == 4 &&
    		   !memcmp( ValueD, Zeroes, dwSizeD ) ) ||
    		 ( dwRetD == ERROR_FILE_NOT_FOUND &&    // the other absent, and one a DWORD of 0
    		   dwRetS == ERROR_SUCCESS &&
    		   dwSizeS == 4 &&
    		   !memcmp( ValueS, Zeroes, dwSizeS ) )
         )
	   )
    {
        bRet = TRUE;
    }

	if ( ! bRet )
		printf( "Value %s failed to match\n", szValueName );

    return bRet;
}

//////////////////////////////////////////////////////////////////////////
//
// Function: ReconcileScheduleKeys()
//
// Parameters:
//		HKEY - Handle to source registry key to copy from
//		HKEY - Handle to destination registry to copy to
//		TCHAR * - Name of key to open/create
//
// Returns:
//		N/A at the moment, need to return true/false
//
// Description:  
//	This recursive function copies the hSource keys to the hDest including all
//  associated values.
//
//////////////////////////////////////////////////////////////////////////
// 8/21/03 - SKENNED function created.
//////////////////////////////////////////////////////////////////////////
void ReconcileScheduleKeys( HKEY hSKey, HKEY hDKey, TCHAR *szLocalScanKey )
{
	HKEY hSLocalScanKey = NULL;
    HKEY hDProductControlKey = NULL;

	TCHAR szSScanKeyName[ MAX_PATH ] = {0};

	DWORD dwSIndex;
    DWORD dwSize;

    typedef enum 
    {
        SCHED_MIGRATION_MATCH_SCAN_ITEMS = 1,
    	SCHED_MIGRATION_MATCH_SCHEDULE,
        SCHED_MIGRATION_EXACT
    };

    DWORD  dwSchedMigrationType = SCHED_MIGRATION_MATCH_SCAN_ITEMS;

    LONG lRet;
    LONG lResult;
    DWORD dwValType = 0;

    // get the roam schedule migration settings

	if( ERROR_SUCCESS == RegOpenKeyEx( hDKey, "ProductControl", NULL, KEY_READ, &hDProductControlKey ) )
    {
        dwSize = sizeof(dwSchedMigrationType);
        lResult = SymSaferRegQueryValueEx( hDProductControlKey, "ScheduleMatch", NULL, &dwValType, (BYTE *)&dwSchedMigrationType, &dwSize );

        RegCloseKey( hDProductControlKey );
    }

    // enumerate LocalScans, looking for ClientServer schedules

	TCHAR szDScanKeyName[ MAX_PATH ] = {0};
	
	DWORD dwDIndex;
    HKEY hDTempKey = NULL;
	
	// There is some delay in the creation of the Scheduled scan registry keys by rtvscan.
	// Wait for the keys to be created and start our processing after that. At the most
	// we will wait 15 seconds.
	int nInterval, nIterations;
	nInterval = 100;
	nIterations = 15000/nInterval;

	for(int i=0; i < nIterations; ++i)
	{
		if( ERROR_SUCCESS == RegOpenKeyEx( hDKey, szLocalScanKey, NULL, KEY_READ, &hDTempKey ) )
		{
			dwSize = sizeof( szDScanKeyName );
			
			dwDIndex = 0;
			lRet = RegEnumKeyEx( hDTempKey, dwDIndex, szDScanKeyName, &dwSize, NULL, NULL, NULL, NULL );
			
			for(; lRet == ERROR_SUCCESS; ++dwDIndex)
			{
				if( !strncmp( szDScanKeyName, "ClientServerScheduledScan_", strlen( "ClientServerScheduledScan_" )))
				{
					Sleep(nInterval);
					break;
				}
				lRet = RegEnumKeyEx( hDTempKey, dwDIndex, szDScanKeyName, &dwSize, NULL, NULL, NULL, NULL );
			}
			RegCloseKey(hDTempKey);
			if( !strncmp( szDScanKeyName, "ClientServerScheduledScan_", strlen( "ClientServerScheduledScan_" )))
				break;
		}
		
		Sleep(nInterval);
	}

	if( ERROR_SUCCESS == RegOpenKeyEx( hSKey, szLocalScanKey, NULL, KEY_READ, &hSLocalScanKey ) )
	{
        dwSize = sizeof( szSScanKeyName );

        dwSIndex = 0;
		lRet = RegEnumKeyEx( hSLocalScanKey, dwSIndex, szSScanKeyName, &dwSize, NULL, NULL, NULL, NULL );
		while( ERROR_SUCCESS == lRet )
		{
            if ( ! strncmp( szSScanKeyName, "ClientServerScheduledScan_", strlen( "ClientServerScheduledScan_" ) ) )
            {
                // this is one the schedules I need to fix - see if I can find a matching scan in the new keys
                
                HKEY hSScanKey = NULL;
                HKEY hSScanSchedKey = NULL;

				szDScanKeyName[0] = 0;

				printf( "Source scan %s\n", szSScanKeyName );

                // open the scan itself

                lResult = RegOpenKeyEx( hSLocalScanKey, szSScanKeyName, NULL, KEY_READ, &hSScanKey );
                lResult = RegOpenKeyEx( hSScanKey, "Schedule", NULL, KEY_READ, &hSScanSchedKey );

                HKEY hDLocalScanKey = NULL;
                HKEY hDScanKey = NULL;
                HKEY hDScanSchedKey = NULL;

                DWORD dwLastStartTime = 0;
                
                dwSize = sizeof(dwLastStartTime);
                lResult = SymSaferRegQueryValueEx( hSScanSchedKey, "LastStart", NULL, &dwValType, (BYTE *)&dwLastStartTime, &dwSize );
                
                // now check all the scans in the destination

                // note that I always loop through all of them - there may be more than one that matches, depending on
                // what type of match has been selected, and I may need to fix more than one.

                if( ERROR_SUCCESS == RegOpenKeyEx( hDKey, szLocalScanKey, NULL, KEY_READ, &hDLocalScanKey ) )
                {
                    dwSize = sizeof( szDScanKeyName );

                    dwDIndex = 0;
                    lRet = RegEnumKeyEx( hDLocalScanKey, dwDIndex, szDScanKeyName, &dwSize, NULL, NULL, NULL, NULL );
					if(lRet != ERROR_SUCCESS)
					{
						Sleep(5000);
						lRet = RegEnumKeyEx( hDLocalScanKey, dwDIndex, szDScanKeyName, &dwSize, NULL, NULL, NULL, NULL );
					}

                    while( ERROR_SUCCESS == lRet )
                    {
                        if ( ! strncmp( szDScanKeyName, "ClientServerScheduledScan_", strlen( "ClientServerScheduledScan_" ) ) )
                        {
                            // this is one the schedules I need to check - see if I can find a matching scan in the new keys

							printf( "Dest scan %s\n", szSScanKeyName );

                            lResult = RegOpenKeyEx( hDLocalScanKey, szDScanKeyName, NULL, KEY_READ, &hDScanKey);
                            lResult = RegOpenKeyEx( hDScanKey, "Schedule", NULL, KEY_WRITE|KEY_READ, &hDScanSchedKey );

                            BOOL bMigrate = FALSE;

                            switch ( dwSchedMigrationType )
                            {
                            case SCHED_MIGRATION_MATCH_SCHEDULE:
                                bMigrate =  
                                    ValuesEqual( hSScanSchedKey, hDScanSchedKey, "Type", FALSE ) &&
                                    ValuesEqual( hSScanSchedKey, hDScanSchedKey, "TimeWindowWeekly", FALSE ) &&
                                    ValuesEqual( hSScanSchedKey, hDScanSchedKey, "TimeWindowDaily", FALSE ) &&
                                    ValuesEqual( hSScanSchedKey, hDScanSchedKey, "TimeWindowMonthly", FALSE ) &&
                                    ValuesEqual( hSScanSchedKey, hDScanSchedKey, "DayOfWeek", FALSE ) &&
                                    ValuesEqual( hSScanSchedKey, hDScanSchedKey, "MinOfDay", FALSE ) &&
                                    ValuesEqual( hSScanSchedKey, hDScanSchedKey, "DayOfMonth", FALSE );

                                break;

                            case SCHED_MIGRATION_EXACT:
                                bMigrate =  
                                    // when to scan stuff
                                    ValuesEqual( hSScanSchedKey, hDScanSchedKey, "MissedEventEnabled", TRUE ) &&
                                    ValuesEqual( hSScanSchedKey, hDScanSchedKey, "Type", TRUE ) &&
                                    ValuesEqual( hSScanSchedKey, hDScanSchedKey, "TimeWindowWeekly", TRUE ) &&
                                    ValuesEqual( hSScanSchedKey, hDScanSchedKey, "TimeWindowDaily", TRUE ) &&
                                    ValuesEqual( hSScanSchedKey, hDScanSchedKey, "TimeWindowMonthly", TRUE ) &&
                                    ValuesEqual( hSScanSchedKey, hDScanSchedKey, "DayOfWeek", TRUE ) &&
                                    ValuesEqual( hSScanSchedKey, hDScanSchedKey, "MinOfDay", TRUE ) &&
                                    ValuesEqual( hSScanSchedKey, hDScanSchedKey, "DayOfMonth", TRUE ) &&
                                    ValuesEqual( hSScanSchedKey, hDScanSchedKey, "RandomizeDayEnabled", TRUE ) &&
                                    ValuesEqual( hSScanSchedKey, hDScanSchedKey, "RandomizeWeekEnabled", TRUE ) &&
                                    ValuesEqual( hSScanSchedKey, hDScanSchedKey, "RandomizeMonthEnabled", TRUE ) &&
                                    ValuesEqual( hSScanSchedKey, hDScanSchedKey, "RandomizeDayRange", TRUE ) &&
                                    ValuesEqual( hSScanSchedKey, hDScanSchedKey, "RandomizeWeekStart", TRUE ) &&
                                    ValuesEqual( hSScanSchedKey, hDScanSchedKey, "RandomizeWeekEnd", TRUE ) &&
                                    ValuesEqual( hSScanSchedKey, hDScanSchedKey, "RandomizeMonthRange", TRUE) &&
                                    // what to scan stuff
                                    ValuesEqual( hSScanKey, hDScanKey, "ScanAllDrives", TRUE ) &&
                                    ValuesEqual( hSScanKey, hDScanKey, "FileType", TRUE ) &&
                                    ValuesEqual( hSScanKey, hDScanKey, "Types", TRUE ) &&
                                    ValuesEqual( hSScanKey, hDScanKey, "ZipFile", TRUE ) &&
                                    ValuesEqual( hSScanKey, hDScanKey, "Directories", TRUE ) &&
                                    ValuesEqual( hSScanKey, hDScanKey, "Files", TRUE );

                                break;

                            case SCHED_MIGRATION_MATCH_SCAN_ITEMS:
                            default:
                                bMigrate =  
                                    ValuesEqual( hSScanSchedKey, hDScanSchedKey, "Type", FALSE ) &&
                                    ValuesEqual( hSScanKey, hDScanKey, "ScanAllDrives", FALSE ) &&
                                    ValuesEqual( hSScanKey, hDScanKey, "FileType", FALSE ) &&
                                    ValuesEqual( hSScanKey, hDScanKey, "Types", FALSE ) &&
                                    ValuesEqual( hSScanKey, hDScanKey, "ZipFile", FALSE ) &&
                                    ValuesEqual( hSScanKey, hDScanKey, "Directories", FALSE ) &&
                                    ValuesEqual( hSScanKey, hDScanKey, "Files", FALSE );

                                break;
                            }

                            if ( bMigrate )
                            {
                                // so this scan looks pretty much like the old one, move the last start time

                                lRet = RegSetValueEx( hDScanSchedKey, "LastStart", NULL, REG_DWORD, (BYTE *)&dwLastStartTime, sizeof( dwLastStartTime ) );
                                printf( "Adjusting LastStart for scan, result %d time %x\n", lRet, dwLastStartTime );
                            }

                            RegCloseKey( hDScanKey );
                            RegCloseKey( hDScanSchedKey );
                        }

                        ++dwDIndex;
                        dwSize = sizeof( szDScanKeyName );

                        lRet = RegEnumKeyEx( hDLocalScanKey, dwDIndex, szDScanKeyName, &dwSize, NULL, NULL, NULL, NULL );
                    }

                    RegCloseKey( hDLocalScanKey );
                }

                lResult = RegOpenKeyEx( hSLocalScanKey, szSScanKeyName, NULL, KEY_READ, &hSScanKey );
                lResult = RegOpenKeyEx( hSScanKey, "Schedule", NULL, KEY_READ, &hSScanSchedKey );

            }

            ++dwSIndex;
			dwSize = sizeof( szSScanKeyName );

			lRet = RegEnumKeyEx( hSLocalScanKey, dwSIndex, szSScanKeyName, &dwSize, NULL, NULL, NULL, NULL );
		}

        RegCloseKey( hSLocalScanKey );
	}
}

//////////////////////////////////////////////////////////////////////////
//
// Function: CopyValues()
//
// Parameters:
//		HKEY - Handle to source registry key to copy from
//		HKEY - Handle to destination registry to copy to
//
// Returns:
//		N/A at the moment, need to return true/false
//
// Description:  
//	This function enumerates all the values of hSource and copies them to 
//  hDest.  Currently it only handles DWORD and SZ values.
//
//////////////////////////////////////////////////////////////////////////
// 8/21/03 - SKENNED function created.
//////////////////////////////////////////////////////////////////////////
void CopyValues( HKEY hSKey, HKEY hDKey )
{
	TCHAR szName[MAX_PATH] = {0};
	DWORD dwIndex          = 0;
	DWORD dwSize           = sizeof( szName );
	DWORD dwType           = 0;

	DWORD dwData           = 0;
	DWORD dwDatasize       = 0;
	TCHAR szData[MAX_PATH] = {0};
   
	LONG lRet = RegEnumValue( hSKey, dwIndex, szName, &dwSize, NULL, &dwType, NULL, NULL );
	while( ERROR_SUCCESS == lRet )
	{
		switch( dwType )
		{
			case REG_DWORD:
				dwSize = sizeof( szName );
				dwDatasize = sizeof( DWORD );
				RegEnumValue( hSKey, dwIndex, szName, &dwSize, NULL, NULL, (LPBYTE)&dwData, &dwDatasize );
				RegSetValueEx( hDKey, szName, NULL, REG_DWORD, (CONST BYTE *)&dwData, dwDatasize );
				break;
			case REG_SZ:
				dwSize = sizeof( szName );
				dwDatasize = sizeof( szData );
				RegEnumValue( hSKey, dwIndex, szName, &dwSize, NULL, NULL, (LPBYTE)szData, &dwDatasize );
				RegSetValueEx( hDKey, szName, NULL, REG_SZ, (CONST BYTE *)szData, dwDatasize );
				break;
		}

		++dwIndex;
		dwSize = sizeof( szName );
		lRet = RegEnumValue( hSKey, dwIndex, szName, &dwSize, NULL, &dwType, NULL, NULL );
	}
}

//////////////////////////////////////////////////////////////////////////
//
// Function: CopyKeys()
//
// Parameters:
//		HKEY - Handle to source registry key to copy from
//		HKEY - Handle to destination registry to copy to
//		TCHAR * - Name of key to open/create
//
// Returns:
//		N/A at the moment, need to return true/false
//
// Description:  
//	This recursive function copies the hSource keys to the hDest including all
//  associated values.
//
//////////////////////////////////////////////////////////////////////////
// 8/21/03 - SKENNED function created.
//////////////////////////////////////////////////////////////////////////
void CopyKeys( HKEY hSKey, HKEY hDKey, TCHAR *szKeyname )
{
	HKEY hSubkey = NULL;
	HKEY hWSubkey = NULL;
	TCHAR szSubkeyname[ MAX_PATH ] = {0};
	DWORD dwSize = sizeof( szSubkeyname );
	DWORD dwIndex = 0;

	if( ERROR_SUCCESS != RegCreateKeyEx( hDKey, szKeyname, NULL, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE | KEY_READ, NULL, &hWSubkey, NULL ))
		return;

	if( ERROR_SUCCESS == RegOpenKeyEx( hSKey, szKeyname, NULL, KEY_READ, &hSubkey ) )
	{
		LONG lRet = RegEnumKeyEx( hSubkey, dwIndex, szSubkeyname, &dwSize, NULL, NULL, NULL, NULL );
		while( ERROR_SUCCESS == lRet )
		{
			++dwIndex;
			dwSize = sizeof( szSubkeyname );
			CopyKeys( hSubkey, hWSubkey, szSubkeyname );
			lRet = RegEnumKeyEx( hSubkey, dwIndex, szSubkeyname, &dwSize, NULL, NULL, NULL, NULL );
		}

		CopyValues( hSubkey, hWSubkey );

		if( hSubkey )
			RegCloseKey( hSubkey );
	}

	if( hWSubkey )
		RegCloseKey( hWSubkey );
}

//////////////////////////////////////////////////////////////////////////
//
// Function: CopyRegKeys()
//
// Parameters:
//		HKEY - Handle to source registry key to copy from
//		HKEY - Handle to destination registry to copy to
//
// Returns:
//		BOOL - TRUE if all keys copied without error
//
// Description:  
//	This function copies the settings we need to preserve during a migration
//
//////////////////////////////////////////////////////////////////////////
// 8/21/03 - SKENNED function created.
//////////////////////////////////////////////////////////////////////////
BOOL CopyRegKeys( HKEY hSource, HKEY hDest )
{
	BOOL bRet = TRUE;

	CopyKeys( hSource, hDest, _T("LocalScans") );

	return bRet;
}

//////////////////////////////////////////////////////////////////////////
//
// Function: OpenMigrateTreeHKML()
//
// Parameters:
//		DWORD - Mode to open reg key in, ie KEY_READ or KEY_WRITE
//
// Returns:
//		HKEY - Handle to the registry key if successful, NULL otherwise
//
// Description:  
//	This opens the Software\\Symantec\\Install\\MigrateSettings\\HKEY_LOCAL_MACHINE
//  registry key.
//
//////////////////////////////////////////////////////////////////////////
// 11/11/03 - JMEADE function created to fix 1-1YTCI4.
//////////////////////////////////////////////////////////////////////////
HKEY OpenMigrateTreeHKLM( DWORD dwMode )
{
	HKEY hKey = NULL;

	RegCreateKeyEx(	HKEY_LOCAL_MACHINE,
					_T("Software\\Symantec\\XInstall\\MigrateSettings\\HKEY_LOCAL_MACHINE"),
					0,
					NULL,
					REG_OPTION_NON_VOLATILE,
					dwMode,
					NULL,
					&hKey,
					NULL );

	return hKey;
}

//////////////////////////////////////////////////////////////////////////
//
// Function: OpenSavTreeHKLM()
//
// Parameters:
//		DWORD - Mode to open reg key in, ie KEY_READ or KEY_WRITE
//
// Returns:
//		HKEY - Handle to the registry key if successful, NULL otherwise
//
// Description:  
//	This opens the HKEY_LOCAL_MACHINE\\Software\\INTEL\\LANDesk\\VirusProtect6\\CurrentVersion
//  registry key 
//
//////////////////////////////////////////////////////////////////////////
// 8/21/03 - SKENNED function created.
//////////////////////////////////////////////////////////////////////////
HKEY OpenSavTreeHKLM( DWORD dwMode )
{
	HKEY hKey = NULL;

	RegOpenKeyEx(	HKEY_LOCAL_MACHINE,
					_T("Software\\INTEL\\LANDesk\\VirusProtect6\\CurrentVersion"),
					0,
					dwMode,
					&hKey );

	return hKey;
}

void SaveScheduleSettings( )
{
    HKEY hSource= OpenSavTreeHKLM( KEY_READ );
    HKEY hDest = OpenMigrateTreeHKLM( KEY_WRITE | KEY_READ );

    // actually saves all the settings

    CopyRegKeys( hSource, hDest );

	RegCloseKey( hDest );
	RegCloseKey( hSource );
}

void RestoreScheduleSettings( )
{
    HKEY hDest = OpenSavTreeHKLM( KEY_WRITE | KEY_READ );
    HKEY hSource = OpenMigrateTreeHKLM( KEY_WRITE | KEY_READ );

	ReconcileScheduleKeys( hSource, hDest, _T("LocalScans") );
	
	NukeMigrateTreeHKLM( );

	RegCloseKey( hDest );
	RegCloseKey( hSource );
}


BOOL SetClientParent( char *szParent )
{
    char line[IMAX_PATH];
    char szTemp[IMAX_PATH];
	BOOL  bCheckingInToNewParent = FALSE;

    DWORD dwClientType = 0;
    DWORD dwError;

    BOOL  bRet = TRUE;

    GetHomeDir( HomeDir, sizeof(HomeDir) );

    // Get the client type
    dwClientType = GetVal(hMainKey, "ClientType", 0);

    // Are we a client running on Windows 2000+?
    if ( dwClientType == CLIENT_TYPE_CONNECTED || dwClientType == CLIENT_TYPE_STANDALONE )
    {
        // Yep. Set the path to the Windows common app data directory
        dwError = GetAppDataDirectory( SAV_COMMON_APP_DATA , szTemp, sizeof(szTemp) ) ;

        // If we failed to get the Win2K directory, use the install/home dir.
        if ( dwError != ERROR_SUCCESS )
            strcpy( szTemp, HomeDir );

        sssnprintf(line,sizeof(line),"%s\\GRC.DAT",szTemp);
    }
    else
    {
        // Use the install/home dir in all other cases
        sssnprintf(line,sizeof(line),"%s\\GRC.DAT",HomeDir);
    }

    // I need to know whether I am already managed or not (on the network, that is)

    DWORD dwConnected = 0;
    DWORD dwConnectedSize = sizeof( dwConnected );

    LONG lResult;
    DWORD dwType;

    char szCurrentParent[256] = "";

    DWORD dwLen = sizeof(szCurrentParent);
    BOOL  bLastParentGood = TRUE;
    BOOL  bCurrentParentGood = FALSE;

    lResult = SymSaferRegQueryValueEx( hMainKey, _T("Parent"), NULL, &dwType, (unsigned char *)szCurrentParent, &dwLen );
    if ( lResult == ERROR_SUCCESS && (! stricmp( szCurrentParent, szNoParent ) || strlen(szCurrentParent) == 0) )
    {
        // I am currently set to no parent, check the last real parent, otherwise I didn't shutdown
        // and Parent is a current parent

        bCurrentParentGood = FALSE;
        strcpy( szCurrentParent, "" );
        lResult = SymSaferRegQueryValueEx( hMainKey, _T("LastParent"), NULL, &dwType, (unsigned char *)szCurrentParent, &dwLen );
    }
    else
    {
		if ( lResult == ERROR_SUCCESS && strlen(szCurrentParent) > 0 )
		{
			bCurrentParentGood = TRUE;
		}
    }

    // szCurrentParent is the last known parent - see if the parent has changed

    if ( strlen( szCurrentParent ) == 0 || ! stricmp( szCurrentParent, szNoParent ) )
    {
        bLastParentGood = FALSE;
    }

    char szComputerName[256];
    NTSGetComputerName( szComputerName, NULL );

	lResult = 0;
 	DWORD dwRoamBootstrapForceUnAuth = 0;
 	DWORD dwRoamBootstrapForceUnAuthSize = sizeof(DWORD);
 	dwType = REG_DWORD;
 	lResult = SymSaferRegQueryValueEx( hProductControlKey, _T("RoamBootstrapForce"), NULL, &dwType, (unsigned char *)&dwRoamBootstrapForceUnAuth, &dwRoamBootstrapForceUnAuthSize );
     if ( ERROR_SUCCESS != lResult )
     {
 		// Don't force it since we've seen issues at HP where certs don't get repopulated.
 		//dwRoamBootstrapForceUnAuth = 1;      // By Default do this
 		lResult = 0;
 	}
 	

    // now see if things have changed

    if ( ! stricmp( szCurrentParent, szParent ) )
    {
		// only do this if there was no current good parent
		// it is slightly expensive and if I don't need to do it I won't

		if ( ! bCurrentParentGood )
		{
			// the parent is the same as the last time I had a parent, but it isn't currently
			// the parent - set the correct parent key

			RegSetValueEx( hMainKey, _T("Parent"), NULL, REG_SZ, (const unsigned char*)szParent, strlen( szParent ) );

			// line contains the path to the grc.dat file I will build
			// I use this to force the client to check in - he likely hasn't yet

			// note - dropping a GRC file of any kind deletes all clientserverscheduled scans

			FILE *fGRCFile = fopen( line, "w" );
			if ( fGRCFile )
			{

				BYTE FossilGRCTime[8] = { 0, 0, 0, 0x47, 0x47, 0x47, 0x00, 0x00};

				// build a skeleton GRC file to check in

				_ftprintf( fGRCFile, "[KEYS]\n" );
				_ftprintf( fGRCFile, "!KEY!=$REGROOT$\n" );
				_ftprintf( fGRCFile, "Connected=D1\n" );              // this puts me on the network

				// I write a fossil time so the server will send me the schedules again

	            _ftprintf( fGRCFile, "GRCUpdateTime=B%02x%02x%02x%02x%02x%02x%02x%02x\n", // this is very old, so I get new from parent
                     FossilGRCTime[0],
                     FossilGRCTime[1],
                     FossilGRCTime[2],
                     FossilGRCTime[3],
                     FossilGRCTime[4],
                     FossilGRCTime[5],
                     FossilGRCTime[6],
                     FossilGRCTime[7] );

				fclose( fGRCFile );

				DWORD One = 1;
				RegSetValueEx( hProductControlKey, _T("ProcessGRCNow"), NULL, REG_DWORD, (const unsigned char*)&One, sizeof(One) );

				// might to be more vigilant about this, or let CheckParentAndReset figure out that the 
				// parent isn't there?
			}
		}
    }
    else
    {
        DWORD dwType;
        LONG  lResult; 

		bCheckingInToNewParent = TRUE; //if we are here means it is checking in to a new parent
        // check for a valid old parent - if this was "NoParent" then just continue and set the new one

        if ( bLastParentGood && stricmp( szCurrentParent, szNoParent ) )
        {
            // now check completely out with my old parent - I want to vanish since I am moving to a new server

            BYTE GUID[16];
            dwLen = sizeof(GUID);
            lResult = SymSaferRegQueryValueEx( hMainKey, _T("GUID"), NULL, &dwType, (unsigned char *)GUID, &dwLen );

            char szClientID[256];

            sssnprintf( szClientID, sizeof(szClientID), "Clients\\%s_::_%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x", szComputerName, 
                     GUID[3],
                     GUID[2],
                     GUID[1],
                     GUID[0],
                     GUID[7],
                     GUID[6],
                     GUID[5],
                     GUID[4],
                     GUID[11],
                     GUID[10],
                     GUID[9],
                     GUID[8],
                     GUID[15],
                     GUID[14],
                     GUID[13],
                     GUID[12] );

            if(SendCOM_DEL_KEY(NULL, szCurrentParent, SENDCOM_REMOTE_IS_SERVER, szClientID) == ERROR_SUCCESS)
 			{
			 	///logging checking out client from its old good parent////////////////////////
				SendCOM_LOG_CLIENT_MOVE(NULL, szCurrentParent, SENDCOM_REMOTE_IS_SERVER, COM_LOG_CLIENT_MOVE_ROAM_FROM, szComputerName);
 			}

        }

        // a new parent - set the wheels in motion

        lResult = SymSaferRegQueryValueEx( hMainKey, _T("Connected"), NULL, &dwType, (unsigned char *)&dwConnected, &dwConnectedSize );

        SaveScheduleSettings( );

        // line contains the path to the grc.dat file I will build

        FILE *fGRCFile = fopen( line, "w" );
        if ( fGRCFile )
        {
            BYTE FossilGRCTime[8] = { 0, 0, 0, 0x47, 0x47, 0x47, 0x00, 0x00};

            // build a skeleton GRC file to set the parent and get the
            // full GRC file from the new parent

            _ftprintf( fGRCFile, "[KEYS]\n" );
            _ftprintf( fGRCFile, "!KEY!=$REGROOT$\n" );
            _ftprintf( fGRCFile, "AlertDirectory=S\\%s\\VPALERT$\n", szParent );
            _ftprintf( fGRCFile, "RemoteHomeDirectory=S\\%s\\VPHOME\n", szParent );
            _ftprintf( fGRCFile, "Parent=S%s\n", szParent );
            _ftprintf( fGRCFile, "FullGRCUpdateCounter=D1\n" );   // this is unused, but customarily set
            _ftprintf( fGRCFile, "GRCUpdateTime=B%02x%02x%02x%02x%02x%02x%02x%02x\n", // this is very old, so I get new from parent
                     FossilGRCTime[0],
                     FossilGRCTime[1],
                     FossilGRCTime[2],
                     FossilGRCTime[3],
                     FossilGRCTime[4],
                     FossilGRCTime[5],
                     FossilGRCTime[6],
                     FossilGRCTime[7] );
            _ftprintf( fGRCFile, "Connected=D1\n" );              // this puts me on the network

            // I set the parent to NULL here so that we don't try to check out with the old parent, and I don't
            // have a window where I set the parent key - the grc.dat processing code will try to check out
            // of parent. I do this myself a little further down, but from the LastParent key.

            char szBlankParent[] = "";

            RegSetValueEx( hMainKey, _T("Parent"), NULL, REG_SZ, (const unsigned char*)szBlankParent, strlen( szBlankParent ) );

            fclose( fGRCFile );

            DWORD dwGRCTimeSize = sizeof( FossilGRCTime );
            RegSetValueEx( hMainKey, _T("GRCUpdateTime"), NULL, REG_BINARY, (const unsigned char*)&FossilGRCTime, dwGRCTimeSize );

            DWORD One = 1;
            RegSetValueEx( hProductControlKey, _T("ProcessGRCNow"), NULL, REG_DWORD, (const unsigned char*)&One, sizeof(One) );

			if (dwRoamBootstrapForceUnAuth)
 			{
                 OutputLine( _T("BootStrapForce = 1\n"), NULL, TRUE );
 
 				//
 				// Talked With Rich Sutton
 				// Remove files in pki/roots dir
 				// 
 				HKEY hKey = NULL;
 				DWORD regOptions = KEY_READ;
 				TCHAR szInstallDir[1024];
 				DWORD dwSize = 1024;
 				lResult = RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T("Software\\Intel\\LanDesk\\VirusProtect6\\CurrentVersion\\"), NULL, regOptions, &hKey);
 				if (lResult == ERROR_SUCCESS)
 				{
 					lResult = SymSaferRegQueryValueEx(hKey, _T("Home Directory"), NULL, NULL, LPBYTE(szInstallDir), &dwSize);
 					RegCloseKey(hKey);
 				}
 	
 				if (lResult == ERROR_SUCCESS)
 				{
 					TCHAR strAllFiles [1024];
 					TCHAR strFolder [1024];
 					TCHAR strSFile [1024];
 					_stprintf(strFolder, "%spki\\roots", szInstallDir);
 					_stprintf(strAllFiles,"%s\\*.*", strFolder);
 
 					DWORD		dwFileAttrs					= INVALID_FILE_ATTRIBUTES;
 					dwFileAttrs &= ~FILE_ATTRIBUTE_READONLY;
 					dwFileAttrs &= ~FILE_ATTRIBUTE_HIDDEN;
 			
 					WIN32_FIND_DATA hFindData;
 
 					BOOL bRetFindDelete = TRUE;
 					ZeroMemory( &hFindData, sizeof(hFindData) );
 					HANDLE hFind = FindFirstFile( strAllFiles, &hFindData );
 					while( (INVALID_HANDLE_VALUE != hFind) && (bRetFindDelete) )
 					{
 						_tcscpy (strSFile,strFolder);
 						_tcscat (strSFile,"\\");
 						_tcscat (strSFile,hFindData.cFileName);
 						if( !(hFindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) )
 						{
 							OutputLine( strSFile, NULL, TRUE );
 							OutputLine( " \n", NULL, TRUE );
 							SetFileAttributes(strSFile, dwFileAttrs);
 							DeleteFile( strSFile );
 						}
 
 						bRetFindDelete = FindNextFile( hFind, &hFindData );
 					}
 					
 					if( hFind )
 						FindClose( hFind );
 				}
 
 			}
 
 			// Talked with Rich Sutton
 			// Set cert to reload in case roaming to different server goups with different certs
 			// than existing.
 			One = 1;
 			RegSetValueEx( hProductControlKey, szReg_Val_ReloadRootCertsNow, NULL, REG_DWORD, (const unsigned char*)&One, sizeof(One) );

            // now wait on sav to pick up the GRC dat file - verify that the service is running,
            // rather than boring the user with a pointless delay

            DWORD StartTime;
            DWORD EndTime;

            if ( IsSAVRunning( ) )
            {

                BOOL bNewSettings = FALSE;

                StartTime = GetFineLinearTime( );
                EndTime = StartTime + 60 * 1000;

                while ( GetFineLinearTime( ) < EndTime && ! bNewSettings )
                {
                    DWORD dwProcessGRCNow;
                    DWORD dwProcessGRCNowSize = sizeof( dwProcessGRCNow );

                    DWORD dwType;
                    LONG  lResult; 

                    Sleep( 5 );

                    lResult = SymSaferRegQueryValueEx( hProductControlKey, _T("ProcessGRCNow"), NULL, &dwType, (unsigned char *)&dwProcessGRCNow, &dwProcessGRCNowSize );
                    if ( ERROR_SUCCESS == lResult )
                    {
                        if ( dwProcessGRCNow == 0 )
                        {
                            bNewSettings = TRUE;   // sav has processed the file
                        }
                    }
                }
            }

            // now check if I was already on the net - if I wasn't, then I need to stop/start the service
            // to get it on the network
			SC_HANDLE   schSCManager = NULL;
			schSCManager = lpfnOpenSCManager(
										NULL,                   // machine (NULL == local)
										NULL,                   // database (NULL == default)
										SC_MANAGER_ALL_ACCESS   // access required
										);

			char szLine[1024];
            if ( IsSAVRunning( ) && dwConnected == 0 )
            {
				LoadString(g_ResLoader.GetResourceInstance(), IDS_SAV_NOT_MANAGED, szLine, 1024);
                OutputLine( szLine, NULL, FALSE );

				if ( schSCManager )
				{
	                if ( IsServiceRunning( schSCManager, _T(SERVICE_NAME) ) )
					{
	                    StopDriver( schSCManager, _T(SERVICE_NAME) );
					}
				}
            }
            else if ( ! IsSAVRunning( ) )
            {
				LoadString(g_ResLoader.GetResourceInstance(), IDS_SAV_NOT_MANAGED, szLine, 1024);
                OutputLine( szLine, NULL, FALSE );
            }

			if ( ! IsSAVRunning( ) && schSCManager )
			{
	            StartDriver( schSCManager, _T(SERVICE_NAME) );
				lpfnCloseServiceHandle(schSCManager);
			}

            // now wait for me to get picked up by the parent

            StartTime = GetFineLinearTime( );
            EndTime = StartTime + 60 * 1000;

            BOOL bParentOwnsMe = FALSE;

            while ( GetFineLinearTime( ) < EndTime && ! bParentOwnsMe )
            {
                BYTE GRCTime[8];
                DWORD dwType;
                LONG  lResult; 

                DWORD dwGRCTimeSize = sizeof( GRCTime );

                Sleep( 5 );

                lResult = SymSaferRegQueryValueEx( hMainKey, _T("GRCUpdateTime"), NULL, &dwType, GRCTime, &dwGRCTimeSize );
                if ( ERROR_SUCCESS == lResult )
                {
                    if ( memcmp( FossilGRCTime, GRCTime, sizeof(GRCTime) ) )
                    {
                        bParentOwnsMe = TRUE;   // the parent has sent a new GRC file
                    }
                }
            }

	        if ( bParentOwnsMe )
		    {
			    // I have been taken over - fix up the schedule entries
				RestoreScheduleSettings( );
	        }

            bRet = bParentOwnsMe;   // return whether parent has me or not
        }
        else
        {
			LoadString(g_ResLoader.GetResourceInstance(), IDS_ERROR_CREATING_GRC, szBuffer, MAX_BUFFER);
            ReportLastError( szBuffer, NULL );
            bRet = FALSE;
        }
    }

    // clean this up

    RegDeleteValue( hMainKey, _T("LastParent"));

	if( (bRet == TRUE) && (bCheckingInToNewParent == TRUE) )
 	{
 		///logging add client to a new parent///////////////////////
		SendCOM_LOG_CLIENT_MOVE(NULL, szParent, SENDCOM_REMOTE_IS_SERVER, COM_LOG_CLIENT_MOVE_ROAM_TO, szComputerName);
 	}

    return bRet;
}


VOID ReportErrorByStringID( DWORD dwStringID, BOOL bReportWin32Error )
{
	DWORD dwRet = GetLastError( );


    LoadString(g_ResLoader.GetResourceInstance(), dwStringID, szBuffer2, MAX_BUFFER);

	char szError[1024];
	LoadString(g_ResLoader.GetResourceInstance(), IDS_ERROR, szError, MAX_BUFFER);

	sssnprintf( szBuffer, sizeof(szBuffer), _T("%s - %s (%x)\n"), szBuffer2, szError, dwRet);
	ReportLastError( szBuffer, NULL, bReportWin32Error );
}


void ReportCreateProcessResult( char *szExe, char *szWorkingDir, char *szCommandLine, DWORD dwExitCode )
{
	char szBuffer[1024];
	char szLine[1024];

	LoadString(g_ResLoader.GetResourceInstance(), IDS_CREATE_PROCESS_RESULT, szBuffer, MAX_BUFFER);
	sssnprintf(szLine, sizeof(szLine), szBuffer, szExe, szWorkingDir, szCommandLine, dwExitCode, dwExitCode );
	OutputLine( szLine, NULL, FALSE );
}	


void ReportRemoteCopyFileResult( char *szParent, char *szSourceFile, char *szTargetFile, DWORD dwResult )
{
	char szBuffer[1024];
	char szLine[1024];

	LoadString(g_ResLoader.GetResourceInstance(), IDS_REMOTE_COPY_FILE_RESULT, szBuffer, MAX_BUFFER);
	sssnprintf(szLine, sizeof(szLine), szBuffer, szParent, szSourceFile, szTargetFile, dwResult, dwResult );
	OutputLine( szLine, NULL, FALSE );
}	

// used to build lists of servers for timing and selection

typedef struct tagTIMESERVER
{
    CHAR    *szServerName;

    int     nTimeSamples;
    int     nGoodTimeSamples;
    int     nIndex;
    int     nClients;

    DWORD   dwAverageTime;
    DWORD   dwMinimumTime;          // used to set a minimum response time for controlled failover

    BYTE    TrailingChar;           // indicates relationship to the next server
    char    szServerList[1027];     // 1024 + 3 for padding

    DWORD   dwTimeArray[1];         // variable size - depends on the amount of averaging that is done
} TIMESERVER;

#define NO_RESPONSE 0xfffffffe      // just less than ffffffff
#define CLIENT_COUNT_NOT_FOUND 0xfffffffd	// the server's ClientCount couldn't be read



// average the good times in the server entry

void AverageTime( TIMESERVER *pTimeServer )
{
    int nCount = pTimeServer->nTimeSamples;

    DWORD dwTotal = 0;
    int   nActualSamples = 0;

    int i;

    for ( i=0; i<nCount; i++ )
    {
        DWORD dwCurrTime = pTimeServer->dwTimeArray[i];

        if ( dwCurrTime != NO_RESPONSE )
        {
            dwTotal += dwCurrTime;
            nActualSamples += 1;
        }
    }

    if ( nActualSamples > 0 )
    {
        pTimeServer->dwAverageTime = dwTotal / nActualSamples;

        // handle rounding up

        DWORD dwRem = dwTotal % nActualSamples;
        double fBreak = nActualSamples / 2.0;

        if ( dwRem > fBreak )
        {
            pTimeServer->dwAverageTime += 1;
        }
    }
    else
    {
        pTimeServer->dwAverageTime = NO_RESPONSE;
    }

    pTimeServer->nGoodTimeSamples = nActualSamples;
}
 BOOL FindBestServer( TIMESERVER **ServerList, int nServerCount, char *szBestServer, char *szBestServerList, TIMESERVER **pGoodServerReturn )
 {
 	OutputLine( "In FindBestServer\n", NULL, TRUE );
 
 	char szLine[512];
 
 	BOOL bNextRoundGood = FALSE;
 
     // I have all the time samples, get the averages
 
     for ( int i=0; i<nServerCount; i++ )
     {
         AverageTime( ServerList[i] );
         LoadString(g_ResLoader.GetResourceInstance(), IDS_SERVER_AVG_TIME, szBuffer, MAX_BUFFER);
         _stprintf(szLine, szBuffer, ServerList[i]->szServerName, ServerList[i]->dwAverageTime, ServerList[i]->TrailingChar );
         OutputLine( szLine, NULL, TRUE );
     }
 
     // now find the best server
 
     DWORD dwNextServerListElapsedTime = 0xffffffff;
     TIMESERVER *pGoodServer = NULL;
 
     BOOL bSkipPriorityChain = FALSE;
 
     for ( int i=0; i<nServerCount && ! bDone; i++ )
     {
         // note I check the trailing char in front of me, not after me - what controls
         // skipping me is what the previous guy said - this was broken for quite a while
 
         if ( bSkipPriorityChain && ServerList[i-1]->TrailingChar == '>' )
         {
             continue;
         }
 
         bSkipPriorityChain = FALSE;
 
         if ( (ServerList[i]->nGoodTimeSamples > 0) &&							// need at least 1 good sample
              ServerList[i]->dwAverageTime <= ServerList[i]->dwMinimumTime &&	// has to meet the minimum time    
              ServerList[i]->dwAverageTime < dwNextServerListElapsedTime )		// and quicker than whatever I already have
         {
             // this server was good and quicker than what I had
 
             dwNextServerListElapsedTime = ServerList[i]->dwAverageTime;
             pGoodServer = ServerList[i];
             pGoodServer->nIndex = i;
             LoadString(g_ResLoader.GetResourceInstance(), IDS_SERVER_QUICKER_AVG_TIME, szBuffer, MAX_BUFFER);
             _stprintf(szLine, szBuffer, ServerList[i]->szServerName, ServerList[i]->dwAverageTime );
             OutputLine( szLine, NULL, TRUE );
 
             bNextRoundGood = TRUE;
         }
 
         // if the servers are separated by , or =, then I just move to the next
         // if the separator is > and I just saw a good time, then I have rated
         // it and so skip past the rest of the priority chain
 
         if ( ServerList[i]->dwAverageTime != NO_RESPONSE &&
              ServerList[i]->dwAverageTime != 0xffffffff &&
              ServerList[i]->dwAverageTime <= ServerList[i]->dwMinimumTime &&
              ServerList[i]->TrailingChar == '>' )
         {
             bSkipPriorityChain = TRUE;
         }
     }
 
     // I have found the best responding server
 
     if ( bNextRoundGood && pGoodServer )
     {
         if (szBestServerList) 
             strcpy( szBestServerList, pGoodServer->szServerList );
 
         strcpy( szBestServer, pGoodServer->szServerName );
 
     }
     else
     {
         if (szBestServerList) 
             strcpy( szBestServerList, "" );
 
         strcpy( szBestServer, "" );
 
     }
 
     if (pGoodServerReturn)
     {
         *pGoodServerReturn = pGoodServer;
     }
 
 	OutputLine( "Out FindBestServer\n", NULL, TRUE );
 
     return bNextRoundGood;
 }
 
 
 DWORD GetServerList( TCHAR *szServer, TCHAR *szType, int nLevel, char *szLevelList, DWORD *lpdwElapsedTime )
 {
     BYTE  sendBuf[MAX_PACKET_DATA];
 
     DWORD dwItemsSent = 1;
     DWORD dwRet;
 
 	TCHAR szLogMsg[1024];
 	_stprintf(szLogMsg, "In GetServerList with Server = %s\n", szServer);
 	OutputLine( szLogMsg, NULL, TRUE );
 
     memset(sendBuf, 0, MAX_PACKET_DATA);
 
     // build the registry key to read
 	//
     sprintf( (char *) sendBuf, "RoamManaging%sLevel%d", szType, nLevel );
     
 
     // send == root key name,list of values  buf=[name0name0name00]
     // do get values reply. data of values (trunc if not fit) status = number of successful gets
     // reply buf=[WWdataWWdataWWdata]  WW=word len of data
 
     DWORD dwStartTime = GetFineLinearTime( );
 
     dwRet = /* ! CheckOnNetwork( ) ? ERROR_SEND_FAILED : */ SendCOM_GET_VALUES(NULL, (char *)(const char *) szServer, SENDCOM_REMOTE_IS_SERVER, "ProductControl", (char *) sendBuf, sizeof(sendBuf), &dwItemsSent);
 
     if ( ERROR_SUCCESS == dwRet )
     {
 		TCHAR szLogMsg[1024];
 		_stprintf(szLogMsg, "Success in getting Timing values for server = %s\n", szServer);
 		OutputLine( szLogMsg, NULL, TRUE );
         // suck the string result returned out
 
         WORD wBytes = * (WORD *) sendBuf;
         char *szKey = (char *)sendBuf+2;
 
         if ( wBytes > 0 )
         {
             memcpy( szLevelList, szKey, wBytes );
             szLevelList[wBytes] = 0;
         }
         else
         {
             strcpy( szLevelList, "" );
         }
     }
     else
     {
         strcpy( szLevelList, "" );
 		TCHAR szLogMsg[1024];
 		_stprintf(szLogMsg, "Failure in getting Timing values for server = %s. Error = %d\n", szServer, dwRet);
 		OutputLine( szLogMsg, NULL, TRUE );
 
     }
 
     *lpdwElapsedTime = ElapsedFineLinearTime( dwStartTime, GetFineLinearTime( ) );
 
 	_stprintf(szLogMsg, "Elapsed Time for Server %s is %d\n", szServer, *lpdwElapsedTime);
 	OutputLine( szLogMsg, NULL, TRUE );
 
 	OutputLine( "Out GetServerList\n", NULL, TRUE );
 
     return dwRet;
 }
 
 BOOL GetServerResponseTimesAndNewServerList( TIMESERVER **ServerList, char *szType, int nLevel, char *szWorkingServerList, int nServerCount, int nTimeToAverage, BOOL &bFirstRound )
 {
 	DWORD dwRet;
 
 	OutputLine( "In GetServerResponseTimesAndNewServerList\n", NULL, TRUE );
 
     // as long as there are servers on the list and times to be gotten
 
     int nTimeSampleIndex;
     int nServerIndex;
 
 	char szLine[512];
 
     // if this is the first round I ping the first machine - this is to get
     // the communication channel live - there is typically a long delay on getting
     // the first packet to the first machine - I do this once to get this going.
     // I immediately get that same sample again
 
     if (bFirstRound)
     {
         dwRet = GetServerList( ServerList[0]->szServerName, 
                                szType, nLevel, 
                                szWorkingServerList, 
                                &ServerList[0]->dwTimeArray[0] );
 
         bFirstRound = FALSE;
     }
 
     for ( nTimeSampleIndex=0; nTimeSampleIndex < nTimeToAverage && ! bDone; nTimeSampleIndex++ )
     {
         // I sample the servers on the inner loop so that they are sharing the same network
         // conditions as much as possible
 
         for ( nServerIndex=0; nServerIndex<nServerCount && ! bDone; nServerIndex++ )
         {
             // and get his list of servers, if any
             dwRet = GetServerList( ServerList[nServerIndex]->szServerName, 
                                    szType, nLevel, 
                                    szWorkingServerList, 
                                    &ServerList[nServerIndex]->dwTimeArray[nTimeSampleIndex] );
 
             // dwRet = ERROR_SUCCESS;
             if ( dwRet == ERROR_SUCCESS )
             {
 #if 0
                 PrintIndent( nLevel, NULL );
                 LoadString(szBufferNULL, IDS_TIME_SERVER_LEVEL_DELAY, szBuffer, MAX_BUFFER);
                 _stprintf(szLine, szBuffer, 
                         nTimeSampleIndex, ServerList[nServerIndex]szServerName, nLevel, ServerList[nServerIndex]dwTimeArray[nTimeSampleIndex], dwRet, szWorkingServerList );
                 OutputLine( szLine, NULL, TRUE );
 #endif
                 // save the good server list
 
                 strcpy( ServerList[nServerIndex]->szServerList, szWorkingServerList );
             }
             else
             {
                 PrintIndent( nLevel, NULL );
                 LoadString(g_ResLoader.GetResourceInstance(), IDS_FAILED_TIME_SERVER_LEVEL_DELAY, szBuffer, MAX_BUFFER);
 
                 _stprintf(szLine, szBuffer, nTimeSampleIndex, ServerList[nServerIndex]->szServerName, nLevel, ServerList[nServerIndex]->dwTimeArray[nTimeSampleIndex], dwRet );
                 OutputLine( szLine, NULL, TRUE );
 
                 ServerList[nServerIndex]->dwTimeArray[nTimeSampleIndex] = NO_RESPONSE;
             }
 
             if (nTimeSampleIndex == 0)
                 ServerList[nServerIndex]->nTimeSamples = 0;
 
             ServerList[nServerIndex]->nTimeSamples += 1;
         }
     }
 
 	OutputLine( "Out GetServerResponseTimesAndNewServerList\n", NULL, TRUE );
 
 	return TRUE;
 }
 BOOL BuildServerList( char *szCurrentServerList, TIMESERVER ***ServerList, int &nServerCount, int &nTimeToAverage )
 {
 	OutputLine( "In BuildServerList\n", NULL, TRUE );
 
     // update the server list if possible
 
     char szNextServer[128];
 
     BOOL bRet = FALSE;
 
     DWORD dwRet = ERROR_SUCCESS;
 
     LPSTR szRestOfServerList;
     LPSTR szWorkingServer;
 
     strcpy( szNextServer, "" );
 
     szRestOfServerList = szCurrentServerList;
     szWorkingServer = szCurrentServerList;
 
     nServerCount = CountServers( szCurrentServerList );
 
     int nSize = nServerCount * sizeof(TIMESERVER *);
     *ServerList = (TIMESERVER **) malloc( nSize );
 
     memset( *ServerList, 0, nSize );
 
 	TCHAR szLogMsg[1024];
 	_stprintf(szLogMsg, "ServerCount = %d\n", nSize);
 	OutputLine( szLogMsg, NULL, TRUE );
 
     // walk the server list and build the array
 
     int i;
 
     // as long as there are servers on the list
     for ( i=0; (i<nServerCount) && (szRestOfServerList != NULL) && (szRestOfServerList[0] != 0); i++ )
     {
 		_stprintf(szLogMsg, "Current ServerList = %s\n", szRestOfServerList);
 		OutputLine( szLogMsg, NULL, TRUE );
 
         char cBreakCharacter;
 
         szWorkingServer = szRestOfServerList;
         szRestOfServerList = strpbrk( szRestOfServerList, ",>=" );
 
         if ( szRestOfServerList )
         {
             // there are more servers - save the delimiter and nuke the it
 
             cBreakCharacter = szRestOfServerList[0];
 
             szRestOfServerList[0] = 0;
             szRestOfServerList += 1;
         }
         else
         {
             cBreakCharacter = ',';
         }
 
         // handle any trailing time criteria
 
 		OutputLine("Before check Minimum Time\n", NULL, TRUE );
 
         DWORD dwMinimumTime = 0x7fffffff;       // the default is no minimum, or at least a really long time
         char *szMinimumTime = strpbrk( szWorkingServer, "(" );
 
         if (szMinimumTime)
         {
             char *szActualMinimum = _tcsinc( szMinimumTime );
             szMinimumTime[0] = 0;               // clear the trailing time from the server name
 
             sscanf( szActualMinimum, "%d", &dwMinimumTime );
 
             // note I don't check or need a trailing ) - all I need is the closing delimiter to the
             // next server name - it is fine if it is present
         }
 
         nSize = sizeof(TIMESERVER) + (nTimeToAverage - 1)*sizeof(DWORD);
         TIMESERVER *TimeServerEntry = (TIMESERVER *) malloc( nSize );
         memset( TimeServerEntry, 0, nSize );
 
         TimeServerEntry->TrailingChar = cBreakCharacter;
 		
 		_stprintf(szLogMsg, "Before setting Time Buff ServerName = %s\n", szWorkingServer);
 		OutputLine( szLogMsg, NULL, TRUE );
       //_tcscpy( TimeServerEntryszServerName, szWorkingServer );
        TimeServerEntry->szServerName = szWorkingServer;  
 		
 		TimeServerEntry->dwMinimumTime = dwMinimumTime;
        TimeServerEntry->nTimeSamples = 0;
        strcpy( TimeServerEntry->szServerList, "" );
 
 		// put the server list back - it doesn't really belong to me
 
 		OutputLine("Before setting -1 ServerList time\n", NULL, TRUE );
 
 		if (szRestOfServerList != NULL)
 			szRestOfServerList[-1] = cBreakCharacter;
 
 		OutputLine("AFTER setting -1 ServerList time\n", NULL, TRUE );
 
         (*ServerList)[i] = TimeServerEntry;
 
 		OutputLine("AFTER setting -1 TimeServerEntry\n", NULL, TRUE );
 
     }
 
 	OutputLine( "Out BuildServerList\n", NULL, TRUE );
 
     return bRet;
 }
 
 
 void FreeServerList( int nServerCount, TIMESERVER ***ServerList )
 {
     // free the server list for the next loop
 
     for ( int i=0; i<nServerCount; i++ )
     {
         free ((*ServerList)[i]);
     }
 
     free( *ServerList );
 
     *ServerList = NULL;
 }
 
 DWORD GetUpdatedCopyOfByNameFile( char *szByNameIniFile, char *szType, char *szCurrentServerList )
 {
 	OutputLine( "In GetUpdatedCopyOfByNameFile\n", NULL, TRUE );
 
     DWORD dwRet = ERROR_SUCCESS;
 
 	// what I have is a list of servers, presumably separated by , since 
 	// nothing else makes a lot of sense - I will ignore the significance of
     // < and = and ,
 
     if (szCurrentServerList != NULL && strlen( szCurrentServerList ) > 0 )
     {
 		TCHAR szLogMsg[1024];
 		_stprintf(szLogMsg, "ServerList = %s\n", szCurrentServerList);
 		OutputLine( szLogMsg, NULL, TRUE );
 
         // update the server list if possible
 
 		char szWorkingServerList[1024];
 
 		char szNextServer[128];
 
 		dwRet = ERROR_SUCCESS;
 
 		LPSTR szRestOfServerList;
 		LPSTR szWorkingServer;
 
 		strcpy( szNextServer, "" );
 
         szRestOfServerList = szCurrentServerList;
         szWorkingServer = szCurrentServerList;
 
         int nServerCount;
         int nTimeToAverage = 1;		// time isn't critical, so I only check 1 time
 
         TIMESERVER **ServerList = NULL;
 		BOOL bFirstRound = TRUE;
 
         (void) BuildServerList( szCurrentServerList, &ServerList, nServerCount, nTimeToAverage );
 
         // the list is built, now ping the servers and see wassup. 
 
         (void) GetServerResponseTimesAndNewServerList( ServerList, szType, 1, szWorkingServerList, nServerCount, nTimeToAverage, bFirstRound );
 
         TIMESERVER *pGoodServer;
 		char szBestServer[512];
 
         dwRet = ERROR_SUCCESS;
 
         do
         {
             (void) FindBestServer( ServerList, nServerCount, szBestServer, NULL, &pGoodServer );
 
             if (pGoodServer)
             {
 				TCHAR szLogMsg[1024];
 				_stprintf(szLogMsg, "Good Server Found. Best = %s\n", szBestServer);
 				OutputLine( szLogMsg, NULL, TRUE );
 
                 char szRemotePath[MAX_PATH];
 
                 strcpy( szRemotePath, "$\\" );      // set the path to start at the NAV home directory
                 strcat( szRemotePath, szByNameIniFile );
 
                 // and get the file from the server
   
                 dwRet = CopyFileFromRemoteMachine( szBestServer, 0, szRemotePath, szByNameIniFile );
 
                 if (dwRet != ERROR_SUCCESS)
                 {
                     // this server didn't have what I want - remove him from the list, and ask again
 
 					printf( "error getting file %s from server %s\n", szRemotePath, szBestServer );
 
                     pGoodServer->dwTimeArray[0] = NO_RESPONSE;
                 }
 				else
 				{
 					printf( "got file %s from server %s and put in file %s\n", szRemotePath, szBestServer, szByNameIniFile );
 				}
             }
 			else
 			{
 				OutputLine( "NO Good Server Found\n", NULL, TRUE );
 			}
         } while ( pGoodServer && dwRet != ERROR_SUCCESS );
 
         FreeServerList( nServerCount, &ServerList );
 
     }
 
 	OutputLine( "Out GetUpdatedCopyOfByNameFile\n", NULL, TRUE );
 
     return dwRet;
 }
 
 typedef enum 
 {
     PATTERN_LIST_END = 1,
 	PATTERN_ERROR
 };
 
 BOOL GetPattern( char *szPattern, int nSizeOfPattern, char *szPatternValue, int nSizeOfPatternValue, char **szWalkPatterns, LPDWORD lpdwResult )
 {
 	*lpdwResult = PATTERN_ERROR;
 
 	BOOL bPattOk = FALSE;
 
 	if ( **szWalkPatterns == 0 )
 	{
 		bPattOk = FALSE;
 		*lpdwResult = PATTERN_LIST_END;
 	}
 	else
 	{
 		char *szPattEnd = strchr( *szWalkPatterns, '=' );
 		if ( szPattEnd )
 		{
 			int nPattLen = szPattEnd - *szWalkPatterns;
 			memcpy( szPattern, *szWalkPatterns, nPattLen );
 			szPattern[nPattLen] = 0;
 
 			szPattEnd += 1;		// skip the =
 
 			if ( *szPattEnd != 0 )
 			{
 				*lpdwResult = ERROR_SUCCESS;
 				bPattOk = TRUE;
 
 				// there is a value present
 
 				char *szValueEnd = strchr( szPattEnd, ' ' );
 				if ( szValueEnd == NULL )
 				{
 					// the value ends at the end of the string, point at the end of the string, otherwise i fall through
 					// and treat the " " as the beginning of a comment
 
 					szValueEnd = szPattEnd + strlen( szPattEnd );
 				}
 
 				int nValueLen = szValueEnd - szPattEnd;
 
 				memcpy( szPatternValue, szPattEnd, nValueLen );
 				szPatternValue[nValueLen] = 0;
 			}
 		}
 	}
 
 	// advance to the next pattern
 
 	*szWalkPatterns += strlen( *szWalkPatterns ) + 1;
 
 	if ( ! bPattOk )
 		{
 			*szPattern = 0;
 			*szPatternValue = 0;
 		}
 
 	return bPattOk;
 }
 
 BOOL MatchNameAndGetSection( char *szName, char *szNamePatterns, char *szSectionForName, char *szPoundString )
 {
 	OutputLine( "In MatchNameAndGetSection\n", NULL, TRUE );
 
     BOOL bRet = FALSE;
 
     char szPattern[512];
 	char szPatternValue[512];
 
 	DWORD dwResult;
 
 	char *szWalkPatterns = szNamePatterns;
 
     char *szPoundPointer = szPoundString;
 	
     while( GetPattern( szPattern, sizeof( szPattern ), szPatternValue, sizeof( szPatternValue ), &szWalkPatterns, &dwResult ) )
 	{
 
 		char *szPatternPointer = szPattern;
 		char *szNamePointer = szName;
 	    
 		szPoundPointer = szPoundString;
 
 		// this uses the greedy matching - don't put global stuff like * first
 
 		BOOL bMatchFailed = FALSE;
 
 		while ( ! bMatchFailed && *szPatternPointer != 0 && *szNamePointer != 0 )
 		{
 			char cNext = *szPatternPointer;
 
 			switch (cNext)
 			{
 			case '#':
 				{
 					*szPoundPointer++ = *szNamePointer;
 					break;
 				}
 
 			case '?':
 				{
 					// matches any character, but there must be a character
 					break;
 				}
 
 			case '*':
 				{
 					// matches any number of characters, including zero
 
 					while ( *szNamePointer != 0 )
 					{
 						szNamePointer++;	// skip to the end of the name
 					}
 
 					break;
 				}
 
 			default:
 				{
 					if (cNext == *szNamePointer)
 					{
 						// matched, just skip
 					}
 					else
 					{
 						// matched failed, next pattern
 
 						bMatchFailed = TRUE;
 
 						break;
 					}
 				}
 			}
 
 			szNamePointer++;
 			szPatternPointer++;
 
 		}
 
 		// if this matched, and the name and the pattern are exhausted, 
 		// or the pattern isn't exhausted, but all that is left is a
 		// star then this is a match - this means this is a real match
 
 		if (! bMatchFailed && 
 			*szNamePointer == 0 && 
 			( *szPatternPointer == 0 || *szPatternPointer == '*' ) )
 		{
 			// this is a match
 
 			bRet = TRUE;
 
 			break;
 		}
 	}
 
 	// terminate the #string found
 
 	*szPoundPointer = 0;
 
 	strcpy( szSectionForName, szPatternValue );
 
 	OutputLine( "Out MatchNameAndGetSection\n", NULL, TRUE );
 
     return bRet;
 
 }
 
 DWORD FindByNameParent( char *szType, char *szLocalByNameKey, char *szParentServerList )
 {
 	OutputLine( "In FindByNameParent\n", NULL, TRUE );
 
     char szByNameIniFile[1024];
 
     char szCurrentServerList[1024];
 
     DWORD dwRet = E_FAIL;
 
     BOOL bDone = FALSE;
 
 	LPSTR szEndOfName;
 	char szComputerName[256];
     NTSGetComputerName( szComputerName, NULL );
 
 	TCHAR szLogMsg[1024];
 	_stprintf(szLogMsg, "Computer Name = %s\n", szComputerName);
 	OutputLine( szLogMsg, NULL, TRUE );
 
     // set the ini file name - this is required.
 
     strcpy( szByNameIniFile, szLocalByNameKey );
 
     // the server list may be empty, which means that the local copy is the only
     // golden copy - if the server list is present that may be used to get updated
     // copies of the list
 
     char szIniFilePath[MAX_PATH];
 
     strcpy( szCurrentServerList, "" );          
 
     szEndOfName = strchr( szByNameIniFile, ',' );
     if (szEndOfName)
     {
         // pull off the server list
 
         _tcscpy( szCurrentServerList, szEndOfName+1 );
 
         // and fix up the file name
 
         *szEndOfName = 0;
     }
 
 	_stprintf(szLogMsg, "INI File = %s\n", szByNameIniFile);
 	OutputLine( szLogMsg, NULL, TRUE );
 
     // hmmm - in case of error I press ahead with the file at hand - should maybe
     // say something?
 	// Remotely copy the Master.ini file here.
     (void) GetUpdatedCopyOfByNameFile( szByNameIniFile, szType, szCurrentServerList );
 
     // get the file sections so I can parse the name
 
     char szNamePatterns[33000];
     char szWorkingDir[MAX_PATH];
 
     (void) getcwd( szWorkingDir, sizeof( szWorkingDir ) );
 
     sprintf( szIniFilePath, "%s\\%s", szWorkingDir, szByNameIniFile );
              
 	_stprintf(szLogMsg, "Local INI File Path = %s\n", szIniFilePath);
 	OutputLine( szLogMsg, NULL, TRUE );
 
     dwRet = GetPrivateProfileSection( "NamePatterns", szNamePatterns, sizeof( szNamePatterns ), szIniFilePath );
 
     char szSectionName[1024] = "";
 	char szPoundString[1024] = "";
 	char szServerList[1024] = "";
 	char szServerListName[1024] = "";
 
     BOOL bRet = MatchNameAndGetSection( szComputerName, szNamePatterns, szSectionName, szPoundString );
 	if ( bRet )
 	{
 		printf( "client %s match found for section %s pound string %s\n", szComputerName, szSectionName, szPoundString );
 		_stprintf(szLogMsg, "client %s match found for section %s pound string %s\n", szComputerName, szSectionName, szPoundString);
 		OutputLine( szLogMsg, NULL, TRUE );
 
 		// found a section that matches my name - now find the entry which will be my  - the name of the section
 		// belongs to him - the key name is the string that was sucked in to the ####'s
 
 		dwRet = GetPrivateProfileString( szSectionName, szPoundString, "", szServerListName, sizeof( szServerListName ), szIniFilePath );
 		if (dwRet > 0)
 		{
 			printf( "client %s will use server list %s\n", szComputerName, szServerListName );
 			_stprintf(szLogMsg, "client %s will use server list %s\n", szComputerName, szServerListName);
 			OutputLine( szLogMsg, NULL, TRUE );
 			// I got the server list, go get that, and I am pretty much good to go
 
 			dwRet = GetPrivateProfileString( "ServerLists", szServerListName, "", szServerList, sizeof( szServerList ), szIniFilePath );
 			if (dwRet > 0)
 			{
 				printf( "client %s finds that server list %s is %s\n", szComputerName, szServerListName, szServerList );
 				_stprintf(szLogMsg, "client %s finds that server list %s is %s\n", szComputerName, szServerListName, szServerList);
 				OutputLine( szLogMsg, NULL, TRUE );
 			}
 			else
 			{
 				printf( "client %s could not find server list %s\n", szComputerName, szServerListName );
 				_stprintf(szLogMsg, "client %s could not find server list %s\n", szComputerName, szServerListName);
 				OutputLine( szLogMsg, NULL, TRUE );
 			}
 		}
 		else
 		{
 			printf( "client %s could not find %s in section %s\n", szComputerName, szPoundString, szSectionName );
 			_stprintf(szLogMsg, "client %s could not find %s in section %s\n", szComputerName, szPoundString, szSectionName );
 			OutputLine( szLogMsg, NULL, TRUE );
 		}
 	}
 	else
 	{
 		printf( "client %s matches no pattern\n", szComputerName );
 		_stprintf(szLogMsg, "client %s matches no pattern\n", szComputerName );
 		OutputLine( szLogMsg, NULL, TRUE );
 	}
 
 	if (bRet && dwRet > 0)
 	{
 		dwRet = ERROR_SUCCESS;
 
 		strcpy( szParentServerList, szServerList );
 
 		OutputLine( "SUCCESS\n", NULL, TRUE );
 
 	}
 	else
 	{
 		dwRet = E_FAIL;
 		OutputLine( "FAILED\n", NULL, TRUE );
 		strcpy( szParentServerList, "" );
 	}
 
 	OutputLine( "Out FindByNameParent\n", NULL, TRUE );
 
     return dwRet;        
 }
 


DWORD FindClosestParent( char *szClosestParent, char *szType )
{
    char szCurrentServerList[1024];
    char szNextServerList[1024];
    char szWorkingServerList[1024];

    char szNextServer[128];
    char szLine[512];

    int  nLevel = 0;

    DWORD dwNextServerListElapsedTime;

    DWORD dwRet = ERROR_SUCCESS;
    BOOL bDone = FALSE;
    BOOL bFirstRound = TRUE;

    LPSTR szRestOfServerList;
    LPSTR szWorkingServer;

    // start with the local list
    BOOL bByName = FALSE;
	dwRet = GetLocalServerList( szCurrentServerList, szType, &dwNextServerListElapsedTime, &bByName);
	LoadString(g_ResLoader.GetResourceInstance(), IDS_LOCAL_MACHINE_STARTS_WITH_SERVER, szBuffer, MAX_BUFFER);

    sssnprintf (szLine, sizeof(szLine), szBuffer, szCurrentServerList );
    OutputLine( szLine, NULL, TRUE );

    if ( strlen(szCurrentServerList) == 0 )
    {
		LoadString(g_ResLoader.GetResourceInstance(), IDS_NO_REGIONAL_SERVERS_FOR_TYPE, szBuffer, MAX_BUFFER);
        sssnprintf(szLine, sizeof(szLine), szBuffer, szType );
        OutputLine( szLine, NULL, FALSE );
        strcpy( szClosestParent, "" );
        return ERROR_GENERAL;
    }

  	if (bByName)
    {
	    char szParentServerList[1024];

        dwRet = FindByNameParent( szType, szCurrentServerList, szParentServerList );
		_tcscpy( szCurrentServerList, szParentServerList );
    }

	strcpy( szNextServer, "" );

	if (dwRet != ERROR_SUCCESS)
	{
		strcpy( szClosestParent, "" );
        return ERROR_GENERAL;
	}

	while ( ! bDone )
	{
		BOOL bNextRoundGood = FALSE;

		dwNextServerListElapsedTime;
		nLevel += 1;

		szRestOfServerList = szCurrentServerList;
		szWorkingServer = szCurrentServerList;

		int nServerCount = CountServers( szCurrentServerList );

		int nTimeToAverage;
		nTimeToAverage = GetVal( hProductControlKey, "TimesToAverage", 7 );

		// the UI only allows values from 2 to 10
		if (nTimeToAverage < 2)
			nTimeToAverage = 2;
		else if (nTimeToAverage > 10)
			nTimeToAverage = 10;

		int nSize = nServerCount * sizeof(TIMESERVER *);
		TIMESERVER **ServerList = (TIMESERVER **) malloc( nSize );

		if (ServerList == NULL)
			return ERROR_MEMORY;

		memset( ServerList, 0, nSize );

		// walk the server list and build the array
		// as long as there are servers on the list
		for ( int i=0; i<nServerCount && szRestOfServerList != NULL && szRestOfServerList[0] != 0; i++ )
		{
			char cBreakCharacter;

			szWorkingServer = szRestOfServerList;
			szRestOfServerList = strpbrk( szRestOfServerList, ",>=" );

			if ( szRestOfServerList )
			{
				// there are more servers - save the delimiter and nuke the it

				cBreakCharacter = szRestOfServerList[0];

				szRestOfServerList[0] = 0;
				szRestOfServerList += 1;
			}
			else
			{
				cBreakCharacter = ',';
			}

			// handle any trailing time criteria

			DWORD dwMinimumTime = 0x7fffffff;       // the default is no minimum, or at least a really long time
			char *szMinimumTime = strpbrk( szWorkingServer, "(" );

			if (szMinimumTime)
			{
				char *szActualMinimum = _tcsinc( szMinimumTime );
				szMinimumTime[0] = 0;               // clear the trailing time from the server name

				sscanf( szActualMinimum, "%d", &dwMinimumTime );

				// note I don't check or need a trailing ) - all I need is the closing delimiter to the
				// next server name - it is fine if it is present
			}

			nSize = sizeof(TIMESERVER) + (nTimeToAverage - 1)*sizeof(DWORD);
			TIMESERVER *TimeServerEntry = (TIMESERVER *) malloc( nSize );
			memset( TimeServerEntry, 0, nSize );

			TimeServerEntry->TrailingChar = cBreakCharacter;
			TimeServerEntry->szServerName = szWorkingServer;
			TimeServerEntry->dwMinimumTime = dwMinimumTime;
			TimeServerEntry->nClients = CLIENT_COUNT_NOT_FOUND;
			TimeServerEntry->nTimeSamples = 0;

			strcpy( TimeServerEntry->szServerList, "" );

			ServerList[i] = TimeServerEntry;
		}

		// the list is built, now ping the servers and see wassup. 
		// as long as there are servers on the list and times to be gotten

		// if this is the first round I ping the first machine - this is to get
		// the communication channel live - there is typically a long delay on getting
		// the first packet to the first machine - I do this once to get this going.
		// I immediately get that same sample again

		if (bFirstRound)
		{
			dwRet = GetRoamServerList( ServerList[0]->szServerName,
								SENDCOM_COMM_NONE,
								szType, nLevel, 
								szWorkingServerList, 
								&ServerList[0]->dwTimeArray[0] );

			bFirstRound = FALSE;
		}

		for ( int nTimeSampleIndex=0; nTimeSampleIndex < nTimeToAverage; nTimeSampleIndex++ )
		{
			// I sample the servers on the inner loop so that they are sharing the same network
			// conditions as much as possible

			// if this is the first sample, we want to keep the connections alive
			// if this is the last sample, we want to close the connections
			SENDCOM_HINTS hint = SENDCOM_COMM_NONE;
			if (nTimeSampleIndex == 0)
				hint = SENDCOM_COMM_KEEP_ALIVE;
			else if (nTimeSampleIndex == (nTimeToAverage-1))
				hint = SENDCOM_COMM_CLOSE;

			for ( int nServerIndex=0; nServerIndex<nServerCount; nServerIndex++ )
			{
				// and get his list of servers, if any
				dwRet = GetRoamServerList( ServerList[nServerIndex]->szServerName, hint,
									szType, nLevel, 
									szWorkingServerList, 
									&ServerList[nServerIndex]->dwTimeArray[nTimeSampleIndex] );

				if ( dwRet == ERROR_SUCCESS )
				{
					// if we successfully connected to this server, and this is the first time talking to it,
					// try to read it's clientcount
					if (ServerList[nServerIndex]->nClients == CLIENT_COUNT_NOT_FOUND)
						ServerList[nServerIndex]->nClients = GetRemoteVal(ServerList[nServerIndex]->szServerName,
																SENDCOM_REMOTE_IS_SERVER,
																szReg_Key_ProductControl,
																szReg_Val_ClientCount,
																CLIENT_COUNT_NOT_FOUND);
#if 1
					PrintIndent( nLevel, NULL );
					LoadString(g_ResLoader.GetResourceInstance(), IDS_TIME_SERVER_LEVEL_DELAY, szBuffer, MAX_BUFFER);
					_stprintf(szLine, szBuffer, 
							nTimeSampleIndex, ServerList[nServerIndex]->szServerName, nLevel, ServerList[nServerIndex]->dwTimeArray[nTimeSampleIndex], dwRet, szWorkingServerList );
					OutputLine( szLine, NULL, TRUE );
#endif
					// save the good server list

					strcpy( ServerList[nServerIndex]->szServerList, szWorkingServerList );
				}
				else
				{
					PrintIndent( nLevel, NULL );
					LoadString(g_ResLoader.GetResourceInstance(), IDS_FAILED_TIME_SERVER_LEVEL_DELAY, szBuffer, MAX_BUFFER);

					_stprintf(szLine, szBuffer, nTimeSampleIndex, ServerList[nServerIndex]->szServerName, nLevel, ServerList[nServerIndex]->dwTimeArray[nTimeSampleIndex], dwRet );
					OutputLine( szLine, NULL, TRUE );

					ServerList[nServerIndex]->dwTimeArray[nTimeSampleIndex] = NO_RESPONSE;
				}

				if (nTimeSampleIndex == 0)
					ServerList[nServerIndex]->nTimeSamples = 0;

				ServerList[nServerIndex]->nTimeSamples += 1;
			}
		}

		// I have all the time samples, get the averages

		for ( int i=0; i<nServerCount; i++ )
		{
			AverageTime( ServerList[i] );
			LoadString(g_ResLoader.GetResourceInstance(), IDS_SERVER_AVG_TIME, szBuffer, MAX_BUFFER);
			_stprintf(szLine, szBuffer, ServerList[i]->szServerName, ServerList[i]->dwAverageTime, ServerList[i]->TrailingChar );
			OutputLine( szLine, NULL, TRUE );
		}

		// now find the best server

		dwNextServerListElapsedTime = 0xffffffff;
		TIMESERVER *pGoodServer = NULL;

		BOOL bSkipPriorityChain = FALSE;

		for ( int i=0; i<nServerCount; i++ )
		{
			// note I check the trailing char in front of me, not after me - what controls
			// skipping me is what the previous guy said - this was broken for quite a while

			if ( bSkipPriorityChain && ServerList[i-1]->TrailingChar == '>' )
			{
				continue;
			}

			bSkipPriorityChain = FALSE;

			if ( (ServerList[i]->nGoodTimeSamples > 0) &&							// need at least 1 good sample
				ServerList[i]->dwAverageTime <= ServerList[i]->dwMinimumTime &&	// has to meet the minimum time    
				ServerList[i]->dwAverageTime < dwNextServerListElapsedTime )		// and quicker than whatever I already have
			{
				// this server was good and quicker than what I had

				dwNextServerListElapsedTime = ServerList[i]->dwAverageTime;
				pGoodServer = ServerList[i];
				pGoodServer->nIndex = i;
				LoadString(g_ResLoader.GetResourceInstance(), IDS_SERVER_QUICKER_AVG_TIME, szBuffer, MAX_BUFFER);
				_stprintf(szLine, szBuffer, ServerList[i]->szServerName, ServerList[i]->dwAverageTime );
				OutputLine( szLine, NULL, TRUE );

				bNextRoundGood = TRUE;
			}

			// if the servers are separated by , or =, then I just move to the next
			// if the separator is > and I just saw a good time, then I have rated
			// it and so skip past the rest of the priority chain

			if ( ServerList[i]->dwAverageTime != NO_RESPONSE &&
				ServerList[i]->dwAverageTime != 0xffffffff &&
				ServerList[i]->dwAverageTime <= ServerList[i]->dwMinimumTime &&
				ServerList[i]->TrailingChar == '>' )
			{
				bSkipPriorityChain = TRUE;
			}
		}

		// I have found the best responding server

		if ( bNextRoundGood && pGoodServer )
		{
			strcpy( szNextServerList, pGoodServer->szServerList );
			strcpy( szNextServer, pGoodServer->szServerName );
		}

		// I am done with the list of machines, check for another round or done

		if ( bNextRoundGood && strlen( szNextServerList ) == 0 )
		{
			// the fastest server has no children

			// here is the behavior matrix

			//          next    end/,           =                   >
			// prev 
			//
			//  front/,         this 1          LB here to last =   this 1
			//  =               back to 1st =   back to 1st =       back to 1st =
			//  >               this 1          LB here to last =   this 1

			// first move to the start of any = chain

			int nCurrIndex = pGoodServer->nIndex;

			while ( nCurrIndex >= 1 && 
					ServerList[nCurrIndex-1]->TrailingChar == '=' )
			{
				nCurrIndex -= 1;
			}

			// here is the behavior matrix, with line 2 deleted since we have no
			// leading =

			//          next    end/,           =                   >
			// prev 
			//
			//  front/,         this 1          LB here to last =   this 1
			//  >               this 1          LB here to last =   this 1

			// if this is a , or > server, then I am done

			if ( ServerList[nCurrIndex]->TrailingChar == ',' ||
				ServerList[nCurrIndex]->TrailingChar == '>'
			)
			{
				pGoodServer = ServerList[nCurrIndex];
			}
			else
			{
				// I am at the start of an = chain

				// I need to pick one - the rule is find the one with the smallest
				// number of clients, and if none have the key with the number of clients
				// then pick one at random (sorry, but I can't do any better)

				// first pick up the client counts if I can

				int nLastEquals = nCurrIndex;

				for ( int i=nCurrIndex; i<nServerCount; i++ )
				{
					if ( ServerList[i]->dwAverageTime != NO_RESPONSE &&
						ServerList[i]->dwAverageTime <= ServerList[i]->dwMinimumTime &&	// has to meet the minimum time    
						ServerList[i]->dwAverageTime != 0xffffffff )
					{
						// don't need to do anything, since we've already read the ClientCount above
					}
					else
						ServerList[i]->nClients = CLIENT_COUNT_NOT_FOUND;

					nLastEquals = i;

					if ( ServerList[i]->TrailingChar != '=' )
						break;
				}

				// now find the one with the least clients

				int nLeastClients = 0x7fffffff;

				pGoodServer = NULL;

				BOOL bSomeDontHaveClientCount = FALSE;
				int nGoodServers = 0;

				for ( int i=nCurrIndex; i<=nLastEquals; i++ )
				{
					// look at the servers that seemed to be alive and find the one with the least clients

					if ( ServerList[i]->dwAverageTime != NO_RESPONSE &&
						ServerList[i]->dwAverageTime <= ServerList[i]->dwMinimumTime &&	// has to meet the minimum time    
						ServerList[i]->dwAverageTime != 0xffffffff )
					{
						if ( ServerList[i]->nClients != CLIENT_COUNT_NOT_FOUND &&
							ServerList[i]->nClients < nLeastClients )
						{
							pGoodServer = ServerList[i];
							nLeastClients = ServerList[i]->nClients;
						}

						nGoodServers += 1;

						if ( ServerList[i]->nClients == CLIENT_COUNT_NOT_FOUND )
						{
							bSomeDontHaveClientCount = TRUE;
						}
					}
				}

				if ( bSomeDontHaveClientCount )
				{
					// some don't have client count - pick one at random - what else can I do

					srand( (unsigned)time( NULL ) );

					// note - this isn't completely equal, but it can't be since these are all
					// integers, if the number of servers doesn't divide RAND_MAX evenly then 
					// some get shorted slightly.

					// to balance this off, I rotate the slot based on the time

					// this is an easy way to work around the problem

					int nTheOneILike = rand( ) % nGoodServers;
					int nRotationFactor = time(NULL) % nGoodServers;
					LoadString(g_ResLoader.GetResourceInstance(), IDS_RANDOM_RESULTS, szBuffer, MAX_BUFFER);
					_stprintf(szLine, szBuffer, nTheOneILike, nRotationFactor );
					OutputLine( szLine, NULL, TRUE );

					nTheOneILike = (nTheOneILike + nRotationFactor) % nGoodServers;

					// find the good server I want

					int nGoodServerIx = 0;
					for ( int i=nCurrIndex; i<=nLastEquals; i++ )
					{
						if ( ServerList[i]->dwAverageTime != NO_RESPONSE &&
							ServerList[i]->dwAverageTime <= ServerList[i]->dwMinimumTime &&	// has to meet the minimum time    
							ServerList[i]->dwAverageTime != 0xffffffff )
						{
							if ( nGoodServerIx == nTheOneILike )
							{
								pGoodServer = ServerList[i];
								break;
							}

							nGoodServerIx += 1;
						}
					}
				}
				else
				{
					// all have a client count

					// pGoodServer has the one with the least clients
				}
			}

			if(pGoodServer)
				strcpy( szClosestParent, pGoodServer->szServerName );

			bDone = TRUE;
		}
		else
		{
			if ( bNextRoundGood )
			{
				// I got a new server list, update the server list, and do it again
				strcpy( szCurrentServerList, szNextServerList );
			}
			else
			{
				// none of the servers in the list answered - make the guy who gave me the list the parent

				// this won't correctly handle the case of server1=server2=server3 above me, but you really
				// shouldn't set up a case list that - load balanced servers, and priority chained servers
				// are mainly intended for the leaf level

				// this behavior should probably be on a reg key, since they might want no parent at all
				// in which case I would return an error and set no parent

				strcpy( szClosestParent, szNextServer );
				bDone = TRUE;
			}
		}

		// free the server list for the next loop

		for ( int i=0; i<nServerCount; i++ )
		{
			free (ServerList[i]);
		}

		free( ServerList );

	}

    return dwRet;        
}


DWORD FindNearestTypeParent( char *szType, char *szParent, DWORD *lpdwElapsedTime, BOOL bInteractive )
{
    char szClosestParent[128];
    DWORD StartTime = 0;
    DWORD dwRet = 0;

    if ( bInteractive )
    {
        StartTime = GetFineLinearTime( );
        FindClosestParent( szClosestParent, szType );
    }
    else
    {
        // not interactive - try periodically until I find a parent - the only reason not to find
        // one is that the network isn't up, or I don't have a list of Level0 servers

        BOOL bDone = FALSE;
        BOOL bFirst = TRUE;

        while ( ! bDone )
        {
            StartTime = GetFineLinearTime( );
            dwRet = FindClosestParent( szClosestParent, szType );

            // either a hard error (no Level0 list) or I got a parent
            if ( dwRet != ERROR_SUCCESS || strlen(szClosestParent) > 0 )
            {
                bDone = TRUE;
                break;
            }

            if ( bFirst )
            {
                char szLine[1024];
                bFirst = FALSE;
				LoadString(g_ResLoader.GetResourceInstance(), IDS_DIDNT_FIND_PARENT, szBuffer, MAX_BUFFER);
                sssnprintf( szLine, sizeof(szLine), szBuffer, szType );
                OutputLine( szLine, NULL, TRUE);
            }

            Sleep( g_dwCheckNewParentInterval );     // I try fairly often, since trying is cheap
        }
    }

    *lpdwElapsedTime = ElapsedFineLinearTime( StartTime, GetFineLinearTime( ) );
    strcpy( szParent, szClosestParent );
    return dwRet;
}

DWORD TimeNetwork( char *szServerList, char *szElapsedTime, char *szDelayTime )
{
    char szCurrentServerList[10240];
    char szWorkingServerList[10240];

    char szNextServer[128];
    char szLine[10240];

    int  nLevel = 0;
    int  nDelayTime;
    int  nElapsedTime;

    DWORD dwRet = ERROR_SUCCESS;

    LPSTR szRestOfServerList;
    LPSTR szWorkingServer;

    if ( szServerList != NULL )
    {
        strcpy( szCurrentServerList, szServerList );
    }
    else
    {
		LoadString(g_ResLoader.GetResourceInstance(), IDS_NO_SERVER_LIST, szLine, 10240);
		OutputLine(szLine, NULL, TRUE);
		return (DWORD) -1;
    }

	LoadString(g_ResLoader.GetResourceInstance(), IDS_LOCAL_MACHINE_STARTS_WITH_SERVER, szBuffer, MAX_BUFFER);
    sssnprintf (szLine, sizeof(szLine), szBuffer, szCurrentServerList );
    OutputLine( szLine, NULL, TRUE );

    strcpy( szNextServer, "" );

    sscanf( szElapsedTime, "%d", &nElapsedTime );
    sscanf( szDelayTime, "%d", &nDelayTime );

    szRestOfServerList = szCurrentServerList;
    szWorkingServer = szCurrentServerList;

    int nServerCount = CountServers( szCurrentServerList );

    int nTimeToAverage;
    nTimeToAverage = GetVal( hProductControlKey, "TimesToAverage", 7 );

	// the UI only allows values from 2 to 10
	if (nTimeToAverage < 2)
		nTimeToAverage = 2;
	else if (nTimeToAverage > 10)
		nTimeToAverage = 10;

	int nSize = nServerCount * sizeof(TIMESERVER *);
    TIMESERVER **ServerList = (TIMESERVER **) malloc( nSize );

    memset( ServerList, 0, nSize );

    // walk the server list and build the array

    int i;

    // as long as there are servers on the list
    for ( i=0; i<nServerCount && szRestOfServerList != NULL && szRestOfServerList[0] != 0; i++ )
    {
        char cBreakCharacter;

        szWorkingServer = szRestOfServerList;
        szRestOfServerList = strpbrk( szRestOfServerList, ",>=" );

        if ( szRestOfServerList )
        {
            // there are more servers - save the delimiter and nuke the it

            cBreakCharacter = szRestOfServerList[0];

            szRestOfServerList[0] = 0;
            szRestOfServerList += 1;
        }
        else
        {
            cBreakCharacter = ',';
        }

        nSize = sizeof(TIMESERVER) + (nTimeToAverage - 1)*sizeof(DWORD);
        TIMESERVER *TimeServerEntry = (TIMESERVER *) malloc( nSize );
        memset( TimeServerEntry, 0, nSize );

        TimeServerEntry->TrailingChar = cBreakCharacter;
        TimeServerEntry->szServerName = szWorkingServer;
        TimeServerEntry->nTimeSamples = 0;

        strcpy( TimeServerEntry->szServerList, "" );

        ServerList[i] = TimeServerEntry;
    }

    // the list is built, now ping the servers and see wassup. 

    // as long as there are servers on the list and times to be gotten

    time_t tStartTime = time(NULL);
    time_t tEndTime = tStartTime + nElapsedTime;

    int nTimeSampleIndex;
    int nServerIndex;

    while ( time(NULL) < tEndTime )
    {
        for ( nTimeSampleIndex=0; nTimeSampleIndex < nTimeToAverage; nTimeSampleIndex++ )
        {
            // I sample the servers on the inner loop so that they are sharing the same network
            // conditions as much as possible

			// if this is the first sample, we want to keep the connections alive
			// if this is the last sample, we want to close the connections
			SENDCOM_HINTS hint = SENDCOM_COMM_NONE;
			if (nTimeSampleIndex == 0)
				hint = SENDCOM_COMM_KEEP_ALIVE;
			else if (nTimeSampleIndex == (nTimeToAverage-1))
				hint = SENDCOM_COMM_CLOSE;

			for ( nServerIndex=0; nServerIndex<nServerCount; nServerIndex++ )
            {
                // and get his list of servers, if any
                dwRet = GetRoamServerList( ServerList[nServerIndex]->szServerName, hint,
                                       "Parent", nLevel, 
                                       szWorkingServerList, 
                                       &ServerList[nServerIndex]->dwTimeArray[nTimeSampleIndex] );

                // dwRet = ERROR_SUCCESS;
                if ( dwRet == ERROR_SUCCESS )
                {
#if 0
                    PrintIndent( nLevel, NULL );
					LoadString(g_ResLoader.GetResourceInstance(), IDS_TIME_SERVER_LEVEL_DELAY, szBuffer, MAX_BUFFER);
                    sssnprintf(szLine, sizeof(szLine), szBuffer, 
                            nTimeSampleIndex, ServerList[nServerIndex]->szServerName, nLevel, ServerList[nServerIndex]->dwTimeArray[nTimeSampleIndex], dwRet, szWorkingServerList );
                    OutputLine( szLine, NULL, TRUE );
#endif
                    // save the good server list

                    strcpy( ServerList[nServerIndex]->szServerList, szWorkingServerList );
                }
                else
                {
                    PrintIndent( nLevel, NULL );
					LoadString(g_ResLoader.GetResourceInstance(), IDS_FAILED_TIME_SERVER_LEVEL_DELAY, szBuffer, MAX_BUFFER);
                    sssnprintf(szLine, sizeof(szLine), szBuffer, nTimeSampleIndex, ServerList[nServerIndex]->szServerName, nLevel, ServerList[nServerIndex]->dwTimeArray[nTimeSampleIndex], dwRet );
                    OutputLine( szLine, NULL, TRUE );

                    ServerList[nServerIndex]->dwTimeArray[nTimeSampleIndex] = NO_RESPONSE;
                }

				if (nTimeSampleIndex == 0)
	                ServerList[nServerIndex]->nTimeSamples = 0;

				ServerList[nServerIndex]->nTimeSamples += 1;
            }
        }

        // I have all the time samples, get the averages

        for ( i=0; i<nServerCount; i++ )
        {
            AverageTime( ServerList[i] );
			LoadString(g_ResLoader.GetResourceInstance(), IDS_SERVER_AVG_TIME_ALL, szBuffer, MAX_BUFFER);
            sssnprintf(szLine, sizeof(szLine), szBuffer, time(NULL) - tStartTime, ServerList[i]->szServerName, ServerList[i]->dwAverageTime, ServerList[i]->TrailingChar, 
                    ServerList[i]->dwTimeArray[0],
                    ServerList[i]->dwTimeArray[1],
                    ServerList[i]->dwTimeArray[2],
                    ServerList[i]->dwTimeArray[3],
                    ServerList[i]->dwTimeArray[4],
                    ServerList[i]->dwTimeArray[5],
                    ServerList[i]->dwTimeArray[6]
                   );
            OutputLine( szLine, NULL, TRUE );
        }

        Sleep( nDelayTime );
    }
    // free the server list for the next loop

    for ( i=0; i<nServerCount; i++ )
    {
        free (ServerList[i]);
    }

    free( ServerList );
    return dwRet;        
}

DWORD FindNearestMachineParent( BOOL bInteractive )
{
    char szClosestParent[128];
    char szLine[512];

    DWORD ElapsedTime = 0;
    DWORD StartTime;

    BOOL bWorked = FALSE;

    (void) FindNearestTypeParent( "Parent", szClosestParent, &ElapsedTime, bInteractive );
	if ( strlen (szClosestParent) > 0 )
    {
		LoadString(g_ResLoader.GetResourceInstance(), IDS_NEAREST_PARENT, szBuffer, MAX_BUFFER);
		sssnprintf( szLine, sizeof(szLine), szBuffer, "",szClosestParent, ElapsedTime );
	}
	else
	{
		LoadString(g_ResLoader.GetResourceInstance(), IDS_ERROR_FIND_PARENT, szBuffer, MAX_BUFFER);
		sssnprintf( szLine, sizeof(szLine), szBuffer, "");
	}
    OutputLine( szLine, NULL, FALSE );

    if ( strlen (szClosestParent) > 0 )
    {
        StartTime = GetFineLinearTime( );

        bWorked = SetClientParent( szClosestParent );

        ElapsedTime = ElapsedFineLinearTime( StartTime, GetFineLinearTime( ) );

        if ( bWorked )
        {
			LoadString(g_ResLoader.GetResourceInstance(), IDS_PARENT_OWNS_CLIENT, szBuffer, MAX_BUFFER);
            sssnprintf(szLine, sizeof(szLine), szBuffer, szClosestParent, ElapsedTime );
            OutputLine( szLine, NULL, FALSE );
        }
        else
        {
			LoadString(g_ResLoader.GetResourceInstance(), IDS_PARENT_HAS_NOT_TAKEN_OVER, szBuffer, MAX_BUFFER);
            sssnprintf(szLine, sizeof(szLine), szBuffer, szClosestParent, ElapsedTime );
            OutputLine( szLine, NULL, FALSE );
        }
    }

    return 0;
}

 // Determine if the specified file is trusted. Copy and adapted from
 // ccSymInterfaceLoader.h, CSymInterfaceTrustedLoader class.
 bool IsTrustedFile (const TCHAR *szPath)
 {
 	if (szPath == NULL)
 		return false;
 
     ccVerifyTrust::ccVerifyTrust_IVerifyTrust objVerifyTrustLoader;
     ccVerifyTrust::CVerifyTrustPtr            ptrVerifyTrust;
 
     // Load and create the IVerifyTrust interface
     // If we can not create the trust validation object we will let the load continue
     if (SYM_SUCCEEDED (objVerifyTrustLoader.CreateObject(ptrVerifyTrust.m_p)) && ptrVerifyTrust != NULL)
     {
         if (ptrVerifyTrust->Create(true /*bLoadTrustPolicy*/) != ccVerifyTrust::eNoError)
             ptrVerifyTrust.Release();
     }
 
 	// Make sure file exists.
 	if (::GetFileAttributes (szPath) == INVALID_FILE_ATTRIBUTES)
 		return false;
 
 	// Test for valid symantec digital signature.
     if (ptrVerifyTrust != NULL)
     {
 		// Only fail in case of eVerifyError
         // We let all other errors continue because eVerifyError is the only guarantee that 
         // the validation did indeed fail and it was not a subsystem or other error
         if (ptrVerifyTrust->VerifyFile (szPath, ccVerifyTrust::eSymantecSignature) == ccVerifyTrust::eVerifyError)
             return false;
     }
     return true;
 }
 
 ///////////////////////////////////////////////////////////////////////////////
 //
 // Function name: IsWindows2000
 //
 // Description  : Checks to see if the operating system is Windows 2000 or later.
 //
 // Return Values: TRUE is the OS is Win2K
 //
 ///////////////////////////////////////////////////////////////////////////////
 // 3/5/2000 -   TCASHIN: Function created / header added 
 ///////////////////////////////////////////////////////////////////////////////
 BOOL IsWindows2000()
 {
     OSVERSIONINFO   osInfo;
     BOOL            bRet = FALSE;
 
     osInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO); 
 
     if ( GetVersionEx (&osInfo) )
     {
         if ( osInfo.dwMajorVersion >= 5 )
         {
             bRet = TRUE;
         }
     }
 
     return bRet;
 }
 
 BOOL IsWinNT( void )
 {
     OSVERSIONINFO os;
     os.dwOSVersionInfoSize = sizeof( OSVERSIONINFO );
     GetVersionEx( &os );
     return(BOOL)( os.dwPlatformId == VER_PLATFORM_WIN32_NT );
 }
 
 BOOL SetESMParent( char *szParent )
 {
     BOOL  bRet = TRUE;
 
 	OutputLine( "SetESMParent\n", NULL, TRUE );
 
     // now get the parent ESM.dat file
 
     char szLocalESMDir[1024];
     char szRemoteESMPath[1024];
 	DWORD dwLocalESMDir = sizeof(szLocalESMDir);
 
     DWORD dwType;
     LONG  lResult; 
 
 	_tcscpy(szLocalESMDir, "");
 	lResult = RegQueryValueEx( hProductControlKey, _T("ESMInstallPath"), NULL, &dwType, (unsigned char *)szLocalESMDir, &dwLocalESMDir );
     if ( strlen(szLocalESMDir) <= 0 )
 	{
 		_tcscpy( szLocalESMDir, "c:\\Program Files\\Symantec\\ESM\\" );
 	}
 
 	OutputLine( szLocalESMDir, NULL, TRUE );
 
 #if 0
     // now set the bin directory based on platform
 
     if ( IsWindows2000() )
     {
         _tcscat( szLocalESMDir, "bin\\w2k-ix86\\" );
     }
     else if (IsWinNT( ))
     {
         _tcscat( szLocalESMDir, "bin\\nt-ix86\\" );
     }
     else
     {
         // I don't do windows 9x
 
         bRet = FALSE;
     }
 #endif
 
     DWORD dwRemoteESMPathSize = sizeof( szRemoteESMPath );
 
     char szRegExe[] = "reg.exe";
     char szProxyToken[] = "proxy1.tkn";
     char szLocalPath[1024];
     char szRegExePath[1024];
 
 	// set the default value
 	_tcscpy( szRemoteESMPath, "ESM" );
 
 	OutputLine( "Search for ProductControlKey\\ParentESMPath", NULL, TRUE );
 
     lResult = RegQueryValueEx( hProductControlKey, _T("ParentESMPath"), NULL, &dwType, (unsigned char *)szRemoteESMPath, &dwRemoteESMPathSize );
     if ( strlen(szRemoteESMPath) > 0 )
     {
 		OutputLine( szRemoteESMPath, NULL, TRUE );
 
         // finish build the remote source path
 
         char szRemotePath[1024];
         char szRemoteDir[1024];
 
         strcpy( szRemoteDir, "$\\" );      // set the path to start at the SAV home directory
         strcat( szRemoteDir, szRemoteESMPath );
 		strcat( szRemoteDir, "\\" );
 
         // get the proxy token file
 
         _tcscpy( szLocalPath, szLocalESMDir );
         _tcscat( szLocalPath, szProxyToken );
 
         _tcscpy( szRemotePath, szRemoteDir );
         _tcscat( szRemotePath, szProxyToken );
 
 		OutputLine( "Copy ESM registration token file from Parent Machine\n", NULL, TRUE );
 
         DWORD dwRet = CopyFileFromRemoteMachine( szParent, 0, szRemotePath, szLocalPath );
         if ( dwRet != ERROR_SUCCESS )
         {
             ReportErrorByStringID( IDS_ERROR_RETRIEVING_ESM_PROXY, FALSE );
 
 			bRet = FALSE;
 			TCHAR szLogMsg[1024];
 			_stprintf(szLogMsg, "Error in Copy ESM registration token file = %s from Parent Machine %s. Error = %d\n", szRemotePath, szParent, dwRet);
 			OutputLine( szLogMsg, NULL, TRUE );
 
         }
 		else OutputLine( "Successful Copy ESM registration token file from Parent Machine\n", NULL, TRUE );
 		
 
         // get the registration exe
 
         _tcscpy( szRegExePath, szLocalESMDir );
         _tcscat( szRegExePath, szRegExe );
 
         _tcscpy( szRemotePath, szRemoteDir );
         _tcscat( szRemotePath, szRegExe );
 
 		OutputLine( "Copy ESM registration exe from Parent Machine\n", NULL, TRUE );
         dwRet = CopyFileFromRemoteMachine( szParent, 0, szRemotePath, szRegExePath );
         if ( dwRet != ERROR_SUCCESS )
         {
             ReportErrorByStringID( IDS_ERROR_RETRIEVING_ESM_REG, FALSE );
 
 			bRet = FALSE;
 
 			TCHAR szLogMsg[1024];
 			_stprintf(szLogMsg, "Error in Copy ESM registration exe = %s from Parent Machine %s. Error = %d\n", szRemotePath, szParent, dwRet);
 			OutputLine( szLogMsg, NULL, TRUE );
 
         }
 		else OutputLine( "Successful Copy ESM registration exe from Parent Machine\n", NULL, TRUE );
 	
     }
     else
     {
 		SetLastError(lResult);
 
 		LoadString(g_ResLoader.GetResourceInstance(), IDS_ERROR_RETRIEVING_ESM_PATH, szBuffer, MAX_BUFFER);
 		ReportLastError( szBuffer, NULL );
 
 		OutputLine( "ProductControlKey\\ParentESMPath NOT Found\n", NULL, TRUE );
 
 		bRet = FALSE;
     }
 
     // all the files are in place, let's try to register
     // szLocalPath still has the path to the executable
 
 	if(bRet != FALSE)
 	{
 		if ((GetVal( hProductControlKey, szReg_Val_CheckSigRoamESM, 1) == 0) || (IsTrustedFile(szRegExePath) != false))
 		{
 			OutputLine( "ESM registration exe from Parent Machine is Signed or a fallout key found\n", NULL, TRUE );
 
 			char szCommandLine[512];
 
 			_tcscpy( szCommandLine, szRegExe );
 			_tcscat( szCommandLine, " -Er " );
 			_tcscat( szCommandLine, szProxyToken );
 
 			STARTUPINFO StartupInfo = { 0 };
 			PROCESS_INFORMATION ProcessInformation = { 0 };
 
 			// launch the registration
 
 			BOOL bResult = CreateProcess( 
 				szRegExePath, szCommandLine, 
 				NULL, NULL, FALSE, NORMAL_PRIORITY_CLASS, NULL,
 				szLocalESMDir,
 				&StartupInfo,
 				&ProcessInformation );
 
 			if (bResult)
 			{
 				TCHAR szLogMsg[1024];
 				_stprintf(szLogMsg, "ESM registration exe Launched successfully with params = \"%s\"\n", szCommandLine);
 				OutputLine( szLogMsg, NULL, TRUE );
 
 				DWORD dwRet;
 				BOOL  bResult;
 				DWORD dwExitCode;
 
 				// wait until he is done
 
 				do
 				{
 					dwRet = WaitForSingleObject( ProcessInformation.hProcess, INFINITE );
 
 					bResult = GetExitCodeProcess( ProcessInformation.hProcess, &dwExitCode );
 
 				} while ( dwRet == WAIT_OBJECT_0 && dwExitCode == STILL_ACTIVE  );
 
 				// now that he has exited - see what happened - a result of 0 is good;
 
 				CloseHandle(ProcessInformation.hProcess);
 				CloseHandle(ProcessInformation.hThread);
 
 				bRet = ( dwExitCode == 0 );
 				ReportCreateProcessResult( szRegExePath, szLocalESMDir, szCommandLine, dwExitCode );
 			}
 			else
 			{
 				// couldn't launch for some reason
 
 				ReportErrorByStringID( IDS_ERROR_LAUNCHING_ESM_REG, FALSE );
 
 				bRet = FALSE;
 				TCHAR szLogMsg[1024];
 				_stprintf(szLogMsg, "ESM registration exe failed to launch with params = \"%s\" with error\n", szCommandLine);
 				OutputLine( szLogMsg, NULL, TRUE );
 
 			}
 
 			// see if it worked - if it did, then I roam no more!
 
 			if (bRet)
 			{
 				// get rid of everything that tells me to roam
 
 				g_bRoamESM = FALSE;
 
 				LONG lResult = RegDeleteValue( hProductControlKey, "RoamESM" );
 				if (lResult != ERROR_SUCCESS)
 				{
 					// couldn't delete key for some reason
 
 					ReportErrorByStringID( IDS_ERROR_RETRIEVING_ESM_PROXY, FALSE );
 				}
 			}
 		} // If Trusted File
 		else 
 		{
 			bRet = FALSE;
 			TCHAR szLogMsg[1024];
 			_stprintf(szLogMsg, "ESM registration exe = %s from Parent Machine is NOT Signed and no fallout key found\n", szRegExePath);
 			OutputLine( szLogMsg, NULL, TRUE );
 		}
 	}
 
     return bRet;
 }
 
 DWORD FindNearestESMParent( BOOL bInteractive )
 {
     char szClosestParent[128];
     char szLine[512];
     DWORD ElapsedTime = 0;
     DWORD StartTime;
     BOOL bWorked = FALSE;
 
 	OutputLine( "FindNearestESMParent\n", NULL, TRUE );
 
     (void) FindNearestTypeParent( "ESM", szClosestParent, &ElapsedTime, bInteractive );
 	if ( strlen (szClosestParent) > 0 )
 	{
 		LoadString(g_ResLoader.GetResourceInstance(), IDS_NEAREST_PARENT, szBuffer, MAX_BUFFER);
 		_stprintf( szLine, szBuffer, "ESM", szClosestParent, ElapsedTime );
     }
 	else
 	{
 		LoadString(g_ResLoader.GetResourceInstance(), IDS_ERROR_FIND_PARENT, szBuffer, MAX_BUFFER);
 		_stprintf( szLine, szBuffer, "ESM");
 	}
 	OutputLine( szLine, NULL, FALSE );
     if ( strlen (szClosestParent) > 0 )
     {
         StartTime = GetFineLinearTime( );
 
         bWorked = SetESMParent( szClosestParent );
 
         ElapsedTime = ElapsedFineLinearTime( StartTime, GetFineLinearTime( ) );
 
         if ( bWorked )
         {
 			LoadString(g_ResLoader.GetResourceInstance(), IDS_PARENT_OWNS_ESM_CLIENT, szBuffer, MAX_BUFFER);
             _stprintf(szLine, szBuffer, szClosestParent, ElapsedTime );
             OutputLine( szLine, NULL, FALSE );
         }
         else
         {
 			LoadString(g_ResLoader.GetResourceInstance(), IDS_PARENT_HAS_NOT_TAKEN_OVER_ESM_CLIENT, szBuffer, MAX_BUFFER);
             _stprintf(szLine, szBuffer, szClosestParent, ElapsedTime );
             OutputLine( szLine, NULL, FALSE );
         }
     }
 
     return 0;
 }
 
 BOOL SetITAParent( char *szParent )
 {
     BOOL  bRet = TRUE;
 
 	OutputLine( "SetITAParent\n", NULL, TRUE );
 
     // now get the parent ITA.dat file
 
     char szLocalITADir[1024];
     char szRemoteITAPath[1024];
 	DWORD dwLocalITADir = sizeof(szLocalITADir);
 
     DWORD dwType;
     LONG  lResult; 
 
     // note - it would be better to get the install directory from the install key
     // but this will do for now - we are in a hurry
 
 	_tcscpy(szLocalITADir, "");
 	lResult = RegQueryValueEx( hProductControlKey, _T("ITAInstallPath"), NULL, &dwType, (unsigned char *)szLocalITADir, &dwLocalITADir );
     if ( strlen(szLocalITADir) <= 0 )
 	{
 	    _tcscpy( szLocalITADir, "c:\\Program Files\\Symantec\\ITA\\bin\\" );
 	}
 
     DWORD dwRemoteITAPathSize = sizeof( szRemoteITAPath );
 
     char szTokGenExe[] = "iatokgen.exe";
     char szAgtCfgExe[] = "iaagtcfg.exe";
     char szTokenTxt[]  = "itatoken.txt";
     char szToken[]     = "itatoken.tok";
 
     char szTokenTextPath[1024];
     char szTokGenExePath[1024];
     char szAgtCfgExePath[1024];
 
 	// set the default value
 	_tcscpy( szRemoteITAPath, "ITA" );
 
 	OutputLine( "Search for ProductControlKey\\ParentITAPath", NULL, TRUE );
 
     lResult = RegQueryValueEx( hProductControlKey, _T("ParentITAPath"), NULL, &dwType, (unsigned char *)szRemoteITAPath, &dwRemoteITAPathSize );
     if ( strlen(szRemoteITAPath) > 0 )
     {
 		OutputLine( szRemoteITAPath, NULL, TRUE );
 
         // finish build the remote source path
 
         char szRemotePath[1024];
         char szRemoteDir[1024];
 
         strcpy( szRemoteDir, "$\\" );      // set the path to start at the SAV home directory
         strcat( szRemoteDir, szRemoteITAPath );
 		strcat( szRemoteDir, "\\" );
 
         // and get the needed files
 
 		// token text file
 
         _tcscpy( szTokenTextPath, szLocalITADir );
         _tcscat( szTokenTextPath, szTokenTxt );
 
         _tcscpy( szRemotePath, szRemoteDir );
         _tcscat( szRemotePath, szTokenTxt );
 
 		OutputLine( "Copy ITA registration token file from Parent Machine\n", NULL, TRUE );
 
         DWORD dwRet = CopyFileFromRemoteMachine( szParent, 0, szRemotePath, szTokenTextPath );
         if ( dwRet != ERROR_SUCCESS )
         {
             ReportErrorByStringID( IDS_ERROR_RETRIEVING_ITA_TOKENTXT, FALSE );
 
 			bRet = FALSE;
 			
 			TCHAR szLogMsg[1024];
 			_stprintf(szLogMsg, "Error in Copy ITA registration token = %s from Parent Machine %s. Error = %d\n", szRemotePath, szParent, dwRet);
 			OutputLine( szLogMsg, NULL, TRUE );
 
         }
 		else OutputLine( "Successful Copy ITA registration token from Parent Machine\n", NULL, TRUE );
 		
 
 		// token gen utility
 
         _tcscpy( szTokGenExePath, szLocalITADir );
         _tcscat( szTokGenExePath, szTokGenExe );
 
         _tcscpy( szRemotePath, szRemoteDir );
         _tcscat( szRemotePath, szTokGenExe );
 
 		OutputLine( "Copy ITA registration exe file from Parent Machine\n", NULL, TRUE );
 
         dwRet = CopyFileFromRemoteMachine( szParent, 0, szRemotePath, szTokGenExePath );
 		if ( dwRet != ERROR_SUCCESS )
         {
             ReportErrorByStringID( IDS_ERROR_RETRIEVING_ITA_TOKGENEXE, FALSE );
 
 			bRet = FALSE;
 
 			TCHAR szLogMsg[1024];
 			_stprintf(szLogMsg, "Error in Copy ITA registration exe = %s from Parent Machine %s. Error = %d\n", szRemotePath, szParent, dwRet);
 			OutputLine( szLogMsg, NULL, TRUE );
         }
 		else OutputLine( "Successful Copy ITA registration exe from Parent Machine\n", NULL, TRUE );
 
 		// agent config utility
 
         _tcscpy( szAgtCfgExePath, szLocalITADir );
         _tcscat( szAgtCfgExePath, szAgtCfgExe );
 
         _tcscpy( szRemotePath, szRemoteDir );
         _tcscat( szRemotePath, szAgtCfgExe );
 
 		OutputLine( "Copy ITA registration cfg exe file from Parent Machine\n", NULL, TRUE );
 
         dwRet = CopyFileFromRemoteMachine( szParent, 0, szRemotePath, szAgtCfgExePath );
         if ( dwRet != ERROR_SUCCESS )
         {
             ReportErrorByStringID( IDS_ERROR_RETRIEVING_ITA_AGTCFGEXE, FALSE );
 
 			bRet = FALSE;
 
 			TCHAR szLogMsg[1024];
 			_stprintf(szLogMsg, "Error in Copy ITA registration cfg exe = %s from Parent Machine %s. Error = %d\n", szRemotePath, szParent, dwRet);
 			OutputLine( szLogMsg, NULL, TRUE );
         }
 		else OutputLine( "Successful Copy ITA registration cfg exe from Parent Machine\n", NULL, TRUE );
 
     }
     else
     {
 		SetLastError(lResult);
 
 		LoadString(g_ResLoader.GetResourceInstance(), IDS_ERROR_RETRIEVING_ITA_PATH, szBuffer, MAX_BUFFER);
 		ReportLastError( szBuffer, NULL );
 
 		OutputLine( "ProductControlKey\\ParentITAPath NOT Found", NULL, TRUE );
 
 		bRet = FALSE;
     }
 
     // all the files are in place, let's try to build a token file and register
     // szTokGenExe and szAgtCfgEXe still have the path to the executable
 
 	if(bRet != FALSE)
 	{
 		if ((GetVal( hProductControlKey, szReg_Val_CheckSigRoamITA, 1) == 0) || 
 			((IsTrustedFile(szTokGenExePath) != false) && (IsTrustedFile(szAgtCfgExePath) != false)))
 		{
 			OutputLine( "ITA registration exe and cfg exe from Parent Machine is Signed or a fallout key found\n", NULL, TRUE );
 
 			char szCommandLine[1024];
 			char szTokenFilePath[1024];
 
 			_tcscpy( szTokenFilePath, szLocalITADir );
 			_tcscat( szTokenFilePath, szToken );
 
 			_tcscpy( szCommandLine, szTokGenExe );
 			_tcscat( szCommandLine, " " );
 			_tcscat( szCommandLine, szTokenTxt );	// build from this text token file
 			_tcscat( szCommandLine, " " );
 			_tcscat( szCommandLine, szToken );		// this binary token file
 
 			STARTUPINFO StartupInfo = { 0 };
 			PROCESS_INFORMATION ProcessInformation = { 0 };
 
 			// launch the token generation
 
 			BOOL bResult = CreateProcess( 
 				szTokGenExePath, szCommandLine, 
 				NULL, NULL, FALSE, NORMAL_PRIORITY_CLASS, NULL,
 				szLocalITADir,
 				&StartupInfo,
 				&ProcessInformation );
 
 			if (bResult)
 			{
 				DWORD dwRet;
 				BOOL  bResult;
 				DWORD dwExitCode;
 
 				TCHAR szLogMsg[1024];
 				_stprintf(szLogMsg, "ITA  registration exe Launched successfully with params = \"%s\"\n", szCommandLine);
 				OutputLine( szLogMsg, NULL, TRUE );
 
 				// wait until he is done
 
 				do
 				{
 					dwRet = WaitForSingleObject( ProcessInformation.hProcess, INFINITE );
 
 					bResult = GetExitCodeProcess( ProcessInformation.hProcess, &dwExitCode );
 
 				} while ( dwRet == WAIT_OBJECT_0 && dwExitCode == STILL_ACTIVE  );
 
 				// now that he has exited - see what happened - a result of 0 is good;
 
 				CloseHandle(ProcessInformation.hProcess);
 				CloseHandle(ProcessInformation.hThread);
 
 				bRet = ( dwExitCode == 0 );
 
 				ReportCreateProcessResult( szTokGenExePath, szLocalITADir, szCommandLine, dwExitCode );
 			}
 			else
 			{
 				// couldn't launch for some reason
 
 				ReportErrorByStringID( IDS_ERROR_LAUNCHING_ITA_TOKGENEXE, FALSE );
 
 				bRet = FALSE;
 
 				TCHAR szLogMsg[1024];
 				_stprintf(szLogMsg, "ITA registration exe failed to launch with params = \"%s\" with error\n", szCommandLine);
 				OutputLine( szLogMsg, NULL, TRUE );
 
 			}
 
 			if (bRet)
 			{
 				// token generation worked, now try to configure the agent
 
 				char szCommandLine[1024];
 				char szTokenFilePath[1024];
 
 				_tcscpy( szTokenFilePath, szLocalITADir );
 				_tcscat( szTokenFilePath, szToken );
 
 				_tcscpy( szCommandLine, szAgtCfgExe );
 				_tcscat( szCommandLine, " -T:" );	// configure using the following token file
 				_tcscat( szCommandLine, szToken );
 
 				STARTUPINFO StartupInfo = { 0 };
 				PROCESS_INFORMATION ProcessInformation = { 0 };
 
 				// launch the registration
 
 				BOOL bResult = CreateProcess( 
 					szAgtCfgExePath, szCommandLine, 
 					NULL, NULL, FALSE, NORMAL_PRIORITY_CLASS, NULL,
 					szLocalITADir,
 					&StartupInfo,
 					&ProcessInformation );
 
 				if (bResult)
 				{
 					DWORD dwRet;
 					BOOL  bResult;
 					DWORD dwExitCode;
 
 					
 					TCHAR szLogMsg[1024];
 					_stprintf(szLogMsg, "ITA  registration cfg exe Launched successfully with params = \"%s\"\n", szCommandLine);
 					OutputLine( szLogMsg, NULL, TRUE );
 
 					// wait until he is done
 
 					do
 					{
 						dwRet = WaitForSingleObject( ProcessInformation.hProcess, INFINITE );
 
 						bResult = GetExitCodeProcess( ProcessInformation.hProcess, &dwExitCode );
 
 					} while ( dwRet == WAIT_OBJECT_0 && dwExitCode == STILL_ACTIVE  );
 
 					// now that he has exited - see what happened - a result of 0 is good;
 					CloseHandle(ProcessInformation.hProcess);
 					CloseHandle(ProcessInformation.hThread);
 
 					bRet = ( dwExitCode == 0 );
 
 					ReportCreateProcessResult( szAgtCfgExePath, szLocalITADir, szCommandLine, dwExitCode );
 				}
 				else
 				{
 					// couldn't launch for some reason
 
 					ReportErrorByStringID( IDS_ERROR_LAUNCHING_ITA_AGTCFGEXE, FALSE );
 
 					bRet = FALSE;
 
 					TCHAR szLogMsg[1024];
 					_stprintf(szLogMsg, "ITA registration cfg exe failed to launch with params = \"%s\" with error\n", szCommandLine);
 					OutputLine( szLogMsg, NULL, TRUE );
 				}
 			}
 
 			// see if it worked - if it did, then I roam no more!
 
 			if (bRet)
 			{
 				// get rid of everything that tells me to roam
 
 				g_bRoamITA = FALSE;
 
 				LONG lResult = RegDeleteValue( hProductControlKey, "RoamITA" );
 				if (lResult != ERROR_SUCCESS)
 				{
 					// couldn't delete key for some reason
 
 					ReportErrorByStringID( IDS_ERROR_STOPPING_ITA_ROAM, FALSE );
 				}
 			}
 		} // If Trusted Files
 		else 
 		{
 			bRet = FALSE;
 			TCHAR szLogMsg[1024];
 			_stprintf(szLogMsg, "ITA registration exe = %s cfg = %s from Parent Machine is NOT Signed and no fallout key found\n", szTokGenExePath, szAgtCfgExePath);
 			OutputLine( szLogMsg, NULL, TRUE );
 		}
 	}
 
     return bRet;
 }
 
 DWORD FindNearestITAParent( BOOL bInteractive )
 {
 
     char szClosestParent[128];
     char szLine[512];
     DWORD ElapsedTime = 0;
     DWORD StartTime;
     BOOL bWorked = FALSE;
 
 	OutputLine( "FindNearestITAParent\n", NULL, TRUE );
 
     (void) FindNearestTypeParent( "ITA", szClosestParent, &ElapsedTime, bInteractive );
 	if ( strlen (szClosestParent) > 0 )
 	{
 		LoadString(g_ResLoader.GetResourceInstance(), IDS_NEAREST_PARENT, szBuffer, MAX_BUFFER);
 		_stprintf( szLine, szBuffer, "ITA", szClosestParent, ElapsedTime );
     }
 	else
 	{
 		LoadString(g_ResLoader.GetResourceInstance(), IDS_ERROR_FIND_PARENT, szBuffer, MAX_BUFFER);
 		_stprintf( szLine, szBuffer, "ITA");
 	}
 	OutputLine( szLine, NULL, FALSE );
     if ( strlen (szClosestParent) > 0 )
     {
         StartTime = GetFineLinearTime( );
 
         bWorked = SetITAParent( szClosestParent );
 
         ElapsedTime = ElapsedFineLinearTime( StartTime, GetFineLinearTime( ) );
 
         if ( bWorked )
         {
 			LoadString(g_ResLoader.GetResourceInstance(), IDS_PARENT_OWNS_ITA_CLIENT, szBuffer, MAX_BUFFER);
             _stprintf(szLine, szBuffer, szClosestParent, ElapsedTime );
             OutputLine( szLine, NULL, FALSE );
         }
         else
         {
 			LoadString(g_ResLoader.GetResourceInstance(), IDS_PARENT_HAS_NOT_TAKEN_OVER_ITA_CLIENT, szBuffer, MAX_BUFFER);
             _stprintf(szLine, szBuffer, szClosestParent, ElapsedTime );
             OutputLine( szLine, NULL, FALSE );
         }
     }
 
     return 0;
 }
 

DWORD FindNearestParent( BOOL bInteractive )
{
    if ( g_bRoamClient )
        FindNearestMachineParent( bInteractive );

	 
 	//
 	// NMCI and Bank of America need ESM and ITA back
 	//
 	if ( g_bRoamESM )
     {
         FindNearestESMParent( bInteractive );
     }
 
     if ( g_bRoamITA )
     {
         FindNearestITAParent( bInteractive );
     }

    return 0;
}

DWORD ForceNearestMachineParent( char *szNewParent )
{
    char szLine[512];
    DWORD ElapsedTime = 0;
    DWORD StartTime;

    if ( strlen (szNewParent) > 0 )
    {
		LoadString(g_ResLoader.GetResourceInstance(), IDS_FORCING_NEW_PARENT, szBuffer, MAX_BUFFER);
        sssnprintf(szLine, sizeof(szLine), szBuffer, szNewParent );
        OutputLine( szLine, NULL, FALSE );

		StartTime = GetFineLinearTime( );

		BOOL bWorked = SetClientParent( szNewParent );

		ElapsedTime = ElapsedFineLinearTime( StartTime, GetFineLinearTime( ) );

        if ( bWorked )
        {
			LoadString(g_ResLoader.GetResourceInstance(), IDS_PARENT_OWNS_CLIENT, szBuffer, MAX_BUFFER);
            sssnprintf(szLine, sizeof(szLine), szBuffer, szNewParent, ElapsedTime );
            OutputLine( szLine, NULL, FALSE );
        }
        else
        {
			LoadString(g_ResLoader.GetResourceInstance(), IDS_PARENT_HAS_NOT_TAKEN_OVER, szBuffer, MAX_BUFFER);
            sssnprintf(szLine, sizeof(szLine), szBuffer, szNewParent, ElapsedTime );
            OutputLine( szLine, NULL, FALSE );
        }
    }

    return 0;
}

DWORD CheckAndResetParent( BOOL bInteractive )
{
    char szParent[128] = "";
    char szLine[512];

    DWORD dwConnected = 0;
    DWORD dwPattern = 0;
    DWORD dwType;

    LONG  lResult; 

    DWORD dwLen = sizeof(szParent);
    lResult = SymSaferRegQueryValueEx( hMainKey, _T("Parent"), NULL, &dwType, (unsigned char *)szParent, &dwLen );

    // see if I have a parent at all

    if ( lResult == ERROR_SUCCESS && strlen(szParent) > 0 && stricmp( szParent, szNoParent) )
    {
        DWORD dwConnectedSize = sizeof( dwConnected );

        lResult = SymSaferRegQueryValueEx( hMainKey, _T("Connected"), NULL, &dwType, (unsigned char *)&dwConnected, &dwConnectedSize );

        // I do, see if the parent is fine

        dwPattern = GetRemoteVal( szParent, SENDCOM_REMOTE_IS_SERVER, "", "UsingPattern", 0xffffffff );
        if ( dwPattern != 0xffffffff && dwConnected != 0 )
        {
            // everything is fine, fine, fine
			LoadString(g_ResLoader.GetResourceInstance(), IDS_PARENT_IS_FINE, szBuffer, MAX_BUFFER);
            sssnprintf( szLine, sizeof(szLine), szBuffer, szParent, dwConnected, dwPattern );
            OutputLine( szLine, NULL, FALSE );

            return 0;
        }
    }

    // something is wrong, either no parent, not connected, etc.
	LoadString(g_ResLoader.GetResourceInstance(), IDS_PROBLEM_WITH_PARENT, szBuffer, MAX_BUFFER);
    sssnprintf( szLine, sizeof(szLine), szBuffer, szParent, dwConnected, dwPattern );
    OutputLine( szLine, NULL, FALSE );

    if ( !bInteractive )
    {
        // not interactive, try to fix it
        FindNearestMachineParent( NOT_INTERACTIVE );      // this will fix pretty much anything
    }

    return 0;
}


DWORD CopyFileFromRemoteMachine( LPSTR szParent, SENDCOM_HINTS hints, char *szRemotePath, char *szLocalPath )
{
	DWORD dwRet = MasterFileCopy( szParent, hints, szRemotePath, NULL, 0, szLocalPath, COPY_SAVE_DATE | COPY_ALWAYS );

	ReportRemoteCopyFileResult( szParent, szRemotePath, szLocalPath, dwRet );

	return dwRet;
}



DWORD MachineShutdown( )
{
    DWORD dwClientType = 0;

    BOOL  bRet = TRUE;

    GetHomeDir( HomeDir, sizeof(HomeDir) );

    // Get the client type
    dwClientType = GetVal(hMainKey,"ClientType",0);

    char szCurrentParent[256] = "";

    DWORD dwType;
    LONG  lResult; 

    DWORD dwLen = sizeof(szCurrentParent);
    lResult = SymSaferRegQueryValueEx( hMainKey, _T("Parent"), NULL, &dwType, (unsigned char *)szCurrentParent, &dwLen );

    // Save my current parent, and set up a fake parent so I can't check in when I next start up
    // at least, that is, until I get a chance to set up the parent correctly

    // As a side effect this lets me fix the problem where I am on a machine that starts up off the network
    // and then after I have already checked in the network comes alive. The problem is that
    // I don't check in again until the check-in interval has passed, which might be a long time.

    if ( lResult == ERROR_SUCCESS && stricmp( szCurrentParent, szNoParent ) )
    {
        RegSetValueEx( hMainKey, _T("LastParent"), NULL, REG_SZ, (unsigned char *)szCurrentParent, strlen(szCurrentParent) );

        // now check out with my old parent - I don't want to vanish, so I just set the good flag off
        // assuming that the parent is now honoring the good flag, then he will ignore me until I check
        // in again.

        BYTE GUID[16];
        dwLen = sizeof(GUID);
        lResult = SymSaferRegQueryValueEx( hMainKey, _T("GUID"), NULL, &dwType, (unsigned char *)GUID, &dwLen );

        char szClientID[256];
        char szComputerName[256];

        NTSGetComputerName( szComputerName, NULL );

        // first build the fully GUID'd name - this is where I live on my parent

        sssnprintf( szClientID, sizeof(szClientID), "Clients\\%s_::_%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x", szComputerName, 
                 GUID[3],
                 GUID[2],
                 GUID[1],
                 GUID[0],
                 GUID[7],
                 GUID[6],
                 GUID[5],
                 GUID[4],
                 GUID[11],
                 GUID[10],
                 GUID[9],
                 GUID[8],
                 GUID[15],
                 GUID[14],
                 GUID[13],
                 GUID[12] );

        // Make sure to tell the parent to honor the good flag - this is off by default

        PutRemoteVal( szCurrentParent, SENDCOM_REMOTE_IS_SERVER, szClientID, "good", 0 );
    }

    // mark no parent

    RegSetValueEx( hMainKey, _T("Parent"), NULL, REG_SZ, (unsigned char *)szNoParent, strlen(szNoParent) );

    return bRet;
}

typedef struct tagHOSTBLOCK
{
    DWORD dwSize;
    BYTE  AddrArray[1];
} HOSTBLOCK;

// returns if the address has changed

BOOL GetNetworkAddressCheckUnchanged( HOSTBLOCK **szLastAddrList, HOSTBLOCK **szCurrAddrList, BOOL bSaveGetLastAddr = FALSE)
{
    char szHostName[1024] = "";
    HOSTENT *HostEntry = NULL;

    if ( *szLastAddrList != NULL )
    {
        free( *szLastAddrList );
		*szLastAddrList = NULL;
    }

	*szLastAddrList = *szCurrAddrList;

	//
	// 1-2C1D5A ---- grab last address from the registry.
	// In case we are looping, if *szCurrAddrList is not empty, use that. 
	// For this specific fix, we will always grab from registry, but maybe
	// later this function will be used in a loop.
	//
	if ((bSaveGetLastAddr) && (NULL == *szCurrAddrList))
	{
		DWORD dwLastNetSize;
        DWORD dwType;
        LONG lResult = SymSaferRegQueryValueEx( hProductControlKey, _T("LastNetAddressToCompare"), NULL, &dwType, NULL, &dwLastNetSize );

        if ( ERROR_SUCCESS == lResult )
        {
            *szLastAddrList = (HOSTBLOCK *)malloc( dwLastNetSize + 4 );
            (*szLastAddrList)->dwSize = dwLastNetSize;
            lResult = SymSaferRegQueryValueEx( hProductControlKey, _T("LastNetAddressToCompare"), NULL, &dwType, (unsigned char *)(*szLastAddrList)->AddrArray, &dwLastNetSize );
        }
		/* else okay since it would be set to *szCurrAddrList --- handled correctly by code before this change --- even if NULL */
	}

    HOSTBLOCK *szAddrList = NULL;

    if ( fgethostname != NULL ) fgethostname( szHostName, sizeof( szHostName) );
    if ( fgethostbyname != NULL ) HostEntry = fgethostbyname( szHostName );

    int i = 0;
    int nAddrCount = 0;

    if ( HostEntry != NULL )
    {
        // we have a network card, is there a cable on it?

        while ( HostEntry->h_addr_list[i] != 0 )
        {
            nAddrCount += 1;
            i += 1;
        }

        static BYTE LoopBackAddr[4] = {127,0,0,0};
        static BYTE LoopBackAddr1[4] = {127,0,0,1};

        int nSize = nAddrCount * HostEntry->h_length;

        if ( nAddrCount == 1 && HostEntry->h_addrtype == 2 && // tcp-ip
             ( ! memcmp( HostEntry->h_addr_list[0], LoopBackAddr, sizeof(LoopBackAddr)) ||
               ! memcmp( HostEntry->h_addr_list[0], LoopBackAddr1, sizeof(LoopBackAddr1)) ) )
        {
            // loopback is not a real address! so dont make it one
            szAddrList = NULL;
        }
        else
        {
            szAddrList = (HOSTBLOCK *) malloc( nSize + 4 );
            szAddrList->dwSize = nSize;

            // smash all the address blocks together

            for ( i=0; i<nAddrCount; i++ )
            {
                memcpy( &szAddrList->AddrArray[i*HostEntry->h_length], HostEntry->h_addr_list[i], HostEntry->h_length );
            }
        }
    }
    else
    {
        szAddrList = NULL;
		*szCurrAddrList = szAddrList;
		return FALSE;
    }

    // post for return

    *szCurrAddrList = szAddrList;

	//
	// 1-2C1D5A --- save the addresses to compare next time.
	// Use different registry key LastNetAddressToCompare (instead of LastNetAddress) so we don't
	// collide any way with previous usage. This is safe.
	if ((bSaveGetLastAddr) && ( szAddrList ))
    {
        RegSetValueEx( hProductControlKey, _T("LastNetAddressToCompare"), NULL, REG_BINARY, (const unsigned char*)szAddrList->AddrArray, szAddrList->dwSize );
    }

	// CRT Fix for the defect ID: 1-B9EME. Return False if gethostbyname function returns NULL.
    // so that the system tries to find parent function every time this function is executed
    if ( szAddrList == NULL )
    {
        return TRUE;
    }

    // and see if the lists are the same

    if ( *szLastAddrList == NULL || szAddrList == NULL )
    {
        return FALSE;
    }

    if ( (*szLastAddrList)->dwSize != szAddrList->dwSize )
    {
        return FALSE;
    }

    if ( memcmp( (*szLastAddrList)->AddrArray, szAddrList->AddrArray, szAddrList->dwSize ) )
    {
        return FALSE;
    }

    return TRUE;
}


// 1-2C1D5A ---- Check if saved IP addresses are different (by XOR). If so return TRUE.
BOOL AreNetworkAddressesDifferent()
{
	if (g_bWinsockPresent)
	{
		HOSTBLOCK *szCurrAddrList = NULL;
		HOSTBLOCK *szLastAddrList = NULL;

		// TRUE flag indicates we are pulling from SAVED Network address
		BOOL bRet = GetNetworkAddressCheckUnchanged( &szLastAddrList, &szCurrAddrList, TRUE);

		if (szCurrAddrList)
			free(szCurrAddrList);

		if (szLastAddrList)
			free(szLastAddrList);

		// Reverse logic. If unchanged, then it is NOT different and vice-versa.
		return !bRet;
	}
 
	return FALSE;
}

// returns if on the internet - VERY IMPORTANT NOTE - all this does is tell you 
// whether a dialup connection is active or not - if you have a network card this always 
// returns true - nowadays that includes almost all laptops - pure dialup is getting to be
// pretty rare - it is only found on machines that never touch a network (some home users)

BOOL CheckOnDialUpNetwork(  )
{
    BOOL bOnNet = FALSE;
    BOOL bInternetPresent = FALSE;
    BOOL bInternetPresentEx = FALSE;

    char szHostName[1024] = "";
    char szConnectionName[1024] = "";
    DWORD dwFlags = 0;

    HOSTENT *HostEntry = NULL;

    if (fInternetGetConnectedStateEx)
    {
        bInternetPresentEx = fInternetGetConnectedStateEx( &dwFlags, szConnectionName, sizeof( szConnectionName ), 0 );
    }

    if (fInternetGetConnectedState)
    {
        bInternetPresent = fInternetGetConnectedState( &dwFlags, 0 );
    }

	// printf( "InternetGetConnectedStateEx on? %d %x %x - flags %x connection name %s\n", bInternetPresent, fInternetGetConnectedStateEx, fInternetGetConnectedState, dwFlags, szConnectionName );

    if ( fgethostname != NULL ) fgethostname( szHostName, sizeof( szHostName) );
    if ( fgethostbyname != NULL ) HostEntry = fgethostbyname( szHostName );

    int i = 0;
    int nAddrCount = 0;

    // CRT Fix for the defect ID: 1-B9EME. Return internet connected status if gethostbyname function returns NULL.	
    if ( HostEntry == NULL )
        return bInternetPresent | bInternetPresentEx;
    else  //  HostEntry != NULL 
    {
        // we have a network card, is there a cable on it?

        while ( HostEntry->h_addr_list[i] != 0 )
        {
			// LPBYTE pAddr = (LPBYTE) HostEntry->h_addr_list[i];

			// printf( "addr %d type %d is %d.%d.%d.%d\n", i, HostEntry->h_addrtype, pAddr[0], pAddr[1], pAddr[2], pAddr[3] );

            nAddrCount += 1;
            i += 1;
        }

        static BYTE LoopBackAddr[4] = {127,0,0,0};
        static BYTE LoopBackAddr1[4] = {127,0,0,1};

        // int nSize = nAddrCount * HostEntry->h_length;

        if ( nAddrCount == 1 && HostEntry->h_addrtype == 2 && // tcp-ip
             ( ! memcmp( HostEntry->h_addr_list[0], LoopBackAddr, sizeof(LoopBackAddr)) ||
               ! memcmp( HostEntry->h_addr_list[0], LoopBackAddr1, sizeof(LoopBackAddr1)) ) )
        {
            // loopback is not a real address! so dont make it one

            bOnNet = FALSE;
        }
        else
        {
            bOnNet = TRUE;
        }
    }

    static BOOL bLastOnNet = 47;
    static BOOL bLastInternetPresent = 47;
    static BOOL bLastInternetPresentEx = 47;

    if (bOnNet != bLastOnNet || 
        bInternetPresent != bLastInternetPresent || 
        bInternetPresentEx != bLastInternetPresentEx )
    {
        {
            char szLine[1024];

            sssnprintf( szLine, sizeof(szLine), "CheckOnDialUpNetwork changed %d %d %d - %s\n", bOnNet, bInternetPresent, bInternetPresentEx, szConnectionName ) ;
            OutputDebugLine( szLine, NULL, FALSE );

            i = 0;
            while ( HostEntry->h_addr_list[i] != 0 )
            {
                LPBYTE pAddr = (LPBYTE) HostEntry->h_addr_list[i];

                sssnprintf( szLine, sizeof(szLine), "addr %d type %d name %s is %d.%d.%d.%d\n", i, HostEntry->h_addrtype, HostEntry->h_name, pAddr[0], pAddr[1], pAddr[2], pAddr[3] );
                OutputDebugLine( szLine, NULL, FALSE );

                i += 1;
            }
        }

        bLastInternetPresentEx = bInternetPresentEx;
        bLastInternetPresent = bInternetPresent;
        bLastOnNet = bOnNet;
    }

    return bInternetPresent | bInternetPresentEx;
}


// returns if on the internet - VERY IMPORTANT NOTE - all this does is tell you 
// whether a dialup connection is active or not - if you have a network card this always 
// returns true - nowadays that includes almost all laptops - pure dialup is getting to be
// pretty rare - it is only found on machines that never touch a network (some home users)

BOOL DialUpNetworkPresent( BOOL bSilent )
{
    BOOL bRet = CheckOnDialUpNetwork( );

    if ( ! bRet && ! bSilent )
    {
        char szLine[1024];

        LoadString(g_ResLoader.GetResourceInstance(), IDS_ERROR_REQUIRED_NETWORK_NOT_PRESENT, szLine, MAX_BUFFER);
        OutputLine( szLine, NULL, FALSE );
    }

    return bRet;
}




// Win NT Roaming Code Begins


void WinNTService( void *param )
{
    DWORD dwWait = 0;
    HANDLE hCheckParentTimer = NULL;
	HANDLE hFindNearestParentTimer = NULL; // in case Winsock not present
	DWORD dwWaitHandles = 0;
	DWORD dwNearestParentTimerHandlePos = 0;

    LONG lResult = 0;

    HOSTBLOCK *szCurrAddrList = NULL;
    HOSTBLOCK *szLastAddrList = NULL;

    (void) param;

    // create the event object. The control handler function signals
    // this event when it receives the "stop" control code.
    //
    hServerStopEvent = CreateEvent(
                                  NULL,    // no security attributes
                                  TRUE,    // manual reset event
                                  FALSE,   // not-signalled
                                  NULL);   // no name

    if ( hServerStopEvent == NULL )
        goto cleanup;

    // set the nearest parent on startup - I will add checking for a network address change later
    // there are various tricky issues with that, including networks that aren't up when I start,
    // networks that are up, but don't work, and networks that change while I am running.

    FindNearestParent( NOT_INTERACTIVE );

    // Create a waitable timer.
    hCheckParentTimer = lpfnCreateWaitableTimer(NULL, FALSE, NULL);
    if ( !hCheckParentTimer )
    {
		LoadString(g_ResLoader.GetResourceInstance(), IDS_FUNCTION_FAILED, szBuffer, MAX_BUFFER);
		_tprintf(szBuffer, _T("CreateWaitableTimer"), GetLastError());
        goto cleanup;
    }

    LARGE_INTEGER liDueTime;

    liDueTime.QuadPart = g_dwCheckParentInterval;
    liDueTime.QuadPart *= -10000;           // convert to .1 microseconds

    if ( !lpfnSetWaitableTimer( hCheckParentTimer, &liDueTime, g_dwCheckParentInterval, NULL, NULL, 0) )
    {
		LoadString(g_ResLoader.GetResourceInstance(), IDS_FUNCTION_FAILED, szBuffer, MAX_BUFFER);		
        _tprintf(szBuffer, _T("SetWaitableTimer"), GetLastError());
        goto cleanup;
    }

	HANDLE hArray[3]; // max. of 3 handles will be used

	// these handles are always present
	hArray[0] = hCheckParentTimer;
	hArray[1] = hServerStopEvent;

	dwWaitHandles = 2; // one more may be added below
	
	// initialize to dummy position that's outside valid range
	dwNearestParentTimerHandlePos = (DWORD) -1;

	if (g_bWinsockPresent)
	{
	    DWORD dwLastNetSize;
	    DWORD dwType;

	    lResult = SymSaferRegQueryValueEx( hProductControlKey, _T("LastNetAddress"), NULL, &dwType, NULL, &dwLastNetSize );
	    if ( ERROR_SUCCESS == lResult )
	    {
			szCurrAddrList = (HOSTBLOCK *)malloc( dwLastNetSize + 4 );
			szCurrAddrList->dwSize = dwLastNetSize;
			lResult = SymSaferRegQueryValueEx( hProductControlKey, _T("LastNetAddress"), NULL, &dwType, (unsigned char *)szCurrAddrList->AddrArray, &dwLastNetSize );
		}

		GetNetworkAddressCheckUnchanged( &szLastAddrList, &szCurrAddrList );
	}
	else
	{
	    // Create a waitable timer.
	    hFindNearestParentTimer = lpfnCreateWaitableTimer(NULL, FALSE, NULL);
		if ( !hFindNearestParentTimer )
		{
			LoadString(g_ResLoader.GetResourceInstance(), IDS_FUNCTION_FAILED, szBuffer, MAX_BUFFER);
			_tprintf(szBuffer, _T("CreateWaitableTimer"), GetLastError());
			goto cleanup;
		}

	    liDueTime.QuadPart = g_dwFindNearestParentInterval;
	    liDueTime.QuadPart *= -10000;           // convert to .1 microseconds
		if ( !lpfnSetWaitableTimer( hFindNearestParentTimer, &liDueTime, g_dwFindNearestParentInterval, NULL, NULL, 0) )
		{
			LoadString(g_ResLoader.GetResourceInstance(), IDS_FUNCTION_FAILED, szBuffer, MAX_BUFFER);		
			_tprintf(szBuffer, _T("SetWaitableTimer"), GetLastError());
			goto cleanup;
		}
		// add handle to handle array
		hArray[dwWaitHandles] = hFindNearestParentTimer;
		dwNearestParentTimerHandlePos = dwWaitHandles;
	}

    while ( ! bDone )
    {
		dwWait = WaitForMultipleObjectsEx(
                                         dwWaitHandles,
                                         hArray,        // object-handle array
                                         FALSE,         // wait option
                                         3 * 60 * 1000, // time-out interval, 3 min.
                                         FALSE );

        if ( dwWait == WAIT_OBJECT_0 + 0 )
        {
            // Parent Timer check event
            // I woke up. Check to see if everything is still cool, and then go back to sleep

			// don't bother checking if we're not roaming
			if ((g_bRoamClient) || (g_bRoamESM) || (g_bRoamITA))
				CheckAndResetParent( NOT_INTERACTIVE );
        }
        else if ( dwWait == WAIT_OBJECT_0 + 1 )
        {
			// stop event
            bDone = TRUE;
        }
        else if ( (dwWait == WAIT_TIMEOUT) && g_bWinsockPresent )
        {
            // check if network address has changed - this is cheap so I do it often
            if ( !GetNetworkAddressCheckUnchanged( &szLastAddrList, &szCurrAddrList ) )
                FindNearestParent( NOT_INTERACTIVE );
        }
		else if (!g_bWinsockPresent && (dwWait == WAIT_OBJECT_0 + dwNearestParentTimerHandlePos))
		{
			FindNearestParent( NOT_INTERACTIVE );
		}
    }

    if ( ! KeepSCMHappyOnStop( 10000 ) ) goto cleanup;

    // save my last network address block

    if ( szCurrAddrList )
    {
        RegSetValueEx( hProductControlKey, _T("LastNetAddress"), NULL, REG_BINARY, (const unsigned char*)szCurrAddrList->AddrArray, szCurrAddrList->dwSize );
    }

    // I am leaving - clear my parent settings
	if( g_bRoamClient )
		MachineShutdown( );
	
cleanup:
    if ( hServerStopEvent )
        CloseHandle(hServerStopEvent);

	if(hCheckParentTimer)
	{
		lpfnCancelWaitableTimer( hCheckParentTimer );
		CloseHandle( hCheckParentTimer );
	}

	if(hFindNearestParentTimer)
	{
		lpfnCancelWaitableTimer(hFindNearestParentTimer);
		CloseHandle(hFindNearestParentTimer);
	}

	if(szCurrAddrList)
		free(szCurrAddrList);
	if(szLastAddrList)
		free(szLastAddrList);

}

void WinNTServiceStop( )
{
    bDone = TRUE;

    if ( hServerStopEvent )
        SetEvent(hServerStopEvent);
}



// Win NT Roaming Code Ends

//
//  FUNCTION: GetLastErrorText
//
//  PURPOSE: copies error message text to string
//
//  PARAMETERS:
//    lpszBuf - destination buffer
//    dwSize - size of buffer
//
//  RETURN VALUE:
//    destination buffer
//
//  COMMENTS:
//
LPTSTR GetLastErrorText( LPTSTR lpszBuf, DWORD dwSize )
{
    DWORD dwRet;
    LPTSTR lpszTemp = NULL;

    dwRet = FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |FORMAT_MESSAGE_ARGUMENT_ARRAY,
                           NULL,
                           GetLastError(),
						   MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
                           (LPTSTR)&lpszTemp,
                           0,
                           NULL );

    // supplied buffer is not long enough
    if ( !dwRet || ( (long)dwSize < (long)dwRet+14 ) )
        lpszBuf[0] = TEXT('\0');
    else
    {
        lpszTemp[lstrlen(lpszTemp)-2] = TEXT('\0');  //remove cr and newline character
        sssnprintf( lpszBuf, dwSize, TEXT("%s (0x%x)"), lpszTemp, GetLastError() );
    }

    if ( lpszTemp )
        LocalFree((HLOCAL) lpszTemp );

    return lpszBuf;
}

int NTSGetComputerName(char *pComputerName,wchar_t *pWchar)
{
    DWORD i;
    char computerName[NTS_MAX_COMPUTERNAME];
    memset(&computerName[0],0,NTS_MAX_COMPUTERNAME);
    i = NTS_MAX_COMPUTERNAME;
    GetComputerName(&computerName[0],&i);
    if ( pComputerName != NULL )
        memcpy(pComputerName,&computerName[0],NTS_MAX_COMPUTERNAME);
    if ( pWchar != NULL )
    {
        memset(pWchar,0,NTS_MAX_COMPUTERNAME * sizeof(wchar_t));
        mbstowcs(pWchar,&computerName[0],NTS_MAX_COMPUTERNAME);
    }
    return ++i;
}

DWORD LoadWinInet( void )
{
	TCHAR szPath[MAX_PATH];
	ZeroMemory(szPath, sizeof(szPath));
	::GetSystemDirectory( szPath, MAX_PATH );
	_tcscat( szPath, _T("\\wininet.dll"));
    g_hWinInet = LoadLibrary(szPath);
    if ( g_hWinInet )
    {
        fInternetGetConnectedState = (LPFNINTERNETGETCONNECTEDSTATE)GetProcAddress(g_hWinInet, "InternetGetConnectedState");
        fInternetGetConnectedStateEx = (LPFNINTERNETGETCONNECTEDSTATEEX)GetProcAddress(g_hWinInet, "InternetGetConnectedStateEx");

        if ( fInternetGetConnectedState == NULL )
		{
			// printf("import of InternetGetConnectedState failed - reason %x\n", GetLastError( ) );
			return ERROR_GENERAL;
		}

        if ( fInternetGetConnectedStateEx == NULL )
		{
			// printf("import of InternetGetConnectedStateEx failed - reason %x\n", GetLastError( ) );
			return ERROR_GENERAL;
		}
    }
	else
	{
		// printf("wininet LoadLibrary failed - reason %x\n", GetLastError( ) );
	}

    return ERROR_SUCCESS;
}

DWORD UnloadWinInet(void)
{
	if(g_hWinInet)
		FreeLibrary(g_hWinInet);
    return ERROR_SUCCESS;
}


DWORD LoadWinSock( void )
{
    DWORD ret = ERROR_GENERAL;

	TCHAR szPath[MAX_PATH];
	ZeroMemory(szPath, sizeof(szPath));
	::GetSystemDirectory( szPath, MAX_PATH );
	_tcscat( szPath, _T("\\ws2_32.dll"));
    g_hWinSock = LoadLibrary(szPath);
    if ( g_hWinSock )
    {
        fWSAStartup = (LPFN_WSASTARTUP)GetProcAddress(g_hWinSock, "WSAStartup");
        fWSACleanup = (LPFN_WSACLEANUP)GetProcAddress(g_hWinSock, "WSACleanup");
        fgethostname = (LPFN_GETHOSTNAME)GetProcAddress(g_hWinSock, "gethostname");
        fgethostbyname = (LPFN_GETHOSTBYNAME)GetProcAddress(g_hWinSock, "gethostbyname");

        if ( fWSAStartup == NULL ) goto err;
        if ( fWSACleanup == NULL ) goto err;
        if ( fgethostname == NULL ) goto err;
        if ( fgethostbyname == NULL ) goto err;

        ret = fWSAStartup( MAKEWORD( 2, 2 ), &WsaData );
    }

err:
    return ret;
}

DWORD UnloadWinSock(void)
{
	if(g_hWinSock)
	{
	    if ( fWSACleanup != NULL )
			fWSACleanup( );
		FreeLibrary(g_hWinSock);
	}
    return ERROR_SUCCESS;
}


//  FUNCTION: DeinitWindowsLibraries
//
//  PURPOSE: decrement reference count on any librariesload any libraries specific to Windows version
//
//  RETURN VALUE:  BOOL for success/failure
//    
//
static BOOL DeinitWindowsLibraries()
{
	BOOL bFreedKernel32 = TRUE;
	BOOL bFreedAdvapi32 = TRUE;

	if(g_hKernel32)
		bFreedKernel32 = FreeLibrary(g_hKernel32);
	if(g_hAdvapi32)
		bFreedAdvapi32 = FreeLibrary(g_hAdvapi32);

	return (bFreedAdvapi32 && bFreedKernel32);
}

//
//  FUNCTION: InitNTLibraries
//
//  PURPOSE: load libraries and save entrypoints to 
//			 NT specific functions.
//
//  RETURN VALUE:  BOOL for success in obtaining all entrypoints
//    
//  COMMENTS:  LoadLibrary/GetProcAddress permits linking one
//             app for all versions.
//
static BOOL InitNTLibraries()
{
	BOOL bKernel32LoadError = FALSE;
	BOOL bAdvapi32LoadError = FALSE;

	TCHAR szPath[MAX_PATH];
	ZeroMemory(szPath, sizeof(szPath));
	::GetSystemDirectory( szPath, MAX_PATH );
	_tcscat( szPath, _T("\\advapi32.dll"));
	g_hAdvapi32 = LoadLibrary(szPath);
	if(!g_hAdvapi32) 
	{
		LoadString(g_ResLoader.GetResourceInstance(), IDS_ERROR_LOADING_LIBRARY, szBuffer, MAX_BUFFER);
		sssnprintf(szBuffer2, sizeof(szBuffer2), szBuffer, _T("advapi32"));
		OutputLine(szBuffer2, NULL, FALSE);
		return FALSE;
	}

	ZeroMemory(szPath, sizeof(szPath));
	::GetSystemDirectory( szPath, MAX_PATH );
	_tcscat( szPath, _T("\\Kernel32.dll"));
    g_hKernel32 = LoadLibrary(szPath);
    if( g_hKernel32 == NULL ) 
	{
		FreeLibrary(g_hAdvapi32);
		LoadString(g_ResLoader.GetResourceInstance(), IDS_ERROR_LOADING_LIBRARY, szBuffer, MAX_BUFFER);
		sssnprintf(szBuffer2, sizeof(szBuffer2), szBuffer, _T("Kernel32"));
		OutputLine(szBuffer2, NULL, FALSE);
		return FALSE;
	}

#ifdef _UNICODE
	lpfnOpenSCManager = (LPFNOPENSCMANAGER)GetProcAddress(g_hAdvapi32, _T("OpenSCManagerW"));
	lpfnStartService = (LPFNSTARTSERVICE)GetProcAddress(g_hAdvapi32, _T("StartServiceW));
	lpfnCreateWaitableTimer = (LPFNCREATEWAITABLETIMER)GetProcAddress(g_hKernel32, _T("CreateWaitableTimerW"));
#else
	lpfnOpenSCManager = (LPFNOPENSCMANAGER)GetProcAddress(g_hAdvapi32, _T("OpenSCManagerA"));
	lpfnStartService = (LPFNSTARTSERVICE)GetProcAddress(g_hAdvapi32, _T("StartServiceA"));
	lpfnCreateWaitableTimer = (LPFNCREATEWAITABLETIMER)GetProcAddress(g_hKernel32, _T("CreateWaitableTimerA"));
#endif
	lpfnQueryServiceStatus = (LPFNQUERYSERVICESTATUS)GetProcAddress(g_hAdvapi32, _T("QueryServiceStatus"));
	lpfnControlService = (LPFNCONTROLSERVICE)GetProcAddress(g_hAdvapi32, _T("ControlService"));
	lpfnCloseServiceHandle = (LPFNCLOSESERVICEHANDLE)GetProcAddress(g_hAdvapi32, _T("CloseServiceHandle"));
	lpfnSetWaitableTimer = (LPFNSETWAITABLETIMER)GetProcAddress(g_hKernel32, _T("SetWaitableTimer"));
	lpfnCancelWaitableTimer = (LPFNCANCELWAITABLETIMER)GetProcAddress(g_hKernel32, _T("CancelWaitableTimer"));

	if(!lpfnOpenSCManager || 
	   !lpfnQueryServiceStatus || 
	   !lpfnStartService || 
	   !lpfnControlService || 
	   !lpfnCloseServiceHandle)
		bAdvapi32LoadError = TRUE;

	if(!lpfnCreateWaitableTimer ||
	   !lpfnSetWaitableTimer ||
	   !lpfnCancelWaitableTimer)
		bKernel32LoadError = TRUE;

	if(bAdvapi32LoadError || bKernel32LoadError)
	{
		FreeLibrary(g_hKernel32);
		FreeLibrary(g_hAdvapi32);

		if(bKernel32LoadError)
		{
			LoadString(g_ResLoader.GetResourceInstance(), IDS_ERROR_LIBRARY_FUNCTIONS, szBuffer, MAX_BUFFER);
			sssnprintf(szBuffer2, sizeof(szBuffer2), szBuffer, _T("Kernel32"));
			OutputLine(szBuffer2, NULL, FALSE);
		}

		if(bAdvapi32LoadError)
		{
			LoadString(g_ResLoader.GetResourceInstance(), IDS_ERROR_LIBRARY_FUNCTIONS, szBuffer, MAX_BUFFER);
			sssnprintf(szBuffer2, sizeof(szBuffer2), szBuffer, _T("advapi32"));
			OutputLine(szBuffer2, NULL, FALSE);
		}

		return FALSE;
	}

	return TRUE;
}

//
//  FUNCTION: CheckWindowsVersion
//
//  PURPOSE: determine Windows platform
//
//  RETURN VALUE:  BOOL for success in determining platform
//    
//  COMMENTS:  Sets global variables g_bWin2000plus
//
static BOOL CheckWindowsVersion()
{
    OSVERSIONINFO ver;
    ver.dwOSVersionInfoSize = sizeof( OSVERSIONINFO );

    GetVersionEx( &ver );

	if(ver.dwMajorVersion >= 5)
		g_bWin2000plus = TRUE;

	if(g_bWin2000plus)
		return TRUE;

	LoadString(g_ResLoader.GetResourceInstance(), IDS_ERROR_WINDOWS_VERSION, szBuffer, MAX_BUFFER);
	OutputLine(szBuffer, NULL, FALSE);
    return FALSE;
}


//
//  FUNCTION: InitWindowsLibraries
//
//  PURPOSE: load any libraries specific to Windows version
//
//  RETURN VALUE:  BOOL for success/failure
//    
//
static BOOL InitWindowsLibraries()
{
	BOOL bInitialized = FALSE;

	if(!CheckWindowsVersion())
		return FALSE;

	bInitialized = InitNTLibraries();

	return bInitialized;
}


static void LoadSettings( )
{
    g_dwCheckNewParentInterval = GetVal( hProductControlKey, szReg_Val_CheckForNewParentInterval, 30 ) * 1000;
    g_dwCheckParentInterval = GetVal( hProductControlKey, szReg_Val_CheckParentInterval, 120 ) * 60 * 1000;
    g_dwSampleCountForParentCheck = GetVal( hProductControlKey, szReg_Val_SampleCountForParentCheck, 7 );
	g_dwFindNearestParentInterval = GetVal( hProductControlKey, szReg_Val_FindNearestParentInterval, 60) * 60 * 1000;
	g_bRoamClient = GetVal( hProductControlKey, szReg_Val_RoamClient, 0 );
	g_bRoamESM = GetVal( hProductControlKey, szReg_Val_RoamESM, 0 );
	g_bRoamITA = GetVal( hProductControlKey, szReg_Val_RoamITA, 0 );

    return;
}

BOOL RoamingInit( BOOL bRunningAsService, BOOL bRunningInRtvscan, LPDWORD lpdwResult )
{
    (void)bRunningAsService;
    (void)bRunningInRtvscan;
    BOOL bRet = TRUE;

    *lpdwResult = 0;

	if( !InitWindowsLibraries() )
	{
        // this library is required
		*lpdwResult = (DWORD) -1;
        bRet = FALSE;
	}
    else
    {
        if( LoadWinSock() == ERROR_SUCCESS )
		{
            g_bWinsockPresent = TRUE;
		}

        if( LoadWinInet() == ERROR_SUCCESS )
		{
            g_bWinInetPresent = TRUE;
		}	

		LoadSettings( );
    }

    return bRet;
}

BOOL RoamingDeinit( LPDWORD lpdwResult )
{
    *lpdwResult = 0;

	//Commented the following line as a fix for 1-B9EME
	//UnloadWinInet( );
    UnloadWinSock( );
	DeinitWindowsLibraries( );

    char szLine[1024];
    sssnprintf( szLine, sizeof(szLine), "main all done\n" );
    OutputDebugLine( szLine, NULL, FALSE );

    return TRUE;
}

void __cdecl RoamingThread ( void *lpThreadData )
{
    // I am to roam - launch the appropriate code
    WinNTService( lpThreadData );
}

DWORD StopRoamingThread( void )
{
    WinNTServiceStop( );
	return 0;
}
