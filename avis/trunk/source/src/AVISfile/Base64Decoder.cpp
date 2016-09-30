//
// IBM AntiVirus Immune System
//
// File Name: Base64Decoder.cpp
// Author:    Milosz Muszynski
//
// This class implements a Base64 decoding
// 
// This class is an internal implementation of the interface defined 
// by the Decoder class
//
// This implementation is system exception safe. It will intercept the
// system exception handler and return a SystemException error in case
// system exception occurred
//
// Usually base64 decoding routines work with buffers that contain either
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
#include "Base64Decoder.h"
#include "ZExcept.h"


// ------ constructors/destructors ---------------------------------------

std::string	Base64Decoder::typeName( "base64decode" );

Base64Decoder::Base64Decoder()
: DataManipulator(), _error( NoError ), _bufLen( 0 ), _systemExceptionCode( 0 )
{
}


Base64Decoder::~Base64Decoder()
{
}


// ------ public methods -------------------------------------------------

bool	Base64Decoder::Process( const uchar* in , uint& inSize,
			      		             uchar* out, uint& outSize )
{
	ZExceptionTranslator exceptionTranslator;
	exceptionTranslator.init();

	bool success = false;
	try
	{
		success = decode( in, inSize, out, outSize ); 
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


bool	Base64Decoder::Done( uchar* out, uint& outSize )
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


DataManipulator::ErrorCode Base64Decoder::Error()
{
	return _error;
}


uint Base64Decoder::ExceptionCode()
{
	return _systemExceptionCode;
}


// ------ protected methods ----------------------------------------------


//
// decode
//

bool	Base64Decoder::decode( const uchar* in , uint inSize,
			      		             uchar* out, uint& outSize )
{
	_currentIn  = 0;
	_currentOut = 0;
	_outSize    = outSize;

	int offset;

	for ( offset = 0; offset < DataInUnitSize && offset < inSize; offset++ )
	{
		uchar c = in[ inSize - 1 - offset ];
		if ( c == '\r' || c == '\n' )
			break;
	}

	offset %= DataInUnitSize;
	if ( offset >= inSize )
		offset = 0;

	_total = _bufLen + inSize - offset; // skip the last line if shorter than DataInUnitSize

	_in = in;
	_out = out;

	if ( _total < DataInUnitSize )
	{
		outSize = 0;
		memcpy( _buffer + _bufLen, _in, inSize ); // accumulate the new data for the next time
	    _bufLen += inSize;
	}
	else
	{
		outSize = decode();
		memcpy( _buffer, _in + _total - _bufLen, inSize + _bufLen - _total ); // move the leftover to the buffer for the next time
	    _bufLen = inSize + _bufLen - _total;
	}

	return _error == NoError;
}


//
// done
//

bool	Base64Decoder::done( uchar* out, uint& outSize )
{
	_currentIn  = 0;
	_currentOut = 0;
	_outSize    = outSize;

	_total = _bufLen;

	_out = out;

	outSize = decode();

	_bufLen = 0;

	return _error == NoError;
}


//
// getChar
//

int Base64Decoder::getChar()
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

void Base64Decoder::putChar( char c )
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
// decode
//

long Base64Decoder::decode()
{
	// based on the public domain code from http://www.ece.cmu.edu/afs/ece.cmu.edu/support/base64/src/
	static char alphabet[65] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    static char inAlphabet[256];
	static char decoder[256];

	long   n = 0;
    int    bits;
	int    charCount;
	char   c;
	int    ic;

    for (int i = (sizeof alphabet) - 2; i >= 0 ; i--) 
	{
	    inAlphabet[ alphabet[i] ] = 1;
	    decoder   [ alphabet[i] ] = i;
    }

    charCount = 0;
    bits = 0;
    while ( ( ic = getChar() ) != -1 )
	{
		c = (char)ic;
	    if ( c == '=' )
	        break;
	    if ( c > 255 || !inAlphabet[c] )
	        continue;
	    bits += decoder[c];
	    charCount++;
	    if (charCount == 4)
	    {
	        n++; putChar( ( bits >> 16 ) & 0xff );
	        n++; putChar( ( bits >> 8 ) & 0xff );
	        n++; putChar( ( bits & 0xff ) );
	        bits = 0;
	        charCount = 0;
		} 
		else 
		{
	        bits <<= 6;
	    }
    }
    if ( ic == -1 ) 
	{
	    if (charCount) 
		{
			//failed - base64 encoding incomplete
			_error = DataError;
		}
    } 
	else /* c == '=' */
	{ 
	    switch (charCount) 
		{
			case 1:
				{
                    //failed - base64 encoding incomplete: at least 2 bits missing
					_error = DataError;
				}
				break;
			case 2:
				n++; putChar( ( bits >> 10 ) & 0xff );
				break;
			case 3:
				n++; putChar( ( bits >> 16 ) & 0xff );
				n++; putChar( (bits >> 8) & 0xff );
			break;
	    }
    }
	return _error == NoError ? n : 0;
}

