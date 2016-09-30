////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

// Copyright 1998 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/INCLUDE/VCS/sdstrip.h_v   1.0   31 Mar 1998 15:59:54   DCHI  $
//
// Description:
//  Class declarations for Scan and Deliver Stripper
//
//************************************************************************
// $Log:   S:/INCLUDE/VCS/sdstrip.h_v  $
// 
//    Rev 1.0   31 Mar 1998 15:59:54   DCHI
// Initial revision.
// 
//************************************************************************

#pragma once

namespace filter {

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

		//MIKEO: 5/19/06
		//add virtual dtor to allow "delete CContentStripper" (ptr to base)
		//remove Release() method since this object comes from the same DLL instead of a 
		//seperate DLL.  
		//This change allows for using this object with std::auto_ptr or any similiar smart ptr 

		virtual ~CContentStripper() 
		{
		}

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
};

class CContentStripperFactory
{
    public:

        static CContentStripper *CreateInstance();
};

} //namespace filter


