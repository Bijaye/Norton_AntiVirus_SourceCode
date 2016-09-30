// DateTimeException.cpp: implementation of the DateTimeException class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DateTimeException.h"

#include <iostream>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


char *DateTimeException::typeAsChar[] = {
		"DateTimeStr",
		"DateTimeOpAssgnTm",
		"DateTimeParts",
		"DateTimeMonthIntToStr",
		"DateTimeMonthStrToInt"
};


DateTimeException::DateTimeException(TypeOfException t, std::string& di) :
					AVISException(), type(t), detailedInfo(di)
{
	LogException();
}

DateTimeException::DateTimeException(TypeOfException t, const char* di) :
					AVISException(), type(t), detailedInfo(di)
{
	LogException();
}
