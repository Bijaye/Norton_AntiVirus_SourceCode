//
//	IBM AntiVirus Immune System
//
//	File Name:	ToolFactory.h
//	Author:		Andy Klapper
//
//	The ToolFactory class chooses a tool based on a string that is passed
//	to the ToolFactory.  The clients of tool factories do not have to
//	know how may different types of tools are available or how they are
//	selected.  The client always works with a standard interface for
//	each class of tool.
//
//	$Log: /AVISdb/ImmuneSystem/FilterTestHarness/ToolFactory.h $
//	
//	1     8/19/98 9:10p Andytk
//	Initial check in
//

#if !defined(AFX_TOOLFACTORY_H__D2432429_15D4_11D2_9056_947982000000__INCLUDED_)
#define AFX_TOOLFACTORY_H__D2432429_15D4_11D2_9056_947982000000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "MOD_AVIS.h"
#include "MOD_DataManipulator.h"

//#include "Compressor.h"
//#include "DeCompressor.h"
//#include "Scrambler.h"
//#include "UnScrambler.h"
#include "MOD_Encoder.h"
#include "MOD_Decoder.h"
#include "MOD_CRCCalculator.h"

class AVISFILE_API ToolFactory  
{
private:
	ToolFactory();
	virtual ~ToolFactory();

public:
	static DataManipulator	*GetDataManipulator(const std::string& type);
	static DataManipulator	*GetDataUnManipulator(const std::string& type);

	static CRCCalculator	*GetCRCCalculator(const std::string& type);
};

#endif // !defined(AFX_TOOLFACTORY_H__D2432429_15D4_11D2_9056_947982000000__INCLUDED_)
