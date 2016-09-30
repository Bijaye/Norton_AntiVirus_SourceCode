#include "StdAfx.h"
#include "scanitems.h"
#include "scanManager.h"

CScanItems::CScanItems(void)
{
}

CScanItems::~CScanItems(void)
{
}

// Initialization
void CScanItems::Initialize(CScanManager* pScanMgr)
{
    if( pScanMgr )
        m_pScanMgr = pScanMgr;
}

// De-Initialization
void CScanItems::DeInitialize()
{
    if( m_pScanMgr )
        m_pScanMgr = NULL;
}

// IScanItems implementation
int CScanItems::GetScanItemCount()
{
    if( m_pScanMgr )
        return m_pScanMgr->GetScanItemCount();

    CCTRACEE(_T("CScanItems::GetScanItemCount() - Scan manager invalid."));

    return 0;
}

const char* CScanItems::GetScanItemPath(int iIndex)
{
    if( m_pScanMgr )
        return m_pScanMgr->GetScanItemPath(iIndex);

    CCTRACEE(_T("CScanItems::GetScanItemPath() - Scan manager invalid."));

    return NULL;
}