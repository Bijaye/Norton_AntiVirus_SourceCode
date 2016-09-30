// FilterException.cpp: implementation of the FilterException class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "FilterException.h"


char *FilterException::typeAsChar[] = {
										"Constructor",
										"AddToResults",
										"AppendData",
										"CreateRequest",
										"Done",
										"ParseOutValues",
										"SampleName",
										"ContentName"
};

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

FilterException::FilterException(TypeOfException t, std::string& di)
							: AVISException(), type(t), detailedInfo(di)
{
	LogException();
}

FilterException::FilterException(TypeOfException t, const char* di)
							: AVISException(), type(t), detailedInfo(di)
{
	LogException();
}


FilterException::~FilterException()
{

}
