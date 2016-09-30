//
// IBM AntiVirus Immune System
//
// File Name: Deflator.h
// Author:    Milosz Muszynski
//
// This class represents an interface between AVIS compression 
// users and the ZLIB compression/decompression engine
// This class is an internal implementation of the interface defined 
// by the Compressor class
// this class can be instantiated only by the class ToolFactory
//
// $Log: $
//

#ifndef _DEFLATOR_H_
#define _DEFLATOR_H_


//
//
// class Deflator - internal
//
//

#include <string>
#include <memory>

#include <AVIS.h>

#include "AVISFile.h"
#include "DataManipulator.h"
#include "ToolFactory.h"

#ifdef AVISFILE_EXPORTS
#include "ZlibTool.h"
#else
class ZlibTool;
#endif

#pragma warning( disable: 4251 ) // warns that ZlibTool needs to be exported


class AVISFILE_API Deflator : public DataManipulator  
{
protected:
	std::auto_ptr<ZlibTool>	_zlibTool;
	ErrorCode				_error;
	unsigned int			_systemExceptionCode;

public:
	Deflator();
	~Deflator();

private:
	static std::string	typeName;

public:
	bool		Process(const uchar* in, uint& inSize, uchar* out,
							uint& outSize);

	bool		Done(uchar* out, uint& outSize);

	ErrorCode	Error();

	uint		ExceptionCode();

			const std::string&	Name(void)		{ return typeName; };
	static	const std::string&	TypeName(void)	{ return typeName; };
};


#pragma warning( default: 4251 )

#endif _DEFLATOR_H_
