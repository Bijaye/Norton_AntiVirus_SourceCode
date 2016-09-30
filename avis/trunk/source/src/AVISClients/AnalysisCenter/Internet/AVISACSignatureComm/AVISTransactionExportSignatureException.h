// AVISTransactionExportSignatureException.h: interface for the CAVISTransactionExportSignatureException class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AVISTRANSACTIONEXPORTSIGNATUREEXCEPTION_H__F75D2113_F11B_11D2_8A9A_00203529AC86__INCLUDED_)
#define AFX_AVISTRANSACTIONEXPORTSIGNATUREEXCEPTION_H__F75D2113_F11B_11D2_8A9A_00203529AC86__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "AVISTransactionException.h"

class CAVISTransactionExportSignatureException : public  CAVISTransactionException  
{
public:
	enum
	{
		noError,
		noSignatureStoragePathBase,
		signatureNotFoundInDB,
		signatureFileNotFound,
		errorReadingSignatureFile
	};

public:
	CAVISTransactionExportSignatureException(int cause, PSTR pszReason);
	CAVISTransactionExportSignatureException(int cause, CString strReason);
	virtual ~CAVISTransactionExportSignatureException();

};

#endif // !defined(AFX_AVISTRANSACTIONEXPORTSIGNATUREEXCEPTION_H__F75D2113_F11B_11D2_8A9A_00203529AC86__INCLUDED_)
