// HMIResponse.h: interface for the CHMIResponse class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_HMIRESPONSE_H__DC91D847_4F6F_44B7_B93C_5419FF0AD1CD__INCLUDED_)
#define AFX_HMIRESPONSE_H__DC91D847_4F6F_44B7_B93C_5419FF0AD1CD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "HelpMenuItem.h"

class CHMIResponse : public CHelpMenuItem  
{
public:
	CHMIResponse();
	virtual ~CHMIResponse();
    HRESULT DoWork (HWND hMainWnd);
 
};
class CHMIResponseFactory: public CHelpMenuItemFactoryT<CHMIResponse>
{
public:
    static bool ShouldShow();
};
#endif // !defined(AFX_HMIRESPONSE_H__DC91D847_4F6F_44B7_B93C_5419FF0AD1CD__INCLUDED_)
