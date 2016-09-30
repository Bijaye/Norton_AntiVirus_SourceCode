// QuarantineServer.cpp: implementation of the QuarantineServer class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include <iostream>

#include "QuarantineServer.h"
#include <CMclCritSec.h>
#include <CMclAutoLock.h>


//
//	NOTE:	This leaks like a sieve, but it's only temporary until Symantec
//			can get it's act together and deliever the real thing.


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


QuarantineServer::QuarantineServer()
{

}

QuarantineServer::~QuarantineServer()
{

}


bool QuarantineServer::GetQSEnum(QSEnum& qsEnum)
{
	qsEnum.Reset();

	return true;
}


bool QuarantineServer::GetErrorMessage(std::string& errorMessage)
{
	return false;
}


