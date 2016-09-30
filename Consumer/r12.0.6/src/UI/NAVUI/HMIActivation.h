// HMIActivation.h
//
//////////////////////////////////////////////////////////////////////

#if !defined(_HMIACTIVATION_H__9EE5EBC9_F9CE_44a0_BFA9_9FCB85EE8709__INCLUDED_)
#define _HMIACTIVATION_H__9EE5EBC9_F9CE_44a0_BFA9_9FCB85EE8709__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000



#include "HelpMenuItem.h"
#include "NAVInfo.h"

class CHMIActivation : public CHelpMenuItem  
{
public:
	CHMIActivation();
	virtual ~CHMIActivation();
    HRESULT DoWork (HWND hMainWnd);

protected:
};

class CHMIActivationFactory: public CHelpMenuItemFactoryT<CHMIActivation>
{
public:
    static bool ShouldShow();
};

#endif // !defined(_HMIACTIVATION_H__9EE5EBC9_F9CE_44a0_BFA9_9FCB85EE8709__INCLUDED_)
