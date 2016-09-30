/////////////////////////////////////////////////////////////////////
// MemStub.h
//
// Copyright 1992-1996 Symantec, Peter Norton Product Group
/////////////////////////////////////////////////////////////////////
// $Header:   S:/TIMER/VCS/memstub.h_v   1.0   09 May 1997 17:58:12   ksackin  $
//
//    Here is where we prototype the memory allocation and free
// functions that will be used by the RPC subsystem.
//
/////////////////////////////////////////////////////////////////////
//
// $Log:   S:/TIMER/VCS/memstub.h_v  $
// 
//    Rev 1.0   09 May 1997 17:58:12   ksackin
// Initial revision.
// 
//    Rev 1.1   18 Apr 1997 14:08:02   DSACKIN
// Ported from Tesla
// 
//    Rev 1.0   13 Mar 1997 15:19:14   RFULLER
// Initial revision
// 
//    Rev 1.0   08 Nov 1996 11:45:52   DKEEFER
// Initial revision
// 
//    Rev 1.0   13 Jun 1996 00:27:58   DSACKIN
// Initial revision.
//
/////////////////////////////////////////////////////////////////////

#if !defined(__MEMSTUB_H)
#define __MEMSTUB_H

#if !defined(__RPC_H__)
#   include <rpc.h>
#endif

void __RPC_FAR * __RPC_API midl_user_allocate (size_t len);
void __RPC_API midl_user_free(void __RPC_FAR * ptr);

#endif // #if !defined(__MEMSTUB_H)

