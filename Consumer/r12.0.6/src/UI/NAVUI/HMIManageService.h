// HMIManageService.h
//
//////////////////////////////////////////////////////////////////////

#if !defined(_HMIMANAGESERVICE_H__9EE5EBC9_F9CE_44A0_BFA9_9FCB85EE8709__INCLUDED_)
#define _HMIMANAGESERVICE_H__9EE5EBC9_F9CE_44A0_BFA9_9FCB85EE8709__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000



#include "HelpMenuItem.h"
#include "NAVInfo.h"

class CHMIManageService : public CHelpMenuItem  
{
public:
	CHMIManageService();
	virtual ~CHMIManageService();
    HRESULT DoWork (HWND hMainWnd);

protected:
};

class CHMIManageServiceFactory: public CHelpMenuItemFactoryT<CHMIManageService>
{
public:
    static bool ShouldShow();
};

#endif // !defined(_HMIMANAGESERVICE_H__9EE5EBC9_F9CE_44A0_BFA9_9FCB85EE8709__INCLUDED_)
