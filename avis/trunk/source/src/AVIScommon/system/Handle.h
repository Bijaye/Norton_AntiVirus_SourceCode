// Handle.h: interface for the Handle class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_HANDLE_H__2F97112C_6933_11D2_892C_00A0C9DB9E9C__INCLUDED_)
#define AFX_HANDLE_H__2F97112C_6933_11D2_892C_00A0C9DB9E9C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "AVISCommon.h"

class AVISCOMMON_API Handle  
{
public:
	enum { invalidHandle = 0xffffffff };
	Handle() : handle((void*)invalidHandle) {};
	Handle(HANDLE h) : handle(h) {};

	void	SetHandle(HANDLE h) { if (((void*) invalidHandle) != handle) CloseHandle(handle); handle = h; };
	virtual ~Handle() { if (INVALID_HANDLE_VALUE != handle) CloseHandle(handle); 
						handle = 0;	}

	operator HANDLE()	{ return handle; }

private:
	HANDLE	handle;
};

#endif // !defined(AFX_HANDLE_H__2F97112C_6933_11D2_892C_00A0C9DB9E9C__INCLUDED_)
