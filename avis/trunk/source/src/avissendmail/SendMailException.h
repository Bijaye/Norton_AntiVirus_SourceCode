// SendMailException.h: interface for the CSendMailException class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SENDMAILEXCEPTION_H__A80D90B8_E83C_11D2_9134_0004ACEC70EC__INCLUDED_)
#define AFX_SENDMAILEXCEPTION_H__A80D90B8_E83C_11D2_9134_0004ACEC70EC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#pragma warning(disable:4251)

#include "AVISSendMailDLL.h"

#include "SendMailException.h"

//////////////////////////////////////////////////////////////////////////////
//
// Class Name: CSendMailException  
//
// Author: C. Falterer.
//
// Purpose: 
//			.
//
//
//
//
/////////////////////////////////////////////////////////////////////////////
class AVISSENDMAIL_API CSendMailException  
{

public:

	CSendMailException( char *pErrorMsg );

	virtual ~CSendMailException();

	virtual BOOL GetErrorMessage( LPTSTR lpstrError, 
								  UINT nMaxError, 
								  PUINT pnHelpContext = NULL);

private:

	std::string m_strErrorMessage;

	int m_nError;
};

#endif // !defined(AFX_SENDMAILEXCEPTION_H__A80D90B8_E83C_11D2_9134_0004ACEC70EC__INCLUDED_)
