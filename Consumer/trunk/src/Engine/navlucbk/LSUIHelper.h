////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_LSUIHelper_H__3C2E74AD_987E_11D3_B40A_00600831DD76__INCLUDED_)
#define AFX_LSUIHelper_H__3C2E74AD_987E_11D3_B40A_00600831DD76__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// Reg Key Values for current day and current period checks
#define REG_CURRENT_DAY		_T("NAVLUCBK")
#define REG_CURRENT_PERIOD	_T("NAVLUCBK_WPCHECK")
#define REG_NAV				_T("Software\\Symantec\\Norton AntiVirus")

// used to determine how many days must be between visible days in the warning, so if the user logs in on 2 consecutive
// boundry days (ie. 61 days left and then on 60 days left) they won't see the nag until this many days have passed
// (ie. 61 days left and 56 days left would give 2 warnings)
#define MINIMUM_DAYS_BETWEEN_WARNINGS 5

class CLSUIHelper  
{
public:
	// constructors and destructor
	CLSUIHelper();
	CLSUIHelper(BOOL bWarning, BOOL bExpired, long lRemaining, BOOL bSilent = FALSE);
	~CLSUIHelper();

	static void InitializeRegKeys(); // creates registry values that will be used to track LS warnings
	static void CleanUpRegKeys(); // removes registry values that were used to track LS warnings

	BOOL GetLSUIMode();	// determines if LS should be run loud (TRUE) or silent (FALSE)

	void SetLUUIMode(BOOL bSilent){m_bLUIsSilent = bSilent;}
	void SetWarning(BOOL bWarning){m_bWarning = bWarning;}
	void SetExpired(BOOL bExpired){m_bExpired = bExpired;}
	void SetRemaining(long lRemaining){m_lRemaining = lRemaining;}

private:
	static void LSHasBeenRunToday(BOOL bInit = FALSE);	// call after LS has run in non-silent mode
	static BOOL CheckWarningDay(DWORD nCurrent, BOOL bInit = FALSE); // determines if display has been visible during current period
	static BOOL WasLSVisibleToday(BOOL bWeek = FALSE);	// determines if LU was run in non-silent mode today
	static DWORD GetDateAsDW();	// gives the date in the following format: YYYYMMDD

	BOOL m_bLUIsSilent;	// is liveupdate in silent mode?
	BOOL m_bWarning;	// is Subscription in the warning period
	BOOL m_bExpired;	// is Subscription expired
	long m_lRemaining;	// number of days remaining on Subscription
};
#endif //AFX_LSUIHelper_H__3C2E74AD_987E_11D3_B40A_00600831DD76__INCLUDED_