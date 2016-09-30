//
//	IBM AntiVirus Immune System
//
//	File Name:	DataManipulator.h
//	Author:		Andy Klapper
//
//	This class defines the standard data manipulator interface.
//
//	$Log:  $
//	 
//

#if !defined(AFX_DATAMANIPULATOR_H__798DFAA5_7DCD_11D2_ACE8_00A0C9C71BBC__INCLUDED_)
#define AFX_DATAMANIPULATOR_H__798DFAA5_7DCD_11D2_ACE8_00A0C9C71BBC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string>

#include "MOD_AVIS.h"
#include "MOD_AVISFile.h"

class AVISFILE_API DataManipulator  
{
public:
				//	All the error codes that any decendent of DataManipulator
				//	can generate.  I don't particularly like this, but I'm not sure
				//	how else to have generic DataManipulator objects return error
				//	codes without having to down-cast to find out what "32" means
				//	for a particular object.
	enum ErrorCode { NoError = 0,
					UnknownError,
					StreamError, DataError, MemoryError,
						BufferingError, VersionError, SystemException
					};

				//	Takes data in the inBuffer (of length inSize) and places the
				//	results into the outBuffer (up to outSize bytes).  inSize is
				//	changed to show the number of bytes that were not processed
				//	due to the size of the outBuffer.  (The data in inBuffer is
				//	shifted so the first byte of the unproccessed data starts at
				//	inBuffer[0]).
				//	More than one call to Process may be necessary to process the
				//	entire inBuffer and get the results back in the outBuffer.
				//	Returns true if successful (placed data into the outBuffer).

	virtual bool		Process(const uchar* inBuffer, uint& inSize,
								uchar* outBuffer, uint& outSize) = 0;
				//	Some processing algorithms take data in particular sizes and
				//	buffer the data until enougth data has been received.  This
				//	method tells the processing algorithm that no additional data
				//	is coming and to process what data is remaining in it's buffers.
				//	More than one call to Done may be necessary to process the entire
				//	stored buffer and get the results back into the outBuffer.
				//	Returns true if successful (placed data into the outBuffer).
	virtual bool		Done(uchar* outBuffer, uint& outSize) = 0;

				//	Since the return code from Process() and Done() only indicates if
				//	new data exists in the outBuffer a method is needed to indicate if
				//	no data was in the outBuffer because of an error or because the
				//	process algorithm just didn't have any data ready for the caller.
				//	This method returns NoError (evals to zero(0) or false) if no error
				//	has occured or another value if an error did occur.
	virtual ErrorCode	Error(void)	= 0;

				//	Returns the name of this algorithm.
	virtual const std::string& Name(void)	= 0;

	virtual ~DataManipulator();

protected:
	DataManipulator();
};

#endif // !defined(AFX_DATAMANIPULATOR_H__798DFAA5_7DCD_11D2_ACE8_00A0C9C71BBC__INCLUDED_)
