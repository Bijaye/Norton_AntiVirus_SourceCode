// NullDataManipulator.h: interface for the NullDataManipulator class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NULLDATAMANIPULATOR_H__2D9CC667_7E60_11D2_ACEA_00A0C9C71BBC__INCLUDED_)
#define AFX_NULLDATAMANIPULATOR_H__2D9CC667_7E60_11D2_ACEA_00A0C9C71BBC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DataManipulator.h"

class NullDataManipulator : public DataManipulator  
{
public:
	NullDataManipulator();
	virtual ~NullDataManipulator();

	bool Process(const uchar* inBuffer, uint& inSize,
										uchar* outBuffer, uint& outSize);
	bool Done(uchar* outBuffer, uint& outSize) { outSize = 0; return false; };

	ErrorCode Error() { return NoError; };

			const std::string&	Name(void)		{ return typeName; };
	static	const std::string&	TypeName(void)	{ return typeName; };

private:
	static const std::string	typeName;

};

#endif // !defined(AFX_NULLDATAMANIPULATOR_H__2D9CC667_7E60_11D2_ACEA_00A0C9C71BBC__INCLUDED_)
