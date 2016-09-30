// Copyright 1998 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/INCLUDE/VCS/sdsendme.h_v   1.0   15 Apr 1998 16:36:38   JTaylor  $
//
// Description:
//  Header file for Scan and Deliver Send Me class declarations
//
// Contains:
//
// See Also:
//
//************************************************************************
// $Log:   S:/INCLUDE/VCS/sdsendme.h_v  $
// 
//    Rev 1.0   15 Apr 1998 16:36:38   JTaylor
// Initial revision.
// 
//    Rev 1.0   18 Mar 1998 12:32:24   CNACHEN
// Initial revision.
// 
//************************************************************************

#ifndef _SDSENDME_H_

#define _SDSENDME_H_

#define FILTER_TYPE_SELF_DETERMINATION                      1
#define FILTER_TYPE_ALLOW_ALL_BLOODHOUND_AND_NON_REPAIRED   2
#define FILTER_TYPE_ALLOW_ONLY_NON_REPAIRED                 3

#define INIT_STATUS_NO_ERROR                                0
#define INIT_STATUS_MALLOC_ERROR                            1
#define INIT_STATUS_FILE_NOT_FOUND_ERROR                    2
#define INIT_STATUS_GENERAL_FILE_ERROR                      3

#define SUBMIT_STATUS_ALLOW_SUBMISSION                      1
#define SUBMIT_STATUS_REFUSE_SUBMISSION                     2
#define SUBMIT_STATUS_GENERAL_ERROR                         3

#define MAX_DESCRIPTION_LENGTH                              1024

class ViralSubmissionFilter
{
    public:

//********************************************************************
//
// Function:
//  int StartUp()
//
// Description:
//  Initializes and loads all data for the viral filter.
//
// Parameters:
//  lpszDataFileDirectory   Directory without trailing backslash that
//                          contains all filter support data files if any
//  lpszAVDataFileDirectory Path to NAV virus definition files, without
//                          trailing backslash.
//  nFilterType             Specifies what type of filter to use:
//                           FILTER_TYPE_ALLOW_ALL_BLOODHOUND_AND_NON_REPAIRED
//                              allow any bloodhound submission (with
//                              "bloodhound" in the virus name) and all
//                              non-repairable malware for which delete
//                              is not a valid repair option.
//                           FILTER_TYPE_ALLOW_ONLY_NON_REPAIRED
//                              allow all non-repairable malware for which
//                              delete is not a valid repair option.
// Returns:
//
//  INIT_STATUS_NO_ERROR             On successful load of data
//  INIT_STATUS_MALLOC_ERROR         On failure due to failed memory allocation
//  INIT_STATUS_FILE_NOT_FOUND_ERROR On failure due to data file not found
//  INIT_STATUS_GENERAL_FILE_ERROR   On general file error or bad data file
//                                    contents
//
//********************************************************************


        virtual int StartUp
        (
            LPCTSTR         lpszDataFileDirectory,
            LPCTSTR         lpszAVDataFileDirectory, //optional (=NULL)
            int             nFilterType
        ) = 0;

//********************************************************************
//
// Function:
//  int AllowSubmission()
//
// Description:
//  Determines whether or not we wish to allow a submission of a virus or
//  Trojan file.
//
// Parameters:
//  lpszFileName            Full path and filename of the offending file
//  lpstN30                 The NAV N30 record describing the virus/Trojan
//                          in this file
//  bRepairable             TRUE if NAV can repair this, FALSE otherwise
//  lpszReason              Filled in by this function to provide a reason
//                          why the user shouldn't submit, if the function
//                          returns SUBMIT_STATUS_REFUSE_SUBMISSION.
//
// Returns:
//
//  SUBMIT_STATUS_ALLOW_SUBMISSION      if we want to allow the submission
//  SUBMIT_STATUS_REFUSE_SUBMISSION     if we want to refuse the submission
//  SUBMIT_STATUS_GENERAL_ERROR         if there is an error
//
//********************************************************************

        virtual int AllowSubmission
        (
            LPCTSTR         lpszFileName,
            LPN30           lpstN30,
            BOOL            bRepairable,
            LPTSTR          lpszReason
        ) = 0;

//********************************************************************
//
// Function:
//  void ShutDown()
//
// Description:
//  Unloads any data loaded during StartUp
//
// Parameters:
//  None
//
// Returns:
//
//  nothing
//
//********************************************************************


        virtual void ShutDown
        (
            void
        ) = 0;

//********************************************************************
//
// Function:
//  void Release()
//
// Description:
//  Performs a delete this;
//
// Parameters:
//  None
//
// Returns:
//
//  nothing
//
//********************************************************************


        virtual void Release
        (
            void
        ) = 0;
};

#ifdef _SDSENDME_CPP_
#define SDSMDLLImportExport __declspec(dllexport)
#else
#define SDSMDLLImportExport __declspec(dllimport)
#endif

//********************************************************************
//
// Function:
//  ViralSubmissionFilter *CreateInstance()
//
// Description:
//  Creates a new instance of our class
//
// Parameters:
//
//  none
//
// Returns:
//
//  A pointer to the new instance or NULL on error.
//
//********************************************************************



class SDSMDLLImportExport ViralSubmissionFilterFactory
{
    public:

        static ViralSubmissionFilter *CreateInstance();
};

#endif // #ifndef _SDSENDME_H_

