// AVISTransactionException.cpp: implementation of the CAVISTransactionException class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "AVISTransactionException.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CAVISTransactionException::CAVISTransactionException(int cause, PSTR pszReason)
 :	m_cause(cause),
	m_strReason(pszReason)
{
}
/*****/
CAVISTransactionException::CAVISTransactionException(int cause, CString strReason)
 :	m_cause(cause),
	m_strReason(strReason)
{
}
/*****/
CAVISTransactionException::~CAVISTransactionException()
{
}
/*****/
int CAVISTransactionException::GetCause()
{
	return m_cause;
}
/*****/
CString CAVISTransactionException::GetReasonText()
{
	return m_strReason;
}
