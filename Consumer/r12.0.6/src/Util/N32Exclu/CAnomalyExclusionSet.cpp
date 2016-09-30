// /////////////////////////////////////////////////////////////////////////////////////////////////
// Symantec copyright header start
// //////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2005 Symantec Corporation.
// All rights reserved.
//
// /////////////////////////////////////////////////////////////////////////////////////////////////
// Symantec copyright header stop
// //////////////////////

/***
 * Implementation: CAnomalyExclusionSet
 ***/
#ifndef __N32EXCLU_canomalyexclusionset_cpp__
#define __N32EXCLU_canomalyexclusionset_cpp__

#include "n32exclu.h"
#include "CAnomalyExclusionSet.h"

namespace NavExclusions
{
    CAnomalyExclusionSet::CAnomalyExclusionSet()
    {}

    CAnomalyExclusionSet::~CAnomalyExclusionSet()
    {
        this->nuke();
    }

    ExResult CAnomalyExclusionSet::addExclusion(IAnomalyExclusion* pExclusion)
    {
        cc::IStringPtr spStrGuid;
    
        if(!pExclusion)
        {
            CCTRACEE(_T("CAnomalyExclusionSet::addExclusion - Received a null "\
                        "pointer as an argument."));
            return NavExclusions::InvalidArgument;
        }

        if(pExclusion->getExclusionType() != IExclusion::ExclusionType::ANOMALY)
        {
            CCTRACEE(_T("CAnomalyExclusionSet::addExclusion - Received a "\
                        "non-anomaly exclusion as an argument."));
            return NavExclusions::InvalidArgument;
        }

        if(!pExclusion->isInitialized())
        {
            CCTRACEE(_T("CAnomalyExclusionSet::addExclusion - Received an "\
                        "uninitialized exclusion object."));
            return NavExclusions::InvalidArgument;        
        }

        if(pExclusion->getAnomalyGuid(spStrGuid) >= NavExclusions::Fail)
        {
            CCTRACEE(_T("CAnomalyExclusionSet::addExclusion - Failure when "\
                        "trying to retrieve anomaly guid."));
            return NavExclusions::Fail;
        }

        // Check for duplicates
        if(this->findExclusionId(spStrGuid, pExclusion->getVid()) != _INVALID_ID)
        {
            CCTRACEW(_T("CAnomalyExclusionSet::addExclusion - Rejecting add "\
                        "of duplicate exclusion."));
            return NavExclusions::False;
        }
             
        spStrGuid.Release();

        // Add exclusion
        this->m_lstExclusions.push_back(pExclusion);
        if(this->m_lstExclusions.back() == pExclusion)
        {
            pExclusion->AddRef();
            return NavExclusions::Success;
        }
        else
        {   // This should never happen
            CCTRACEE(_T("CAnomalyExclusion::addExclusion - back != argument "\
                        "after a push_back()! Weirdness in STL List?"));
            return NavExclusions::Fail;
        }
    }

    /*** Search Methods - Very much the same with different returns/calls. ***/
    const IAnomalyExclusion* CAnomalyExclusionSet::getExclusion(ExclusionId eid)
    {
        return this->findExclusion(eid);
    }

    const IAnomalyExclusion* CAnomalyExclusionSet::getExclusion(
                                 const cc::IString* pStrGuid, const ULONG ulVid)
    {
        return this->findExclusion(pStrGuid, ulVid);
    }

    ExclusionId CAnomalyExclusionSet::findExclusionId(const cc::IString* pStrGuid, const ULONG ulVid)
    {
        IAnomalyExclusion* pAE = this->findExclusion(pStrGuid, ulVid);
        if(pAE)
            return pAE->getID();
        else
            return _INVALID_ID;
    }

    ExResult CAnomalyExclusionSet::removeExclusion(ExclusionId eid)
    {
        if(eid <= _INVALID_ID) { return NavExclusions::InvalidArgument; }

        IAnomalyExclusion* pAE = NULL;
        std::list<IAnomalyExclusion*>::const_iterator it;
        for(it = this->m_lstExclusions.begin(); 
            it != this->m_lstExclusions.end(); it++)
        {
            if((*it)->getID() == eid)
            {
                pAE = (*it);
                break;
            }
        }

        if(!pAE)
        {
            CCTRACEW(_T("CAnomalyExclusion::removeExclusion - Couldn't find "\
                "an exclusion with the supplied exclusion ID."));
            return NavExclusions::Fail;

        }

        // Remove from list and release our refcount to it.
        this->m_lstExclusions.remove(pAE);
        pAE->Release();

        return NavExclusions::Success;
    }

    ExResult CAnomalyExclusionSet::removeExclusion(const cc::IString* pStrGuid,
                                                   const ULONG ulVid)
    {
        if(pStrGuid == NULL && ulVid <= _INVALID_ID) 
        { return NavExclusions::InvalidArgument; }

        IAnomalyExclusion* pAE = NULL;
        std::list<IAnomalyExclusion*>::const_iterator it;
        for(it = this->m_lstExclusions.begin(); 
            it != this->m_lstExclusions.end(); it++)
        {
            if((*it)->isExactMatch(pStrGuid, ulVid) == NavExclusions::Success)
            {
                pAE = (*it);
                break;
            }
        }

        if(!pAE)
        {
            CCTRACEW(_T("CAnomalyExclusion::removeExclusion - Couldn't find "\
                        "an exclusion with the supplied GUID and VID."));
            return NavExclusions::Fail;
        }

        // Remove from list and release our refcount to it.
        this->m_lstExclusions.remove(pAE);
        pAE->Release();

        return NavExclusions::Success;
    }

    IExclusion::ExclusionState CAnomalyExclusionSet::isExcluded(
                                     const cc::IString* pStrGuid, 
                                     const ULONG ulVid, 
                                     IExclusion::ExclusionState requiredState)
    {
        IExclusion::ExclusionState eState;

        if(!pStrGuid && ulVid <= _INVALID_ID) 
        { return IExclusion::ExclusionState::NOT_EXCLUDED; }

        std::list<IAnomalyExclusion*>::iterator it;
        for(it = this->m_lstExclusions.begin();
            it != this->m_lstExclusions.end(); it++)
        {
            // isValidForMatch handles expiration, deletion, etc.
            if((*it)->isValidForMatch() == NavExclusions::Success
                && (*it)->isMatch((void*)pStrGuid, (void*)&ulVid) 
                    == NavExclusions::Success)
            {
                eState = (*it)->getState();
                // If at least as restrictive as required...
                if((eState & requiredState) == requiredState)
                    return eState;
            }
        }

        return IExclusion::ExclusionState::NOT_EXCLUDED;
    }

    // Removes all contents. Yes, it is safe to do this multiple times
    // because STL IS GREAT.
    ExResult CAnomalyExclusionSet::nuke()
    {
        while(!this->m_lstExclusions.empty())
        {
            this->m_lstExclusions.front()->Release();
            this->m_lstExclusions.pop_front();
        }
        return NavExclusions::Success;
    }

    ExResult CAnomalyExclusionSet::isEmpty()
    {
        if(this->m_lstExclusions.empty()) { return NavExclusions::Success; }
        else { return NavExclusions::False; }
    }

    /*** Privates ***/
    IAnomalyExclusion* CAnomalyExclusionSet::findExclusion(ExclusionId eid)
    {
        if(eid <= _INVALID_ID) { return NULL; } // Yes, you are funny, Mr. User.

        std::list<IAnomalyExclusion*>::const_iterator it;
        for(it = this->m_lstExclusions.begin(); 
            it != this->m_lstExclusions.end(); it++)
        {
            if((*it)->getID() == eid 
                && (*it)->isValidForMatch() == NavExclusions::Success)
            {
                return (*it);
            }
        }

        return NULL;
    }

    // We could do this by finding the ID, then calling getExclusion(ID),
    // but that would cost us O(2N) instead of O(N).
    IAnomalyExclusion* CAnomalyExclusionSet::findExclusion(
                                 const cc::IString* pStrGuid, const ULONG ulVid)
    {
        if(pStrGuid == NULL && ulVid <= _INVALID_ID) 
        { return NULL; } // Yes, you are funny, Mr. User.

        std::list<IAnomalyExclusion*>::const_iterator it;
        for(it = this->m_lstExclusions.begin(); 
            it != this->m_lstExclusions.end(); it++)
        {
            if((*it)->isExactMatch(pStrGuid, ulVid) == NavExclusions::Success
                && (*it)->isValidForMatch() == NavExclusions::Success)
            {
                return (*it);
            }
        }

        return NULL;
    }

    ExclusionId CAnomalyExclusionSet::findDuplicate(IAnomalyExclusion* pAE)
    {
        ExclusionId myId, thisId;
        cc::IStringPtr spGuid;
        ULONG ulVid;
        std::list<IAnomalyExclusion*>::const_iterator it;

        if(!pAE || pAE->getExclusionType() != IExclusion::ExclusionType::ANOMALY)
            return _INVALID_ID;

        myId = pAE->getID();
        ulVid = pAE->getVid();
        if(pAE->getAnomalyGuid(spGuid) >= NavExclusions::Fail)
        {   // Indicate error by returning the same ID
            return myId;
        }

        for(it = this->m_lstExclusions.begin(); 
                it != this->m_lstExclusions.end(); it++)
        {
            thisId = (*it)->getID();
            if(thisId != myId && 
                (*it)->isExactMatch(spGuid, ulVid) == NavExclusions::Success)
            {
                return thisId;
            }
        }

        return _INVALID_ID;
    }
}

#endif
