// Copyright 1997 Symantec
// -------------------------------------------------------------------------
// NAVDOABT.H
// Contains prototype for C helper entry point to invoke the standard
// about box.
//
// NOTE:
//    You should make this function return an error code and you should pass in
//    the language and SKU/PartNumber or put them in the string table and modify
//    this routine to get them.  This is just a crude example that shows how
//    to invoke the standard about COM interface.
//
// $Header:   S:/INCLUDE/VCS/navdoabt.h_v   1.3   21 Oct 1998 17:07:56   CEATON  $
// -------------------------------------------------------------------------
// $Log:   S:/INCLUDE/VCS/navdoabt.h_v  $
// 
//    Rev 1.3   21 Oct 1998 17:07:56   CEATON
// Moved DoSystemWorksAbout() to ABOUTSW.H.
// 
// 
//    Rev 1.2   20 Oct 1998 20:27:58   CEATON
// Added function prototype for DoSystemWorksAbout().
// 
//    Rev 1.1   09 Jun 1997 16:02:54   TIVANOV
// fix the function mangled name
//
//    Rev 1.0   06 Jun 1997 20:01:48   BMCCORK
// Initial revision.
/////////////////////////////////////////////////////////////////////////////

#ifndef _NAVDOABT_H
#define _NAVDOABT_H
/////////////////////////////////////////////////////////////////////////////
// Local Prototypes
extern "C" {
void WINAPI DoStdAbout( HWND hwndParent );
}
#endif

