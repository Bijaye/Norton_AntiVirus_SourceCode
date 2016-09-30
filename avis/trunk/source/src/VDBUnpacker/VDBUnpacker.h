//
//	IBM AntiVirus Immune System
//
//	File Name:	VDBUnpacker.h
//	Author:		Milosz Muszynski
//
//	VDB Package Extraction Utility Module
//
//	$Log:  $
//

//------ common definitions -------------------------------------------

#ifdef VDBUNPACKER_EXPORTS
#define VDBUNPACKER_API __declspec(dllexport)
#else
#define VDBUNPACKER_API __declspec(dllimport)
#endif

//------ common pragmas -----------------------------------------------
#pragma warning( disable : 4251 ) // class needs to have dll-interface warning 

//------ common includes ----------------------------------------------
#include <string>
#include <windows.h>
#include "VDBPackage.h"
