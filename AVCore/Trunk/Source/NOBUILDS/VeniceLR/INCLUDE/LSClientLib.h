// LSService.h: interface for the CLSService class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LSSERVICE_H__6C5D90C5_FBFB_11D2_A985_00A0244D507A__INCLUDED_)
#define AFX_LSSERVICE_H__6C5D90C5_FBFB_11D2_A985_00A0244D507A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <comdef.h>

// for automatic linking under Win32
#ifdef _X86_
	#ifdef _DEBUG
		#pragma comment( lib, "IDALSLIB.LIB" )
	#else
		#pragma comment( lib, "IRALSLIB.LIB" )
	#endif
#endif // _X86_

#import "LSClient.tlb" no_namespace

enum LSBS_PERIOD_TYPE {
	LSBS_DAY = 'D',
	LSBS_WEEK = 'W',
	LSBS_MONTH = 'M',
	LSBS_YEAR = 'Y'
};

class CLSService  
{
public:
	CLSService();
	virtual ~CLSService();

	BOOL Open(LPCTSTR lpszServiceID);
	void Close();

	BOOL SetProperty(LPCTSTR lpszSubscriptionID, LPCTSTR lpszName, LPCTSTR lpszValue);
	BOOL ReactivateSubscription(LPCTSTR lpszSubscriptionID);
	BOOL ExtendSubscription(LPCTSTR lpszSubscriptionID, const enum LSBS_PERIOD_TYPE periodType = LSBS_DAY, const LONG lPeriod = 0, const LONG lWarningLength = 0);

	BOOL GetProperty(LPCTSTR lpszName, _bstr_t* pValue, LPCTSTR lpszSubscriptionID = NULL);
	BOOL GetSubscriptionUrl(_bstr_t* pUrl, LPCTSTR lpszSubscriptionID = NULL);
	LONG GetSubscriptionRemaining();
	BOOL IsWarningPeriod();

	BOOL RemoveService(LPCTSTR lpszSubscriptionID);

	BOOL IsThereVeniceClient();
	BOOL IsVeniceClientActive();
	BOOL GetVeniceID(_bstr_t* pValue);
	BOOL Synchronize();

	HRESULT GetResultCode();
	operator bool();		// Added for QA purpose. Returns true, if the instance was created.

	static BOOL GetDLLPath(_bstr_t* pPath);
	static BOOL ReadSessionFile(LPCTSTR lpszFileName);

protected:
	HRESULT m_hResult;
	_bstr_t m_strServiceID;
	ISSManagerPtr m_pSSManager;
};


#endif // !defined(AFX_LSSERVICE_H__6C5D90C5_FBFB_11D2_A985_00A0244D507A__INCLUDED_)
