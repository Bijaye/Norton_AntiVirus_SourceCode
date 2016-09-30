// HMIClubSymantec.h: interface for the CHMIClubSymantec class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_HMIClubSymantec_H__DC91D847_4F6F_44B7_B93C_5419FF0AD1CD__INCLUDED_)
#define AFX_HMIClubSymantec_H__DC91D847_4F6F_44B7_B93C_5419FF0AD1CD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "HelpMenuItem.h"

class CHMIClubSymantec : public CHelpMenuItem  
{
public:
	CHMIClubSymantec();
	virtual ~CHMIClubSymantec();
    HRESULT DoWork (HWND hMainWnd);
 
};
class CHMIClubSymantecFactory: public CHelpMenuItemFactoryT<CHMIClubSymantec>
{
public:
    static bool ShouldShow();
};
#endif // !defined(AFX_HMIClubSymantec_H__DC91D847_4F6F_44B7_B93C_5419FF0AD1CD__INCLUDED_)
