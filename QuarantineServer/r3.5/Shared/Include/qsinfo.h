// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.

// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the QSINFO32_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// QSINFO32_API functions as being imported from a DLL, wheras this DLL sees symbols
// defined with this macro as being exported.
#ifdef WIN32

#ifdef QSINFO_EXPORTS
#define QSINFO_API __declspec(dllexport)
#else
#define QSINFO_API __declspec(dllimport)
#endif

#else

#define QSINFO_API

#endif


//*****************************************************************************
// 
// QSINFOSTATUS
//

typedef unsigned long QSINFOSTATUS;

#define QSINFOSTATUS_OK           0
#define QSINFOSTATUS_ERROR        1

#ifdef __cplusplus
extern "C" {
#endif

QSINFOSTATUS QSINFO_API QsInfoGetSystemData (HQSERVERITEM hItem, LPTSTR lpFileToQuarantine);

#ifdef __cplusplus
};
#endif
