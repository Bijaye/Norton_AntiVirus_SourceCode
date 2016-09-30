// Copyright 1998 Symantec Corporation
//***************************************************************************
//
// $Header:   S:/ATMIGRAT/VCS/shelllnk.cpv   1.1   19 Nov 1998 13:14:34   MBROWN  $
//
// Description:
//    Shortcut detection/creation functions pulled from Symutil
//
// Contains:
//
//***************************************************************************
// $Log:   S:/ATMIGRAT/VCS/shelllnk.cpv  $
// 
//    Rev 1.1   19 Nov 1998 13:14:34   MBROWN
// Changed method for getting shell folders, deleting NPS links, added entry point for unloading NAV
// 
//    Rev 1.0   28 Sep 1998 11:42:54   MBROWN
// Initial revision.
// 
// 
//***************************************************************************
#include <stdlib.h>
#include "platform.h"
#if defined(SYM_WIN16)
#include <ddeml.h>
#endif
#if defined(SYM_WIN32)
#include <ole2.h>
#include <shlguid.h>
#include <shlobj.h>
typedef IShellLink FAR* LPSHELLLINK;
#endif
#include "cwstring.h"
#include "shelllnk.h"


//***************************************************************************
// Exported Routines
//***************************************************************************





/* --------------------------------------------------------------------------
    Function:   SYMUTIL_LoadLibrary

    Description:Checks and loads requested library.

    Author:     Danky Truong

    Inputs:     Library name.

    Returns:    Handle to the requested library or NULL
 -------------------------------------------------------------------------- */
HINSTANCE PASCAL SYMUTIL_LoadLibrary( LPSTR lpLibName)
{
    auto OFSTRUCT   ofstruct;
    HINSTANCE       hRetVal = NULL;

                                        // See if the file exists...
    if ( OpenFile ( lpLibName, &ofstruct, OF_EXIST) != HFILE_ERROR )
    {
                                        // Load the library...
        hRetVal = LoadLibrary ( lpLibName);

        if ( (WORD)hRetVal >= 32 )
            hRetVal = NULL;
    }

    return hRetVal;
}//SYMUTIL_LoadLibrary


BOOL SYM_EXPORT WINAPI MyShellLinkCreate(
    LPCSTR lpFolder,                    // Folder to append to 'special' folder.
    LPCSTR lpCmdLine,
    LPCSTR lpArguments,
    LPCSTR lpName,
    LPCSTR lpIconPath,
    int nIconIndex,
    int nShowCommand,
    LPCSTR lpDefDir,
    LPSTR lpSpecialFolder)
{
    //char        szPath[SYM_MAX_PATH];
    CWString    szPath(SYM_MAX_PATH*2); 

    szPath = lpSpecialFolder;
    szPath += "\\";
    szPath += lpFolder;
    szPath += "\\";
    szPath += lpName;
    szPath.AppendExtension(".LNK");

    // WES 08/01/97 Dynmically link OLE32 so we don't have the ole foot print for every application
    // we ship, just those that really need it.  This has been wrong for a bit over
    // two years now, whoops.
    HINSTANCE hLibrary = SYMUTIL_LoadLibrary( "OLE32.dll" );
    if ( !hLibrary )
    	return FALSE;

	typedef HRESULT (__stdcall * COCREATEINSTANCE_PROC)(REFCLSID rclsid, LPUNKNOWN pUnkOuter,DWORD dwClsContext, REFIID riid, LPVOID FAR* ppv);
	typedef HRESULT (__stdcall *  COINITIALIZE_PROC)(LPVOID pvReserved);
	typedef void (__stdcall *  COUNINITIALIZE_PROC)(void);
	typedef void (__stdcall * COFREEUNUSEDLIBRARIES_PROC)(void);
    COINITIALIZE_PROC    		CoInitialize = NULL;
    COCREATEINSTANCE_PROC   	CoCreateInstance   = NULL;
    COUNINITIALIZE_PROC     	CoUninitialize   = NULL;
    COFREEUNUSEDLIBRARIES_PROC  CoFreeUnusedLibraries = NULL;

                                        // Bind the function...
    CoInitialize = (COINITIALIZE_PROC) GetProcAddress( hLibrary, "CoInitialize");
    CoCreateInstance = (COCREATEINSTANCE_PROC) GetProcAddress( hLibrary, "CoCreateInstance");
    CoUninitialize = (COUNINITIALIZE_PROC) GetProcAddress( hLibrary, "CoUninitialize");
    CoFreeUnusedLibraries = (COFREEUNUSEDLIBRARIES_PROC) GetProcAddress( hLibrary, "CoFreeUnusedLibraries");
    
    if(!CoInitialize || !CoCreateInstance || !CoUninitialize || !CoFreeUnusedLibraries)
    {
	    FreeLibrary( hLibrary);
	    return FALSE;
	}
    	
    if(FAILED(CoInitialize(NULL)))
	{
        FreeLibrary( hLibrary);
        return FALSE;
    }

    LPSHELLLINK pShellLink;
    LPUNKNOWN pUnkOuter = NULL;
    HRESULT hr;

    if((FAILED(hr = CoCreateInstance(CLSID_ShellLink, pUnkOuter, CLSCTX_INPROC, IID_IShellLink, (LPVOID *)&pShellLink))))
    {
        CoUninitialize();
        FreeLibrary( hLibrary);
        return FALSE;
    }

    pShellLink->SetShowCmd(nShowCommand);
    pShellLink->SetPath(lpCmdLine);
    if(lpArguments)
        pShellLink->SetArguments(lpArguments);
    if(lpName)
        pShellLink->SetDescription(lpName);
    if(lpIconPath)
        pShellLink->SetIconLocation(lpIconPath,nIconIndex);
    if(lpDefDir)
        pShellLink->SetWorkingDirectory(lpDefDir);

    BOOL bReturn = FALSE;

    LPPERSISTFILE pFile;
    if(!FAILED(pShellLink->QueryInterface(IID_IPersistFile, (LPVOID *)&pFile)))
    {
        WCHAR szOlePath[SYM_MAX_PATH];
	    if(MultiByteToWideChar(CP_ACP,MB_PRECOMPOSED,szPath,-1,szOlePath,sizeof(szOlePath)))
            pFile->Save(szOlePath,FALSE);
        pFile->Release();
        bReturn = TRUE;
    }

    pShellLink->Release();

    CoFreeUnusedLibraries();
    CoUninitialize();
    FreeLibrary( hLibrary);

    return bReturn;
}




BOOL SYM_EXPORT WINAPI MyShellLinkBuildPath (
    LPCSTR lpFolder,                    // Folder to append to 'special' folder.
    int    nFolder,                     // Passed to SHGetSpecialFolderLocation
    LPCSTR lpCmdLine,
    LPCSTR lpName,
    LPSTR  lpOutput)
{
    CWString    szOutput(SYM_MAX_PATH);
    CWString    szName(SYM_MAX_PATH);
    CWString    szCmdLine(SYM_MAX_PATH);
    CWString    szFolder(SYM_MAX_PATH);
    TCHAR       szTemp[SYM_MAX_PATH];

    szOutput = lpOutput;

    // Give name precedence over command line
    if(lpName && *lpName)
        {
        lstrcpy(szTemp, lpName);
        szName = szTemp;
        }
    else if(lpCmdLine && *lpCmdLine)
        {
        lstrcpy(szTemp, lpCmdLine);
        szCmdLine = szTemp;
        szName = szCmdLine.ReturnFileName();
        }


    szName.StripExtension();

//    if(!szName.StrLen())
//        return FALSE;
/*
    if(lpFolder == NULL)
    {*/

    LPITEMIDLIST pidl;
    HINSTANCE hLibrary = NULL;

    hLibrary = SYMUTIL_LoadLibrary( "Shell32.dll");
    if ( hLibrary)
    {
        do
        {
            typedef BOOL ( CALLBACK* GETSPECIALFOLDERLOC_PROC) ( HWND, int, LPITEMIDLIST *);
            typedef BOOL ( CALLBACK* GETPATHFROMIDLIST_PROC) (  LPITEMIDLIST, LPSTR);
            GETSPECIALFOLDERLOC_PROC    GetSpecialFolderLoc = NULL;
            GETPATHFROMIDLIST_PROC      GetPathFromIDList   = NULL;

                                        // Bind the function...
            if (!(GetSpecialFolderLoc = (GETSPECIALFOLDERLOC_PROC)
                GetProcAddress( hLibrary, "SHGetSpecialFolderLocation")))
                break;
            if (!(GetPathFromIDList= (GETPATHFROMIDLIST_PROC)
                GetProcAddress( hLibrary, "SHGetPathFromIDList")))
                break;

            GetSpecialFolderLoc(NULL, nFolder, &pidl);
            GetPathFromIDList(pidl, szOutput);

        }while(FALSE);
        FreeLibrary( hLibrary);
    }
                                        // Use dynalink...Danky 03-28-95 03:51pm
    //SHGetSpecialFolderLocation( NULL, CSIDL_STARTUP, &pidl);
    //SHGetPathFromIDList(pidl, lpOutput);
//}
    /*else
        {
        lstrcpy(szTemp, lpFolder);
        szOutput = szTemp;
        }*/
    if(lpFolder && *lpFolder)
        {
        lstrcpy(szTemp, lpFolder);
        szFolder = szTemp;
        szOutput.AppendFileName(szFolder);
        }

    if(szName.StrLen())
        szOutput.AppendFileName(szName);

    lstrcpy(lpOutput, szOutput);

    return TRUE;
}


BOOL SYM_EXPORT WINAPI DirExists(LPCTSTR lpFileName)
{
    DWORD dwAttributes = GetFileAttributes(lpFileName);

    if (dwAttributes == 0xffffffff)
        return FALSE;
    else if (!(dwAttributes & FILE_ATTRIBUTE_DIRECTORY))
        return FALSE;
    else
        return TRUE;
}

/*
BOOL SYM_EXPORT WINAPI FileExists(LPCTSTR lpFileName)
{
    DWORD dwAttributes = GetFileAttributes(lpFileName);

    if (dwAttributes == 0xffffffff)
        return FALSE;
    else if (dwAttributes & FILE_ATTRIBUTE_DIRECTORY)
        return FALSE;
    else
        return TRUE;
}
*/


/*

BOOL SYM_EXPORT WINAPI MyShellLinkExists (
    LPCSTR lpFolder,                    // Folder to append to 'special' folder.
    int    nFolder,                     // Passed to SHGetSpecialFolderLocation
    LPCSTR lpCmdLine,                   // Required
    LPCSTR lpName)                      // Optional
{
    CWString        szPath1(SYM_MAX_PATH);
    CWString        szPath2(SYM_MAX_PATH);

    if(!MyShellLinkBuildPath(lpFolder,nFolder,lpCmdLine,lpName,szPath1))
        return FALSE;

    szPath2 = szPath1;
    szPath1.AppendExtension(".LNK");
    szPath2.AppendExtension(".PIF");

    if(FileExists(szPath1) || FileExists(szPath2))
        return TRUE;

    return FALSE;
}
*/
