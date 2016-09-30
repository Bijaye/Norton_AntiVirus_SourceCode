//----------------------------------------------------------------------
// Copyright 1994 Symantec Corporation
//----------------------------------------------------------------------
//
// $Header:   S:/SYMKRNL/VCS/VXDLOAD.C_v   1.7   19 Mar 1997 21:46:44   BILL  $
//
// Description:
//   Definition of VxD Loader
//
//----------------------------------------------------------------------
// $Log:   S:/SYMKRNL/VCS/VXDLOAD.C_v  $ *
// 
//    Rev 1.7   19 Mar 1997 21:46:44   BILL
// Modified files to build properly under Alpha platform
// 
//    Rev 1.6   30 Aug 1996 15:07:32   BMCCORK
// Added check for failure on namereturnshortname call in VxdLoadImmediate()
// 
//    Rev 1.5   29 Aug 1996 18:09:26   DHERTEL
// Merged changed from Quake E:
// CreateFile() should use OPEN_EXISTING instead of NULL.
// 
//    Rev 1.4   15 Jul 1996 17:25:34   Basil
// Fixed the problem where LoadVxD() is called with full VxD path.  Function 
// used to return error (bRet=1) even though LoadVxDImmediate() succeeded.
// 
//    Rev 1.3   01 Jul 1996 12:04:22   ASUZDALT
// Fixed problem with NameParseFilename():
// It placed '\0' in place of ':' for mapped drive and 
// in place of '\' in UNC path after the server name, this caused problem with 
// creating path to VXD.  
// 
//    Rev 1.2   22 May 1996 12:31:22   SKURTZ
// In the VXDLOAD_SHARED_DIRECTORY area the path that comes from the SharedUsage
// registry key does not contain a filename at the end.  Therefore, NameParseFilename
// yealds undesirable results and VxD searches there would never be found.
//
//    Rev 1.1   24 Apr 1996 11:43:14   BMCCORK
// Added support for QCUST_NOVXD_EMSG to turn off reporting of VxD errors.
// This is for custom Quake customers who don't want to ship symkrnl.vxd or
// a custom equivalent.  Without this, a message box would appear whenever
// an attempt is made to use the VxD.
//
//    Rev 1.0   26 Jan 1996 20:22:28   JREARDON
// Initial revision.
//
//    Rev 1.4   26 Jul 1995 15:18:56   BILL
// Merged branch changes
//
//    Rev 1.3.2.1   07 Jul 1995 13:51:32   KEITH
// The VXD name needs to be Oem.
//
//    Rev 1.3.2.0   27 Jun 1995 18:50:26   RJACKSO
// Branch base for version QAK8
//
//    Rev 1.3   03 Mar 1995 12:55:14   BRAD
// Use short name when dynaloading VxDs.
//
//    Rev 1.2   26 Jan 1995 13:59:16   HENRI
// Added the ShowVxDError function and strings
//
//    Rev 1.1   18 Jan 1995 14:11:08   Basil
// Updated the registry key name for App Paths
//
//    Rev 1.0   15 Nov 1994 15:07:38   REESE
// Initial revision.
//----------------------------------------------------------------------

#ifndef _M_ALPHA // These functions will no longer be exported by SymKrnl(Alpha) as they are only for Windows 9x

#include "platform.h"
#include "xapi.h"
#include "vxdload.h"
#include "symkrnli.h"

#if defined(SYM_WIN32)	//[	This code only applies to Windows '95

#if defined(SYM_DEBUG)
    #include "quakever.h"               // QMODULE_SYMKRNL

    #ifndef SYM_OUTPUT_DEBUG
        #define SYM_OUTPUT_DEBUG(szOut) OutputDebugString(QMODULE_SYMKRNL": "szOut)
    #endif
#else
    #ifndef SYM_OUTPUT_DEBUG
        #define SYM_OUTPUT_DEBUG(szOut)
    #endif
#endif                                  // #if defined(SYM_DEBUG)


typedef struct
	{
	char * pDrive;
	char * pServer;
	char * pPath;
	char * pName;
	char * pExtension;
	} TypeNameParse;

static BOOL LoadVxDPath ( char * szStart, HANDLE hVxDLdr, W32IOCTLDATA * pData, TypeNameParse * pParse );
static BOOL LoadVxDImmediate ( HANDLE hVxDLdr, W32IOCTLDATA * pData, TypeNameParse * pParse );
static void NameParseFilename ( char * szFilename, TypeNameParse * parse );

static char szStandardVxDSuffix[] = "VXD";

UINT SYM_EXPORT WINAPI LoadVxD ( W32IOCTLDATA * pData, char const * lpszVxDFile, WORD dwFlags )
{
	char szVxD[SYM_MAX_PATH];
	TypeNameParse parseVxD;
    HANDLE hVxDLdr;

	SYM_ASSERT(lpszVxDFile != NULL);

    hVxDLdr=CreateFile("\\\\.\\VXDLDR", 0,0,0,OPEN_EXISTING,0,0);
    if(hVxDLdr == INVALID_HANDLE_VALUE)
        return(VXDLOAD_ERR_VXDLOADER);	// Indicate that an error occured while processing

	strcpy(szVxD,lpszVxDFile);
	NameParseFilename(szVxD,&parseVxD);

	if(parseVxD.pDrive != NULL ||	// If a path was specified
	 parseVxD.pServer != NULL ||
	 parseVxD.pPath != NULL)
		{
		if(LoadVxDImmediate(hVxDLdr,pData,&parseVxD))
			{
			CloseHandle(hVxDLdr);
			return(VXDLOAD_ERR_NONE);
			}
		}

	if(parseVxD.pExtension == NULL)	// If no suffix provided
		parseVxD.pExtension = szStandardVxDSuffix;	// Attach one

	if(dwFlags == VXDLOAD_ALL)
		dwFlags = VXDLOAD_MODULE_DIRECTORY |
		          VXDLOAD_CURRENT_DIRECTORY |
				  VXDLOAD_PERAPP_DIRECTORIES |
				  VXDLOAD_SYSTEM_DIRECTORY |
				  VXDLOAD_WINDOWS_DIRECTORY |
				  VXDLOAD_PATH_DIRECTORIES |
				  VXDLOAD_SHARED_DIRECTORY;

	if((dwFlags & VXDLOAD_MODULE_DIRECTORY) != 0)
		{
		char szModule[SYM_MAX_PATH];
		TypeNameParse parseModule;

		DWORD dwModuleLength = GetModuleFileName(NULL,szModule,sizeof(szModule));
		if(dwModuleLength == 0 || dwModuleLength >= sizeof(szModule))
			{
			CloseHandle(hVxDLdr);
			return(VXDLOAD_ERR_UNKNOWN);
			}
		NameParseFilename(szModule,&parseModule);

		parseModule.pName = parseVxD.pName;
		parseModule.pExtension = parseVxD.pExtension;

		if(LoadVxDImmediate(hVxDLdr,pData,&parseModule))
			{
			CloseHandle(hVxDLdr);
			return(VXDLOAD_ERR_NONE);
			}
		}

	if((dwFlags & VXDLOAD_CURRENT_DIRECTORY) != 0)
		{
		char szCurrent[SYM_MAX_PATH];

		DWORD dwCurrentLength = GetCurrentDirectory(sizeof(szCurrent),szCurrent);
		if(dwCurrentLength == 0 || dwCurrentLength >= sizeof(szCurrent))
			{
			CloseHandle(hVxDLdr);
			return(VXDLOAD_ERR_UNKNOWN);
			}

		if(LoadVxDPath(szCurrent,hVxDLdr,pData,&parseVxD))
			{
			CloseHandle(hVxDLdr);
			return(VXDLOAD_ERR_NONE);
			}
		}

	if((dwFlags & VXDLOAD_PERAPP_DIRECTORIES) != 0)
		{
		char szModule[SYM_MAX_PATH];
		TypeNameParse parseModule;
		char szRegistrationPath[256];
		HKEY key;

		DWORD dwModuleLength = GetModuleFileName(NULL,szModule,sizeof(szModule));
		if(dwModuleLength == 0 || dwModuleLength >= sizeof(szModule))
			{
			CloseHandle(hVxDLdr);
			return(VXDLOAD_ERR_UNKNOWN);
			}

		NameParseFilename(szModule,&parseModule);

		SYM_ASSERT(parseModule.pName != NULL);

		strcpy(szRegistrationPath,"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\App Paths\\");
		strcat(szRegistrationPath,parseModule.pName);
		if(parseModule.pExtension != NULL)
			strcat(strcat(szRegistrationPath,"."),parseModule.pExtension);

		if(RegOpenKeyEx(HKEY_LOCAL_MACHINE,szRegistrationPath,0,KEY_QUERY_VALUE,&key) == ERROR_SUCCESS)
			{
			char szPathEnvironment[256];
			DWORD dwPathLength = sizeof(szPathEnvironment);
			BOOL bRet = (RegQueryValueEx(key,"Path",NULL,NULL,szPathEnvironment,&dwPathLength) == ERROR_SUCCESS ? TRUE : FALSE);
			(void)RegCloseKey(key);
			if(bRet && dwPathLength > 0 && dwPathLength < sizeof(szPathEnvironment))
				if(LoadVxDPath(szPathEnvironment,hVxDLdr,pData,&parseVxD))
					{
					CloseHandle(hVxDLdr);
					return(VXDLOAD_ERR_NONE);
					}
			}
		}

	if((dwFlags & VXDLOAD_SHARED_DIRECTORY) != 0)
		{
		char szShared[SYM_MAX_PATH];
		char szRegistrationPath[128];
		DWORD dwSharedLength = sizeof(szShared);

		strcpy(szRegistrationPath,"SOFTWARE\\SYMANTEC\\SharedUsage\\");
		strcat(szRegistrationPath,parseVxD.pName);
		if(parseVxD.pExtension != NULL)
			strcat(strcat(szRegistrationPath,"."),parseVxD.pExtension);

		if(RegQueryValue(HKEY_LOCAL_MACHINE,szRegistrationPath,szShared,&dwSharedLength) == ERROR_SUCCESS)
			{
			if(dwSharedLength == 0 || dwSharedLength >= sizeof(szShared))
				{
				CloseHandle(hVxDLdr);
				return(VXDLOAD_ERR_UNKNOWN);
				}
                        if(LoadVxDPath(szShared,hVxDLdr,pData,&parseVxD))
				{
				CloseHandle(hVxDLdr);
				return(VXDLOAD_ERR_NONE);
				}
			}
		}

	if((dwFlags & VXDLOAD_SYSTEM_DIRECTORY) != 0)
		{
		char szSystem[SYM_MAX_PATH];

		DWORD dwSystemLength = GetSystemDirectory(szSystem,sizeof(szSystem));
		if(dwSystemLength == 0 || dwSystemLength >= sizeof(szSystem))
			{
			CloseHandle(hVxDLdr);
			return(VXDLOAD_ERR_UNKNOWN);
			}

		if(LoadVxDPath(szSystem,hVxDLdr,pData,&parseVxD))
			{
			CloseHandle(hVxDLdr);
			return(VXDLOAD_ERR_NONE);
			}
		}

	if((dwFlags & VXDLOAD_WINDOWS_DIRECTORY) != 0)
		{
		char szWindows[SYM_MAX_PATH];

		DWORD dwWindowsLength = GetWindowsDirectory(szWindows,sizeof(szWindows));
		if(dwWindowsLength == 0 || dwWindowsLength >= sizeof(szWindows))
			{
			CloseHandle(hVxDLdr);
			return(VXDLOAD_ERR_UNKNOWN);
			}

		if(LoadVxDPath(szWindows,hVxDLdr,pData,&parseVxD))
			{
			CloseHandle(hVxDLdr);
			return(VXDLOAD_ERR_NONE);
			}
		}

	if((dwFlags & VXDLOAD_PATH_DIRECTORIES) != 0)
		{
		char szPathEnvironment[256];
		DWORD dwPathLength = GetEnvironmentVariable("PATH",szPathEnvironment,sizeof(szPathEnvironment));
		if(dwPathLength > 0 && dwPathLength < sizeof(szPathEnvironment))
			if(LoadVxDPath(szPathEnvironment,hVxDLdr,pData,&parseVxD))
				{
				CloseHandle(hVxDLdr);
				return(VXDLOAD_ERR_NONE);
				}

		}

	CloseHandle(hVxDLdr);

	return(VXDLOAD_ERR_NOT_FOUND);
}

UINT SYM_EXPORT WINAPI FreeVxD ( W32IOCTLDATA * pData )
{
	DWORD dwBytesReturned;
	USHORT uErr;
	BOOL bRet;
	HANDLE hVxDLdr;

	SYM_ASSERT(pData != NULL);

        hVxDLdr=CreateFile("\\\\.\\VXDLDR", 0,0,0,OPEN_EXISTING,0,0);
	if(hVxDLdr == INVALID_HANDLE_VALUE)
		return(VXDLOAD_ERR_VXDLOADER);

	bRet = DeviceIoControl(hVxDLdr,            // VxD loader handle
			VXDLDR_APIFUNC_UNLOADDEVICE,    // VxDLdr unload VxD function
			(LPVOID)pData,         // input buffer
			sizeof(*pData),           // size of input buffer
			(LPVOID)&uErr,                 // output buffer
			sizeof(uErr),                // size of output buffer
			&dwBytesReturned,                         // bytes returned
			NULL);                          // synchronous API
	SYM_ASSERT(bRet);

	CloseHandle(hVxDLdr);

	if(uErr != 0)
		return(VXDLOAD_ERR_UNKNOWN);	// Couldn't unload the VxD

    return(VXDLOAD_ERR_NONE);
}

static BOOL LoadVxDPath ( char * szStart, HANDLE hVxDLdr, W32IOCTLDATA * pData, TypeNameParse * pParse )
{
	SYM_ASSERT(pParse != NULL);

	while(szStart != NULL)
		{
		char szPath[SYM_MAX_PATH];
		TypeNameParse parsePath;
		BOOL bDone = TRUE;

		// Terminate current path (if not done so already)
		char * szEnd;
		for(szEnd = szStart; *szEnd != '\0'; szEnd = CharNext(szEnd))
			if(*szEnd == ';')
				{
				bDone = FALSE;
				*szEnd = '\0';	// Safe to increment because semi-colon
				break;
				}

		SYM_ASSERT(szEnd-szStart < sizeof(szPath));
		strcpy(szPath,szStart);

		SYM_ASSERT(pParse->pName != NULL);
		strcat(strcat(szPath,"\\"),pParse->pName);

		if(pParse->pExtension != NULL)
			strcat(strcat(szPath,"."),pParse->pExtension);

		NameParseFilename(szPath,&parsePath);

		if(LoadVxDImmediate(hVxDLdr,pData,&parsePath))
			return(TRUE);

		// Move to next path
		if(!bDone)
			szStart = szEnd+1;
		else
			szStart = NULL;
		}
	return(FALSE);
}

static BOOL LoadVxDImmediate ( HANDLE hVxDLdr, W32IOCTLDATA * pData, TypeNameParse * pParse )
{
	char szFilename[SYM_MAX_PATH];
    char szShortname[SYM_MAX_PATH];
	DWORD dwBytesReturned;
	BOOL bRet;

    SYM_ASSERT(hVxDLdr != INVALID_HANDLE_VALUE);
	SYM_ASSERT(pData != NULL);
	SYM_ASSERT(pParse != NULL);

	szFilename[0] = EOS;
	if(pParse->pServer != NULL)
		strcat(strcat(strcat(szFilename,"\\\\"),pParse->pServer), "\\" );
	if(pParse->pDrive != NULL)
		strcat( strcat(szFilename,pParse->pDrive), "\\" );
	if(pParse->pPath != NULL)
		strcat(strcat(szFilename,pParse->pPath), "\\" );

	SYM_ASSERT(pParse->pName != NULL);
	strcat(szFilename,pParse->pName);
	if(pParse->pExtension != NULL)
		strcat(strcat(szFilename,"."),pParse->pExtension);

    szShortname[0] = EOS;
    if (NameReturnShortName(szFilename, szShortname) != NOERR)
        return FALSE;                   // Error getting the short name
        
                                        // This device level function
                                        // requires Oem chars.
    AnsiToOem ( szShortname, szShortname );

	bRet = DeviceIoControl(hVxDLdr,     // VxD loader handle
        VXDLDR_APIFUNC_LOADDEVICE,      // VxDLdr load VxD function
        (LPVOID)szShortname,            // VxD to load
        STRLEN(szShortname)+1,          // size of input buffer
        (LPVOID) pData,                 // output buffer
        sizeof(*pData),                 // size of output buffer
        &dwBytesReturned,               // bytes returned
        NULL);                          // synchronous API
//	SYM_ASSERT(bRet);
                                        // TODO re-enable when MS fixes it.

	return((pData->base.W32IO_ErrorCode == 0) ? TRUE : FALSE);
}
/********************************************************************
* This function should place:
*  "\\\\" - in front of server's name (parse->pServer) - no "\\" after;
*  ":"	  - after drive name (parse->pDrive) - no "\\" after;	
*  "\\"	  - neither in front of path no after path;
*  "\\"   - no in front of filer name, no '.' after
*			because all of them are replaced by '\0
*********************************************************************/	
static void NameParseFilename ( char * szFilename, TypeNameParse * parse )
{
	SYM_ASSERT(szFilename != NULL);
	SYM_ASSERT(parse != NULL);

	if(szFilename[0] == '\\' && szFilename[1] == '\\')
		{
		parse->pDrive = NULL;
		parse->pServer = (szFilename+=2);
		while(*szFilename != '\0' && *szFilename != '\\')
			szFilename = CharNext(szFilename);
		if(*szFilename != '\0')
			{
			*szFilename = '\0'; // Replace backslash with '\0'
			szFilename++;	// Safe to increment because it was backslash
			}
		}
	else
		{
		parse->pServer = NULL;
		if(szFilename[0] != '\0' && *CharNext(szFilename) == ':')
			{
			parse->pDrive = szFilename;
			szFilename = CharNext(szFilename);		// ':'
			szFilename = CharNext(szFilename);		// should be '\\'
			if( *szFilename != '\0' )
				{
				*szFilename = '\0';	// Safe to increment because it is colon
				szFilename++;	// First letter in path
				}
			}
		else
			parse->pDrive = NULL;
		}
	parse->pPath =
	 parse->pExtension = NULL;
	if(*szFilename == '\0')
		{
		parse->pName = NULL;
		return;
		}
	parse->pName = szFilename;
	while(*szFilename != '\0')
		{
		if(*szFilename == '\\')
			{
			if(parse->pPath == NULL)
				parse->pPath = parse->pName;
			parse->pName = szFilename;
			}
		else if(*szFilename == '.')
			parse->pExtension = szFilename;
		szFilename = CharNext(szFilename);
		}
	if(parse->pPath != NULL)
		*parse->pName++ = '\0';	// Terminate the path
	if(parse->pExtension != NULL)
		if(parse->pExtension < parse->pName)
			parse->pExtension = NULL;	// Extension can't presede the name
		else
			*parse->pExtension++ = '\0';	// Safe to inc. because period
	return;
}


void SYM_EXPORT WINAPI ShowVxDError( UINT uErr, LPSTR lpszVxDName, USHORT usErrorCode )
{
#ifndef QCUST_NOVXD_EMSG                // BEM - Introduced to let custom quake DLLs ignore
                                        //       lack of VxD

    extern HINSTANCE    hInst_SYMKRNL;
    auto   char         szMessage[255];
    auto   char         szBuf[512];
    auto   UINT         id;

    switch(uErr)
        {
        case VXDLDR_ERR_FILE_OPEN_ERROR:   id = IDS_VXDERR_COULD_NOT_OPEN;   break;
        case VXDLOAD_ERR_NOT_FOUND:        id = IDS_VXDERR_FILE_NOT_FOUND;   break;
        case VXDLOAD_ERR_VXDLOADER:        id = IDS_VXDERR_LOADER;           break;
        case VXDLDR_ERR_DEVICE_UNLOADABLE: id = IDS_VXDERR_COULD_NOT_UNLOAD; break;
        case VXDLOAD_ERR_UNKNOWN:
        default:                           id = IDS_VXDERR_UNKNOWN;          break;
        }

    LoadString( hInst_SYMKRNL, id, szMessage, sizeof(szMessage) );
    wsprintf( szBuf, szMessage, lpszVxDName, uErr, usErrorCode );
    MessageBox(0, szBuf, NULL, MB_OK);
#else
    SYM_OUTPUT_DEBUG("ShowVxDError called but QCUST_NOVXD_EMSG was defined.\n");
#endif                                  // #ifndef QCUST_NOVXD_EMSG
}

#endif //]

#endif // _M_ALPHA
// End of file
