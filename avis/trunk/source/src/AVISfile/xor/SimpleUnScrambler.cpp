//
//	IBM AntiVirus Immune System
//
//	File Name:	SimpleUnScrambler.cpp
//	Author:		Andy Klapper
//
//	This concrete UnScrambler class unscrambles files using the simple
//	method.
//
//	$Log: /AVISdb/ImmuneSystem/FilterTestHarness/SimpleUnScrambler.cpp $
//
//1     8/19/98 9:10p Andytk
//Initial check in
//

#include "stdafx.h"
#include <assert.h>

#include "AVISFile.h"
#include "SimpleUnScrambler.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


const std::string	SimpleUnScrambler::typeName("xor-vampish");

const uchar	SimpleUnScrambler::scrambleMask[] =
									{ 'Y', 'A', 'K', 'V', 'A', 'M', 'P', 'I',
									  'S', 'H', 'U', 'C', 'H', 'E', 'G', 'O',
									  'Z', 'H', 'E', 'B', 'O', 'L', 'E', '.',
									  '.' };
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

SimpleUnScrambler::SimpleUnScrambler() : DataManipulator(), maskOffset(0),
										 bytesAlreadyConsumed(0)
{

}

SimpleUnScrambler::~SimpleUnScrambler()
{

}

//
//	Note:  This could be made faster by working with ints instead of chars.

bool SimpleUnScrambler::Process(const uchar* inBuffer, uint& inSize, uchar* outBuffer,
								   uint& outSize)
{
	uint	bytesToProcess	= inSize - bytesAlreadyConsumed;
	if (bytesToProcess > outSize)
		bytesToProcess = outSize;

	memcpy(outBuffer, &inBuffer[bytesAlreadyConsumed], bytesToProcess);
	outSize = bytesToProcess;

	uint	buffOffset = 0;

	while (buffOffset < outSize)
	{
		if (maskOffset >= sizeof(scrambleMask))
			maskOffset = 0;
		outBuffer[buffOffset++] ^= scrambleMask[maskOffset++];
	}

	if (bytesToProcess + bytesAlreadyConsumed >= inSize)
	{
		inSize					= 0;
		bytesAlreadyConsumed	= 0;
	}
	else
		bytesAlreadyConsumed	+= bytesToProcess;

	return outSize > 0;
}
