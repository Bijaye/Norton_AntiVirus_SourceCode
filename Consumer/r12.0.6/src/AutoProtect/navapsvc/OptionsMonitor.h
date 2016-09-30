// OptionsMonitor.h: interface for the COptionsMonitor class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OPTIONSMONITOR_H__ACAE1C1F_70F6_4100_95FA_83600CE55678__INCLUDED_)
#define AFX_OPTIONSMONITOR_H__ACAE1C1F_70F6_4100_95FA_83600CE55678__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ccThread.h"

class COptionsMonitorThread : public ccLib::CThread  
{
public:
	COptionsMonitorThread();
	virtual ~COptionsMonitorThread();

	int Run();
};

#endif // !defined(AFX_OPTIONSMONITOR_H__ACAE1C1F_70F6_4100_95FA_83600CE55678__INCLUDED_)
