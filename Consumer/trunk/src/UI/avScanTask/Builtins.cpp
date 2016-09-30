////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

// Builtins.cpp : implementation file
//

#include "stdafx.h"
#include <vector>
#include <winioctl.h>
#include "ScanTask.h"
#include "Builtins.h"
#include "TaskWizard.h"
#include "isVersion.h"
#include "..\\avScanTaskRes\\resource.h"
#include "isSymTheme.h"
#include "ccSymModuleLifetimeMgrHelper.h"
#include "ccOSInfo.h"

/////////////////////////////////////////////////////////////////////////////
// Must define  _WIN32_WINNT = 0x0400 so that we can use the old style 
// of Microsoft File Open dialog in which places bar (my computer, 
// my documents, etc.) is not displayed. Note that OFN_EX_NOPLACESBAR 
// can not be used because we use OFN_ENABLEHOOK flag. 
// See Remarks of OPENFILENAME in MSDN

#ifdef _WIN32_WINNT	
#define _WIN32_WINNT	0x0400
#endif

#include <commdlg.h>
#include <cderr.h>

#undef _WIN32_WINNT
#define _WIN32_WINNT	0x0502
/////////////////////////////////////////////////////////////////////////////


using namespace avScanTask;

/////////////////////////////////////////////////////////////////////////////
// Need to declare this stuff, since we don't want to pull in Win9x DDK.

#ifndef VWIN32_DIOC_DOS_IOCTL
#define VWIN32_DIOC_DOS_IOCTL 1

typedef struct _DIOC_REGISTERS {
    DWORD reg_EBX;
    DWORD reg_EDX;
    DWORD reg_ECX;
    DWORD reg_EAX;
    DWORD reg_EDI;
    DWORD reg_ESI;
    DWORD reg_Flags;
}DIOC_REGISTERS, *PDIOC_REGISTERS;

#endif

// Intel x86 processor status fla
#define CARRY_FLAG  0x1

#pragma pack(1)
typedef struct _DOSDPB {
    BYTE    specialFunc;    // 
    BYTE    devType;        // 
    WORD    devAttr;        // 
    WORD    cCyl;           // number of cylinders
    BYTE    mediaType;      // 
    WORD    cbSec;          // Bytes per sector
    BYTE    secPerClus;     // Sectors per cluster
    WORD    cSecRes;        // Reserved sectors
    BYTE    cFAT;           // FATs
    WORD    cDir;           // Root Directory Entries
    WORD    cSec;           // Total number of sectors in image
    BYTE    bMedia;         // Media descriptor
    WORD    secPerFAT;      // Sectors per FAT
    WORD    secPerTrack;    // Sectors per track
    WORD    cHead;          // Heads
    DWORD   cSecHidden;     // Hidden sectors
    DWORD   cTotalSectors;  // Total sectors, if cbSec is zero
    BYTE    reserved[6];    // 
} DOSDPB, *PDOSDPB;
#pragma pack()



/////////////////////////////////////////////////////////////////////////////
// Function descriptions for this file:

// CScanTask Computer functions
HRESULT Computer_Load(avScanTask::IScanTask* pThis)
{
	HRESULT hr = S_OK;
	DWORD       dwDrives = ::GetLogicalDrives();

	// Find all drives on the system, putting the root path of each one in
	// the aszDrivesToScan array.

	for (TCHAR cDrive[] = _T("A:\\"); cDrive[0] <= 'Z'; cDrive[0]++, dwDrives >>= 1 )
	{
		if ( dwDrives & 1 )
		{
			UINT uType = ::GetDriveType(cDrive);

			// Skip any drives whose type can't be determined, or any
			// disconnected network drives.

			if (DRIVE_UNKNOWN != uType  &&  DRIVE_NO_ROOT_DIR != uType)
			{
				pThis->AddItem(typeDrive, subtypeDriveHD, cDrive[0], NULL, 0);
			}
		}
	}

	DWORD dwCount = 0;
	hr = pThis->GetDriveCount(dwCount);

	if(dwCount <= 0)
		hr = E_FAIL;

	return hr;
}

/////////////////////////////////////////////////////////////////////////////
// CScanTask HardDisks functions
HRESULT HardDisks_Load(avScanTask::IScanTask* pThis)
{
	HRESULT hr = S_OK;
	DWORD dwDrives = ::GetLogicalDrives();

	// Find all drives on the system, putting the root path of each one in
	// the aszDrivesToScan array.

	for (TCHAR cDrive[] = _T("A:\\"); cDrive[0] <= 'Z'; cDrive[0]++, dwDrives >>= 1 )
	{
		if ( dwDrives & 1 )
		{
			// Skip any drives whose type can't be determined, or any
			// disconnected network drives.

			if (DRIVE_FIXED == ::GetDriveType(cDrive))
			{
				pThis->AddItem(typeDrive, subtypeDriveHD, cDrive[0], NULL, 0);
			}
		}
	}

	DWORD dwCount = 0;
	hr = pThis->GetDriveCount(dwCount);

	if(dwCount <= 0)
		hr = E_FAIL;

	return hr;
}

/////////////////////////////////////////////////////////////////////////////
// CScanTask RemovableDrives functions
HRESULT RemovableDrives_Load(avScanTask::IScanTask* pThis)
{
	HRESULT hr = S_OK;
	DWORD dwDrives = ::GetLogicalDrives();

	// Find all drives on the system, putting the root path of each one in
	// the aszDrivesToScan array.

	for (TCHAR cDrive[] = _T("A:\\"); cDrive[0] <= 'Z'; cDrive[0]++, dwDrives >>= 1 )
	{
		if ( dwDrives & 1 )
		{
			UINT uType = ::GetDriveType(cDrive);

			// Skip any drives whose type can't be determined, or any
			// disconnected network drives.

			if (DRIVE_REMOVABLE == uType || DRIVE_CDROM == uType)
			{
				pThis->AddItem(typeDrive, subtypeDriveHD, cDrive[0], NULL, 0);
			}
		}
	}

	DWORD dwCount = 0;
	hr = pThis->GetDriveCount(dwCount);

	if(dwCount <= 0)
		hr = E_FAIL;

	return hr;
}

/////////////////////////////////////////////////////////////////////////////
// CScanTask Floppy functions
HRESULT Floppy_Load(avScanTask::IScanTask* pThis)
{
	HRESULT hr = E_FAIL;
    TCHAR  chDrive, chRet = 0;
    DWORD fdwLogicalDrives = GetLogicalDrives();
    DWORD  dwDriveBit;

    TCHAR  szDrive[] = _T("\\\\.\\x:");
    std::vector<DISK_GEOMETRY> vGeometry(20);
	HANDLE hDrive;
	DWORD dwBytesReturned;
	DWORD dwValidCount;

    for ( chDrive = _T('A'), dwDriveBit = 1;
		    chDrive <= _T('Z') && chRet == 0; chDrive++, dwDriveBit <<= 1 )
    {
        // If the bit is set, query the drive.
        if ( 0 != ( fdwLogicalDrives & dwDriveBit ) )
        {
			// Open the drive.
			szDrive[4] = chDrive;
			hDrive = CreateFile( szDrive, 0, FILE_SHARE_READ, NULL, OPEN_ALWAYS, 0, NULL ); 
			if( hDrive == INVALID_HANDLE_VALUE )
				continue;

			// Get drive geometry from drive.
			if( DeviceIoControl( hDrive, IOCTL_DISK_GET_MEDIA_TYPES, NULL,
						    0, &vGeometry[0], sizeof(DISK_GEOMETRY) * 20, &dwBytesReturned, NULL ) )
			{
				// Compute number of valid DISK_GEOMETRY structures returned.
				dwValidCount = dwBytesReturned / sizeof(DISK_GEOMETRY);

				// Look for floppy geometry type.
				for( DWORD i = 0; i < dwValidCount; ++i )
				{
					switch ( vGeometry[i].MediaType ) 
					{
					case F5_1Pt2_512:	// 5.25, 1.2MB,  512 bytes/sector
					case F3_1Pt44_512:	// 3.5,  1.44MB, 512 bytes/sector
					case F3_2Pt88_512:	// 3.5,  2.88MB, 512 bytes/sector
					case F3_20Pt8_512:	// 3.5,  20.8MB, 512 bytes/sector
					case F3_720_512:	// 3.5,  720KB,  512 bytes/sector
					case F5_360_512:	// 5.25, 360KB,  512 bytes/sector
					case F5_320_512:	// 5.25, 320KB,  512 bytes/sector
					case F5_320_1024:	// 5.25, 320KB,  1024 bytes/sector
					case F5_180_512:	// 5.25, 180KB,  512 bytes/sector
					case F5_160_512:	// 5.25, 160KB,  512 bytes/sector
	                    {
		                    pThis->AddItem(typeDrive, subtypeDriveRemovable, chDrive, NULL, 0);
                            i = dwValidCount;
                            hr = S_OK;
	                    }
                        break;
					}
				}
			}

			// Clean up.
			CloseHandle( hDrive );
        }  
    }

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CScanTask Drives functions
HRESULT Drives_Load(avScanTask::IScanTask* pScanTask)
{
	STAHLSOFT_HRX_TRY(hr)
	{
		CTaskWizard TW;
		hrx << TW.DrivesDialog(pScanTask);

		DWORD dwCount = 0;
		hrx << pScanTask->GetDriveCount(dwCount);
		CCTRCTXI1(L"DriveCount: %d", dwCount);

		if(dwCount == 0)
		{
			CCTRCTXI0(L"User cancels out of the Scan drives dialog");
			hrx << E_FAIL;
		}
	}
	STAHLSOFT_HRX_CATCH_ALL_NO_RETURN(hr)
	return hr;
}

/////////////////////////////////////////////////////////////////////////////
// CScanTask Folders functions
HRESULT Folders_Load(avScanTask::IScanTask* pScanTask)
{
	STAHLSOFT_HRX_TRY(hr)
	{
		CTaskWizard TW;
		hrx << TW.FolderDialog(pScanTask);

		DWORD dwCount = 0;
		hrx << pScanTask->GetFolderCount(dwCount);

		CCTRCTXI1(L"Folder: %d", dwCount);

		if(dwCount == 0)
		{
			CCTRCTXI0(L"User cancels out of the Scan folders dialog");
			hrx << E_FAIL;
		}
	}
	STAHLSOFT_HRX_CATCH_ALL_NO_RETURN(hr)
	return hr;
}

//-------------------------------------------------------------------------
UINT_PTR CALLBACK OFNHookProc(HWND hDlg, UINT uiMsg, WPARAM wParam, LPARAM lParam)
{
	if (WM_INITDIALOG == uiMsg)
	{
		// Center the dialog
		RECT rcScreen = {0}, rcDialog = {0};
		hDlg = ::GetParent(hDlg);
		HWND hWndDesktop = GetDesktopWindow();

		::GetWindowRect(hWndDesktop, &rcScreen);
		::GetClientRect(hDlg, &rcDialog);

		POINT pt = {0};
		pt.x = (rcScreen.right - rcScreen.left)/2 - (rcDialog.right - rcDialog.left)/2;
		pt.y = (rcScreen.bottom - rcScreen.top)/2 - (rcDialog.bottom - rcDialog.top)/2;

		ClientToScreen(hWndDesktop, &pt);
		SetWindowPos(hDlg, NULL, pt.x, pt.y, 0, 0, SWP_NOSIZE);

		// Set OK button text to "Scan"
		ccLib::CStringW sText;
		sText.LoadString(IDS_SCAN);
		SetDlgItemText(hDlg, IDOK, sText.GetString());

		sText.LoadString(IDS_CANCEL);
		SetDlgItemText(hDlg, IDCANCEL, sText.GetString());

		return 1;
	}

	return 0;
}


/////////////////////////////////////////////////////////////////////////////
// CScanTask Files functions
HRESULT Files_Load(avScanTask::IScanTask* pScanTask)
{
	STAHLSOFT_HRX_TRY(hr)
	{
		WCHAR szFiles[MAX_PATH * 50] = {0};
		ccLib::CStringW sTitle;
		ccLib::CStringW sFilter;

		sTitle.LoadString(IDS_SCANFILES_TITLE);
		sFilter.LoadString(IDS_SCANFILES_FILTER);

		// Since we can't save a string with nil ('\0') embedAded we use the sharp (#)
		// as a place holder and replace it with the nils.
		sFilter.Replace(L'#', L'\0');

		OPENFILENAME ofn = { sizeof(OPENFILENAME) // lStructSize;
			, ::GetDesktopWindow() // hwndOwner; 
			, NULL                 // hInstance; 
			, sFilter             // lpstrFilter; 
			, NULL                 // lpstrCustomFilter; 
			, 0                    // nMaxCustFilter; 
			, 0                    // nFilterIndex; 
			, szFiles              // lpstrFile; 
			, sizeof(szFiles)/sizeof(WCHAR)      // nMaxFile; 
			, NULL                 // lpstrFileTitle; 
			, 0                    // nMaxFileTitle; 
			, L"C:\\"           // lpstrInitialDir; 
			, sTitle              // lpstrTitle; 
			, OFN_ALLOWMULTISELECT // Flags;
			| OFN_ENABLESIZING
			| OFN_FILEMUSTEXIST
			| OFN_PATHMUSTEXIST
			| OFN_EXPLORER
			| OFN_HIDEREADONLY
			| OFN_ENABLEHOOK
		};

		ofn.lpfnHook = OFNHookProc;  // Set the hook for centering the dialog

		// Apply Symtheme
		// Create SymTheme Skin window.
		// Do not bail out if failed.
		// Should continue to display our Scan window
		HRESULT hr1 = S_OK;
		CISSymTheme isSymTheme;
		hr1 = isSymTheme.Initialize(GetModuleHandle(NULL));
		if(FAILED(hr1))
		{
			CCTRCTXE1(L"CISSymTheme::Initialize() failed. Error: 0x%08X", hr1);
		}

		// Make sure the user selects only as many files that can fit in the buffer

		// I experience a strange scenario in which GetOpenFileName returns different error code for different OS's:
		// For the scenario in which users select 200 files:
		// GetOpenFileName() returns FALSE fox XP, and TRUE for Vista.
		// And CommDlgExtendedError returns FNERR_BUFFERTOOSMALL for both OS.
		// To handle the difference, I will have to check for OS version

		BOOL bOK = FALSE;
		DWORD dwRet = 0;

		if(ccLib::COSInfo::IsWinVista(true))
		{
			for(; (0 != (bOK = ::GetOpenFileName(&ofn)))
				&& (FNERR_BUFFERTOOSMALL == (dwRet = ::CommDlgExtendedError()));)
			{
				CCTRCTXI1(L"GetOpenFileName returns: %d", bOK);
				CCTRCTXI1(L"CommDlgExtendedError returns: 0x%08x", dwRet);

				ccLib::CStringW sMsg;
				sMsg.LoadString(IDS_ERR_TOOMANYFILES);

				MessageBox(::GetDesktopWindow(), sMsg, CISVersion::GetProductName(), MB_OK);

				*ofn.lpstrFile = L'\0';
			}
		}
		else
		{
			for(; (0 == (bOK = ::GetOpenFileName(&ofn)))
				&& (FNERR_BUFFERTOOSMALL == (dwRet = ::CommDlgExtendedError()));)
			{
				CCTRCTXI1(L"GetOpenFileName returns: %d", bOK);
				CCTRCTXI1(L"CommDlgExtendedError returns: 0x%08x", dwRet);

				ccLib::CStringW sMsg;
				sMsg.LoadString(IDS_ERR_TOOMANYFILES);

				MessageBox(::GetDesktopWindow(), sMsg, CISVersion::GetProductName(), MB_OK);

				*ofn.lpstrFile = L'\0';
			}

		}


		if(!bOK)
		{
			CCTRCTXI0(L"User cancels out of the Scan files dialog");
			hrx << E_FAIL;
		}
		else
		{
			DWORD dwFileAttribs = ::GetFileAttributes(szFiles);

			if((-1 == dwFileAttribs) || 
				(FILE_ATTRIBUTE_DIRECTORY != (FILE_ATTRIBUTE_DIRECTORY & dwFileAttribs)))
			{
				pScanTask->AddItem(typeFile, subtypeFile, NULL, szFiles, 0);
				return S_OK;
			}

			// Get the directory which is stored as the first string in this multiple strings.
			ccLib::CStringW sPath(szFiles);

			// if the directory name does not end with '\\' append a backslash
			PathAddBackslash(sPath.GetBuffer());
			sPath.ReleaseBuffer();

			ccLib::CStringW sFile;

			// Walk the list of files selected
			for (PWCHAR pszFile = szFiles; *(pszFile += ::_tcslen(pszFile) + 1); )
			{
				// Append the next file name to the base path
				sFile.Format(L"%s%s", sPath.GetString(), pszFile);

				CCTRCTXI1(L"Add %s", sFile.GetString());
		
				pScanTask->AddItem(typeFile, subtypeFile, NULL, sFile, 0);
			}

			DWORD dwCount = 0;
			hrx << pScanTask->GetFileCount(dwCount);
				CCTRCTXI1(L"File: %d", dwCount);

			if(dwCount == 0)
			{
				hrx << E_FAIL;
			}
		}
	}
	STAHLSOFT_HRX_CATCH_ALL_NO_RETURN(hr)
	return hr;
}
