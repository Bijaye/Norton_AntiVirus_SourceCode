// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright 1998 - 2003, 2005 Symantec Corporation. All rights reserved.
//************************************************************************
//
// $Header:   S:/INCLUDE/VCS/sdstrip.h_v   1.0   15 Apr 1998 11:39:18   CNACHEN  $
//
// Description:
//  Class declarations for Scan and Deliver Stripper
//
//************************************************************************
// $Log:   S:/INCLUDE/VCS/sdstrip.h_v  $
// 
//    Rev 1.0   15 Apr 1998 11:39:18   CNACHEN
// Initial revision.
// 
//    Rev 1.0   31 Mar 1998 15:59:54   DCHI
// Initial revision.
// 
//************************************************************************

#ifndef _SDSTRIP_H_

#define _SDSTRIP_H_

#ifdef _SDSTRIP_CPP_
#define SDStripDLLImportExport __declspec(dllexport)
#else
#define SDStripDLLImportExport __declspec(dllimport)
#endif

class CContentStripper
{
    public:

        enum INIT_STATUS
        {
            INIT_NO_ERROR,
            INIT_MALLOC_ERROR,
            INIT_FILE_NOT_FOUND_ERROR,
            INIT_GENERAL_FILE_ERROR
        };

        enum STRIP_STATUS
        {
            STRIP_NO_ERROR,
            STRIP_MALLOC_ERROR,
            STRIP_PASSWORD_ENCRYPTED,
            STRIP_FILE_NOT_FOUND_ERROR,
            STRIP_GENERAL_FILE_ERROR
        };

        virtual INIT_STATUS StartUp
        (
            LPCTSTR         lpszDataFileDirectory
        ) = 0;

        virtual STRIP_STATUS Strip
        (
            LPCTSTR         lpszInputFileName,
            LPCTSTR         lpszOutputFileName
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

class SDStripDLLImportExport CContentStripperFactory
{
    public:

        static CContentStripper *CreateInstance();
};

#endif // #ifndef _SDSTRIP_H_

