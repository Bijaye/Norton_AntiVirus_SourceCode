// QuarantineServer.h: interface for the QuarantineServer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_QUARANTINESERVER_H__E36A3DD6_EC26_11D2_ADA9_00A0C9C71BBC__INCLUDED_)
#define AFX_QUARANTINESERVER_H__E36A3DD6_EC26_11D2_ADA9_00A0C9C71BBC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string>

#include "QSEnum.h"

class QuarantineServer  
{
public:
	QuarantineServer();
	virtual ~QuarantineServer();

	bool	GetQSEnum(QSEnum& qsEnum);

private:
	bool	GetErrorMessage(std::string& errorMessage);
};

#endif // !defined(AFX_QUARANTINESERVER_H__E36A3DD6_EC26_11D2_ADA9_00A0C9C71BBC__INCLUDED_)
