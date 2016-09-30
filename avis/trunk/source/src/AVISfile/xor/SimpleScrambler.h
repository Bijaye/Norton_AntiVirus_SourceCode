// SimpleScrambler.h: interface for the SimpleScrambler class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SIMPLESCRAMBLER_H__C580FAF6_6CD6_11D2_8939_00A0C9DB9E9C__INCLUDED_)
#define AFX_SIMPLESCRAMBLER_H__C580FAF6_6CD6_11D2_8939_00A0C9DB9E9C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string>

#include <AVIS.h>
#include "AVISFile.h"
#include "Scrambler.h"
#include "SimpleUnScrambler.h"
#include "ToolFactory.h"

#pragma warning(disable: 4251)	// warns that std::string needs to be exported
								// to dll users (which would be true if it was
								// an internal class)

class AVISFILE_API SimpleScrambler : public DataManipulator
{
private:
	SimpleUnScrambler	unScrambler;

public:
	SimpleScrambler();
	~SimpleScrambler();

private:
	static std::string	typeName;

public:
	bool	Process(const uchar* inBuffer, uint& inSize, uchar* outBuffer,
							uint& outSize);
	bool	Done(uchar* outBuffer, uint& outSize) { outSize = 0; return false; };

	ErrorCode	Error(void)	{ return unScrambler.Error(); };

			const std::string&	Name(void) { return typeName; };
	static	const std::string&	TypeName(void) { return typeName; };
};

#pragma warning(default: 4251)	// try to limit the scope of this a little bit

#endif // !defined(AFX_SIMPLESCRAMBLER_H__C580FAF6_6CD6_11D2_8939_00A0C9DB9E9C__INCLUDED_)
