// HMIProductReg.h: interface for the CHMIProductReg class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_HMIPRODUCTREG_H__01D0734B_900B_4B6C_ACDD_A80437A03048__INCLUDED_)
#define AFX_HMIPRODUCTREG_H__01D0734B_900B_4B6C_ACDD_A80437A03048__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "HelpMenuItem.h"
#include "AVRES.h"

class CHMIProductReg : public CHelpMenuItem  
{
public:
	CHMIProductReg();
	virtual ~CHMIProductReg();

    HRESULT DoWork (HWND hMainWnd);
};

class CHMIProductRegFactory: public CHelpMenuItemFactoryT<CHMIProductReg>
{
public:
    static bool ShouldShow();
};

#endif // !defined(AFX_HMIPRODUCTREG_H__01D0734B_900B_4B6C_ACDD_A80437A03048__INCLUDED_)
