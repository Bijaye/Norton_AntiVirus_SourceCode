//
// IBM AntiVirus Immune System
//
// File Name: Deflator.cpp
// Author:    Milosz Muszynski
//
// This class represents an interface between AVIS compression 
// users and the ZLIB compression/decompression engine
// This class is an internal implementation of the interface defined 
// by the Compressor class
// This class uses methods of the class ZlibTool to do real work
// an instance of the class ZlibTool is contained within this class
//
// $Log: $
//

// ------ defines --------------------------------------------------------


// ------ includes -------------------------------------------------------
#include "stdafx.h"

#include "Deflator.h"
#include "ZExcept.h"


// ------ private constructors/destructors -------------------------------

std::string	Deflator::typeName("deflate");

Deflator::Deflator()
: DataManipulator(), _zlibTool(new ZlibTool()), _error( NoError ), _systemExceptionCode( 0 )
{
}


Deflator::~Deflator()
{
}


// ------ public methods -------------------------------------------------

bool	Deflator::Process( const uchar* in , uint& inSize,
			      		     uchar* out, uint& outSize )
{
	ZExceptionTranslator exceptionTranslator;
	exceptionTranslator.init();

	bool success = false;
	try
	{
		success = _zlibTool->Deflate( in, inSize, out, outSize ); 
	}
	catch( ZException& exc )
	{
		success = false;
		outSize = 0;
		_error  = SystemException;
		_systemExceptionCode = exc.code();
	}

	return success;
}


bool	Deflator::Done( uchar* out, uint& outSize )
{
	ZExceptionTranslator exceptionTranslator;
	exceptionTranslator.init();

	bool success = false;
	try
	{
		success = _zlibTool->DeflateDone( out, outSize );
	}
	catch( ZException& exc )
	{
		success = false;
		outSize = 0;
		_error  = SystemException;
		_systemExceptionCode = exc.code();
	}

	return success;
}


Deflator::ErrorCode Deflator::Error()
{
	int        e = _zlibTool->Error();

	ErrorCode ec = UnknownError;

	switch ( e )
	{
	case Z_OK:            ec = NoError;         break;
	case Z_STREAM_ERROR:  ec = StreamError;		break;
	case Z_DATA_ERROR:    ec = DataError;		break;
	case Z_MEM_ERROR:     ec = MemoryError;		break;
	case Z_BUF_ERROR:     ec = BufferingError;	break;
	case Z_VERSION_ERROR: ec = VersionError;    break;
	case Z_STREAM_END:    ec = NoError;         break;
	default:									break;
	}

	if ( ec == NoError )
		ec = _error;

	return ec;
}


uint Deflator::ExceptionCode()
{
	return _systemExceptionCode;
}
