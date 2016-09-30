//
// IBM AntiVirus Immune System
//
// File Name: Encoder.h
// Author:    Milosz Muszynski
//
// This class represents an interface for 
// a variety of encoding tools available
//
// This class is intended for an external use
// it is a pure abstract class intended to be a base class
// for classes that provide real functionality
// 
// $Log: $
//

#ifndef _ENCODER_H_84990FF0_7A84_11d2_8A63_C87B5A000000
#define _ENCODER_H_84990FF0_7A84_11d2_8A63_C87B5A000000

#include "MOD_AVIS.h"

//
//
// class Encoder - for external use
//
//


class AVISFILE_API Encoder
{
protected:
	Encoder();

public:
	enum ErrorCode {	NoError, DataError, MemoryError, BufferingError, 
						VersionError, SystemException, UnknownError };

	virtual			            ~Encoder();

	virtual bool	            Encode( const uchar* in , uint  inSize,
							                  uchar* out, uint& outSize ) = 0;

	virtual bool	            Done  ( uchar* out, uint& outSize ) = 0;

	virtual const std::string&	Name(void) = 0;

	virtual ErrorCode	        Error() = 0;

	virtual uint                ExceptionCode() = 0;
};


#endif _ENCODER_H_84990FF0_7A84_11d2_8A63_C87B5A000000
