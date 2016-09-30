// SimpleScrambler.cpp: implementation of the SimpleScrambler class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SimpleScrambler.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

std::string SimpleScrambler::typeName("xor-vampish");

SimpleScrambler::SimpleScrambler()
{

}

SimpleScrambler::~SimpleScrambler()
{

}

bool SimpleScrambler::Process(const uchar* inBuffer, uint& inSize, uchar* outBuffer,
							uint& outSize)
{
	return unScrambler.Process(inBuffer, inSize, outBuffer, outSize);
}
