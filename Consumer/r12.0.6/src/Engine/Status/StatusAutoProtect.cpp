// StatusAutoProtect.cpp: implementation of the CStatusAutoProtect class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "StatusAutoProtect.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CStatusAutoProtect::CStatusAutoProtect()
{
    m_lEventID = AV::Event_ID_StatusAP;
}

CStatusAutoProtect::~CStatusAutoProtect()
{
}

void CStatusAutoProtect::MakeStatus() 
{
    CCTRACEI ("CStatusAutoProtect::MakeStatus()");
    
    m_edMasterData.SetData ( AVStatus::propAPCanEnable, (long) g_bIsAdminProcess);
}