// QSEnum.h: interface for the QSEnum class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_QSENUM_H__E36A3DD5_EC26_11D2_ADA9_00A0C9C71BBC__INCLUDED_)
#define AFX_QSENUM_H__E36A3DD5_EC26_11D2_ADA9_00A0C9C71BBC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <AVIS.h>
#include <list>

#include "Sample.h"

class QSEnum  
{
public:
	QSEnum();
	virtual ~QSEnum();

	bool	Reset(void);
	bool	Next(Sample& sample);
	bool	Next(Sample& sample, std::list<Sample>& existing, bool& newRecord, bool& changed);

private:
	ulong		sampleID;

	void GetErrorMessage(std::string& errMsg);
};

#endif // !defined(AFX_QSENUM_H__E36A3DD5_EC26_11D2_ADA9_00A0C9C71BBC__INCLUDED_)
