#include "StdAfx.h"
#include "EraserScanSink.h"
#include "ScanManager.h"

// Constructor.
CEraserScanSink::CEraserScanSink()
{
}

// Destructor
CEraserScanSink::~CEraserScanSink()
{
}

// Initialization
void CEraserScanSink::Initialize(CScanManager* pScanMgr)
{
    if( pScanMgr )
        m_pScanMgr = pScanMgr;
}


// De-Initialization
void CEraserScanSink::DeInitialize()
{
    if( m_pScanMgr )
        m_pScanMgr = NULL;
}

// IScanSink overrides.
SCANSTATUS CEraserScanSink::OnBusy()
{
    if( m_pScanMgr )
    {
        return m_pScanMgr->OnBusy();
    }

    return SCAN_OK;
}

SCANSTATUS CEraserScanSink::OnError(IScanError* pError)
{
    if( m_pScanMgr )
    {
        return m_pScanMgr->OnError(pError);
    }

    return SCAN_OK;
}

SCANSTATUS CEraserScanSink::OnNewItem(const char* pszItem)
{
    if( m_pScanMgr )
    {
        return m_pScanMgr->OnNewItem(pszItem);
    }

    return SCAN_OK;
}

SCANSTATUS CEraserScanSink::OnNewFile(const char* pszLongName, const char* pszShortName)
{
    if( m_pScanMgr )
    {
        return m_pScanMgr->OnNewFile(pszLongName, pszShortName);
    }

    return SCAN_OK;
}

SCANSTATUS CEraserScanSink::OnNewDirectory(const char* pszLongName, const char* pszShortName)
{
    return SCAN_OK;
}

SCANSTATUS CEraserScanSink::OnInfectionFound(IScanInfection* pInfection)
{
    // Set the initial infection status to unhandled
    pInfection->SetUserData(InfectionStatus_Unhandled);

    // Notify Auto-Protect of the infection
    if( m_pScanMgr )
        m_pScanMgr->NotifyAP(pInfection);

    return SCAN_FALSE;
}

SCANSTATUS CEraserScanSink::OnRepairFailed(IScanInfection* pInfection)
{
    return SCAN_OK;
}

SCANSTATUS CEraserScanSink::OnRemoveMimeComponent(IScanInfection* pInfection, char* pszMessage,
    int iMessageBufferSize)
{
    if( m_pScanMgr )
    {
        return m_pScanMgr->OnRemoveMimeComponent(pInfection, pszMessage, iMessageBufferSize);
    }

    return SCAN_OK;
}

SCANSTATUS CEraserScanSink::LockVolume(const char* szVolume, bool bLock)
{
    return SCAN_OK;
}

SCANSTATUS CEraserScanSink::OnCleanFile(const char* pszFileName, const char* pszTempFileName)
{
    return SCAN_OK;
}

// IScanSink2 overrides
SCANSTATUS CEraserScanSink::OnBeginThread()
{
    return SCAN_OK;
}

SCANSTATUS CEraserScanSink::OnEndThread()
{
    return SCAN_OK;
}
