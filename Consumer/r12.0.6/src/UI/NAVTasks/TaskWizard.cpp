// TaskWizard.cpp: implementation of the CTaskWizard class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "ScanTask.h"
#include "NAVTrust.h"
#include "TaskWizard.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTaskWizard::CTaskWizard()
{
    try
    {
        m_pfnTaskWizard = NULL;
        m_pfnFolderDialog = NULL;
        m_pfnDrivesDialog = NULL;

        CNAVInfo navInfo;
        TCHAR szNavEventPath [MAX_PATH] = {0};

        _stprintf ( szNavEventPath, "%s\\navtskwz.dll", navInfo.GetNAVDir() );

        if( NAVToolbox::NAVTRUST_OK != NAVToolbox::IsSymantecSignedImage( szNavEventPath ))
            throw FALSE;

	    m_hmodNAVTaskWiz = LoadLibraryEx( szNavEventPath, NULL, LOAD_WITH_ALTERED_SEARCH_PATH );

        if( m_hmodNAVTaskWiz == NULL )
            throw FALSE;

	    m_pfnTaskWizard = (pfnNAVTaskWizard) GetProcAddress( m_hmodNAVTaskWiz, "NAVTaskWizard" );
        m_pfnFolderDialog = (pfnFolderDialog) GetProcAddress( m_hmodNAVTaskWiz, "FolderDialog" );
        m_pfnDrivesDialog = (pfnDrivesDialog) GetProcAddress( m_hmodNAVTaskWiz, "DrivesDialog" );
    }
    catch(...)
    {
    }
}

CTaskWizard::~CTaskWizard()
{
    if ( m_hmodNAVTaskWiz )
    {
        FreeLibrary ( m_hmodNAVTaskWiz );
        m_hmodNAVTaskWiz = NULL;
    }
}

HRESULT CTaskWizard::NAVTaskWizard ( IScanTask* pNAVScanTask, int iMode )
{
    if ( !m_hmodNAVTaskWiz ||
         !m_pfnTaskWizard )
        return E_FAIL;
    
    return m_pfnTaskWizard ( pNAVScanTask, iMode );
}

HRESULT CTaskWizard::FolderDialog ( IScanTask* pNAVScanTask )
{
    if ( !m_hmodNAVTaskWiz ||
         !m_pfnFolderDialog )
        return E_FAIL;

    return m_pfnFolderDialog (pNAVScanTask); 
}

HRESULT CTaskWizard::DrivesDialog ( IScanTask* pNAVScanTask )
{
    if ( !m_hmodNAVTaskWiz ||
         !m_pfnDrivesDialog )
        return E_FAIL;

    return m_pfnDrivesDialog (pNAVScanTask); 
}
