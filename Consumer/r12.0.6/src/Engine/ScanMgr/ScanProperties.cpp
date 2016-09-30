#include "StdAfx.h"
#include "scanproperties.h"
#include "ScanManager.h"

CScanProperties::CScanProperties(void)
{
}

CScanProperties::~CScanProperties(void)
{
}

// Initialization
void CScanProperties::Initialize(CScanManager* pScanMgr)
{
    if( pScanMgr )
        m_pScanMgr = pScanMgr;
}

// De-Initialization
void CScanProperties::DeInitialize()
{
    if( m_pScanMgr )
        m_pScanMgr = NULL;
}

int CScanProperties::GetProperty(const char* szProperty, int iDefault)
{
    if( m_pScanMgr )
        return m_pScanMgr->GetProperty(szProperty, iDefault);

    CCTRACEE(_T("CScanProperties::GetProperty() - No valid scan manager returning default value."));

    return iDefault;
}

const char* CScanProperties::GetProperty(const char* szProperty, const char* szDefault)
{
    if( m_pScanMgr )
        return m_pScanMgr->GetProperty(szProperty, szDefault);

    CCTRACEE(_T("CScanProperties::GetProperty() - No valid scan manager returning default value."));

    return szDefault;
}