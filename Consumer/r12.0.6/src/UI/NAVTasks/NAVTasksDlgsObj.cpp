// NAVTasksDlgsObj.cpp : Implementation of CNAVTasksDlgs
#include "stdafx.h"
#include <commdlg.h>
#include "Resource.h"
#include "..\NavTasksRes\ResResource.h"
#include "NAVTasks.h"
#include "ScanTask.h"
#include "NAVTasksDlgsObj.h"
#include "TaskWizard.h"
#include "ccResourceLoader.h"

// Resource Loader
extern cc::CResourceLoader g_ResLoader;

/////////////////////////////////////////////////////////////////////////////
// CNAVTasksDlgs

STDMETHODIMP CNAVTasksDlgs::ShowFolderListDlg(HWND hWnd, VARIANT vScanTask, ESFLDO eShowFolderListDlgOptions)
{
	if (VT_BYREF != vScanTask.vt)
    {
        CCTRACEI ( "Invalid arg in ShowFolderListDlg");
        return E_INVALIDARG;
    }

    // Call into NAVTaskWizard, this returns S_OK or S_FALSE for cancel
    //
    CTaskWizard TW;
	return TW.FolderDialog(reinterpret_cast<IScanTask*>(vScanTask.byref));
}

STDMETHODIMP CNAVTasksDlgs::ShowDriveListDlg(VARIANT vScanTask)
{
	if (VT_BYREF != vScanTask.vt)
    {
        CCTRACEI ( "Invalid arg in ShowDriveListDlg");
        return E_INVALIDARG;
    }

    // Call into NAVTaskWizard, this returns S_OK or S_FALSE for cancel
    //
    CTaskWizard TW;
	return TW.DrivesDialog(reinterpret_cast<IScanTask*>(vScanTask.byref));
}

STDMETHODIMP CNAVTasksDlgs::ShowFileListDlg(VARIANT vScanTask)
{
	TCHAR szFiles[MAX_PATH * 50] = { _T('\0') }
	    , szTitle[128]
	    , szFilter[128];
	IScanTask* m_pScanTask = reinterpret_cast<IScanTask*>(vScanTask.byref);
	OPENFILENAME ofn = { sizeof(OPENFILENAME) // lStructSize;
	                   , ::GetDesktopWindow() // hwndOwner; 
	                   , NULL                 // hInstance; 
	                   , szFilter             // lpstrFilter; 
	                   , NULL                 // lpstrCustomFilter; 
	                   , 0                    // nMaxCustFilter; 
	                   , 0                    // nFilterIndex; 
	                   , szFiles              // lpstrFile; 
	                   , SIZEOF(szFiles)      // nMaxFile; 
	                   , NULL                 // lpstrFileTitle; 
	                   , 0                    // nMaxFileTitle; 
	                   , _T("C:\\")           // lpstrInitialDir; 
	                   , szTitle              // lpstrTitle; 
	                   , OFN_ALLOWMULTISELECT // Flags;
	                   | OFN_ENABLESIZING
	                   | OFN_FILEMUSTEXIST
	                   | OFN_PATHMUSTEXIST
	                   | OFN_EXPLORER
	                   | OFN_HIDEREADONLY
	                   | OFN_ENABLEHOOK
	};

	ofn.lpfnHook = OFNHookProc;  // Set the hook for centering the dialog

	// For localization
	g_ResLoader.Initialize();
	::LoadString(g_ResLoader.GetResourceInstance(), IDS_SCANFILES_TITLE,  szTitle,  SIZEOF(szTitle));
	::LoadString(g_ResLoader.GetResourceInstance(), IDS_SCANFILES_FILTER, szFilter, SIZEOF(szFilter));

	// Since we can't save a string with nil ('\0') embeded we use the sharp (#)
	// as a place holder and replace it with the nils.
	*::_tcsrchr(szFilter, _T('#')) = _T('\0');
	*::_tcschr(szFilter, _T('#')) = _T('\0');

	// Make sure the user selects only as many files that can fit in the buffer
	for (BOOL bOK; 0 == (bOK = ::GetOpenFileName(&ofn))
	            && FNERR_BUFFERTOOSMALL == ::CommDlgExtendedError(); )
	{
		TCHAR szMsg[128], szCaption[128];

		::LoadString(g_ResLoader.GetResourceInstance(), IDS_Err_TooManyFiles, szMsg, SIZEOF(szMsg));
		::LoadString(g_ResLoader.GetResourceInstance(), IDS_NAME, szCaption, SIZEOF(szCaption));
		::MessageBox(::GetDesktopWindow(), szMsg, szCaption, MB_OK);

		*ofn.lpstrFile = _T('\0');
	}

	if (bOK)
	{
		TCHAR szPath[MAX_PATH];

        DWORD dwFileAttribs = ::GetFileAttributes(szFiles);

		if (( dwFileAttribs == -1 ) || (FILE_ATTRIBUTE_DIRECTORY != (FILE_ATTRIBUTE_DIRECTORY & dwFileAttribs )))
		{
			m_pScanTask->AddItem(typeFile, subtypeFile, NULL, szFiles, 0);

			return S_OK;
		}

		// Save a pointer to the location where the file name should begin
		PTCHAR pszBase = ::_tcscpy(szPath, szFiles) + ::_tcslen(szPath);

		// if the directory name does not end with '\\' append a backslash
		if (_T('\\') != pszBase[-1])
			*pszBase++ = _T('\\');

		// Walk the list of files selected
		for (PTCHAR pszFile = szFiles; *(pszFile += ::_tcslen(pszFile) + 1); )
		{
			// Append the next file name to the base path
			::_tcscpy(pszBase, pszFile);
			m_pScanTask->AddItem(typeFile, subtypeFile, NULL, szPath, 0);
		}

		return S_OK;
	}

	return S_FALSE;
}

UINT_PTR CALLBACK CNAVTasksDlgs::OFNHookProc(HWND hDlg, UINT uiMsg, WPARAM wParam, LPARAM lParam)
{
	// Center the dialog
	if (WM_INITDIALOG == uiMsg)
	{
		RECT rcScreen, rcDialog;
		hDlg = ::GetParent(hDlg);

		::GetWindowRect(::GetDesktopWindow(), &rcScreen);
		::GetClientRect(hDlg, &rcDialog);

		::MoveWindow(hDlg, (rcScreen.right - rcDialog.right) / 2
		                 , (rcScreen.bottom  - rcDialog.bottom) / 2
		                 , rcDialog.right, rcDialog.bottom, TRUE);

		return 1;
	}

	return 0;
}

