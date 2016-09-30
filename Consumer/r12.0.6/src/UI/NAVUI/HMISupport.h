// HMISupport.h: interface for the CHMISupport class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_HMISUPPORT_H__529B3ECD_619E_4724_8A01_88300B24E7BA__INCLUDED_)
#define AFX_HMISUPPORT_H__529B3ECD_619E_4724_8A01_88300B24E7BA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "HelpMenuItem.h"

class CHMISupport : public CHelpMenuItem  
{
public:
	CHMISupport();
	virtual ~CHMISupport();

    HRESULT DoWork (HWND hMainWnd);
 
};
class CHMISupportFactory: public CHelpMenuItemFactoryT<CHMISupport>
{
public:
    static bool ShouldShow();
};
#endif // !defined(AFX_HMISUPPORT_H__529B3ECD_619E_4724_8A01_88300B24E7BA__INCLUDED_)
