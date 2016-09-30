// AVISTransactionImportSampleException.cpp: implementation of the CAVISTransactionImportSampleException class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "AVISACSampleComm.h"
#include "AVISTransactionImportSampleException.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CAVISTransactionImportSampleException::CAVISTransactionImportSampleException(int cause, PSTR pszReason)
 :	CAVISTransactionException(cause, pszReason)
{
}
/*****/
CAVISTransactionImportSampleException::CAVISTransactionImportSampleException(int cause, CString strReason)
 :	CAVISTransactionException(cause, strReason)
{
}
/*****/
CAVISTransactionImportSampleException::~CAVISTransactionImportSampleException()
{
}
