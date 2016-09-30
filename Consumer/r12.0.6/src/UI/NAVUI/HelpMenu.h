// HelpMenu.h: interface for the CHelpMenu class.
//
// This class is a container for CHelpMenuItem classes and handles
// the calls from the INSWHelpMenuEx interface from the
// INSWIntegratorProduct(Plugin). 
//
// Add the menu items to the contructor as needed. Pass the 
// INSWHelpMenuEx calls into the CHelpMenu functions.
// 
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_HELPMENU_H__E0A0837F_C70A_4147_8F22_F2C64465EFBF__INCLUDED_)
#define AFX_HELPMENU_H__E0A0837F_C70A_4147_8F22_F2C64465EFBF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "HelpMenuItem.h"
#include "swmisc.h"

#include <vector>
typedef ::std::vector <CHelpMenuItem*> vecHelpMenuItems;
typedef vecHelpMenuItems::iterator iterHelpMenuItems;

class CHelpMenu  
{
public:
	CHelpMenu();
	virtual ~CHelpMenu();

    HRESULT ResetMenuItemEnum();
    HRESULT GetNextMenuItem( WORD *pdwItemID,
                             SYMSW_MENUITEMTYPE *pdwItemType,
                             UINT *pnItemText);
    HRESULT DoMenuItem ( WORD dwItemID,
                         HWND hMainWnd);
protected:
	WORD m_wCurrentIndex;
    vecHelpMenuItems m_vecHelpMenuItems;
};
#endif // !defined(AFX_HELPMENU_H__E0A0837F_C70A_4147_8F22_F2C64465EFBF__INCLUDED_)
