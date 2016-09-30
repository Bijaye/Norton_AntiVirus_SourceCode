// /////////////////////////////////////////////////////////////////////////////////////////////////
// Symantec copyright header start
// //////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2004 Symantec Corporation.
// All rights reserved.
//
// /////////////////////////////////////////////////////////////////////////////////////////////////
// Symantec copyright header stop
// //////////////////////
#ifndef __GLOBALEVENT_H__
#define __GLOBALEVENT_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// @class CGlobalEvent
//
// This class is used to manipulate named events that are shared across
// all windows sessions.
//
// The handle returned by the operator HANDLE method can be used with any
// of the Win32 synchronizaton functions, as well as the Event functions.

class CGlobalEvent
{
public:
	// Constructor
	CGlobalEvent();
	~CGlobalEvent();

	// Methods
	BOOL Create( LPCSTR szEventName, BOOL bManualReset = TRUE );
	BOOL CreateW( LPCWSTR szEventName, BOOL bManualReset = TRUE );

	// Operator overides.
	operator HANDLE () { return m_hEvent; };

private:
	// Disallowed
	CGlobalEvent( CGlobalEvent& other );
	
	// Handle to event object
	HANDLE		m_hEvent;

	// Seciruty descriptor for event object
	SECURITY_DESCRIPTOR m_sdEvent;
};

#endif // ifndef __GLOBALEVENT_H__