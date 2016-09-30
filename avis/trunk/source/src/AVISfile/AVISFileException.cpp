// AVISFileException.cpp: implementation of the AVISFileException class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "AVISFileException.h"

char *AVISFileException::typeAsChar[] = {
										"CompoundDataManip",
										"NullDataManip",
										"ToolFactory"
};

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

AVISFileException::AVISFileException(TypeOfException t, std::string& di)
							: AVISException(), type(t), detailedInfo(di)
{
	LogException();
}

AVISFileException::AVISFileException(TypeOfException t, const char* di)
							: AVISException(), type(t), detailedInfo(di)
{
	LogException();
}

AVISFileException::~AVISFileException()
{

}
