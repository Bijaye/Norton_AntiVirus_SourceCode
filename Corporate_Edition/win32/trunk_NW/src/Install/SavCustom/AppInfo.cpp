// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
/********************************************************
	Author: Gordon Roylance
	Class: Language Resource File Functionality

	Copywrite (C) 1997 Intel Corporation
********************************************************/

#include "stdafx.h"
#include "AppInfo.h"
#include "SymSaferRegistry.h"


CString CLangResource::m_sDefaultTag;

// Constructors
CLangResource::CLangResource()
{
	m_dwOptions = 0;
	m_hInstLang = 0;
	m_sLangSubDir = DEFAULT_DIR;
}

CLangResource::CLangResource(DWORD dwOptions,CString sFileID,CString sLangDir)
{
	m_dwOptions = 0;
	m_hInstLang = 0;
	m_sLangSubDir = DEFAULT_DIR;

	FileSettings(dwOptions,sFileID,sLangDir);
}

// Language File Settings
void CLangResource::FileSettings(DWORD dwOptions,CString sFileID,CString sLangDir)
{
	if ((!dwOptions || m_dwOptions == dwOptions) && 
		(sFileID.IsEmpty() || m_sFileID == sFileID) &&
		(sLangDir.IsEmpty() || m_sLangSubDir == sLangDir))
		return;
	
	if (dwOptions)
		m_dwOptions = dwOptions;

	if (!sFileID.IsEmpty())
		m_sFileID = sFileID;

	if (!sLangDir.IsEmpty())
		m_sLangSubDir = sLangDir;

	if (!sFileID.IsEmpty() && !(m_dwOptions & NON_RESOURCE))
		m_sFileID = FORMAT_STRING + m_sFileID + DLL_EXTENSION;

	CString sTempTag(m_sDefaultTag);
	SetLangAbbrev(EMPTY,m_sLangSubDir,m_sFileID);
	m_sLangTag = m_sDefaultTag;
	if (!sTempTag.IsEmpty())
		m_sDefaultTag = sTempTag;

	m_sLangFile.Format(m_sFileID,m_sLangTag);

	if (m_dwOptions & ENGLISH_INCLUDED && SameLocale(m_sLangTag,DEFAULT_LANGTAG))
	{
		m_sLangPath = GetModulePath();
		m_sLangFile = GetModuleName();
	}
	else
	{
		m_sLangPath.Empty();
		GetLangFilePath();
	}
}


// Destructor
CLangResource::~CLangResource()
{
	ReleaseLangFile();
}


// Load Language Resource File
DWORD CLangResource::SetLangResource(DWORD dwOptions,CString sFileID,CString sLangDir)
{
	DWORD dwRet = ReleaseLangFile();

	if (!dwRet)
	{
		FileSettings(dwOptions,sFileID,sLangDir);

		if (m_dwOptions & NON_RESOURCE)
			return ERROR_ACCESS_DENIED;

		if (m_dwOptions & ENGLISH_INCLUDED && SameLocale(m_sLangTag,DEFAULT_LANGTAG))
		{
			AfxSetResourceHandle(AfxGetInstanceHandle());
			m_hInstLang = AfxGetInstanceHandle();
			return 0;
		} 

		if (!m_sLangPath.IsEmpty())
		{
			m_hInstLang = LoadLibrary(m_sLangPath + BSLASH + m_sLangFile);

#ifdef _WIN32
			if (m_hInstLang)
				AfxSetResourceHandle(m_hInstLang);
			else
				dwRet = GetLastError();
#else
			if (m_hInstLang >= HINSTANCE_ERROR)
				AfxSetResourceHandle(m_hInstLang);
			else if (!m_hInstLang)
				dwRet = 1;
			else
				dwRet = (WORD)m_hInstLang;
#endif
		}
		else
			dwRet = ERROR_FILE_NOT_FOUND;
	}

	return dwRet;
}


// Return Resource Handle
HINSTANCE CLangResource::GetLangHandle()
{
	return m_hInstLang;
}


// Free Language Resource File
DWORD CLangResource::ReleaseLangFile()
{
	DWORD dwRet = 0;

	if (AfxGetInstanceHandle() != m_hInstLang)
	{
#ifdef _WIN32
		if (m_hInstLang && !FreeLibrary(m_hInstLang))
			dwRet = GetLastError();
#else
		if (m_hInstLang >= HINSTANCE_ERROR)
		{
			FreeLibrary(m_hInstLang);
			if (!m_hInstLang)
				dwRet = 1;
			else if (m_hInstLang < HINSTANCE_ERROR) 
				dwRet = (WORD)m_hInstLang;
		}
#endif
	}

	m_hInstLang = 0;
	return dwRet;
}


// Find Resource File Path
CString CLangResource::GetLangFilePath()
{
	CString sTemp(m_sLangPath + BSLASH + m_sLangFile);

	if (!m_sLangPath.IsEmpty() && FileExists(sTemp))
		return m_sLangPath;

	m_sLangPath = GetLangsDirectory(m_sLangSubDir,m_sLangFile);
	return m_sLangPath;
}


// Return Resource Filename and Path
CString CLangResource::GetLangFile()
{
	if (m_sLangPath.IsEmpty())
	{
		GetLangFilePath();
		if (m_sLangPath.IsEmpty())
			return m_sLangPath;
	}
	return m_sLangPath + BSLASH + m_sLangFile;
}


// Determine Whether Resource File Found
BOOL CLangResource::IsFileFound()
{
	if (!m_sLangPath.IsEmpty())
		return TRUE;

	return FALSE;
}


// Return Resource Filename
CString CLangResource::GetLangFileName()
{
	return m_sLangFile;
}


// Return FileNotFound Error Message
CString CLangResource::NoFileMessage()
{
	return CString(ERR_FILE_MISSING) + GetLangFileName();
}


// Return Language Resource Tag (Static member)
CString CLangResource::GetLangAbbrev(BOOL bDefault)
{
	CString sLangTag;

	if (bDefault && m_sDefaultTag.GetLength()==LANGTAG_SIZE)
		return m_sDefaultTag;

#ifdef _WIN32
	LCID locale = GetThreadLocale();
	GetLocaleInfo(locale,LOCALE_SABBREVLANGNAME,sLangTag.GetBuffer(SBUFFER),SBUFFER);

#else
	GetProfileString(LANGID_SECTION,LANGID_KEY,DEFAULT_LANGTAG,sLangTag.GetBuffer(SBUFFER),SBUFFER);

#endif
	sLangTag.ReleaseBuffer();
	sLangTag.MakeLower();
	sLangTag = sLangTag.Left(LANGTAG_SIZE);

	return sLangTag;
}


// Compare the two letter identifiers
BOOL CLangResource::SameLocale(CString sFirst, CString sSecond)
{
	sFirst.MakeLower();
	sSecond.MakeLower();
	return sFirst.Left(LANGTAG_SIZE-1) == sSecond.Left(LANGTAG_SIZE-1);
}


// Set Language Resource Tag (Static member)
void CLangResource::SetLangAbbrev(CString sLangTag,CString sProduct,CString sFile)
{
	if (!sLangTag.IsEmpty() || (sLangTag.IsEmpty() && sProduct.IsEmpty() && sFile.IsEmpty()))
		m_sDefaultTag = sLangTag;
	if (sProduct.IsEmpty())
	{
		if (sLangTag.IsEmpty())
			m_sDefaultTag = GetLangAbbrev(FALSE);
		return;
	}

	CString sUserKey;
#ifdef _WIN32
	HKEY regKey;
	DWORD type,size = MBUFFER;

	if (!RegOpenKeyEx(HKEY_LOCAL_MACHINE,DEFAULT_REGKEY,0,KEY_QUERY_VALUE,&regKey))
	{
		SymSaferRegQueryValueEx(regKey,DEFAULT_KEYNAME,0,&type,(LPBYTE)sUserKey.GetBuffer(MBUFFER),&size);
		sUserKey.ReleaseBuffer();
		RegCloseKey(regKey);
	}
#else
	GetProfileString(DEFAULT16_REGKEY,DEFAULT_KEYNAME,EMPTY,sUserKey.GetBuffer(MBUFFER),MBUFFER);
	sUserKey.ReleaseBuffer();
#endif

	// Special case for english
	BOOL bFileFound = SameLocale(m_sDefaultTag,DEFAULT_LANGTAG);

	// Check the default variable
	if (!bFileFound && !m_sDefaultTag.IsEmpty())
	{
		bFileFound = !GetLangsDirectory(sProduct,sFile,TRUE).IsEmpty();
		bFileFound = bFileFound && (SameLocale(m_sDefaultTag,sLangTag) || sLangTag.IsEmpty());
	}

	// Check the Intel Registry Key Language(s)
	if (!bFileFound && !sUserKey.IsEmpty())
	{
		CString sSelect(sUserKey),sTemp(sUserKey);
		do
		{
			int pos = sTemp.Find(COMMA);
			if (pos != -1)
			{
				sSelect = sTemp.Left(pos);
				sTemp = sTemp.Mid(pos+1);
			}
			else
			{
				sSelect = sTemp;
				sTemp.Empty();
			}

			if (SameLocale(sSelect,DEFAULT_LANGTAG))
			{
				m_sDefaultTag = DEFAULT_LANGTAG;
				bFileFound = TRUE;
				break;
			}
			m_sDefaultTag = sSelect;
			bFileFound = !GetLangsDirectory(sProduct,sFile,TRUE).IsEmpty();
			bFileFound = bFileFound && SameLocale(m_sDefaultTag,sSelect);

		} while (!sTemp.IsEmpty() && !bFileFound);
	}

	// Check the Operating System Language
	if (!bFileFound)
	{
		m_sDefaultTag = GetLangAbbrev(FALSE);
		if (!SameLocale(m_sDefaultTag,DEFAULT_LANGTAG))
		{
			bFileFound = !GetLangsDirectory(sProduct,sFile,TRUE).IsEmpty();
			bFileFound = bFileFound && SameLocale(m_sDefaultTag,GetLangAbbrev(FALSE));
		}
	}

	// Set default language abbreviation to English if given file/dir not found
	if (!bFileFound)
		m_sDefaultTag = DEFAULT_LANGTAG;


	// Add language tag to the Intel LANDesk user selection language key
	m_sDefaultTag.MakeLower();
	sUserKey.MakeLower();
	if (sUserKey.Find(m_sDefaultTag) == -1)
	{
		if (sUserKey.IsEmpty())
			sUserKey = m_sDefaultTag;
		else
			sUserKey += COMMA + m_sDefaultTag;
	}

#ifdef _WIN32
	if (!RegOpenKeyEx(HKEY_LOCAL_MACHINE,DEFAULT_REGKEY,0,KEY_SET_VALUE,&regKey))
	{
		RegSetValueEx(regKey,DEFAULT_KEYNAME,0,REG_SZ,(LPBYTE)(LPSTR)(LPCSTR)sUserKey,sUserKey.GetLength()+1);
		RegCloseKey(regKey);
	}
#endif
	WriteProfileString(DEFAULT16_REGKEY,DEFAULT_KEYNAME,sUserKey);
}


// Find the Language Abbreviation in order of precedence (Static member)
BOOL CLangResource::FindClosestAbbrev(CString sPath,CString& sSubDir,CString sFile,BOOL bSetDefault)
{
	if (sPath.IsEmpty())
		return FALSE;

	if (!m_sDefaultTag.IsEmpty() && SearchForAbbrev(m_sDefaultTag,sPath,sSubDir,sFile,bSetDefault))
		return TRUE;

	if (SearchForAbbrev(GetLangAbbrev(FALSE),sPath,sSubDir,sFile,bSetDefault))
		return TRUE;

	if (SearchForAbbrev(DEFAULT_LANGTAG,sPath,sSubDir,sFile,bSetDefault))
		return TRUE;

	return FALSE;
}


// Search for the closest match to language abbreviation (Static member)
BOOL CLangResource::SearchForAbbrev(CString sLangTag,CString sPath,CString& sSubDir,CString sFile,BOOL bSetDefault)
{
	CString sTmpDir,sTemp(sFile);

	if (!sSubDir.IsEmpty())
		sTmpDir = sSubDir + BSLASH + sLangTag;

	if (sTemp.Find(FORMAT_STRING) != -1)
		sFile.Format(sTemp,sLangTag);

	if (FileExists(sPath + sTmpDir + sFile))
	{
		if (bSetDefault)
			m_sDefaultTag = sLangTag;
		sSubDir = sTmpDir;
		return TRUE;
	}

	CFileFind fSearch;
	BOOL bFileFound;

	if (!sSubDir.IsEmpty())
	{
		sTmpDir = sTmpDir.Left(sTmpDir.GetLength()-1);
		sTmpDir += WILDCARD_QMARK;
		bFileFound = fSearch.FindFile(sPath + sTmpDir);
	}
	else if (sTemp.Find(FORMAT_STRING) != -1)
	{
		sFile.Format(sTemp,sLangTag.Left(LANGTAG_SIZE - 1) + WILDCARD_QMARK);
		bFileFound = fSearch.FindFile(sPath + sFile);
	}
	else
		return FALSE;

	if (bFileFound)
	{
		fSearch.FindNextFile();
		int qmark = -1;

		if (!sSubDir.IsEmpty())
		{
			int pos = sTmpDir.ReverseFind(BSLASH[0]);
			if (pos != -1)
				sTmpDir = sTmpDir.Left(pos+1);
			sTmpDir += fSearch.GetFileName();

			if (sTemp.Find(FORMAT_STRING) != -1)
				sFile.Format(sTemp,fSearch.GetFileName());
		}
		else
		{
			qmark = sFile.Find(WILDCARD_QMARK);
			int pos = sFile.ReverseFind(BSLASH[0]);
			if (pos != -1)
				sFile = sFile.Left(pos+1);
			sFile += fSearch.GetFileName();
		}

		if (FileExists(sPath + sTmpDir + sFile))
		{
			if (bSetDefault)
			{
				if (!sSubDir.IsEmpty())
					m_sDefaultTag = fSearch.GetFileName();
				else if (qmark != -1 && sFile.GetLength() > qmark)
					m_sDefaultTag = sLangTag.Left(LANGTAG_SIZE - 1) + sFile.GetAt(qmark);
			}
			sSubDir = sTmpDir;
			return TRUE;
		}
	}
	return FALSE;
}


// Return Path to LANGS Directory (Static member)
CString CLangResource::GetLangsDirectory(CString sPlatform,CString sFile,BOOL bSetDefault,CString sPath)
{
	CString sSubDir;

	if (sPath.IsEmpty())
		sPath = GetModulePath();

	if (!sFile.IsEmpty())
	{
		sFile = BSLASH + sFile;
		if (FindClosestAbbrev(sPath,sSubDir,sFile,bSetDefault))
			return sPath;
	}

	sSubDir = BSLASH LANGS_DIR;
	if (!sPlatform.IsEmpty())
		sFile = BSLASH + sPlatform + sFile;

    int pos = sPath.GetLength(); 

	while (pos != -1)
	{                                  
		if (sPlatform.IsEmpty() && FileExists(sPath + sSubDir + sFile))
			break;
		else if (!sPlatform.IsEmpty() && FindClosestAbbrev(sPath,sSubDir,sFile,bSetDefault))
			break;
		pos = sPath.ReverseFind(BSLASH[0]);
		if (pos != -1)
			sPath = sPath.Left(pos);

	}
	if (pos != -1)
	{
		if (!sPlatform.IsEmpty())
			sPath += sSubDir + BSLASH + sPlatform;
		else
			sPath += sSubDir;
	}
	else
		sPath.Empty();

	return sPath;
}


/********************************************************
	Functions: Application Level Functionality

	Copywrite (C) 1997 Intel Corporation
********************************************************/

// Get Main Module Path
CString GetModulePath()
{
	CString sTemp;
	GetModuleFileName(AfxGetInstanceHandle(),sTemp.GetBuffer(LBUFFER),LBUFFER);
	sTemp.ReleaseBuffer();
	sTemp.MakeLower();
	int pos = sTemp.ReverseFind(BSLASH[0]);
	if (pos != -1)
		return sTemp.Left(pos);
	return sTemp;
}


// Get Main Module Filename
CString GetModuleName()
{
	CString sTemp;
	GetModuleFileName(AfxGetInstanceHandle(),sTemp.GetBuffer(LBUFFER),LBUFFER);
	sTemp.ReleaseBuffer();
	sTemp.MakeLower();
	int pos = sTemp.ReverseFind(BSLASH[0]);
	if (pos != -1)
		return sTemp.Right(sTemp.GetLength()-pos-1);
	return sTemp;
}


// Ensure string has short path form
CString GetShortPath(CString sPath)
{
	CString sTemp(sPath);
#ifdef _WIN32
	GetShortPathName(sPath,sTemp.GetBuffer(LBUFFER),LBUFFER);
	sTemp.ReleaseBuffer();
#endif
	return sTemp;
}


// Get Computer Name
CString GetComputerName()
{
	DWORD size = LBUFFER;
	CString sTemp;
#ifdef _WIN32
	GetComputerName(sTemp.GetBuffer(LBUFFER), &size);
#else
	GetPrivateProfileString(CNAME_SECTION,CNAME_KEY,EMPTY,sTemp.GetBuffer(LBUFFER),(int)size,CNAME_FILE);
#endif
	sTemp.ReleaseBuffer();
	sTemp.MakeLower();
	return sTemp;
}


// Get System Directory
CString GetSystemRoot()
{
	CString sTemp;
	GetWindowsDirectory(sTemp.GetBuffer(LBUFFER),LBUFFER);
	sTemp.ReleaseBuffer();
	sTemp.MakeLower();
	int pos = sTemp.Find(BSLASH);
	if (pos != -1)
		return sTemp.Left(pos);
	return sTemp;
}


// Get Root from System Drive
CString GetSystemPath()
{
	CString sTemp;
	GetSystemDirectory(sTemp.GetBuffer(LBUFFER),LBUFFER);
	sTemp.ReleaseBuffer();
	sTemp.MakeLower();
	if (sTemp.GetAt(sTemp.GetLength()-1) == BSLASH[0])
		return sTemp.Left(sTemp.GetLength()-1);
	return sTemp;
}


// Get Windows Directory
CString GetWindowsPath()
{
	CString sTemp;
	GetWindowsDirectory(sTemp.GetBuffer(LBUFFER),LBUFFER);
	sTemp.ReleaseBuffer();
	sTemp.MakeLower();
	if (sTemp.GetAt(sTemp.GetLength()-1) == BSLASH[0])
		return sTemp.Left(sTemp.GetLength()-1);
	return sTemp;
}


// Get Filename of Complete Path
CString GetFileName(CString sFile)
{
	int pos = sFile.ReverseFind(BSLASH[0]);

	if (pos != -1)
		return sFile.Right(sFile.GetLength()-pos-1);
	return sFile;
}


// Get Directory of Complete Path
CString GetDirPath(CString sPath)
{
	int pos = sPath.ReverseFind(BSLASH[0]);
	if (pos != -1)
		return sPath.Left(pos);
	return sPath;
}


// Get Temporary Filename
DWORD GetTempFile(CString sFile, CString& sTempFile)
{
	DWORD dwCount = INIT_FILECOUNT, dwRtnVal = 0;
#ifdef _WIN32
	CString sNewFile(GetFileName(sFile));

	int pos = sNewFile.ReverseFind(FILENAME_DOT[0]);
	if (pos != -1)
		sNewFile = sNewFile.Left(pos);

	sNewFile = sNewFile.Left(INIT_NAMESIZE) + INIT_BUFFERNAME;
	sNewFile += INIT_FMT_HEX TMP_EXTENSION;

	do
	{
		sTempFile.Format(GetDirPath(sFile) + BSLASH + sNewFile,dwCount);
		dwCount++;
	} while(FileExists(sTempFile));

#endif
	return dwRtnVal;
}


// Relinquish message loop
void Relinquish()
{
	MSG msg;
	while(PeekMessage(&msg,NULL,NULL,NULL,PM_REMOVE))       
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}


// Return Windows NT version
int IsNT()
{
#ifdef _WIN32
	OSVERSIONINFO os;
	os.dwOSVersionInfoSize=sizeof(OSVERSIONINFO);
	GetVersionEx(&os);
	if (os.dwPlatformId == VER_PLATFORM_WIN32_NT)
		return os.dwMajorVersion;
#endif
	return 0;
}


// Return Windows version
int IsWinYear()
{
#ifdef _WIN32
	OSVERSIONINFO os;
	os.dwOSVersionInfoSize=sizeof(OSVERSIONINFO);
	GetVersionEx(&os);
	if (os.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS)
	{
		if (!os.dwMinorVersion)
			return 95;
		else
			return 98;
	}
#endif
	return 0;
}


// Return Hex String Value
DWORD StrToUL(CString sData, int nBase)
{
	DWORD dwValue = 0;
	LPSTR pEnd;

	sData.MakeLower();
	if (sData.GetLength() > 6 && sData.Find("0x") == 0)
	{
		dwValue |= (strtol(sData.Left(sData.GetLength()-4),&pEnd,16)) << 16;
		sData = "0x" + sData.Right(4);
		nBase = 16;
	}

	dwValue |= strtol(sData,&pEnd,nBase);

	return dwValue;
}


// Ensure format strings are exact
BOOL ExactFormat(CString sFormat,CString sTypes)
{
#ifdef _WIN32	
	sTypes.MakeLower();
	sFormat.MakeLower();
	int pos,pos2;
	do
	{
		pos = sTypes.ReverseFind('%');
		pos2 = sFormat.ReverseFind('%');
		if (pos != -1 && pos2 != -1)
		{
			TCHAR cPos=0, cPos2=0;
			
			if (pos+1 < sTypes.GetLength())
				cPos = sTypes.GetAt(pos+1);
			if (pos2+1 < sFormat.GetLength())
				cPos2 = sFormat.GetAt(pos2+1);

			if (cPos == 'c' || cPos == 'o' || cPos == 'i' || cPos == 'x' || cPos == 'u')
				cPos = 'd';
			if (cPos2 == 'c' || cPos2 == 'o' || cPos2 == 'i' || cPos2 == 'x' || cPos2 == 'u')
				cPos2 = 'd';

			if (cPos == 'f' || cPos == 'g')
				cPos = 'e';
			if (cPos2 == 'f' || cPos2 == 'g')
				cPos2 = 'e';

			if (cPos == cPos2 && (cPos == 'd' || cPos == 'e' || cPos == 's'))
			{
				sTypes = sTypes.Left(pos);
				sFormat = sFormat.Left(pos2);
			}
			else
				return FALSE;
		}
		else if (pos == -1 && pos2 == -1)
			return TRUE;

	} while (pos != -1 && pos2 != -1);
#endif

	return FALSE;
}


// Add string to multi-string list
DWORD AddStrToList(LPSTR buf,LPCSTR item,DWORD delimiter)
{        
	DWORD total=0;

#ifdef _WIN32
	LPSTR tmp, q = buf;
	DWORD size;
	BOOL found=FALSE;

	while (*q) 
	{
		if (delimiter && (tmp = _tcschr(q,(char)delimiter)) && tmp != q)
			size = tmp-q+1;
		else
			size = strlen(q)+1;

		if (!_tcsnicmp(q,item,size-1))
			found = TRUE;
		total += size;
		q += size;
	}
 	if (!found) 
	{
		if (*buf && delimiter)
			*(q-1) = (char)delimiter;
		_tcscpy(q,item);
		size = strlen(q)+1;
		total += size;
		q += size;
		*q = 0;
	}
	total++;
#endif
	return total;
}



// Remove string from multi-string list
DWORD DelStrFromList(LPSTR buf,DWORD len,LPCSTR item,DWORD delimiter,DWORD divider)
{         
	DWORD total=0;
	
#ifdef _WIN32
	LPSTR tmp, q = buf;
	DWORD size;

	while (*q) 
	{
		if (delimiter && (tmp = _tcschr(q,(char)delimiter)) && tmp != q)
			size = tmp-q+divider;
		else
			size = strlen(q)+divider;

		len -= size;
		if (!_tcsnicmp(q,item,size-divider)) 
			memmove(q,q+size,len);
		else 
		{
			total += size;
			q += size;
		}
	}
	if (*buf && *(q-1) == (char)delimiter)
		*(q-1) = '\0';

	if (total)
		total++;
#endif
	return total;
}


// Check for File or Directory Existence
BOOL FileExists(LPCSTR sFile)
{
#ifdef _WIN32
	return (GetFileAttributes(sFile) != (DWORD) -1);
#else
	CFileStatus status;
	return CFile::GetStatus(sFile,status);
#endif
}


// Check File Attributes
DWORD GetFileAttrib(LPCSTR sFile)
{
#ifdef _WIN32
	DWORD dwAttrib = GetFileAttributes(sFile);
	if (dwAttrib != (DWORD)-1)
		return dwAttrib;
#else
	CFileStatus status;
	if (CFile::GetStatus(sFile,status))
		return status.m_attribute;
#endif
	return 0;
}


// Check File Attributes
DWORD CompareTime(LPCSTR sFirst, LPCSTR sSecond)
{
#ifdef _WIN32
	struct _stat statFirst;
	struct _stat statSecond;
	if (!_stat(sFirst,&statFirst) && !_stat(sSecond,&statSecond))
	{
		if (statFirst.st_mtime == statSecond.st_mtime)
			return 0;
		if (statFirst.st_mtime < statSecond.st_mtime)
			return -1;
		if (statFirst.st_mtime > statSecond.st_mtime)
			return 1;
	}
#else
	CFileStatus statFirst, statSecond;
	if (CFile::GetStatus(sFirst,statFirst) && CFile::GetStatus(sSecond,statSecond))
	{
		if (statFirst.m_mtime == statSecond.m_mtime)
			return 0;
		if (statFirst.m_mtime < statSecond.m_mtime)
			return -1;
		if (statFirst.m_mtime > statSecond.m_mtime)
			return 1;
	}
#endif
	return 2;
}


// CopyFile Without Read-only Flag
BOOL CopyAFile(LPCSTR sSrc, LPCSTR sDest, BOOL bFail)
{
#ifdef _WIN32
	DWORD dwAtt = GetFileAttributes(sDest);
	if (dwAtt != (DWORD)-1)
		SetFileAttributes(sDest,dwAtt & ~FILE_ATTRIBUTE_READONLY);

	bFail = CopyFile(sSrc,sDest,bFail);
	if (bFail)
	{
		dwAtt = GetFileAttributes(sDest);
		if (dwAtt != (DWORD)-1)
			SetFileAttributes(sDest,dwAtt & ~FILE_ATTRIBUTE_READONLY);
	}
#endif
	return bFail;
}


// Update Files on System Reboot
DWORD UpdateOnReboot(CString sSrc, CString sDest)
{
	DWORD rtnVal = 0;

#ifdef _WIN32
	CString sRename;

	if (!sDest.IsEmpty())
	{
		rtnVal = GetTempFile(sDest,sRename);

		if (!rtnVal && !CopyAFile(sSrc,sRename,FALSE))
			rtnVal = GetLastError();
	}
	if (rtnVal)
		return rtnVal;

	if (IsNT())
	{
		if (sDest.IsEmpty())
		{
			if (!MoveFileEx(sSrc,NULL,MOVEFILE_DELAY_UNTIL_REBOOT))
				rtnVal = GetLastError();
		}
		else
		{
			if (MoveFileEx(sRename,sDest,MOVEFILE_DELAY_UNTIL_REBOOT | MOVEFILE_REPLACE_EXISTING))
				rtnVal = GetLastError();
		}
		if (rtnVal == ERROR_ALREADY_EXISTS)
			rtnVal = 0;
	}
	else
	{
		CString sWinInit(GetWindowsPath() + BSLASH + WININIT_FILE);

		if (sDest.IsEmpty())
		{
			DWORD dwCount = 0;
			CString sKey;
			do
			{
				sKey.Format(WININIT_KEY FMT_DECIMAL,dwCount);
				GetPrivateProfileString(WININIT_SECTION,sKey,EMPTY,sRename.GetBuffer(LBUFFER),LBUFFER,sWinInit);
				sRename.ReleaseBuffer();
				dwCount++;
			} while (!sRename.IsEmpty());

			sSrc = GetShortPath(sSrc);
			WritePrivateProfileString(WININIT_SECTION,sKey,sSrc,sWinInit);
		}
		else
		{
			sSrc = GetShortPath(sRename);
			sDest = GetShortPath(sDest);
			WritePrivateProfileString(WININIT_SECTION,sDest,sSrc,sWinInit);
		}
	}
#endif
	return rtnVal;
}


// Restart Windows
void RebootSystem(BOOL bWinNT)
{
#ifdef _WIN32
	if (bWinNT)
	{
        HANDLE hToken;
        TOKEN_PRIVILEGES tkp;

		if ( OpenProcessToken(GetCurrentProcess(),TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken) )
        {
		    if ( LookupPrivilegeValue(NULL, TEXT("SeShutdownPrivilege"), &tkp.Privileges[0].Luid) )
            {
		        tkp.PrivilegeCount = 1;
		        tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

		        AdjustTokenPrivileges(hToken, FALSE, &tkp, 0, (PTOKEN_PRIVILEGES)NULL, 0);
            }

            CloseHandle( hToken );
        }
	}
	ExitWindowsEx( EWX_REBOOT, 0 );
#endif
}


// Retrieve System Error Message
CString GetSystemMessage(DWORD dwError)
{
	CString sError;

#ifdef _WIN32
	LPTSTR pMsg;
	if (FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL,dwError,0,(LPTSTR)&pMsg,SBUFFER,NULL))
	{
		sError = pMsg;
		sError.TrimLeft();
		sError.TrimRight();
		LocalFree(pMsg);
	}
#else        
	sError = strerror(dwError);
#endif
	return sError;
}


// Display Last Error
void DisplayLastErrorMessage()
{
	CString sError;
	
#ifdef _WIN32
	sError = GetSystemMessage(GetLastError());
#else
	sError = GetSystemMessage(NULL);
#endif

	if (!sError.IsEmpty())
		AfxMessageBox(sError,MB_OK | MB_TASKMODAL);
}


// Determine if parent directory is empty besides the child directory 
BOOL IsParentEmpty(CString& sPath)
{
	CFileFind fSearch;
	CString sTemp;
	int pos = sPath.ReverseFind(BSLASH[0]);

	if (pos != -1)
	{
		sTemp = sPath.Right(sPath.GetLength()-pos-1);
		sPath = sPath.Left(pos);
		BOOL bFileFound = fSearch.FindFile(sPath + BSLASH WILDCARD_SEARCH);
		while (bFileFound)
		{
			bFileFound = fSearch.FindNextFile();
			if (fSearch.IsDots() || (fSearch.IsDirectory() && !sTemp.CompareNoCase(fSearch.GetFileName())))
				continue;
			else
			{
				sPath += BSLASH + sTemp;
				return FALSE;
			}
		}
		return TRUE;
	}
	return FALSE;
}


// Uses batch file to delete executable and dependencies
BOOL DeleteMyself(CStringArray *pArray)
{
	CString sAppPath(GetShortPath(GetModulePath())),sAppName(GetModuleName());
	CString sBatchPath(GetShortPath(GetWindowsPath()));
	std::ofstream outFile(sBatchPath + BSLASH BATCH_FILE);

#ifdef _WIN32
	SetCurrentDirectory(sBatchPath);
#else
	_chdir(sBatchPath);
#endif

	// Create batch file
	if (!outFile.is_open())
		return FALSE;

	outFile << REPEAT_LABEL << std::endl;
	outFile << DEL_FILE + sAppPath + BSLASH + sAppName << std::endl;
	if (pArray)
	{
		for (int index = 0; index < pArray->GetSize(); index++)
			outFile << DEL_FILE + sAppPath + BSLASH + pArray->GetAt(index) << std::endl;
	}
	outFile << IF_EXISTS + sAppPath + BSLASH + sAppName + GOTO_LABEL << std::endl;
	outFile << REMOVE_DIR + sAppPath << std::endl;

	while (IsParentEmpty(sAppPath))
		outFile << REMOVE_DIR + sAppPath << std::endl;

#ifndef _WIN32
	outFile << DEL_FILE + sBatchPath + BSLASH BATCH_PIF << std::endl;
#endif
	outFile << DEL_FILE + sBatchPath + BSLASH BATCH_FILE << std::endl;
	outFile.close();

#ifdef _WIN32
	PROCESS_INFORMATION pi;
	STARTUPINFO si;
	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	si.dwFlags = STARTF_USESHOWWINDOW;
	si.wShowWindow = SW_HIDE;

	// Create the batch file process with low priority and suspended
	if (CreateProcess(NULL,(LPSTR)(LPCSTR)(sBatchPath + BSLASH BATCH_FILE),NULL,NULL,FALSE,
	 CREATE_SUSPENDED | IDLE_PRIORITY_CLASS,NULL,sBatchPath,&si,&pi))
	{
		// Lower batch file thread priority
		SetThreadPriority(pi.hThread, THREAD_PRIORITY_IDLE);

		// Raise application priority
		SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL);
		SetPriorityClass(GetCurrentProcess(), HIGH_PRIORITY_CLASS);

		CloseHandle(pi.hProcess);
		ResumeThread(pi.hThread);
		CloseHandle(pi.hThread);
		SHChangeNotify(SHCNE_RMDIR, SHCNF_PATH, sAppPath, 0);
		return TRUE;
	}
#else
	if (WinExec(sBatchPath + BSLASH BATCH_FILE,SW_HIDE) >= (UINT)HINSTANCE_ERROR)
		return TRUE;
#endif
	return FALSE;
}

