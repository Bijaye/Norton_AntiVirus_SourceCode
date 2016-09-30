//
// IBM AntiVirus Immune System
//
// File Name: ZlibTool.cpp
// Author:    Milosz Muszynski
//
// This class represents a connection between AVIS compression and
// decompression users and the ZLIB compression engine
// This class provides an easier to use unterface for ZLIB functions
// 
// The idea for this class should be credited to Mark Nelson and
// his ZlibEngine class
//
// $Log: $
//


// ------ defines --------------------------------------------------------



// ------ includes -------------------------------------------------------


#include "stdafx.h"
#include "ZlibTool.h"



// ------ protected methods ----------------------------------------------

void ZlibTool::Init()
{
	memset( (z_stream *)this, sizeof( z_stream ), 0 );

	zalloc  = 0;
	zfree   = 0;
	opaque  = 0;

	avail_in = 0;

	_first  = true;
}


// ------ constructors/destructors ---------------------------------------

// constructor
ZlibTool::ZlibTool() : _err( Z_OK )
{
	Init();

	_done = false;
}


// destructor
ZlibTool::~ZlibTool()
{
	
	// check if we need to clean up
	if ( !_first && !_done )
	{ // if

			_err = inflateEnd( this );

			_first = false;
			_done  = false;

	} // end if 
	
}


// ------ public methods -------------------------------------------------

bool ZlibTool::Inflate( const uchar* in , uint& inSize,
			        		  uchar* out, uint& outSize )
{
	if ( _err == Z_STREAM_END )
		return false;

	if ( inSize != 0 )
	{
		if ( outSize != 0 )
		{
			next_out  = out;
			avail_out = outSize;
		}

		if ( _first )
		{
			_err = inflateInit( this );
			_first = false;
		}

		if ( _err == Z_OK )
		{
			if ( avail_in == 0 )
			{
				next_in  = (uchar*)in;
				avail_in = inSize;
			}

			_err = inflate( this, Z_NO_FLUSH );

			outSize -= avail_out;
			if ( avail_in == 0 )
				inSize = 0;
		}
	}
	else
	{
		_err = Z_OK;
		outSize = 0;
		inSize  = 0;
	}

	bool rc = _err == Z_OK || _err == Z_STREAM_END;
	if ( rc == false )
	{
		Init();
	}
	return rc;
}


bool ZlibTool::InflateDone( uchar* out, uint& outSize )
{

	if ( _err == Z_STREAM_END )
	{ // if

		inflateEnd( this );

		Init();

		_err = Z_OK;

		_done = true;

		return false;

	} // end if 

	next_out  = out;
	avail_out = outSize;

	_err = inflate( this, Z_FINISH );

	outSize	-= avail_out;

	bool rc = _err == Z_OK || _err == Z_STREAM_END;

	if ( rc == false )
	{ // if

		inflateEnd( this );

		Init();

		_err = Z_OK;

		_done = true;

	} // end if

	return rc;
}


bool ZlibTool::Deflate( const uchar* in , uint& inSize,
			      	  	      uchar* out, uint& outSize, int level )
{
	if ( _err == Z_STREAM_END )
		return false;

	if ( inSize != 0 )
	{
		if ( outSize != 0 )
		{
			next_out  = out;
			avail_out = outSize;
		}

		if ( _first )
		{
			_err = deflateInit( this, level );
			_first = false;
		}

		if ( _err == Z_OK )
		{
			if ( avail_in == 0 )
			{
				next_in  = (uchar*)in;
				avail_in = inSize;
			}

			_err = deflate( this, Z_NO_FLUSH );

			outSize -= avail_out;
			if ( avail_in == 0 )
				inSize = 0;
		}
	}
	else
	{
		_err = Z_OK;
		outSize = 0;
		inSize = 0;
	}

	bool rc = _err == Z_OK || _err == Z_STREAM_END;
	if ( rc == false )
	{
		Init();
	}
	return rc;
}


bool ZlibTool::DeflateDone( uchar* out, uint& outSize )
{
	if ( _err == Z_STREAM_END )
	{
		deflateEnd( this );
		Init();
		_err = Z_OK;
		return false;
	}

	next_out  = out;
	avail_out = outSize;

	_err = deflate( this, Z_FINISH );

	outSize	-= avail_out;

	bool rc = _err == Z_OK || _err == Z_STREAM_END;
	if ( rc == false )
	{
		deflateEnd( this );
		Init();
		_err = Z_OK;
	}
	return rc;
}




