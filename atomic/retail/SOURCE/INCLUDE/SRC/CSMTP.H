/////////////////////////////////////////////////////////////////////
// CSMTP.H - Created - 11/19/96
//
// Copyright 1992-1996 Symantec, Peter Norton Product Group
/////////////////////////////////////////////////////////////////////
// $Header:   S:/INCLUDE/VCS/CSMTP.H_v   1.0   24 Apr 1997 20:46:44   JBRENNA  $
//
// Description:
//    This is the declaration of the CSMTPMail class.  It is derived
//    from the CEMail base class.
//
/////////////////////////////////////////////////////////////////////
//
// $Log:   S:/INCLUDE/VCS/CSMTP.H_v  $
// 
//    Rev 1.0   24 Apr 1997 20:46:44   JBRENNA
// Initial revision.
// 
//    Rev 1.0   13 Mar 1997 15:10:46   RFULLER
// Initial revision
// 
//    Rev 1.0   20 Nov 1996 14:29:46   DSACKIN
// Initial revision.
//
/////////////////////////////////////////////////////////////////////

#if !defined(__CSMTP_H)
#define __CSMTP_H

#if !defined(__CMAIL_H)
#   include "CMail.h"
#endif

#if !defined(_OSSOCKET_H_)
#   include "OSSocket.h"
#endif

#if !defined(_SMTPMAIL_H_)
#   include "SMTPMail.h"
#endif

class CSMTPMail : public CEMail
{

public:

   CSMTPMail();
   ~CSMTPMail();

                                        // This is an override from the base
                                        // class to send the actual SMTP mails
   BOOL Send(VOID);


/////////////////////////////////////////////////////////////////////
// Utility functions

private:

                                        // Convert our internal address into
                                        // Internet Mail format
   VOID BuildRecipientList(
      LPINTERNET_EMAIL_LIST *lpTop);


                                        // Remove our internet style address
                                        // list
   VOID DeleteRecipientList(
      LPINTERNET_EMAIL_LIST *lpTop);
};



#endif // if !defined(__CSMTP_H)
