//
//	IBM AntiVirus Immune System
//
//	File Name:	CompoundDataManipulator.h
//	Author:		Andy Klapper
//
//	This class makes a list of DataManipulator objects look like a single
//	DataManipulator object.
//
//	$Log:  $
//	 
//

#if !defined(AFX_OMPOUNDDATAMANIPULATOR_H__2D9CC668_7E60_11D2_ACEA_00A0C9C71BBC__INCLUDED_)
#define AFX_OMPOUNDDATAMANIPULATOR_H__2D9CC668_7E60_11D2_ACEA_00A0C9C71BBC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DataManipulator.h"


class  AVISFILE_API CompoundDataManipulator : public DataManipulator  
{
public:
	CompoundDataManipulator();
	virtual ~CompoundDataManipulator();

	bool	Process(const uchar* inBuffer, uint& inSize,
					uchar* outBuffer, uint& outSize);
	bool	Done(uchar* outBuffer, uint& outSize);
	ErrorCode Error(void);

			const std::string&	Name(void)		{ return typeName; };
	static	const std::string&	TypeName(void)	{ return typeName; };

	bool	Add(DataManipulator& dataManipulator);

private:
	static const std::string	typeName;

	enum { MaxMembers = 10, BufferSize = 4096 };

	class ManipulatorState
	{
		DataManipulator&	manipulator;
		uint				maxSize;
		uint				inSize;
		uchar*				buffer;

		ManipulatorState(DataManipulator& dManipulator, uint mSize, uint iSize,
							uchar* buff) :	manipulator(dManipulator), maxSize(mSize),
											inSize(iSize), buffer(buff)
		{ };

		friend class CompoundDataManipulator;
	};

	ManipulatorState*	states[MaxMembers];
	uint				length;
	bool				inProcess;

	bool	RProcess(uint i);
	bool	RDone(uint i);
};

#endif // !defined(AFX_OMPOUNDDATAMANIPULATOR_H__2D9CC668_7E60_11D2_ACEA_00A0C9C71BBC__INCLUDED_)
