////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef CHKNAVFILES_H
#define CHKNAVFILES_H

#include "stdafx.h"
#include "ccAppPlugin.h"
#include "GlobalEvent.h"
#include "global.h"



class CPlugin : 
	public ISymBaseImpl< CSymThreadSafeRefCount >, 
	public ICCAppPlugin
{
public:
	CPlugin();
	~CPlugin();
	
	SYM_INTERFACE_MAP_BEGIN()                
		SYM_INTERFACE_ENTRY( IID_CCAppPlugin, ICCAppPlugin ) 
	SYM_INTERFACE_MAP_END() 

	// ISymAppPlugin Methods
	virtual void Run( ICCApp* pSymApp );	
	virtual bool CanStopNow(){return true;}
	virtual void RequestStop(){	SetEvent( m_hKillEvent );}
	virtual const char * GetObjectName(){return "Check NAV Files";}
	virtual unsigned long OnMessage( int iMessage, unsigned long ulParam ) {return 0;}


private:
	// Event used to exit the plugin.
	HANDLE m_hKillEvent;
	
	// Event signaling a NAV uninstall
	CGlobalEvent m_NAVUninstallEvent;
	

};

#endif