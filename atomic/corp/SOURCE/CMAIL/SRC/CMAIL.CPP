/////////////////////////////////////////////////////////////////////
// CMAIL.CPP - Created - 11/19/96
//
// Copyright 1992-1996 Symantec, Peter Norton Product Group
/////////////////////////////////////////////////////////////////////
// $Header:   S:/CMAIL/VCS/cmail.cpv   1.0   18 Apr 1997 13:47:18   DSACKIN  $
//
// Description:
//    This will contain the implementation for the CEMail class.
//
/////////////////////////////////////////////////////////////////////
//
// $Log:   S:/CMAIL/VCS/cmail.cpv  $
// 
//    Rev 1.0   18 Apr 1997 13:47:18   DSACKIN
// Ported from Tesla
// 
//    Rev 1.0   13 Mar 1997 15:09:00   RFULLER
// Initial revision
// 
//    Rev 1.1   20 Nov 1996 22:10:08   DSACKIN
// Added a recipient count and also a Reset function
//
//    Rev 1.0   20 Nov 1996 14:27:32   DSACKIN
// Initial revision.
//
/////////////////////////////////////////////////////////////////////

#include "afx.h"

#include "CMail.h"


/////////////////////////////////////////////////////////////////////
// CEMail::CEMail
//
// Purpose:
//    Initialize our base e-mail class
//
// Parameters:
//    none.
//
// Returns:
//    none.
//
// See Also:
//
//
/////////////////////////////////////////////////////////////////////
// 11/19/96 - DSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
CEMail::CEMail()
{
                                        // Clear out the DataBase variables
   m_szPostOffice.Empty();
   m_szUserName.Empty();
   m_szPassword.Empty();

                                        // Clear out the Messaging strings
   m_szSender.Empty();
   m_szSubject.Empty();
   m_szMessage.Empty();

   m_dwRecipients = 0;
}


/////////////////////////////////////////////////////////////////////
// CEMail::~CEMail
//
// Purpose:
//    Clean up after our CEMail object
//
// Parameters:
//    none.
//
// Returns:
//    none.
//
// See Also:
//
//
/////////////////////////////////////////////////////////////////////
// 11/19/96 - DSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
CEMail::~CEMail()
{
   m_szlistRecipients.RemoveAll();

                                        // To be neat and tidy
   m_dwRecipients = 0;
}


/////////////////////////////////////////////////////////////////////
// VOID CEMail::Reset(VOID)
//
// Purpose:
//    Resets the contents of the current e-mail
//
// Parameters:
//    none.
//
// Returns:
//    none.
//
// See Also:
//
//
/////////////////////////////////////////////////////////////////////
// 11/20/96 - DSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
VOID CEMail::Reset(VOID)
{
                                        // Clear out the DataBase variables
   m_szPostOffice.Empty();
   m_szUserName.Empty();
   m_szPassword.Empty();

                                        // Clear out the Messaging strings
   m_szSender.Empty();
   m_szSubject.Empty();
   m_szMessage.Empty();

   m_szlistRecipients.RemoveAll();

   m_dwRecipients = 0;
}

/////////////////////////////////////////////////////////////////////
// DataBase Functions


/////////////////////////////////////////////////////////////////////
// CEMail::SetPostOffice
//
// Purpose:
//    This will assign a postoffice member for us
//
// Parameters:
//    LPTSTR lpcszPostOffice
//
// Returns:
//    none.
//
// See Also:
//
//
/////////////////////////////////////////////////////////////////////
// 11/19/96 - DSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
VOID CEMail::SetPostOffice(
   LPCTSTR lpcszPostOffice)
{
   m_szPostOffice = lpcszPostOffice;
}


/////////////////////////////////////////////////////////////////////
// CEMail::SetUserName
//
// Purpose:
//    Set up the UserName for the e-mail object
//
// Parameters:
//    LPCTSTR lpszUserName - User name for accessing the DataBase
//
// Returns:
//    none.
//
// See Also:
//
//
/////////////////////////////////////////////////////////////////////
// 11/19/96 - DSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
VOID CEMail::SetUserName(
   LPCTSTR lpszUserName)
{
   m_szUserName = lpszUserName;
}


/////////////////////////////////////////////////////////////////////
// CEMail::SetPassword
//
// Purpose:
//    Set up the Password for the e-mail object
//
// Parameters:
//    LPCTSTR lpszPassword - Password for accessing the DataBase
//
// Returns:
//    none.
//
// See Also:
//
//
/////////////////////////////////////////////////////////////////////
// 11/19/96 - DSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
VOID CEMail::SetPassword(
      LPCTSTR lpszPassword)
{
   m_szPassword = lpszPassword;
}


/////////////////////////////////////////////////////////////////////
// Here is where all of the actual messaging functions go

/////////////////////////////////////////////////////////////////////
// VOID CEMail::SetSender
//
// Purpose:
//    Set up the Sender for this e-mail object
//
// Parameters:
//    LPCTSTR lpszSender - Sender that the mail is coming from.
//
// Returns:
//    none.
//
// See Also:
//
//
/////////////////////////////////////////////////////////////////////
// 11/19/96 - DSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
VOID CEMail::SetSender(
   LPCTSTR lpszSender)
{
   m_szSender = lpszSender;
}



/////////////////////////////////////////////////////////////////////
// BOOL CEMail::AddRecipient(
//
// Purpose:
//    Adds a recipient to our list of people that this e-mail is
//    going to be sent to.
//
// Parameters:
//    LPCTSTR lpszRecipient - Recipient to add to our list
//
// Returns:
//    TRUE  - Added successfully to our list.
//    FALSE - Something went wrong when adding to the list.
//
// See Also:
//
//
/////////////////////////////////////////////////////////////////////
// 11/19/96 - DSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
BOOL CEMail::AddRecipient(
   LPCTSTR lpszRecipient)
{
BOOL     bReturn = TRUE;

   try
   {
      m_szlistRecipients.AddTail (lpszRecipient);

                                        // Increment the number of recipients
                                        // if we are successful
      m_dwRecipients++;
   }

   catch (CMemoryException)
   {
      bReturn = FALSE;
   }

   return bReturn;
}



/////////////////////////////////////////////////////////////////////
// VOID CEMail::SetSubject(
//
// Purpose:
//    This will set the subject of the current e-mail object
//
// Parameters:
//    LPCTSTR lpszSubject - Subject to be set.
//
// Returns:
//    none.
//
// See Also:
//
//
/////////////////////////////////////////////////////////////////////
// 11/19/96 - DSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
VOID CEMail::SetSubject(
   LPCTSTR lpszSubject)
{
   m_szSubject = lpszSubject;
}



/////////////////////////////////////////////////////////////////////
// VOID CEMail::SetMessage(
//
// Purpose:
//    This will set the message content of the current e-mail
//
// Parameters:
//    LPCTSTR lpszMessage - Message contents
//
// Returns:
//    none.
//
// See Also:
//
//
/////////////////////////////////////////////////////////////////////
// 11/19/96 - DSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
VOID CEMail::SetMessage(
   LPCTSTR lpszMessage)
{
   m_szMessage = lpszMessage;
}


/////////////////////////////////////////////////////////////////////
// CEMail::Send
//
// Purpose:
//    Sends the e-mail we have been working on.
//    This is a virtual function, and should always be overriden
//
// Parameters:
//    none.  Takes the internal variables and uses them.
//
// Returns:
//    TRUE  - Mail sent successfully
//    FALSE - Mail was not sent successfully
//
// See Also:
//
//
/////////////////////////////////////////////////////////////////////
// 11/20/96 - DSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
BOOL CEMail::Send(VOID)
{
   if (m_dwRecipients > 0)
      return TRUE;
   else
      return FALSE;
}

