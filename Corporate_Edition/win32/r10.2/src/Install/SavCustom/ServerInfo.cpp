// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
#include "stdafx.h"
#include "ServerInfo.h"
#include "ILdvpdist.h"
#include "FileParser.h"
#include "Bldnum.h"
#include "cregent.h"


DWORD AddServerInfo(CTargetInfo *pInfo, CString sPath)
{
	BOOL bLocalServer = !GetComputerName().CompareNoCase(pInfo->m_sServer);

	if (sPath.IsEmpty() && bLocalServer && pInfo->m_dwFlags & PERFORM_UNINSTALL)
		sPath = pInfo->m_sLocalRep;
	else if (sPath.IsEmpty())
		sPath = GetModulePath();

	CString sCmdFile(sPath + BSLASH SERVER_FILE);
	long dwFlags = DEST_REMOTE;
	dwFlags |= pInfo->m_dwFlags;

	if (bLocalServer)
	{
		dwFlags &= ~(PARTIAL_SCRIPT | DEST_REMOTE);
		dwFlags |= ALL_LOCAL;

		if (dwFlags & CLIENT_FILES)
		{
			CString sCmdPath(sPath);
			if (!(dwFlags & PERFORM_UNINSTALL))
				sCmdPath = sPath + CLIENT32_DIR;
 
			AddComputer(pInfo->m_sServer,OSVER_MASK(dwFlags));
			AddProduct(COMPON_TYPE,sCmdPath + BSLASH CLIENT32_FILE,sCmdPath,"",dwFlags);
			AddProductVarString("%DEST_PATH%",pInfo->m_sLocalRep);

			if (IsNT() == 3)
				AddProductVarValue("NT3_ONLY",TRUE);

			// Mail Client Extensions removed
			AddProductVarValue("EXCHANGE_CLIENT",FALSE);
			AddProductVarValue("NOTES_CLIENT",FALSE);
			AddProductVarValue("CCMAIL_CLIENT",FALSE);

			// Remove VP5 Version
			RemoveOldVersion();
		}
		
		if (dwFlags & ADMIN_FILES)
		{
			CString sCommFile = sPath + BSLASH CONSOLE_FILE;
			long dwTmp = dwFlags;

			if (dwFlags & PERFORM_INSTALL && RegKeyExists(HKLM,"~\\Console",NULL))
			{
				dwTmp &= ~PERFORM_INSTALL;
				dwTmp |= PERFORM_REINSTALL;
			}

			AddComputer(pInfo->m_sServer,OSVER_MASK(dwTmp));
			AddProduct(ADMIN_TYPE,sCommFile,sPath,"",dwTmp);
			AddProductVarString("%DEST_PATH%",pInfo->m_sLocalRep);

			AddProductVarValue(AMS_TOKEN,dwTmp & AMS_ADMIN_FILES);
			AddProductVarValue(ADMIN_VERSION,PRODUCTVERSION | (BUILDNUMBER << 16));
		}

		AddComputer(pInfo->m_sServer,OSVER_MASK(dwFlags));
		AddProduct(SERVER_TYPE,sCmdFile,sPath,"",dwFlags);
		AddProductVarString("%DEST_PATH%",pInfo->m_sLocalRep);
		if (!UpdatePDS(!(dwFlags & PERFORM_UNINSTALL)))
			AddProductVarValue("UPDATE_PDS",FALSE);
	}
	else
	{
		AddComputer(pInfo->m_sServer,OSVER_MASK(dwFlags));
		AddProduct(SERVER_TYPE,sCmdFile,sPath,"",dwFlags);
		if (dwFlags & (PERFORM_INSTALL | PERFORM_REINSTALL))
			AddProductVarString("%DEST_PATH%",pInfo->m_sPath);
		else
			AddProductVarString("%DEST_PATH%",pInfo->m_sLocalRep);

		// Exclude server snapins on initial push
		if (dwFlags & VAL_WINNT_ONLY)
		{
			AddProductVarValue("EXCHANGE_SERVER",FALSE);
			AddProductVarValue("NOTES_SERVER",FALSE);
			if (dwFlags & PERFORM_UNINSTALL)
				AddProductVarString(PERFORM_SECTION,"NetWare");
		}
	}

	if (dwFlags & VAL_WINNT_ONLY)
	{
		AddProductVarString("%REGROOT%",REGHEADER);
		AddProductVarString("%HOME%",pInfo->m_sLocalRep);
		AddProductVarString("%FILE_SERVER%",pInfo->m_sServer);
		AddProductVarString("%AUTO_DEF%","");
		AddProductVarString("%SERV_VER%","");
	}
	else
	{
		AddProductVarString("%REGROOT%","VirusProtect6");
		AddProductVarString(PERFORM_SECTION,"NetWare");
		AddProductVarString("%HOME%","{internal}");
		AddProductVarString("%FILE_SERVER%","{internal}");

		// Defines for NetWare Scripts
		if (dwFlags & AUTO_START)
			AddProductVarString("%AUTO_DEF%","AUTOLOAD");
		else
			AddProductVarString("%AUTO_DEF%","");
		if (dwFlags & NDS_TYPE)
			AddProductVarString("%SERV_VER%","NDS");
		else
			AddProductVarString("%SERV_VER%","BINDERY");

	}
	// Profile Information
	if (dwFlags & NDS_TYPE)
	{
		AddProductVarString("%OU%",QUOTE + pInfo->m_sOU + QUOTE);
		AddProductVarString("%PASSWORD%",pInfo->m_sPassword);
		AddProductVarString("%USERNAME%",QUOTE + pInfo->m_sUserName + QUOTE);

	}
	else
	{
		AddProductVarString("%OU%","");
		AddProductVarString("%PASSWORD%","");
		AddProductVarString("%USERNAME%","");
	}

	AddProductVarString("%UNCPATH%",pInfo->m_sPath);
	AddProductVarString("%PARENT%",pInfo->m_sParent);
	AddProductVarString("%DOMAIN%",QUOTE + pInfo->m_sDomain + QUOTE);
	AddProductVarString("%PASSWD%",pInfo->m_sDomPasswd);
	AddProductVarString(LANG_ID,CLangResource::GetLangAbbrev());
	if (dwFlags & AUTO_START)
		AddProductVarValue(STARTUP_TYPE,SERVICE_AUTO_START);
	else
		AddProductVarValue(STARTUP_TYPE,SERVICE_DEMAND_START);

	// Optionally install CLIENT SUPPORT FILES
	if (dwFlags & CLIENT_FILES)
		AddProductVarValue(CLIENT_TOKEN,TRUE);

	// Optionally install ADMIN
	if (dwFlags & ADMIN_FILES)
		AddProductVarValue(ADMIN_TOKEN,TRUE);

	// Optionally install ADMIN cdimage files
	if (dwFlags & ADMIN_CDIMAGE)
		AddProductVarValue(ADMIN_SETUP,TRUE);

	// Optionally install AMS2
	AddProductVarValue(AMS_TOKEN,dwFlags & AMS_SERVER_FILES);

	// Special variables for Update and Uninstall
	AddProductVarString(REMOTE_PATH,sPath);

#ifdef DONGLE
	// Dongle Service
	AddProductVarValue(DONGLE_TOKEN,TRUE);
#endif

	// On UNINSTALL remove AMS and COMMON files
	if (dwFlags & PERFORM_UNINSTALL && bLocalServer)
	{
		if (dwFlags & AMS_SERVER_FILES)
		{
			AddComputer(pInfo->m_sServer,OSVER_MASK(dwFlags));
			AddProduct(COMPON_TYPE,sPath + BSLASH + LEGACY_FILE,sPath,"",dwFlags);
			AddProductVarString("%DEST_PATH%",pInfo->m_sLocalRep);
			AddProductVarString(PERFORM_SECTION,"Ams2");
			AddProductVarValue(AMS_TOKEN,TRUE);
		}

		AddComputer(pInfo->m_sServer,OSVER_MASK(dwFlags));
		AddProduct(CLEANUP_TYPE,sPath + BSLASH + LEGACY_FILE,sPath,"",dwFlags);
		AddProductVarString("%DEST_PATH%",pInfo->m_sLocalRep);
		AddProductVarString(PERFORM_SECTION,"Common");

		if (IsNT() == 3)
			AddProductVarValue("NT3_ONLY",TRUE);

		CString sTempPath;
		GetTempPath(MBUFFER,sTempPath.GetBuffer(MBUFFER));
		sTempPath.ReleaseBuffer();
		if (sTempPath.Right(1) == BSLASH)
			sTempPath = sTempPath.Left(sTempPath.GetLength()-1);
		AddProductVarString("%TEMP_PATH%",sTempPath);
	}
	return 0;
}

HRESULT StartComputerUpdate(BOOL bSilent, LPCSTR sLangID, DWORD dwFlags)
{
	HRESULT hRtn;
	long dwAction = 0;
	if (bSilent)
		dwAction = RUN_SILENTLY;
	dwAction |= UPDATE_NOW | dwFlags;
	hRtn = UpdateComputers(dwAction, sLangID);
	ReleaseInterfaces();
	return hRtn;
}

BOOL UpdatePDS(BOOL bOnInstall)
{
	TCHAR sKey[MAX_PATH];
	CString sTemp("SOFTWARE\\Intel\\LANDesk\\AMS2\\Installed\\Files");
	CString sName(GetSystemPath() + "\\CBA\\PDS.EXE");
	DWORD size = MAX_PATH, keySize;
	BOOL bKeyChanged = TRUE;

	memset(sKey,0,MAX_PATH);
	if (!RegKeyGetValue(HKLM,sTemp,sName,(LPBYTE)sKey,MAX_PATH,NULL,NULL,&size))
	{
		if (bOnInstall)
		{
			keySize = AddStrToList(sKey,"LDVP_CBA");
			if (keySize == size)
				bKeyChanged = FALSE;
			else
				RegKeySetValue(HKLM,sTemp,sName,REG_MULTI_SZ,(LPBYTE)sKey,keySize,NULL);
		}
		else
		{
			keySize = DelStrFromList(sKey,size,"LDVP_CBA");
			if (keySize > 0)
			{
				bKeyChanged = FALSE;
				RegKeySetValue(HKLM,sTemp,sName,REG_MULTI_SZ,(LPBYTE)sKey,keySize,NULL);
			}
			else
				RegKeyDeleteValue(HKLM,sTemp,sName,NULL);
		}
	}
	return bKeyChanged;
}


DWORD GetOldVersion()
{
	DWORD dwVersion = 0;
	CString cstrLocalServer;
	
	cstrLocalServer = VP6_ADDRESSCACHE_REG_KEY; 
	cstrLocalServer += "\\";
	cstrLocalServer += GetComputerName();

	if (RegKeyGetValue(HKLM, cstrLocalServer, "ProductVersion",(LPBYTE)&dwVersion,4,NULL))
		RegKeyGetValue(HKLM,VP5_REGHEADER,"ProductVersion",(LPBYTE)&dwVersion,4,NULL);

	return ((dwVersion & 0xffff)/100);
}


void RemoveOldVersion()
{
	DWORD dwVersion = GetOldVersion();

	if (dwVersion == 5)
	{
		CString sPath;
		DWORD dwError = RegKeyGetValue(HKLM,VP5_REGHEADER,"Home Directory",(LPBYTE)sPath.GetBuffer(MBUFFER),MBUFFER,NULL);
		sPath.ReleaseBuffer();

		if (!dwError && FileExists(sPath + "\\vpremove.exe"))
		{
			AddProductVarValue("REMOVE_VP5",TRUE);
			AddProductVarString("%VP5_PATH%",sPath);
		}
	}
}

DWORD ServerMajorRelease(CString sServer)
{
	DWORD dwVersion = 0;

	// Set the location to the NAV Server
	CString cstrNavServer = VP6_ADDRESSCACHE_REG_KEY;
	cstrNavServer += "\\";

	LPTSTR lpszNavServer = cstrNavServer.GetBuffer(MBUFFER);
	cstrNavServer.ReleaseBuffer();
	_tcscat(lpszNavServer, sServer);
	
	if (!RegKeyGetValue(HKLM,lpszNavServer,"ProductVersion",(LPBYTE)&dwVersion,4,NULL))
		return (dwVersion & 0xffff)/100;
	return 0;
}

DWORD GetServerInfo(CTargetInfo* pInfo,CString sServer,DWORD dwAction,LPTM pTrans)
{
	DWORD dwTemp,dwFlags,dwGood,dwVersion,dwRtnVal;
	CString sTemp, sRegRoot("~");
	LPSTR pBuf, pServer = (LPSTR)(LPCSTR)sServer;
	char sBuf[MBUFFER];
	BOOL bLocalServer = !GetComputerName().CompareNoCase(sServer);

	if (ServerMajorRelease(sServer) == 5 && bLocalServer)
		sRegRoot = VP5_REGHEADER;

	// Get flags from PONG data
	dwFlags = 0;
	dwTemp = 0;
	
	// Set the location to the NAV Server
	CString cstrNavServer = VP6_ADDRESSCACHE_REG_KEY;
	cstrNavServer += "\\";

	LPTSTR lpszNavServer = cstrNavServer.GetBuffer(MBUFFER);
	cstrNavServer.ReleaseBuffer();
	_tcscat(lpszNavServer, sServer);
	
	if (dwRtnVal = RegKeyGetValue(HKLM, lpszNavServer,"Good",(LPBYTE)&dwGood,4,NULL))
		return dwRtnVal;

	// allow local server's "Good" flag to be reset, because server could be down
	if (!dwGood && !bLocalServer)
		return ERROR_INVALID_DATA;

	if (dwRtnVal = RegKeyGetValue(HKLM, lpszNavServer,"Flags",(LPBYTE)&dwTemp,4,NULL))
		return dwRtnVal;

	if (dwTemp & PF_SUPPORT_CLIENTS)
		dwFlags |= CLIENT_FILES;

	if (dwTemp & PF_USING_AMS)
		dwFlags |= AMS_SERVER_FILES;

	if (dwTemp & PF_PLATFORM_WINNT)
	{
		sTemp.Empty();
		DWORD tmp = RegKeyGetValue(HKLM,sRegRoot + "\\Console","LastRanFrom",(BYTE*)sTemp.GetBuffer(MAX_PATH),MAX_PATH,sServer,pTrans);
		sTemp.ReleaseBuffer();

		if (bLocalServer && RegKeyExists(HKLM,sRegRoot + "\\Console",NULL))
			tmp = ERROR_BLOCK_NOT_READ;

		if (!sTemp.IsEmpty() || tmp == ERROR_BLOCK_NOT_READ)
			dwFlags |= ADMIN_FILES;

		if (dwTemp & PF_USING_AMS && (!sTemp.IsEmpty() || tmp == ERROR_BLOCK_NOT_READ))
			dwFlags |= AMS_ADMIN_FILES;

		dwFlags |= VAL_WINNT_ONLY;
	}
	else if (dwTemp & PF_NETWARE_3)
		dwFlags |= VAL_NW3_ONLY;
	else if (dwTemp & PF_NETWARE_4)
		dwFlags |= VAL_NW4_ONLY;
	else if (dwTemp & PF_NETWARE_5)
		dwFlags |= VAL_NW5_ONLY;

	// Allow local server to reinstall directly
	if (dwAction & PERFORM_UPDATE && bLocalServer)
	{
		dwAction &= ~PERFORM_UPDATE;
		if (ServerMajorRelease(sServer) != 5)
			dwAction |= PERFORM_REINSTALL;
		else
			dwAction |= PERFORM_INSTALL;
	}

	// Fill in target structure
	pInfo->m_sServer = sServer;
	pInfo->m_dwFlags = dwFlags | dwAction;
	pInfo->m_dwDisplayType = 0;
	pInfo->m_dwError = 0;

	// Retrieve DOMAIN / CONSOLE PASSWORD
	if (dwRtnVal = GetDomainInfo(pInfo->m_sServer,pInfo->m_sDomain,pInfo->m_sDomPasswd,pTrans))
		return dwRtnVal;

	// Retrieve PARENT
	pBuf = pInfo->m_sParent.GetBuffer(MBUFFER);

	CString cstrParent;
	cstrParent = VP6_CURRVER_REG_KEY;

	if (!dwGood || (pInfo->m_dwError = RegKeyGetValue(HKLM, cstrParent,"Parent",(LPBYTE)pBuf,MBUFFER,NULL)))
		pInfo->m_dwError = RegKeyGetValue(HKLM,sRegRoot,"Parent",(LPBYTE)pBuf,MBUFFER,pServer,pTrans);
	pInfo->m_sParent.ReleaseBuffer();
	if (pInfo->m_dwError)
		return pInfo->m_dwError;

	// Retrieve HOME DIRECTORY
	pInfo->m_dwError = RegKeyGetValue(HKLM,sRegRoot,"Home Directory",(LPBYTE)pInfo->m_sLocalRep.GetBuffer(MBUFFER),MBUFFER,pServer,pTrans);
	pInfo->m_sLocalRep.ReleaseBuffer();
	if (pInfo->m_dwError)
		return pInfo->m_dwError;

	// Retrieve UNCPATH
	pInfo->m_dwError = RegKeyGetValue(HKLM,sRegRoot,"UNCPath",(LPBYTE)pInfo->m_sPath.GetBuffer(MBUFFER),MBUFFER,pServer,pTrans);
	pInfo->m_sPath.ReleaseBuffer();
	if (pInfo->m_dwError)
		return pInfo->m_dwError;

	// Modify Version 5 Server Licenses and Home Directories
	if (ServerMajorRelease(sServer) == 5)
	{
		// Rebuild HOME DIRECTORY
		CString sNewPath(pInfo->m_sLocalRep);
		int pos = -1;
		sNewPath.MakeLower();
		if (((pos = sNewPath.Find("intel")) != -1) ||
			((pos = sNewPath.Find("landesk")) != -1) ||
			((pos = sNewPath.Find("vp5")) != -1))
		{
			pInfo->m_sLocalRep = sNewPath.Left(pos) + SYMANTEC_DIRECTORY;
		}
		else
		{
			pInfo->m_sLocalRep = GetDirPath(pInfo->m_sLocalRep) + BSLASH SYMANTEC_DIRECTORY;
		}

		// Rebuild UNCPATH
		sNewPath = pInfo->m_sPath;
		pos = -1;
		sNewPath.MakeLower();
		if (((pos = sNewPath.Find("intel")) != -1) ||
			((pos = sNewPath.Find("landesk")) != -1) ||
			((pos = sNewPath.Find("vp6")) != -1))
		{
			pInfo->m_sPath = sNewPath.Left(pos) + SYMANTEC_DIRECTORY;
		}
		else
		{
			pInfo->m_sPath = GetDirPath(pInfo->m_sPath) + BSLASH SYMANTEC_DIRECTORY;
		}
	}

	if (pInfo->m_dwFlags & (VAL_NW4_ONLY | VAL_NW5_ONLY))
	{
		// Retrieve PROFILE USERNAME
		pInfo->m_dwError = RegKeyGetValue(HKLM,sRegRoot,"ProfileUserName",(LPBYTE)pInfo->m_sUserName.GetBuffer(MBUFFER),MBUFFER,pServer,pTrans);
		pInfo->m_sUserName.ReleaseBuffer();
		if (pInfo->m_dwError && pInfo->m_dwError != ERROR_BLOCK_NOT_READ)
			return pInfo->m_dwError;

		// Retrieve NDS CONTAINER
		pInfo->m_dwError = RegKeyGetValue(HKLM,sRegRoot,"Container",(LPBYTE)pInfo->m_sOU.GetBuffer(MBUFFER),MBUFFER,pServer,pTrans);
		pInfo->m_sOU.ReleaseBuffer();
		if (pInfo->m_dwError && pInfo->m_dwError != ERROR_BLOCK_NOT_READ)
			return pInfo->m_dwError;

		// Determine if NDS or BINDERY
		// Set OS = VAL_NW4_ONLY for NDS (could be NW4 or NW5)
		// Set OS = VAL_NW3_ONLY for BINDERY (could be NW3, NW4 or NW5)
		pInfo->m_dwFlags &= ~(VAL_NW4_ONLY | VAL_NW5_ONLY);
		if (!pInfo->m_sOU.IsEmpty())
		{
			pInfo->m_dwFlags |= NDS_TYPE;
			pInfo->m_dwFlags |= VAL_NW4_ONLY;
		}
		else
			pInfo->m_dwFlags |= VAL_NW3_ONLY;

		// Retrieve PROFILE PASSWORD
		pBuf = pInfo->m_sPassword.GetBuffer(MBUFFER);
		pInfo->m_dwError = RegKeyGetValue(HKLM,sRegRoot,"ProfilePassword",(LPBYTE)pBuf,MBUFFER,pServer,pTrans);
		pInfo->m_sPassword.ReleaseBuffer();
		if (!pInfo->m_sPassword.IsEmpty())
			pInfo->m_sPassword = pInfo->m_sPassword.Mid(7);
		if (pInfo->m_dwError && pInfo->m_dwError != ERROR_BLOCK_NOT_READ)
			return pInfo->m_dwError;
	}

	// Retrieve STARTUP_TYPE / ADMIN_CDIMAGE
	if (dwVersion = ServerMajorRelease(sServer))
	{
		GetTempPath(MBUFFER,sTemp.GetBuffer(MBUFFER));
		sTemp.ReleaseBuffer();
		if (sTemp.Right(1) != BSLASH)
			sTemp += BSLASH;
		
		BOOL bCopied = FALSE;
		if (dwVersion == 5)
		{
			CString sFile(sTemp + "path.ini");
			if (!pTrans->CopyFile(pServer, "$\\path.ini",sFile,COPY_READONLY | COPYFILE_ALWAYS,FALSE))
			{
				if (GetPrivateProfileString("Console_Install_Options","CopyAdmin","",sBuf,MBUFFER,sFile) && atoi(sBuf) == 2)
					pInfo->m_dwFlags |= ADMIN_CDIMAGE;
			}
			sFile =sTemp + "vpregdb.def";
			if (bCopied = !pTrans->CopyFile(pServer, "$\\vpregdb.def",sFile,COPY_READONLY | COPYFILE_ALWAYS,FALSE))
			{
				if (pInfo->m_dwFlags & VAL_WINNT_ONLY)
				{
					if (GetPrivateProfileString("RtvScan_Service_Inst","StartType","2",sBuf,MBUFFER,sFile) && atoi(sBuf) == 2)
						pInfo->m_dwFlags |= AUTO_START;
				}
				else
				{
					CFileSection fRegFile(sFile);
					if (fRegFile.FindFirstLine(sTemp,"AUTOEXEC.NCF") && !sTemp.CompareNoCase("vpstart"))
						pInfo->m_dwFlags |= AUTO_START;
				}
			}
		}
		else
		{
			CString sFile(sTemp + "vpdata.var");
			if (bCopied = !pTrans->CopyFile(pServer, "$\\vpdata.var",sFile,COPY_READONLY | COPYFILE_ALWAYS,FALSE))
			{
				CFileSection fRegFile(sFile);
				if (pInfo->m_dwFlags & VAL_WINNT_ONLY)
				{
					if (GetPrivateProfileString("VARIABLES","STARTUP_TYPE","",sBuf,MBUFFER,sFile) && atoi(sBuf) == SERVICE_AUTO_START)
						pInfo->m_dwFlags |= AUTO_START;
				}
				else
				{
					sTemp = "AUTOLOAD";
					if (fRegFile.FindNextMatchLine(sTemp,"DEFINES"))
						pInfo->m_dwFlags |= AUTO_START;
				}
				if (GetPrivateProfileString("VARIABLES","ADMIN_SETUP","",sBuf,MBUFFER,sFile) && atoi(sBuf))
					pInfo->m_dwFlags |= ADMIN_CDIMAGE;
			}
		}
		if (!bCopied)
		{
			pInfo->m_dwFlags |= AUTO_START;
		}
	}
	return dwRtnVal;
}


DWORD GetDomainInfo(CString sServer, CString& sDomain, CString& sDomPasswd, LPTM pTrans)
{
	DWORD dwGood = 0;
	DWORD dwRtnVal = 0;
	CString sRegRoot("~");
	LPSTR pBuf;
	CString cstrNavServer;
	LPTSTR lpszNavServer;

	if ( TRUE == ClientIsServer() ) 
	{
		// Set the location to the NAV Server
		cstrNavServer = VP6_ADDRESSCACHE_REG_KEY;
		cstrNavServer += "\\";
	}
	else
	{
		cstrNavServer = VP6_ADMIN_SECURITY_KEY;
	}

	lpszNavServer = cstrNavServer.GetBuffer(MBUFFER);
	cstrNavServer.ReleaseBuffer();

	if ( TRUE == ClientIsServer() )
		_tcscat(lpszNavServer, sServer);
	
	if (ServerMajorRelease(sServer) == 5 && !GetComputerName().CompareNoCase(sServer))
		sRegRoot = VP5_REGHEADER;

	if ( TRUE == ClientIsServer () )
	{
		// Retrieve CONSOLE PASSWORD
		pBuf = sDomPasswd.GetBuffer(MBUFFER);

		// Try to get the password directly from the server first because the
		// ConsolePassword in the AddressCache may not be valid
		if (RegKeyGetValue(HKLM,sRegRoot,"ConsolePassword",(LPBYTE)pBuf,MBUFFER,(LPSTR)(LPCSTR)sServer,pTrans))
		{
			dwRtnVal = RegKeyGetValue(HKLM, lpszNavServer,"ConsolePassword",(LPBYTE)pBuf,MBUFFER,NULL);
		}

		sDomPasswd.ReleaseBuffer();
	}
	else
	{
		// Client Uninstall Password
		pBuf = sDomPasswd.GetBuffer(MBUFFER);
		dwRtnVal = RegKeyGetValue(HKLM, lpszNavServer, "VPUninstallPassword", (LPBYTE)pBuf, MBUFFER, NULL);
		sDomPasswd.ReleaseBuffer();
	}

	return dwRtnVal;
}

DWORD GetDomainNameAndPassword(CString sServer, CString& sDomain, CString& sDomPasswd, LPTM pTrans)
{
	DWORD dwGood, dwRtnVal;
	CString sRegRoot("~");
	LPSTR pBuf;

	if (ServerMajorRelease(sServer) == 5 && !GetComputerName().CompareNoCase(sServer))
		sRegRoot = VP5_REGHEADER;
	
	if (dwRtnVal = RegKeyGetValue(HKLM,"~\\AddressCache\\" + sServer,"Good",(LPBYTE)&dwGood,4,NULL))
		return dwRtnVal;

	// Retrieve DOMAIN NAME
	pBuf = sDomain.GetBuffer(MBUFFER);
	if (!dwGood || (dwRtnVal = RegKeyGetValue(HKLM,"~\\AddressCache\\" + sServer,"Domain",(LPBYTE)pBuf,MBUFFER,NULL)))
		dwRtnVal = RegKeyGetValue(HKLM,sRegRoot,"DomainName",(LPBYTE)pBuf,MBUFFER,(LPSTR)(LPCSTR)sServer,pTrans);
	sDomain.ReleaseBuffer();
	if (dwRtnVal)
		return dwRtnVal;

	// Retrieve CONSOLE PASSWORD
	pBuf = sDomPasswd.GetBuffer(MBUFFER);

	// Try to get the password directly from the server first because the
	// ConsolePassword in the AddressCache may not be valid
	if (RegKeyGetValue(HKLM,sRegRoot,"ConsolePassword",(LPBYTE)pBuf,MBUFFER,(LPSTR)(LPCSTR)sServer,pTrans))
	{
		dwRtnVal = RegKeyGetValue(HKLM,"~\\AddressCache\\" + sServer,"ConsolePassword",(LPBYTE)pBuf,MBUFFER,NULL);
	}

	sDomPasswd.ReleaseBuffer();

	return dwRtnVal;
}


////////////////////////////////////////////////////////////////////////////
//
// Function: ClientIsServer
//
// Description: Checks to see if the NAVCORP client is a server.  This
// function will look at the following value pair in the registry
// to determine the client type:
//
// [HKEY_LOCAL_MACHINE\Software\INTEL\LANDesk\VirusProtect6\CurrentVersion]
// "ClientType"=REG_DWORD
// 
// 1 = Connected Client; 2 = Standalone Client; 4 = Server
//
// Returns: TRUE if Server
//          FALSE otherwise
//
/////////////////////////////////////////////////////////////////////////////
// 7/16/99 MHOTTA Function Created
/////////////////////////////////////////////////////////////////////////////

BOOL ClientIsServer()
{
	CRegistryEntry* cReg;
	DWORD dwType;
	DWORD dwSize;
	DWORD dwDataBuf = 0;
	BOOL bReturn = FALSE;

	try
	{
		cReg = new CRegistryEntry( HKEY_LOCAL_MACHINE, 
									SERVER_CHECK_KEY, 
									CLIENTTYPE_KEY );
	}
	catch (std::bad_alloc &) {}

	if( NULL == cReg )
	{
		return( FALSE );
	}

	if( ERROR_SUCCESS == cReg->OpenKey(KEY_READ) )
	{
		dwSize = sizeof( dwDataBuf );
		if( ERROR_SUCCESS == cReg->GetValue( &dwType, 
											(LPBYTE)&dwDataBuf,
											&dwSize ) )
		{
			switch( dwDataBuf )
			{
			case CLIENTTYPE_CONNECTED:
				bReturn = FALSE;
				break;
			case CLIENTTYPE_STANDALONE:
				bReturn = FALSE;
				break;
			case CLIENTTYPE_SERVER:
				bReturn = TRUE;
				break;
			default:
				break;
			}
		}

		cReg->CloseKey();
	}

    delete cReg;

    return( bReturn );
}
