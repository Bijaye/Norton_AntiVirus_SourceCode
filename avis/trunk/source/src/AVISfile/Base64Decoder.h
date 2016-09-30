//
// IBM AntiVirus Immune System
//
// File Name: Base64Decoder.h
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


#ifndef _BASE64DECODER_H_23DA6570_7A87_11d2_8A63_C87B5A000000
#define _BASE64DECODER_H_23DA6570_7A87_11d2_8A63_C87B5A000000


//
//
// class Base64Decoder - internal
//
//

class AVISFILE_API Base64Decoder : public DataManipulator
{
protected:
	ErrorCode				_error;
	unsigned int			_systemExceptionCode;

public:
	Base64Decoder();
	~Base64Decoder();

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
	enum        {	DataInUnitSize  = 72   };

	bool		decode( const uchar* in, uint inSize, uchar* out, uint& outSize );
	bool		done  ( uchar* out, uint& outSize );

	long		decode  ();
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


#endif _BASE64DECODER_H_23DA6570_7A87_11d2_8A63_C87B5A000000
