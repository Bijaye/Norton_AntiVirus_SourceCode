// ompoundDataManipulator.cpp: implementation of the CompoundDataManipulator class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"


#include "CompoundDataManipulator.h"
#include "NullDataManipulator.h"

#include "AVISFileException.h"

const std::string	CompoundDataManipulator::typeName("CompoundDataManipulator");


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CompoundDataManipulator::CompoundDataManipulator() : DataManipulator(),
	length(0), inProcess(false)
{
	static	NullDataManipulator	nullDataManipulator;

	states[0]	= new ManipulatorState(nullDataManipulator, 0, 0, NULL);
	length		= 1;
}

CompoundDataManipulator::~CompoundDataManipulator()
{
	for (int i = 0; i < length; i++)
	{
		if (!(0 == i || i == length-1))
//		if (i != length-1)
			delete [] states[i]->buffer;
		delete states[i];
	}
}

bool CompoundDataManipulator::Process(const uchar* inBuffer, uint& inSize,
									  uchar* outBuffer, uint& outSize)
{
	if (length < 0)
		throw AVISFileException(AVISFileException::CompoundDataManip, "Process, length < 0");

	inProcess	= true;

	if (0 == length)
	{
		if (outBuffer < inBuffer)
			throw AVISFileException(AVISFileException::CompoundDataManip, "Process, 0 == length, outBuffer < inBuffer");
	
		memcpy(outBuffer, inBuffer, inSize);
		outSize = inSize;

		return true;
	}
	else if (1 == length)
	{
		return states[0]->manipulator.Process(inBuffer, inSize, outBuffer, outSize);
	}

	states[0]->buffer	= (uchar*) inBuffer;
	states[0]->inSize	= inSize;

	states[length-1]->inSize	= 0;
	states[length-1]->maxSize	= outSize;
	states[length-1]->buffer	= outBuffer;

	RProcess(0);

	inSize	= states[0]->inSize;
	outSize = states[length-1]->inSize;

	return outSize > 0 && NoError == Error();
}

bool CompoundDataManipulator::RProcess(uint i)
{
	if (i >= length -1)
		throw AVISFileException(AVISFileException::CompoundDataManip, "RProcess, i >= length -1");

	ManipulatorState	*inState	= states[i];
	ManipulatorState	*outState	= states[i+1];
	ManipulatorState	*endState	= states[length-1];
	uint				oldEndOutSize=endState->inSize;

	if (i == length -2)	// is the current outState the endState?
	{
		uint	outSize =	outState->maxSize - outState->inSize;

		while (inState->manipulator.Process(inState->buffer, inState->inSize,
											&outState->buffer[outState->inSize],
											outSize= outState->maxSize - outState->inSize)
				&& 0 != outSize)
		{
			outState->inSize+= outSize;
		}
	}
	else
	{
		if (outState->inSize > 0)		// we have old data in the pipe
			while (RProcess(i+1));				// push it through

		if (endState->inSize == endState->maxSize)	// are we already full ?
			return false;

		while (inState->manipulator.Process(inState->buffer, inState->inSize,
											outState->buffer,
											outState->inSize = outState->maxSize)
				&& 0 != outState->inSize)
		{
			while (RProcess(i+1));
		}
	}

	return oldEndOutSize != endState->inSize;
}

bool CompoundDataManipulator::Done(uchar* outBuffer, uint& outSize)
{
	if (length < 0)
		throw AVISFileException(AVISFileException::CompoundDataManip, "Done, length < 0");
	else if (1 == length)
		return states[0]->manipulator.Done(outBuffer, outSize);

	inProcess	= true;

	if (0 == length)
		return true;

	states[0]->buffer	= NULL;
	states[0]->inSize	= 0;

	states[length-1]->inSize	= 0;
	states[length-1]->maxSize	= outSize;
	states[length-1]->buffer	= outBuffer;

	RDone(0);

	outSize = states[length-1]->inSize;

	return outSize > 0 && NoError == Error();
}

bool CompoundDataManipulator::RDone(uint i)
{
	bool	rc = false;

	if (i >= length -1)
		throw AVISFileException(AVISFileException::CompoundDataManip, "RDone, i >= length -1");

	ManipulatorState	*inState	= states[i];
	ManipulatorState	*outState	= states[i+1];
	ManipulatorState	*endState	= states[length-1];
	uint				oldEndOutSize= endState->inSize;

	if (i == length-2)		// is this the current outState the endState?
	{
		uint	outSize = outState->maxSize - outState->inSize;

		while (inState->manipulator.Done(&outState->buffer[outState->inSize], outSize) &&
				0 != outSize)
		{
			outState->inSize	+= outSize;
			outSize				= outState->maxSize - outState->inSize;
		}
	}
	else
	{
		if (outState->inSize > 0)		// we have old data in the pipe
			while (RProcess(i+1));		// push it through

		if (endState->inSize == endState->maxSize)	// are we already full
			return false;

		while (inState->manipulator.Done(&outState->buffer[outState->inSize],
										outState->inSize = outState->maxSize)
				&& 0 != outState->inSize)
		{
			while (RProcess(i+1));
		}
		
		RDone(i+1);
	}

	inProcess = rc;			// if nothing else to do we are done processing.

	return oldEndOutSize == outState->inSize && NoError == Error();
}

DataManipulator::ErrorCode CompoundDataManipulator::Error()
{
	for (int i = 0; i < length; i++)
	{
		if (DataManipulator::NoError != states[i]->manipulator.Error())
			return states[i]->manipulator.Error();
	}

	return DataManipulator::NoError;
}

bool	CompoundDataManipulator::Add(DataManipulator& dataManipulator)
{
	if (inProcess || length >= 9)
		return false;

	states[length] = states[length-1];

	uchar	*buffer	= NULL;
	if (0 != length-1)
		buffer	= new uchar[BufferSize];

	states[length-1] = new ManipulatorState(dataManipulator, BufferSize, 0,
											buffer);
	++length;

	return true;
}
