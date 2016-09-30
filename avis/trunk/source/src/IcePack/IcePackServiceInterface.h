// IcePackServiceInterface.h: interface for the IcePackServiceInterface class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ICEPACKSERVICEINTERFACE_H__CEC1FE01_00B2_11D3_ADD3_00A0C9C71BBC__INCLUDED_)
#define AFX_ICEPACKSERVICEINTERFACE_H__CEC1FE01_00B2_11D3_ADD3_00A0C9C71BBC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "CService.h"

class IcePackServiceInterface : public CService  
{
public:
	IcePackServiceInterface(LPTSTR name, LPTSTR display, DWORD type) :
								CService(name, display, type)
	{
		m_pThis = this;

//		m_hPauseNotifyEvent	= NULL;
	};

	virtual ~IcePackServiceInterface() {};

	void	Run();

	DECLARE_SERVICE(IcePackServiceInterface, IcePack)

protected:
	void	OnStop();
	void	Init();
	void	DeInit();
};

#endif // !defined(AFX_ICEPACKSERVICEINTERFACE_H__CEC1FE01_00B2_11D3_ADD3_00A0C9C71BBC__INCLUDED_)
