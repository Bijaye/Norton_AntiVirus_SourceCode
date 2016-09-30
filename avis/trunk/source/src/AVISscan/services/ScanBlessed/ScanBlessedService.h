// ScanBlessedService.h: interface for the ScanBlessedService class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SCANBLESSEDSERVICE_H__F25AAA51_DC10_11D2_AD8A_00A0C9C71BBC__INCLUDED_)
#define AFX_SCANBLESSEDSERVICE_H__F25AAA51_DC10_11D2_AD8A_00A0C9C71BBC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <ScanService.h>

class ScanBlessedService : public ScanService  
{
public:
	ScanBlessedService();
	virtual ~ScanBlessedService();

protected:
	void TypeSpecificInitialization(void);

};

#endif // !defined(AFX_SCANBLESSEDSERVICE_H__F25AAA51_DC10_11D2_AD8A_00A0C9C71BBC__INCLUDED_)
