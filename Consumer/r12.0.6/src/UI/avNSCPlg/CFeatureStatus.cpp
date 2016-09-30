// ----------------------------------------------------------------------------
// PROPRIETARY/CONFIDENTIAL.  Use of this product is subject to license terms.
// Copyright (C) 2005 Symantec Corporation.  All rights reserved.
// ----------------------------------------------------------------------------

#include "stdafx.h"

#include "CFeature.h"
#include "util.h"

// overrides for GetStatus
nsc::NSCRESULT CAutoProtectFeature::GetStatus( const nsc::IContext* context_in, 
                                                const nsc::IConsoleType*& status_out) 
                                                const throw()
{
	// CodeReview: Is this really necessary?
	CROSS_PROCESS_LOCK();

	CCTRCTXI0(_T("Start"));

    // check params
    if(status_out != NULL || context_in == NULL)
    {
        TRACEE(_T("Bad param"));
        return nsc::NSCERR_ILLEGALPARAM;
    }

    nsc::NSCRESULT result = nsc::NSC_FAIL;
    nsc::IConsoleTypePtr spStatus = NULL;
    long lStatus = AVStatus::statusError;

    // If we fail to get real status use the above defaults
    CEventData* pEventData = NULL;
    if(CFeature::getInternalStatus (context_in, pEventData) &&
        pEventData)
    {
        ccLib::CSingleLock lock (&m_critStatus, INFINITE, FALSE);
        pEventData->GetData(AVStatus::propAPStatus, lStatus);
    }    

	// CodeReview: How do set this to an "Error" state?
	//  Below, it seems like we are on or off, isn't there
	//  an Unknown state, too?
	//

	// Set the status.
	switch(lStatus)
	{
		case AVStatus::statusEnabled:
			result = CreateStatus (&spStatus, nsc::ONOFF_CONSOLETYPE, true);
            CHECK_NSCRESULT(result);
			break;
		case AVStatus::statusDisabled:
		case AVStatus::statusNotRunning:
		case AVStatus::statusNotInstalled:			// NOT USED
		case AVStatus::statusError:
		default:
			result = CreateStatus (&spStatus, nsc::ONOFF_CONSOLETYPE, false);
            CHECK_NSCRESULT(result);
			break;
		}
    if(NSC_FAILED(result))
    {
        TRACEE(_T("CreateStatus failed for AP"));
        return result;
    }

	status_out = spStatus;
	status_out->AddRef();

    return result;
}

nsc::NSCRESULT CEmailScanningFeature::GetStatus(const nsc::IContext* context_in, 
                                                 const nsc::IConsoleType*& status_out) 
                                                 const throw()
{
    CROSS_PROCESS_LOCK();

	CCTRCTXI0(_T("Start"));

    // check params
    if(status_out != NULL || context_in == NULL)
    {
        TRACEE(_T("Bad param"));
        return nsc::NSCERR_ILLEGALPARAM;
    }

    nsc::NSCRESULT result = nsc::NSC_FAIL;
    nsc::IConsoleTypePtr spStatus = NULL;
    cc::IStringPtr spStatusStr = NULL;

    // CodeReview: Be pessimistic, assume failure
	long lStatus = AVStatus::statusError;
	long lSMTPVal = 0;
	long lPOPVal = 0;

    // If we fail to get real status use the above defaults
    CEventData* pEventData = NULL;
    if(CFeature::getInternalStatus (context_in, pEventData) &&
        pEventData)
    {
        ccLib::CSingleLock lock (&m_critStatus, INFINITE, FALSE);
        pEventData->GetData(AVStatus::propEmailStatus, lStatus);
        pEventData->GetData(AVStatus::propEmailSMTP, lSMTPVal);
        pEventData->GetData(AVStatus::propEmailPOP, lPOPVal);
    }    

	switch(lStatus)
	{
		case AVStatus::statusEnabled:
			if(lPOPVal && lSMTPVal)
			{
				result = CreateStatus (&spStatus, nsc::ONOFF_CONSOLETYPE, true);
				CHECK_NSCRESULT(result);
			}
			else if(lPOPVal && !lSMTPVal)
			{
                ccLib::CString sIncoming;	
	            if(!sIncoming.LoadString(IDS_STATUS_EMAIL_INCOMING))
                {
                    TRACEE(_T("LoadString failed on %d with error %d"), IDS_STATUS_EMAIL_INCOMING, ::GetLastError());
                    sIncoming = "";
                }
                spStatusStr.Attach(ccSym::CStringImpl::CreateStringImpl(sIncoming));
                if(!spStatusStr)
                {
                    TRACEE(_T("String implementation failed for incoming in email status."));
                    return nsc::NSC_FAIL;
                }
				result = CreateStatus (&spStatus, spStatusStr);
				CHECK_NSCRESULT(result);
			}
			else if(!lPOPVal && lSMTPVal)
			{
                ccLib::CString sOutgoing;	
	            if(!sOutgoing.LoadString(IDS_STATUS_EMAIL_OUTGOING))
                {
                    TRACEE(_T("LoadString failed on %d with error %d"), IDS_STATUS_EMAIL_OUTGOING, ::GetLastError());
                    sOutgoing = "";
                }
                spStatusStr.Attach(ccSym::CStringImpl::CreateStringImpl(sOutgoing));
                if(!spStatusStr)
                {
                    TRACEE(_T("String implementation failed for outgoing in email status."));
                    return nsc::NSC_FAIL;
                }
				result = CreateStatus (&spStatus, spStatusStr);
				CHECK_NSCRESULT(result);
			}
			break;
		case AVStatus::statusDisabled:
			result = CreateStatus (&spStatus, nsc::ONOFF_CONSOLETYPE, false);
			CHECK_NSCRESULT(result);
			break;
		case AVStatus::statusNotRunning:
		case AVStatus::statusNotInstalled:
		case AVStatus::statusError:
		default:
			result = CreateStatus (&spStatus, nsc::ONOFF_CONSOLETYPE, false);
			CHECK_NSCRESULT(result);
			break;
	}

    if(NSC_FAILED(result))
    {
        TRACEE(_T("CreateStatus failed for email scanning"));
        return result;
    }

    status_out = spStatus;
	status_out->AddRef();

    return result;
}

nsc::NSCRESULT CSpywareProtectionFeature::GetStatus(  const nsc::IContext* context_in, 
                                    const nsc::IConsoleType*& status_out) 
                                    const throw()
{
    CROSS_PROCESS_LOCK();

	CCTRCTXI0(_T("Start"));

    // check params
    if(status_out != NULL || context_in == NULL)
    {
        TRACEE(_T("Bad param"));
        return nsc::NSCERR_ILLEGALPARAM;
    }

    nsc::NSCRESULT result = nsc::NSC_FAIL;
	long lSpywareOn = 0;
	long lSpywareInstalled = 0;
    nsc::IConsoleTypePtr spStatus = NULL;
    cc::IStringPtr spStatusStr = NULL;

    // If we fail to get real status use the above defaults
    CEventData* pEventData = NULL;
    if(CFeature::getInternalStatus (context_in, pEventData) &&
        pEventData)
    {
        ccLib::CSingleLock lock (&m_critStatus, INFINITE, FALSE);
        pEventData->GetData(AVStatus::propSpywareCat, lSpywareOn);
		pEventData->GetData(AVStatus::propSpywareInstalled, lSpywareInstalled);
    }    
               
    // Spyware Feature status only determined by Spyware Cat
    // ignore ap, ap-threat and adware setting
	if(0 == lSpywareInstalled)
	{
		result = CreateStatus (&spStatus, nsc::NOTINSTALLED_CONSOLETYPE);
		CHECK_NSCRESULT(result);
	}
	else if(0 != lSpywareOn)
	{
		result = CreateStatus (&spStatus, nsc::ONOFF_CONSOLETYPE, true);
		CHECK_NSCRESULT(result);
	}
	else
	{
		result = CreateStatus (&spStatus, nsc::ONOFF_CONSOLETYPE, false);
		CHECK_NSCRESULT(result);
	}

    if(NSC_FAILED(result))
	{
        TRACEE(_T("CreateStatus failed for Spyware"));
		return result;
	}
		
    status_out = spStatus;
	status_out->AddRef();

    return result;
}

nsc::NSCRESULT CIFPFeature::GetStatus(const nsc::IContext* context_in, 
                                       const nsc::IConsoleType*& status_out) 
                                       const throw()
{
    CROSS_PROCESS_LOCK();

	CCTRCTXI0(_T("Start"));

    // check params
    if(status_out != NULL || context_in == NULL)
    {
        TRACEE(_T("Bad param"));
        return nsc::NSCERR_ILLEGALPARAM;
    }

    nsc::NSCRESULT result = nsc::NSC_FAIL;
    nsc::IConsoleTypePtr spStatus = NULL;
    long lStatus = AVStatus::statusError;

    // If we fail to get real status use the above defaults
    CEventData* pEventData = NULL;
    if(CFeature::getInternalStatus (context_in, pEventData) &&
        pEventData)
    {
        ccLib::CSingleLock lock (&m_critStatus, INFINITE, FALSE);
        pEventData->GetData(AVStatus::propIWPStatus, lStatus);
    }    

	// CodeReview: Why are we returning an error string here when
	//  elsewhere we return false for errors?

	switch(lStatus)
	{
		case AVStatus::statusEnabled:
			result = CreateStatus (&spStatus, nsc::ONOFF_CONSOLETYPE, true);
            CHECK_NSCRESULT(result);
			break;
		case AVStatus::statusNotAvailable: // IWP not in this layout
		case AVStatus::statusNotRunning: // Yielding - console should use NPF status
			result = CreateStatus (&spStatus, nsc::NOTINSTALLED_CONSOLETYPE);
            CHECK_NSCRESULT(result);
			break;
        case AVStatus::statusError:
            {
                ccLib::CString sError;
                cc::IStringPtr spStatusStr;
                if (!sError.LoadString(IDS_STATUS_ERROR))
                {
                    TRACEE(_T("CIFPFeature::GetStatus - LoadString failed to load string %d with error %d"),
                              IDS_STATUS_ERROR, ::GetLastError());
                    sError = "";
                }
                spStatusStr = ccSym::CStringImpl::CreateStringImpl(sError);
                if(!spStatusStr)
                {
                    TRACEE(_T("String implementation failed for error in IFP."));
                    return nsc::NSC_FAIL;
                }
                result = CreateStatus (&spStatus, spStatusStr);
                CHECK_NSCRESULT(result);
                break;
            }
		case AVStatus::statusDisabled:
		case AVStatus::statusNotInstalled:
		default:
			result = CreateStatus (&spStatus, nsc::ONOFF_CONSOLETYPE, false);
            CHECK_NSCRESULT(result);
			break;
	}

    if(NSC_FAILED(result))
	{
        TRACEE(_T("CreateStatus failed for IFP"));
		return result;
	}
		
    status_out = spStatus;
	status_out->AddRef();

    return result;
}

nsc::NSCRESULT CIMScanningFeature::GetStatus(const nsc::IContext* context_in, 
                                              const nsc::IConsoleType*& status_out) 
                                              const throw()
{
    CROSS_PROCESS_LOCK();

	CCTRCTXI0(_T("Start"));

    // check params
    if(status_out != NULL || context_in == NULL)
    {
        TRACEE(_T("Bad param"));
        return nsc::NSCERR_ILLEGALPARAM;
    }

    nsc::NSCRESULT result = nsc::NSC_FAIL;
    nsc::IConsoleTypePtr spStatus = NULL;
    cc::IStringPtr spStatusStr = NULL;

	// CodeReview: Hmm... We need an error state to init with here.
	//  Otherwise, error states are weird.  Logic doesn't flow.
	long lMSN = AVStatus::IM_Status_NotInstalled;
	long lAOL = AVStatus::IM_Status_NotInstalled;
	long lYahoo = AVStatus::IM_Status_NotInstalled;

    // If we fail to get real status use the above defaults
    CEventData* pEventData = NULL;
    if(CFeature::getInternalStatus (context_in, pEventData) &&
        pEventData)
    {
        ccLib::CSingleLock lock (&m_critStatus, INFINITE, FALSE);
        pEventData->GetData(AVStatus::propIMMSN, lMSN);
        pEventData->GetData(AVStatus::propIMYIM, lYahoo);
        pEventData->GetData(AVStatus::propIMAOL, lAOL);
    }    

    // No IMs installed.
    if((lMSN == AVStatus::IM_Status_NotInstalled) && 
		(lAOL == AVStatus::IM_Status_NotInstalled) && 
		(lYahoo == AVStatus::IM_Status_NotInstalled))
	{
        ccLib::CString sNone;	
	    if(!sNone.LoadString(IDS_STATUS_IM_NONE))
        {
            TRACEE(_T("LoadString failed on %d with error %d"), IDS_STATUS_IM_NONE, ::GetLastError());
            sNone = "";
        }
        spStatusStr.Attach(ccSym::CStringImpl::CreateStringImpl(sNone));
        if(!spStatusStr)
        {
            TRACEE(_T("String implementation failed for no IMs installed."));
            return nsc::NSC_FAIL;
        }
		result = CreateStatus (&spStatus, spStatusStr);
		CHECK_NSCRESULT(result);
	}
    // Protection ON for all IMs installed.
    else if(((lMSN == AVStatus::IM_Status_Protected) || (lMSN == AVStatus::IM_Status_NotInstalled)) && 
		((lAOL == AVStatus::IM_Status_Protected) || (lAOL == AVStatus::IM_Status_NotInstalled)) && 
		((lYahoo == AVStatus::IM_Status_Protected) || (lYahoo == AVStatus::IM_Status_NotInstalled)))
	{
		result = CreateStatus (&spStatus, nsc::ONOFF_CONSOLETYPE, true);
		CHECK_NSCRESULT(result);
	}
    // Protection OFF for all IMs installed.
    else if(((lMSN == AVStatus::IM_Status_NotProtected) || (lMSN == AVStatus::IM_Status_NotInstalled)) && 
		    ((lAOL == AVStatus::IM_Status_NotProtected) || (lAOL == AVStatus::IM_Status_NotInstalled)) && 
		    ((lYahoo == AVStatus::IM_Status_NotProtected) || (lYahoo == AVStatus::IM_Status_NotInstalled)))
	{
		result = CreateStatus (&spStatus, nsc::ONOFF_CONSOLETYPE, false);
		CHECK_NSCRESULT(result);
	}
    // Protected ON for some IMs installed.
	else 
	{
        ccLib::CString sPartial;	
	    if(!sPartial.LoadString(IDS_STATUS_IM_PARTIAL))
        {
            TRACEE(_T("LoadString failed on %d with error %d"), IDS_STATUS_IM_PARTIAL, ::GetLastError());
            sPartial = "";
        }
		spStatusStr.Attach(ccSym::CStringImpl::CreateStringImpl(sPartial));
        if(!spStatusStr)
        {
            TRACEE(_T("String implementation failed for partial IMs installed."));
            return nsc::NSC_FAIL;
        }
		result = CreateStatus (&spStatus, spStatusStr);
		CHECK_NSCRESULT(result);
	}

    if(NSC_FAILED(result))
    {
        TRACEE(_T("CreateStatus failed"));
        return result;
    }

    status_out = spStatus;
	status_out->AddRef();

    return result;
}

nsc::NSCRESULT CWormBlockingFeature::GetStatus(const nsc::IContext* context_in, 
                                                const nsc::IConsoleType*& status_out) 
                                                const throw()
{
    CROSS_PROCESS_LOCK();

	CCTRCTXI0(_T("Start"));

    // check params
    if(status_out != NULL || context_in == NULL)
    {
        TRACEE(_T("Bad param"));
        return nsc::NSCERR_ILLEGALPARAM;
    }

    nsc::NSCRESULT result = nsc::NSC_FAIL;
    nsc::IConsoleTypePtr spStatus = NULL;
	long lOEHVal = 0;
	long lSMTPVal = 0;

    // If we fail to get real status use the above defaults
    CEventData* pEventData = NULL;
    if(CFeature::getInternalStatus (context_in, pEventData) &&
        pEventData)
    {
        ccLib::CSingleLock lock (&m_critStatus, INFINITE, FALSE);
        pEventData->GetData(AVStatus::propEmailSMTP, lSMTPVal);
        pEventData->GetData(AVStatus::propEmailOEH, lOEHVal);
    }    					

    // outbound email scanning and OEH must be on for worm blocking
    if(lSMTPVal && lOEHVal)
	{
		result = CreateStatus (&spStatus, nsc::ONOFF_CONSOLETYPE, true);
        CHECK_NSCRESULT(result);
	}
	else
	{
		result = CreateStatus (&spStatus, nsc::ONOFF_CONSOLETYPE, false);
        CHECK_NSCRESULT(result);
	}

    if(NSC_FAILED(result))
    {
        TRACEE(_T("CreateStatus failed in worm blocking"));
        return result;
    }

    status_out = spStatus;
	status_out->AddRef();

    return result;
}

nsc::NSCRESULT CFSSFeature::GetStatus(const nsc::IContext* context_in, 
                                       const nsc::IConsoleType*& status_out) 
                                       const throw()
{
    CROSS_PROCESS_LOCK();

	CCTRCTXI0(_T("Start"));

    // check params
    if(status_out != NULL || context_in == NULL)
    {
        TRACEE(_T("Bad param"));
        return nsc::NSCERR_ILLEGALPARAM;
    }

    nsc::NSCRESULT result = nsc::NSC_FAIL;
	long lStatus = AVStatus::statusError;
    nsc::IConsoleTypePtr spStatus = NULL;
	std::wstring strDate;
	cc::IStringPtr spDateString;
    ccLib::CString sIncomplete;	
	LPCWSTR pStrDate;

    // If we fail to get real status use the above defaults
    CEventData* pEventData = NULL;
    if(CFeature::getInternalStatus (context_in, pEventData) &&
        pEventData)
    {
        ccLib::CSingleLock lock (&m_critStatus, INFINITE, FALSE);
        pEventData->GetData(AVStatus::propFSSStatus, lStatus);
        pEventData->GetData(AVStatus::propFSSDateS, strDate);
    }    					

	switch(lStatus)
	{
		case AVStatus::statusEnabled:
			pStrDate = strDate.c_str(); 
			spDateString.Attach(ccSym::CStringImpl::CreateStringImpl(pStrDate));
            if(!spDateString)
            {
                TRACEE(_T("String implementation failed for enabled date in FSS."));
                return nsc::NSC_FAIL;
            }
			result = CreateStatus (&spStatus, spDateString);
			CHECK_NSCRESULT(result);
			break;
		case AVStatus::statusDisabled:
		case AVStatus::statusError:
		default:
	        if(!sIncomplete.LoadString(IDS_STATUS_FSS_INCOMPLETE))
            {
                TRACEE(_T("LoadString failed on %d with error %d"), IDS_STATUS_FSS_INCOMPLETE, ::GetLastError());
                sIncomplete = "";
            }
			spDateString.Attach(ccSym::CStringImpl::CreateStringImpl(sIncomplete));
            if(!spDateString)
            {
                TRACEE(_T("String implementation failed for incomplete in FSS."));
                return nsc::NSC_FAIL;
            }
			result = CreateStatus (&spStatus, spDateString);
			CHECK_NSCRESULT(result);
			break;
	}

    if(NSC_FAILED(result))
    {
        TRACEE(_T("CreateStatus failed"));
        return result;
    }

    status_out = spStatus;
	status_out->AddRef();

    return result;
}

nsc::NSCRESULT CVirusDefsFeature::GetStatus(const nsc::IContext* context_in, 
                                             const nsc::IConsoleType*& status_out) 
                                             const throw()
{
    CROSS_PROCESS_LOCK();

	CCTRCTXI0(_T("Start"));

    // check params
    if(status_out != NULL || context_in == NULL)
    {
        TRACEE(_T("Bad param"));
        return nsc::NSCERR_ILLEGALPARAM;
    }

    nsc::NSCRESULT result = nsc::NSC_FAIL;
    nsc::IConsoleTypePtr spStatus = NULL;
    cc::IStringPtr spDateString = NULL;
	long lStatus = AVStatus::statusError;
	std::wstring strDate;

    // If we fail to get real status use the above defaults
    CEventData* pEventData = NULL;
    if(CFeature::getInternalStatus (context_in, pEventData) &&
        pEventData)
    {
        ccLib::CSingleLock lock (&m_critStatus, INFINITE, FALSE);
        pEventData->GetData(AVStatus::propDefsStatus, lStatus);
        pEventData->GetData(AVStatus::propDefsDate, strDate);
    }    					

	LPCWSTR pStrDate;

	switch(lStatus)
	{
		case AVStatus::statusEnabled:
			pStrDate = strDate.c_str(); 
			spDateString.Attach(ccSym::CStringImpl::CreateStringImpl(pStrDate));
            if(!spDateString)
            {
                TRACEE(_T("String implementation failed for date in virus defs."));
                return nsc::NSC_FAIL;
            }
			result = CreateStatus (&spStatus, spDateString);
			CHECK_NSCRESULT(result);
			break;
		case AVStatus::statusDisabled:
		case AVStatus::statusError:
		default:
            ccLib::CString sError;	
	        if(!sError.LoadString(IDS_STATUS_ERROR))
            {
                    TRACEE(_T("LoadString failed on %d with error %d"), IDS_STATUS_ERROR, ::GetLastError());
                    sError = "";
            }
			spDateString.Attach(ccSym::CStringImpl::CreateStringImpl(sError));
            if(!spDateString)
            {
                TRACEE(_T("String implementation failed for error in virus defs."));
                return nsc::NSC_FAIL;
            }
			result = CreateStatus (&spStatus, spDateString);
			CHECK_NSCRESULT(result);
			break;
	}
    if(NSC_FAILED(result))
    {
        TRACEE(_T("CreateStatus failed in VirusDefs"));
        return result;
    }

    status_out = spStatus;
	status_out->AddRef();

    return result;
}

