// HelpMenuItem.h: interface for the CHelpMenuItem class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_HELPMENUITEM_H__BB749A52_8A9B_4B23_A53C_0D01009733FB__INCLUDED_)
#define AFX_HELPMENUITEM_H__BB749A52_8A9B_4B23_A53C_0D01009733FB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "swmisc.h" // NSW include

class CHelpMenuItem  
{
public:
	CHelpMenuItem();
	virtual ~CHelpMenuItem();

    virtual void SetItemID ( WORD wItemID );                    // Menu order
    virtual void SetMenuType ( SYMSW_MENUITEMTYPE menuType );   // Menu type (from Integrator)
    virtual void SetTextID ( WORD wTextID );                    // Resource ID for the label
 
    virtual WORD                GetItemID ();
    virtual SYMSW_MENUITEMTYPE  GetMenuType ();
    virtual WORD                GetTextID ();
    virtual HRESULT             DoWork (HWND hMainWnd) = 0;     // Gets called when the item is clicked

protected:
    SYMSW_MENUITEMTYPE m_MenuType;
    WORD m_wItemID;
    WORD m_wTextID;
};

template< class _T_Item >
class CHelpMenuItemFactoryT
{
public:
    static bool CreateObject(CHelpMenuItem** ppItem)
    {
        try
        {
            _ASSERTE(ppItem != NULL);
            if(ppItem == NULL)
                return false;
            *ppItem = new _T_Item;
            return true;
        }
        catch(...)
        {}
        return false;
    }
};
#endif // !defined(AFX_HELPMENUITEM_H__BB749A52_8A9B_4B23_A53C_0D01009733FB__INCLUDED_)
