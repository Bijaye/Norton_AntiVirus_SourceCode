// MappedMemory.h: interface for the MappedMemory class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAPPEDMEMORY_H__D16768B8_69B7_11D2_892F_00A0C9DB9E9C__INCLUDED_)
#define AFX_MAPPEDMEMORY_H__D16768B8_69B7_11D2_892F_00A0C9DB9E9C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string>

#include "AVIS.h"

#include "AVISCommon.h"
#include "Handle.h"
#include "MapView.h"

class AVISCOMMON_API MappedMemory  
{
public:
	MappedMemory(const std::string& fileName, const uint maxSize);
	virtual ~MappedMemory();

	uchar*	Begining(void)	{ return (uchar *) view; };
	uchar*	CurrEnd(void)	{ return (0 == (uchar *) view) ? NULL :
												&(((uchar*)view)[currEnd]); }
	void	ExtendEnd(uint length) { currEnd += length;
									 if (currEnd > maxSize) currEnd = maxSize;}
	uint	LengthLeft(void){ return maxSize - currEnd; }

  bool	WriteToFile(const std::string& fileName);

private:
	MapView	view;
	uint	maxSize;
	uint	currEnd;
};

#endif // !defined(AFX_MAPPEDMEMORY_H__D16768B8_69B7_11D2_892F_00A0C9DB9E9C__INCLUDED_)
