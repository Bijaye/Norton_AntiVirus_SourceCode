// StatusItem.cpp: implementation of the CStatusItem class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "StatusItem.h"
#include "StatusPropertyNames.h"    // Names of Settings keys

// Static inits
ccLib::CCriticalSection CStatusItem::m_critMasterData;
CEventData CStatusItem::m_edMasterData;

/////////////////////////////////////////////////////////////////////////////
// CStatusItem

CStatusItem::CStatusItem():m_bInit (false) 
{    
}

CStatusItem::~CStatusItem()
{
}

// Make our own copy of this new data. Should only get called during init.
//
void CStatusItem::Initialize ( CEventData& edNewData )
{
    ccLib::CSingleLock lock ( &m_critMasterData, INFINITE, FALSE );
    m_edMasterData = edNewData;
}

void CStatusItem::UpdateStatus ()
{
    ccLib::CSingleLock lock ( &m_critMasterData, INFINITE, FALSE );

    // Update this item.
    //
    MakeStatus ();
    m_bInit = true;   
}


