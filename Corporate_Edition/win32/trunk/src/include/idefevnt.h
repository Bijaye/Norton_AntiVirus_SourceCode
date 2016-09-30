// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright 1998 - 2003, 2005 Symantec Corporation. All rights reserved.
//*************************************************************************
// idefevnt.h - created 11/6/98 1:57:15 PM
//
// $Header$
//
// Contains: Contains interface definition for DefWatch event
//           handler objects.
//
//*************************************************************************
// $Log$
//*************************************************************************

#ifndef _idefevnt_h_
#define _idefevnt_h_ 

#ifdef INITGUID
#include "initguid.h"
#endif

//*************************************************************************
// 
// IDefWatchEventHandler
// 
//*************************************************************************

// {66E44FA0-7608-11d2-8E55-72C9EE000000}
DEFINE_GUID( IID_DefWatchEventHandler,
    0x66e44fa0, 0x7608, 0x11d2, 0x8e, 0x55, 0x72, 0xc9, 0xee, 0x0, 0x0, 0x0);

class IDefWatchEventHandler : public IUnknown
{
public:    
    //*************************************************************************
    // STDMETHOD OnnNewDefsInstalled( )
    //
    // Description: This routine is invoked when the DefWatch daemon detects 
    //              new virus definitions.  The return value is ignored.
    //
    // Returns: HRESULT - S_OK
    //
    // Notes:   Note that your handler is invoked from the DefWatch service 
    //          under Windows NT, you will most likely be invoked under the
    //          NT_LOCALAUTHORITY user context, not the context of the 
    //          currently logged in user.
    //
    //*************************************************************************
    // 11/10/98 DBUCHES, created - header added.
    //*************************************************************************
    STDMETHOD( OnNewDefsInstalled( ) ) 
        PURE;
};

typedef IDefWatchEventHandler* PDEFWATCHEVENTHANDLER;

#endif

