// Copyright 1998 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/SENDME/VCS/sdsendme.cpv   1.1   16 Apr 1998 11:42:58   CNACHEN  $
//
// Description:
//  Source file containing Scan and Deliver Send Me Implementation
//
// Contains:
//
// See Also:
//
//************************************************************************
// $Log:   S:/SENDME/VCS/sdsendme.cpv  $
// 
//    Rev 1.1   16 Apr 1998 11:42:58   CNACHEN
// added self-determination policy.
// 
//    Rev 1.0   18 Mar 1998 12:31:48   CNACHEN
// Initial revision.
// 
//************************************************************************

#include "platform.h"
#include <string.h>

#define _SDSENDME_CPP_

#include "n30type.h"

#include "sdsendme.h"
#include "sdsmi.h"      // implementation header file
#include "sdsmstrs.h"     // send me resource string #define file
#include "AVRESBranding.h"     // send me resource string #define file
#include "ccLib.h"
#include "ccLibDllLink.h"

//********************************************************************
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
//********************************************************************

int ViralSubmissionFilterImp::StartUp(LPCTSTR lpszDataFileDirectory,
									  LPCTSTR lpszAVDataFileDirectory, // optional (=NULL)
									  int nFilterType)
{
    // remember what we're filtering!
    m_nFilterType = nFilterType;

    return(INIT_STATUS_NO_ERROR);
}

//********************************************************************
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
//  SUBMIT_STATUS_ALLOW_SUBMISSION      if we want to allow the submission
//  SUBMIT_STATUS_REFUSE_SUBMISSION     if we want to refuse the submission
//  SUBMIT_STATUS_GENERAL_ERROR         if there is an error
//********************************************************************

///////////////////////////////////////////////////////////////////////
// TODO: THIS IS BROKEN!
// N30 STRUCT NO LONGER USED IN QUARANTINE, BUT SND ViralSubmissionFilter
// HAS NOT BEEN UPDATED.
// ViralSubmissionFilter ALSO CURRENTLY USES wControl AND wControl2
// PROPERTIES, WHICH WE ARE NOT SURE ARE STILL NEEDED OR VALID
///////////////////////////////////////////////////////////////////////
int ViralSubmissionFilterImp::AllowSubmission(LPCTSTR lpszFileName,
											  LPN30 lpstN30,
											  BOOL bRepairable,
											  LPTSTR lpszReason)
{
    switch (m_nFilterType)
    {
        case FILTER_TYPE_ALLOW_ALL_BLOODHOUND_AND_NON_REPAIRED:
            return(AllowBloodhoundAndNonRepairable(lpstN30, bRepairable, lpszReason));
        case FILTER_TYPE_ALLOW_ONLY_NON_REPAIRED:
            return(AllowNonRepairable(lpstN30,bRepairable,lpszReason));
        case FILTER_TYPE_SELF_DETERMINATION:
            return(AllowNonRepairable(lpstN30,bRepairable,lpszReason));
        default:
            // no such policy!
            return(SUBMIT_STATUS_GENERAL_ERROR);
    }
}

//********************************************************************
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
//  nothing
//********************************************************************

void ViralSubmissionFilterImp::ShutDown(void)
{
    // n/a
}


//********************************************************************
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
//  nothing
//********************************************************************
                           
void ViralSubmissionFilterImp::Release(void)
{
    delete this;
}


//********************************************************************
// Function:
//  int AllowNonRepairable()
//
// Description:
//  Determines whether or not we should allow a sample, given that we
//  only want to accept non-repairable malware where delete is not a valid
//  option. (Delete is perfectly valid for trojan horses, for instance)
//
// Parameters:
//
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
//********************************************************************

int ViralSubmissionFilterImp::AllowNonRepairable(LPN30 lpstN30,
												 BOOL bRepairable,
												 LPTSTR lpszReason)
{
    // check to see if we're dealing with a file that we don't want the user
    // to forward to SARC!

    // if it can be repaired, forget it!
    if (bRepairable == TRUE)
    {
        if (LoadString(IDS_REFUSE_SUBMIT_REPAIRABLE,lpszReason) != TRUE)
            return(SUBMIT_STATUS_GENERAL_ERROR);

        return(SUBMIT_STATUS_REFUSE_SUBMISSION);
    }

    // If the following two bits are set, we don't want the user to submit
    // a sample because it's a non-repairable item that we already know about
    // that just needs to be deleted by the user. Bits:
    //
    //  1. The NO_CLEAN bit will be set.
    //  2. The NO_GENERIC bit will be set.
    if ((lpstN30->wControl & bitCTRL_NOTCLEAN) &&
        (lpstN30->wControl2 & N30_CTRL2_NO_GENERIC))
    {
        // no way!

        if ((lpstN30->wControl2 & bitINFO_MASKEXT) == bitINFO_AGENT)
        {
            // trojan horse!
            if (LoadString(IDS_REFUSE_SUBMIT_KNOWN_TROJAN_DELETE_OK,
                           lpszReason) != TRUE)
                return(SUBMIT_STATUS_GENERAL_ERROR);
        }
        else
        {
            // virus!
            if (LoadString(IDS_REFUSE_SUBMIT_KNOWN_VIRUS_DELETE_OK,
                           lpszReason) != TRUE)
                return(SUBMIT_STATUS_GENERAL_ERROR);
        }

        return(SUBMIT_STATUS_REFUSE_SUBMISSION);
    }

    // It's OK to send it off!

    *lpszReason = 0;

    return(SUBMIT_STATUS_ALLOW_SUBMISSION);
}

//********************************************************************
// Function:
//  int AllowBloodhoundAndNonRepairable()
//
// Description:
//  Determines whether or not we should allow a sample, given that we
//  only want to accept (1) non-repairable malware where delete is not a valid
//  option. (Delete is perfectly valid for trojan horses, for instance) or
//  (2) any bloodhound detections
//
// Parameters:
//
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
//********************************************************************
                                 
int ViralSubmissionFilterImp::AllowBloodhoundAndNonRepairable(LPN30 lpstN30,
															  BOOL bRepairable,
															  LPTSTR lpszReason)
{
    char szVirusName[N30_SIZE_VIRUSNAME+1];

    // check to see if we're dealing with a file that we don't want the user
    // to forward to SARC!

    // see if its a bloodhound detection first - if so, its cool to send it!

    strncpy(szVirusName,lpstN30->lpVName,N30_SIZE_VIRUSNAME);
    szVirusName[N30_SIZE_VIRUSNAME] = 0;

    strupr(szVirusName);

    if (strstr(szVirusName,"BLOODHOUND") != NULL)
    {
        // its OK!
        return(SUBMIT_STATUS_ALLOW_SUBMISSION);
    }

    // if it can be repaired, forget it!
    if (bRepairable == TRUE)
    {
        if (LoadString(IDS_REFUSE_SUBMIT_REPAIRABLE,lpszReason) != TRUE)
            return(SUBMIT_STATUS_GENERAL_ERROR);

        return(SUBMIT_STATUS_REFUSE_SUBMISSION);
    }

    // If the following two bits are set, we don't want the user to submit
    // a sample because it's a non-repairable item that we already know about
    // that just needs to be deleted by the user. Bits:
    //
    //  1. The NO_CLEAN bit will be set.
    //  2. The NO_GENERIC bit will be set.
    if ((lpstN30->wControl & bitCTRL_NOTCLEAN) &&
        (lpstN30->wControl2 & N30_CTRL2_NO_GENERIC))
    {
        // no way!

        if ((lpstN30->wControl2 & bitINFO_MASKEXT) == bitINFO_AGENT)
        {
            // trojan horse!
            if (LoadString(IDS_REFUSE_SUBMIT_KNOWN_TROJAN_DELETE_OK,
                           lpszReason) != TRUE)
                return(SUBMIT_STATUS_GENERAL_ERROR);
        }
        else
        {
            // virus!
            if (LoadString(IDS_REFUSE_SUBMIT_KNOWN_VIRUS_DELETE_OK,
                           lpszReason) != TRUE)
                return(SUBMIT_STATUS_GENERAL_ERROR);
        }

        return(SUBMIT_STATUS_REFUSE_SUBMISSION);
    }

    // It's OK to send it off!

    *lpszReason = 0;

    return(SUBMIT_STATUS_ALLOW_SUBMISSION);
}

//********************************************************************
// Function:
//  BOOL LoadString()
//
// Description:
//  Loads a string from our string DLL.
//
// Parameters:
//  nStringNum                  String number to load, found in sdsmstrs.h
//  lpszString                  Result placed here; must be
//                              MAX_DESCRIPTION_LENGTH characters long
//
// Returns:
//  TRUE on success
//  FALSE on failure
//********************************************************************


BOOL ViralSubmissionFilterImp::LoadString(int nStringNum,
										  LPTSTR lpszString)
{
    HINSTANCE       hInstance;
    int             nResult;

    // find our DLL
    hInstance = LoadLibrary(RESOURCE_DLL);
    if (hInstance == NULL)
        return(FALSE);

	if(m_csProductName.IsEmpty())
	{
		CBrandingRes BrandRes;
		m_csProductName = BrandRes.ProductName();
	}

    // load our string
    nResult = ::LoadString(hInstance,
                           nStringNum,
                           lpszString,
                           MAX_DESCRIPTION_LENGTH);

	switch(nStringNum)
	{
	case IDS_REFUSE_SUBMIT_REPAIRABLE:
	case IDS_REFUSE_SUBMIT_KNOWN_TROJAN_DELETE_OK:
	case IDS_REFUSE_SUBMIT_KNOWN_VIRUS_DELETE_OK:
      {
	      CString csBuffer(lpszString);
	      wsprintf(lpszString, csBuffer, m_csProductName);
      }
		break;

	default:
		break;
	}


    // free our DLL
    if (FreeLibrary((HMODULE)hInstance) == FALSE)
        return(FALSE);

    // return
    if (nResult == 0)
        return(FALSE);

    return(TRUE);
}

//********************************************************************
// Function:
//  ViralSubmissionFilter *CreateInstance()
//
// Description:
//  Creates a new instance of our class
//
// Parameters:
//  none
//
// Returns:
//  A pointer to the new instance or NULL on error.
//********************************************************************

ViralSubmissionFilter *ViralSubmissionFilterFactory::CreateInstance(void)
{
    return new ViralSubmissionFilterImp;
}
