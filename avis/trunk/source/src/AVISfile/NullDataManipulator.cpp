// NullDataManipulator.cpp: implementation of the NullDataManipulator class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "NullDataManipulator.h"

#include "AVISFileException.h"

const std::string	NullDataManipulator::typeName("");

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

NullDataManipulator::NullDataManipulator()
{

}

NullDataManipulator::~NullDataManipulator()
{

}

bool NullDataManipulator::Process(const uchar* inBuffer, uint& inSize, uchar* outBuffer,
								   uint& outSize)
{
	if (inSize > outSize)
		throw AVISFileException(AVISFileException::NullDataManip, "Process, inSize > outSize");

	memcpy(outBuffer, inBuffer, inSize);
	outSize = inSize;
	inSize	= 0;

	return true;
}
