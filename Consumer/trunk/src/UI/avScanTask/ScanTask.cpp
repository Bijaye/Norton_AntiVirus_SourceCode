////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

// ScanTask.cpp : implementation file
//
#include "Stdafx.h"
#include <time.h>
#include <Mstask.h>
#include <math.h>           // For pow()
#include "ScanTask.h"
#include "ScanTaskDefines.h"
#include "ScanTaskLoader.h"
#include "Builtins.h"
#include "NAVInfo.h"
#include "GlobalEvent.h"
#include "GlobalEvents.h"
#include "HNAVOPTS32.h"
#include "ccOSInfo.h"
#include "ccSingleLock.h"
#include "ccSymStringImpl.h"
#include "TaskWizard.h"
#include "Switches.h"
#include "isVersion.h"
#include "..\\avScanTaskRes\\Resource.h"
#include "SMHelper.h"   // For saving FSS time to ccSettings
#include "SharedStatusKeys.h"
#include "Optnames.h"
#include "NAVSettingsHelperEx.h"
#include "isSymTheme.h"
#include <Lmcons.h>
#include <AccCtrl.h>
#include <Aclapi.h>

using namespace avScanTask;

//-------------------------------------------------------------------------
CScanTask::CScanTask()
{
	m_sProductName = CISVersion::GetProductName();
	reset();
}
//-------------------------------------------------------------------------
CScanTask::~CScanTask()
{
	Empty();
}
//-------------------------------------------------------------------------
HRESULT CScanTask::AddItem(avScanTask::SCANITEMTYPE type, avScanTask::SCANITEMSUBTYPE subtype, 
						   WCHAR chDrive, LPCWSTR szPath, ULONG ulVID) throw()
{
	CCTRCTXI0(L"Enter");
	STAHLSOFT_HRX_TRY(hr)
	{
		ccLib::CSingleLock csl(&m_Crit, INFINITE, FALSE);

		CScanItem item;
		item.SetItemType(type, subtype);

		if(type == typeDrive)
		{
			CCTRCTXI1(L"chDrive: %c", chDrive);
			hrx << (item.SetDrive(chDrive)? S_OK : E_FAIL);
		}
		else if(szPath)
		{
			CCTRCTXI1(L"szPath: %s", szPath);
			hrx << (item.SetPath(szPath)? S_OK : E_FAIL);
		}

		if(typeFileVID == type || typeFileVIDCompressed == type)
			hrx << (item.SetVID(ulVID)? S_OK : E_FAIL);

		m_Items.push_back(item);

		switch(type)
		{
		case typeDrive:
			m_uDriveCnt++;
			break;
		case typeFolder:
			m_uFolderCnt++;
			break;
		case typeFile:
			m_uFileCnt++;
			break;
		case typeFileVID:
		case typeFileVIDCompressed:
			m_uFileVIDCnt++;
			break;
		}
	}
	STAHLSOFT_HRX_CATCH_ALL_NO_RETURN(hr)
		CCTRCTXI1(L"Exit. HR: 0x%08X", hr);
	return hr;
}
//-------------------------------------------------------------------------
// Always call EnumReset() BEFORE using EnumNext() for the first time!!
//-------------------------------------------------------------------------
HRESULT CScanTask::EnumReset() throw()
{
	m_Iterator = m_Items.begin();
	return S_OK;
}
//-------------------------------------------------------------------------
HRESULT CScanTask::EnumNext(avScanTask::SCANITEMTYPE &type, avScanTask::SCANITEMSUBTYPE &subtype, 
							WCHAR &chDrive, cc::IString** pPath, ULONG& ulVID) throw()
{
	CCTRCTXI0(L"Enter");

	ccLib::CExceptionInfo exceptionInfo;

	try
	{
		ccLib::CSingleLock csl(&m_Crit, INFINITE, FALSE);

		// If the list is at the end, or empty, just return.
		if (m_Iterator == m_Items.end() || m_Items.empty())
			return E_FAIL;

		m_Iterator->GetItemType(type, subtype);

		if(typeDrive == type)
		{
			chDrive = m_Iterator->GetDrive();
		}
		else 
		{
			*pPath = new ccSym::CStringImpl(m_Iterator->GetPath());
			(*pPath)->AddRef();
		}

		// If this is a filename\VID item also give the VID back
		if(typeFileVID == type || typeFileVIDCompressed == type)
			ulVID = m_Iterator->GetVID();

		m_Iterator++;
	}
	CCCATCHMEM(exceptionInfo)

		if(exceptionInfo.IsException())
		{
			CCTRCTXI0(L"Exception");
			return E_FAIL;
		}
		else
		{
			return S_OK;
		}
}
//-------------------------------------------------------------------------
HRESULT CScanTask::GetType(avScanTask::SCANTASKTYPE& type) throw()
{
	ccLib::CSingleLock csl(&m_Crit, INFINITE, FALSE);
	type = m_Type;
	return S_OK;
}
//-------------------------------------------------------------------------
HRESULT CScanTask::SetType(avScanTask::SCANTASKTYPE type) throw()
{
	ccLib::CSingleLock csl(&m_Crit, INFINITE, FALSE);
	m_Type = type;
	return S_OK;
}
//-------------------------------------------------------------------------
HRESULT CScanTask::GetItemCount(DWORD& dwCount) throw()
{
	ccLib::CSingleLock csl(&m_Crit, INFINITE, FALSE);
	dwCount = m_uDriveCnt + m_uFolderCnt + m_uFileCnt + m_uFileVIDCnt;
	return S_OK;
}
//-------------------------------------------------------------------------
HRESULT CScanTask::GetDriveCount(DWORD& dwCount) throw()
{
	ccLib::CSingleLock csl(&m_Crit, INFINITE, FALSE);
	dwCount = m_uDriveCnt;
	return S_OK;
}
//-------------------------------------------------------------------------
HRESULT CScanTask::GetFolderCount(DWORD& dwCount) throw()
{
	ccLib::CSingleLock csl(&m_Crit, INFINITE, FALSE);
	dwCount = m_uFolderCnt;
	return S_OK;
}
//-------------------------------------------------------------------------
HRESULT CScanTask::GetFileCount(DWORD& dwCount) throw()
{
	ccLib::CSingleLock csl(&m_Crit, INFINITE, FALSE);
	dwCount = m_uFileCnt;
	return S_OK;
}
//-------------------------------------------------------------------------
HRESULT CScanTask::GetFileVIDCount(DWORD& dwCount) throw()
{
	ccLib::CSingleLock csl(&m_Crit, INFINITE, FALSE);
	dwCount = m_uFileVIDCnt;
	return S_OK;
}
//-------------------------------------------------------------------------
HRESULT CScanTask::GetLastRunTime(time_t& time) throw()
{
	ccLib::CSingleLock csl(&m_Crit, INFINITE, FALSE);
	CopyMemory(&time, &m_timeLastRun, sizeof(time_t));
	return S_OK;
}
//-------------------------------------------------------------------------
HRESULT CScanTask::SetLastRunTime(const time_t& time) throw()
{
	ccLib::CSingleLock csl(&m_Crit, INFINITE, FALSE);
	CopyMemory(&m_timeLastRun, &time, sizeof(time_t));
	return S_OK;
}
//-------------------------------------------------------------------------
HRESULT CScanTask::GetName(cc::IString** pName) throw()
{
	ccLib::CExceptionInfo exceptionInfo;
	try
	{
		ccLib::CSingleLock csl(&m_Crit, INFINITE, FALSE);
		*pName = new ccSym::CStringImpl(m_sName);
		(*pName)->AddRef();
	}
	CCCATCHMEM(exceptionInfo);
	return (exceptionInfo.IsException()? E_FAIL : S_OK);
}
//-------------------------------------------------------------------------
HRESULT CScanTask::SetName(LPCWSTR szName) throw()
{
	if(!szName)
		return E_POINTER;

	ccLib::CSingleLock csl(&m_Crit, INFINITE, FALSE);
	m_sName = szName;
	return S_OK;
}
//-------------------------------------------------------------------------
HRESULT CScanTask::SetPath(LPCWSTR szPath) throw()
{
	if(!szPath)
		return E_POINTER;

	ccLib::CSingleLock csl(&m_Crit, INFINITE, FALSE);
	m_sPath = szPath;
	return S_OK;
}
//-------------------------------------------------------------------------
HRESULT CScanTask::GetPath(cc::IString** pPath) throw()
{
	ccLib::CExceptionInfo exceptionInfo;
	try
	{
		ccLib::CSingleLock csl(&m_Crit, INFINITE, FALSE);
		*pPath = new ccSym::CStringImpl(m_sPath);
		(*pPath)->AddRef();
	}
	CCCATCHMEM(exceptionInfo);
	return (exceptionInfo.IsException()? E_FAIL : S_OK);
}

//-------------------------------------------------------------------------
HRESULT CScanTask::GetCanSchedule(bool& bCanSchedule) throw()
{
	ccLib::CSingleLock csl(&m_Crit, INFINITE, FALSE);
	bCanSchedule = m_bCanSchedule;
	return S_OK;
}

//-------------------------------------------------------------------------
HRESULT CScanTask::SetCanSchedule(bool bCanSchedule) throw()
{
	ccLib::CSingleLock csl(&m_Crit, INFINITE, FALSE);
	m_bCanSchedule = bCanSchedule;
	return S_OK;
}

//-------------------------------------------------------------------------
HRESULT CScanTask::GetCanEdit(bool& bCanEdit) throw()
{
	ccLib::CSingleLock csl(&m_Crit, INFINITE, FALSE);
	bCanEdit = m_bCanEdit;
	return S_OK;
}

//-------------------------------------------------------------------------
HRESULT CScanTask::SetCanEdit(bool bCanEdit) throw()
{
	ccLib::CSingleLock csl(&m_Crit, INFINITE, FALSE);
	m_bCanEdit = bCanEdit;
	return S_OK;
}
//-------------------------------------------------------------------------
HRESULT CScanTask::IsEmpty(bool& bEmpty) throw()
{
	ccLib::CSingleLock csl(&m_Crit, INFINITE, FALSE);
	bEmpty = m_Items.empty();
	return S_OK;
}
//-------------------------------------------------------------------------
HRESULT CScanTask::Empty() throw()
{
	ccLib::CSingleLock csl(&m_Crit, INFINITE, FALSE);
	m_Items.clear();
	m_uDriveCnt = m_uFolderCnt = m_uFileCnt = m_uFileVIDCnt = 0;
	return S_OK;
}

//-------------------------------------------------------------------------
HRESULT CScanTask::Save(void) throw()
{
	if(!m_sPath.GetLength() || scanUnset == m_Type)
	{
		CCTRCTXE0(L"Invalid path or unset scan type");
		return E_FAIL;
	}

	ccLib::CSingleLock csl(&m_Crit, INFINITE, FALSE);

	CCTRCTXI0(L"Enter");

	STAHLSOFT_HRX_TRY(hr)
	{
		CHNAVOPTS32 NavOpts;
		hrx << NAVOPT_SUCCEEDED(NavOpts.Allocate());

		// Write the task name as binary data
		// because NavOpts support only ANSI string, not UNICODE string.
		LPBYTE lpBuffer = (LPBYTE)(m_sName.GetString());
		DWORD dwBufferSize = m_sName.GetLength() * sizeof(WCHAR);
		hrx << NAVOPT_SUCCEEDED(NavOpts.SetBinaryValue(szOptName, lpBuffer, dwBufferSize));

		// Write the version number.
		hrx << NAVOPT_SUCCEEDED(NavOpts.SetDwordValue(szOptVersionMajor, m_wVersionMajor));
		hrx << NAVOPT_SUCCEEDED(NavOpts.SetDwordValue(szOptVersionMinor, m_wVersionMinor));
		hrx << NAVOPT_SUCCEEDED(NavOpts.SetDwordValue(szOptVersionInLine, m_wVersionInline));

		// Write scan type.
		hrx << NAVOPT_SUCCEEDED(NavOpts.SetDwordValue(szOptType, m_Type));

		// Write the time the task was last run.
		hrx << NAVOPT_SUCCEEDED(NavOpts.SetDwordValue(szOptLastRun, static_cast<DWORD>(m_timeLastRun)));

		// Write the bCanEdit
		hrx << NAVOPT_SUCCEEDED(NavOpts.SetDwordValue(szOptCanEdit, m_bCanEdit));

		// Write the bCanSchedule
		hrx << NAVOPT_SUCCEEDED(NavOpts.SetDwordValue(szOptCanSchedule, m_bCanSchedule));

		// If this is a built-in task, we're done.
		if (scanCustom != m_Type)
		{
			hrx << NAVOPT_SUCCEEDED(NavOpts.Save(m_sPath));

			if ( m_Type == scanQuick )
			{
				// Save date into ccSettings, also triggering an event.
				//
				CSMHelper SMHelper;

				SYMRESULT sr = SMHelper.Load(ISShared::NAVNIS_Status_QuickScan_Key);
				if(SYM_FAILED(sr))
				{
					// No cache data
					CCTRCTXE1(L"Failed to set settings 0x%x", sr);
				}
				else
				{
					CCTRCTXI1(L"saving FSS time %d", m_timeLastRun);
					sr = SMHelper.GetSettings()->PutDword(ISShared::NAVNIS_Status_QuickScan_DateTime, 
						(DWORD) m_timeLastRun); // In 64-bit this won't work.
					if(SYM_SUCCEEDED(sr))
					{
						// send events
						if(!SMHelper.Save (ISShared::NAVNIS_Status_QuickScan_Key, true))
						{
							CCTRCTXI0(L"Failed to save ISShared::NAVNIS_Status_QuickScan_Key");
						}
					}
				}
			}

			if(scanMyComputer == m_Type)
			{
				// If this is the My Computer scan we need to announce that
				// it is being changed for status.
				//
				CGlobalEvent eventMyComputer;
				if(eventMyComputer.CreateW(SYM_REFRESH_FULLSYSTEMSCAN_EVENT))
					PulseEvent(eventMyComputer);

				// Save date into ccSettings, also triggering an event.
				//
				{
					CSMHelper SMHelper;

					SYMRESULT sr = SMHelper.Load(ISShared::NAVNIS_Status_FSS_Key);
					if(SYM_FAILED(sr))
					{
						// No cache data
						CCTRCTXE1(L"Failed to set settings 0x%x", sr);
					}
					else
					{
						CCTRCTXI1(L"saving FSS time %d", m_timeLastRun );
						sr = SMHelper.GetSettings()->PutDword(ISShared::NAVNIS_Status_FSS_DateTime, 
							(DWORD) m_timeLastRun); // In 64-bit this won't work.
						if(SYM_SUCCEEDED(sr))
						{
							// send events
							if(!SMHelper.Save(ISShared::NAVNIS_Status_FSS_Key, true))
							{
								CCTRCTXE0(L"Failed to save ISShared::NAVNIS_Status_FSS_Key failed");
							}
						}
					}
				}

				CRegKey key;
				LONG rc = 0;

				rc = key.Create(HKEY_LOCAL_MACHINE,
					L"SOFTWARE\\Symantec\\Norton AntiVirus\\LastScan",
					0, 0, KEY_QUERY_VALUE|KEY_WRITE);
				if(ERROR_SUCCESS == rc)
				{
					// Convert from time_t to SYSTEMTIME
					SYSTEMTIME stLastScan;
					ZeroMemory(&stLastScan, sizeof(SYSTEMTIME));

					time_t timeCreationTime = m_timeLastRun;

					struct tm* pTime;
					pTime = localtime(&timeCreationTime);
					if(pTime)
					{
						timeCreationTime = mktime(pTime);

						if((time_t)(-1) != timeCreationTime)
						{
							stLastScan.wYear = static_cast<WORD>(pTime->tm_year+1900);
							stLastScan.wMonth = static_cast<WORD>(pTime->tm_mon+1);
							stLastScan.wDay = static_cast<WORD>(pTime->tm_mday);
							stLastScan.wHour = static_cast<WORD>(pTime->tm_hour);
							stLastScan.wMinute = static_cast<WORD>(pTime->tm_min);
							stLastScan.wSecond = static_cast<WORD>(pTime->tm_sec);

							DWORD dwBufSize = sizeof(stLastScan);

							rc = key.SetBinaryValue(L"SystemTime", (LPBYTE) &stLastScan, dwBufSize);
							if(ERROR_SUCCESS != rc)
							{
								CCTRCTXE1(L"Failed to save SystemTime. Error: %d", rc);
							}
						}
					}
				}
			}

			return S_OK;
		}

		// Write the drives to scan.
		hrx << NAVOPT_SUCCEEDED(NavOpts.SetDwordValue(szOptDriveCount, m_uDriveCnt));

		UINT uItemNumber = 1;
		std::list<CScanItem>::const_iterator It;
		char szEntry[64] = {0};
		SCANITEMTYPE Type = typeNone;
		SCANITEMSUBTYPE SubType = subtypeNone;

		if(m_uDriveCnt)
		{
			for(It = m_Items.begin(); It != m_Items.end(); It++)
			{
				(*It).GetItemType(Type, SubType);

				if (Type == typeDrive)
				{
					sprintf(szEntry, "Drive%lu", uItemNumber++);

					// Save drive letter as binary data
					// because navopt32 accepts only ANSI string
					CStringW sDrive = (LPCWSTR)(*It).GetDrive();
					LPBYTE lpBuffer = (LPBYTE)(sDrive.GetString());
					dwBufferSize = (sDrive.GetLength() + 1) * sizeof(WCHAR);
					hrx << NAVOPT_SUCCEEDED(NavOpts.SetBinaryValue(szEntry, lpBuffer, dwBufferSize));

					CCTRCTXI1(L"Write to scan file: %s", sDrive.GetString());
				}
			}
		}

		// Write the folders to scan.
		hrx << NAVOPT_SUCCEEDED(NavOpts.SetDwordValue(szOptFolderCount, m_uFolderCnt));

		if (m_uFolderCnt)
		{
			uItemNumber = 1;

			for(It = m_Items.begin(); It != m_Items.end(); It++)
			{
				(*It).GetItemType(Type, SubType);

				if(typeFolder == Type)
				{
					sprintf(szEntry, "Folder%lu", uItemNumber);

					// Save folder path as binary data
					// because navopt32 accepts only ANSI string
					CStringW sFolder = (*It).GetPath();
					LPBYTE lpBuffer = (LPBYTE)(sFolder.GetString());
					dwBufferSize = (sFolder.GetLength() + 1) * sizeof(WCHAR);
					hrx << NAVOPT_SUCCEEDED(NavOpts.SetBinaryValue(szEntry, lpBuffer, dwBufferSize));

					CCTRCTXI1(L"Write to scan file: %s", sFolder.GetString());


					sprintf(szEntry, "FolderSubs%lu", uItemNumber++);
					hrx << NAVOPT_SUCCEEDED(NavOpts.SetDwordValue(szEntry, (subtypeFolderAndSubs == SubType)? 1 : 0));
				}
			}
		}

		// Write the files to scan.
		hrx << NAVOPT_SUCCEEDED(NavOpts.SetDwordValue(szOptFileCount, m_uFileCnt));

		if (m_uFileCnt)
		{
			uItemNumber = 1;

			for(It = m_Items.begin(); It != m_Items.end(); It++)
			{
				(*It).GetItemType(Type, SubType);

				if (Type == typeFile)
				{
					sprintf(szEntry, "File%lu", uItemNumber++);

					// Save file name as binary data
					// because navopt32 accepts only ANSI string
					CStringW sFile = (*It).GetPath();
					LPBYTE lpBuffer = (LPBYTE)(sFile.GetString());
					dwBufferSize = (sFile.GetLength() + 1) * sizeof(WCHAR);
					hrx << NAVOPT_SUCCEEDED(NavOpts.SetBinaryValue(szEntry, lpBuffer, dwBufferSize));

					CCTRCTXI1(L"Write to scan file: %s", sFile.GetString());
				}
			}
		}

		// Write the file\VID items to scan.
		hrx << NAVOPT_SUCCEEDED(NavOpts.SetDwordValue(szOptFileVIDCount, m_uFileVIDCnt));

		if (m_uFileVIDCnt)
		{
			uItemNumber = 1;

			for(It = m_Items.begin(); It != m_Items.end(); It++)
			{
				(*It).GetItemType(Type, SubType);

				if (Type == typeFileVID || Type == typeFileVIDCompressed )
				{
					// If this is a compressed file-VID item write out the compressed flag
					if( Type == typeFileVIDCompressed )
					{
						sprintf(szEntry, "FileVIDCompressed%lu", uItemNumber);
						hrx << NAVOPT_SUCCEEDED(NavOpts.SetDwordValue(szEntry, 1));
					}

					// Write out the path
					sprintf(szEntry, "FileVIDPath%lu", uItemNumber);

					CStringW sPath = (*It).GetPath();
					LPBYTE lpBuffer = (LPBYTE)(sPath.GetString());
					dwBufferSize = (sPath.GetLength() + 1) * sizeof(WCHAR);
					hrx << NAVOPT_SUCCEEDED(NavOpts.SetBinaryValue(szEntry, lpBuffer, dwBufferSize));

					CCTRCTXI1(L"Write to scan file: %s", sPath.GetString());

					// Write out the VID
					sprintf(szEntry, "FileVIDId%lu", uItemNumber++);
					hrx << NAVOPT_SUCCEEDED(NavOpts.SetDwordValue(szEntry, (*It).GetVID()));
				}
			}
		}

		// We're done!
		hrx << NAVOPT_SUCCEEDED(NavOpts.Save(m_sPath));
	}
	STAHLSOFT_HRX_CATCH_ALL_NO_RETURN(hr)
		CCTRCTXI1(L"Exit. HR: 0x%08X", hr);
	return hr;
}

//-------------------------------------------------------------------------
HRESULT CScanTask::Load(LPCWSTR szFileName, bool bItems/*= true*/) throw()
{
	ccLib::CSingleLock csl(&m_Crit, INFINITE, FALSE);

	CCTRCTXI0(L"Enter");

	STAHLSOFT_HRX_TRY(hr)
	{
		CCTRCTXI1(L"Load %s", szFileName);

		CHNAVOPTS32 NavOpts;
		hrx << NAVOPT_SUCCEEDED(NavOpts.Allocate());
		hrx << NAVOPT_SUCCEEDED(NavOpts.Load(szFileName));

		// Read the task name
		BYTE byBuffer[(MAX_PATH+1)*2] = {0};
		DWORD dwBufferSize = sizeof(byBuffer);
		ZeroMemory(&byBuffer, dwBufferSize);

		hrx << NAVOPT_SUCCEEDED(NavOpts.GetBinaryValue(szOptName, (LPBYTE)&byBuffer, dwBufferSize));
		if(m_sName.Compare(L"*"))
			m_sName = (LPCWSTR)(byBuffer);

		CCTRCTXI1(L"ScanName: %s", m_sName.GetString());

		// Read the version number.
		hrx << NAVOPT_SUCCEEDED(NavOpts.GetDwordValue(szOptVersionMajor, m_wVersionMajor, 0xFFFFFFFF));
		hrx << NAVOPT_SUCCEEDED(NavOpts.GetDwordValue(szOptVersionMinor, m_wVersionMinor, 0xFFFFFFFF));
		hrx << NAVOPT_SUCCEEDED(NavOpts.GetDwordValue(szOptVersionInLine, m_wVersionInline, 0xFFFFFFFF));

		SCANTASKTYPE SType = scanUnset;
		DWORD dw = 0;

		// See if this is a built-in task.
		hrx << NAVOPT_SUCCEEDED(NavOpts.GetDwordValue(szOptType, dw, scanUnset));

		SType = SCANTASKTYPE(dw);
		if (scanUnset == SType)
			m_Type = scanCustom;
		else
			m_Type = SType;

		// Read the last run time.
		hrx << NAVOPT_SUCCEEDED(NavOpts.GetDwordValue(szOptLastRun, (DWORD&)m_timeLastRun, 0));

		// Read the CanEdit value
		hrx << NAVOPT_SUCCEEDED(NavOpts.GetDwordValue(szOptCanEdit, dw, 0));
		if(1 == dw)
			m_bCanEdit = true;

		// Read the CanSchedule value
		hrx << NAVOPT_SUCCEEDED(NavOpts.GetDwordValue(szOptCanSchedule, dw, 0));
		if(1 == dw)
			m_bCanSchedule = true;

		// Set the internal path name
		m_sPath = szFileName;

		// Only do the _Load if it's a built-in (!custom) and
		// the bItems is on. bItems = UI OK.
		if(scanCustom != m_Type && !bItems)
			return S_OK;

		if (scanCustom != m_Type)
		{
			// This is the loading code for built-in scans.
			// This will trigger UI, so we only call it if UI
			// has been requested by the bItems flag.
			switch(m_Type)
			{
			case scanCustom:
				break;
			case scanComputer:
				return Computer_Load(this);
			case scanHardDrives:
				return HardDisks_Load(this);
			case scanRemovable:
				return RemovableDrives_Load(this);
			case scanAFloppy:
				return Floppy_Load(this);
			case scanFolder:
				return Folders_Load(this);
			case scanFiles:
				return Files_Load(this);
			case scanSelectedDrives:
				return Drives_Load(this);
			case scanQuick:
				// For the quick scan clients need to just query for the type
				// and then run the quick scan (i.e: navw32)
				return S_OK;
			default:
				hrx << E_FAIL;
			}
		}   // end built-ins
		else
		{
			// This is the loader for custom scans. Since it never triggers UI
			// we always load them.

			// Read the drives.
			DWORD dwDriveCount = 0;
			hrx << NAVOPT_SUCCEEDED(NavOpts.GetDwordValue(szOptDriveCount, dwDriveCount, 0));

			char szEntry[64] = {0};

			for(unsigned int uIndex = 1; uIndex <= dwDriveCount; uIndex++)
			{
				// Get drive name
				ZeroMemory(&byBuffer, dwBufferSize);
				sprintf(szEntry, "Drive%lu", uIndex);
				hrx << NAVOPT_SUCCEEDED(NavOpts.GetBinaryValue(szEntry, (LPBYTE)&byBuffer, dwBufferSize));
				CString sDrive = (LPCWSTR)(byBuffer);
				if(sDrive.Compare(L"*"))
				{
					CCTRCTXI1(L"Add %c", sDrive.GetAt(0));
					AddItem(typeDrive, subtypeDriveHD, sDrive.GetAt(0), NULL, 0);
				}
			}

			// Read the folders.
			DWORD dwFolderCount = 0;
			hrx << NAVOPT_SUCCEEDED(NavOpts.GetDwordValue(szOptFolderCount, dwFolderCount, 0));

			for(uIndex = 1; uIndex <= dwFolderCount; uIndex++)
			{
				ZeroMemory(&byBuffer, dwBufferSize);
				sprintf(szEntry, "Folder%lu", uIndex);
				hrx << NAVOPT_SUCCEEDED(NavOpts.GetBinaryValue(szEntry, (LPBYTE)&byBuffer, dwBufferSize));
				CStringW sFolder = (LPCWSTR)(byBuffer);
				if(sFolder.Compare(L"*"))
				{
					sprintf(szEntry, "FolderSubs%lu", uIndex);
					hrx << NAVOPT_SUCCEEDED(NavOpts.GetDwordValue(szEntry, dw, 0));

					CCTRCTXI1(L"Add %s", sFolder);

					// Skip checking result here since we do not want 
					// exit just because we failed to add one item
					AddItem(typeFolder, (0 == dw)? subtypeFolder : subtypeFolderAndSubs, NULL, sFolder.GetString(), 0);
				}
			}

			// Read the files.
			DWORD dwFileCount = 0;
			hrx << NAVOPT_SUCCEEDED(NavOpts.GetDwordValue(szOptFileCount, dwFileCount, 0));

			for(uIndex = 1; uIndex <= dwFileCount; uIndex++)
			{
				ZeroMemory(&byBuffer, dwBufferSize);
				sprintf(szEntry, "File%lu", uIndex);
				hrx << NAVOPT_SUCCEEDED(NavOpts.GetBinaryValue(szEntry, (LPBYTE)&byBuffer, dwBufferSize));
				CStringW sFile = (LPCWSTR)(byBuffer);
				if(sFile.Compare(L"*"))
				{
					CCTRCTXI1(L"Add %s", sFile);

					// Skip checking result here since we do not want 
					// exit just because we failed to add one item
					AddItem(typeFile, subtypeFile, NULL, sFile, 0);
				}
			}

			// Read the filename\VID items.
			DWORD dwFileVIDCount = 0;
			hrx << NAVOPT_SUCCEEDED(NavOpts.GetDwordValue(szOptFileVIDCount, dwFileVIDCount, 0));

			for(uIndex = 1; uIndex <= dwFileVIDCount; uIndex++)
			{
				ZeroMemory(&byBuffer, dwBufferSize);
				// Read the path
				sprintf(szEntry, "FileVIDPath%lu", uIndex);
				hrx << NAVOPT_SUCCEEDED(NavOpts.GetBinaryValue(szEntry, (LPBYTE)&byBuffer, dwBufferSize));
				CStringW sFile = (LPCWSTR)(byBuffer);

				// Read the VID
				DWORD dwVID = 0;
				sprintf(szEntry, "FileVIDId%lu", uIndex);
				hrx << NAVOPT_SUCCEEDED(NavOpts.GetDwordValue(szEntry, dwVID, 0UL));

				// See if this is a compressed file VID item
				SCANITEMTYPE itemType = typeFileVID;
				DWORD dwCompressed = 0;
				sprintf(szEntry, "FileVIDCompressed%lu", uIndex);
				if(NAVOPTS32_OK == NavOpts.GetDwordValue(szEntry, dwCompressed, 0UL))
				{
					if(1 == dwCompressed)
					{
						itemType = typeFileVIDCompressed;
					}
				}

				if(sFile.Compare(L"*"))
				{
					// Skip checking result here since we do not want 
					// exit just because we failed to add one item
					CCTRCTXI1(L"Add: %s", sFile.GetString());
					AddItem(itemType, subtypeFile, NULL, sFile, dwVID);
				}
			}
		}   // end custom files
	}
	STAHLSOFT_HRX_CATCH_ALL_NO_RETURN(hr)
		CCTRCTXI1(L"Exit. HR: 0x%08X", hr);
	return hr;
}
//-------------------------------------------------------------------------
HRESULT CScanTask::GetScheduled(EScheduled& eScheduled) throw()
{
	ccLib::CSingleLock csl(&m_Crit, INFINITE, FALSE);

	CCTRCTXI0(L"Enter");

	STAHLSOFT_HRX_TRY(hr)
	{
		// Set to "Not Available" by default
		eScheduled = EScheduled_na;

		// If you can't schedule it, it must not be available.
		if(!m_bCanSchedule)
		{
			return S_OK;
		}

		// Create ITaskScheduler object
		CComPtr<ITaskScheduler> spTaskScheduler;
		hrx << spTaskScheduler.CoCreateInstance(CLSID_CTaskScheduler, NULL, CLSCTX_INPROC_SERVER);

		// Setup schedule name
		CComBSTR bstrScheduleName;
		hrx << getScheduleName(bstrScheduleName);

		IUnknown* pUnkTask = NULL;
		CComQIPtr<ITask> spITask;

		// Check if a scheduler file exist for this task
		hr = spTaskScheduler->Activate(bstrScheduleName, IID_ITask, &pUnkTask);

		if(SUCCEEDED(hr))
		{
			// Query for ITask interface
			spITask = pUnkTask;
			pUnkTask->Release();
			CComPtr<ITaskTrigger> spITaskTrigger;

			// Get the first trigger time, 
			// if there is none count this as "not scheduled".
			hr = spITask->GetTrigger(0, &spITaskTrigger);
			if(SUCCEEDED(hr))
			{
				// Scheduled
				eScheduled = EScheduled_yes;			
			}
			else
			{
				// Set to not scheduled by default
				eScheduled = EScheduled_no;
				CCTRCTXI1(L"spITask->GetTrigger failed. HR:0x%08X", hr);
			}
		}
		else if (E_INVALIDARG == hr ||
			HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND) == hr)
		{
			// Not scheduled
			eScheduled = EScheduled_no;
			CCTRCTXI1(L"Task has never been scheduled. HR:0x%08X", hr);
		}
		else if(HRESULT_FROM_WIN32(ERROR_ACCESS_DENIED) == hr)
		{
			// Scheduled by some other user (other admin for My Computer)
			eScheduled = EScheduled_na;
			CCTRCTXE1(L"spTaskScheduler->Activate Failed. Access is denied. HR:0x%08X", hr);
		}
	}
	STAHLSOFT_HRX_CATCH_ALL_NO_RETURN(hr)
		CCTRCTXI1(L"Task schedule type: %d", eScheduled);
	CCTRCTXI1(L"Exit. HR: 0x%08X", hr);
	return S_OK;
}
//-------------------------------------------------------------------------
HRESULT CScanTask::GetLastRunDate(DATE& lastRunDate) throw()
{
	ccLib::CSingleLock csl(&m_Crit, INFINITE, FALSE);

	CCTRCTXI0(L"Enter");

	STAHLSOFT_HRX_TRY(hr)
	{
		// Get last run time
		time_t  timeCreationTime;
		hrx << GetLastRunTime(timeCreationTime);

		struct tm* pTime;
		pTime = localtime(&timeCreationTime);
		hrx << (pTime? E_FAIL : S_OK);

		timeCreationTime = mktime(pTime);
		hrx << ((-1 == timeCreationTime) ? E_FAIL : S_OK);

		// Sanity check the result from mktime. On Brazilian mktime (0) returns 3600. Why???
		// Defect # 364009
		hrx << ((100000 >= timeCreationTime) ? E_FAIL : S_OK);

		// The following tm to DATE conversion routine has been lovingly
		// borrowed from MFC - OLEVAR.CPP - _AfxOleDateFromTm
		//
		// The unixTimeToSystemTime from MSDN was found to be inaccurate.
		//

		// One-based array of days in year at month start
		//
		int aiMonthDays[13] =
		{0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365};

		WORD wYear = static_cast<WORD>(pTime->tm_year+1900);
		WORD wMonth = static_cast<WORD>(pTime->tm_mon + 1);

		//  Check for leap year and set the number of days in the month
		bool bLeapYear = ((wYear & 3) == 0) &&
			((wYear % 100) != 0 || (wYear % 400) == 0);

		// Cache the date in days and time in fractional days
		//
		LONG lDate;
		double dblTime;
		double dtDest;

		//It is a valid date; make Jan 1, 1AD be 1
		//
		lDate = wYear*365L + wYear/4 - wYear/100 + wYear/400 +
			aiMonthDays[wMonth-1] + pTime->tm_mday;

		//  If leap year and it's before March, subtract 1:
		if (wMonth <= 2 && bLeapYear)
			--lDate;

		//  Offset so that 12/30/1899 is 0
		lDate -= 693959L;

		dblTime = (static_cast<LONG>(pTime->tm_hour) * 3600L) +  // hrs in seconds
			(static_cast<LONG>(pTime->tm_min) * 60L) +  // mins in seconds
			(static_cast<LONG>(pTime->tm_sec)) / 86400.;

		dtDest = (double) lDate + ((lDate >= 0) ? dblTime : -dblTime);
		lastRunDate = dtDest;

	}
	STAHLSOFT_HRX_CATCH_ALL_NO_RETURN(hr)
		CCTRCTXI1(L"Exit. LastRunDate: %d", lastRunDate);
	return S_OK;
}
//-------------------------------------------------------------------------

HRESULT CScanTask::Scan(void) throw()
{
	return Scan(NULL, NULL);
}

HRESULT CScanTask::Scan(LPCWSTR szScanSwitches) throw()
{
	return Scan(NULL, szScanSwitches);
}

HRESULT CScanTask::Scan(HANDLE* phWait, LPCWSTR szScanSwitches) throw()
{
	ccLib::CSingleLock csl(&m_Crit, INFINITE, FALSE);

	if(!m_sPath.GetLength())
		return E_FAIL;

	CCTRCTXI0(L"Enter");

	STAHLSOFT_HRX_TRY(hr)
	{	
		// Command line is c:\progra~1\norton~1\navw32.exe /task:"c:\...\task.sca"
		ccLib::CStringW sExePath;
		ccLib::CStringW sParameters;
		CNAVInfo NAVInfo;
		WCHAR szShortNAVDir[MAX_PATH] = {0};

		if(!GetShortPathName(NAVInfo.GetNAVDir(), szShortNAVDir, MAX_PATH))
			hrx << E_FAIL;

		sExePath = szShortNAVDir;
		sExePath += L"\\navw32.exe";

		SCANTASKTYPE scanType = m_Type;
		if((scanComputer == scanType) 
			|| (scanRemovable == scanType)
			|| (scanAFloppy == scanType)
			|| (scanSelectedDrives == scanType)
			|| (scanFolder == scanType)
			|| (scanFiles == scanType)
			|| (scanCustom == scanType))
		{
			sParameters = SWITCH_DISABLE_ERASER_SCAN;
			sParameters += L" ";
		}

		sParameters += SWITCH_TASKFILE;
		sParameters += L"\"";
		sParameters += m_sPath;
		sParameters += L"\"";

		// If clients pass in a switch for navw32.exe,
		// append it.
		if(szScanSwitches)
		{
			if(wcslen(szScanSwitches))
			{
				CCTRCTXI1(L"Addition switches: %", szScanSwitches);

				sParameters += L" ";
				sParameters += szScanSwitches;
			}
		}

		CCTRCTXI2(L"Scan Command: %s %s", sExePath.GetString(), sParameters.GetString());

		// Since '.scan' files are associated with a scanning application we can run the file.
		//			
		SHELLEXECUTEINFO seinfo;
		memset (&seinfo, 0, sizeof(seinfo));
		seinfo.cbSize = sizeof(SHELLEXECUTEINFO);

		// Add SEE_MASK_NOZONECHECKS here to fix an issue on 
		// Japanese system in which launching scanner in safe mode
		// will get access denied error.
		seinfo.fMask = SEE_MASK_NOCLOSEPROCESS | SEE_MASK_NOZONECHECKS;

		seinfo.hwnd = NULL;
		seinfo.lpVerb = L"open";
		seinfo.lpFile = sExePath;
		seinfo.lpParameters = sParameters;
		seinfo.lpDirectory = NULL;
		seinfo.nShow = SW_SHOW;

		BOOL bRet = ShellExecuteEx(&seinfo);
		if(phWait)
		{
			*phWait = seinfo.hProcess;
		}
		else
			::CloseHandle(seinfo.hProcess);

		if(!bRet)
		{
			DWORD dwError = GetLastError();

			CCTRCTXI1(L"ShellExecuteEx result: %d", dwError);

			hrx << HRESULT_FROM_WIN32(dwError);
		}
	}
	STAHLSOFT_HRX_CATCH_ALL_NO_RETURN(hr)
		CCTRCTXI1(L"Exit. HR: 0x%08X", hr);
	return hr;
}
//-------------------------------------------------------------------------
HRESULT CScanTask::ScheduleTask(bool bWeekly) throw()
{
	ccLib::CSingleLock csl(&m_Crit, INFINITE, FALSE);

	// If you can't schedule this item, just return
	if(!m_bCanSchedule)
		return S_FALSE;

	CCTRCTXI0(L"Enter");

	STAHLSOFT_HRX_TRY(hr)
	{	
		EScheduled eScheduled;
		hrx << GetScheduled(eScheduled);
		if(EScheduled_yes != eScheduled)
		{
			// This task has not been scheduled, so make a new one.
			hrx << makeSchedule(bWeekly);
		}

		// Don't show if they were just scheduling the weekly scan.
		// such as scheduling FSS during cfgwiz.
		if(bWeekly)
			return S_OK;

		// Show the scheduler
		hrx << showSchedule();
	}
	STAHLSOFT_HRX_CATCH_ALL_NO_RETURN(hr)
		CCTRCTXI1(L"Exit. HR: 0x%08X", hr);
	return hr;
}
//-------------------------------------------------------------------------
HRESULT CScanTask::Edit() throw()
{
	ccLib::CSingleLock csl(&m_Crit, INFINITE, FALSE);

	// If you can't schedule this item, just return
	if(!m_bCanSchedule)
		return S_FALSE;

	CCTRCTXI0(L"Enter");

	STAHLSOFT_HRX_TRY(hr)
	{	
		// Get current schedule name
		CComBSTR bstrOldScheduleName;
		hrx << getScheduleName(bstrOldScheduleName);

		// Get current task name
		ccLib::CStringW sOldTaskName(m_sName);

		int iMode = 1;
		bool bDuplicate = false;

		do
		{
			// Display Edit wizard
			CTaskWizard NTW;
			hr = NTW.NAVTaskWizard(this, iMode);

			// S_OK: User clicked Finished on the dialog
			// S_FALSE: User cancelled out of the dialog

			if(hr == S_OK)
			{
				// Get new task name
				ccLib::CStringW sNewTaskName(m_sName);

				// Validate the name, if it changed.
				if(sNewTaskName.CompareNoCase(sOldTaskName))
				{
					// Check for duplicate name with the parent list

					ScanTaskCollection_Loader loader;
					IScanTaskCollectionPtr spScanTaskCollection;

					hrx << (SYM_SUCCEEDED(loader.CreateObject(&spScanTaskCollection))? S_OK : E_FAIL);
					hrx << spScanTaskCollection->CheckDuplicateTaskName(m_sName, bDuplicate);
				}

				if(!bDuplicate)
				{
					hr = Save();
					if(SUCCEEDED(hr))
					{
						// If the task name changed we need to change
						// the scheduled task name to syncronize, 
						// if there is a schedule. We also need to do this
						// if there is a schedule with no triggers (EScheduled_no)
						//
						HRESULT hr1 = renameSchedule(bstrOldScheduleName);
						if(HRESULT_FROM_WIN32(ERROR_FILE_EXISTS) == hr1)
						{
							CCTRCTXI1(L"A work item already exists in the scheduler: %s", bstrOldScheduleName);
						}
						else
						{
							hr = hr1;
						}
					}
					return hr;
				}
				else
				{
					// Dup
					iMode = 2; // Duplicate mode
					bDuplicate = true;

					// Tell the user they have a duplicate named task
					ccLib::CString sError;
					sError.LoadString(IDS_ERR_DUP_NAME);
					MessageBox(NULL, sError, m_sProductName, MB_OK|MB_ICONWARNING);

					// Put back the old name
					m_sName = sOldTaskName;
				}
			}
		}
		while(hr == S_OK && bDuplicate);
	}
	STAHLSOFT_HRX_CATCH_ALL_NO_RETURN(hr)
		CCTRCTXI1(L"Exit. HR: 0x%08X", hr);
	return hr;
}
//-------------------------------------------------------------------------
HRESULT CScanTask::DeleteSchedule() throw()
{
	ccLib::CSingleLock csl(&m_Crit, INFINITE, FALSE);

	CCTRCTXI0(L"Enter");

	STAHLSOFT_HRX_TRY(hr)
	{
		CComPtr<ITaskScheduler> spTaskScheduler;
		hrx << spTaskScheduler.CoCreateInstance(CLSID_CTaskScheduler,
			NULL, CLSCTX_INPROC_SERVER);

		CComBSTR bstrScheduleName;
		hrx << getScheduleName(bstrScheduleName);

		// Delete schedule if there is one.
		hrx << spTaskScheduler->Delete(bstrScheduleName);
	}
	STAHLSOFT_HRX_CATCH_ALL_NO_RETURN(hr)
		CCTRCTXI1(L"Exit. HR: 0x%08X", hr);
	return hr;
}
//-------------------------------------------------------------------------
HRESULT CScanTask::makeSchedule(bool bDefaultWeekly) throw()
{
	CCTRCTXI0(L"Enter");

	STAHLSOFT_HRX_TRY(hr)
	{	
		CComPtr<ITaskScheduler> spTaskScheduler;
		hrx << spTaskScheduler.CoCreateInstance(CLSID_CTaskScheduler,
			NULL, CLSCTX_INPROC_SERVER);

		CComBSTR bstrScheduleName;
		hrx << getScheduleName(bstrScheduleName);

		IUnknown* pUnkTask = NULL;
		hr = spTaskScheduler->NewWorkItem(bstrScheduleName, 
			CLSID_CTask, IID_ITask,	&pUnkTask);
		if(FAILED(hr))
		{
			// If there is already a scheduled item we should 
			// just return true, since we are confirming the creation of
			// a schedule for this item.
			if(HRESULT_FROM_WIN32(ERROR_FILE_EXISTS) == hr)
			{
				return S_OK;
			}
			else if(HRESULT_FROM_WIN32(ERROR_ACCESS_DENIED) == hr)
			{
				CCTRCTXE1(L"NewWorkItem failed. HR: 0x%08X", hr);
				hrx << hr;
			}
		}

		// If we fail here it's probably because the task already exists.

		// We can switch to a smart pointer now.
		CComQIPtr<ITask> spITask(pUnkTask);
		pUnkTask->Release();

		// Put a nice friendly comment in the comment field
		ccLib::CStringW sComment;
		ccLib::CStringW sFormat;
		sFormat.LoadString(IDS_SCHEDULE_COMMENT);
		sComment.Format(sFormat.GetString(), m_sProductName.GetString());

		hrx << spITask->SetComment(sComment.GetString());

		// Command line is c:\progra~1\norton~1\navw32.exe /task:"c:\...\task.sca"
		//
		ccLib::CStringW sExePath;
		ccLib::CStringW sParameters;
		CNAVInfo NAVInfo;
		WCHAR szShortNAVDir[MAX_PATH] = {0};

		if(!GetShortPathName(NAVInfo.GetNAVDir(), szShortNAVDir, MAX_PATH))
			hrx << E_FAIL;

		sExePath = szShortNAVDir;
		sExePath += L"\\navw32.exe";

		SCANTASKTYPE scanType = m_Type;
		if((scanComputer == scanType) 
			|| (scanRemovable == scanType)
			|| (scanAFloppy == scanType)
			|| (scanSelectedDrives == scanType)
			|| (scanFolder == scanType)
			|| (scanFiles == scanType)
			|| (scanCustom == scanType))
		{
			sParameters = SWITCH_DISABLE_ERASER_SCAN;
			sParameters += L" ";
		}

		sParameters += SWITCH_TASKFILE;
		sParameters += L"\"";
		sParameters += m_sPath;
		sParameters += L"\"";

		hrx << spITask->SetApplicationName(sExePath);
		hrx << spITask->SetParameters(sParameters);
		hrx << spITask->SetFlags(TASK_FLAG_RUN_ONLY_IF_LOGGED_ON);

		//
		// All this junk is to put the COMPUTERNAME\username into the task. Otherwise
		// we run as a local system service.
		//
		// The COMPUTERNAME\username (no password) and TASK_FLAG_RUN_ONLY_IF_LOGGED_ON
		// flag is Task Scheduler's pseudo-impersonation method. This isn't really documented.
		// I figured it out by looking at the Windows Critical Update item in the scheduler.
		//
		// If the about step fails, it's probably because we are on an OS without
		// security, like 9x. Not stopping because of this error is easier than
		// putting in a bunch of OS checking that will get out of date.

		ccLib::CStringW sAccountName;
		WCHAR szUserName[200] = {0};
		DWORD dwUserNameSize = sizeof(szUserName)/sizeof(WCHAR);
		WCHAR szComputerName[100];
		DWORD dwComputerNameSize = sizeof(szComputerName)/sizeof(WCHAR);
		LPWSTR pwszPassword = NULL;

		if(!GetUserName(szUserName, &dwUserNameSize) || 
			!GetComputerName(szComputerName, &dwComputerNameSize))
		{
			CCTRCTXE0(L"GetUserName or GetComputerName failed");        
			hrx << E_FAIL;
		}

		sAccountName.Format(L"%s\\%s", szComputerName, szUserName);

		hr = spITask->SetAccountInformation(sAccountName.GetString(), pwszPassword);

		if(FAILED(hr))
		{
			CCTRCTXE2(L"SetAccountInformation(%S) failed. HR: 0x%08X", 
				sAccountName.GetString(), hr);
		}

		// If this is the weekly "My Computer" scan
		if(bDefaultWeekly)
		{
			TASK_TRIGGER trigger;
			ZeroMemory(&trigger, sizeof(TASK_TRIGGER));

			SYSTEMTIME sysTime = {0};
			GetSystemTime(&sysTime);

			trigger.wBeginYear = sysTime.wYear;
			trigger.wBeginMonth = sysTime.wMonth;
			trigger.wBeginDay = sysTime.wDay;

			DWORD dwTime = 20;
			DWORD dwDayOfWeek = 5;

			// ScanDay is the exponent power of 2 to convert to MSTASK's day of week
			// 5 turns into 2^5 = 0x20 = TASK_FRIDAY
			try
			{
				READ_SETTING_START()
					READ_SETTING_DWORD(SCANNER_ScanTime, dwTime, 20)
					READ_SETTING_DWORD(SCANNER_ScanDay, dwDayOfWeek, 5)
					READ_SETTING_END
			}
			catch (_com_error& e)
			{
				CCTRCTXI0(L"OEM settings SCANNER:ScanTime and SCANNER:ScanDay not found");
			}

			CCTRCTXI2(L"Time for scan is hour %d, day %d", dwTime, dwDayOfWeek);

			trigger.wStartHour = LOWORD(dwTime); // 8 PM (24-hour clock) by default

			// Convert afterwards for readability
			dwDayOfWeek = (DWORD)pow(2,dwDayOfWeek);

			TRIGGER_TYPE_UNION triggertype;
			ZeroMemory(&triggertype, sizeof(TRIGGER_TYPE_UNION));

			WEEKLY weekly = {0};
			weekly.WeeksInterval = 1;               // Every week
			weekly.rgfDaysOfTheWeek = LOWORD(dwDayOfWeek);

			triggertype.Weekly = weekly;
			trigger.Type = triggertype;
			trigger.cbTriggerSize = sizeof(trigger);
			trigger.TriggerType = TASK_TIME_TRIGGER_WEEKLY;    // It's a weekly operation

			// Save the trigger
			CComPtr<ITaskTrigger> spITaskTrigger;
			WORD wNewTrigger = 0;
			hrx << spITask->CreateTrigger(&wNewTrigger, &spITaskTrigger);
			hrx << spITaskTrigger->SetTrigger(&trigger);
		}

		// Save the Task ( trigger is attached )
		CComPtr<IPersistFile> spIPersistFile;
		hrx << spITask->QueryInterface( IID_IPersistFile,
			(void **)&spIPersistFile);

		// We don't change the return value on Save because the file might already exist. Bug
		// in IE 4.0 . Defect # 361077.

		// Saves the scheduler task to disk. 
		// The resulting scheduler file is c:\windows\Tasks\FileName.job
		spIPersistFile->Save(NULL, TRUE);

		//
		// Vista support:
		// If this method is run in a context of a process with 
		// elevated-admin privileges such as cfgwiz, the resulting scheduler 
		// file can be accessed only by processes that are executed 
		// with elevated-admin privileges. 
		// We need to update DACL of the scheduler file 
		// to allow non elevated-admin proceses 
		// such as ccApp accessing the file. 
		// 
		bool bVistaOrLater = false;
		if(ccLib::COSInfo::IsWinVista(bVistaOrLater))
		{
			// Setup path to the scheduler job file
			// of this scan task
			// C:\Windows\Tasks\FileName.job

			ccLib::CStringW sScheduleJobFile;

			if(!GetWindowsDirectory(sScheduleJobFile.GetBufferSetLength(MAX_PATH), MAX_PATH))
			{
				DWORD dwError = GetLastError();
				CCTRCTXE1(L"GetWindowsDirectory failed. Error: %d", dwError);
				hrx << HRESULT_FROM_WIN32(dwError);
			}
			sScheduleJobFile.ReleaseBuffer();

			// Add backslash
			PathAddBackslash(sScheduleJobFile.GetBuffer());
			sScheduleJobFile.ReleaseBuffer();

			// Add "Tasks"
			sScheduleJobFile += L"Tasks\\";
			sScheduleJobFile += bstrScheduleName;
			sScheduleJobFile += L".job";

			CCTRCTXI1(L"ScheduleJobFile: %s", sScheduleJobFile.GetString());

			hrx << setFullAccessToFile(sScheduleJobFile.GetString());
		}
	}
	STAHLSOFT_HRX_CATCH_ALL_NO_RETURN(hr)
		CCTRCTXI1(L"Exit. HR: 0x%08X", hr);
	return hr;
}
//-------------------------------------------------------------------------
HRESULT CScanTask::showSchedule() throw()
{
	CCTRCTXI0(L"Enter");

	STAHLSOFT_HRX_TRY(hr)
	{
		// Create SymTheme Skin window.
		// Do not bail out if failed.
		// Should continue to display Scheduler dialog
		HRESULT hr1 = S_OK;
		CISSymTheme isSymTheme;
		hr1 = isSymTheme.Initialize(GetModuleHandle(NULL));
		if(FAILED(hr1))

		{
			CCTRCTXE1(L"CISSymTheme::Initialize() failed. Error: 0x%08X", hr1);
		}

		CComPtr<ITaskScheduler> spTaskScheduler;
		hrx << spTaskScheduler.CoCreateInstance(CLSID_CTaskScheduler,
			NULL, CLSCTX_INPROC_SERVER);

		// Setup schedule name
		CComBSTR bstrScheduleName;
		hrx << getScheduleName(bstrScheduleName);

		CComPtr<IUnknown> spUnkTask; 
		CComQIPtr<ITask> spITask;

		// Check if a scheduler file exist for this task
		hrx << spTaskScheduler->Activate(bstrScheduleName, IID_ITask, &spUnkTask);
		spITask = spUnkTask;

		TASKPAGE tpType = TASKPAGE_SCHEDULE;

		bool bPersistChanges = TRUE;
		HPROPSHEETPAGE phPage;
		CComPtr<IProvideTaskPage> spIProvTaskPage;

		hrx << spITask->QueryInterface(IID_IProvideTaskPage, (void**)&spIProvTaskPage);
		hrx << spIProvTaskPage->GetPage(tpType,	bPersistChanges,&phPage);

		PROPSHEETHEADER psh;
		ZeroMemory(&psh, sizeof(PROPSHEETHEADER));
		psh.dwSize = sizeof(PROPSHEETHEADER);
		psh.dwFlags = PSH_DEFAULT | PSH_NOAPPLYNOW | PSH_USEPAGELANG | PSH_MODELESS;
		psh.phpage = &phPage;
		psh.nPages = 1;

		HWND hwndParent = GetForegroundWindow();

		if(!IsWindow(hwndParent))
			hwndParent = GetDesktopWindow();

		psh.hwndParent = hwndParent;

		// We get to customize the title
		psh.pszCaption = bstrScheduleName;

		// Show it!
		HWND hwndDlg = (HWND)PropertySheet(&psh);

		if(NULL == hwndDlg)
		{
			CCTRCTXE0(L"Failed to create property sheet");
			hrx << E_FAIL;
		}

		// Check the width of created property page
		RECT r = {0};
		LONG lPropPageWidth = 0;
		LONG left = 0;
		LONG right = 0;

		HWND hPage = NULL;

		// PropSheet_IndexToHwnd() does not work here
		PropSheet_SetCurSel(hwndDlg, 0, 0);
		hPage = PropSheet_GetCurrentPageHwnd(hwndDlg);

		if (hPage == NULL || !GetWindowRect(hPage, &r))
		{
			CCTRCTXE0(L"Failed to find the first page");
			hrx << E_FAIL;
		}

		left = r.left;
		right = r.right;

		// Enumerate all children
		HWND hChild = NULL;
		while(NULL != (hChild = FindWindowEx(hPage, hChild, NULL, NULL)))
		{
			if(GetWindowRect(hChild, &r))
			{
				if(r.right > right)
				{
					right = r.right;
					lPropPageWidth = right - left;
				}
			}
		}

		PropSheet_SetCurSel(hwndDlg, 0, 0);
		hPage = PropSheet_GetCurrentPageHwnd(hwndDlg);
		if(hPage != NULL && GetWindowRect(hPage, &r) && ((r.right - r.left + 2) < lPropPageWidth))
		{
			CCTRCTXI0(L"Resizing property page");

			// Create a fake page
			DLGTEMPLATE dt = {0};
			dt.style = DS_MODALFRAME | WS_POPUP | WS_VISIBLE | WS_CAPTION | WS_SYSMENU;
			dt.cy = 100;

			// Convert pixels to dialog units
			RECT rt = {0};
			rt.right = 100;
			MapDialogRect(hwndDlg, &rt);
			dt.cx = static_cast<short>((lPropPageWidth * 100) / rt.right);

			// Create property page
			PROPSHEETPAGE pp = {0};
			pp.dwSize = sizeof(pp);
			pp.dwFlags = PSP_DLGINDIRECT | PSP_USETITLE;
			pp.pResource = &dt;
			pp.pszTitle = _T("");

			HPROPSHEETPAGE page = CreatePropertySheetPage(&pp);

			if(page && PropSheet_AddPage(hwndDlg, page))
			{
				PropSheet_RecalcPageSizes(hwndDlg);
				PropSheet_RemovePage(hwndDlg, 1, page);
			}
			else
			{
				CCTRCTXW0(L"Failed to create/add a fake page");
			}
		}

		// Message loop
		MSG msg;
		while(GetMessage(&msg,NULL,0,0))
		{ 
			if(!PropSheet_IsDialogMessage(hwndDlg, &msg))
			{ 
				TranslateMessage(&msg); 
				DispatchMessage(&msg); 
			}
			if(PropSheet_GetCurrentPageHwnd(hwndDlg) == NULL)
			{
				DestroyWindow(hwndDlg);
				break;
			}
		} 
	}
	STAHLSOFT_HRX_CATCH_ALL_NO_RETURN(hr)

		CCTRCTXI1(L"Exit. HR: 0x%08X", hr);
	return hr;
}
//-------------------------------------------------------------------------
// Used to syncronize our Task name with our Schedule name
//-------------------------------------------------------------------------
HRESULT CScanTask::renameSchedule(CComBSTR bstrOldScheduleName) throw()
{
	CCTRCTXI0(L"Enter");

	STAHLSOFT_HRX_TRY(hr)
	{
		CComPtr<ITaskScheduler> spTaskScheduler;
		hrx << spTaskScheduler.CoCreateInstance(CLSID_CTaskScheduler, 
			NULL, CLSCTX_INPROC_SERVER);

		// Read in old task
		CComPtr<IUnknown> spUnkTask;
		hr = spTaskScheduler->Activate(bstrOldScheduleName, IID_ITask, &spUnkTask);
		if(FAILED(hr))
		{
			CCTRCTXI0(L"Skipping schedule rename since this task has never been scheduled.");
			return S_FALSE;
		}

		// If we fail here it's probably because the task already exists.

		// We can switch to a smart pointer now.
		CComQIPtr<ITask> spIOldTask(spUnkTask);

		CComPtr<IPersistFile> spIOldPersistFile;
		hrx << spIOldTask->QueryInterface(IID_IPersistFile, (void**) &spIOldPersistFile);

		// Get filename
		LPOLESTR lpstrOldFileName;
		hrx << spIOldPersistFile->GetCurFile(&lpstrOldFileName);

		// Ditch the LPOLESTR and just store it locally instead
		WCHAR szOldFileName[MAX_PATH] = {0};
		wcsncpy(szOldFileName, lpstrOldFileName, MAX_PATH);

		LPMALLOC lpMalloc;
		hrx << CoGetMalloc(1, &lpMalloc);
		lpMalloc->Free(lpstrOldFileName);

		// Create new task
		CComBSTR bstrScheduleName;
		hrx << getScheduleName(bstrScheduleName);

		IUnknown *pINewTask = NULL;
		hrx << spTaskScheduler->NewWorkItem(bstrScheduleName,
			CLSID_CTask, IID_ITask, &pINewTask);

		// We can switch to a smart pointer now.
		CComQIPtr <ITask> spINewTask;
		spINewTask = pINewTask;
		pINewTask->Release();

		CComPtr <IPersistFile> spINewPersistFile;
		hrx << spINewTask->QueryInterface(IID_IPersistFile, 
			(void **)&spINewPersistFile);

		// Get new filename
		LPOLESTR lpolestrNewFileName;
		hrx << spINewPersistFile->GetCurFile(&lpolestrNewFileName);

		// Ditch the LPOLESTR and just store it locally instead
		WCHAR szNewFileName[MAX_PATH] = {0};
		wcsncpy(szNewFileName, lpolestrNewFileName, MAX_PATH);

		lpMalloc->Free(lpolestrNewFileName);

		// Save old task in new filename
		hrx << spIOldPersistFile->Save(szNewFileName, FALSE);   // Save As ...

		// Delete old task
		hrx << spTaskScheduler->Delete(szOldFileName);
	}
	STAHLSOFT_HRX_CATCH_ALL_NO_RETURN(hr)
		CCTRCTXI1(L"Exit. HR: 0x%08X", hr);
	return hr;
}
//-------------------------------------------------------------------------
HRESULT CScanTask::reset()
{
	CCTRCTXI0(L"Enter");
	STAHLSOFT_HRX_TRY(hr)
	{
		Empty();

		m_Type = scanUnset;
		m_bCanEdit = FALSE;
		m_bCanSchedule = FALSE;
		m_uDriveCnt = 0;
		m_uFileCnt = 0;
		m_uFolderCnt = 0;
		m_uFileVIDCnt = 0;
		m_wVersionMajor = 8;
		m_wVersionMinor = 0;
		m_wVersionInline = 0;
		m_timeLastRun = 0;

		EnumReset();
	}
	STAHLSOFT_HRX_CATCH_ALL_NO_RETURN(hr)
		CCTRCTXI1(L"Exit. HR: 0x%08X", hr);
	return hr;
}
//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
HRESULT CScanTask::getScheduleName(CComBSTR& bstrName) throw()
{
	// Construct task name in the format 
	// "ProductName - TaskName - UserName"
	bstrName = m_sProductName;
	bstrName.Append(" - ");
	bstrName.Append(m_sName);

	// Get user name
	ccLib::CStringW sUserName;
	DWORD dwSize = 200;
	if(GetUserName(sUserName.GetBufferSetLength(dwSize), &dwSize))
	{
		sUserName.ReleaseBuffer();

		if(sUserName.GetLength())
		{
			bstrName.Append(L" - ");
			bstrName.Append(sUserName);
		}

	}

	return S_OK;
}
//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
HRESULT CScanTask::setFullAccessToFile(LPCWSTR lpszFile)
{
	CCTRCTXI0(L"Enter");
	PSID pSID = NULL;
	PACL pACL = NULL;
	LPWSTR lpszDomainName = NULL;

	STAHLSOFT_HRX_TRY(hr)
	{
		if(!lpszFile)
			return E_INVALIDARG;

		// Get user name

		CStringW sUserName;
		DWORD dwSize = UNLEN + 1;
		DWORD dwRes = 0;

		if(!GetUserName(sUserName.GetBufferSetLength(dwSize), &dwSize))
		{
			dwRes = GetLastError();
			CCTRCTXE1(L"GetUserName failed. Error: %d", dwRes);
			hrx << HRESULT_FROM_WIN32(dwRes);
		}
		sUserName.ReleaseBuffer();

		CCTRCTXI1(L"UserName: %s", sUserName.GetString());

		DWORD dwSIDSize = 0;
		DWORD dwDomainNameSize = 0;
		SID_NAME_USE eNameUse;
		memset(&eNameUse, 0, sizeof(SID_NAME_USE));

		// Call LookupAccountName with dwSIDSize=0 and dwDomainNameSize=0 
		// to get buffer size for SID and lpszDomainName.

		if(!LookupAccountName(
			NULL,			// Target system-> NULL for local system
			sUserName,		// Account name -> current user
			pSID,			// SID 
			&dwSIDSize,		// size of SID buffer
			lpszDomainName, // Name of domain where the account name is found
			&dwDomainNameSize, // size of domain name
			&eNameUse))			// SID_NAME_USE
		{
			dwRes = GetLastError();

			if(ERROR_INSUFFICIENT_BUFFER == dwRes)
			{
				CCTRCTXI1(L"SIDSize: %d", dwSIDSize);
				CCTRCTXI1(L"DomainNameSize: %d", dwDomainNameSize);

				// Allocate buffer for pSID and szDomainNameSize
				pSID = (PSID) malloc(dwSIDSize);
				memset(pSID, 0, sizeof(dwSIDSize));
				
				lpszDomainName = (LPWSTR) malloc(dwDomainNameSize * sizeof(WCHAR));
				memset(lpszDomainName, 0, sizeof(dwDomainNameSize * sizeof(WCHAR)));

				if(!pSID || !lpszDomainName)
				{
					CCTRCTXE0(L"malloc failed");
					hrx << HRESULT_FROM_WIN32(ERROR_NOT_ENOUGH_MEMORY);
				}
			}
			else
			{
				dwRes = GetLastError();
				CCTRCTXE1(L"LookupAccountName failed. Error: %d", dwRes);
				hrx << HRESULT_FROM_WIN32(dwRes);
			}
		}

		// Now get the SID of current user
		if(!LookupAccountName(
			NULL,			// Target system-> NULL for local system
			sUserName,		// Account name -> current user
			pSID,			// SID 
			&dwSIDSize,		// size of SID buffer
			lpszDomainName, // Name of domain where the account name is found
			&dwDomainNameSize, // size of domain name
			&eNameUse))			// SID_NAME_USE
		{
			dwRes = GetLastError();
			CCTRCTXE1(L"LookupAccountName failed. Error: %d", dwRes);
			hrx << HRESULT_FROM_WIN32(dwRes);
		}

		const int NUM_ACES = 1;
		EXPLICIT_ACCESS ea;
		ea.grfAccessPermissions = GENERIC_ALL;
		ea.grfAccessMode = GRANT_ACCESS;
		ea.grfInheritance = NO_INHERITANCE;
		ea.Trustee.TrusteeForm = TRUSTEE_IS_SID;
		ea.Trustee.TrusteeType = TRUSTEE_IS_USER;
		ea.Trustee.ptstrName = (LPTSTR) pSID;

		// Set full control for current user.
		dwRes = SetEntriesInAcl(NUM_ACES, &ea, NULL, &pACL);

		if(ERROR_SUCCESS != dwRes)
		{
			CCTRCTXE1(L"SetEntriesInAcl failed. Error: %d", dwRes);
			hrx << HRESULT_FROM_WIN32(dwRes);
		}

		// Try to modify the object's DACL.
		dwRes = SetNamedSecurityInfo(
			const_cast<LPWSTR>(lpszFile),  // name of the object
			SE_FILE_OBJECT,              // type of object
			DACL_SECURITY_INFORMATION,   // change only the object's DACL
			NULL, NULL,                  // do not change owner or group
			pACL,                        // DACL specified
			NULL);                       // do not change SACL

		if(ERROR_SUCCESS != dwRes)
		{
			CCTRCTXE1(L"SetNamedSecurityInfo failed. Error: %d", dwRes);
			hrx << HRESULT_FROM_WIN32(dwRes);
		}
	}
	STAHLSOFT_HRX_CATCH_ALL_NO_RETURN(hr)

	if(pACL)
		LocalFree(pACL);

	if(lpszDomainName)
		free(lpszDomainName);

	if(pSID)
		free(pSID);

	CCTRCTXI1(L"Exit. HR: 0x%08X", hr);
	return hr;
}
