// AVISTransactionImportSampleException.h: interface for the CAVISTransactionImportSampleException class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AVISTRANSACTIONIMPORTSAMPLEEXCEPTION_H__DC22CB08_E92A_11D2_8A99_00203529AC86__INCLUDED_)
#define AFX_AVISTRANSACTIONIMPORTSAMPLEEXCEPTION_H__DC22CB08_E92A_11D2_8A99_00203529AC86__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "AVISTransactionException.h"

class CAVISTransactionImportSampleException : public CAVISTransactionException  
{
public:
	enum
	{
		noError,
		noSampleStoragePathBase,
		filterCritical,
		sampleStorageError,
		unableToCreateSampleDirectory,
		errorStoringSample,
		errorCreatingImportCompleteFlagFile,
		errorSettingFinishedDate,
		unhandledFilterState
	};

public:
	CAVISTransactionImportSampleException(int cause, PSTR pszReason);
	CAVISTransactionImportSampleException(int cause, CString strReason);
	virtual ~CAVISTransactionImportSampleException();
};

#endif // !defined(AFX_AVISTRANSACTIONIMPORTSAMPLEEXCEPTION_H__DC22CB08_E92A_11D2_8A99_00203529AC86__INCLUDED_)
