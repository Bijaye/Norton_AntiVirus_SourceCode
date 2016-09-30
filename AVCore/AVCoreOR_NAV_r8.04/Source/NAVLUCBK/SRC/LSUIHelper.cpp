#include "stdafx.h"

#include "LSUIHelper.h"


/////////////////////////////////////////////////////////////////////////////
// CLSUIHelper -
//		default constructor - acts as if LU was run manually
CLSUIHelper::CLSUIHelper()
{
	m_bWarning = FALSE;
	m_bExpired = FALSE;
	m_lRemaining = 1;
	m_bLUIsSilent = FALSE;
} //CLSUIHelper


/////////////////////////////////////////////////////////////////////////////
// CLSUIHelper -
//		bWarning - TRUE if subscription is in warning period
//		bExpired - TRUE if subscription has expired
//		lRemaining - days remaining on subscription
//		bSilent - TRUE if LiveUpdate is being run in silent mode
CLSUIHelper::CLSUIHelper(BOOL bWarning, BOOL bExpired, long lRemaining, BOOL bSilent)
{
	m_bWarning = bWarning;
	m_bExpired = bExpired;
	m_lRemaining = lRemaining;
	m_bLUIsSilent = bSilent;
} //CLSUIHelper(...)


/////////////////////////////////////////////////////////////////////////////
// ~CLSUIHelper -
//		nothing to destruct
CLSUIHelper::~CLSUIHelper()
{
} //~CLSUIHelper


/////////////////////////////////////////////////////////////////////////////
// InitializeRegKeys -
//		This function will create the registry keys and values needed to 
//		keep track of the LiveSubscribe visibility
void CLSUIHelper::InitializeRegKeys()
{
	LSHasBeenRunToday(TRUE);
	CheckWarningDay(0, TRUE);
} //InitializeRegKeys


/////////////////////////////////////////////////////////////////////////////
// CleanUpRegKeys -
//		This function will delete values created by LSHasBeenRunToday
//		and CheckWarningDay
void CLSUIHelper::CleanUpRegKeys()
{
	CRegKey Key;
	DWORD dwValue = 0;

	LONG rc = Key.Open(HKEY_LOCAL_MACHINE,REG_NAV,KEY_ALL_ACCESS);

	if(rc == ERROR_SUCCESS)
    {
		Key.DeleteValue(REG_CURRENT_DAY);
		Key.DeleteValue(REG_CURRENT_PERIOD);
		Key.Close();
	}
} //CleanUpLSNagRegValues

/////////////////////////////////////////////////////////////////////////////
// WasLSVisibleToday -
//		bYesterday	- TRUE if you want to know about past week
//		This function will check to see if LS has been run non-Silently today
//		returns TRUE if LS has been run non-Silently today
//				FALSE if LS has not been run today
BOOL CLSUIHelper::WasLSVisibleToday(BOOL bWeek)
{
	BOOL bRet = FALSE;

	CRegKey Key;
	DWORD dwValue = 0;

	LONG rc = Key.Open(HKEY_LOCAL_MACHINE, REG_NAV, KEY_ALL_ACCESS);

	DWORD dwToday = GetDateAsDW();

    if(rc == ERROR_SUCCESS)
    {
		if (ERROR_SUCCESS == Key.QueryValue(dwValue,REG_CURRENT_DAY))
			if(bWeek)
			{
				if(((dwToday - MINIMUM_DAYS_BETWEEN_WARNINGS) < dwValue) && (dwValue <= dwToday))
					bRet = TRUE;
			}
			else
			{
				if(dwToday == dwValue)
					bRet = TRUE;
			}

        Key.Close();
    }

	return bRet;
} //WasLSVisibleToday

/////////////////////////////////////////////////////////////////////////////
// LSHasBeenRunToday -
//		bInit		- TRUE for initial run (sets up registry)
//		This function will set registry value so next run today will not
//		display the UI
void CLSUIHelper::LSHasBeenRunToday(BOOL bInit)
{
	CRegKey Key;
	DWORD dwValue = GetDateAsDW();

	LONG rc = Key.Create(HKEY_LOCAL_MACHINE, REG_NAV);

    if(rc == ERROR_SUCCESS)
    {
		if(bInit)
			dwValue = 0;

		Key.SetValue(dwValue,REG_CURRENT_DAY);

        Key.Close();
    }
	
} //LSHasBeenRunToday

/////////////////////////////////////////////////////////////////////////////
// GetDateAsLong -
//		returns the date in the format YYYYMMDD
DWORD CLSUIHelper::GetDateAsDW()
{
	SYSTEMTIME currentTime = {0};
	GetSystemTime(&currentTime);

	DWORD dwDate = currentTime.wYear * 10000;
	dwDate += currentTime.wMonth * 100;
	dwDate += currentTime.wDay;

	return dwDate;
} //GetDateAsLong

/////////////////////////////////////////////////////////////////////////////
// CheckWarningDay -
//		nCurrent	- current warning interval (day 60, day 30, etc.)
//		bInit		- TRUE for initial run (sets up registry)
//		returns TRUE if function has already been called with this value
//				FALSE if new value
BOOL CLSUIHelper::CheckWarningDay(DWORD nCurrent, BOOL bInit)
{
	BOOL bRet = TRUE; 
	CRegKey Key;
	DWORD dwValue = 0;

	LONG rc = Key.Create(HKEY_LOCAL_MACHINE, REG_NAV);

    if(rc == ERROR_SUCCESS)
    {
		if(bInit)
			Key.SetValue(nCurrent, REG_CURRENT_PERIOD);
		else
		{
			Key.QueryValue(dwValue,REG_CURRENT_PERIOD);
			if((dwValue != nCurrent) && (!WasLSVisibleToday(TRUE)))
			{
				bRet = FALSE;
				Key.SetValue(nCurrent, REG_CURRENT_PERIOD);
			}
		}

        Key.Close();
    }

	return bRet;
} //CheckWarningDay

/////////////////////////////////////////////////////////////////////////////
// GetLSUIMode -
//		returns TRUE if LU should be run Loud
//				FALSE if LU should be run Silent
BOOL CLSUIHelper::GetLSUIMode()
{
	BOOL bDisplayLSUI = TRUE;

	// running ALU
	if (m_bLUIsSilent)
	{
		BOOL bDisplayedToday = WasLSVisibleToday();

		if (!bDisplayedToday)
		{
			// in warining period and not expired
			if (m_bWarning && !m_bExpired)
			{
				// if 60, 30, 15, 7 days remaining make LS visible
				// should display once for each, CheckWarningDay() allows us to show
				// even if they don't run on the exact day, will not display on 
				// consecutive days
				if(m_lRemaining > 60)
				{
					if(CheckWarningDay(90))
						bDisplayLSUI = FALSE;
				}
				else if((60 >= m_lRemaining) && (m_lRemaining > 30))
				{
					if(CheckWarningDay(60))
						bDisplayLSUI = FALSE;
				}
				else if((30 >= m_lRemaining) && (m_lRemaining > 15))
				{
					if(CheckWarningDay(30))
						bDisplayLSUI = FALSE;
				}
				else if((15 >= m_lRemaining) && (m_lRemaining > 7))
				{
					if(CheckWarningDay(15))
						bDisplayLSUI = FALSE;
				}
				else if(  7 >= m_lRemaining)
				{
					if(CheckWarningDay(7))
						bDisplayLSUI = FALSE;
				}
			}
		}
		else // already been displayed today
			bDisplayLSUI = FALSE;

		// if running loud, notify system
		if (bDisplayLSUI)
			LSHasBeenRunToday();
	}

	return bDisplayLSUI;
}