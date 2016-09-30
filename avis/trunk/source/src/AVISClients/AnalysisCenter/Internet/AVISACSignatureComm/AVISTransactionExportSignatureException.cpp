// AVISTransactionExportSignatureException.cpp: implementation of the CAVISTransactionExportSignatureException class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "AVISACSignatureComm.h"
#include "AVISTransactionExportSignatureException.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CAVISTransactionExportSignatureException::CAVISTransactionExportSignatureException(int cause, PSTR pszReason)
 :	CAVISTransactionException(cause, pszReason)
{
}
/*****/
CAVISTransactionExportSignatureException::CAVISTransactionExportSignatureException(int cause, CString strReason)
 :	CAVISTransactionException(cause, strReason)
{
}
/*****/
CAVISTransactionExportSignatureException::~CAVISTransactionExportSignatureException()
{

}
