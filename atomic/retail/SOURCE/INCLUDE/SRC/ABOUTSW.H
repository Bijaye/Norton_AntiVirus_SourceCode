// Copyright 1997 Symantec
// -------------------------------------------------------------------------
// ABOUTSW.H
// Contains prototype for C helper entry point to invoke the standard
// about box.
//
// NOTE:
//    You should make this function return an error code and you should pass in
//    the language and SKU/PartNumber or put them in the string table and modify
//    this routine to get them.  This is just a crude example that shows how
//    to invoke the standard about COM interface.
//
// $Header:
// -------------------------------------------------------------------------
// $Log:
// 
/////////////////////////////////////////////////////////////////////////////

#ifndef _ABOUTSW_H
#define _ABOUTSW_H

/////////////////////////////////////////////////////////////////////////////
// Local Prototypes
extern "C" {
LPTSTR WINAPI GetSystemWorksKey( PHKEY phKey,LPTSTR szKey );
BOOL WINAPI DoSystemWorksAbout( HWND hwndParent );
}
#endif
