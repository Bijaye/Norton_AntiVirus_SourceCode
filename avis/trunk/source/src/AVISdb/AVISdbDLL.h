
#ifndef AVISDBDLL_H
#define AVISDBDLL_H

// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the AVISDB_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// AVISDB_API functions as being imported from a DLL, wheras this DLL sees symbols
// defined with this macro as being exported.
#ifdef AVISDB_EXPORTS
#define AVISDB_API __declspec(dllexport)
#define EXPIMP_TEMPLATE
#else
#define AVISDB_API __declspec(dllimport)
#define EXPIMP_TEMPLATE	extern
#endif

#include <string>

//EXPIMP_TEMPLATE template AVISDB_API std::string;

//
//	If you want to open a database other than AVISdata then
//	your code should set this prior to calling your first dll
//	function/class.
AVISDB_API void AVISDatabaseName(std::string& avisDBName);

#endif
