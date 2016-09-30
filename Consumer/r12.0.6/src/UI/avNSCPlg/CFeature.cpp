// ----------------------------------------------------------------------------
// PROPRIETARY/CONFIDENTIAL.  Use of this product is subject to license terms.
// Copyright (C) 2005 Symantec Corporation.  All rights reserved.
// ----------------------------------------------------------------------------
#include "stdafx.h"

//#include "CNAVActionsCollection.h"

#include "CFeature.h"
//#include "CClientCallback.h"

#include "util.h"

//#include "StatusPropertyNames.h"
#include "GlobalMutex.h"
#include "avccModuleID.h"   // module id's

#include "AVRESBranding.h" // Required in order to get product name

// Statics
CFeature::mapSessionStatus CFeature::m_mapLocalStatus;
ccLib::CCriticalSection CFeature::m_critStatus;

CFeature::~CFeature()
{}

// IWrapper 
nsc::NSCRESULT CFeature::GetName(const nsc::IContext* /*context_in*/, 
                                            const cc::IString*& result_out) 
                                            const throw()
{
    ccLib::CString strTemp;
    if (!strTemp.LoadString (m_dwNameID))
    {
        TRACEE(_T("LoadString failed on %d with error %d"), m_dwNameID, ::GetLastError());
        strTemp = "";
    }

    cc::IStringPtr spStrTemp = ccSym::CStringImpl::CreateStringImpl(strTemp);
    if(!spStrTemp)
    {
        TRACEE(_T("String implementation failed for feature GetName."));
        return nsc::NSC_FAIL;
    }

	result_out = spStrTemp;
    return nsc::NSC_SUCCESS;
}
    
nsc::NSCRESULT CFeature::GetGUID(const nsc::IContext* /*context_in*/, 
                                            GUID& guid_out) 
                                            const throw()
{
    guid_out = m_guid;
    return nsc::NSC_SUCCESS;
}


nsc::NSCRESULT CFeature::GetDescription(const nsc::IContext* context_in, 
										const cc::IString*& description_out)
										const throw()
{
	CCTRCTXI0(_T("called"));

	UINT nID = m_dwDescriptionID;
	if(!IsAdmin(context_in))
		nID = m_dwDescriptionIDNonAdmin;

	ccLib::CString strTemp;
    if (!strTemp.LoadString(nID))
    {
        TRACEE(_T("LoadString failed on %d with error %d"), nID, ::GetLastError());
        strTemp = "";
    }

    cc::IStringPtr spStrTemp = ccSym::CStringImpl::CreateStringImpl(strTemp);
    if(!spStrTemp)
    {
        TRACEE(_T("String implementation failed for feature GetDescription."));
        return nsc::NSC_FAIL;
    }

	description_out = spStrTemp;
    return nsc::NSC_SUCCESS;
}


nsc::NSCRESULT CFeature::GetHelpURI(                const nsc::IContext* /*context_in*/, 
                                            const cc::IString*& uri_out) 
                                            const throw()
{
    cc::IStringPtr spStrTemp = ccSym::CStringImpl::CreateStringImpl(m_sHelpURI);
    if(!spStrTemp)
    {
        TRACEE(_T("String implementation failed for feature GetHelpURI."));
        return nsc::NSC_FAIL;
    }

	uri_out = spStrTemp;
    return nsc::NSC_SUCCESS;
}


nsc::NSCRESULT CFeature::GetMonitorsCollection( const nsc::IContext* /*context_in*/,
                                            const nsc::IMonitorsCollection*& monitorCollection_out)
                                            const throw()
{
    return nsc::NSC_DOESNOTEXIST;

}
                                                    

nsc::NSCRESULT CFeature::GetPropertyBag(            const nsc::IContext* /*context_in*/, 
                                            const nsc::IPropertyBag*&  /*propertyBag_out*/)
                                            const throw()
{
    return nsc::NSC_DOESNOTEXIST;
}


nsc::NSCRESULT CFeature::OnMarkedDirty() const throw()
{
    return nsc::NSC_SUCCESS;
}


// IFeature 
nsc::NSCRESULT CFeature::GetStatus( const nsc::IContext* /*context_in*/, 
                                const nsc::IConsoleType*& /*status_out*/) 
                                const throw()
{
	return nsc::NSC_DOESNOTEXIST;
}


nsc::NSCRESULT CFeature::GetHealth( const nsc::IContext* /*context_in*/, 
                                const nsc::IHealth*& /*health_out*/) 
                                const throw()
{
	return nsc::NSC_DOESNOTEXIST;
}

nsc::NSCRESULT CFeature::GetActions(const nsc::IContext* /*context_in*/, 
                                const nsc::IActionsCollection*& /*actions_out*/)
                                const throw()
{
	return nsc::NSC_DOESNOTEXIST;
}


nsc::NSCRESULT CFeature::GetTTL(const nsc::IContext* /*context_in*/, 
                                int& /*ttl_out*/) 
                                const throw()
{
	return nsc::NSC_DOESNOTEXIST;
}

bool CFeature::getInternalStatus (const nsc::IContext* context_in, CEventData*& pReturnStatus)
{
    pReturnStatus = NULL;
    
    if( context_in == NULL)
    {
        CCTRCTXE0("No context data");
        return false;
    }
    
    // Create our mutex name, which is per-session
    DWORD dwSessionID = 0;
    if ( NSC_FAILED (context_in->GetWindowsSessionID(dwSessionID)))
        return false;

	CCTRCTXI1(_T("context_in->GetWindowsSessionID() dwSessionID=%d"), dwSessionID);

    char szTempSession [10] = {0};
	CString strMutexName = NAV_STATUS_HELPER_MUTEX_MACHINEWIDE_NSC_CLEAN;
    ultoa(dwSessionID, szTempSession, 10);
    strMutexName += szTempSession;
	//CCTRCTXI1(_T("Per-session mutex name: %s"), strMutexName);

    // Get the local status object
    ccLib::CSingleLock lock ( &CFeature::m_critStatus, INFINITE, FALSE );

	// Check mutex for this session
    ccLib::CMutex mutexStatusCleanBit;

    // It's OK if the session data doesn't exist, that just means it's a new request.
    if ( 0 == m_mapLocalStatus[dwSessionID].GetCount() ||
        !mutexStatusCleanBit.Open ( SYNCHRONIZE, FALSE, strMutexName, TRUE ))
    {
        // Dirty, refresh status
        //
        AV::IAvEventFactoryPtr pProvider;

        if (SYM_FAILED (AV::NAVEventFactory_IAVEventFactory::CreateObject(GETMODULEMGR(), &pProvider)) ||
            pProvider.m_p == NULL )
        {
            CCTRCTXE0(_T("Could not create IAvFactory object."));
            assert(false);
		    return false;
        }
        else
		{
			// We have to set the event ID before sending
			CEventData edTemp;
			edTemp.SetData ( AV::Event_Base_propType, AV::Event_ID_StatusRequest);
			edTemp.SetData ( AV::Event_Base_propSessionID, dwSessionID);
			edTemp.SetData ( AVStatus::propRequesterID, AV_MODULE_ID_AVNSCPLG );

			// Create a list of features that we are querying for.
			// 
			// &$ make this a single list determined in CProduct.
			std::vector <long> vecFeatures;
			vecFeatures.push_back ( (long)AV::Event_ID_StatusAP );
			vecFeatures.push_back ( (long)AV::Event_ID_StatusEmail );
			vecFeatures.push_back ( (long)AV::Event_ID_StatusIWP );
			vecFeatures.push_back ( (long)AV::Event_ID_StatusIMScan );
			vecFeatures.push_back ( (long)AV::Event_ID_StatusFSS );
			vecFeatures.push_back ( (long)AV::Event_ID_StatusDefs );
			vecFeatures.push_back ( (long)AV::Event_ID_StatusLicensing );

			edTemp.SetData ( AVStatus::propRequestedFeatures,(BYTE*)&vecFeatures[0], (long) vecFeatures.size()*sizeof(long));

			SYMRESULT s_result;
			CEventData* pReturnEvent;
			int iSleepCount = 0;

			while(iSleepCount < 30)
			{
				s_result = pProvider->SendAvEvent (edTemp, pReturnEvent);
				if(SYM_FAILED(s_result))
				{
					CCTRCTXE1(_T("Send AV Event failed in getInternalStatus, SYMRESULT=%d"), s_result);
					assert(false);
					return false;
				}

				// This is a synchronous call
				if ( pReturnEvent )
				{
					// Copy our new status to the local copy
					if (!pReturnEvent->GetNode ( AVStatus::propCurrentStatus, m_mapLocalStatus[dwSessionID] ))
					{
						CCTRCTXE0(_T("No status bag in getInternalStatus. Sleep(1sec)"));

						iSleepCount++;
						Sleep(1000);
                        continue;
					}
					else
					{
						pReturnStatus = &m_mapLocalStatus[dwSessionID];
						CCTRCTXI0("GetNode() succeeded - Cache miss");
						return true; // Success!
					}
				}
				else
				{
					CCTRCTXE0(_T("No return event in getInternalStatus. Sleep(1sec)"));

					iSleepCount++;
					Sleep(1000);
					continue;
				}
			}
		}
    }
    
    // Clean bit is on so we can use this data safely.
    CCTRCTXI0("Cache hit!");
    pReturnStatus = &m_mapLocalStatus[dwSessionID];
    return true;
}


//////////////////////////////////////////////////////////////////////////////

nsc::NSCRESULT CNAVLicenseFeature::GetName(const nsc::IContext* /*context_in*/, 
                                            const cc::IString*& result_out) 
                                            const throw()
{
	CBrandingRes BrandRes;
	ccLib::CString strTemp = BrandRes.ProductName();

    cc::IStringPtr spStrTemp = ccSym::CStringImpl::CreateStringImpl(strTemp);
    if(!spStrTemp)
    {
        TRACEE(_T("String implementation failed for feature GetName."));
        return nsc::NSC_FAIL;
    }

	result_out = spStrTemp;
    return nsc::NSC_SUCCESS;
}
