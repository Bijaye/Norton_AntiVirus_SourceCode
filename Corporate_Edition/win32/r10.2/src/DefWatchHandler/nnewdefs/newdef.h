// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
/////////////////////////////////////////////////////////////////////////////
// $Header:   S:/QUAR32/VCS/DLLMAIN.CPv   1.3   25 Mar 1998 21:48:38   JTaylor  $
/////////////////////////////////////////////////////////////////////////////
//
// Dllmain.cpp - contains exported functions for ScanObj project
//
/////////////////////////////////////////////////////////////////////////////
// $Log:   S:/QUAR32/VCS/DLLMAIN.CPv  $
/////////////////////////////////////////////////////////////////////////////

#ifndef _NEWDEF_H_
#define _NEWDEF_H_

#include "idefevnt.h"

// {0F0E0EE0-760F-11d2-8E55-72C9EE000000}
DEFINE_GUID(CLSID_DefWatchEventHandler, 
0xf0e0ee0, 0x760f, 0x11d2, 0x8e, 0x55, 0x72, 0xc9, 0xee, 0x0, 0x0, 0x0);



class CNewDefHandler : public IDefWatchEventHandler
{
public:
    CNewDefHandler();
    ~CNewDefHandler();

    // 
    // IUnknown
    // 
    STDMETHOD( QueryInterface(REFIID, void**) );
    STDMETHOD_( ULONG, AddRef() );
    STDMETHOD_( ULONG, Release() );
    
    // 
    // IDefWatchEventHandler
    // 
    STDMETHOD( OnNewDefsInstalled( ) ) ;

private:
    DWORD m_dwRef;

};







#endif
