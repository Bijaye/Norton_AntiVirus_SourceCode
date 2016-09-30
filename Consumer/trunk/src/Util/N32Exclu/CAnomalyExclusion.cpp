////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

/***
 * Implementation for VID+GUID exclusion object; subclass of IExclusion.
 * Adds certain methods directly concerning VID+GUID exclusions as well
 * as methods that require consideration of implementation specifics.
 ***/

#pragma once

#ifndef __N32EXCLU_canomalyexclusion_cpp__
#define __N32EXCLU_canomalyexclusion_cpp__

#include <wchar.h>
#include "CAnomalyExclusion.h"
#include "ccString.h"
#include "ccSymKeyValueCollectionImpl.h"

#define __MY_INTERFACE IAnomalyExclusion
#define __MY_CLASS CAnomalyExclusion
#define __MY_CLASSNAME "CAnomalyExclusion"
#define __N32EXCLU_IMPLEMENTATION_

namespace NavExclusions
{
    CAnomalyExclusion::CAnomalyExclusion()
    {
        if(this->initMe())
        {
            m_spProperties->SetValue(IExclusion::EType, 
                       (ULONG)IExclusion::ExclusionType::ANOMALY);
        }
    }

    CAnomalyExclusion::CAnomalyExclusion(cc::IKeyValueCollection* rhs)
    {
        this->m_spProperties = rhs;
        this->m_bInitialized = true;
    }

    CAnomalyExclusion::~CAnomalyExclusion()
    {this->uninitMe();}

    NavExclusions::ExResult CAnomalyExclusion::setAnomalyGuid(const cc::IString* guid)
    {
        if(!this->isInitialized()) { return NavExclusions::NotInitialized; }

        if(!guid)
        {
            CCTRACEI(_T("CAnomalyExclusion::setAnomalyGuid - Received NULL argument, ")\
                        _T("removing item."));

            // Um.. it's already null - escape.
            if(!m_spProperties->GetExists(IAnomalyExclusion::Property::AnomalyGuid))
                return NavExclusions::Success;

            if(m_spProperties->Remove(IAnomalyExclusion::Property::AnomalyGuid))
                return NavExclusions::Success;
            else
                return NavExclusions::Fail;
        }
        else 
        {
            if(!m_spProperties->SetValue(IAnomalyExclusion::Property::AnomalyGuid, guid))
                return NavExclusions::Fail;
            else
                return NavExclusions::Success;
        }
    }

    ExResult CAnomalyExclusion::getAnomalyGuid(cc::IString*& pStrGuid)
    {
        if(pStrGuid != NULL)
        {
            CCTRACEI(_T("CAnomalyExclusion::getAnomalyGuid() - Received a ")\
                     _T("preinitialized string - releasing."));
            pStrGuid->Release();
            pStrGuid = NULL;
        }

        return this->getStringFromBag((size_t)CAnomalyExclusion::Property::AnomalyGuid,
                                       pStrGuid);
    }

    NavExclusions::ExResult CAnomalyExclusion::setVid(const ULONG vid)
    {
        if(!this->isInitialized()) { return NavExclusions::NotInitialized; }

        if(!m_spProperties->SetValue(CAnomalyExclusion::Property::Vid, vid))
            return NavExclusions::Fail;
        else
            return NavExclusions::Success;
    }

    const ULONG CAnomalyExclusion::getVid()
    {
        ULONG myVid;
        if(!this->isInitialized()) { return _INVALID_VID; }

        if(!m_spProperties->GetValue(CAnomalyExclusion::Property::Vid, myVid))
            return _INVALID_VID;
        else
            return myVid;
    }

    ExResult CAnomalyExclusion::hasGuid()
    {
        if(!this->isInitialized()) { return NavExclusions::NotInitialized; }

        if(m_spProperties->GetExists(IAnomalyExclusion::Property::AnomalyGuid))
            return NavExclusions::Success;
        else
            return NavExclusions::False;
    }

    ExResult CAnomalyExclusion::hasVid()
    {
        if(!this->isInitialized()) { return NavExclusions::NotInitialized; }

        if(m_spProperties->GetExists(IAnomalyExclusion::Property::Vid))
            return NavExclusions::Success;
        else
            return NavExclusions::False;
    }

    NavExclusions::ExResult CAnomalyExclusion::isVidMatch(const ULONG rhsVid)
    {
        ULONG myVid = this->getVid();
        if(myVid == _INVALID_VID || rhsVid == _INVALID_VID)
            return NavExclusions::False;
        else if(myVid == rhsVid)
            return NavExclusions::Success;
        else
            return NavExclusions::False;
    }

    NavExclusions::ExResult CAnomalyExclusion::isGuidMatch(const cc::IString* rhsGuid)
    {
        cc::IString* myGuid = NULL;
        ExResult res;

        if(!this->isInitialized()) 
        { 
            CCTRACEE(_T("CAnomalyExclusion::isGuidMatch - Not initialized."));
            return NavExclusions::NotInitialized; 
        }

        res = this->getStringFromBag(IAnomalyExclusion::Property::AnomalyGuid, myGuid);
        if(res >= NavExclusions::Fail)
        {
            CCTRACEE(_T("CAnomalyExclusion::isGuidMatch - Error while retrieving ")\
                        _T("string from bag."));
            return res;
        }
        else if(res == NavExclusions::False)
        {
            CCTRACEI(_T("CAnomalyExclusion::isGuidMatch - No guid!"));
            return NavExclusions::False;
        }

        if(!myGuid || !rhsGuid)
            return NavExclusions::False;
        if(myGuid->GetLength() == 0 || rhsGuid->GetLength() == 0)
            return NavExclusions::False;
        else if(wcscmp(myGuid->GetStringW(), rhsGuid->GetStringW()) == 0)
            return NavExclusions::Success;
        else
            return NavExclusions::False;
    }

    // Required overrides from CExclusion/IExclusion - business methods!
    NavExclusions::ExResult CAnomalyExclusion::isMatch(const void* pGuid, 
                                                       const void* pVid)
    {
        ExResult vidRes, guidRes;
        vidRes = guidRes = (NavExclusions::NotInitialized);
        if(!this->isInitialized()) 
        { 
            CCTRACEE(_T("CAnomalyExclusion::isMatch - Not initialized."));
            return NavExclusions::NotInitialized; 
        }

        if(!pGuid && !pVid) 
        { 
            CCTRACEE(_T("CAnomalyExclusion::isMatch - Invalid arguments."));
            return NavExclusions::InvalidArgument; 
        }

        if(this->hasVid() != NavExclusions::Success && 
            this->hasGuid() != NavExclusions::Success) 
        {
            CCTRACEE(_T("CAnomalyExclusion::isMatch - VID and GUID not set."));
            return NavExclusions::NotInitialized; 
        }

        // Even if we get both arguments, we only need one to match.
        // Hence, we presume success and catch the exceptional cases
        // where we receive neither a guid nor a vid or we haven't
        // had a guid or vid set.
        // Also, if they pass in an argument that we don't have a
        // proper match for, we also assume success.
        if(pGuid && this->hasGuid() == NavExclusions::Success)
        {
            guidRes = (this->isGuidMatch((const cc::IString*)pGuid));
        }

        if(pVid && this->hasVid() == NavExclusions::Success)
        {
            vidRes = (this->isVidMatch(*((const ULONG*)pVid)));
        }

        // True < False < Failure; if two failures, doesn't matter which
        return (vidRes>guidRes?guidRes:vidRes);
    }

    ExResult CAnomalyExclusion::getDisplayName(cc::IString*& pStrName)
    {
        if(pStrName != NULL)
        {
            CCTRACEI(_T("CAnomalyExclusion::getDisplayName received a ")\
                     _T("preinitialized string; auto-releasing."));
            pStrName->Release();
            pStrName = NULL;
        }

        return this->getStringFromBag((size_t)IExclusion::Property::EThreatName,
                                      pStrName);
    }

    ExResult CAnomalyExclusion::isExactMatch(const cc::IString* pStrGuid, 
                                             const ULONG ulVid)
    {
        cc::IStringPtr spStr;

        if(this->getVid() != ulVid)
            return NavExclusions::False;

        if(this->getStringFromBag(IAnomalyExclusion::Property::AnomalyGuid, 
            spStr) == NavExclusions::Success)
        {
            if(!pStrGuid || !spStr)
                return NavExclusions::False;

            if(spStr->GetLength() != pStrGuid->GetLength())
                return NavExclusions::False;

            if(wcsncmp(spStr->GetStringW(), 
                       pStrGuid->GetStringW(), 
                       spStr->GetLength()
                       ) == 0)
                return NavExclusions::Success;
        }

        if(!spStr && !pStrGuid)
            return NavExclusions::Success;

        return NavExclusions::False;
    }

    IAnomalyExclusion* CAnomalyExclusion::clone()
    {
        cc::IStringPtr spStr;
        time_t tExpiry;
        ExResult res;
        cc::IKeyValueCollection* pUserData;

        CAnomalyExclusion* ptr = new(std::nothrow) CAnomalyExclusion();
        ptr->AddRef();
        if(!ptr->m_spProperties->SetCloneInterface(true))
        {
            CCTRACEE(_T("CAnomalyExclusion::clone - Could not set clone interface."));
            delete ptr;
            return NULL;
        }

        if(ptr->setActive(this->isActive() == NavExclusions::Success) != NavExclusions::Success)
        {
            CCTRACEE(_T("CAnomalyExclusion::clone - Could not set Active state."));
            delete ptr;
            return NULL;
        }

        if(ptr->setDeleted(this->isDeleted() == NavExclusions::Success) != NavExclusions::Success)
        {
            CCTRACEE(_T("CAnomalyExclusion::clone - Could not set Deleted state."));
            delete ptr;
            return NULL;
        }

        if(ptr->setState(this->getState()) != NavExclusions::Success)
        {
            CCTRACEE(_T("CAnomalyExclusion::clone - Could not set exclusion state."));
            delete ptr;
            return NULL;
        }

        res = this->getAnomalyGuid(spStr);
        if(res >= NavExclusions::Fail)
        {
            CCTRACEE(_T("CAnomalyExclusion::clone - Failure when getting anomaly guid."));
            delete ptr;
            return NULL;
        }
        else if(res == NavExclusions::Success)
        {
            if(ptr->setAnomalyGuid(spStr) != NavExclusions::Success)
            {
                CCTRACEE(_T("CAnomalyExclusion::clone - Failure when setting anomaly guid."));
                delete ptr;
                return NULL;
            }
            spStr.Release();
        }

        res = this->getDisplayName(spStr);
        if(res >= NavExclusions::Fail)
        {
            CCTRACEE(_T("CAnomalyExclusion::clone - Failure when getting threat name."));
            delete ptr;
            return NULL;
        }
        else if(res == NavExclusions::Success)
        {
            if(ptr->setDisplayName(spStr) != NavExclusions::Success)
            {
                CCTRACEE(_T("CAnomalyExclusion::clone - Failure when setting threat name."));
                delete ptr;
                return NULL;
            }
            spStr.Release();
        }
        else
        {
            CCTRACEE(_T("CAnomalyExclusion::clone - No threat name! Illegal exclusion."));
            delete ptr;
            return NULL;
        }

        res = this->getExclusionGuid(spStr);
        if(res >= NavExclusions::Fail)
        {
            CCTRACEE(_T("CExclusionExclusion::clone - Failure when getting Exclusion guid."));
            delete ptr;
            return NULL;
        }
        else if(res == NavExclusions::Success)
        {
            if(ptr->setExclusionGuid(spStr) != NavExclusions::Success)
            {
                CCTRACEE(_T("CExclusionExclusion::clone - Failure when setting Exclusion guid."));
                delete ptr;
                return NULL;
            }
            spStr.Release();
        }
        else
        {
            CCTRACEE(_T("CAnomalyExclusion::clone - No exclusion GUID! Illegal exclusion."));
            delete ptr;
            return NULL;
        }

        if(ptr->setID(this->getID()) != NavExclusions::Success)
        {
            CCTRACEE(_T("CAnomalyExclusion::clone - Could not set Exclusion ID."));
            delete ptr;
            return NULL;
        }

        if(this->getVid() > _INVALID_VID && 
           ptr->setVid(this->getVid()) >= NavExclusions::Fail)
        {
            CCTRACEE(_T("CAnomalyExclusion::clone - Could not set VID (if any)."));
            delete ptr;
            return NULL;
        }

        if((tExpiry = this->getExpiryStamp()) > 0)
        {
            if(ptr->setExpiryStamp(tExpiry) != NavExclusions::Success)
            {
                CCTRACEE(_T("CAnomalyExclusion::clone - Could not copy exclusion timestamp."));
                delete ptr;
                return NULL;
            }
        }

        if(this->m_spProperties->GetValue(IExclusion::Property::UserData, 
            (ISymBase*&)pUserData))
        {
            if(!ptr->m_spProperties->SetValue(IExclusion::Property::UserData, 
                (ISymBase*&)pUserData))
            {
                CCTRACEE(_T("CAnomalyExclusion::clone - Could not copy user data."));
                delete ptr;
                return NULL;
            }
        }

        if(!ptr->m_spProperties->SetCloneInterface(false))
        {
            CCTRACEE(_T("CAnomalyExclusion::clone - Could not unset clone interface."));
            delete ptr;
            return NULL;
        }

        return ptr;
    }

/*** MACRO THIS ***/
#include "GenericExclusion.h"
/*** END MACRO THIS ***/
#undef __MY_INTERFACE
#undef __MY_CLASS
#undef __MY_CLASSNAME

}
#endif