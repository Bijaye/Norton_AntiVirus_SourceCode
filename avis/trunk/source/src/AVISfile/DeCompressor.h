//
//	IBM AntiVirus Immune System
//
//	File Name:	DeCompressor.h
//	Author:		Andy Klapper
//
//	This class defines the standard interface to all DeCompressor classes.
//
//	$Log: /AVISdb/ImmuneSystem/FilterTestHarness/DeCompressor.h $
//	 
//	1     8/19/98 9:10p Andytk
//	Initial check in
//

#if !defined(AFX_DECOMPRESSOR_H__D2432422_15D4_11D2_9056_947982000000__INCLUDED_)
#define AFX_DECOMPRESSOR_H__D2432422_15D4_11D2_9056_947982000000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <string>

#include "AVIS.h"
#include "AVISFile.h"


class AVISFILE_API DeCompressor  
{
protected:
	DeCompressor();

public:
	enum ErrorCode { NoError = 0, StreamError, DataError, MemoryError,
					 BufferingError, VersionError, SystemException,
					 UnknownError };

	virtual			~DeCompressor();

	virtual bool	Inflate(const uchar* in, uint& inSize,
									 uchar* out, uint& outSize) = 0;
	virtual bool	Done(uchar* out, uint& outSize) = 0;

	virtual ErrorCode Error() = 0;
	virtual const std::string&	Name(void)	= 0;
};

#endif // !defined(AFX_DECOMPRESSOR_H__D2432422_15D4_11D2_9056_947982000000__INCLUDED_)
