// ScanLatestService.h: interface for the ScanLatestService class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SCANLATESTSERVICE_H__0C651241_DC71_11D2_AD8B_00A0C9C71BBC__INCLUDED_)
#define AFX_SCANLATESTSERVICE_H__0C651241_DC71_11D2_AD8B_00A0C9C71BBC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <ScanService.h>

class ScanLatestService : public ScanService  
{
public:
	ScanLatestService();
	virtual ~ScanLatestService();

	void TypeSpecificInitialization(void);

};

#endif // !defined(AFX_SCANLATESTSERVICE_H__0C651241_DC71_11D2_AD8B_00A0C9C71BBC__INCLUDED_)
