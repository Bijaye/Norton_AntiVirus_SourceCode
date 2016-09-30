// SampleStream.h: interface for the SampleStream class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SAMPLESTREAM_H__8DF42B21_03F0_11D3_ADD7_00A0C9C71BBC__INCLUDED_)
#define AFX_SAMPLESTREAM_H__8DF42B21_03F0_11D3_ADD7_00A0C9C71BBC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <MappedMemory.h>
#include <CompoundDataManipulator.h>
#include <md5\MD5Calculator.h>
#include <xor\SimpleScrambler.h>
#include <deflate\Deflator.h>

class MyTraceClient; //#include <MyTraceClient.h>
#include "IcePackAgentComm.h"
#include "Sample.h"

class SampleStream : public IcePackAgentComm::Stream  
{
public:
	SampleStream(Sample& samp);

	virtual ~SampleStream();

	//
	//	Stream methods
	bool init(ulong& errorCode) {	readPos = 0;
									errorCode = IcePackAgentComm::NoError;
									return true;
								}

	bool read(uchar* inBuf, ulong& 	inSize, ulong& 	errorCode )
								{
									if (readPos + inSize > contEnd)
										inSize = contEnd - readPos;
									if (inSize > 0)
									{
										memcpy(inBuf, &contents[readPos], inSize);
										readPos	+= inSize;
									}

									errorCode = IcePackAgentComm::NoError;

									return inSize > 0;
								}

	bool end(ulong& errorCode)	{	errorCode = IcePackAgentComm::NoError;
									return contEnd <= readPos; }

	char *start()					{ return contents; }

	long size()					{ return contEnd; }

	uint SampleSize()			{ return sampleSize; }
	bool Stripped()				{ return stripped; }
	bool Compressed()			{ return compressed; }
	bool Scrambled()			{ return scrambled; }

	std::string	SampleMD5(void)	{ return md5SampleResult; }

private:
	Sample&							sample;
	bool							stripped;
	bool							compressed;
	bool							scrambled;
	uint							sampleSize;
	uint							contSize;
	uint							contEnd;
	uint							readPos;
	char*							contents;
	std::string						md5SampleResult;
	MD5Calculator					md5Sample;
	MD5Calculator					md5Content;
	static uint Size(Sample& sample);

	bool	Strip();

public:
	enum	{ BufferSize = 4096 };
};

#endif // !defined(AFX_SAMPLESTREAM_H__8DF42B21_03F0_11D3_ADD7_00A0C9C71BBC__INCLUDED_)
