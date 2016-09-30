/////////////////////////////////////////////////////////////////////
// AT_SMTP.CPP - Created - 12/05/96
//
// Copyright 1992-1996 Symantec, Peter Norton Product Group
/////////////////////////////////////////////////////////////////////
// $Header:   S:/NAVALERT/VCS/at_smtp.cpv   1.1   22 May 1997 14:30:12   DSACKIN  $
//
// Description:
//    This file will contain the code for setting and getting of
//    the SMTP options.
//
/////////////////////////////////////////////////////////////////////
//
// $Log:   S:/NAVALERT/VCS/at_smtp.cpv  $
// 
//    Rev 1.1   22 May 1997 14:30:12   DSACKIN
// Converted back to TCHAR
//
//    Rev 1.0   18 Apr 1997 13:57:24   DSACKIN
// Ported from Tesla
//
//    Rev 1.0   13 Mar 1997 15:11:54   RFULLER
// Initial revision
//
//    Rev 1.1   07 Jan 1997 13:47:02   ksackin
// Added access functions to get and to set the SMTP Author and Subject member
// functions.
//
//    Rev 1.0   05 Dec 1996 01:49:54   DSACKIN
// Initial revision.
//
/////////////////////////////////////////////////////////////////////


#include <afx.h>
#include "AT_OPTS.H"
#include "AT_TGT.H"



/////////////////////////////////////////////////////////////////////
// CAlertOptions::GetSMTPPostOffice
//
// Purpose:
//    Returns the SMTP post office.
//
// Parameters:
//    none.
//
// Returns:
//    The current SMTP post office
//
// See Also:
//
//
/////////////////////////////////////////////////////////////////////
// 12/05/96 - DSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
LPCTSTR CAlertOptions::GetSMTPPostOffice( void )
{
   return m_cszSMTPPostOffice;
};


/////////////////////////////////////////////////////////////////////
// CAlertOptions::GetSMTPAuthor
//
// Purpose:
//    Returns the SMTP Author.
//
// Parameters:
//    none.
//
// Returns:
//    The current SMTP author.
//
// See Also:
//
//
/////////////////////////////////////////////////////////////////////
// 01/07/97 - KSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
LPCTSTR CAlertOptions::GetSMTPAuthor( void )
{
   return m_cszSMTPAuthor;
};

/////////////////////////////////////////////////////////////////////
// CAlertOptions::GetSMTPSubject
//
// Purpose:
//    Returns the SMTP subject.
//
// Parameters:
//    none.
//
// Returns:
//    The current SMTP subject
//
// See Also:
//
//
/////////////////////////////////////////////////////////////////////
// 01/07/97 - KSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
LPCTSTR CAlertOptions::GetSMTPSubject( void )
{
   return m_cszSMTPSubject;
};


/////////////////////////////////////////////////////////////////////
// void CAlertOptions::SetSMTPPostOffice( LPCWSTR lpcszNewPostOffice ) {m_cszSMTPPostOffice = lpcszNewPostOffice;};
//
// Purpose:
//    This sets the SMTP post office.
//
// Parameters:
//    LPCWSTR lpcszNewPostOffice - [in] - New post office.
//
// Returns:
//    none.
//
// See Also:
//
//
/////////////////////////////////////////////////////////////////////
// 12/05/96 - DSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
void CAlertOptions::SetSMTPPostOffice( LPCTSTR lpcszNewPostOffice )
{
   m_cszSMTPPostOffice = lpcszNewPostOffice;
};

/////////////////////////////////////////////////////////////////////
// void CAlertOptions::SetSMTPAuthor( LPCWSTR lpcszNewAuthor ) {m_cszSMTPAuthor = lpcszNewAuthor;};
//
// Purpose:
//    This sets the SMTP author string.
//
// Parameters:
//    LPCWSTR lpcszNewAuthor - [in] - New author.
//
// Returns:
//    none.
//
// See Also:
//
//
/////////////////////////////////////////////////////////////////////
// 01/07/97 - KSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
void CAlertOptions::SetSMTPAuthor( LPCTSTR lpcszNewAuthor )
{
   m_cszSMTPAuthor = lpcszNewAuthor;
};

/////////////////////////////////////////////////////////////////////
// void CAlertOptions::SetSMTPSubject( LPCWSTR lpcszNewSubject ) {m_cszSMTPSubject = lpcszNewSubject;};
//
// Purpose:
//    This sets the SMTP subject string.
//
// Parameters:
//    LPCWSTR lpcszNewSubject - [in] - New subject string.
//
// Returns:
//    none.
//
// See Also:
//
//
/////////////////////////////////////////////////////////////////////
// 01/07/97 - KSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
void CAlertOptions::SetSMTPSubject( LPCTSTR lpcszNewSubject )
{
   m_cszSMTPSubject = lpcszNewSubject;
};


