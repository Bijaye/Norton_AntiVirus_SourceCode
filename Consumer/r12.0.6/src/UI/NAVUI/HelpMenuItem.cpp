// HelpMenuItem.cpp: implementation of the CHelpMenuItem class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "HelpMenuItem.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CHelpMenuItem::CHelpMenuItem()
{
    m_MenuType = eSymSWType_URL;
    m_wItemID = 0;
    m_wTextID = 0;
}

CHelpMenuItem::~CHelpMenuItem()
{
}

void CHelpMenuItem::SetMenuType ( SYMSW_MENUITEMTYPE menuType )
{
    m_MenuType = menuType; 
}

void CHelpMenuItem::SetTextID ( WORD wItemID )
{
    m_wTextID = wItemID;
}
 
SYMSW_MENUITEMTYPE  CHelpMenuItem::GetMenuType ()
{
    return m_MenuType;
}

WORD CHelpMenuItem::GetTextID ()
{
    return m_wTextID;
}

WORD CHelpMenuItem::GetItemID ()
{
    return m_wItemID;
}

void CHelpMenuItem::SetItemID (WORD wItemID)
{
    m_wItemID = wItemID;
}