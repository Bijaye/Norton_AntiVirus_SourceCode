/////////////////////////////////////////////////////////////////////
// MemStub.CPP
//
// Copyright 1992-1996 Symantec, Peter Norton Product Group
/////////////////////////////////////////////////////////////////////
// $Header:   S:/CLINK/VCS/memstub.cpv   1.0   09 May 1997 18:35:16   ksackin  $
//
// This file contains ONLY the memory allocate and de-allocation
// functions for the RPC functions.
//
/////////////////////////////////////////////////////////////////////
//
// $Log:   S:/CLINK/VCS/memstub.cpv  $
// 
//    Rev 1.0   09 May 1997 18:35:16   ksackin
// Initial revision.
// 
//    Rev 1.0   18 Apr 1997 13:46:22   DSACKIN
// Ported from Tesla
// 
//    Rev 1.0   13 Mar 1997 15:08:58   RFULLER
// Initial revision
// 
//    Rev 1.0   08 Nov 1996 14:54:40   DSACKIN
// Initial revision.
// 
//    Rev 1.0   07 Nov 1996 14:56:36   DSACKIN
// Initial revision.
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
