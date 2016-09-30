// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once


#include <iostream>
#include <tchar.h>
#include "SymTestIncludes.h"
#include "DebugStream.h"
#include "SavApTestFile.h"

//Provides the declaration of a debug stream.  extern places it in global scope,
//rather than file scope.  Paired up with definition in stdafx.cpp.
extern DebugStream debug;

//Useful typedefs that are used cross-file
typedef std::vector<SavApTestFile> TestFileVector;
typedef TestFileVector::iterator TestFileIter;

typedef std::vector<FILEACTION> FileActionVector;
typedef FileActionVector::iterator FileActionIterator;

typedef std::vector<std::string> DirectoryVector;
typedef DirectoryVector::iterator DirectoryIterator;

typedef std::vector<std::string> ActionVector;
typedef ActionVector::const_iterator ActionIter;