// HMIProducts.h: interface for the CHMIProducts class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_HMIPRODUCTS_H__14AE92C6_FE3B_478B_A08E_0EF3F125AECC__INCLUDED_)
#define AFX_HMIPRODUCTS_H__14AE92C6_FE3B_478B_A08E_0EF3F125AECC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string>

#include "HelpMenuItem.h"
#include "NAVInfo.h"

class CHMIProducts : public CHelpMenuItem  
{
public:
	CHMIProducts();
	virtual ~CHMIProducts();
    HRESULT DoWork (HWND hMainWnd);
};

class CHMIProductsFactory: public CHelpMenuItemFactoryT<CHMIProducts>
{
public:
    static bool ShouldShow();
};
#endif // !defined(AFX_HMIPRODUCTS_H__14AE92C6_FE3B_478B_A08E_0EF3F125AECC__INCLUDED_)
