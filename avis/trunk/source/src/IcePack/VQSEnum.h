// VQSEnum.h: interface for the VQSEnum class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_VQSENUM_H__E36A3DD5_EC26_11D2_ADA9_00A0C9C71BBC__INCLUDED_)
#define AFX_VQSENUM_H__E36A3DD5_EC26_11D2_ADA9_00A0C9C71BBC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <AVIS.h>
#include <list>

#include <qserver.h>

#include "Sample.h"

class VQSEnum  
{
public:
	VQSEnum();
	virtual ~VQSEnum();

	bool	Set(IEnumQuarantineServerItems *items);
	bool	Reset(void);
	bool	Next(Sample& sample, std::list<Sample>& existing, bool& newRecord,
					std::list<Sample>::iterator& i);
//	bool	Next(Sample& sample, std::list<Sample>& existing, bool& newRecord, bool& changed);

private:
	ulong						sampleID;
	IEnumQuarantineServerItems *items;

	void GetErrorMessage(std::string& errMsg);
};

#endif // !defined(AFX_VQSENUM_H__E36A3DD5_EC26_11D2_ADA9_00A0C9C71BBC__INCLUDED_)
