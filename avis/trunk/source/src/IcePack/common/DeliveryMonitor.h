// DeliveryMonitor.h: interface for the DeliveryMonitor class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DELIVERYMONITOR_H__E4E7A0A3_C228_11D2_AD54_00A0C9C71BBC__INCLUDED_)
#define AFX_DELIVERYMONITOR_H__E4E7A0A3_C228_11D2_AD54_00A0C9C71BBC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <CMclThread.h>

class DeliveryMonitor : public CMclThreadHandler  
{
public:
	DeliveryMonitor();
	virtual ~DeliveryMonitor();

	unsigned	ThreadHandlerProc();
};

#endif // !defined(AFX_DELIVERYMONITOR_H__E4E7A0A3_C228_11D2_AD54_00A0C9C71BBC__INCLUDED_)
