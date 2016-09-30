//
// IBM AntiVirus Immune System
//
// File Name: Base64Encoder.h
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


#ifndef _BASE64ENCODER_H_23DA6571_7A87_11d2_8A63_C87B5A000000
#define _BASE64ENCODER_H_23DA6571_7A87_11d2_8A63_C87B5A000000


//
//
// class Base64Encoder - internal
//
//

class AVISFILE_API Base64Encoder : public DataManipulator
{
protected:
	ErrorCode				_error;
	unsigned int			_systemExceptionCode;

public:
	Base64Encoder();
	~Base64Encoder();

private:
	static std::string	typeName;

public:
	bool		Process( const uchar* in, uint& inSize, uchar* out, uint& outSize );
	bool		Done  ( uchar* out, uint& outSize );

	ErrorCode	Error();

	uint		ExceptionCode();

				const std::string&	Name(void)		{ return typeName; };
	static		const std::string&	TypeName(void)	{ return typeName; };

protected:
	enum		{	DataInUnitSize  = 54	};

	bool		encode( const uchar* in, uint inSize, uchar* out, uint& outSize );
	bool		done  ( uchar* out, uint& outSize );

	long		encode  ();
	int			getChar ();
	void		putChar ( char c );

	uchar		_buffer[ DataInUnitSize ];
	uint        _bufLen;
	uint        _total;
	uint		_currentIn;
	uint		_currentOut;
	uint		_outSize;

	const uchar * _in;
	uchar *       _out;
};


#endif _BASE64ENCODER_H_23DA6571_7A87_11d2_8A63_C87B5A000000
