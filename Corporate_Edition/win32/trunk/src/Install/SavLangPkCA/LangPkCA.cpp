
#include "stdafx.h"
#include "LangPkCA.h"
#include <comdef.h>
#include <SETUPAPI.h>
#include "BldNum.h"

HMODULE g_CabModuleHandle;			    //Module handle to the CAB support DLL
SETUPITERATECABPROC g_CabIterateProc;	//SetupIterateCab procedure pointer

map <string , string> g_mapLPs;
string g_sLangId = "";
string g_sGroupDestination = "";   

CString GetMSIProperty( MSIHANDLE hInstall, CString strProperty )
{
	CString strRet = "", strTemp;
	char *pCAData = NULL;
	DWORD dwLen = 0;	// This needs to be 0 so that we get ERROR_MORE_DATA error
	UINT nRet = 0;

	strProperty.LockBuffer();

	try
	{
		nRet = MsiGetProperty( hInstall, strProperty, "", &dwLen );
		if( ERROR_MORE_DATA != nRet )
		{
			strTemp.Format( "Error getting length of %s (nRet = %d)", strProperty, nRet );
			::MessageBox(NULL, strTemp, "Error*", MB_OK);
			throw( strTemp );
		}

		// Allocate our memory and grab the data, add one for the trailing null
		pCAData = new char[ ++dwLen ];
		if( !pCAData )
			throw( CString("Unable to allocate memory for data buffer") );

		nRet = MsiGetProperty( hInstall, strProperty, pCAData, &dwLen );
		if( ERROR_SUCCESS != nRet )
		{
			strTemp.Format( "Unable to obtain the %s (nRet = %d)", strProperty, nRet );
			::MessageBox(NULL, strTemp, "Error", MB_OK);
			throw( strTemp );
		}

		// Save the value
		strRet.Format( "%s", pCAData );

		// Log the value to the MSI log file
		strTemp.Format( "%s=%s", strProperty, strRet );
		MSILogMessage( hInstall, strTemp );
	}
	catch( CString strErr )
	{
		MSILogMessage( hInstall, strErr );
		strRet = "";
		MessageBox(NULL, "*** Error ***", 0, MB_OK);
	}

	strProperty.UnlockBuffer();

	if( pCAData )
		delete [] pCAData;

	return strRet;
}

UINT MSILogMessage( MSIHANDLE hInstall, CString& szString )
{
	UINT uiRetVal = MSILogMessage( hInstall, szString.LockBuffer() );
	szString.UnlockBuffer();
	return uiRetVal;
}

UINT MSILogMessage( MSIHANDLE hInstall, TCHAR* szString )
{
	UINT uiRetVal = ERROR_SUCCESS;
	CString szLogLine( LOG_PREFIX );

	szLogLine += szString;

	PMSIHANDLE hRec = MsiCreateRecord( 1 );

	if( hRec )
	{
		MsiRecordClearData( hRec );
		MsiRecordSetString( hRec, 0, (LPCSTR)szLogLine );
		MsiProcessMessage( hInstall, INSTALLMESSAGE_INFO, hRec );
	}

	OutputDebugString( szLogLine );

	return uiRetVal;
}


HRESULT CabInitialize(MSIHANDLE hInstall)
{
	HRESULT returnVal = E_FAIL;

	//Have we successfully initialized yet?
	if ((g_CabModuleHandle == NULL) && (g_CabIterateProc == NULL))
	{
		//No - try to load it
		g_CabModuleHandle = LoadLibrary("SetupApi.dll");
		if (g_CabModuleHandle)
		{
			//Is the procedure supported in the local SETUPAPI?
			g_CabIterateProc = (SETUPITERATECABPROC) GetProcAddress(g_CabModuleHandle, SETUPITERATECABNAME);
			if (g_CabIterateProc != NULL)
			{
				returnVal = S_OK;
			} 
			else
			{
				//No - clear all vars
				FreeLibrary(g_CabModuleHandle);
				g_CabModuleHandle = NULL;
				MSILogMessage(hInstall, "GetProcAddress failed on SetupApi.dll");
				returnVal = E_FAIL;
			}
		}
		else
		{
			MSILogMessage(hInstall, "Failed to load SetupApi.dll");
			returnVal = E_FAIL;
		}
	}

	return returnVal;
}


UINT WINAPI CabHandler(PVOID context, UINT notificationCode, UINT param1, UINT param2)
//Callback used by SetupIterateCab to list or extract files from a CAB
{
	CabArgs* args = NULL;
	FILE_IN_CABINET_INFO* fileInfo = NULL;
	FILEPATHS* extractedInfo = NULL;
	TCHAR szTarget[MAX_PATH];

	args = (CabArgs*) context;

	switch(notificationCode)
	{
		case SPFILENOTIFY_FILEEXTRACTED:
			extractedInfo = (FILEPATHS*) param1;
			if (extractedInfo && args)
			{
				if (_mbsicmp((LPBYTE) args->targetFilename.c_str(), (LPBYTE) extractedInfo->Target) == 0)
					args->extractedFile = true;

				return extractedInfo->Win32Error;
			}
			else if(extractedInfo && g_sGroupDestination != "")
			{
				return extractedInfo->Win32Error;
			}
			else
			{
				return ERROR_INVALID_FUNCTION;
			}
			break;

		case SPFILENOTIFY_FILEINCABINET:
			fileInfo = (FILE_IN_CABINET_INFO*) param1;

			//Validate paramters
			if (fileInfo == NULL && g_sGroupDestination == "")
				return FILEOP_SKIP;

			//Is this the file we want to extract?
			if(args != NULL)
			{
				if (_mbsicmp((LPBYTE) args->sourceFilename.c_str(), (LPBYTE) fileInfo->NameInCabinet) == 0)
				{
					//Yes - extract it
					lstrcpyn(fileInfo->FullTargetName, args->targetFilename.c_str(), sizeof(fileInfo->FullTargetName)/sizeof(fileInfo->FullTargetName[0]));
					return FILEOP_DOIT;
				}
				else if (args->extractedFile)
				{
					//No, but we've already extracted that file, so abort
					return FILEOP_ABORT;
				}
				else
				{
					//No - skip it
					return FILEOP_SKIP;
				}
			}
			else if(g_sGroupDestination != "") // We are copying all files from the .cab file
			{
				_tcscpy(szTarget, g_sGroupDestination.c_str());
				lstrcat(szTarget, fileInfo->NameInCabinet);
				lstrcpy(fileInfo->FullTargetName, szTarget);
				return FILEOP_DOIT;
			}
			break;

		case SPFILENOTIFY_NEEDNEWCABINET:
			return ERROR_INSTALL_FAILURE;
			break;

		default:
			//This seems to be the safest general return code for CAB notificatons
			return NO_ERROR;
	}
	return NO_ERROR;
}


BOOL DeleteDirectory(const TCHAR* sPath, MSIHANDLE hInstall) 
{
	HANDLE hFind;    // file handle
	WIN32_FIND_DATA FindFileData;
	BOOL bDirFound = FALSE;
	TCHAR sDirPath[MAX_PATH];
	TCHAR FileName[MAX_PATH];
	CString strMsg;
	_tcscpy(FileName, sPath);
	_tcscat(FileName, "\\");    // searching all files
	_tcscpy(sDirPath, sPath);
	_tcscat(sDirPath, "\\*.*");

	// find the first file
	hFind = FindFirstFile(sDirPath, &FindFileData);

	if(hFind == INVALID_HANDLE_VALUE) 
	{
		strMsg.Format("Failed to find files at location - %s", sDirPath);
		MSILogMessage(hInstall, strMsg);
		return FALSE;
	}

	_tcscpy(sDirPath, FileName);

	bool bSearch = true;
	while(bSearch) 
	{    // until we find an entry
		if(FindNextFile(hFind, &FindFileData)) 
		{
			if(_tcscmp(FindFileData.cFileName, ".") == 0 || _tcscmp(FindFileData.cFileName, "..") == 0 || 
			   _tcscmp(FindFileData.cFileName, g_sLangId.c_str()) == 0)
				continue;

			_tcscat(FileName, FindFileData.cFileName);
			if((FindFileData.dwFileAttributes &	FILE_ATTRIBUTE_DIRECTORY)) 
			{
				if(!DeleteDirectory(FileName, hInstall)) 
				{
					FindClose(hFind);
					return FALSE;    // directory couldn't be deleted
				}
				// remove the empty directory
				RemoveDirectory(FileName);
				_tcscpy(FileName, sDirPath);
			
			}
			else 
			{
				if(!DeleteFile(FileName)) 
				{
					// delete the file
					FindClose(hFind);
					return FALSE;
				}
				_tcscpy(FileName, sDirPath);
			}
		}
		else 
		{
			if(GetLastError() == ERROR_NO_MORE_FILES)
				bSearch = false;
			else 
			{
				// some error occurred; close the handle and return FALSE
				FindClose(hFind);
				return FALSE;
			}

		}
	}
	FindClose(hFind);                  // close the file handle
	RemoveDirectory(sPath);
	return TRUE;     // remove the empty directory
}


string CreateTempResDir(string sInstallLocation, MSIHANDLE hInstall)
{
	string szTempDir;
	CString sStr;

	szTempDir = sInstallLocation;
	szTempDir = sInstallLocation + "res\\tmp001";

	if(CreateDirectory((LPCTSTR)szTempDir.c_str(), NULL) == 0)
	{
		DWORD dw = GetLastError();
		sStr.Format("Failed to create Directory  %s - Error = %lu", szTempDir, dw);
		MSILogMessage(hInstall, sStr);
		return "";
	}

	return "tmp001";
}





BOOL CabDecompressFile( string sourceFilename, string sInstallLocation, MSIHANDLE hInstall)
//Decompresses a file compressed with Win9x/NT4+ CAB file compression
{
	HRESULT returnVal = E_FAIL;
	BOOL bRet = FALSE;
	CabArgs args;
	string sCabTempFileName, szLangId, targetFilename;
	CString sMsiMessage;
	map <string, string>::iterator it;
	
	for(it = g_mapLPs.begin(); it != g_mapLPs.end(); it++)
	{
		szLangId = (*it).first;
		sCabTempFileName = (*it).second;
		string sTempResPath = CreateTempResDir(sInstallLocation, hInstall);
		
		if(sTempResPath == "")
			return false;

		targetFilename = sInstallLocation + "res\\" + sTempResPath + "\\" + sourceFilename;

		//Decompress file
		args.sourceFilename = sourceFilename;
		args.targetFilename = targetFilename;
		args.extractedFile = false;

		bRet = g_CabIterateProc(sCabTempFileName.c_str(), NULL, (PSP_FILE_CALLBACK) CabHandler, (PVOID) &args);
		if (bRet)
		{
			if (args.extractedFile) // Successfully extracted one file from the cabinate
			{
				DWORD dwInfoSize = GetFileVersionInfoSize((const char*)targetFilename.c_str(), NULL);
				LPBYTE lpData = new BYTE[dwInfoSize];
				GetFileVersionInfo((const char*)targetFilename.c_str(), NULL, dwInfoSize, lpData);
				VS_FIXEDFILEINFO *vInfo;
				UINT dwInfoSize2;

				if(VerQueryValue(lpData, "\\", (LPVOID*)&vInfo, &dwInfoSize2))
				{
					string sFormat = _T("%d.%d.%d.%d");
					TCHAR sVer[MAX_PATH];
					wsprintf(sVer, sFormat.c_str(), HIWORD(vInfo->dwFileVersionMS), 
						LOWORD(vInfo->dwFileVersionMS), HIWORD(vInfo->dwFileVersionLS) ,LOWORD(vInfo->dwFileVersionLS));

					CString csProductVer;
					csProductVer.Format("%d.%d.%d.%d", MAINPRODUCTVERSION, SUBPRODUCTVERSION, INLINEPRODUCTVERSION, BUILDNUMBER);
					if(_tcscmp(sVer, csProductVer) == 0) // ** change the != to ==
					{	// Extract all files
						g_sGroupDestination = sInstallLocation + "res\\" + sTempResPath + "\\";
						bRet = g_CabIterateProc(sCabTempFileName.c_str(), 0, (PSP_FILE_CALLBACK) CabHandler, 0);
					}
					else
					{	// Remove the file extracted from the cabinate
						string sTempDir = sInstallLocation + "res\\" + sTempResPath;
						DeleteDirectory(sTempDir.c_str(), hInstall);
						sMsiMessage.Format("Failed to install language pack - %s, version mismatch", sCabTempFileName.c_str());
						delete [] lpData;
						MSILogMessage(hInstall, sMsiMessage);
						//bRet = FALSE;
						continue;
					}
				}
				delete [] lpData;

				string szLangDir = sInstallLocation + "res\\" + szLangId;
				_trename(g_sGroupDestination.c_str(), szLangDir.c_str());
				g_sGroupDestination = "";
			}
			else
			{
				//** Failed to extract the first file
				sMsiMessage.Format("Failed to extract the first file - '%s' from the language pack ", sourceFilename.c_str());
				MSILogMessage(hInstall, sMsiMessage);
				bRet = FALSE;
			}
		}
		else
		{
			// ** Extraction failed completely
			sMsiMessage.Format("Extraction of language pack - '%s' failed completely", sCabTempFileName.c_str());
			MSILogMessage(hInstall, sMsiMessage);
			bRet = FALSE;
		}
	}
	return bRet;
}



BOOL GetLanguagePacks(string sFilePattern, string sLocation)
{
	BOOL bRet = TRUE, bFind = true;
	string szLangID, szTemp;

	//** get msi property and assign to szLpDir**
	WIN32_FIND_DATA stFind;
	HANDLE hResult;

	if( INVALID_HANDLE_VALUE != (hResult = FindFirstFile(sFilePattern.c_str(), &stFind)) )
		bRet = true;

	while(hResult != INVALID_HANDLE_VALUE && bFind)
	{
		szTemp = stFind.cFileName;
		szLangID = szTemp.replace(0, 9, "");
		szLangID = szLangID.replace(szLangID.length() - 4, 4, "");

		g_mapLPs[szLangID] = sLocation;
		g_mapLPs[szLangID] = g_mapLPs[szLangID] + "Tools\\" + "Language Packs\\" + stFind.cFileName;

		bFind = FindNextFile(hResult, &stFind);
	}

	return bRet;
}


UINT __stdcall InstallLanguagePacks(MSIHANDLE hInstall)
{
	DWORD dwReturnVal = 0;
	CString	sCAParams;
	string sSourceDir, sInstallDir;;

	sCAParams = GetMSIProperty( hInstall, "CustomActionData" );
	int iIndex = sCAParams.Find(", ");
	sSourceDir = (LPCTSTR)sCAParams.Left(iIndex);
	sInstallDir = (LPCTSTR)sCAParams.Right(sCAParams.GetLength() - (iIndex + 2) );

	iIndex = (int) sSourceDir.find_last_of("\\", sSourceDir.length() - 2);

	sSourceDir = sSourceDir.substr(0, iIndex + 1);

	if( sCAParams.GetLength() == 0 )
	{
		throw( _T("No CustomActionData present for InstallLanguagePacks.") );
	}

	std::string szLpSearchPattern = sSourceDir;
	szLpSearchPattern = szLpSearchPattern + "Tools\\Language Packs\\lang-sav*.cab";
	if(GetLanguagePacks(szLpSearchPattern, sSourceDir))
	{
		if(SUCCEEDED(CabInitialize(hInstall)))
		{
			if(CabDecompressFile("actares.dll", sInstallDir, hInstall))
				MSILogMessage( hInstall, "Failed to extract file from cab file" );

		}
		else
			MSILogMessage( hInstall, "CabInitialize succeeded" );
	}

	return dwReturnVal;
}


UINT __stdcall RemoveLanguagePacks(MSIHANDLE hInstall)
{
	DWORD dwReturnVal = ERROR_FILE_NOT_FOUND;
	string sSearchPattern, sInstallDir, sLangId;
	CString	sCAParams;
	BOOL bRet = false;
	BOOL bFind = true;

	sCAParams = GetMSIProperty( hInstall, "CustomActionData" );
	int iIndex = sCAParams.Find(", ");
	sInstallDir = (LPCTSTR)sCAParams.Left(iIndex);
	g_sLangId = (LPCTSTR)sCAParams.Right(sCAParams.GetLength() - (iIndex + 2) );


	if( sCAParams.GetLength() == 0 )
	{
		throw( _T("No CustomActionData present for RemoveLanguagePacks.") );
	}

	sSearchPattern = sInstallDir + "res";

	DeleteDirectory(sSearchPattern.c_str(), hInstall);

	return 0;
}
