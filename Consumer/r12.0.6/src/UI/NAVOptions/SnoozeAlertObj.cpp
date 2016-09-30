// SnoozeAlertObj.cpp : Implementation of CSnoozeAlert

#include "stdafx.h"
#include "SnoozeAlertObj.h"
#include "SnoozeAlert.h"

STDMETHODIMP CSnoozeAlertObj::Snooze(long hParentWnd, SnoozeFeature snoozeFeatures, BOOL bCanTurnOffAP, BOOL* pResult)
{
	TRACEHR(h);

	try
	{
		// h.Verify(IsItSafe(), E_ACCESSDENIED);

		*pResult = CSnoozeAlert::Snooze(hParentWnd, snoozeFeatures, bCanTurnOffAP);
	}
	catch (_com_error& e)
	{
		h = e;
	}

	return h;
}

STDMETHODIMP CSnoozeAlertObj::SetSnoozePeriod(SnoozeFeature snoozeFeatures, long lValue)
{
	TRACEHR(h);

	try
	{
		// h.Verify(IsItSafe(), E_ACCESSDENIED);

		CSnoozeAlert::SetSnoozePeriod(snoozeFeatures, lValue);
	}
	catch (_com_error& e)
	{
		h = e;
	}
	return h;
}

STDMETHODIMP CSnoozeAlertObj::GetSnoozePeriod(SnoozeFeature snoozeFeature, long* pValue)
{
	TRACEHR(h);

	try
	{
		*pValue = CSnoozeAlert::GetSnoozePeriod(snoozeFeature);
	}
	catch (_com_error& e)
	{
		h = e;
	}
	return h;
}
