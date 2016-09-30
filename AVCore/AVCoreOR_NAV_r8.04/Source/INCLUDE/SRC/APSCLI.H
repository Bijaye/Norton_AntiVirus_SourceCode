/////////////////////////////////////////////////////////////////////
// APSCli.h
//
// Copyright 1992-1996 Symantec, Peter Norton Product Group
/////////////////////////////////////////////////////////////////////
// $Header:   S:/INCLUDE/VCS/APSCLI.H_v   1.0   09 May 1997 18:41:00   ksackin  $
//
//    Here we will declare all of the functions that will be used by
// the CNAVAPSVCClient side
//
/////////////////////////////////////////////////////////////////////
//
// $Log:   S:/INCLUDE/VCS/APSCLI.H_v  $
// 
//    Rev 1.0   09 May 1997 18:41:00   ksackin
// Initial revision.
// 
//    Rev 1.0   06 Feb 1997 20:51:50   RFULLER
// Initial revision
// 
//    Rev 1.0   19 Jul 1996 00:43:54   DSACKIN
// Initial revision.
//
/////////////////////////////////////////////////////////////////////

#if !defined(__APSCLI_H)
#define __APSCLI_H

#if !defined(__LINKCLI_H)
#  include "LinkCli.h"
#endif // !defined(__LINKCLI_H)

#if !defined(__APSCOMM_H)
#  include "apscomm.h"
#endif // !defined(__APSCOMM_H)


/////////////////////////////////////////////////////////////////////
// Function declarations

class CNAVAPSVCClient : public CLinkClient
{

public:
   CNAVAPSVCClient();
   ~CNAVAPSVCClient();

   DWORD SendPacket(
      LPVOID pPacket);                  // [in] - Packet to send

   DWORD SendCommand(
      DWORD dwCommand);                 // [in] - Command to send to server

};

#endif // if !defined(__APSCLI_H)

