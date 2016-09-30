// FileReporter.h: interface for the FileReporter class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FILEREPORTER_H__B7AEEB35_4D86_11D2_BE8E_0004ACEC6C96__INCLUDED_)
#define AFX_FILEREPORTER_H__B7AEEB35_4D86_11D2_BE8E_0004ACEC6C96__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Reporter.h"

#include <CMclAutoLock.h>
#include <CMclCritSec.h>

class FileReporter : public Reporter
{
private:
	CFile log;
	CFile errors;

	CMclCritSec	critSec;


public:
	FileReporter(const char* okayLog, const char* errorLog) :
		log( okayLog, CFile::modeCreate | CFile::modeWrite),
		errors( errorLog, CFile::modeCreate | CFile::modeWrite)
	{ };


	void Success(std::string& message)
	{
		CMclAutoLock	lock(critSec);

		log.Write(message.c_str(), message.length());
		log.Write("\r\n", 2);
	}

	void Failure(std::string& message)
	{
		CMclAutoLock	lock(critSec);

		errors.Write(message.c_str(), message.length());
		errors.Write("\r\n", 2);
		errors.Flush();
	}

	void Success(const char* message)
	{
		CMclAutoLock	lock(critSec);

		log.Write(message, strlen(message));
		log.Write("\r\n", 2);
	}

	void Failure(const char* message)
	{
		CMclAutoLock	lock(critSec);

		errors.Write(message, strlen(message));
		errors.Write("\r\n", 2);
		errors.Flush();
	}
};

#endif // !defined(AFX_FILEREPORTER_H__B7AEEB35_4D86_11D2_BE8E_0004ACEC6C96__INCLUDED_)
