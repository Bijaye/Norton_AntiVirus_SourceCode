// AVISTransactionException.h: interface for the CAVISTransactionException class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AVISTRANSACTIONEXCEPTION_H__DC22CB07_E92A_11D2_8A99_00203529AC86__INCLUDED_)
#define AFX_AVISTRANSACTIONEXCEPTION_H__DC22CB07_E92A_11D2_8A99_00203529AC86__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "impexp.h"

#pragma	warning( disable : 4251 )

class IMPEXP CAVISTransactionException
{
private:
	int m_cause;
	CString m_strReason;

public:
	enum
	{
		noError,
		gatewayError,
		openRequestError,
		invalidRequestHandle
	};

public:
	CAVISTransactionException();
	CAVISTransactionException(int cause, PSTR pszReason);
	CAVISTransactionException(int cause, CString strReason);
	virtual ~CAVISTransactionException();

public:
	int GetCause();
	CString GetReasonText();
};

#endif // !defined(AFX_AVISTRANSACTIONEXCEPTION_H__DC22CB07_E92A_11D2_8A99_00203529AC86__INCLUDED_)
