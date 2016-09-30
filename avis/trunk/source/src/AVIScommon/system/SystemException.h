//
//	IBM AntiVirus Immune System
//
//	File Name:	SystemException.h
//
//	Author:		Andy Klapper
//
//	Objects of type SystemException are thrown when "system" or "C" level
//	problems are encountered (like divide by zero or memory access violations)
//
//	This code was inspired by information that Milosz provided.
//	(specifically the existance of a function that allows system level
//	exceptions to be translated and rethrown.
//
//	$Log:  $
//	
//
//	IBM/Symantec Confidential
//

#ifndef _SYSTEMEXCEPTION_H_
#define _SYSTEMEXCEPTION_H_

#include "AVISException.h"
#include <string>

#pragma	warning( disable: 4251 )	// warns that std::string needs to be exported
									// to dll users (which would be true if it was
									// an internal class)

class AVISCOMMON_API SystemException : public AVISException
{
public:
	enum TypeOfException	{
		AccessViolation,
		ArrayBoundsExceeded,
		ExceptionBreakPoint,
		DataTypeMisalignment,
		FloatingPointDenormalOperand,
		FloatingPointDivideByZero,
		FloatingPointInexactResult,
		FloatingPointInvalidOperation,
		FloatingPointOverflow,
		FloatingPointStackCheck,
		FloatingPointUnderflow,
		IllegalInstruction,
		InPageError,
		IntDivideByZero,
		IntOverflow,
		InvalidDisposition,
		NonContinuableException,
		PrivInstruction,
		SingleStep,
		StackOverflow,
		DebugControlC,

		Unknown
							};

	SystemException(TypeOfException t);

	~SystemException() {};

	TypeOfException	Type(void)		{ return type; };
	std::string		ClassAsString()	{ return "SystemException"; };
	std::string		DetailedInfo()	{ return ""; };
	std::string		TypeAsString(void) { return std::string(typeAsChar[type]); };

	static void Init(void);			// call this method to initialize the translation,
									// this must be called once per thread.

private:
	TypeOfException	type;

	static char		*typeAsChar[];
};

#pragma warning( default: 4251 )	// try to limit the scope of this a little bit


#endif
