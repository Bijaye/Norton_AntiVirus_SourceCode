/////////////////////////////////////////////////////////////////////
// SERIAL.H - Created - 08/01/97
//
// Copyright 1992-1997 Symantec, Peter Norton Product Group
/////////////////////////////////////////////////////////////////////
// $Header:   S:/FILELINK/VCS/SERIAL.H_v   1.0   17 Mar 1998 19:45:18   SEDWARD  $
//
// Description:
//    This file contains the function serialization class definition.
//
/////////////////////////////////////////////////////////////////////
//
// $Log:   S:/FILELINK/VCS/SERIAL.H_v  $
// 
//    Rev 1.0   17 Mar 1998 19:45:18   SEDWARD
// Initial revision.
// 
//    Rev 1.0   27 Jan 1998 12:43:58   KSACKIN
// Initial revision.
// 
//    Rev 1.0   02 Dec 1997 10:44:20   DSACKIN
// Initial revision.
//
//    Rev 1.0   02 Aug 1997 02:39:26   DSACKIN
// Initial revision.
//
/////////////////////////////////////////////////////////////////////

#if !defined(__SERIAL_H)
#define __SERIAL_H

#if !defined(__LINKINFO_H)
#   include "linkinfo.h"
#endif // #if !defined(__LINKINFO_H)

/////////////////////////////////////////////////////////////////////
// This very simple class is very important to serializing the
// RPC functions.
//
// To Use This Class:
//
//    1.) Declare a CLinkClientSerializer variable on the local
//        stack of the function.
//
//    2.) Use the Connect member function to make any RPC
//        connections.
//
//    Note: The CLinkClientSerializer will make sure and
//          close the connection for you when you are done.
//
/////////////////////////////////////////////////////////////////////

class CLinkSerializer
{
public:

    CLinkSerializer();
    ~CLinkSerializer();

    BOOL Connect(
        PRPC_LINK_INFO      pLinkInfo,
        RPC_BINDING_HANDLE* pBindingHandle);

};




#endif // if !defined(__SERIAL_H)
