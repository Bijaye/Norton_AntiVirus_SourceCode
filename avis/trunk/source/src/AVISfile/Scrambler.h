// Scrambler.h: interface for the Scrambler class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SCRAMBLER_H__C580FAF7_6CD6_11D2_8939_00A0C9DB9E9C__INCLUDED_)
#define AFX_SCRAMBLER_H__C580FAF7_6CD6_11D2_8939_00A0C9DB9E9C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string>

#include "AVIS.h"
#include "AVISFile.h"

class AVISFILE_API Scrambler  
{
protected:
	Scrambler();

public:
	virtual			~Scrambler();

	virtual bool	Scramble(uchar* buffer, uint bufferSize) = 0;

	virtual const std::string&	Name(void) = 0;
};

#endif // !defined(AFX_SCRAMBLER_H__C580FAF7_6CD6_11D2_8939_00A0C9DB9E9C__INCLUDED_)
