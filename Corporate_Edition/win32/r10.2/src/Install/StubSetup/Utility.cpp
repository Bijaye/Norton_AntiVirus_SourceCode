// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
// Utility.cpp : Utility routines
//

#include "stdafx.h"
#include "Utility.h"
#include "_UtilityErrors.h"

#include <sys/stat.h>
#include <fcntl.h>
#include <io.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern "C" 
{
	DWORD APIENTRY WNetGetResourceInformationA (
    LPNETRESOURCE lpNetResource,
    LPVOID lpBuffer,
    LPDWORD cbBuffer,
    LPTSTR *lplpSystem
    );
#define WNetGetResourceInformation  WNetGetResourceInformationA
}

BOOL IsPathValid(CString strPath, BOOL bAutoLogon, CWnd* pParentWnd)
{
	BOOL bResult = TRUE;

	if(strPath.GetLength()<=1) return FALSE;

	// If this is a UNC path, make sure that we're logged on...
	if(strPath.Find("\\\\")==0 && bAutoLogon)
	{
		NETRESOURCE netRes;

		char szBuf[512];
		char *pBuf=NULL;

		netRes.lpRemoteName = (LPTSTR)strPath.GetBuffer(strPath.GetLength());
		netRes.lpLocalName = NULL;
		netRes.lpProvider = NULL;
		netRes.dwType = RESOURCETYPE_DISK;
		DWORD dwSize=512;
			
		AfxGetApp()->BeginWaitCursor();

		DWORD dwRet = WNetGetResourceInformation(&netRes, szBuf, &dwSize, &pBuf);

		CString strRemote;
		strRemote = netRes.lpRemoteName;

		strPath.ReleaseBuffer();

		AfxGetApp()->EndWaitCursor();

		// The WNetGetResInfo(...) fn used above behaves slightly different
		// under Windows NT. NT doesn't break the lpRemoteName element of the NETRES
		// structure into the network resource.
//		if(pApp->GetWinPlatform()==VER_PLATFORM_WIN32_NT && pBuf!=NULL)
		if(pBuf != NULL)
		{
			int nPos=-1;
			CString strBuf;
			strBuf = strRemote;

			if((nPos=strBuf.Find(pBuf))>=0)
			{
				strRemote = strBuf.Left(nPos);
			}
		}
		
		if(dwRet==NO_ERROR)
		{
			if(netRes.dwScope != RESOURCE_CONNECTED)
			{
				DWORD dwFlags = CONNECT_INTERACTIVE;

				netRes.lpRemoteName = (LPTSTR)strRemote.GetBuffer(strRemote.GetLength());

				dwRet = WNetAddConnection3(pParentWnd->m_hWnd, &netRes, NULL, NULL, dwFlags);
				strRemote.ReleaseBuffer();

				if(dwRet!=NO_ERROR)
				{
					LPVOID lpMsgBuf;
					CString strMessage;
				
					FormatMessage( 
						FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
						NULL,
						dwRet,
						MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
						(LPTSTR) &lpMsgBuf, 0, NULL );// Display the string.

//					strMessage.Format( IDS_NET_CONNECTIONERROR, lpMsgBuf );
				
					LocalFree( lpMsgBuf ); // Free the buffer.

//					AfxMessageBox( strMessage, MB_OK|MB_ICONINFORMATION );

					return FALSE;
				}
			}
		} 
		else 
		{
//			AfxMessageBox(IDS_NET_RESOURCENOTFOUND, MB_OK|MB_ICONINFORMATION);
			return FALSE;
		}
	}	
	
	// Now, check the validity of the path...

	if((strPath.GetLength()==3 && strPath[1]==':' && strPath[2]=='\\') ||
		(strPath.GetLength()==2 && strPath[1]==':'))
	{
		// Temporarily return TRUE, eventually make sure drive letter valid...
		bResult = TRUE;
	}
	else
	{
		WIN32_FIND_DATA find;

		if(strPath.GetAt(strPath.GetLength()-1)=='\\')
			strPath = strPath.Left(strPath.GetLength()-1);

		HANDLE hResult = FindFirstFile(strPath, &find);
			
		if(hResult == INVALID_HANDLE_VALUE 
			|| !(find.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
		{
			bResult = FALSE;
		}
		if(hResult != INVALID_HANDLE_VALUE) ::FindClose(hResult);
	}
	return bResult;
}

BOOL BuildPathName(CString& strPath, CString& strFilename)
{
	if(strPath.GetLength()<=1) return FALSE;

	if(strPath[strPath.GetLength()-1]!='\\')
		strPath += "\\";

	strPath += strFilename;
	return TRUE;
}

BOOL GetRoot(CString& strRoot, CString& strPath, BOOL bTrailingSlash)
{
	int nIndex;
	if(strPath.IsEmpty()) return FALSE;

	BOOL bUNC=FALSE;
	if(strPath.Find("\\\\")==0) bUNC = TRUE;

	strRoot = strPath;

	if(!bUNC)
	{
		while ((nIndex= strRoot.ReverseFind('\\')) >= 0)
		{
			if(bUNC && nIndex==1) break;
			strRoot= strRoot.Left(nIndex);
		}
		if(bTrailingSlash) strRoot+= '\\';
		strRoot.MakeUpper();
	}
	else
	{
		while ((nIndex= strRoot.ReverseFind('\\')) >= 0)
		{
			CString strBuf;
			strBuf = strRoot.Left(nIndex);

			int nNext = strBuf.ReverseFind('\\');
			if(nNext == 1) break;

			strRoot= strBuf;
		}
		if(bTrailingSlash) strRoot+= '\\';
	}
	return TRUE;
}

BOOL IsPathEmpty(CString& strPath)
{
	CString strWildCard;
	strWildCard.Format("%s\\*.*", strPath);

	WIN32_FIND_DATA FindData;

	BOOL bEmpty = TRUE;

	// find first entry in directory
	HANDLE hFind= ::FindFirstFile(strWildCard, &FindData);

	// directory is empty? -> we're done
    if (hFind==INVALID_HANDLE_VALUE)
       return TRUE;

    do {
		//	Note: Win32 FindFirstFile/FindNextFile under Windows NT 
		//	report folders '.' and '..', whereas under Windows 95 they do not.
		//	To make the dwIDs agree on the two platforms, we'll skip
		//	these two folders.
		if( _tcscmp( FindData.cFileName, TEXT(".") ) &&
			_tcscmp( FindData.cFileName, TEXT("..") ) )
		{
			bEmpty = FALSE;
			break;
		}

	} while (::FindNextFile( hFind, &FindData ));

    ::FindClose(hFind);
  
	return bEmpty;
}

BOOL FileExists(CString& strFile)
{
	WIN32_FIND_DATA find;

	HANDLE hResult = FindFirstFile(strFile, &find);
		
	if(hResult == INVALID_HANDLE_VALUE )
	{
		return FALSE;
	}

	::FindClose(hResult);
	return TRUE;
}

BOOL GetUniqueFile(CString& strExisting, CString& strUnique)
{
	CString strPrefix("");
	CString strSuffix("");

	if(!::FileExists(strExisting)) 
	{
		strUnique = strExisting;
		return TRUE;
	}

	int nIndex = strExisting.ReverseFind('.');
	if(nIndex >= 0)
	{
		strPrefix = strExisting.Left(nIndex);
		strSuffix = strExisting.Right(strExisting.GetLength() - nIndex);
	}

	WIN32_FIND_DATA find;

	HANDLE hResult;

	int nCount = 0;
	strUnique.Format("%s%d%s", strPrefix, nCount, strSuffix);

	while((hResult = FindFirstFile(strUnique, &find)) != INVALID_HANDLE_VALUE)
	{
		::FindClose(hResult);

		++nCount;

		if(nCount > 10000) return FALSE;

		strUnique.Format("%s%02d%s", strPrefix, nCount, strSuffix);
	}
	return TRUE;
}

#if 0
DWORD _CopyFile(CString strSource, CString strTarget, CProgressCtrl* pProgress, int nBufferSize, BOOL* pbCancel)
{
	struct _finddata_t fileinfo;
	int nTotalBytes= 0;
	int nTotalBlocks= 0;
	int handle= _findfirst( strSource, &fileinfo);
	if (handle < 0)
	{
		return UTILITYERROR_POSTCOPYERROR;
	}
	_findclose(handle);

	CFile inFile, outFile;
	if (!inFile.Open( strSource, CFile::modeRead|CFile::shareDenyWrite|CFile::typeBinary ))
	{
		return UTILITYERROR_POSTCOPYERROR;
	}

	if (!outFile.Open( strTarget, CFile::modeWrite|CFile::modeCreate|CFile::typeBinary|CFile::shareExclusive ))
	{
		inFile.Close();
		return UTILITYERROR_POSTCOPYERROR;
	}

	int nBlocks= 0;
	int inBytes;

	int nCount= (fileinfo.size+nBufferSize-1) / nBufferSize;
	int nStep = max(nCount / 1000, 1);

	if(nCount > 1) 
	{
		if(pProgress) pProgress->SetRange( 0, nCount / nStep);
//		pProgress->SetRange( 0, nCount-1 );
	}

	if(pProgress) pProgress->SetPos(0);
	if(pProgress) pProgress->UpdateWindow();

	int nBlock = 0;
	BYTE *pBuf = (BYTE*)malloc( nBufferSize );

	for (int i= 0; ; i+= inBytes)
	{
		++nBlock;

		if ((inBytes= inFile.Read( pBuf, nBufferSize ))==0)
			break;

		if(pbCancel)
		{
			if(*pbCancel) break;
		}

		outFile.Write( pBuf, inBytes );

		if(nCount > 1) 
		{
//			TRACE("pProgress->SetPos(%d)\n", nBlock / nStep);
			if(pProgress) pProgress->SetPos(nBlock / nStep);
			if(pProgress) pProgress->UpdateWindow();
		}
	}

	free(pBuf);

	outFile.Close();
	inFile.Close();

	return 0;
}
#endif

DWORD CreateDirectories(CString sPath)
{
	if(sPath.IsEmpty()) return 1;

	CString sTemp(sPath);
	DWORD rtnVal = 0;
	while (!::CreateDirectory(sTemp,NULL))
	{
		rtnVal = GetLastError();
		if (rtnVal == ERROR_PATH_NOT_FOUND || rtnVal == ERROR_FILE_NOT_FOUND)
		{
			int pos = sPath.ReverseFind('\\');
			if (pos != -1)
			{
				rtnVal = CreateDirectories(sPath.Left(pos));
				if (!rtnVal)
					continue;
			}
		}
		break;
	}
	if (rtnVal == ERROR_ALREADY_EXISTS)
		rtnVal = 0;
	return rtnVal;
}

