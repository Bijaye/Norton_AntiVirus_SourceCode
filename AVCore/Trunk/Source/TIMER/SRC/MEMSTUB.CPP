/////////////////////////////////////////////////////////////////////
// MemStub.CPP
//
// Copyright 1992-1996 Symantec, Peter Norton Product Group
/////////////////////////////////////////////////////////////////////
// $Header:   S:/TIMER/VCS/memstub.cpv   1.0   09 May 1997 17:58:52   ksackin  $
//
// This file contains ONLY the memory allocate and de-allocation
// functions for the RPC functions.
//
/////////////////////////////////////////////////////////////////////
//
// $Log:   S:/TIMER/VCS/memstub.cpv  $
// 
//    Rev 1.0   09 May 1997 17:58:52   ksackin
// Initial revision.
// 
//    Rev 1.1   18 Apr 1997 14:08:00   DSACKIN
// Ported from Tesla
// 
//    Rev 1.0   13 Mar 1997 15:19:14   RFULLER
// Initial revision
// 
//    Rev 1.0   08 Nov 1996 11:45:50   DKEEFER
// Initial revision
// 
//    Rev 1.0   13 Jun 1996 00:27:44   DSACKIN
// Initial revision.
//
/////////////////////////////////////////////////////////////////////


#include <rpc.h>

#include "MemStub.h"

void __RPC_FAR * __RPC_API midl_user_allocate (size_t len)
{
   return (new(unsigned char [len]));
}

void __RPC_API midl_user_free(void __RPC_FAR * ptr)
{
   delete(ptr);
}
