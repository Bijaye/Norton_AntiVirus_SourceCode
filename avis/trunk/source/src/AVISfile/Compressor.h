//
// IBM AntiVirus Immune System
//
// File Name: Compressor.h
// Author:    Milosz Muszynski
//
// This class represents an interface between AVIS compression 
// users and the ZLIB compression/decompression engine
// This class is intended for an external use
// it is a pure abstract class intended to be a base class
// for classes that provide real functionality
// 
// $Log: $
//

#ifndef _COMPRESSOR_H_
#define _COMPRESSOR_H_

#include <string>

#include "AVIS.h"
#include "AVISFile.h"

//
//
// class Compressor - for external use
//
//


class AVISFILE_API Compressor
{
protected:
	Compressor();

public:
	enum ErrorCode {	NoError, StreamError, DataError, MemoryError,
						BufferingError, VersionError, SystemException,
						UnknownError };

	virtual			~Compressor();

	virtual bool	Compress(const uchar* in, uint& inSize,
							 uchar* out, uint& outSize ) = 0;

	virtual bool	Done(uchar* out, uint& outSize ) = 0;

	virtual const std::string&	Name(void)	= 0;

	virtual ErrorCode	Error() = 0;
};


#endif // #ifndef _COMPRESSOR_H_
