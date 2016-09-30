//
//	IBM AntiVirus Immune System
//
//	File Name:	SystemException.cpp
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


#include "stdafx.h"
#include "SystemException.h"

#include <iostream>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


char *SystemException::typeAsChar[] = {
		"AccessViolation",
		"ArrayBoundsExceeded",
		"ExceptionBreakPoint",
		"DataTypeMisalignment",
		"FloatingPointDenormalOperand",
		"FloatingPointDivideByZero",
		"FloatingPointInexactResult",
		"FloatingPointInvalidOperation",
		"FloatingPointOverflow",
		"FloatingPointStackCheck",
		"FloatingPointUnderflow",
		"IllegalInstruction",
		"InPageError",
		"IntDivideByZero",
		"IntOverflow",
		"InvalidDisposition",
		"NonContinuableException",
		"PrivInstruction",
		"SingleStep",
		"StackOverflow",
		"DebugControlC",

		"Unknown"
};


SystemException::SystemException(TypeOfException t) :
					AVISException(), type(t)
{
	LogException();
}


void SystemExceptionTranslator( unsigned int u, EXCEPTION_POINTERS* pExp )
{
	switch (u)
	{
	case EXCEPTION_ACCESS_VIOLATION:
		throw SystemException(SystemException::AccessViolation);

	case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:
		throw SystemException(SystemException::ArrayBoundsExceeded);

	case EXCEPTION_BREAKPOINT:
		throw;

	case EXCEPTION_DATATYPE_MISALIGNMENT:
		throw SystemException(SystemException::DataTypeMisalignment);

	case EXCEPTION_FLT_DENORMAL_OPERAND:
		throw SystemException(SystemException::FloatingPointDenormalOperand);

	case EXCEPTION_FLT_DIVIDE_BY_ZERO:
		throw SystemException(SystemException::FloatingPointDivideByZero);

	case EXCEPTION_FLT_INEXACT_RESULT:
		throw SystemException(SystemException::FloatingPointInexactResult);

	case EXCEPTION_FLT_INVALID_OPERATION:
		throw SystemException(SystemException::FloatingPointInvalidOperation);

	case EXCEPTION_FLT_OVERFLOW:
		throw SystemException(SystemException::FloatingPointOverflow);

	case EXCEPTION_FLT_STACK_CHECK:
		throw SystemException(SystemException::FloatingPointStackCheck);

	case EXCEPTION_FLT_UNDERFLOW:
		throw SystemException(SystemException::FloatingPointUnderflow);

	case EXCEPTION_ILLEGAL_INSTRUCTION:
		throw SystemException(SystemException::IllegalInstruction);

	case EXCEPTION_IN_PAGE_ERROR:
		throw SystemException(SystemException::InPageError);

	case EXCEPTION_INT_DIVIDE_BY_ZERO:
		throw SystemException(SystemException::IntDivideByZero);

	case EXCEPTION_INT_OVERFLOW:
		throw SystemException(SystemException::IntOverflow);

	case EXCEPTION_INVALID_DISPOSITION:
		throw SystemException(SystemException::InvalidDisposition);

	case EXCEPTION_NONCONTINUABLE_EXCEPTION:
		throw SystemException(SystemException::NonContinuableException);

	case EXCEPTION_PRIV_INSTRUCTION:
		throw SystemException(SystemException::PrivInstruction);

	case EXCEPTION_SINGLE_STEP:
		throw;

	case EXCEPTION_STACK_OVERFLOW:
		throw SystemException(SystemException::StackOverflow);

	default:
		throw SystemException(SystemException::Unknown);
	}
}

void SystemException::Init()
{
    _set_se_translator(SystemExceptionTranslator);
}
