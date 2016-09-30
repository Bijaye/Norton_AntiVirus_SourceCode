//
// IBM AntiVirus Immune System
//
// File Name: Inflator.cpp
// Author:    Milosz Muszynski
//
// This class represents an interface between AVIS decompression 
// users and the ZLIB compression/decompression engine
// This class is an internal implementation of the interface defined 
// by the UnCompressor class
// This class uses methods of the class ZlibTool to do real work
// an instance of the class ZlibTool is contained within this class
//
// $Log: $
//


// ------ includes -------------------------------------------------------
#include "stdafx.h"

#include "AVISFile.h"
#include "Inflator.h"
#include "ZExcept.h"

// ------ private constructors/destructors -------------------------------

std::string	Inflator::typeName("deflate");

Inflator::Inflator()
: DataManipulator(), _zlibTool(new ZlibTool()), _error( NoError ), _systemExceptionCode( 0 )
{
}


Inflator::~Inflator()
{
}


// ------ public methods -------------------------------------------------

bool	Inflator::Process(const uchar* in , uint& inSize,
			        			uchar* out, uint& outSize)
{
	ZExceptionTranslator exceptionTranslator;
	exceptionTranslator.init();

	bool success = false;
	try
	{
		success = _zlibTool->Inflate( in, inSize, out, outSize ); 
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


bool	Inflator::Done(uchar* out, uint& outSize)
{
	ZExceptionTranslator exceptionTranslator;
	exceptionTranslator.init();

	bool success = false;
	try
	{
		success = _zlibTool->InflateDone( out, outSize );
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


Inflator::ErrorCode Inflator::Error()
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


uint Inflator::ExceptionCode()
{
	return _systemExceptionCode;
}

