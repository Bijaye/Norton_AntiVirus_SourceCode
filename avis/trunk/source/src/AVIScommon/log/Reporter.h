//
//	IBM AntiVirus Immune System
//
//	File Name:	Reporter.h
//
//	Author:		Andy Klapper
//
//	This class defines an interface to be used to report success/failure
//	of each operation.
//
//	$Log:  $
//	
//
#ifndef REPORTER_H
#define REPORTER_H


#include <string>

class Reporter
{
public:
	Reporter()			{};
	virtual ~Reporter() {};

	virtual void Success(std::string& message) = 0;
	virtual void Failure(std::string& message) = 0;

	virtual void Success(const char* message) = 0;
	virtual void Failure(const char* message) = 0;

};


#endif
