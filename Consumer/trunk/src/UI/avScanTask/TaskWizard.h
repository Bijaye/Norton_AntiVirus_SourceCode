////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

// TaskWizard.h : header/implementation file
//
#pragma once

// Helper class for calling NAV Task Wizard
typedef HRESULT (WINAPI *pfnNAVTaskWizard)(avScanTask::IScanTask*, int);
typedef HRESULT (WINAPI *pfnFolderDialog)(avScanTask::IScanTask*);
typedef HRESULT (WINAPI *pfnDrivesDialog)(avScanTask::IScanTask*);

class CTaskWizard  
{
public:
	CTaskWizard();
	virtual ~CTaskWizard();

	HRESULT NAVTaskWizard(avScanTask::IScanTask* pScanTask, int iMode);
    HRESULT FolderDialog(avScanTask::IScanTask* pScanTask);
    HRESULT DrivesDialog (avScanTask::IScanTask* pScanTask);

protected:
    HMODULE m_hmodNAVTaskWiz;

    pfnNAVTaskWizard m_pfnTaskWizard;
    pfnFolderDialog m_pfnFolderDialog;
    pfnDrivesDialog m_pfnDrivesDialog;
};

