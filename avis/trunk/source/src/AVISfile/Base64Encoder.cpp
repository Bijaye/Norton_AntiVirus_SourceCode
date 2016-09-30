//
// IBM AntiVirus Immune System
//
// File Name: Base64Encoder.cpp
// Author:    Milosz Muszynski
//
// This class implements a Base64 encoding
// 
// This class is an internal implementation of the interface defined 
// by the Encoder class
//
// This implementation is system exception safe. It will intercept the
// system exception handler and return a SystemException error in case
// system exception occurred
//
// Usually base64 encoding routines work with buffers that contain either
// all the data at one time or data in chunks of size that is a multiple
// of some constants (except for the last chunk of data). 
// This implementation works with buffers of any size and uses its own 
// small internal buffer to hide buffer size limitations from the user
//
// $Log: $
//

// ------ includes -------------------------------------------------------
#include <stdafx.h>
#include <string>
#include <memory>

#include <AVIS.h>
#include "AVISFile.h"
#include "DataManipulator.h"
#include "ToolFactory.h"
#include "Base64Encoder.h"
#include "ZExcept.h"


// ------ constructors/destructors ---------------------------------------

std::string	Base64Encoder::typeName( "base64encode" );

Base64Encoder::Base64Encoder()
: DataManipulator(), _error( NoError ), _bufLen( 0 ), _systemExceptionCode( 0 )
{
}


Base64Encoder::~Base64Encoder()
{
}


// ------ public methods -------------------------------------------------

bool	Base64Encoder::Process( const uchar* in , uint& inSize,
			      		             uchar* out, uint& outSize )
{
	ZExceptionTranslator exceptionTranslator;
	exceptionTranslator.init();

	bool success = false;
	try
	{
		success = encode( in, inSize, out, outSize ); 
	}
	catch( ZException& exc )
	{
		success = false;
		outSize = 0;
		_error  = SystemException;
		_systemExceptionCode = exc.code();
	}

	inSize = 0;

	return success;
}


bool	Base64Encoder::Done( uchar* out, uint& outSize )
{
	ZExceptionTranslator exceptionTranslator;
	exceptionTranslator.init();

	bool success = false;
	try
	{
		success = done( out, outSize );
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


DataManipulator::ErrorCode Base64Encoder::Error()
{
	return _error;
}


uint Base64Encoder::ExceptionCode()
{
	return _systemExceptionCode;
}


// ------ protected methods ----------------------------------------------


//
// encode
//

bool	Base64Encoder::encode( const uchar* in , uint inSize,
			      		             uchar* out, uint& outSize )
{
	_currentIn  = 0;
	_currentOut = 0;
	_outSize    = outSize;

	_total = _bufLen + inSize - ( ( _bufLen + inSize ) % DataInUnitSize ); // rounded off to unit size

	_in = in;
	_out = out;

	if ( _total == 0 )
	{
		outSize = 0;
		memcpy( _buffer + _bufLen, _in, inSize ); // accumulate the new data for the next time
	    _bufLen += inSize;
	}
	else
	{
		outSize = encode();
		memcpy( _buffer, _in + _total - _bufLen, inSize + _bufLen - _total ); // move the leftover to the buffer for the next time
	    _bufLen = inSize + _bufLen - _total;
	}

	return _error == NoError;
}


//
// done
//

bool	Base64Encoder::done( uchar* out, uint& outSize )
{
	_currentIn  = 0;
	_currentOut = 0;
	_outSize    = outSize;

	_total = _bufLen;

	_out = out;

	outSize = encode();

	_bufLen = 0;

	return _error == NoError;
}


//
// getChar
//

int Base64Encoder::getChar()
{
	int c;

	if ( _currentIn < _bufLen )
	{
		c = (int)(unsigned char)_buffer[ _currentIn ];
		_currentIn++;
	}
	else if ( _currentIn < _total )
	{
		c = (int)(unsigned char)_in[ _currentIn - _bufLen ];
		_currentIn++;
	}
	else
	{
		c = -1;
	}

	return c;
}


//
// putChar
//

void Base64Encoder::putChar( char c )
{
	if ( _error == NoError )
	{
		if ( _currentOut < _outSize )
		{
			_out[ _currentOut++ ] = c;
		}
		else
		{
			_error = BufferingError;
		}
	}
}


//
// encode
//

long Base64Encoder::encode()
{
	long n = 0;
	static char alphabet[65] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

	int cols = 0;
	int bits = 0;
	int c;
	int charCount = 0;

	while ( ( c = getChar() ) != -1 )
	{
		bits += c;
		charCount++;

		if ( charCount == 3 )
		{
			n++; putChar( alphabet[ (bits >> 18) & 0x3f ] );
			n++; putChar( alphabet[ (bits >> 12) & 0x3f ] );
			n++; putChar( alphabet[ (bits >>  6) & 0x3f ] );
			n++; putChar( alphabet[  bits & 0x3f ] );

			cols += 4;
			if ( cols == 72 )
			{
				n++; putChar( '\r' );
				n++; putChar( '\n' );
				cols = 0;
			}
			bits = 0;
			charCount = 0;
		}
		else
		{
			bits <<= 8;
		}
	}
	if ( charCount != 0 )
	{
		bits <<= 16 - (8 * charCount );
		n++; putChar( alphabet[ (bits >> 18) & 0x3f ] );
		n++; putChar( alphabet[ (bits >> 12) & 0x3f ] );
		if ( charCount == 1 )
		{
			n++; putChar( '=' );
			n++; putChar( '=' );
		}
		else
		{
			n++; putChar( alphabet[ (bits >>  6) & 0x3f ] );
			n++; putChar( '=' );
		}
		if ( cols > 0 )
		{
			n++; putChar( '\r' );
			n++; putChar( '\n' );
		}
	}
	return n;
}

