////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
// agentplugin.h  - Contains definitions and structures used to plug into
// the NAV Tray Agent.
//
///////////////////////////////////////////////////////////////////////////////

#if !defined( _AGENTPLUGIN_H_ )
#define _AGENTPLUGIN_H_ 

///////////////////////////////////////////////////////////////////////////////
// Interfaces

struct IAgentPlugin
{
	//
	// Called to initialize plugin object.
	//
	// Parameters: Parent window to use when the plugin needs to display a window.
	//
	// Return value: true on success, false otherwise.
	//
	virtual bool Initialize( HWND hParentWnd ) = 0;
	
	//
	// Called to destory plugin object.
	//
	// Parameters: none.
	//
	virtual void DeleteObject( void ) = 0;

	//
	// Called when the agent wants to know the ID associated with this plugin.  
	// This is used to determine the order in which the plugins will be called
	// when adding menu items.
	//
	// Return value: The ID associated with this plugin.
	//
	virtual DWORD GetID( void ) = 0;

	//
	// Called when the popup is about to be displayed.  Plugins can
	// add thier own menu items.  For the moment, we do not allow more
	// than 10 command items. ( Even this is excessive... )
	//
	// Paremeters:	hPopup - handle to the popup menu
	//				iIndex - Zero-based position at which to insert the first menu item.
	//				LOWORD( nCmdRange ) - Lowest possible value to use for command ID.
	//				HIWORD( nCmdRange ) - Highest possible value to use for command ID.
	//
	virtual void OnPopupMenu( HMENU hPopop, int iIndex, DWORD nCmdRange ) = 0;

	//
	// Called when a command is issued.
	//
	// Paremeters:	wCommand - ID of command.
	//
	// Return value: return true if command was handled, false otherwise.
	//
	virtual bool OnCommand( WORD wCommand ) = 0;
};

///////////////////////////////////////////////////////////////////////////////
// Exported function pointer.  All Agent plugins must export a function in 
// this form.
//
// extern "C" __declspec(dllexport) IAgentPlugin* WINAPI CreateObject( DWORD dwReserved )
//	
typedef IAgentPlugin* ( WINAPI *PFNCREATEOBJECT ) ( DWORD dwReserved );

///////////////////////////////////////////////////////////////////////////////
// Registry key for plugins.  Under HKEY_LOCAL_MACHINE.

const TCHAR REGKEY_NAVAGENT_PLUGIN[] = _T("SOFTWARE\\Symantec\\Norton AntiVirus\\AgentPlugins");

///////////////////////////////////////////////////////////////////////////////
#endif // #if !defined( _AGENTPLUGIN_H_ )