// StatusVirusDefs.cpp: implementation of the CStatusVirusDefs class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "StatusVirusDefs.h"
#include "time.h"
#include "atltime.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CStatusVirusDefs::CStatusVirusDefs()
{
    m_lEventID = AV::Event_ID_StatusDefs;
}

CStatusVirusDefs::~CStatusVirusDefs()
{
}

/////////////////////////////////////////////////////////////////////////////
// CDefsStatus member functions

void CStatusVirusDefs::MakeStatus() 
{
    CCTRACEI ("CStatusVirusDefs::MakeStatus()");
    
    long lStatus = AVStatus::statusError;
    m_edMasterData.GetData ( AVStatus::propDefsStatus, lStatus ); 

    if (lStatus == AVStatus::statusNotInstalled || lStatus == AVStatus::statusError)
	{
		CCTRACEW("VD status unknown");
        return;
	}

    // Reset the properties
    //
    m_edMasterData.SetData ( AVStatus::propDefsAge, (long) 0 ); 

    // Time in secs / secs / hours / days
    time_t timeDefTime = 0;

    // Get def date
    //
    if ( !m_edMasterData.GetData ( AVStatus::propDefsTime, (time_t) timeDefTime ))
	{
		CCTRACEW("Could not find Defs time!");
        return;
	}

    ATL::CTime timeDefs (timeDefTime);
    ATL::CTimeSpan timespan = ATL::CTime::GetCurrentTime() - timeDefs;

	DWORD dwDays = (DWORD)timespan.GetDays ();

	CCTRACEI("VD age %d days, (%s)", dwDays, timeDefs.Format("%#c"));

    m_edMasterData.SetData ( AVStatus::propDefsAge, dwDays);

	return;
}
