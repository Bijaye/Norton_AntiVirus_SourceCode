//
//	IBM AntiVirus Immune System
//
//	File Name:	ToolFactory.cpp
//	Author:		Andy Klapper
//
//	The ToolFactory class takes a string and returns the tool associated
//	with that string.  It frees the calling class from having to know
//	about the number and type of tools that are available.  The calling
//	class just deals with the abstract interface to each kind of tool.
//
//	$Log: /AVISdb/ImmuneSystem/FilterTestHarness/ToolFactory.cpp $
//
//1     8/19/98 9:10p Andytk
//Initial check in
//

#include "stdafx.h"

#include "AVISFile.h"
#include <string.h>

#include "ToolFactory.h"

#include "SimpleScrambler.h"
#include "SimpleUnScrambler.h"
#include "Deflator.h"
#include "Inflator.h"
#include "Base64Encoder.h"
#include "Base64Decoder.h"
#include "MD5Calculator.h"
#include "NullDataManipulator.h"
#include "AVISFileException.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

ToolFactory::ToolFactory()
{

}

ToolFactory::~ToolFactory()
{

}

DataManipulator *ToolFactory::GetDataManipulator(const std::string& type)
{
	if (type == Deflator::TypeName())
	{
		return new Deflator;
	}
	else if (type == SimpleScrambler::TypeName())
	{
		return new SimpleScrambler;
	}
	else if (type == Base64Encoder::TypeName())
	{
		return new Base64Encoder;
	}
	else if (type == NullDataManipulator::TypeName())
	{
		return new NullDataManipulator;
	}
	else
		throw AVISFileException(AVISFileException::ToolFactory, "GetDataManipulator, unknown type");


	return 0;
}

DataManipulator *ToolFactory::GetDataUnManipulator(const std::string& type)
{
	if (type == Inflator::TypeName())
	{
		return new Inflator;
	}
	else if (type == SimpleUnScrambler::TypeName())
	{
		return new SimpleUnScrambler;
	}
	else if (type == Base64Decoder::TypeName())
	{
		return new Base64Decoder;
	}
	else if (type == NullDataManipulator::TypeName())
	{
		return new NullDataManipulator;
	} 
	else
		throw AVISFileException(AVISFileException::ToolFactory, "GetDataUnManipulator, unknown type");

	return 0;
}

CRCCalculator *ToolFactory::GetCRCCalculator(const std::string& type)
{
  if (type == MD5Calculator::TypeName())
    {
      return new MD5Calculator;
    }
  else
    {          
      std::string _msg("GetCRCCalculator, unknown type: type=[");
      _msg.append(type);
      _msg.append("] MD5Calculator::TypeName() = [");
      _msg.append(MD5Calculator::TypeName());
      throw AVISFileException(AVISFileException::ToolFactory, _msg);
    }
  
  return 0;
}


