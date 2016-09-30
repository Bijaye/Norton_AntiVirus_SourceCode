// Copyright 1998 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/INCLUDE/VCS/sdfilter.h_v   1.1   12 Jun 1998 11:50:56   jtaylor  $
//
// Description:
//  Class declarations for Scan and Deliver File Filter
//
// See Also:
//
//************************************************************************
// $Log:   S:/INCLUDE/VCS/sdfilter.h_v  $
// 
//    Rev 1.1   12 Jun 1998 11:50:56   jtaylor
// Updated from newvir.
// 
// 
//    Rev 1.3   08 Jun 1998 14:15:50   CNACHEN
// Changed prototype of filter function to provide text info on problem.
// 
//    Rev 1.0   08 Jun 1998 14:15:04   CNACHEN
// Initial revision.
// 
//    Rev 1.2   18 May 1998 14:49:22   CNACHEN
// 
//    Rev 1.1   18 May 1998 14:48:40   CNACHEN
// Added ifdefs for DX
// 
//    Rev 1.0   12 Mar 1998 18:01:38   CNACHEN
// Initial revision.
// 
//************************************************************************

#ifndef _SDFILTER_H_

#define _SDFILTER_H_

#ifndef SYM_DOSX

#ifdef _SDFILTI_CPP_
#define SDFilterDLLImportExport __declspec(dllexport)
#else
#define SDFilterDLLImportExport __declspec(dllimport)
#endif

#else
#define SDFilterDLLImportExport
#endif

#define INIT_NO_ERROR               0
#define INIT_MALLOC_ERROR           1
#define INIT_FILE_NOT_FOUND_ERROR   2
#define INIT_GENERAL_FILE_ERROR     3

#define NO_POSSIBLE_THREAT          0
#define POSSIBLE_THREAT             1
#define POSSIBLE_ARCHIVED_THREAT    2
#define FILTER_FILE_NOT_FOUND_ERROR 3
#define FILTER_MALLOC_ERROR         4
#define FILTER_GENERAL_FILE_ERROR   5

#define MAX_FILTER_DESCRIPTION_LENGTH       512

class CMaliciousCodeFilter
{
    public:

        virtual int StartUp
        (
            LPCTSTR         lpszDataFileDirectory
        ) = 0;

        virtual int Filter
        (
            LPCTSTR         lpszFileName,
            LPTSTR          lpszReason
        ) = 0;

        virtual void ShutDown
        (
            void
        ) = 0;

        virtual void Release
        (
            void
        ) = 0;

};

class SDFilterDLLImportExport CMaliciousCodeFilterFactory
{
    public:

        static CMaliciousCodeFilter *CreateInstance();
};

#endif // #ifndef _SDFILTER_H_
