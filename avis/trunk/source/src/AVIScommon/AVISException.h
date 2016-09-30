// AVISException.h: interface for the AVISException class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AVISEXCEPTION_H__82ED1F1B_E5DA_11D2_AD9E_00A0C9C71BBC__INCLUDED_)
#define AFX_AVISEXCEPTION_H__82ED1F1B_E5DA_11D2_AD9E_00A0C9C71BBC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "AVISCommon.h"
#include <string>

class AVISCOMMON_API AVISException  
{
public:
	AVISException()				{};
	virtual ~AVISException()	{};

	virtual std::string ClassAsString(void) = 0;
	virtual std::string	DetailedInfo(void)	= 0;
	virtual std::string TypeAsString(void)	= 0;

	virtual std::string	FullString(void);

	void	LogException(void);
};

#endif // !defined(AFX_AVISEXCEPTION_H__82ED1F1B_E5DA_11D2_AD9E_00A0C9C71BBC__INCLUDED_)
