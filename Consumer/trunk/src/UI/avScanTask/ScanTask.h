////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

// ScanTask.h : header file
//
#pragma once

#include <list>
#include "ScanTaskInterface.h"
#include "ScanItem.h"
#include "ccCriticalSection.h"
#include "ccSymStringImpl.h"

//-------------------------------------------------------------------------
class CScanTask: 
	public ISymBaseImpl<CSymThreadSafeRefCount>,
	public avScanTask::IScanTask
{
public:
	SYM_INTERFACE_MAP_BEGIN()                
		SYM_INTERFACE_ENTRY(avScanTask::IID_IScanTask, avScanTask::IScanTask)
	SYM_INTERFACE_MAP_END()

    // Constructor/Destructor
	CScanTask();
    virtual ~CScanTask();

	// IScanTask
	virtual HRESULT AddItem(avScanTask::SCANITEMTYPE type, avScanTask::SCANITEMSUBTYPE subtype, 
		WCHAR chDrive, LPCWSTR szPath, ULONG ulVID) throw();
	virtual HRESULT EnumReset() throw();
	virtual HRESULT EnumNext(avScanTask::SCANITEMTYPE& type, avScanTask::SCANITEMSUBTYPE& subtype, 
		WCHAR &chDrive, cc::IString** sPath, ULONG& ulVID) throw();
	virtual HRESULT GetType(avScanTask::SCANTASKTYPE& type) throw();
	virtual HRESULT SetType(avScanTask::SCANTASKTYPE type) throw();
	virtual HRESULT GetItemCount(DWORD& dwCount) throw();
	virtual HRESULT GetDriveCount(DWORD& dwCount) throw();
	virtual HRESULT GetFolderCount(DWORD& dwCount) throw();
	virtual HRESULT GetFileCount(DWORD& dwCount) throw();
	virtual HRESULT GetFileVIDCount(DWORD& dwCount) throw();
	virtual HRESULT GetLastRunTime(time_t& time) throw() ;
	virtual HRESULT SetLastRunTime(const time_t& time) throw();
	virtual HRESULT GetLastRunDate(DATE& lastRunDate) throw();
	virtual HRESULT SetName (LPCWSTR szName) throw();			
	virtual HRESULT GetName(cc::IString** sName) throw();
	virtual HRESULT SetPath(LPCWSTR szPath) throw();			
	virtual HRESULT GetPath(cc::IString** sPath) throw();
	virtual HRESULT GetCanSchedule(bool& bSchedule) throw();
	virtual HRESULT SetCanSchedule(bool bCanSchedule) throw();
	virtual HRESULT GetCanEdit(bool& bCanEdit) throw();		
	virtual HRESULT SetCanEdit(bool bCanEdit) throw();
	virtual HRESULT IsEmpty(bool& bEmpty) throw();
	virtual HRESULT Empty() throw();
	virtual HRESULT Save(void) throw();
	virtual HRESULT Load(LPCWSTR szFileName, bool bItems=true) throw();
	virtual HRESULT GetScheduled(avScanTask::EScheduled&) throw();		
	virtual HRESULT ScheduleTask(bool bWeekly) throw();
	virtual HRESULT Edit(void) throw();
	virtual HRESULT DeleteSchedule() throw(); 
	virtual HRESULT Scan(void) throw(); 
    virtual HRESULT Scan(LPCWSTR szScanSwitches) throw(); 
    virtual HRESULT Scan(HANDLE* phWait, LPCWSTR szScanSwitches) throw(); 

protected:
	// Reset the task object.
	HRESULT reset();

	// Construct task name in the format 
	// "ProductName - TaskName - UserName"
	HRESULT getScheduleName(CComBSTR& bstrName);

	// Scheduler a task
	HRESULT makeSchedule(bool bDefaultWeekly);

	// Change task name appear in the scheduler
	HRESULT renameSchedule(CComBSTR bstrOldScheduleName);

	// Show MS scheduler wizard
	HRESULT showSchedule();

	// Vista support
	// Set full access to the given file for current logged on user
	HRESULT setFullAccessToFile(LPCWSTR lpszFile);

protected:
	// Critical section for m_Items
	ccLib::CCriticalSection m_Crit;

	// List of scan items.
	std::list<CScanItem> m_Items;

	// Iterator used to traverse the scan items list.
	std::list<CScanItem>::iterator m_Iterator;

	// Task type.
	avScanTask::SCANTASKTYPE m_Type;

	// Time the task was created/last modified/last run.
	time_t m_timeLastRun;
	
	// Number of drives, folders, files, and file\VID items.
	DWORD m_uDriveCnt;
	DWORD m_uFolderCnt;
	DWORD m_uFileCnt;
    DWORD m_uFileVIDCnt;

	// Task file version.
    DWORD m_wVersionMajor;
    DWORD m_wVersionMinor;
    DWORD m_wVersionInline;

    // Name of Task - e.g. "My Computer" - translated!
    ccLib::CStringW m_sName;

	// Branding product name
	ccLib::CStringW m_sProductName;

    // Full path to the task file
    ccLib::CStringW m_sPath;

    // Is the user allowed to edit this?
    bool m_bCanEdit;

    // Is the user allowed to schedule this?
    bool m_bCanSchedule;
};
