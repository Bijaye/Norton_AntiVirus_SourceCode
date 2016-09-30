////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

// TaskWizard.cpp : implementation file
//
#include "stdafx.h"
#include "ScanTask.h"
#include "NAVTrust.h"
#include "TaskWizard.h"
#include "NAVInfo.h"

//-------------------------------------------------------------------------
CTaskWizard::CTaskWizard()
{
	STAHLSOFT_HRX_TRY(hr)
    {
        m_pfnTaskWizard = NULL;
        m_pfnFolderDialog = NULL;
        m_pfnDrivesDialog = NULL;

        CNAVInfo navInfo;
		CStringW sNavTaskWizard;
		sNavTaskWizard.Format(L"%s\\navtskwz.dll", navInfo.GetNAVDir());

		hrx << ((NAVToolbox::NAVTRUST_OK != NAVToolbox::IsSymantecSignedImage(sNavTaskWizard))? E_FAIL : S_OK);

	    m_hmodNAVTaskWiz = LoadLibraryEx(sNavTaskWizard, NULL, LOAD_WITH_ALTERED_SEARCH_PATH);
		hrx << (!m_hmodNAVTaskWiz? E_POINTER : S_OK);

	    m_pfnTaskWizard = (pfnNAVTaskWizard) GetProcAddress(m_hmodNAVTaskWiz, "NAVTaskWizard");
        m_pfnFolderDialog = (pfnFolderDialog) GetProcAddress(m_hmodNAVTaskWiz, "FolderDialog");
        m_pfnDrivesDialog = (pfnDrivesDialog) GetProcAddress(m_hmodNAVTaskWiz, "DrivesDialog");
    }
	STAHLSOFT_HRX_CATCH_ALL_NO_RETURN(hr)
}
//-------------------------------------------------------------------------
CTaskWizard::~CTaskWizard()
{
    if(m_hmodNAVTaskWiz)
    {
        FreeLibrary(m_hmodNAVTaskWiz);
        m_hmodNAVTaskWiz = NULL;
    }
}
//-------------------------------------------------------------------------
HRESULT CTaskWizard::NAVTaskWizard(avScanTask::IScanTask* pNAVScanTask, int iMode)
{
    if(!m_hmodNAVTaskWiz ||
		!m_pfnTaskWizard)
        return E_FAIL;
    
    return m_pfnTaskWizard(pNAVScanTask, iMode);
}
//-------------------------------------------------------------------------
HRESULT CTaskWizard::FolderDialog(avScanTask::IScanTask* pNAVScanTask)
{
    if(!m_hmodNAVTaskWiz ||
		!m_pfnFolderDialog)
        return E_FAIL;

    return m_pfnFolderDialog(pNAVScanTask); 
}
//-------------------------------------------------------------------------
HRESULT CTaskWizard::DrivesDialog(avScanTask::IScanTask* pNAVScanTask)
{
    if(!m_hmodNAVTaskWiz ||
		!m_pfnDrivesDialog)
        return E_FAIL;

    return m_pfnDrivesDialog(pNAVScanTask); 
}
