//
//	IBM AntiVirus Immune System
//
//	File Name:	Decoder.h
//	Author:		Andy Klapper
//
//	This class defines the standard Decoder interface.
//
//	$Log: /AVISdb/ImmuneSystem/FilterTestHarness/Decoder.h $
//	 
//	1     8/19/98 9:10p Andytk
//	Initial check in
//

#if !defined(AFX_DECODER_H__D2432428_15D4_11D2_9056_947982000000__INCLUDED_)
#define AFX_DECODER_H__D2432428_15D4_11D2_9056_947982000000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "MOD_AVIS.h"

class AVISFILE_API Decoder
{
protected:
	Decoder();

public:
	enum ErrorCode {	NoError, DataError, MemoryError, BufferingError, 
						VersionError, SystemException, UnknownError };

	virtual			            ~Decoder();

	virtual bool	            Decode( const uchar* in , uint  inSize,
							                  uchar* out, uint& outSize ) = 0;

	virtual bool	            Done  ( uchar* out, uint& outSize ) = 0;

	virtual const std::string&	Name(void) = 0;

	virtual ErrorCode	        Error() = 0;

	virtual uint                ExceptionCode() = 0;
};

#endif // !defined(AFX_DECODER_H__D2432428_15D4_11D2_9056_947982000000__INCLUDED_)





