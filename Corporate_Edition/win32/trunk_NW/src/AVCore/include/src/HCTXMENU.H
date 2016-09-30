// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.

/******************************************************************************\
*
*                                 HCTXMENU.H
*
\******************************************************************************/

// Need the following 3 preprocessor statements--somewhere--and in this order.
#define __USE_REGISTRY

#include <platform.h>
#include <symcfg.h>   // Any Quake dependency problem here?


/******************************************************************************\
*                              CONSTANTS
\******************************************************************************/

// Used for obtaining Registry info.
const LPSTR HELPDESK_KEY            = "Info Desk"; 
const LPSTR CONTEXT_MENU_SECTION    = "Context Menu"; 

// Values in the Context Menu section.
const LPSTR  WHAT_MENU              = "Menu";
const LPSTR  SKIP_MENUITEMS         = "Menu Items";
const LPSTR  QUICK_PU_HELP          = "Quick PU Help";

// Masks for SKIP_MENUITEMS.
const UINT  SKIP_HOWTO              = 0x0001;
const UINT  SKIP_HELPDESK           = 0x0002;

// Default values to be used when Registry values DNE.
const BYTE WHAT_MENU_DEFAULT        = 1;      // Show our context menu, not Microsoft's.
const UINT SKIP_MENUITEMS_DEFAULT   = 0x0000; // Show all menu items.
const BYTE QUICK_PU_HELP_DEFAULT    = 1;      // Use quick pop-up help.


/******************************************************************************\
*                          HANDY INLINE FUNCTIONS
\******************************************************************************/

inline VOID GetDwordRegValue( CFGREGKEY &crk, LPCSTR lpszName, LPDWORD lpdwData, DWORD dwDefault )
    {
    DWORD dwThrowAway1, dwThrowAway2 = sizeof( DWORD );
    crk.lpszName = lpszName;
    if( ConfigRegQueryValue( &crk, NULL, &dwThrowAway1, lpdwData, &dwThrowAway2 ) != 
        ERROR_SUCCESS )                                             
        *lpdwData = dwDefault;               
    
    } // GetRegValue

inline VOID GetByteRegValue( CFGREGKEY &crk, LPCSTR lpszName, LPBYTE lpbData, BYTE bDefault )
    {
    DWORD dwThrowAway1, dwThrowAway2 = sizeof( BYTE );
    crk.lpszName = lpszName;
    if( ConfigRegQueryValue( &crk, NULL, &dwThrowAway1, lpbData, &dwThrowAway2 ) != 
        ERROR_SUCCESS )                                             
        *lpbData = bDefault;                 
    
    } // GetRegValue

/******************************************************************************\
*                       Sample "How do I use it" code
*
* NOTE: To insure that StdHlp's behavior matches the current settings the 
*		Registry values should be read every time a context menu is displayed or
* 		the user double-clicks w/secondary mouse button.
\******************************************************************************/

#ifdef NEVERDEFINED

//************
// Prepare to load values regarding the context menu.
CFGREGKEY crk = 
	{ 
	HKEY_CURRENT_USER, 
	NULL, 
	NULL, 
	HELPDESK_KEY, 
	CONTEXT_MENU_SECTION,
	NULL, 
	NULL,
	NULL
	};

BYTE bContextMenu;
DWORD dwSkipItems;
BYTE bQuickPUHelp;

// Whose context menu do we use? Microsoft's (only one item, "What's This?"), or
// ours ("What's This?" plus separator and possibly "Info Desk" and/or "How To").
GetByteRegValue( crk, WHAT_MENU, &bContextMenu), WHAT_MENU_DEFAULT );
																
// What menu items do we skip from showing?
GetDwordRegValue( crk, SKIP_MENUITEMS, &dwSkipItems), SKIP_MENUITEMS_DEFAULT );

if( dwSkipItems & SKIP_HOWTO != 0 ) // Show "How To" menu item (and associated popup menu).
	{
	//**TODO: Fill me in with code to add "How To" to context menu.
	}

if( dwSkipItems & SKIP_HELPDESK != 0 ) // Show "Info Desk" menu item (formerly "Help Desk").
	{
	//**TODO: Fill me in with code to add "Info Desk" to context menu.
	}

// Do we allow quick popup help?
GetByteRegValue( crk, QUICK_PU_HELP, &bQuickPUHelp), QUICK_PU_HELP_DEFAULT );
if( bQuickPUHelp )
	{
	//**TODO: Set up Quake to catch double-clicks w/secondary mouse button or whatever.
	}

#endif //NEVERDEFINED