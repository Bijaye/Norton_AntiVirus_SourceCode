/////////////////////////////////////////////////////////////////////
// CMAIL.H - Created - 11/19/96
//
// Copyright 1992-1996 Symantec, Peter Norton Product Group
/////////////////////////////////////////////////////////////////////
// $Header:   S:/INCLUDE/VCS/CMAIL.H_v   1.0   24 Apr 1997 20:47:28   JBRENNA  $
//
// Description:
//    This header will contain the definition of our base CEMail class.
//    all other e-mail handlers should be derived from this one.
//
/////////////////////////////////////////////////////////////////////
//
// $Log:   S:/INCLUDE/VCS/CMAIL.H_v  $
// 
//    Rev 1.0   24 Apr 1997 20:47:28   JBRENNA
// Initial revision.
// 
//    Rev 1.0   13 Mar 1997 15:10:44   RFULLER
// Initial revision
// 
//    Rev 1.1   20 Nov 1996 22:10:36   DSACKIN
// Added a recipient count and a Reset function
//
//    Rev 1.0   20 Nov 1996 14:29:24   DSACKIN
// Initial revision.
//
/////////////////////////////////////////////////////////////////////

#if !defined(__CMAIL_H)
#define __CMAIL_H


class CEMail
{

protected:
                                        // DataBase Variables
   CString         m_szPostOffice;
   CString         m_szUserName;
   CString         m_szPassword;

                                        // Message function variables
   CString         m_szSender;
   CStringList     m_szlistRecipients;  // List of recipients
   CString         m_szSubject;
   CString         m_szMessage;

   DWORD           m_dwRecipients;      // count of recipients

public:

                                        // Constructor / Destructor
   CEMail();
   ~CEMail();

                                        // Clears out all of the internal
                                        // variables
   VOID Reset(VOID);

/////////////////////////////////////////////////////////////////////
// Generic E-Mail functionality

                                        // DataBase Functions
   VOID SetPostOffice(
      LPCTSTR lpszPostOffice);

   VOID SetUserName(
      LPCTSTR lpszUserName);

   VOID SetPassword(
      LPCTSTR lpszPassword);

                                        // Message functions
   VOID SetSender(
      LPCTSTR lpszSender);

   BOOL AddRecipient(
      LPCTSTR lpszRecipient);

   VOID SetSubject(
      LPCTSTR lpszSubject);

   VOID SetMessage(
      LPCTSTR lpszMessage);

/////////////////////////////////////////////////////////////////////
// Virtual Send function that will be overridden for the different
// types of e-mail

                                        // Wrap up and send the e-mail
   virtual BOOL Send(VOID);

};

#endif // if !defined(__CMAIL_H)
