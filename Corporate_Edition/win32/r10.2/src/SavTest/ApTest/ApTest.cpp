// ApTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "SavApTestTool.h"
#include "ApTestToolSettings.h"
#include "teestream.h"
#include <fstream>

int _tmain(int argc, _TCHAR* argv[])
{
	std::string logFileName( "aptest_" );
	char countString[50] = {0};

	//GetTickCount makes sure that we don't have log file name collisions between runs
	itoa( GetTickCount(), countString, 10 );
	logFileName.append( countString );
	logFileName.append( ".log" );

	//The human readable APTest log file
	std::ofstream logFile( logFileName.c_str() );

	//TeeStream will redirect to cout and our log file
	TeeStream theTeeStream( std::cout.rdbuf(), logFile.rdbuf() );

	//This call does two things:
	//1)replaces cout's rdbuf with theTeeStream
	//2)saves off a pointer to cout's original rdbuf
	std::streambuf* originalCout = std::cout.rdbuf( &theTeeStream );

	if ( NULL == originalCout )
		std::cout << "Error: failed to split output to both screen and log file." << std::endl;

	//Enable debug messages
	debug.Enable();

	SavApTestTool tool;

	tool.Configure();
	tool.Execute();

//	ApTestToolSettings settings;
//	settings.SetFileActions( "open, read, close" );

//	for ( ApTestToolSettings::ActionIter i = settings.GetActionBegin(); i != settings.GetActionEnd(); ++i )
//		std::cout << *i;

    //Reset cout to its original rdbuf 
    std::cout.rdbuf( originalCout );

	//Close the log file
	if ( logFile.is_open() )
		logFile.close();

	return 0;
}