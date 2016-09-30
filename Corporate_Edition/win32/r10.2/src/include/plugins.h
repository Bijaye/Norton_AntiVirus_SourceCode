// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
//
// PLUGINS.H
//

#ifndef __PLUGINS_H
#define __PLUGINS_H

//
// Types
//
struct CPlugin
{
	IUnknown*	m_pUnknown;
	DWORD		m_dwThreadControl;
};

//
// Definitions
//
#define _IID_IRTVPlugin		{ 0x74CB568F, 0xD5F5, 0x11D1, { 0xA2, 0xCC, 0x0, 0xA0, 0xC9, 0x74,0x9E, 0xDB } }
#define REG_PLUGINS_BASE	_T( "SOFTWARE\\Intel\\LANDesk\\VirusProtect6\\CurrentVersion\\VPTRAY\\Plugins" )
#define REG_ENABLE_VALUE	_T( "Enable Plugins" )
#define VI(x)				virtual x __stdcall
#define MAX_PLUGINS			32

//
// Prototypes
//
void LoadPlugins();
void UnloadPlugins();

#endif /* ndef __PLUGINS_H */
