#include "StdAfx.h"
#include "scansink.h"
#include "ScanManager.h"

CScanSink::CScanSink(void)
{
}

CScanSink::~CScanSink(void)
{
}

// Initialization
void CScanSink::Initialize(CScanManager* pScanMgr)
{
    if( pScanMgr )
        m_pScanMgr = pScanMgr;
}

// De-Initialization
void CScanSink::DeInitialize()
{
    if( m_pScanMgr )
        m_pScanMgr = NULL;
}

// IScanSink overrides.
SCANSTATUS CScanSink::OnBusy()
{
    if( m_pScanMgr )
    {
        return m_pScanMgr->OnBusy();
    }

    return SCAN_OK;
}

SCANSTATUS CScanSink::OnError(IScanError* pError)
{
    if( m_pScanMgr )
    {
        return m_pScanMgr->OnError(pError);
    }

    return SCAN_OK;
}

SCANSTATUS CScanSink::OnNewItem(const char* pszItem)
{
    if( m_pScanMgr )
    {
        return m_pScanMgr->OnNewItem(pszItem);
    }

    return SCAN_OK;
}

SCANSTATUS CScanSink::OnNewFile(const char* pszLongName, const char* pszShortName)
{
    // Now we implement IScanSink3 and recieve the OnNewFile2() callback
    _ASSERT(FALSE);
    return SCAN_OK;
}

SCANSTATUS CScanSink::OnNewDirectory(const char* pszLongName, const char* pszShortName)
{
    // Now we implement IScanSink3 and recieve the OnNewDirectory2() callback
    _ASSERT(FALSE);
    return SCAN_OK;
}

SCANSTATUS CScanSink::OnInfectionFound(IScanInfection* pInfection)
{
    if( !m_pScanMgr )
    {
        CCTRACEE(_T("CScanSink::OnInfectionFound() - No scan manager to give infection to"));
        return SCAN_FALSE;
    }

    // Notify AP of the detection
    m_pScanMgr->NotifyAP(pInfection);

    // Set the initial infection status to unhandled
    pInfection->SetUserData(InfectionStatus_Unhandled);

    // Process this infection now that we are the only thread running
    return m_pScanMgr->OnInfectionFound(pInfection);
}

SCANSTATUS CScanSink::OnRepairFailed(IScanInfection* pInfection)
{
    if( !m_pScanMgr )
    {
        CCTRACEE(_T("CScanSink::OnRepairFailed() - No scan manager to give infection to"));
        return SCAN_FALSE;
    }

    return m_pScanMgr->OnRepairFailed(pInfection);
}

SCANSTATUS CScanSink::OnRemoveMimeComponent(IScanInfection* pInfection, char* pszMessage,
                                                  int iMessageBufferSize)
{
    // This only does work on email scans which are single threaded, so synchronization
    // is not performed.

    if( m_pScanMgr )
    {
        return m_pScanMgr->OnRemoveMimeComponent(pInfection, pszMessage, iMessageBufferSize);
    }

    return SCAN_OK;
}

SCANSTATUS CScanSink::LockVolume(const char* szVolume, bool bLock)
{
    return SCAN_OK;
}

SCANSTATUS CScanSink::OnCleanFile(const char* pszFileName, const char* pszTempFileName)
{
    if( m_pScanMgr )
    {
        return m_pScanMgr->OnCleanFile(pszFileName, pszTempFileName);
    }

    return SCAN_OK;
}

// IScanSink2 overrides
SCANSTATUS CScanSink::OnBeginThread()
{
    if( m_pScanMgr )
        return m_pScanMgr->OnBeginThread();

    return SCAN_OK;
}

SCANSTATUS CScanSink::OnEndThread()
{
    if( m_pScanMgr )
        return m_pScanMgr->OnEndThread();

    return SCAN_OK;
}

// IScanSink3 overrides
//
// Called for each new file found.  Return values:
//      SCAN_OK - process the found file normally.
//      SCAN_FALSE - skip this file.
//      SCAN_ABORT - abort the scan process now.
//		lAttribute is the translated attributes defined in enum eFileAttributes
//
SCANSTATUS CScanSink::OnNewFile2( const char * pszLongName, const char * pszShortName, ULONGLONG lAttribute )
{
    SCANSTATUS retStatus = SCAN_OK;

    if( m_pScanMgr )
    {
        retStatus = m_pScanMgr->OnNewFile(pszLongName, pszShortName);
    }

    return retStatus;
}

//
// Called for each new directory found.  Return values:
//      SCAN_OK - process the found directory normally.
//      SCAN_FALSE - skip this directory.
//      SCAN_ABORT - abort the scan process now.
//		lAttribute is the translated attributes defined in enum eFileAttributes
//
SCANSTATUS CScanSink::OnNewDirectory2( const char * pszLongName, const char * pszShortName, ULONGLONG lAttribute )
{
    if( m_pScanMgr )
    {
        return m_pScanMgr->OnNewDirectory(pszLongName, pszShortName);
    }

    return SCAN_OK;
}

//
// Called when failed to delete an infected file.  Return values:
//      SCAN_OK - no action will be taken.
//      SCAN_ABORT - abort the scan process now.
SCANSTATUS CScanSink::OnDeleteFailed( IScanInfection* pInfection )
{
    if( !m_pScanMgr )
    {
        CCTRACEE(_T("CScanSink::OnDeleteFailed() - No scan manager to give infection to"));
        return SCAN_FALSE;
    }

    return m_pScanMgr->OnDeleteFailed(pInfection);
}

//
// Called when a container is completed for scanning.  This is only called for top level containers and
// is not called for nested containers or for files that are not containers.  Return values:
//      SCAN_OK - no action will be taken.
//      SCAN_DELETE - Delete the container file.
//      SCAN_ABORT - abort the scan process now.
SCANSTATUS CScanSink::OnPostProcessContainer( const char * pszLongName, const char * pszShortName )
{
    if( m_pScanMgr )
    {
        m_pScanMgr->OnPostProcessContainer(pszLongName, pszShortName);
    }

    return SCAN_OK;
}

//
// Called when a new child is found in a compressed file
//      SCAN_OK - process the child normally.
//      SCAN_FALSE - skip this child.
//      SCAN_ABORT - abort the scan process now.
SCANSTATUS CScanSink::OnNewCompressedChild( ::cc::IDeccomposerContainerObject * pIDecomposerContainerObject, const char * pszChildName )
{
    if( m_pScanMgr )
    {
        return m_pScanMgr->OnNewCompressedChild(pIDecomposerContainerObject, pszChildName);
    }

    return SCAN_OK;
}
