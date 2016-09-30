//
//	IBM AntiVirus Immune System
//
//	File Name:	MD5Calculator.h
//	Author:		Andy Klapper
//
//	This class implements the CRCCalculator interface using the MD5
//	algorithm.
//
//	$Log: /AVISdb/ImmuneSystem/FilterTestHarness/MD5Calculator.h $
//	
//	1     8/19/98 9:10p Andytk
//	Initial check in
//

#if !defined(AFX_MD5CALCULATOR_H__D2432427_15D4_11D2_9056_947982000000__INCLUDED_)
#define AFX_MD5CALCULATOR_H__D2432427_15D4_11D2_9056_947982000000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "CRCCalculator.h"

#include "ToolFactory.h"
#include "md5.h"

#pragma warning(disable: 4251)	// warns that std::string needs to be exported
								// to dll users (which would be true if it was
								// an internal class)

class AVISFILE_API MD5Calculator : public CRCCalculator  
{
public:
	MD5Calculator();
	~MD5Calculator();

private:
	MD5_CTX	context;

	static std::string	typeName;

public:
	bool	CRCCalculate(const uchar* buffer, uint bufferSize);
	bool	Done(std::string& crc);

			const std::string&	Name(void)		{ return typeName; };
	static	const std::string&	TypeName(void)	{ return typeName; };


	friend CRCCalculator*	ToolFactory::GetCRCCalculator(const std::string&);
};

#pragma warning(default: 4251)	// try to limit the scope of this a little bit

#endif // !defined(AFX_MD5CALCULATOR_H__D2432427_15D4_11D2_9056_947982000000__INCLUDED_)
