////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

// ScanTaskCollection.cpp : implementation file
//
#include "StdAfx.h"
#include <Mstask.h>
#include "ScanTaskCollection.h"
#include "ScanTaskLoader.h"
#include "ScanTaskDefines.h"
#include "TaskWizard.h"
#include "ccSingleLock.h"
#include "ccStringInterface.h"
#include "isVersion.h"
#include "..\\avScanTaskRes\\Resource.h"

using namespace avScanTask;

//-------------------------------------------------------------------------
CScanTaskCollection::CScanTaskCollection(void)
{
	STAHLSOFT_HRX_TRY(hr)
	{
		// Get branding product name
		m_sProductName = CISVersion::GetProductName();
		
		hrx << addTasks(m_NavInfo.GetNAVCommonDir());
		hrx << addTasks(m_NavInfo.GetNAVUserDir());
	}
	STAHLSOFT_HRX_CATCH_ALL_NO_RETURN(hr)
}
//-------------------------------------------------------------------------
CScanTaskCollection::~CScanTaskCollection(void)
{
    emptyTaskList();
}
//-------------------------------------------------------------------------
HRESULT CScanTaskCollection::GetCount(DWORD& dwCount) throw()
{
	ccLib::CSingleLock csl(&m_Crit, INFINITE, FALSE);
	dwCount = static_cast<DWORD>(m_ScanTaskColl.size());
	return S_OK;
}

//-------------------------------------------------------------------------
HRESULT CScanTaskCollection::GetItem(DWORD dwIndex, avScanTask::IScanTask** ppItem) throw()
{
	CCTRCTXI0(L"Enter");

	STAHLSOFT_HRX_TRY(hr)
	{
		hrx << (!ppItem? E_INVALIDARG : S_OK);

		ccLib::CSingleLock csl(&m_Crit, INFINITE, FALSE);

		// Search the collection
		if(dwIndex >= 0 && dwIndex < m_ScanTaskColl.size())
		{
			*ppItem = m_ScanTaskColl.at(dwIndex);
			(*ppItem)->AddRef();
		}
		else
		{
			hrx << E_INVALIDARG;
		}
	}
	STAHLSOFT_HRX_CATCH_ALL_NO_RETURN(hr)
	CCTRCTXI1(L"Exit. HR: 0x%08X", hr);
	return hr;
}
//-------------------------------------------------------------------------
HRESULT CScanTaskCollection::GetItem(LPCWSTR szTaskName, avScanTask::IScanTask** ppItem) throw()
{
	CCTRCTXI0(L"Enter");

	STAHLSOFT_HRX_TRY(hr)
	{
		hrx << ((!szTaskName || !ppItem)? E_INVALIDARG : S_OK);

		ccLib::CSingleLock csl(&m_Crit, INFINITE, FALSE);

		std::vector<avScanTask::IScanTask*>::iterator it;
		hrx << findItem(szTaskName, it);

		if(it != m_ScanTaskColl.end())
		{
			*ppItem = (*it);
			(*ppItem)->AddRef();
		}
	}
	STAHLSOFT_HRX_CATCH_ALL_NO_RETURN(hr)
	CCTRCTXI1(L"Exit. HR: 0x%08X", hr);
	return hr;
}
//-------------------------------------------------------------------------
HRESULT CScanTaskCollection::CreateItem(avScanTask::IScanTask** ppItem) throw()
{
	CCTRCTXI0(L"Enter");

	STAHLSOFT_HRX_TRY(hr)
	{
		hrx << (!ppItem? E_INVALIDARG : S_OK);

		ccLib::CSingleLock csl(&m_Crit, INFINITE, FALSE);

		*ppItem = NULL;

		ScanTask_Loader loader;
		IScanTaskPtr spScanTask;
		hrx << (SYM_SUCCEEDED(loader.CreateObject(&spScanTask))? S_OK : E_FAIL);

		// Only custom scans can be created.
		hrx << spScanTask->SetType(scanCustom);

		// You can edit and schedule custom scans!
		hrx << spScanTask->SetCanEdit(true);
		hrx << spScanTask->SetCanSchedule(true);

		// We don't like duplicate scan task names!
		bool bDuplicate = false;
		int iMode = 0;  // modeCreate

		//Saving off Current working directory, since the MS open file dlg changes it
		WCHAR szCurrentDir[MAX_PATH + 1] = {0};
		DWORD dwRet = 0;
		dwRet = GetCurrentDirectory(MAX_PATH + 1, szCurrentDir);
		if(0 == dwRet || dwRet > MAX_PATH + 1)
		{
			CCTRCTXE0(L"GetCurrentDirectory() failed");
		}

		// Start the Wizard and let it do it's magic. Poof!
		do 
		{
			CCTRCTXI0(L"Launching wizard");

			CTaskWizard NTW;
			hr = NTW.NAVTaskWizard(spScanTask, iMode);

			// S_OK: User clicked Finished on the dialog
			// S_FALSE: User cancelled out of the dialog

			if(S_OK == hr)
			{
				// Check for duplicate task names
				cc::IStringPtr pTaskName;
				hrx << spScanTask->GetName(&pTaskName);
				hrx << CheckDuplicateTaskName(pTaskName->GetStringW(), bDuplicate);

				if(bDuplicate)
				{
					iMode = 2;  // modeDuplicate

					// Tell the user they have a duplicate named task
					CStringW sError;
					sError.LoadString(IDS_ERR_DUP_NAME);
					MessageBox(GetDesktopWindow(), sError, m_sProductName, MB_OK|MB_ICONWARNING);
				}
			}
		}
		while(bDuplicate && (S_OK == hr));

		//Restoring current directory
		if(!SetCurrentDirectory(szCurrentDir))
		{
			CCTRCTXE0(L"SetCurrentDirectory() failed");
		}

		if(S_OK == hr)
		{
			// Save will generate a new custom scan name
			CStringW sFilePath;

			hrx << generateFilePath(sFilePath);

			hrx << spScanTask->SetPath(sFilePath);
			hr = spScanTask->Save();

			if(SUCCEEDED(hr))
			{
				// Add the new object to the collection.
				(*ppItem) = spScanTask;
				(*ppItem)->AddRef();
				m_ScanTaskColl.push_back(spScanTask.Detach());
			}
			else
			{
				// Tell the user we couldn't save the task
				CStringW sFormat;
				CStringW sError;
				sFormat.LoadString(IDS_ERR_CANT_SAVE_TASK);
				sError.Format(sFormat.GetString(), m_sProductName.GetString());

				MessageBox(GetDesktopWindow(), sError, m_sProductName, MB_OK|MB_ICONERROR);
			}
		}
	}
	STAHLSOFT_HRX_CATCH_ALL_NO_RETURN(hr)
	CCTRCTXI1(L"Exit. HR: 0x%08X", hr);
	return hr;
}
//-------------------------------------------------------------------------
HRESULT CScanTaskCollection::DeleteItem(LPCWSTR szTaskName)
{
	CCTRCTXI0(L"Enter");

	STAHLSOFT_HRX_TRY(hr)
	{
		hrx << (!szTaskName? E_INVALIDARG : S_OK);

		ccLib::CSingleLock csl(&m_Crit,	INFINITE, FALSE);

		// Find	the	item with the same name
		std::vector<IScanTask*>::iterator it;
		hrx << findItem(szTaskName, it);
		
		if(m_ScanTaskColl.end()	== it)
		{
			CCTRCTXE1(L"Task not found: %s", szTaskName);
			hrx << E_FAIL;
		}

		IScanTaskPtr spScanTask(*it);

		// If we aren't	allowed	to edit	it, don't delete it.
		bool bCanEdit =	false;
		hrx << spScanTask->GetCanEdit(bCanEdit);
		if(!bCanEdit)
		{
			CCTRCTXI0(L"Delete called on uneditable	task");
			return S_FALSE;
		}

		// Schedule, if	there is one
		if(FAILED(spScanTask->DeleteSchedule()))
		{
			CCTRCTXI0(L"DeleteSchedule failed probably because the task has not been scheduled");
		}
		
		cc::IStringPtr pTaskPath;
		hrx << spScanTask->GetPath(&pTaskPath);

		// Delete the file itself
		hr = deleteFileAgressive(pTaskPath->GetStringW());
		if(FAILED(hr))
		{
			CCTRCTXE0(L"Failed to delete the task file itself");
		}

		// Remove it from memory
		spScanTask.Release();
		(*it)->Release();
		m_ScanTaskColl.erase(it);
 	}
	STAHLSOFT_HRX_CATCH_ALL_NO_RETURN(hr)
	CCTRCTXI1(L"Exit. HR: 0x%08X", hr);
	return hr;
}
//-------------------------------------------------------------------------
HRESULT CScanTaskCollection::CheckDuplicateTaskName(LPCWSTR szTaskName, bool& bDuplicate) throw()
{
	CCTRCTXI0(L"Enter");
	STAHLSOFT_HRX_TRY(hr)
	{
		hrx << (!szTaskName? E_INVALIDARG : S_OK);

		ccLib::CSingleLock csl(&m_Crit,	INFINITE, FALSE);

		std::vector<IScanTask*>::iterator it;
		hrx << findItem(szTaskName, it);

		if(m_ScanTaskColl.end() != it)
			bDuplicate = true;
		else
			bDuplicate = false;
	}
	STAHLSOFT_HRX_CATCH_ALL_NO_RETURN(hr)
	CCTRCTXI1(L"Exit. HR: 0x%08X", hr);
	return hr;
}
//-------------------------------------------------------------------------
HRESULT CScanTaskCollection::DeleteAll(void)
{
	CCTRCTXI0(L"Enter");

	STAHLSOFT_HRX_TRY(hr)
	{
		ccLib::CSingleLock csl(&m_Crit, INFINITE, FALSE);

		// Remove scheduler associated with the tasks
		CComPtr<ITaskScheduler> spTaskScheduler;
		hrx << spTaskScheduler.CoCreateInstance(CLSID_CTaskScheduler, NULL, CLSCTX_INPROC_SERVER);

		CComPtr<IEnumWorkItems> spIEnum;
		hrx << spTaskScheduler->Enum(&spIEnum);

		LPWSTR *lpwszName = NULL;
		DWORD dwFetchedTasks = 0;

		while(SUCCEEDED(spIEnum->Next(1, &lpwszName, &dwFetchedTasks)) &&
			(dwFetchedTasks != 0))
		{
			// Compare front of schedule to "Norton AntiVirus"
			if(0 == wcsncmp(lpwszName[0], m_sProductName, m_sProductName.GetLength()))
			{
				// Skip checking result here since
				// we do not want to bail out just
				// because we fail to delete schedule of one task
				spTaskScheduler->Delete(lpwszName[0]);                
			}
		}

		CoTaskMemFree(lpwszName);

		// Now delete the built-in scan tasks. Leave the users'.
		ccLib::CStringW sTaskDir;
		ccLib::CStringW sFilePath;

		HANDLE hFindFile = NULL;
		WIN32_FIND_DATA FileData = {0};

		sTaskDir.Format(L"%s\\%s\\*.%s", m_NavInfo.GetNAVCommonDir(),
			TASK_SUBDIR, TASK_FILE_EXTENSION);

		if(INVALID_HANDLE_VALUE != (hFindFile = ::FindFirstFile(sTaskDir, &FileData)))
		{
			do 
			{
				// Skip directories
				if(FileData.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY)
					continue;

				// Setup path to the scan file
				sFilePath.Format(L"%s\\%s\\%s", m_NavInfo.GetNAVCommonDir(),
					TASK_SUBDIR, FileData.cFileName);

				CCTRCTXI1(L"Delete %s", sFilePath.GetString());

				// Delete the scan file
				// Skip checking result here since
				// we do not want to bail out just
				// because we fail to delete one task file
				deleteFileAgressive(sFilePath);

			}while(FindNextFile(hFindFile, &FileData));

			FindClose(hFindFile);
		}
	}
	STAHLSOFT_HRX_CATCH_ALL_NO_RETURN(hr)
	CCTRCTXI1(L"Exit. HR: 0x%08X", hr);
	return hr;
}
//-------------------------------------------------------------------------
HRESULT CScanTaskCollection::generateFilePath(CStringW& sFilePath)
{
	CCTRCTXI0(L"Enter");

	STAHLSOFT_HRX_TRY(hr)
	{
		// Generate a new custom scan name in the user's directory.
		//
		// Add the beginning of the full path "C:\documents and ..."
		CStringW sTemp(m_NavInfo.GetNAVUserDir());
		sTemp.Append(L"\\");
		sTemp.Append(TASK_SUBDIR);
		sTemp.Append(L"\\");

		WCHAR szTempPath[MAX_PATH] = {0};
		WCHAR szFilePath [MAX_PATH] = {0};
		WCHAR szPrefix[] = L"\0";

		if(-1 == GetFileAttributes(sTemp))
		{
			// The Tasks folder doesn't exist so make it.
			// If we fail to make it, we can't make the task so bail.
			if(!m_NavInfo.MakeNAVUserDir())
				hrx << E_FAIL;

			if(!CreateDirectory(sTemp, NULL))
				hrx << E_FAIL;
		}

		if(GetTempFileName(sTemp, szPrefix, 0, szTempPath))
		{
			// change the file extension to '.scn' instead of '.tmp' so the shell will associate it
    		// with NAVW.EXE
			wcscpy(szFilePath, szTempPath);
			wcscpy(wcsrchr(szFilePath, L'.'), L".");
			wcscat(szFilePath, TASK_FILE_EXTENSION);

			// It actually creates the .tmp file, which we don't want. We will 
			// rename the .tmp file to a .scn file.
			if(!MoveFile(szTempPath, szFilePath))
			{
				// .SCA file already exists.
				// Don't leave the .tmp file around.
				DeleteFile(szTempPath);
				hrx << E_FAIL;
			}

			sFilePath = szFilePath;
		}
 	}
	STAHLSOFT_HRX_CATCH_ALL_NO_RETURN(hr)
	CCTRCTXI1(L"Exit. HR: 0x%08X", hr);
	return hr;
}
//-------------------------------------------------------------------------
HRESULT CScanTaskCollection::emptyTaskList()
{
	std::vector<IScanTask*>::iterator it;

	for(it = m_ScanTaskColl.begin(); it != m_ScanTaskColl.end(); ++it)
	{
		dynamic_cast<IScanTask*>(*it)->Release();
	}
		
	m_ScanTaskColl.clear();

	return S_OK;
}
//-------------------------------------------------------------------------
HRESULT CScanTaskCollection::addTasks(LPCWSTR szDir)
{
	CCTRCTXI1(L"Enter. Load scan tasks from %s", szDir);

	STAHLSOFT_HRX_TRY(hr)
	{
		hrx << (!szDir? E_INVALIDARG : S_OK);

		// Each .scan file in the Tasks directory gets listed in the scan
		// task list.
		WCHAR szTasksDir[MAX_PATH]= {0};
		HANDLE hFindFile = 0;
		WIN32_FIND_DATA FileData = { 0 };
		ScanTask_Loader loader;
		SYMRESULT sr = SYM_OK;

		wcscpy(szTasksDir, szDir);
		wcscat(szTasksDir, L"\\");
		wcscat(szTasksDir, TASK_SUBDIR);
		wcscat(szTasksDir, L"\\*.");
		wcscat(szTasksDir, TASK_FILE_EXTENSION);

		hFindFile = FindFirstFile(szTasksDir, &FileData);

		// If there is no .sca files, return.
		if(INVALID_HANDLE_VALUE == hFindFile)
			return S_OK;

		do
		{
			// Skip directories
			if(FILE_ATTRIBUTE_DIRECTORY == FileData.dwFileAttributes)
				continue;

			// Load the task
			wcscpy(wcsrchr(szTasksDir, L'\\') + 1, FileData.cFileName);
			CCTRCTXI1(L"Load %s", szTasksDir);

			IScanTaskPtr spScanTask;
			sr = loader.CreateObject(&spScanTask);
			if(SYM_SUCCEEDED(sr))
			{
				// Don't return if the load fails. Why? Otherwise if you have one
				// bad/corrupted task the whole list is empty.
				if(SUCCEEDED(spScanTask->Load(szTasksDir, false)))
				{
					// Add the new object to the collection.
					m_ScanTaskColl.push_back(spScanTask.Detach());
				}
			}
		} 
		while(FindNextFile(hFindFile, &FileData));

		FindClose(hFindFile);
	}
	STAHLSOFT_HRX_CATCH_ALL_NO_RETURN(hr)
	CCTRCTXI1(L"Exit. HR: 0x%08X", hr);
	return hr;
}
//-------------------------------------------------------------------------
// This function will try to remove the read-only attrib on a file
// if it is set, prior to deleting it.
//-------------------------------------------------------------------------
HRESULT CScanTaskCollection::deleteFileAgressive(LPCWSTR szFilePath)
{
	if(!szFilePath)
		return E_INVALIDARG;

    // Try to remove the read-only attrib if it is set
    DWORD dwFileAttribs = 0;
    dwFileAttribs = GetFileAttributes(szFilePath);
    
    if(-1 != dwFileAttribs)
    {
        if(dwFileAttribs & FILE_ATTRIBUTE_READONLY)
        {
            // File is marked read-only, let's try to fix that
            dwFileAttribs = dwFileAttribs & (~FILE_ATTRIBUTE_READONLY);
            if(!SetFileAttributes(szFilePath, dwFileAttribs))
				return E_FAIL;
        }
    }

    // Truncate the file to 0 bytes so the NProtect doesn't
    // put it in the protected Recycle Bin
    FILE * pFile = 0;
    pFile =_wfopen(szFilePath, L"w");

    if(pFile)
		fclose(pFile);

    // Try delete
	if(DeleteFile(szFilePath))
		return S_OK;
	else
		return E_FAIL;
}
//-------------------------------------------------------------------------
HRESULT CScanTaskCollection::findItem(LPCWSTR szTaskName, std::vector<avScanTask::IScanTask*>::iterator& it)
{
	for(it = m_ScanTaskColl.begin(); it!=m_ScanTaskColl.end(); ++it)
	{
		cc::IStringPtr spName;
		(*it)->GetName(&spName);

		// Check for matching name
		if(0 == _wcsicmp(spName->GetStringW(), szTaskName))
			break;
	}

	return S_OK;
}
