//
//	IBM AntiVirus Immune System
//
//	File Name:	MD5Calculator.cpp
//	Author:		Andy Klapper
//
//	The MD5Calculator is a concrete sub class of CRCCalculator and
//	provides the CRCCalculator interface using the MD5 algorithm.
//
//	$Log: /AVISdb/ImmuneSystem/FilterTestHarness/MD5Calculator.cpp $
//
//1     8/19/98 9:10p Andytk
//Initial check in
//

#include "stdafx.h"
#include "AVISFile.h"
#include "MD5Calculator.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

std::string	MD5Calculator::typeName("md5");

MD5Calculator::MD5Calculator() : CRCCalculator()
{
	MD5Init(&context);
}

MD5Calculator::~MD5Calculator()
{

}


bool MD5Calculator::CRCCalculate(const uchar* buffer, uint bufferSize)
{
	MD5Update(&context, buffer, bufferSize);

	return true;
}


bool MD5Calculator::Done(std::string& crcString)
{
	uchar	crc[16];
	char	buffer[4];
	MD5Final(crc, &context);

	crcString = "";

	for (int i=0; i< 16; i++)
	{
		sprintf(buffer, "%02x", crc[i]);
		crcString	+= buffer;
	}

	return true;
}

