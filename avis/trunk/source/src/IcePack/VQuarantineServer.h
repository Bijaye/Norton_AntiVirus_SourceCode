// VQuarantineServer.h: interface for the VQuarantineServer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_VQuarantineServer_H__E36A3DD6_EC26_11D2_ADA9_00A0C9C71BBC__INCLUDED_)
#define AFX_VQuarantineServer_H__E36A3DD6_EC26_11D2_ADA9_00A0C9C71BBC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string>
#include <CMclCritSec.h>
#include <qserver.h>

#include "VQSEnum.h"

class VQuarantineServer  
{
public:
	VQuarantineServer();
	virtual ~VQuarantineServer();

	bool	GetVQSEnum(VQSEnum& VQSEnum);

private:
	CMclCritSec					inUse;
	IQuarantineServer			*qs;

	bool	GetErrorMessage(std::string& errorMessage);
};

class QSThreadLife
{
public:
	QSThreadLife()	{ CoInitializeEx(NULL, COINIT_MULTITHREADED); }
	~QSThreadLife()	{ CoUninitialize();	}
};


#endif // !defined(AFX_VQuarantineServer_H__E36A3DD6_EC26_11D2_ADA9_00A0C9C71BBC__INCLUDED_)
