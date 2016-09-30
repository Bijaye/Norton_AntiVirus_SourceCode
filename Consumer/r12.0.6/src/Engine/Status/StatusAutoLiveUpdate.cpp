// StatusAutoLiveUpdate.cpp: implementation of the CStatusAutoLiveUpdate class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "StatusAutoLiveUpdate.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CStatusAutoLiveUpdate::CStatusAutoLiveUpdate()
{
    m_lEventID = AV::Event_ID_StatusALU;
}

CStatusAutoLiveUpdate::~CStatusAutoLiveUpdate()
{
}

void CStatusAutoLiveUpdate::MakeStatus() 
{
    CCTRACEI ("CStatusAutoLiveUpdate::MakeStatus()");

    m_edMasterData.SetData ( AVStatus::propALUCanEnable, (long) g_bIsAdminProcess);

    CCTRACEI("Can enable ALU: %d", (DWORD)g_bIsAdminProcess);
}
