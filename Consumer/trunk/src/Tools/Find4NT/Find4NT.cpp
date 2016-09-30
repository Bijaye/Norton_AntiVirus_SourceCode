////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#include <windows.h>
#include <commctrl.h>
#include <shlobj.h>
#include <tchar.h>
#include "resource.h"
#include "Find4NT.h"


HINSTANCE g_hInst;

//entry point
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
				   LPSTR lpszCmdLine, int nCmdShow)
{
	
	TCHAR szIniName[MAX_PATH] = {0}; //path to the optional .ini file
	TCHAR szSection[MAX_PATH] = _T("Path");			  //section needed in the .ini file
	TCHAR szPath[MAX_PATH] = _T("Path");			  //item needed in above section of .ini to contain path to 4NT
	TCHAR szCurDir[MAX_PATH] = {0};					 
	TCHAR szFileLoc[MAX_PATH] = {0};				  
	BOOL bExists = FALSE;			
	BOOL bIsFound = FALSE;
	HKEY hkResult;
	CHAR *pdest = NULL;
	INT iCh = '.';

	g_hInst = hInstance;
	
	//find out where the .ini file will be sitting...
	GetModuleFileName(hInstance, szIniName, sizeof(szIniName));
	
	pdest = strrchr( szIniName, iCh);
    *(pdest+1) = 'i';
	*(pdest+2) = 'n';
	*(pdest+3) = 'i';
  
	if (GetFileAttributes(szIniName) != -1)			 //check to see if an .ini file exists.
				bExists = TRUE;
	
	if (bExists)		
	{
		//get the value in the .ini file.
		GetPrivateProfileString(szSection, szPath, NULL, szCurDir, sizeof(szCurDir), szIniName);
		//check to ensure that .exe file is where is should be.
		wsprintf(szFileLoc, "%s\\4NT.EXE", szCurDir);
		  if (GetFileAttributes(szFileLoc) != -1)
		  {
			  bIsFound = TRUE;
			  wsprintf(szCurDir, "%s\\4NT.EXE", szCurDir);
		  }
		wsprintf(szFileLoc, "%s\\4DOS.COM", szCurDir);
		  if (GetFileAttributes(szFileLoc) != -1)
		  {
			  bIsFound = TRUE;
			  wsprintf(szCurDir, "%s\\4DOS.COM", szCurDir);
		  }
	}

	//if the .exe file was found properly added the environment variable and exit.  there is no need
	//to prompt user for any reason.
	if (bIsFound)
	{
		RegOpenKeyEx(HKEY_CURRENT_USER, _T("Environment"), 0, KEY_WRITE, &hkResult);
		RegSetValueEx(hkResult, _T("NAVBuild4NT"), 0, REG_SZ, (BYTE*)szCurDir, sizeof(szCurDir));
		RegCloseKey(hkResult);
		PostMessage(HWND_BROADCAST, WM_SETTINGCHANGE, NULL, NULL);
	}

	
	//if the .ini did not exist or had errors, we need to prompt the user.
	if (!bExists || !bIsFound)
	{
		//tell the user that there was a problem with the .ini file
		if(bIsFound)
			MessageBox(NULL, "The was an error reading the path from Find4NT.ini. You will now be presented with a dialog box to fill it in manually.",
		        			 "NAV Build", MB_ICONINFORMATION | MB_OK);
	
		InitCommonControls();
		//launch app to find the file
		DialogBox(hInstance, _T("MAIN_DLG"), NULL, (DLGPROC) MainDlgProc);
	}

	return 0;
}




BOOL WINAPI MainDlgProc(HWND hWnd, UINT nMsg, WPARAM wParam, LPARAM lParam)
{
	TCHAR szCurDir[MAX_PATH] = {0};
	TCHAR szFileLoc[MAX_PATH] = {0};
	BOOL bIsFound = FALSE;
	HKEY hkResult;


	switch(nMsg)
	{
	case WM_INITDIALOG:
		//initialize the dialog box.
		SendMessage(hWnd, WM_SETICON, ICON_BIG, (long) LoadIcon(g_hInst, _T("MAIN_ICON")));
		return TRUE;

	case WM_CLOSE:
		//warn users that closing will cause a build failure.. give option not to quit.
		if (MessageBox(hWnd, "WARNING: Not selecting a valid path to 4NT or 4DOS will cause a build failure! Are you sure you want to Quit?", 
						 "WARNING!", MB_ICONWARNING | MB_YESNO | MB_DEFBUTTON2) == IDYES)
			DestroyWindow(hWnd);
		return TRUE;

	case WM_COMMAND:
        switch (wParam)
        {
        case IDCANCEL:
			//warn users that cancelling will cause a build failure, give option not to quit.
			if (MessageBox(hWnd, "WARNING: Not selecting a valid path to 4NT or 4DOS will cause a build failure! Are you sure you want to Quit?", 
							 "WARNING!", MB_ICONWARNING | MB_YESNO | MB_DEFBUTTON2) == IDYES)
				EndDialog(hWnd, 0);
            break;

        case IDC_BROWSE:
		    //if the user presses browse, pop up a folder tree browser.
			GetDlgItemText(hWnd, IDC_EDIT, szCurDir, MAX_PATH);
			if (BrowseForFolder(hWnd, _T("Select the folder containing 4DOS.COM/NT.EXE..."), szCurDir, szCurDir))
				SetDlgItemText(hWnd, IDC_EDIT, szCurDir);
           	break;
		
        case IDOK:
			//uptdate the folder location in the entry field when the user presses OK.
			GetDlgItemText(hWnd, IDC_EDIT, szCurDir, MAX_PATH);
			wsprintf(szFileLoc, "%s\\4NT.EXE", szCurDir);
			//make sure the file is located where the user told us it would be.
			if (GetFileAttributes(szFileLoc) != -1)
			{
				bIsFound = TRUE;
				wsprintf(szCurDir, "%s\\4NT.EXE", szCurDir);
			}
			wsprintf(szFileLoc, "%s\\4DOS.COM", szCurDir);
			if (GetFileAttributes(szFileLoc) != -1)
			{
				bIsFound = TRUE;
				wsprintf(szCurDir, "%s\\4DOS.COM", szCurDir);
			}
			
			//if the file could not be found, show the error and go back.
			if (!bIsFound)
				MessageBox(hWnd, "ERROR: The path you selected does not contain 4DOS.COM or 4NT.EXE. Please select another path.", 
								 "ERROR!",MB_ICONERROR);
			else
			{	
				//if the file was found correctly, set the environment variable and close the dialog.			
				RegOpenKeyEx(HKEY_CURRENT_USER, _T("Environment"), 0, KEY_WRITE, &hkResult);
				RegSetValueEx(hkResult, _T("NAVBuild4NT"), 0, REG_SZ, (BYTE*)szCurDir, sizeof(szCurDir));
				RegCloseKey(hkResult);
				PostMessage(HWND_BROADCAST, WM_SETTINGCHANGE, NULL, NULL);
							
				EndDialog(hWnd, 0);
			}
			break;
		}
		
		return TRUE;
	}
	return FALSE;
}



//=============================================================================
// BrowseForFolder(HWND, LPCSTR, LPTSTR, LPTSTR)
//
// 		hWnd			Parent window   
//		szDialogText	Dialog title text
//		szInitialDir	Default directory, NULL is OK
// 		szFolderName	Returning buffer (should be MAX_PATH in length)
//
// Returns: TRUE if sucessfull, FALSE if the user hits Cancel
//
//=============================================================================
BOOL BrowseForFolder(HWND hWnd, LPCSTR szDialogText, LPTSTR szInitialDir, LPTSTR szFolderName)
{
	BROWSEINFO		BrowseInfo;
	LPITEMIDLIST	pIDList;
	TCHAR			szBuffer[MAX_PATH];


	ZeroMemory(&BrowseInfo, sizeof(BROWSEINFO));

	BrowseInfo.hwndOwner = hWnd;
	BrowseInfo.pidlRoot = NULL;
	BrowseInfo.pszDisplayName = szBuffer;
	BrowseInfo.lpszTitle = szDialogText;
	BrowseInfo.ulFlags = BIF_RETURNONLYFSDIRS;
	BrowseInfo.lpfn = (BFFCALLBACK) BrowseForFolderCB;
	BrowseInfo.lParam = (LPARAM) szInitialDir;

	pIDList = SHBrowseForFolder(&BrowseInfo);

	if (pIDList)
	{
         if (SHGetPathFromIDList(pIDList, szFolderName))
			 return TRUE;
	}
	return FALSE;
}

int CALLBACK BrowseForFolderCB(HWND hWnd, UINT nMsg, LPARAM lParam, LPARAM lpData)
{
	if (nMsg == BFFM_INITIALIZED)
	{
		if (lpData)
         SendMessage(hWnd, BFFM_SETSELECTION, 1, lpData);
	}

	return 0;
}

