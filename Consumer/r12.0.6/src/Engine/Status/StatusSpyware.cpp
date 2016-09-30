#include "stdafx.h"
#include "StatusSpyware.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CStatusSpyware::CStatusSpyware()
{
    m_lEventID = AV::Event_ID_StatusSpyware;
}

CStatusSpyware::~CStatusSpyware()
{
}

void CStatusSpyware::MakeStatus() 
{
	// Save data
    //
    m_edMasterData.SetData ( AVStatus::propSpywareCanEnable, g_bIsAdminProcess );
}