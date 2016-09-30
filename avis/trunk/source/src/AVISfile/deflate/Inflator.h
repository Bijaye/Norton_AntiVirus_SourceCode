//
// IBM AntiVirus Immune System
//
// File Name: Inflator.h
// Author:    Milosz Muszynski
//
// This class represents an interface between AVIS decompression 
// users and the ZLIB compression/decompression engine
// This class is an internal implementation of the interface defined 
// by the UnCompressor class
// this class can be instantiated only by the class ToolFactory
//
// $Log: $
//

#ifndef _INFLATOR_H_
#define _INFLATOR_H_

#include <memory>
#include <string>

#include "AVISFile.h"
#include "DataManipulator.h"
#include "ToolFactory.h"

//
//
// class Inflator- internal
//
//

#pragma warning( disable: 4251 ) // warns that ZlibTool needs to be exported

#ifdef AVISFILE_EXPORTS
#include "Zlib.h"
#include "ZlibTool.h"
#else
class ZlibTool;
#endif

class AVISFILE_API Inflator : public DataManipulator  
{
protected:
	std::auto_ptr<ZlibTool>	_zlibTool;

public:
	Inflator();
	~Inflator();

private:
	static	std::string	typeName;
			ErrorCode	_error;
			uint		_systemExceptionCode;

public:
	bool		Process(const uchar* in, uint& inSize,
						uchar* out, uint& outSize);

	bool		Done(uchar* out, uint& outSize);

	ErrorCode	Error();

	uint		ExceptionCode();

			const std::string&	Name(void)		{ return typeName; };
	static	const std::string&	TypeName(void)	{ return typeName; };
};


#pragma warning( default: 4251 )

#endif _UNZIP_H_
