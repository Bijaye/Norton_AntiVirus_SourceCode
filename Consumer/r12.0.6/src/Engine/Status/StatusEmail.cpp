// StatusEmail.cpp: implementation of the CStatusEmail class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "StatusEmail.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CStatusEmail::CStatusEmail()
{
    m_lEventID = AV::Event_ID_StatusEmail;
}

CStatusEmail::~CStatusEmail()
{
}


void CStatusEmail::MakeStatus()
{
    // Save data
    //
    m_edMasterData.SetData ( AVStatus::propEmailCanEnable, g_bIsAdminProcess );
}