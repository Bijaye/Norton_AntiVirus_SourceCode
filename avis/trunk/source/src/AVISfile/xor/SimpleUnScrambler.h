//
//	IBM AntiVirus Immune System
//
//	File Name:	SimpleUnScrambler.h
//	Author:		Andy Klapper
//
//	The class implements the UnScrambler interface using the Simple
//	unscrambling algorithm.
//
//	$Log: /AVISdb/ImmuneSystem/FilterTestHarness/SimpleUnScrambler.h $
//	
//	1     8/19/98 9:10p Andytk
//	Initial check in
//

#if !defined(AFX_SimpleUnScrambler_H__D2432425_15D4_11D2_9056_947982000000__INCLUDED_)
#define AFX_SimpleUnScrambler_H__D2432425_15D4_11D2_9056_947982000000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "DataManipulator.h"

#include "ToolFactory.h"

class SimpleScrambler;

#pragma warning(disable: 4251)	// warns that std::string needs to be exported
								// to dll users (which would be true if it was
								// an internal class)

class AVISFILE_API SimpleUnScrambler : public DataManipulator  
{
public:
	SimpleUnScrambler();
	virtual ~SimpleUnScrambler();

private:
	static const uchar	scrambleMask[];
	
	int		maskOffset;
	uint	bytesAlreadyConsumed;

	static const std::string	typeName;

public:
	bool	Process(const uchar* inBuffer, uint& inSize, uchar* outBuffer,
							uint& outSize);
	bool	Done(uchar* outBuffer, uint& outSize) { outSize = 0; return false; };

	ErrorCode	Error(void)	{ return NoError; };

			const std::string&	Name(void)		{ return typeName; };
	static	const std::string&	TypeName(void)	{ return typeName; };

	friend class SimpleScrambler;
};

#pragma warning(default: 4251)	// try to limit the scope of this a little bit

#endif // !defined(AFX_SimpleUnScrambler_H__D2432425_15D4_11D2_9056_947982000000__INCLUDED_)
