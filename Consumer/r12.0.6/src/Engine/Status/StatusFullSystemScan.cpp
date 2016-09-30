// StatusFullSystemScan.cpp: implementation of the CStatusFullSystemScan class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "StatusFullSystemScan.h"
#include "ATLComTime.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CStatusFullSystemScan::CStatusFullSystemScan()
{
    m_lEventID = AV::Event_ID_StatusFSS;
}

CStatusFullSystemScan::~CStatusFullSystemScan()
{

}

void CStatusFullSystemScan::MakeStatus ()
{

    // Figure out how many days since FSS
    //
	long value = 0;

    DATE dateFSS = 0;
    long lSize = sizeof (DATE);
    if (m_edMasterData.GetData ( AVStatus::propFSSDate, (BYTE*)&dateFSS, lSize ))
	{
        ATL::COleDateTime timeFSS (dateFSS);
		CCTRACEI("Last Scan: %s", (LPCTSTR)timeFSS.Format("%#c"));

        ATL::COleDateTimeSpan timespan = ATL::COleDateTime::GetCurrentTime() - timeFSS;
        value = (long)timespan.GetDays();
	}
	else
	{
		CCTRACEE("Failed get AVStatus::szFSSDate");
	}

    m_edMasterData.SetData ( AVStatus::propFSSAge, value );
}