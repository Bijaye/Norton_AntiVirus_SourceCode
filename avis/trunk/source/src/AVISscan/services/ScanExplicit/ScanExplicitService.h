// ScanExplicitService.h: interface for the ScanExplicitService class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SCANEXPLICITSERVICE_H__0C65124E_DC71_11D2_AD8B_00A0C9C71BBC__INCLUDED_)
#define AFX_SCANEXPLICITSERVICE_H__0C65124E_DC71_11D2_AD8B_00A0C9C71BBC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <ScanService.h>

class ScanExplicitService : public ScanService  
{
public:
	ScanExplicitService();
	virtual ~ScanExplicitService();

	void TypeSpecificInitialization(void);

};

#endif // !defined(AFX_SCANEXPLICITSERVICE_H__0C65124E_DC71_11D2_AD8B_00A0C9C71BBC__INCLUDED_)
