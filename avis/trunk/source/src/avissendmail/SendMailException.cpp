// SendMailException.cpp: implementation of the CSendMailException class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SendMailException.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////
//
// Function Name: 
//
// Author: C. Falterer.
//
// Purpose: 
//			.
//
// Input Parameters: 
//
// Return Value: 
//
//
/////////////////////////////////////////////////////////////////////////////
CSendMailException::CSendMailException( char *pErrorMsg )
{

}

//////////////////////////////////////////////////////////////////////////////
//
// Function Name: 
//
// Author: C. Falterer.
//
// Purpose: 
//			.
//
// Input Parameters: 
//
// Return Value: 
//
//
/////////////////////////////////////////////////////////////////////////////
CSendMailException::~CSendMailException()
{

}

BOOL CSendMailException::GetErrorMessage( LPTSTR lpstrError, 
										  UINT nMaxError,
										  PUINT pnHelpContext /*= NULL*/)
{

	char text[512];

	if(m_nError == 0)
	{
		wsprintf(text, "%s error", m_strErrorMessage);
	}
	else 
	{
		wsprintf(text, "%s error #%d",  m_strErrorMessage, m_nError);
	}

	strncpy(lpstrError, text, nMaxError - 1);

	return TRUE;

}
